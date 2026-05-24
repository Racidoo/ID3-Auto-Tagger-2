#include "Media/Track/TrackModelRow.h"
#include "Components/MediaLabel.h"
#include "Core/IconProvider.h"
#include "Interfaces/ITrack.h"

TrackModelRow::TrackModelRow(const std::shared_ptr<ITrack> &_trackInterface)
    : track(_trackInterface) {
    RebuildSortCache();
}

TrackModelRow::~TrackModelRow() {}

std::shared_ptr<ITrack> TrackModelRow::get_track() const { return track; }

const DownloadStatus &TrackModelRow::get_status() const { return status; }
const wxBitmap &TrackModelRow::get_cover() const { return cover; }
const wxString &TrackModelRow::get_title() const { return title; }
const wxString &TrackModelRow::get_artist() const { return artist; }
const wxString &TrackModelRow::get_album() const { return album; }
const wxString &TrackModelRow::get_genre() const { return genre; }
const wxString &TrackModelRow::get_length() const { return length; }
const wxString &TrackModelRow::get_tracknumber() const { return tracknumber; }

const std::optional<std::string> &TrackModelRow::get_sortTitle() const {
    return sortTitle;
}
const std::optional<std::string> &TrackModelRow::get_sortArtist() const {
    return sortArtist;
}
const std::optional<std::string> &TrackModelRow::get_sortAlbum() const {
    return sortAlbum;
}
const std::optional<std::string> &TrackModelRow::get_sortGenre() const {
    return sortGenre;
}
std::size_t TrackModelRow::get_sortLength() const { return sortLength; }
std::optional<std::size_t> TrackModelRow::get_sortTracknumber() const {
    return sortTracknumber;
}

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

    cover = MediaLabel::loadImage(track->get_image(), wxSize(64, 64));
    title = wxString::FromUTF8(track->get_name().value_or(std::string{}));
    artist = wxString::FromUTF8(track->get_artist().value_or(std::string{}));
    album = wxString::FromUTF8(track->get_albumName().value_or(std::string{}));
    genre = wxString::FromUTF8(track->get_genre().value_or(std::string{}));
    tracknumber = track->get_trackNumber().has_value()
                      ? std::to_string(track->get_trackNumber().value())
                      : "";

    if (track->is_verified()) {
        status = {100, DownloadState::Verified};
    } else {
        status = {0, DownloadState::NotDownloaded};
    }
    // else {
    //     status = {-1, DownloadState::Failed};
    // }

    sortTitle = title.Lower();
    sortArtist = artist.Lower();
    sortAlbum = album.Lower();
    sortGenre = genre.Lower();
    sortLength = track->get_length();
    sortTracknumber = track->get_trackNumber();

    int minutes = (sortLength / 60) % 60;
    int seconds = sortLength % 60;

    wxString duration =
        wxString(std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") +
                 std::to_string(seconds));

    length = duration;
}
