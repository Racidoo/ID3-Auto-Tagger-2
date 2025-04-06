#if !defined(TRACKLABEL_H)
#define TRACKLABEL_H

#include <memory>
// #include <string>
// #include <vector>
// #include <wx-3.0/wx/grid.h>
// #include <wx/image.h>
#include <wx/mstream.h>
// #include <wx-3.0/wx/sizer.h>
// #include <wx-3.0/wx/statbmp.h>
#include <wx/url.h>
// #include <wx-3.0/wx/wfstream.h>
// #include <wx-3.0/wx/wx.h>

#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

#include <taglib/attachedpictureframe.h>
#include <taglib/audioproperties.h>
#include <taglib/fileref.h>
#include <taglib/id3v2tag.h>
#include <taglib/mpegfile.h>
#include <taglib/textidentificationframe.h>

#include "CircleProgressBar.h"
#include "Spotify/Track.h"

wxDECLARE_EVENT(EVT_TRACK_DOWNLOAD, wxCommandEvent);

class TrackLabel : public wxPanel {
  private:
    // TagLib::MPEG::File track;
    bool m_isActive = false;
    std::unique_ptr<Spotify::Track> spotifyTrack;
    std::unique_ptr<TagLib::MPEG::File> localTrack;
    CircleProgressBar *progressBar;
    // std::vector<wxString> &m_activeSongs;

    void create(const wxBitmap &_albumCover, const wxString &_title,
                const wxString &_artist, const wxString &_album,
                const wxString &_genre, int _length);

  public:
    TrackLabel(wxWindow *_parent, const wxString &_songPath);
    TrackLabel(wxWindow *_parent, const Spotify::Track &_track);
    ~TrackLabel();

    Spotify::Track *get_spotifyTrack() const { return spotifyTrack.get(); }
    CircleProgressBar *get_ProgressBar() const { return progressBar; }
    
    wxBitmap loadImageFromURL(const wxString &url);
    wxBitmap loadImageFromTag(TagLib::MPEG::File &track);
    void onClick(wxMouseEvent &event);
    void onDownloadButtonClick(wxMouseEvent &event);
    void onTaskComplete(wxCommandEvent &);

    wxSize DoGetBestSize() const override;
};

#endif // TRACKLABEL_H
