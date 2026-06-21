#pragma once

#include <memory>
#include <vector>
#include <wx/dataview.h>
#include <wx/wx.h>

class IVideo;
class VideoModelRow;
class DownloadStatus;

class VideoModel : public wxDataViewVirtualListModel {
  public:
    enum Columns {
        COL_PROGRESS,
        COL_COVER,
        COL_TITLE,
        COL_CHANNEL,
        COL_LENGTH,
        COL_UPLOAD_DATE,
        COL_MAX
    };

    struct FilterState {
        // bool showVerified;
        // bool showUnverified;

        std::string searchQuery;
    };

    explicit VideoModel();

    std::shared_ptr<IVideo> GetVideo(std::shared_ptr<VideoModelRow> _row);
    std::shared_ptr<VideoModelRow> GetRowByIndex(std::size_t _index) const;

    void SetFilterState(const FilterState &_state);

    void AddRows(const std::vector<std::shared_ptr<VideoModelRow>> &_batch);
    void MergeRows(const std::vector<std::shared_ptr<IVideo>> &_batch);
    void RemoveRow(std::size_t _rowIndex);
    void RebuildVisibleVideos();
    bool MatchesSearch(const std::shared_ptr<VideoModelRow> &_row) const;
    // bool MatchesFilter(const std::shared_ptr<VideoModelRow> &_row) const;
    unsigned int GetColumnCount() const override;
    wxString GetColumnType(unsigned int _col) const override;
    void GetValueByRow(wxVariant &_variant, unsigned int _row,
                       unsigned int _col) const override;
    bool SetValueByRow(const wxVariant &, unsigned int, unsigned int) override;
    // bool HasDefaultCompare() const override;
    // int Compare(const wxDataViewItem &_item1, const wxDataViewItem &_item2,
    //             unsigned int _column, bool _ascending) const override;
    void SetDownloadStatusByRow(std::size_t _row,
                                const DownloadStatus &_status);
    void SortByHeader(unsigned int _column, bool _ascending);

  private:
    std::vector<std::shared_ptr<VideoModelRow>> visibleRows;
    std::unordered_map<std::string, std::shared_ptr<VideoModelRow>> allVideos;

    FilterState filterState;
};
