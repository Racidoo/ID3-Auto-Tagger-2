#include "YouTubeWindow.h"
#include "Downloader.h"
#include "Interfaces/IVideo.h"
#include "VideoLabel.h"

YouTubeWindow::YouTubeWindow(wxWindow *_parent, Downloader *_downloader)
    : wxScrolledWindow(_parent), downloader(_downloader) {
    this->SetScrollRate(15, 15);

    auto toolbarSizer = new wxBoxSizer(wxHORIZONTAL);

    wxTextCtrl *searchBar =
        new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                       wxDefaultSize, wxTE_PROCESS_ENTER);
    searchBar->Bind(wxEVT_TEXT_ENTER, [this, searchBar](wxCommandEvent &event) {
        this->search(searchBar->GetValue());
    });
    wxBitmapButton *searchButton = new wxBitmapButton(
        this, wxID_ANY, wxArtProvider::GetBitmap(wxART_SEARCH),
        wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    searchButton->Bind(wxEVT_BUTTON, [this, searchBar](wxCommandEvent &event) {
        this->search(searchBar->GetValue());
    });
    searchBar->SetFocus();

    auto searchSizer = new wxBoxSizer(wxHORIZONTAL);
    searchSizer->Add(searchBar, 1, wxEXPAND, 5);
    searchSizer->Add(searchButton, 0, wxEXPAND, 5);

    auto mainSizer = new wxBoxSizer(wxVERTICAL);

    videoWindow = new VideoWindow(this);

    mainSizer->Add(searchSizer, 0, wxEXPAND, 5);
    mainSizer->Add(toolbarSizer, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(videoWindow, 1, wxEXPAND | wxLEFT | wxRIGHT, 5);

    SetSizerAndFit(mainSizer);

    this->Bind(EVT_MEDIA_LABEL_CLICKED, &YouTubeWindow::startDownload, this);

    this->Bind(EVT_MEDIA_WINDOW_EXPAND_CLICKED, [this](wxCommandEvent event) {
        wxLogDebug("MediaWindow expand clicked. type: " + event.GetString());
    });
}

YouTubeWindow::~YouTubeWindow() {}

void YouTubeWindow::search(const wxString &_searchText) {
    if (!downloader || !downloader->initializeYouTube()) {
        std::cerr << "Downloader/YouTube not fully initialized" << std::endl;
        return;
    }
    ISearchResult result = downloader->fetchResource(
        _searchText.ToStdString(), {ISearchResult::SearchCategory::Video});
    showSearchResults(std::move(result));
}

void YouTubeWindow::showSearchResults(ISearchResult result) {
    videoWindow->deleteChildren();
    for (auto &video : result.videos)
        videoWindow->appendChildren(new VideoLabel(videoWindow, video));

    FitInside();
    Layout();
}

void YouTubeWindow::startDownload(wxCommandEvent &_event) {
    if (!downloader) {
        std::cerr << "Downloader not fully initialized" << std::endl;
        return;
    }

    auto *label = dynamic_cast<VideoLabel *>(_event.GetEventObject());
    if (!label)
        return;

    std::string id = label->get_source()->get_id();
    std::string type = "<removed for now, fix later>";

    std::cout << "Clicked object ID: " << id << ", Type: " << type << std::endl;
    // track search for artist not supported
    if (type == "artist") {
        return;
    }

    VideoLabel *chosenVideoLabel = videoWindow->getLabel(id);
    if (!chosenVideoLabel) {
        wxLogDebug("invalid videoLabel");
        return;
    }

    auto chosenVideo = chosenVideoLabel->get_source();
    if (!chosenVideo) {
        wxLogDebug("invalid video");
        return;
    }
    downloader->downloadResource(
        {chosenVideo}, [chosenVideoLabel](int progress) {
            wxLogDebug(wxString(std::to_string(progress)));
            // chosenVideoLabel->get_ProgressBar()->SetProgress(progress);
        });
}