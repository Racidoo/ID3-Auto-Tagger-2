#if !defined(TRACKLABEL_H)
#define TRACKLABEL_H

#include <memory>
#include <taglib/fileref.h>
#include <taglib/mpegfile.h>
#include <wx/wx.h>

#include "TrackInterface.h"

namespace Spotify {
class Track;
}

class CircleProgressBar;
class ScrollText;

wxDECLARE_EVENT(EVT_TRACK_DOWNLOAD, wxCommandEvent);
wxDECLARE_EVENT(EVT_TRACK_VERIFY, wxCommandEvent);
wxDECLARE_EVENT(EVT_TRACK_DELETE, wxCommandEvent);
wxDECLARE_EVENT(EVT_TRACKLABEL_CLICKED, wxCommandEvent);

class TrackLabel : public wxPanel {
  private:
    bool isActive;

    std::shared_ptr<TrackInterface> data;

    CircleProgressBar *progressBar;
    wxStaticBitmap *coverBitmap;
    ScrollText *titleText;
    ScrollText *artistText;
    ScrollText *albumText;
    ScrollText *genreText;
    wxStaticText *lengthText;
    wxFlexGridSizer *rootSizer;
    wxBitmapButton *actionVerify;
    wxBitmapButton *actionDelete;

    void Create();

    void onClick(wxMouseEvent &_event);
    void onDownloadButtonClick(wxMouseEvent &_event);

  public:
    TrackLabel(wxWindow *_parent, std::shared_ptr<TrackInterface> _data);
    ~TrackLabel();

    // Progress, Cover, Title + Artist, Album, Genre, Length
    static constexpr inline int columnWidths[] = {32, 32, 64, 64,
                                                  64, 64, 64, 64};

    std::shared_ptr<TrackInterface> get_data() const;

    CircleProgressBar *get_ProgressBar() const { return progressBar; }

    void Update();
    void Update(std::shared_ptr<TrackInterface> _data);

    wxSize DoGetBestSize() const override;
};

#endif // TRACKLABEL_H
