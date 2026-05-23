#include "Media/Track/LocalTrackPanel.h"
#include "Interfaces/ITrack.h"
#include "Local/LocalTrackService.h"
#include "Media/Track/Renderers/DownloadStatusRenderer.h" // DownloadState
#include "Media/Track/TrackModel.h"
#include "Media/Track/TrackModelRow.h"

LocalTrackPanel::LocalTrackPanel(wxWindow *_parent) : TrackPanel(_parent) {

    ctrl->AppendBitmapColumn("Verify", TrackModel::COL_VERIFY,
                             wxDATAVIEW_CELL_INERT, 64, wxALIGN_CENTER);

    ctrl->AppendBitmapColumn("Delete", TrackModel::COL_DELETE,
                             wxDATAVIEW_CELL_INERT, 64, wxALIGN_CENTER);
}

void LocalTrackPanel::ApplyChangeToSelectedRows(LocalTrack::tag_type_t _type,
                                                const std::string &_value) {
    wxDataViewItemArray selectedDataViewItems;
    ctrl->GetSelections(selectedDataViewItems);
    for (auto item : selectedDataViewItems) {
        auto rowIndex = model->GetRow(item);
        auto row = model->GetRowByIndex(rowIndex);
        auto track = model->GetTrack(row);
        if (!track)
            return;
        auto local = std::dynamic_pointer_cast<LocalTrack>(track);
        if (!local)
            return;
        switch (_type) {
        case LocalTrack::tag_type_t::TITLE: {
            local->set_name(_value);
            break;
        }
        case LocalTrack::tag_type_t::ARTIST: {
            local->set_artist(_value);
            break;
        }
        case LocalTrack::tag_type_t::ALBUM: {
            local->set_albumName(_value);
            break;
        }
        case LocalTrack::tag_type_t::ALBUM_ARTIST: {
            local->set_albumArtist(_value);
            break;
        }
        case LocalTrack::tag_type_t::COPYRIGHT: {
            local->set_copyright(_value);
            break;
        }
        case LocalTrack::tag_type_t::GENRE: {
            local->set_genre(_value);
            break;
        }
        case LocalTrack::tag_type_t::LABEL: {
            local->set_label(_value);
            break;
        }
        case LocalTrack::tag_type_t::YEAR: {
            local->set_year(std::stoi(_value));
            break;
        }
        case LocalTrack::tag_type_t::TRACK: {
            local->set_trackNumber(std::stoul(_value));
            break;
        }
        case LocalTrack::tag_type_t::DISC: {
            local->set_discNumber(std::stoul(_value));
            break;
        }
        case LocalTrack::tag_type_t::FILENAME: {
            LocalTrackService::renameTrack(local, _value);
            break;
        }
        }
        model->RowChanged(rowIndex);
        // make sure that changes are already visible
        row->RebuildSortCache();
    }
}

bool LocalTrackPanel::HandleColumnAction(
    int _column, std::size_t _rowIndex, const std::shared_ptr<TrackModelRow> &_row,
    const std::shared_ptr<ITrack> &_track) {

    if (TrackPanel::HandleColumnAction(_column, _rowIndex, _row, _track)) {
        return true;
    }

    auto local = std::dynamic_pointer_cast<LocalTrack>(_track);
    if (!local)
        return false;

    if (_column == TrackModel::COL_DELETE) {
        wxMessageDialog dialog(this, "Delete selected track?",
                               "Confirm deletion", wxYES_NO | wxICON_WARNING);

        if (dialog.ShowModal() == wxID_YES) {
            model->RemoveRow(_rowIndex);
            LocalTrackService::deleteTrack(local);
        }

        return true;
    }

    if (_column == TrackModel::COL_VERIFY) {
        wxCommandEvent evt(EVT_TRACK_VERIFY);
        wxPostEvent(GetParent(), evt);

        // auto track = model->GetTrack(_row);
        // TagService::researchMissingTags(track, searchService);

        // model->RowChanged(_rowIndex);
        // _row->RebuildSortCache();
        return true;
    }

    return false;
}