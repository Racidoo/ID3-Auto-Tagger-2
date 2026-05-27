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
    wxString tracknumber;
    wxString length;
    wxString albumArtist;
    wxString discnumber;
    wxString copyright;
    wxString label;
    wxString year;

    DownloadStatus status;
    std::optional<std::string> sortTitle;
    std::optional<std::string> sortArtist;
    std::optional<std::string> sortAlbum;
    std::optional<std::string> sortGenre;
    std::optional<std::size_t> sortTracknumber;
    std::size_t sortLength;
    std::optional<std::string> sortAlbumArtist;
    std::optional<std::size_t> sortDiscnumber;
    std::optional<std::string> sortCopyright;
    std::optional<std::string> sortLabel;
    std::optional<std::size_t> sortYear;

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
    const wxString &get_tracknumber() const;
    const wxString &get_albumArtist() const;
    const wxString &get_discnumber() const;
    const wxString &get_copyright() const;
    const wxString &get_label() const;
    const wxString &get_year() const;

    const std::optional<std::string> &get_sortTitle() const;
    const std::optional<std::string> &get_sortArtist() const;
    const std::optional<std::string> &get_sortAlbum() const;
    const std::optional<std::string> &get_sortGenre() const;
    std::size_t get_sortLength() const;
    std::optional<std::size_t> get_sortTracknumber() const;
    const std::optional<std::string> &get_sortAlbumArtist() const;
    std::optional<std::size_t> get_sortDiscnumber() const;
    const std::optional<std::string> &get_sortCopyright() const;
    const std::optional<std::string> &get_sortLabel() const;
    std::optional<std::size_t> get_sortYear() const;

    void set_status(const DownloadStatus &_status);

    static const wxBitmap &GetCheckmarkBitmap();
    static const wxBitmap &GetXmarkBitmap();
    static const wxBitmap &GetVerifyBitmap();
    static const wxBitmap &GetDeleteBitmap();
    void RebuildSortCache();
};