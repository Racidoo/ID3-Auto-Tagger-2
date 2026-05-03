#include "TrackModel.h"
#include "TrackInterface.h"
#include "TrackModelRow.h"

TrackModel::TrackModel()
    : wxDataViewVirtualListModel(0), filterState{true, true, ""} {}

std::shared_ptr<TrackInterface>
TrackModel::GetTrack(std::shared_ptr<TrackModelRow> _row) {
    return _row->get_track();
}

std::shared_ptr<TrackModelRow>
TrackModel::GetRowByIndex(std::size_t _index) const {
    if (_index >= visibleRows.size())
        return nullptr;

    return visibleRows[_index];
}

void TrackModel::AddRows(
    const std::vector<std::shared_ptr<TrackModelRow>> &_batch) {
    visibleRows.reserve(visibleRows.size() + _batch.size());
    for (auto &&trackInterface : _batch) {
        visibleRows.push_back(std::move(trackInterface));
    }
    // Notify ctrl that row count changed
    SortByHeader(Columns::COL_TITLE, true);
}

void TrackModel::MergeRows(
    const std::vector<std::shared_ptr<TrackInterface>> &_batch) {

    bool changed = false;

    for (const auto &track : _batch) {
        auto id = track->get_id();
        auto [it, inserted] =
            allTracks.emplace(id, std::make_shared<TrackModelRow>(track));
        if (inserted) {
            changed = true;
        }
    }

    if (_batch.empty()) {
        allTracks.clear();
        changed = true;
    }

    if (changed) {
        RebuildVisibleTracks();
    }
}

void TrackModel::RemoveRow(std::size_t _rowIndex) {
    auto row = GetRowByIndex(_rowIndex);
    auto id = row->get_track()->get_id();

    auto it = allTracks.find(id);
    if (it == allTracks.end())
        return;

    allTracks.erase(it);
    visibleRows.erase(visibleRows.begin() + _rowIndex);

    RowDeleted(_rowIndex);
}

void TrackModel::RebuildVisibleTracks() {

    std::vector<std::shared_ptr<TrackModelRow>> newVisibleTracks;
    visibleRows.clear();

    for (auto &[id, track] : allTracks) {

        if (!MatchesFilter(track))
            continue;
        if (!MatchesSearch(track))
            continue;

        newVisibleTracks.push_back(track);
    }

    Cleared();
    AddRows(newVisibleTracks);
}

bool TrackModel::MatchesSearch(
    const std::shared_ptr<TrackModelRow> &_row) const {
    if (filterState.searchQuery.empty())
        return true;

    return _row->get_sortTitle().Contains(filterState.searchQuery) ||
           _row->get_sortArtist().Contains(filterState.searchQuery) ||
           _row->get_sortAlbum().Contains(filterState.searchQuery) ||
           _row->get_sortGenre().Contains(filterState.searchQuery);
}

bool TrackModel::MatchesFilter(
    const std::shared_ptr<TrackModelRow> &_row) const {
    if (filterState.showVerified && _row->get_sortVerified()) {
        return true;
    }
    if (filterState.showUnverified && !_row->get_sortVerified()) {
        return true;
    }
    return false;
}

void TrackModel::SetFilterState(const FilterState &_state) {
    filterState = _state;
    RebuildVisibleTracks();
}

unsigned int TrackModel::GetColumnCount() const { return COL_MAX; }

wxString TrackModel::GetColumnType(unsigned int _col) const {
    switch (_col) {
    case COL_PROGRESS:
        return "wxBitmap";
    case COL_COVER:
        return "wxBitmap";
    case COL_VERIFY:
        return "wxBitmap";
    case COL_DELETE:
        return "wxBitmap";

    default:
        return "string";
    }
}

void TrackModel::GetValueByRow(wxVariant &_variant, unsigned int _row,
                               unsigned int _col) const {
    auto r = visibleRows[_row];

    switch (_col) {
    case COL_PROGRESS:
        _variant << r->get_verified();
        break;
    case COL_COVER:
        _variant << r->get_cover();
        break;
    case COL_TITLE:
        _variant = r->get_title();
        break;
    case COL_ARTIST:
        _variant = r->get_artist();
        break;
    case COL_ALBUM:
        _variant = r->get_album();
        break;
    case COL_GENRE:
        _variant = r->get_genre();
        break;
    case COL_LENGTH:
        _variant = r->get_length();
        break;
    case COL_VERIFY:
        _variant << TrackModelRow::GetVerifyBitmap();
        break;
    case COL_DELETE:
        _variant << TrackModelRow::GetDeleteBitmap();
        break;
    default:
        wxLogDebug(wxT("No view specified for column " + std::to_string(_col)));
    }
}

bool TrackModel::SetValueByRow(const wxVariant &, unsigned int, unsigned int) {
    return false;
}

void TrackModel::SortByHeader(unsigned int _column, bool _ascending) {
    auto compare = [_ascending](const auto &_a, const auto &_b) {
        return _ascending ? _a < _b : _a > _b;
    };
    std::stable_sort(
        visibleRows.begin(), visibleRows.end(),
        [&](const auto &a, const auto &b) {
            switch (_column) {

            case COL_PROGRESS:
                return compare(a->get_sortVerified(), b->get_sortVerified());
            case COL_TITLE:
                return compare(a->get_sortTitle(), b->get_sortTitle());
            case COL_ARTIST:
                return compare(a->get_sortArtist(), b->get_sortArtist());
            case COL_ALBUM:
                return compare(a->get_sortAlbum(), b->get_sortAlbum());
            case COL_GENRE:
                return compare(a->get_sortGenre(), b->get_sortGenre());
            case COL_LENGTH:
                return compare(a->get_sortLength(), b->get_sortLength());
            default:
                wxLogDebug(wxT("Sorting not supported for column " +
                               std::to_string(_column)));
                return false;
            }
        });

    Reset(visibleRows.size());
}
