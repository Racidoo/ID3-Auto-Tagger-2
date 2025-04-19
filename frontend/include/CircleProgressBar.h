#if !defined(CIRCLE_PROGRESS_BAR_H)
#define CIRCLE_PROGRESS_BAR_H

#include <cmath>
#include <filesystem>
#include <wx/dcclient.h>
#include <wx/wx.h>

#include "IconProvider.h"

class CircleProgressBar : public wxWindow {
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
