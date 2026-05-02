#include "TrackModel.h"
#include "IconProvider.h"
#include "MediaLabel.h"
#include "TrackInterface.h"

wxBitmap TrackModel::TrackModelRow::VERIFY_BITMAP(
    wxArtProvider::GetBitmap(wxART_ASSESSMENT, wxART_BUTTON));
wxBitmap TrackModel::TrackModelRow::DELETE_BITMAP(
    wxArtProvider::GetBitmap(wxART_TRASH_XMARK, wxART_BUTTON));

TrackModel::TrackModelRow::TrackModelRow(
    const std::shared_ptr<TrackInterface> &_trackInterface) {

    sortVerified = _trackInterface->is_inBlocklist();
    cover = wxBitmap{};
    title = wxString::FromUTF8(_trackInterface->get_title());
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

    wxTheApp->CallAfter([this, _trackInterface]() mutable {
        cover =
            MediaLabel::loadImage(_trackInterface->get_cover(), wxSize(64, 64));
    });
}

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

void TrackModel::TrackModelRow::set_cover(const wxBitmap &_cover) {
    cover = _cover;
}

TrackModel::TrackModel() : wxDataViewVirtualListModel(0) {}

void TrackModel::AddRows(
    const std::vector<std::shared_ptr<TrackModelRow>> &_batch) {
    rows.reserve(rows.size() + _batch.size());
    visibleRows.reserve(visibleRows.size() + _batch.size());
    for (auto &&row : _batch) {
        rows.emplace_back(std::move(row));
        visibleRows.push_back(rows.size() - 1);
    }
    // Notify ctrl that row count changed
    Reset(visibleRows.size());
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
    auto r = rows[visibleRows[row]];

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
        variant << TrackModelRow::VERIFY_BITMAP;
        break;
    case COL_DELETE:
        variant << TrackModelRow::DELETE_BITMAP;
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
        visibleRows.begin(), visibleRows.end(), [&](size_t a, size_t b) {
            switch (_column) {

            case COL_PROGRESS:
                return compare(rows[a]->get_sortVerified(),
                               rows[b]->get_sortVerified());
            case COL_TITLE:
                return compare(rows[a]->get_sortTitle(),
                               rows[b]->get_sortTitle());
            case COL_ARTIST:
                return compare(rows[a]->get_sortArtist(),
                               rows[b]->get_sortArtist());
            case COL_ALBUM:
                return compare(rows[a]->get_sortAlbum(),
                               rows[b]->get_sortAlbum());
            case COL_GENRE:
                return compare(rows[a]->get_sortGenre(),
                               rows[b]->get_sortGenre());
            case COL_LENGTH:
                return compare(rows[a]->get_sortLength(),
                               rows[b]->get_sortLength());
            default:
                wxLogDebug(wxT("Sorting not supported for column " +
                               std::to_string(_column)));
                return false;
            }
        });

    Reset(visibleRows.size());
}
