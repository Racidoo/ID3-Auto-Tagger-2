#if !defined(CIRCLE_PROGRESS_BAR_H)
#define CIRCLE_PROGRESS_BAR_H

#include <cmath>
#include <filesystem>
#include <wx/dcbuffer.h>
#include <wx/dcclient.h>
#include <wx/wx.h>

#define CIRCLE_PROGRESSBAR_START 0
#define CIRCLE_PROGRESSBAR_FINISH 100
#define CIRCLE_PROGRESSBAR_CANCEL -1

class CircleProgressBar : public wxPanel {
  public:
    CircleProgressBar(wxWindow *_parent, wxWindowID _id = wxID_ANY);

    void SetProgress(int _progress);
    inline int get_progress() const { return progress; }

  protected:
    void OnPaint(wxPaintEvent &_event);

  private:
    int progress;
    DECLARE_EVENT_TABLE()
};

#endif // CIRCLE_PROGRESS_BAR_H
