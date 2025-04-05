#if !defined(TRACKLABEL_H)
#define TRACKLABEL_H

#include <memory>
// #include <string>
#include <vector>
#include <wx-3.0/wx/grid.h>
#include <wx-3.0/wx/image.h>
#include <wx-3.0/wx/mstream.h>
#include <wx-3.0/wx/sizer.h>
#include <wx-3.0/wx/statbmp.h>
#include <wx-3.0/wx/url.h>
#include <wx-3.0/wx/wfstream.h>
#include <wx-3.0/wx/wx.h>

#include <taglib/attachedpictureframe.h>
#include <taglib/fileref.h>
#include <taglib/id3v2tag.h>
#include <taglib/mpegfile.h>
#include <taglib/textidentificationframe.h>

#include "Spotify/Track.h"

wxDECLARE_EVENT(EVT_TRACK_DOWNLOAD, wxCommandEvent);

class TrackLabel : public wxPanel {
  private:
    // TagLib::MPEG::File track;
    bool m_isActive = false;
    std::unique_ptr<Spotify::Track> spotifyTrack;
    // std::vector<wxString> &m_activeSongs;

    void create(const wxBitmap &_albumCover, const wxString &_title,
                const wxString &_artist, const wxString &_album,
                const wxString &_genre, std::vector<wxObject *> elements);

  public:
    TrackLabel(wxWindow *_parent, const wxString &_songPath);
    TrackLabel(wxWindow *_parent, const Spotify::Track &_track);
    ~TrackLabel();

    Spotify::Track *get_spotifyTrack() const { return spotifyTrack.get(); }

    wxBitmap loadImageFromURL(const wxString &url);
    wxBitmap loadImageFromTag(TagLib::MPEG::File &track);
    void onClick(wxMouseEvent &event);
    void onDownloadButtonClick(wxCommandEvent &event);
    void onTaskComplete(wxCommandEvent &);
};

#endif // TRACKLABEL_H
