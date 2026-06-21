#pragma once

#include <memory>
#include <wx/wx.h>

#include "Media/Track/Renderers/DownloadStatusRenderer.h"

class IVideo;

class VideoModelRow {
  private:
    std::shared_ptr<IVideo> video;

    wxBitmap cover;
    wxString title;
    wxString channel;
    wxString length;
    wxString uploadDate;

    DownloadStatus status;
    std::optional<std::string> sortTitle;
    std::optional<std::string> sortChannel;
    std::size_t sortLength;
    std::optional<std::string> sortUploadDate;

  public:
    VideoModelRow(const std::shared_ptr<IVideo> &_videoInterface);
    // wxWidgets don't support copy operations correctly
    VideoModelRow(const VideoModelRow &) = delete;
    VideoModelRow(VideoModelRow &) = delete;
    VideoModelRow &operator=(const VideoModelRow &) = delete;

    ~VideoModelRow();

    std::shared_ptr<IVideo> get_video() const;
    const DownloadStatus &get_status() const;
    const wxBitmap &get_cover() const;
    const wxString &get_title() const;
    const wxString &get_channel() const;
    const wxString &get_length() const;
    const wxString &get_uploadDate() const;

    const std::optional<std::string> &get_sortTitle() const;
    const std::optional<std::string> &get_sortChannel() const;
    std::size_t get_sortLength() const;
    std::optional<std::string> get_sortUploadDate() const;

    void set_status(const DownloadStatus &_status);

    static const wxBitmap &GetCheckmarkBitmap();
    static const wxBitmap &GetXmarkBitmap();
    void RebuildSortCache();
};