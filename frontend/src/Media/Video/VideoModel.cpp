#include "Media/Video/VideoModel.h"
#include "Interfaces/IVideo.h"
#include "Media/Track/Renderers/DownloadStatusRenderer.h"
#include "Media/Video/VideoModelRow.h"

VideoModel::VideoModel() : wxDataViewVirtualListModel(0), filterState{""} {}

std::shared_ptr<IVideo>
VideoModel::GetVideo(std::shared_ptr<VideoModelRow> _row) {
    return _row->get_video();
}

std::shared_ptr<VideoModelRow>
VideoModel::GetRowByIndex(std::size_t _index) const {
    if (_index >= visibleRows.size())
        return nullptr;

    return visibleRows[_index];
}

void VideoModel::AddRows(
    const std::vector<std::shared_ptr<VideoModelRow>> &_batch) {
    visibleRows.reserve(visibleRows.size() + _batch.size());
    for (auto &&videoInterface : _batch) {
        visibleRows.push_back(std::move(videoInterface));
    }
    // Notify ctrl that row count changed
    SortByHeader(Columns::COL_TITLE, true);
}

void VideoModel::MergeRows(const std::vector<std::shared_ptr<IVideo>> &_batch) {

    bool changed = false;

    for (const auto &video : _batch) {
        auto id = video->get_id();
        auto [it, inserted] =
            allVideos.emplace(id, std::make_shared<VideoModelRow>(video));
        if (inserted) {
            changed = true;
        }
    }

    if (_batch.empty()) {
        allVideos.clear();
        changed = true;
    }

    if (changed) {
        RebuildVisibleVideos();
    }
}

void VideoModel::RemoveRow(std::size_t _rowIndex) {
    auto row = GetRowByIndex(_rowIndex);
    auto id = row->get_video()->get_id();

    auto it = allVideos.find(id);
    if (it == allVideos.end())
        return;

    allVideos.erase(it);
    visibleRows.erase(visibleRows.begin() + _rowIndex);

    RowDeleted(_rowIndex);
}

void VideoModel::RebuildVisibleVideos() {

    std::vector<std::shared_ptr<VideoModelRow>> newVisibleVideos;
    visibleRows.clear();

    for (auto &[id, video] : allVideos) {

        // if (!MatchesFilter(video))
        //     continue;
        if (!MatchesSearch(video))
            continue;

        newVisibleVideos.push_back(video);
    }

    Cleared();
    AddRows(newVisibleVideos);
}

bool VideoModel::MatchesSearch(
    const std::shared_ptr<VideoModelRow> &_row) const {
    if (filterState.searchQuery.empty())
        return true;

    return _row->get_sortTitle()
               .value_or(std::string{})
               .contains(filterState.searchQuery) ||
           _row->get_sortChannel()
               .value_or(std::string{})
               .contains(filterState.searchQuery);
}

// bool VideoModel::MatchesFilter(
//     const std::shared_ptr<VideoModelRow> &_row) const {
//     if (filterState.showVerified &&
//         _row->get_status().state == DownloadState::Verified) {
//         return true;
//     }
//     if (filterState.showUnverified &&
//         !(_row->get_status().state == DownloadState::Verified)) {
//         return true;
//     }
//     return false;
// }

void VideoModel::SetFilterState(const FilterState &_state) {
    filterState = _state;
    RebuildVisibleVideos();
}

unsigned int VideoModel::GetColumnCount() const { return COL_MAX; }

wxString VideoModel::GetColumnType(unsigned int _col) const {
    switch (_col) {
    case COL_PROGRESS:
        return "long";
    case COL_COVER:
        return "wxBitmap";
    // case COL_VERIFY:
    //     return "wxBitmap";
    // case COL_DELETE:
    //     return "wxBitmap";
    default:
        return "string";
    }
}

void VideoModel::GetValueByRow(wxVariant &_variant, unsigned int _row,
                               unsigned int _col) const {
    auto r = visibleRows[_row];

    switch (_col) {
    case COL_PROGRESS:
        _variant = static_cast<long>(r->get_status().progress);
        break;
    case COL_COVER:
        _variant << r->get_cover();
        break;
    case COL_TITLE:
        _variant = r->get_title();
        break;
    case COL_CHANNEL:
        _variant = r->get_channel();
        break;
    case COL_LENGTH:
        _variant = r->get_length();
        break;
    case COL_UPLOAD_DATE:
        _variant = r->get_uploadDate();
        break;
    default:
        wxLogDebug(wxT("No view specified for column " + std::to_string(_col)));
    }
}

bool VideoModel::SetValueByRow(const wxVariant &, unsigned int, unsigned int) {
    return false;
}

void VideoModel::SetDownloadStatusByRow(std::size_t _row,
                                        const DownloadStatus &_status) {
    visibleRows[_row]->set_status(_status);
    RowChanged(_row);
}

void VideoModel::SortByHeader(unsigned int _column, bool _ascending) {
    auto compare = [_ascending](const auto &_a, const auto &_b) {
        return _ascending ? _a < _b : _a > _b;
    };
    std::stable_sort(
        visibleRows.begin(), visibleRows.end(),
        [&](const auto &a, const auto &b) {
            switch (_column) {

            case COL_PROGRESS:
                return compare(a->get_status().progress,
                               b->get_status().progress);
            case COL_TITLE:
                return compare(a->get_sortTitle(), b->get_sortTitle());
            case COL_CHANNEL:
                return compare(a->get_sortChannel(), b->get_sortChannel());
            case COL_LENGTH:
                return compare(a->get_sortLength(), b->get_sortLength());
            case COL_UPLOAD_DATE:
                return compare(a->get_sortUploadDate(),
                               b->get_sortUploadDate());
            default:
                wxLogDebug(wxT("Sorting not supported for column " +
                               std::to_string(_column)));
                return false;
            }
        });

    Reset(visibleRows.size());
}
