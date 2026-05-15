#pragma once

#include <algorithm>
#include <wx/dataview.h>
#include <wx/dcbuffer.h>

enum class DownloadState {
    NotDownloaded,
    Downloading,
    Downloaded,
    Verified,
    Failed
};

struct DownloadStatus {
    int progress = 0;
    DownloadState state = DownloadState::NotDownloaded;
};

class DownloadStatusRenderer : public wxDataViewCustomRenderer {
  public:
    DownloadStatusRenderer();

    bool SetValue(const wxVariant &value) override;
    bool GetValue(wxVariant &value) const override;

    bool Render(wxRect rect, wxDC *dc, int state) override;

    wxSize GetSize() const override;

  private:
    DownloadStatus m_status;
};
