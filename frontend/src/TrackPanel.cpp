#include "TrackPanel.h"
#include "Downloader.h"
#include "TrackInterface.h"
#include "TrackModel.h"
#include "TrackModelRow.h"

wxDEFINE_EVENT(EVT_TRACK_SELECTION_CHANGED, wxCommandEvent);

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
    ctrl->Bind(wxEVT_DATAVIEW_ITEM_VALUE_CHANGED,
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

std::vector<std::shared_ptr<TrackInterface>> TrackPanel::GetSelectedRows() {
    std::vector<std::shared_ptr<TrackInterface>> selectedTracks;
    wxDataViewItemArray selectedDataViewItems;
    ctrl->GetSelections(selectedDataViewItems);
    for (auto &&item : selectedDataViewItems) {
        if (!item.IsOk()) {
            wxLogDebug("Item selected but not valid");
            continue;
        }
        selectedTracks.push_back(
            model->GetTrack(model->GetRowByIndex(model->GetRow(item))));
    }
    return selectedTracks;
}

void TrackPanel::ApplyChangeToSelectedRows(LocalTrack::tag_type_t _type,
                                           const std::string &_value) {
    wxDataViewItemArray selectedDataViewItems;
    ctrl->GetSelections(selectedDataViewItems);
    for (auto item : selectedDataViewItems) {
        auto rowIndex = model->GetRow(item);
        auto row = model->GetRowByIndex(rowIndex);
        auto track = model->GetTrack(row);
        if (!track)
            return;
        switch (_type) {
        case LocalTrack::tag_type_t::TITLE: {
            track->set_title(_value);
            break;
        }
        case LocalTrack::tag_type_t::ARTIST: {
            track->set_artist(_value);
            break;
        }
        case LocalTrack::tag_type_t::ALBUM: {
            track->set_album(_value);
            break;
        }
        case LocalTrack::tag_type_t::ALBUM_ARTIST: {
            track->set_albumArtist(_value);
            break;
        }
        case LocalTrack::tag_type_t::COPYRIGHT: {
            track->set_copyright(_value);
            break;
        }
        case LocalTrack::tag_type_t::GENRE: {
            track->set_genre(_value);
            break;
        }
        case LocalTrack::tag_type_t::LABEL: {
            track->set_label(_value);
            break;
        }
        case LocalTrack::tag_type_t::YEAR: {
            track->set_year(_value);
            break;
        }
        case LocalTrack::tag_type_t::TRACK: {
            track->set_trackNumber(_value);
            break;
        }
        case LocalTrack::tag_type_t::DISC: {
            track->set_discNumber(_value);
            break;
        }
        case LocalTrack::tag_type_t::FILENAME: {
            track->get_localTrack()->renameLocalTrack(_value);
            break;
        }
        }
        model->RowChanged(rowIndex);
        // make sure that changes are already visible
        row->RebuildSortCache();
    }
}

void TrackPanel::Search(const wxString &_query, bool _showVerified,
                        bool _showUnverified) {
    model->SetFilterState({_showVerified, _showUnverified, _query});
}

void TrackPanel::OnSelectionChanged(wxDataViewEvent &) {
    wxCommandEvent evt(EVT_TRACK_SELECTION_CHANGED);
    wxPostEvent(GetParent(), evt);
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
