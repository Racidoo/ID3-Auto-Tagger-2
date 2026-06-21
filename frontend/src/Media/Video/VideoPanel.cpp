#include "Media/Video/VideoPanel.h"
#include "Interfaces/IVideo.h"
#include "Media/Video/VideoModel.h"
#include "Media/Video/VideoModelRow.h"

wxDEFINE_EVENT(EVT_VIDEO_DOWNLOAD, wxCommandEvent);
wxDEFINE_EVENT(EVT_VIDEO_SELECTION_CHANGED, wxCommandEvent);

VideoPanel::VideoPanel(wxWindow *_parent) : wxPanel(_parent) {
    auto *sizer = new wxBoxSizer(wxVERTICAL);

    ctrl = new wxDataViewCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                              wxDV_ROW_LINES | wxDV_VERT_RULES | wxDV_MULTIPLE);

    model = new VideoModel();

    // wxWidgets uses reference counting internally
    ctrl->AssociateModel(model);
    model->DecRef();

    // Columns
    ctrl->AppendColumn(
        new wxDataViewColumn("Download", new DownloadStatusRenderer(),
                             VideoModel::COL_PROGRESS, 64, wxALIGN_CENTER));

    ctrl->AppendBitmapColumn("Cover", VideoModel::COL_COVER,
                             wxDATAVIEW_CELL_INERT, 64, wxALIGN_CENTER);

    ctrl->AppendTextColumn("Title", VideoModel::COL_TITLE,
                           wxDATAVIEW_CELL_INERT, 250, wxALIGN_LEFT,
                           wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);

    ctrl->AppendTextColumn("Channel", VideoModel::COL_CHANNEL,
                           wxDATAVIEW_CELL_INERT, 200, wxALIGN_LEFT,
                           wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);

    ctrl->AppendTextColumn("Length", VideoModel::COL_LENGTH,
                           wxDATAVIEW_CELL_INERT, 64, wxALIGN_LEFT,
                           wxDATAVIEW_COL_SORTABLE);

    ctrl->Bind(wxEVT_DATAVIEW_SELECTION_CHANGED,
               &VideoPanel::OnSelectionChanged, this);
    ctrl->Bind(wxEVT_DATAVIEW_ITEM_VALUE_CHANGED,
               &VideoPanel::OnSelectionChanged, this);
    ctrl->GetMainWindow()->Bind(wxEVT_LEFT_DOWN, &VideoPanel::OnLeftDown, this);
    ctrl->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &VideoPanel::OnActivated, this);
    ctrl->Bind(wxEVT_DATAVIEW_COLUMN_SORTED, &VideoPanel::OnColumnSorted, this);

    sizer->Add(ctrl, 1, wxEXPAND);

    SetSizer(sizer);
}

void VideoPanel::Refresh() {}

void VideoPanel::Merge(const std::vector<std::shared_ptr<IVideo>> &_batch) {
    model->MergeRows(_batch);
}

std::vector<std::size_t> VideoPanel::GetSelectedRows() const {
    std::vector<std::size_t> selectedRows;
    wxDataViewItemArray selectedDataViewItems;
    ctrl->GetSelections(selectedDataViewItems);
    for (auto &&item : selectedDataViewItems) {
        if (!item.IsOk()) {
            wxLogDebug("Item selected but not valid");
            continue;
        }
        selectedRows.push_back(model->GetRow(item));
    }
    return selectedRows;
}

std::vector<std::shared_ptr<IVideo>>
VideoPanel::GetVideosOfSelectedRows() const {
    std::vector<std::shared_ptr<IVideo>> videos;
    for (auto &&row : GetSelectedRows()) {
        videos.push_back(GetVideo(row));
    }
    return videos;
}

void VideoPanel::SetDownloadProgress(std::size_t _row, int _progress) {
    model->SetDownloadStatusByRow(_row,
                                  {_progress, DownloadState::Downloading});
}

void VideoPanel::UpdateRow(std::size_t _row) {
    model->GetRowByIndex(_row)->RebuildSortCache();
    model->RowChanged(_row);
}

std::shared_ptr<IVideo> VideoPanel::GetVideo(std::size_t _row) const {
    return model->GetVideo(model->GetRowByIndex(_row));
}

void VideoPanel::Search(const wxString &_query) {
    model->SetFilterState({_query.ToStdString()});
}

void VideoPanel::OnSelectionChanged(wxDataViewEvent &) {
    wxCommandEvent evt(EVT_VIDEO_SELECTION_CHANGED);
    wxPostEvent(GetParent(), evt);
}

void VideoPanel::OnActivated(wxDataViewEvent &event) {
    unsigned rowIndex = model->GetRow(event.GetItem());
    int column = event.GetColumn();

    auto row = model->GetRowByIndex(rowIndex);
    const auto &video = model->GetVideo(row);

    std::cout << rowIndex << ", " << column << " id: " << video->get_id()
              << std::endl;

    if (HandleColumnAction(column, rowIndex, row, video)) {
        return;
    }

    // optional fallback/default behavior
}

void VideoPanel::OnColumnSorted(wxDataViewEvent &_event) {
    wxDataViewColumn *column = _event.GetDataViewColumn();

    if (!column)
        return;

    model->SortByHeader(column->GetModelColumn(),
                        column->IsSortOrderAscending());
}

void VideoPanel::OnLeftDown(wxMouseEvent &event) {
    wxDataViewItem item;
    wxDataViewColumn *column = nullptr;

    auto pos = event.GetPosition();

    ctrl->HitTest(pos, item, column);

    // if (item.IsOk() && column) {

    //     int modelColumn = column->GetModelColumn();

    //     switch (column->GetModelColumn()) {
    //     case VideoModel::Columns::COL_DELETE:
    //         std::cout << "OnLeftDown(): delete" << std::endl;
    //         break;
    //     case VideoModel::Columns::COL_VERIFY:
    //         std::cout << "OnLeftDown(): verify" << std::endl;
    //         break;

    //     default:
    //         break;
    //     }
    // } else {
    //     std::cout << item.IsOk() << " && " << column << std::endl;
    // }

    event.Skip();
}

bool VideoPanel::HandleColumnAction(int _column, std::size_t _rowIndex,
                                    const std::shared_ptr<VideoModelRow> &_row,
                                    const std::shared_ptr<IVideo> &_video) {

    if (_column == VideoModel::COL_PROGRESS) {
        wxCommandEvent evt(EVT_VIDEO_DOWNLOAD);
        wxPostEvent(GetParent(), evt);
        return true;
    }

    return false;
}