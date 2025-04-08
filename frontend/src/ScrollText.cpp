#include "../include/ScrollText.h"

wxBEGIN_EVENT_TABLE(ScrollText, wxPanel) EVT_PAINT(ScrollText::onPaint)
    EVT_ENTER_WINDOW(ScrollText::onEnter) EVT_LEAVE_WINDOW(ScrollText::onLeave)
        EVT_TIMER(wxID_ANY, ScrollText::onTimer) wxEND_EVENT_TABLE()

            ScrollText::ScrollText(wxWindow *parent, wxWindowID _id,
                                   const wxString &_text, const wxPoint &_pos,
                                   const wxSize &_size)
    : wxPanel(parent, _id, _pos, _size, wxFULL_REPAINT_ON_RESIZE | wxBG_STYLE_TRANSPARENT),
      fullText(_text), timer(this),
      font(wxFontInfo(10).Family(wxFONTFAMILY_DEFAULT)) {

    // this->SetBackgroundStyle(wxBG_STYLE_TRANSPARENT); // Needed for custom drawing
    this->SetFont(font);

    // Measure text width
    wxClientDC dc(this);
    dc.SetFont(font);
    dc.GetTextExtent(fullText, &textWidth, nullptr);
}

void ScrollText::onPaint(wxPaintEvent &) {
    wxPaintDC dc(this);
    dc.SetFont(GetFont());

    int textHeight;
    dc.GetTextExtent(fullText, nullptr, &textHeight);

    dc.Clear();

    dc.SetClippingRegion(0, 0, GetClientSize().GetWidth(),
                         GetClientSize().GetHeight());
    dc.DrawText(fullText, -scrollOffset,
                (GetClientSize().GetHeight() - textHeight) / 2);
}

void ScrollText::onEnter(wxMouseEvent &) {
    wxClientDC dc(this);
    dc.SetFont(GetFont());

    int textWidth, textHeight;
    dc.GetTextExtent(fullText, &textWidth, &textHeight);

    int availableWidth = GetClientSize().GetWidth();

    if (textWidth > availableWidth) {
        scrolling = true;
        scrollOffset = 0;
        scrollDirection = 1;
        pauseCounter = pauseTicks;
        timer.Start(50); // Adjust for smoothness
    }
}

void ScrollText::onLeave(wxMouseEvent &) {
    scrolling = false;
    scrollOffset = 0;
    timer.Stop();
    Refresh();
}

void ScrollText::onTimer(wxTimerEvent &) {
    if (!scrolling)
        return;

    wxClientDC dc(this);
    dc.SetFont(GetFont());

    int textWidth, textHeight;
    dc.GetTextExtent(fullText, &textWidth, &textHeight);

    int availableWidth = GetClientSize().GetWidth();

    if (pauseCounter > 0) {
        pauseCounter--;
        return;
    }

    scrollOffset += scrollDirection * 3; // Adjust speed

    if (scrollOffset > (textWidth - availableWidth)) {
        scrollOffset = textWidth - availableWidth;
        scrollDirection = -1;
        pauseCounter = pauseTicks;
    } else if (scrollOffset < 0) {
        scrollOffset = 0;
        scrollDirection = 1;
        pauseCounter = pauseTicks;
    }

    Refresh(); // Trigger paint
}

wxSize ScrollText::DoGetBestSize() const {
    wxWindowDC dc(
        const_cast<ScrollText *>(this)); // wxDC requires non-const window
    dc.SetFont(GetFont());

    int width = 0, height = 0;
    dc.GetTextExtent(fullText, &width, &height);

    // Add some padding maybe?
    return wxSize(width + 4, height + 4);
}