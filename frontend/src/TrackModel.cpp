#include "TrackModel.h"
#include "IconProvider.h"
#include "MediaLabel.h"
#include "TrackInterface.h"

TrackModel::TrackModelRow::TrackModelRow(
    const std::shared_ptr<TrackInterface> &_trackInterface) {

    sortVerified = _trackInterface->is_inBlocklist();

    title = wxString::FromUTF8(_trackInterface->get_title());
    cover = MediaLabel::loadImage(_trackInterface->get_cover(), wxSize(64, 64));
    artist = wxString::FromUTF8(_trackInterface->get_artist());
    album = wxString::FromUTF8(_trackInterface->get_album());
    genre = wxString::FromUTF8(_trackInterface->get_genre());
    sortLength = _trackInterface->get_length();

    int minutes = (sortLength / 60) % 60;
    int seconds = sortLength % 60;

    wxString duration =
        wxString(std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") +
                 std::to_string(seconds));

    length = duration;

    verified =
        sortVerified
            ? wxArtProvider::GetBitmap(wxART_CIRCLE_CHECKMARK, wxART_BUTTON)
            : wxArtProvider::GetBitmap(wxART_CIRCLE_XMARK, wxART_BUTTON);

    sortTitle = title.Lower();
    sortArtist = artist.Lower();
    sortAlbum = album.Lower();
    sortGenre = genre.Lower();
}

TrackModel::TrackModelRow::~TrackModelRow() {}

wxBitmap TrackModel::TrackModelRow::get_verified() const { return verified; }
wxBitmap TrackModel::TrackModelRow::get_cover() const { return cover; }
wxString TrackModel::TrackModelRow::get_title() const { return title; }
wxString TrackModel::TrackModelRow::get_artist() const { return artist; }
wxString TrackModel::TrackModelRow::get_album() const { return album; }
wxString TrackModel::TrackModelRow::get_genre() const { return genre; }
wxString TrackModel::TrackModelRow::get_length() const { return length; }

bool TrackModel::TrackModelRow::get_sortVerified() const {
    return sortVerified;
}
wxString TrackModel::TrackModelRow::get_sortTitle() const { return sortTitle; }
wxString TrackModel::TrackModelRow::get_sortArtist() const {
    return sortArtist;
}
wxString TrackModel::TrackModelRow::get_sortAlbum() const { return sortAlbum; }
wxString TrackModel::TrackModelRow::get_sortGenre() const { return sortGenre; }
std::size_t TrackModel::TrackModelRow::get_sortLength() const {
    return sortLength;
}

const wxBitmap &TrackModel::TrackModelRow::GetVerifyBitmap() {
    static wxBitmap bitmap = [] {
        wxBitmap bmp = wxArtProvider::GetBitmap(wxART_ASSESSMENT, wxART_BUTTON);
        bmp = wxBitmap(
            bmp.ConvertToImage().Rescale(16, 16, wxIMAGE_QUALITY_HIGH));
        return bmp;
    }();
    return bitmap;
}

const wxBitmap &TrackModel::TrackModelRow::GetDeleteBitmap() {
    static wxBitmap bitmap = [] {
        wxBitmap bmp =
            wxArtProvider::GetBitmap(wxART_TRASH_XMARK, wxART_BUTTON);
        bmp = wxBitmap(
            bmp.ConvertToImage().Rescale(16, 16, wxIMAGE_QUALITY_HIGH));
        return bmp;
    }();
    return bitmap;
}

TrackModel::TrackModel() : wxDataViewVirtualListModel(0) {}

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
    const std::shared_ptr<TrackModelRow> &row) const {
    if (filterState.searchQuery.empty())
        return true;

    return row->get_sortTitle().Contains(filterState.searchQuery) ||
           row->get_sortArtist().Contains(filterState.searchQuery) ||
           row->get_sortAlbum().Contains(filterState.searchQuery) ||
           row->get_sortGenre().Contains(filterState.searchQuery);
}

bool TrackModel::MatchesFilter(
    const std::shared_ptr<TrackModelRow> &row) const {
    if (filterState.showVerified && row->get_sortVerified()) {
        return true;
    }
    if (filterState.showUnverified && !row->get_sortVerified()) {
        return true;
    }
    return false;
}

void TrackModel::SetFilterState(const FilterState &_state) {
    filterState = _state;
    RebuildVisibleTracks();
}

unsigned int TrackModel::GetColumnCount() const { return COL_MAX; }

wxString TrackModel::GetColumnType(unsigned int col) const {
    switch (col) {
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

void TrackModel::GetValueByRow(wxVariant &variant, unsigned int row,
                               unsigned int col) const {
    auto r = visibleRows[row];

    switch (col) {
    case COL_PROGRESS:
        variant << r->get_verified();
        break;
    case COL_COVER:
        variant << r->get_cover();
        break;
    case COL_TITLE:
        variant = r->get_title();
        break;
    case COL_ARTIST:
        variant = r->get_artist();
        break;
    case COL_ALBUM:
        variant = r->get_album();
        break;
    case COL_GENRE:
        variant = r->get_genre();
        break;
    case COL_LENGTH:
        variant = r->get_length();
        break;
    case COL_VERIFY:
        variant << TrackModelRow::GetVerifyBitmap();
        break;
    case COL_DELETE:
        variant << TrackModelRow::GetDeleteBitmap();
        break;
    default:
        wxLogDebug(wxT("No view specified for column " + std::to_string(col)));
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
