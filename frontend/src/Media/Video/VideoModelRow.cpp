#include "Media/Video/VideoModelRow.h"
#include "Components/MediaLabel.h"
#include "Core/IconProvider.h"
#include "Interfaces/IVideo.h"

VideoModelRow::VideoModelRow(const std::shared_ptr<IVideo> &_videoInterface)
    : video(_videoInterface) {
    RebuildSortCache();
}

VideoModelRow::~VideoModelRow() {}

std::shared_ptr<IVideo> VideoModelRow::get_video() const { return video; }

const DownloadStatus &VideoModelRow::get_status() const { return status; }
const wxBitmap &VideoModelRow::get_cover() const { return cover; }
const wxString &VideoModelRow::get_title() const { return title; }
const wxString &VideoModelRow::get_channel() const { return channel; }
const wxString &VideoModelRow::get_length() const { return length; }
const wxString &VideoModelRow::get_uploadDate() const { return uploadDate; }

const std::optional<std::string> &VideoModelRow::get_sortTitle() const {
    return sortTitle;
}
const std::optional<std::string> &VideoModelRow::get_sortChannel() const {
    return sortChannel;
}
std::size_t VideoModelRow::get_sortLength() const { return sortLength; }
std::optional<std::string> VideoModelRow::get_sortUploadDate() const {
    return sortUploadDate;
}

void VideoModelRow::set_status(const DownloadStatus &_status) {
    status = _status;
}

const wxBitmap &VideoModelRow::GetCheckmarkBitmap() {
    static wxBitmap bitmap = [] {
        wxBitmap bmp =
            wxArtProvider::GetBitmap(wxART_CIRCLE_CHECKMARK, wxART_BUTTON);
        return bmp;
    }();
    return bitmap;
}

const wxBitmap &VideoModelRow::GetXmarkBitmap() {
    static wxBitmap bitmap = [] {
        wxBitmap bmp =
            wxArtProvider::GetBitmap(wxART_CIRCLE_XMARK, wxART_BUTTON);
        return bmp;
    }();
    return bitmap;
}

void VideoModelRow::RebuildSortCache() {

    cover = MediaLabel::loadImage(video->get_image(), wxSize(64, 64));
    title = wxString::FromUTF8(video->get_name().value_or(std::string{}));
    channel = wxString::FromUTF8(video->get_channelTitle());
    uploadDate = video->get_uploadDate();

    status = {0, DownloadState::NotDownloaded};

    sortTitle = title.Lower();
    sortChannel = channel.Lower();
    sortLength = video->get_length();
    sortUploadDate = video->get_uploadDate();

    int minutes = (sortLength / 60) % 60;
    int seconds = sortLength % 60;

    wxString duration =
        wxString(std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") +
                 std::to_string(seconds));

    length = duration;
}
