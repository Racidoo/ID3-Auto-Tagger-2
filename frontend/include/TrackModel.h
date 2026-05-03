#pragma once

#include <memory>
#include <thread>
#include <vector>
#include <wx/dataview.h>
#include <wx/wx.h>

class TrackInterface;

class TrackModel : public wxDataViewVirtualListModel {
  public:
    enum Columns {
        COL_PROGRESS,
        COL_COVER,
        COL_TITLE,
        COL_ARTIST,
        COL_ALBUM,
        COL_GENRE,
        COL_LENGTH,
        COL_VERIFY,
        COL_DELETE,
        COL_MAX
    };

    class TrackModelRow {
      private:
        wxBitmap verified;
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

        std::thread workerAlbumCover;

      public:
        TrackModelRow(const std::shared_ptr<TrackInterface> &_trackInterface);

        ~TrackModelRow();

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

        static const wxBitmap &GetVerifyBitmap();
        static const wxBitmap &GetDeleteBitmap();
    };

    struct FilterState {
        bool showVerified;
        bool showUnverified;

        wxString searchQuery;
    };

    explicit TrackModel();

    void SetFilterState(const FilterState &_state);

    void AddRows(const std::vector<std::shared_ptr<TrackModelRow>> &_batch);
    void MergeRows(const std::vector<std::shared_ptr<TrackInterface>> &_batch);

    void RebuildVisibleTracks();
    bool MatchesSearch(const std::shared_ptr<TrackModelRow> &row) const;
    bool MatchesFilter(const std::shared_ptr<TrackModelRow> &row) const;
    unsigned int GetColumnCount() const override;
    wxString GetColumnType(unsigned int col) const override;
    void GetValueByRow(wxVariant &variant, unsigned int row,
                       unsigned int col) const override;
    bool SetValueByRow(const wxVariant &, unsigned int, unsigned int) override;
    // bool HasDefaultCompare() const override;
    // int Compare(const wxDataViewItem &_item1, const wxDataViewItem &_item2,
    //             unsigned int _column, bool _ascending) const override;

    void SortByHeader(unsigned int _column, bool _ascending);

  private:
    std::vector<std::shared_ptr<TrackModelRow>> visibleRows;
    std::unordered_map<std::string, std::shared_ptr<TrackModelRow>> allTracks;

    FilterState filterState;
};
