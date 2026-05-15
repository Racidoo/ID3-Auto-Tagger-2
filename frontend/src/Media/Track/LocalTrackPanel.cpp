#include "Media/Track/LocalTrackPanel.h"
#include "Downloader.h"
#include "Interfaces/ITrack.h"
#include "Media/Track/Renderers/DownloadStatusRenderer.h" // DownloadState
#include "Media/Track/TrackModel.h"
#include "Media/Track/TrackModelRow.h"

LocalTrackPanel::LocalTrackPanel(wxWindow *_parent, Downloader *_downloader)
    : TrackPanel(_parent), downloader(_downloader) {

    ctrl->AppendBitmapColumn("Verify", TrackModel::COL_VERIFY,
                             wxDATAVIEW_CELL_INERT, 64, wxALIGN_CENTER);

    ctrl->AppendBitmapColumn("Delete", TrackModel::COL_DELETE,
                             wxDATAVIEW_CELL_INERT, 64, wxALIGN_CENTER);
}

Downloader *LocalTrackPanel::get_downloader() const { return downloader; }

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
        switch (_type) {
        case LocalTrack::tag_type_t::TITLE: {
            track->set_name(_value);
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

void LocalTrackPanel::OnActivated(wxDataViewEvent &event) {
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
        if (!ITrack::verify(track, downloader)) {
            wxMessageBox("Unable to retrieve meta data from SpotifyAPI");
            return;
        }
        model->RowChanged(rowIndex);
        // make sure that changes are already visible
        row->RebuildSortCache();
    }
}