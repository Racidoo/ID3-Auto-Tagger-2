#include "TrackPanel.h"
#include "Downloader.h"
#include "TrackInterface.h"
#include "TrackModel.h"
#include "TrackModelRow.h"

TrackPanel::TrackPanel(wxWindow *parent, Downloader *_downloader)
    : wxPanel(parent), downloader(_downloader) {
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
    ctrl->GetMainWindow()->Bind(wxEVT_LEFT_DOWN, &TrackPanel::OnLeftDown, this);
    ctrl->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &TrackPanel::OnActivated, this);
    ctrl->Bind(wxEVT_DATAVIEW_COLUMN_SORTED, &TrackPanel::OnColumnSorted, this);

    sizer->Add(ctrl, 1, wxEXPAND);

    SetSizer(sizer);
}

Downloader *TrackPanel::get_downloader() const { return downloader; }

void TrackPanel::Refresh() {}

void TrackPanel::MergeTracks(
    const std::vector<std::shared_ptr<TrackInterface>> &_batch) {
    model->MergeRows(_batch);
}

void TrackPanel::Search(const wxString &_query, bool _showVerified,
                        bool _showUnverified) {
    model->SetFilterState({_showVerified, _showUnverified, _query});
}

void TrackPanel::OnSelectionChanged(wxDataViewEvent &event) {
    wxDataViewItem item = event.GetItem();

    if (!item.IsOk())
        return;

    unsigned int rowIndex = model->GetRow(item);
    std::cout << rowIndex << std::endl;

    auto row = model->GetRowByIndex(rowIndex);

    std::cout << "verified: " << row->get_sortVerified()
              << "\ntitle: " << row->get_title()
              << "\nartist: " << row->get_artist()
              << "\nalbum: " << row->get_album()
              << "\ngenre: " << row->get_genre() << "\n"

        ;
}

void TrackPanel::OnActivated(wxDataViewEvent &event) {
    unsigned int rowIndex = model->GetRow(event.GetItem());
    int column = event.GetColumn();

    std::cout << rowIndex << ", " << column << std::endl;

    auto row = model->GetRowByIndex(rowIndex);
    const auto &track = model->GetTrack(row);

    if (column == TrackModel::COL_DELETE) {
        wxMessageDialog dialog(this, "Delete selected track?",
                               "Confirm deletion", wxYES_NO | wxICON_WARNING);

        if (dialog.ShowModal() == wxID_YES) {

            model->RemoveRow(rowIndex);
            downloader->deleteLocalTrack(track);
        }
        return;
    } else if (column == TrackModel::COL_VERIFY) {
        if (!TrackInterface::verify(track, downloader)) {
            wxMessageBox("Unable to retrieve meta data from SpotifyAPI");
            return;
        }
        model->RowChanged(rowIndex);
        // make sure that changes are already visible
        row->RebuildSortCache();
    }
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
