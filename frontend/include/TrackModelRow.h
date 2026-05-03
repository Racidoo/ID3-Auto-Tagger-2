#pragma once

#include <memory>
#include <wx/wx.h>

class TrackInterface;

class TrackModelRow {
  private:
    std::shared_ptr<TrackInterface> track;

    wxBitmap cover;
    wxString title;
    wxString artist;
    wxString album;
    wxString genre;
    wxString length;

    bool sortVerified;
    wxString sortTitle;
    wxString sortArtist;
    wxString sortAlbum;
    wxString sortGenre;
    std::size_t sortLength;

  public:
    TrackModelRow(const std::shared_ptr<TrackInterface> &_trackInterface);
    // wxWidgets don't support copy operations correctly
    TrackModelRow(const TrackModelRow &) = delete;
    TrackModelRow(TrackModelRow &) = delete;
    TrackModelRow &operator=(const TrackModelRow &) = delete;

    ~TrackModelRow();

    std::shared_ptr<TrackInterface> get_track() const;
    wxBitmap get_verified() const;
    wxBitmap get_cover() const;
    wxString get_title() const;
    wxString get_artist() const;
    wxString get_album() const;
    wxString get_genre() const;
    wxString get_length() const;

    bool get_sortVerified() const;
    wxString get_sortTitle() const;
    wxString get_sortArtist() const;
    wxString get_sortAlbum() const;
    wxString get_sortGenre() const;
    std::size_t get_sortLength() const;

    static const wxBitmap &GetCheckmarkBitmap();
    static const wxBitmap &GetXmarkBitmap();
    static const wxBitmap &GetVerifyBitmap();
    static const wxBitmap &GetDeleteBitmap();
    void RebuildSortCache();
};