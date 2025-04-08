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
wxDECLARE_EVENT(EVT_TRACKLABEL_CLICKED, wxCommandEvent);

class TrackLabel : public wxPanel {
  private:
    // TagLib::MPEG::File track;
    bool m_isActive = false;
    std::unique_ptr<Spotify::Track> spotifyTrack;
    std::unique_ptr<TagLib::MPEG::File> localTrack;
    CircleProgressBar *progressBar;

    void create(const wxBitmap &_albumCover, const wxString &_title,
                const wxString &_artist, const wxString &_album,
                const wxString &_genre, int _length);
    void onClick(wxMouseEvent &event);
    void onDownloadButtonClick(wxMouseEvent &event);

  public:
    TrackLabel(wxWindow *_parent, const wxString &_songPath);
    TrackLabel(wxWindow *_parent, const Spotify::Track &_track);
    ~TrackLabel();

    // Progress, Cover, Title + Artist, Album, Genre, Length
    static const inline int columnWidths[] = {64, 64, 200, 200, 150, 50};

    Spotify::Track *get_spotifyTrack() const { return spotifyTrack.get(); }
    TagLib::MPEG::File *get_localTrack() const { return localTrack.get(); }
    CircleProgressBar *get_ProgressBar() const { return progressBar; }

    static wxBitmap loadImageFromURL(const wxString &_url, const wxSize &_size);
    static wxBitmap loadImageFromTag(TagLib::MPEG::File *_track,
                                     const wxSize &_size);

    wxSize DoGetBestSize() const override;
};

#endif // TRACKLABEL_H
