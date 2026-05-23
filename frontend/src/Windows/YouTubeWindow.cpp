#include "Windows/YouTubeWindow.h"
#include "Core/IconProvider.h"
#include "Interfaces/IVideo.h"

YouTubeWindow::YouTubeWindow(wxWindow *_parent) : wxScrolledWindow(_parent) {
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

    videoWindow = new MediaWindow<MediaLabel>(this);

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
    // if (!downloader || !downloader->initializeYouTube()) {
    //     std::cerr << "Downloader/YouTube not fully initialized" << std::endl;
    //     return;
    // }
    // ISearchResult result = downloader->fetchResource(
    //     _searchText.ToStdString(), {ISearchResult::SearchCategory::Video});
    // showSearchResults(std::move(result));
}

void YouTubeWindow::showSearchResults(ISearchResult result) {
    videoWindow->deleteChildren();
    for (auto &video : result.videos)
        videoWindow->appendChildren(new MediaLabel(videoWindow, video, {}));

    FitInside();
    Layout();
}

void YouTubeWindow::startDownload(wxCommandEvent &_event) {
    // if (!downloader) {
    //     std::cerr << "Downloader not fully initialized" << std::endl;
    //     return;
    // }

    // auto *label = dynamic_cast<MediaLabel *>(_event.GetEventObject());
    // if (!label)
    //     return;

    // auto source = label->get_source();

    // if (auto video = std::dynamic_pointer_cast<IVideo>(source)) {
    //     downloader->downloadResource({video}, [](int progress) {
    //         wxLogDebug(wxString(std::to_string(progress)));
    //         // chosenVideoLabel->get_ProgressBar()->SetProgress(progress);
    //     });
    // }
}