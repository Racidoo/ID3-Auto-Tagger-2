#pragma once

#include <memory>
#include <wx/wx.h>

#include "Media/Track/Renderers/DownloadStatusRenderer.h"

class ITrack;

class TrackModelRow {
  private:
    std::shared_ptr<ITrack> track;

    wxBitmap cover;
    wxString title;
    wxString artist;
    wxString album;
    wxString genre;
    wxString length;

    // bool sortVerified;
    DownloadStatus status;
    std::string sortTitle;
    std::string sortArtist;
    std::string sortAlbum;
    std::string sortGenre;
    std::size_t sortLength;

  public:
    TrackModelRow(const std::shared_ptr<ITrack> &_trackInterface);
    // wxWidgets don't support copy operations correctly
    TrackModelRow(const TrackModelRow &) = delete;
    TrackModelRow(TrackModelRow &) = delete;
    TrackModelRow &operator=(const TrackModelRow &) = delete;

    ~TrackModelRow();

    std::shared_ptr<ITrack> get_track() const;
    const DownloadStatus &get_status() const;
    const wxBitmap &get_cover() const;
    const wxString &get_title() const;
    const wxString &get_artist() const;
    const wxString &get_album() const;
    const wxString &get_genre() const;
    const wxString &get_length() const;

    // bool get_sortVerified() const;
    const std::string &get_sortTitle() const;
    const std::string &get_sortArtist() const;
    const std::string &get_sortAlbum() const;
    const std::string &get_sortGenre() const;
    std::size_t get_sortLength() const;

    void set_status(const DownloadStatus &_status);

    static const wxBitmap &GetCheckmarkBitmap();
    static const wxBitmap &GetXmarkBitmap();
    static const wxBitmap &GetVerifyBitmap();
    static const wxBitmap &GetDeleteBitmap();
    void RebuildSortCache();
};