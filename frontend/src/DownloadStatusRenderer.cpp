#include "DownloadStatusRenderer.h"
#include "IconProvider.h"

namespace {
constexpr int DOWNLOAD_FAILED = -1;
constexpr int DOWNLOAD_FINISHED = 100;
} // namespace

DownloadStatusRenderer::DownloadStatusRenderer()
    : wxDataViewCustomRenderer("long", wxDATAVIEW_CELL_INERT,
                               wxDVR_DEFAULT_ALIGNMENT) {}

bool DownloadStatusRenderer::SetValue(const wxVariant &value) {
    long raw = value.GetLong();

    m_status.progress = static_cast<int>(raw);

    if (raw == DOWNLOAD_FAILED) {
        m_status.state = DownloadState::Failed;
    } else if (raw == DOWNLOAD_FINISHED) {
        m_status.state = DownloadState::Verified;
    } else if (raw > 0) {
        m_status.state = DownloadState::Downloading;
    } else {
        m_status.state = DownloadState::NotDownloaded;
    }

    return true;
}

bool DownloadStatusRenderer::GetValue(wxVariant &value) const {
    value = static_cast<long>(m_status.progress);
    return true;
}

wxSize DownloadStatusRenderer::GetSize() const { return wxSize(48, 48); }

bool DownloadStatusRenderer::Render(wxRect rect, wxDC *dc,
                                    int WXUNUSED(state)) {
    dc->SetBackgroundMode(wxTRANSPARENT);

    int width = rect.GetWidth();
    int height = rect.GetHeight();

    int baseSize = std::min(width, height);

    int border = std::max(1, baseSize / 12);

    int radius = (baseSize / 2) - border;

    wxPoint center(rect.x + width / 2, rect.y + height / 2);

    wxBitmap iconBitmap;

    switch (m_status.state) {
    case DownloadState::NotDownloaded: {
        iconBitmap = wxArtProvider::GetBitmap(wxART_CIRCLE_DOWNLOAD);
        break;
    }

    case DownloadState::Verified: {
        iconBitmap = wxArtProvider::GetBitmap(wxART_CIRCLE_CHECKMARK);
        break;
    }

    case DownloadState::Failed: {
        iconBitmap = wxArtProvider::GetBitmap(wxART_CIRCLE_XMARK);
        break;
    }

    default:
        break;
    }

    if (iconBitmap.IsOk()) {
        int iconW = iconBitmap.GetWidth();
        int iconH = iconBitmap.GetHeight();

        int maxSize = baseSize - 4;

        if (iconW > maxSize || iconH > maxSize) {
            wxImage scaled = iconBitmap.ConvertToImage();

            scaled.Rescale(maxSize, maxSize, wxIMAGE_QUALITY_HIGH);

            iconBitmap = wxBitmap(scaled);

            iconW = iconBitmap.GetWidth();
            iconH = iconBitmap.GetHeight();
        }

        wxPoint iconPos(center.x - iconW / 2, center.y - iconH / 2);

        dc->DrawBitmap(iconBitmap, iconPos, true);

        return true;
    }

    int diameter = radius * 2;

    int x = center.x - radius;
    int y = center.y - radius;

    dc->SetBrush(*wxTRANSPARENT_BRUSH);

    wxPen pen(*wxBLACK, border);

    dc->SetPen(pen);

    double startAngle = 90.0;

    double sweepAngle = 360.0 * (m_status.progress / 100.0);

    dc->DrawEllipticArc(x, y, diameter, diameter, startAngle,
                        startAngle + sweepAngle);

    wxString text = wxString::Format("%d%%", m_status.progress);

    wxFont font = dc->GetFont();

    font.SetPointSize(std::max(6, baseSize / 5));

    dc->SetFont(font);

    wxSize textSize = dc->GetTextExtent(text);

    wxPoint textPos(center.x - textSize.GetWidth() / 2,
                    center.y - textSize.GetHeight() / 2);

    dc->DrawText(text, textPos);

    return true;
}
