#if !defined(YOUTUBE_WINDOW_H)
#define YOUTUBE_WINDOW_H

#include <unordered_map>
#include <vector>
// #include <wx/mdi.h>
#include <wx/tglbtn.h>
#include <wx/wx.h>

#include "Downloader.h"
#include "Interfaces/ISearchResult.hpp"
#include "MediaWindow.hpp"

class VideoLabel;

using VideoWindow = MediaWindow<VideoLabel>;

class YouTubeWindow : public wxScrolledWindow {
  private:
    Downloader *downloader;

    VideoWindow *videoWindow;

  public:
    YouTubeWindow(wxWindow *_parent, Downloader *_downloader);
    ~YouTubeWindow();

    void search(const wxString &_searchText);
    void showSearchResults(ISearchResult result);
    void loadAdditionalSearchResults(const wxString &_type);
    void startDownload(wxCommandEvent &_event);
    void verifyTags(wxCommandEvent &_event);
};

#endif // YOUTUBE_WINDOW_H
