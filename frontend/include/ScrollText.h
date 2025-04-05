#if !defined(SCROLL_TEXT_H)
#define SCROLL_TEXT_H

#include <vector>
#include <wx/wx.h>

class ScrollText : public wxPanel {
  public:
    ScrollText(wxWindow *parent, wxWindowID _id, const wxString &_text,
               const wxPoint &_pos = wxDefaultPosition,
               const wxSize &_size = wxDefaultSize);

  private:
    wxString fullText;
    int scrollOffset = 0;
    int scrollDirection = 1; // 1 = right to left, -1 = left to right
    bool scrolling = false;
    int pauseCounter = 0;
    const int pauseTicks = 10; // how many timer ticks to pause at each end
    int textWidth = 0;
    wxTimer timer;
    wxFont font;

    void onPaint(wxPaintEvent &evt);
    void onEnter(wxMouseEvent &evt);
    void onLeave(wxMouseEvent &evt);
    void onTimer(wxTimerEvent &evt);

    wxSize DoGetBestSize() const override;

    wxDECLARE_EVENT_TABLE();
};

#endif // SCROLL_TEXT_H
