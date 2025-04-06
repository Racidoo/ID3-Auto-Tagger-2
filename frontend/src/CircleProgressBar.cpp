#include "../include/CircleProgressBar.h"

BEGIN_EVENT_TABLE(CircleProgressBar, wxWindow)
EVT_PAINT(CircleProgressBar::OnPaint)
END_EVENT_TABLE()

/**
 * @brief Construct a new Circle Progress Bar:: Circle Progress Bar object
 *
 * @param _parent
 * @param _id
 */
CircleProgressBar::CircleProgressBar(wxWindow *_parent, wxWindowID _id)
    : wxWindow(_parent, _id), progress(0) {
    SetSize(64, 64); // Set size for the progress bar
    SetBackgroundStyle(wxBG_STYLE_PAINT);
}

/**
 * @brief Set progress percentage (0 to 100)
 * @param _progress
 */
void CircleProgressBar::SetProgress(int _progress) {
    if (_progress > 100)
        _progress = 100;
    else if (_progress < -1)
        _progress = -1;
    progress = _progress;
    Refresh(); // Trigger redraw
}

/**
 * @brief paints the progress based of internal progress value. Sets special
 * icons on progress values of -1, 0 & 100
 * @param _event
 */
void CircleProgressBar::OnPaint(wxPaintEvent &_event) {

    wxPaintDC dc(this);
    PrepareDC(dc);

    wxSize size = GetSize();

    int width = size.GetWidth() / 2;
    int height = size.GetHeight() / 2;
    int baseSize = std::min(width, height);
    int border = std::max(1, baseSize / 20);

    int radius = std::min(width, height) / 2;
    wxPoint center(width, height);

    wxBitmap iconBitmap;

    if (progress == 0) {
        if (!_iconDownload.IsOk()) {
            wxLogWarning(wxT("Failed to load ") +
                         std::string(std::filesystem::current_path() / "img" /
                                     "progress-download.png"));
        } else
            iconBitmap = _iconDownload;
    } else if (progress == 100) {
        if (!_iconCheck.IsOk()) {
            wxLogWarning(wxT("Failed to load ") +
                         std::string(std::filesystem::current_path() / "img" /
                                     "check-circle.png"));
        } else
            iconBitmap = _iconCheck;
    } else if (progress == -1) {
        if (!_iconXmark.IsOk()) {
            wxLogWarning(wxT("Failed to load ") +
                         std::string(std::filesystem::current_path() / "img" /
                                     "circle-xmark.png"));
        } else
            iconBitmap = _iconXmark;
    }

    if (iconBitmap.IsOk()) {
        int iconW = iconBitmap.GetWidth();
        int iconH = iconBitmap.GetHeight();

        // Scale icon if it's too large
        int maxSize = std::min(width, height);
        if (iconW > maxSize || iconH > maxSize) {
            wxImage scaledImage = iconBitmap.ConvertToImage();
            scaledImage.Rescale(maxSize, maxSize, wxIMAGE_QUALITY_HIGH);
            wxBitmap scaledBitmap(scaledImage);
            iconBitmap = scaledBitmap;
            iconW = iconBitmap.GetWidth();
            iconH = iconBitmap.GetHeight();
        }

        wxPoint iconPos(center.x - iconW / 2, center.y - iconH / 2);
        dc.DrawBitmap(iconBitmap, iconPos, true);
        return;
    }

    int x = center.x - radius;
    int y = center.y - radius;
    int diameter = 2 * radius;

    // Draw progress arc
    wxPen progressPen(*wxBLACK, border);
    dc.SetPen(progressPen);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    // 0% = 90°, 100% = 450° → Sweep clockwise
    double startAngle = 90;                         // start at top
    double sweepAngle = 360.0 * (progress / 100.0); // proportional

    dc.DrawEllipticArc(x, y, diameter, diameter, startAngle,
                       startAngle + sweepAngle);

    // Draw percentage text
    wxString percentText = wxString::Format("%d%%", progress);
    int fontSize =
        std::max(6, baseSize / 5); // Adjust divisor to control font scale

    wxFont font = GetFont();
    font.SetPointSize(fontSize); // Set new point size
    dc.SetFont(font);
    wxSize textSize = dc.GetTextExtent(percentText);
    wxPoint textPos(center.x - textSize.GetWidth() / 2,
                    center.y - textSize.GetHeight() / 2);
    dc.DrawText(percentText, textPos);
}
