#if !defined(CIRCLE_PROGRESS_BAR_H)
#define CIRCLE_PROGRESS_BAR_H

#include <cmath>
#include <filesystem>
#include <wx/dcclient.h>
#include <wx/wx.h>

class CircleProgressBar : public wxWindow {
  public:
    CircleProgressBar(wxWindow *_parent, wxWindowID _id = wxID_ANY);

    void SetProgress(int _progress);

  protected:
    void OnPaint(wxPaintEvent &_event);

  private:
    int progress;
    static inline wxBitmap _iconDownload = wxBitmap(
        (std::filesystem::current_path() / "img" / "progress-download.png")
            .c_str(),
        wxBITMAP_TYPE_PNG);
    static inline wxBitmap _iconCheck = wxBitmap(
        (std::filesystem::current_path() / "img" / "check-circle.png").c_str(),
        wxBITMAP_TYPE_PNG);
    static inline wxBitmap _iconXmark = wxBitmap(
        (std::filesystem::current_path() / "img" / "circle-xmark.png").c_str(),
        wxBITMAP_TYPE_PNG);
    DECLARE_EVENT_TABLE()
};

#endif // CIRCLE_PROGRESS_BAR_H
