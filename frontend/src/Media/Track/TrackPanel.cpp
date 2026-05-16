#include "Media/Track/TrackPanel.h"
#include "Interfaces/ITrack.h"
#include "Media/Track/Renderers/DownloadStatusRenderer.h"
#include "Media/Track/TrackModel.h"
#include "Media/Track/TrackModelRow.h"

wxDEFINE_EVENT(EVT_TRACK_SELECTION_CHANGED, wxCommandEvent);
wxDEFINE_EVENT(EVT_TRACK_DOWNLOAD, wxCommandEvent);

TrackPanel::TrackPanel(wxWindow *_parent) : wxPanel(_parent) {
    auto *sizer = new wxBoxSizer(wxVERTICAL);

    ctrl = new wxDataViewCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                              wxDV_ROW_LINES | wxDV_VERT_RULES | wxDV_MULTIPLE);

    model = new TrackModel();

    // wxWidgets uses reference counting internally
    ctrl->AssociateModel(model);
    model->DecRef();

    // Columns
    ctrl->AppendColumn(
        new wxDataViewColumn("Download", new DownloadStatusRenderer(),
                             TrackModel::COL_PROGRESS, 64, wxALIGN_CENTER));

    ctrl->AppendBitmapColumn("Cover", TrackModel::COL_COVER,
                             wxDATAVIEW_CELL_INERT, 64, wxALIGN_CENTER);

    ctrl->AppendTextColumn("#", TrackModel::COL_TRACKNUMBER,
                           wxDATAVIEW_CELL_INERT, 32, wxALIGN_LEFT,
                           wxDATAVIEW_COL_SORTABLE);

    ctrl->AppendTextColumn("Title", TrackModel::COL_TITLE,
                           wxDATAVIEW_CELL_INERT, 250, wxALIGN_LEFT,
                           wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);

    ctrl->AppendTextColumn("Artist", TrackModel::COL_ARTIST,
                           wxDATAVIEW_CELL_INERT, 200, wxALIGN_LEFT,
                           wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);

    ctrl->AppendTextColumn("Album", TrackModel::COL_ALBUM,
                           wxDATAVIEW_CELL_INERT, 200, wxALIGN_LEFT,
                           wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);

    ctrl->AppendTextColumn("Genre", TrackModel::COL_GENRE,
                           wxDATAVIEW_CELL_INERT, 200, wxALIGN_LEFT,
                           wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);

    ctrl->AppendTextColumn("Length", TrackModel::COL_LENGTH,
                           wxDATAVIEW_CELL_INERT, 64, wxALIGN_LEFT,
                           wxDATAVIEW_COL_SORTABLE);

    // ctrl->AppendBitmapColumn("Verify", TrackModel::COL_VERIFY,
    //                          wxDATAVIEW_CELL_INERT, 64, wxALIGN_CENTER);

    // ctrl->AppendBitmapColumn("Delete", TrackModel::COL_DELETE,
    //                          wxDATAVIEW_CELL_INERT, 64, wxALIGN_CENTER);

    ctrl->Bind(wxEVT_DATAVIEW_SELECTION_CHANGED,
               &TrackPanel::OnSelectionChanged, this);
    ctrl->Bind(wxEVT_DATAVIEW_ITEM_VALUE_CHANGED,
               &TrackPanel::OnSelectionChanged, this);
    ctrl->GetMainWindow()->Bind(wxEVT_LEFT_DOWN, &TrackPanel::OnLeftDown, this);
    ctrl->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &TrackPanel::OnActivated, this);
    ctrl->Bind(wxEVT_DATAVIEW_COLUMN_SORTED, &TrackPanel::OnColumnSorted, this);

    sizer->Add(ctrl, 1, wxEXPAND);

    SetSizer(sizer);
}

void TrackPanel::Refresh() {}

void TrackPanel::MergeTracks(
    const std::vector<std::shared_ptr<ITrack>> &_batch) {
    model->MergeRows(_batch);
}

std::vector<std::size_t> TrackPanel::GetSelectedRows() const {
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

std::vector<std::shared_ptr<ITrack>>
TrackPanel::GetTracksOfSelectedRows() const {
    std::vector<std::shared_ptr<ITrack>> tracks;
    for (auto &&row : GetSelectedRows()) {
        tracks.push_back(GetTrack(row));
    }
    return tracks;
}

void TrackPanel::SetDownloadProgress(unsigned _row, int _progress) {
    model->SetDownloadStatusByRow(_row,
                                  {_progress, DownloadState::Downloading});
}

std::shared_ptr<ITrack> TrackPanel::GetTrack(std::size_t _row) const {
    return model->GetTrack(model->GetRowByIndex(_row));
}

void TrackPanel::Search(const wxString &_query, bool _showVerified,
                        bool _showUnverified) {
    model->SetFilterState(
        {_showVerified, _showUnverified, _query.ToStdString()});
}

void TrackPanel::OnSelectionChanged(wxDataViewEvent &) {
    wxCommandEvent evt(EVT_TRACK_SELECTION_CHANGED);
    wxPostEvent(GetParent(), evt);
}

void TrackPanel::OnActivated(wxDataViewEvent &event) {
    unsigned rowIndex = model->GetRow(event.GetItem());
    int column = event.GetColumn();

    auto row = model->GetRowByIndex(rowIndex);
    const auto &track = model->GetTrack(row);

    if (HandleColumnAction(column, rowIndex, row, track)) {
        return;
    }

    // optional fallback/default behavior
}

void TrackPanel::OnColumnSorted(wxDataViewEvent &_event) {
    wxDataViewColumn *column = _event.GetDataViewColumn();

    if (!column)
        return;

    model->SortByHeader(column->GetModelColumn(),
                        column->IsSortOrderAscending());
}

void TrackPanel::OnLeftDown(wxMouseEvent &event) {
    wxDataViewItem item;
    wxDataViewColumn *column = nullptr;

    auto pos = event.GetPosition();

    ctrl->HitTest(pos, item, column);

    if (item.IsOk() && column) {

        int modelColumn = column->GetModelColumn();

        switch (column->GetModelColumn()) {
        case TrackModel::Columns::COL_DELETE:
            std::cout << "OnLeftDown(): delete" << std::endl;
            break;
        case TrackModel::Columns::COL_VERIFY:
            std::cout << "OnLeftDown(): verify" << std::endl;
            break;

        default:
            break;
        }
    } else {
        std::cout << item.IsOk() << " && " << column << std::endl;
    }

    event.Skip();
}

bool TrackPanel::HandleColumnAction(int _column, unsigned _rowIndex,
                                    const std::shared_ptr<TrackModelRow> &_row,
                                    const std::shared_ptr<ITrack> &_track) {

    if (_column == TrackModel::COL_PROGRESS) {
        wxCommandEvent evt(EVT_TRACK_DOWNLOAD);
        wxPostEvent(GetParent(), evt);
        return true;
    }

    return false;
}