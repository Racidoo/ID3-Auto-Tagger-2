#include "TrackPanel.h"
#include "TrackInterface.h"
#include "TrackModel.h"

TrackPanel::TrackPanel(wxWindow *parent) : wxPanel(parent) {
    auto *sizer = new wxBoxSizer(wxVERTICAL);

    ctrl = new wxDataViewCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                              wxDV_ROW_LINES | wxDV_VERT_RULES | wxDV_MULTIPLE);

    model = new TrackModel();

    // wxWidgets uses reference counting internally
    ctrl->AssociateModel(model);
    model->DecRef();

    // Columns
    ctrl->AppendBitmapColumn("", TrackModel::COL_PROGRESS,
                             wxDATAVIEW_CELL_INERT, 64, wxALIGN_CENTER,
                             wxDATAVIEW_COL_SORTABLE);

    ctrl->AppendBitmapColumn("Cover", TrackModel::COL_COVER,
                             wxDATAVIEW_CELL_INERT, 64, wxALIGN_CENTER);

    ctrl->AppendTextColumn("Title", TrackModel::COL_TITLE,
                           wxDATAVIEW_CELL_INERT, 250, wxALIGN_LEFT,
                           wxDATAVIEW_COL_SORTABLE);

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
                           wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);

    ctrl->AppendBitmapColumn("Verify", TrackModel::COL_VERIFY,
                             wxDATAVIEW_CELL_INERT, 64, wxALIGN_CENTER);

    ctrl->AppendBitmapColumn("Delete", TrackModel::COL_DELETE,
                             wxDATAVIEW_CELL_INERT, 64, wxALIGN_CENTER);

    ctrl->Bind(wxEVT_DATAVIEW_SELECTION_CHANGED,
               &TrackPanel::OnSelectionChanged, this);

    ctrl->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &TrackPanel::OnActivated, this);
    ctrl->Bind(wxEVT_DATAVIEW_COLUMN_SORTED, &TrackPanel::OnColumnSorted, this);

    sizer->Add(ctrl, 1, wxEXPAND);

    SetSizer(sizer);
}

void TrackPanel::Refresh() {}

void TrackPanel::AddTracks(
    const std::vector<std::shared_ptr<TrackInterface>> &_trackInterfaces) {
    std::vector<std::shared_ptr<TrackModel::TrackModelRow>> rows;
    for (auto &&trackInterface : _trackInterfaces) {
        rows.emplace_back(
            std::make_shared<TrackModel::TrackModelRow>(trackInterface));
    }
    model->AddRows(rows);
}

void TrackPanel::OnSelectionChanged(wxDataViewEvent &event) {
    wxDataViewItem item = event.GetItem();

    if (!item.IsOk())
        return;

    unsigned int row = model->GetRow(item);

    // const auto &track = model->GetTrack(row);

    std::cout << row << std::endl;
}

void TrackPanel::OnActivated(wxDataViewEvent &event) {
    unsigned int row = model->GetRow(event.GetItem());
    int column = event.GetColumn();

    std::cout << row << ", " << column << std::endl;

    if (column == TrackModel::COL_DELETE) {
        std::cout << "delete" << std::endl;
    }
}

void TrackPanel::OnColumnSorted(wxDataViewEvent &_event) {
    wxDataViewColumn *column = _event.GetDataViewColumn();

    if (!column)
        return;

    model->SortByHeader(column->GetModelColumn(),
                        column->IsSortOrderAscending());
}
