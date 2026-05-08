#include "TrackModelRow.h"
#include "IconProvider.h"
#include "MediaLabel.h"
#include "TrackInterface.h"

TrackModelRow::TrackModelRow(
    const std::shared_ptr<TrackInterface> &_trackInterface)
    : track(_trackInterface) {
    RebuildSortCache();
}

TrackModelRow::~TrackModelRow() {}

std::shared_ptr<TrackInterface> TrackModelRow::get_track() const {
    return track;
}

const DownloadStatus &TrackModelRow::get_status() const { return status; }
const wxBitmap &TrackModelRow::get_cover() const { return cover; }
const wxString &TrackModelRow::get_title() const { return title; }
const wxString &TrackModelRow::get_artist() const { return artist; }
const wxString &TrackModelRow::get_album() const { return album; }
const wxString &TrackModelRow::get_genre() const { return genre; }
const wxString &TrackModelRow::get_length() const { return length; }

// bool TrackModelRow::get_sortVerified() const { return sortVerified; }
const std::string &TrackModelRow::get_sortTitle() const { return sortTitle; }
const std::string &TrackModelRow::get_sortArtist() const { return sortArtist; }
const std::string &TrackModelRow::get_sortAlbum() const { return sortAlbum; }
const std::string &TrackModelRow::get_sortGenre() const { return sortGenre; }
std::size_t TrackModelRow::get_sortLength() const { return sortLength; }

void TrackModelRow::set_status(const DownloadStatus &_status) {
    status = _status;
}

const wxBitmap &TrackModelRow::GetCheckmarkBitmap() {
    static wxBitmap bitmap = [] {
        wxBitmap bmp =
            wxArtProvider::GetBitmap(wxART_CIRCLE_CHECKMARK, wxART_BUTTON);
        return bmp;
    }();
    return bitmap;
}

const wxBitmap &TrackModelRow::GetXmarkBitmap() {
    static wxBitmap bitmap = [] {
        wxBitmap bmp =
            wxArtProvider::GetBitmap(wxART_CIRCLE_XMARK, wxART_BUTTON);
        return bmp;
    }();
    return bitmap;
}

const wxBitmap &TrackModelRow::GetVerifyBitmap() {
    static wxBitmap bitmap = [] {
        wxBitmap bmp = wxArtProvider::GetBitmap(wxART_ASSESSMENT, wxART_BUTTON);
        return bmp;
    }();
    return bitmap;
}

const wxBitmap &TrackModelRow::GetDeleteBitmap() {
    static wxBitmap bitmap = [] {
        wxBitmap bmp =
            wxArtProvider::GetBitmap(wxART_TRASH_XMARK, wxART_BUTTON);
        return bmp;
    }();
    return bitmap;
}

void TrackModelRow::RebuildSortCache() {

    cover = MediaLabel::loadImage(track->get_cover(), wxSize(64, 64));
    title = wxString::FromUTF8(track->get_title());
    artist = wxString::FromUTF8(track->get_artist());
    album = wxString::FromUTF8(track->get_album());
    genre = wxString::FromUTF8(track->get_genre());

    // sortVerified = track->is_verified();
    if (track->is_verified()) {
        status = {100, DownloadState::Verified};
    } else if (track->get_spotifyTrack()) {
        status = {0, DownloadState::NotDownloaded};
    } else {
        status = {-1, DownloadState::Failed};
    }

    sortTitle = title.Lower();
    sortArtist = artist.Lower();
    sortAlbum = album.Lower();
    sortGenre = genre.Lower();
    sortLength = track->get_length();

    int minutes = (sortLength / 60) % 60;
    int seconds = sortLength % 60;

    wxString duration =
        wxString(std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") +
                 std::to_string(seconds));

    length = duration;
}
