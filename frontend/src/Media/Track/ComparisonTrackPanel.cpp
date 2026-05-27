#include "Media/Track/ComparisonTrackPanel.h"
#include "Media/Track/TrackModel.h"

ComparisonTrackPanel::ComparisonTrackPanel(wxWindow *_parent)
    : TrackPanel(_parent) {

    ctrl->GetColumn(TrackModel::COL_PROGRESS)->SetHidden(true);

    ctrl->AppendTextColumn("Album Artist", TrackModel::COL_ALBUM_ARTIST,
                           wxDATAVIEW_CELL_INERT, 200, wxALIGN_LEFT,
                           wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
    ctrl->AppendTextColumn("Disc-No.", TrackModel::COL_DISCNUMBER,
                           wxDATAVIEW_CELL_INERT, 32, wxALIGN_LEFT,
                           wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
    ctrl->AppendTextColumn("Copyright", TrackModel::COL_COPYRIGHT,
                           wxDATAVIEW_CELL_INERT, 200, wxALIGN_LEFT,
                           wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
    ctrl->AppendTextColumn("Publisher", TrackModel::COL_LABEL,
                           wxDATAVIEW_CELL_INERT, 200, wxALIGN_LEFT,
                           wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
    ctrl->AppendTextColumn("Year", TrackModel::COL_YEAR, wxDATAVIEW_CELL_INERT,
                           64, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE);
}