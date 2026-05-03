#pragma once

#include <memory>
#include <vector>
#include <wx/dataview.h>
#include <wx/wx.h>

class TrackInterface;
class TrackModelRow;

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

    struct FilterState {
        bool showVerified;
        bool showUnverified;

        wxString searchQuery;
    };

    explicit TrackModel();

    std::shared_ptr<TrackInterface>
    GetTrack(std::shared_ptr<TrackModelRow> _row);
    std::shared_ptr<TrackModelRow> GetRowByIndex(std::size_t _index) const;

    void SetFilterState(const FilterState &_state);

    void AddRows(const std::vector<std::shared_ptr<TrackModelRow>> &_batch);
    void MergeRows(const std::vector<std::shared_ptr<TrackInterface>> &_batch);
    void RemoveRow(std::size_t _rowIndex);
    void RebuildVisibleTracks();
    bool MatchesSearch(const std::shared_ptr<TrackModelRow> &_row) const;
    bool MatchesFilter(const std::shared_ptr<TrackModelRow> &_row) const;
    unsigned int GetColumnCount() const override;
    wxString GetColumnType(unsigned int _col) const override;
    void GetValueByRow(wxVariant &_variant, unsigned int _row,
                       unsigned int _col) const override;
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
