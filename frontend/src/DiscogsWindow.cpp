#include "DiscogsWindow.h"
#include "Downloader.h"
#include "IconProvider.h"
#include "TrackPanel.h"

DiscogsWindow::DiscogsWindow(wxWindow *_parent, Downloader *_downloader)
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
    trackPanel = new TrackPanel(this, downloader);

    mainSizer->Add(searchSizer, 0, wxEXPAND, 5);
    mainSizer->Add(toolbarSizer, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(trackPanel, 1, wxEXPAND | wxLEFT | wxRIGHT, 5);

    SetSizerAndFit(mainSizer);

    // this->Bind(EVT_MEDIA_LABEL_CLICKED, &DiscogsWindow::startDownload, this);

    // this->Bind(EVT_MEDIA_WINDOW_EXPAND_CLICKED, [this](wxCommandEvent event)
    // {
    //     wxLogDebug("MediaWindow expand clicked. type: " + event.GetString());
    // });
}

void DiscogsWindow::search(const wxString &_searchText) {
    if (!downloader || !downloader->initializeDiscogs()) {
        std::cerr << "Discogs not fully initialized" << std::endl;
        return;
    }
    Discogs::DiscogsAPI::SearchParams params{};
    params.query = _searchText.ToStdString();

    auto results = downloader->get_discogs()->search(params);
    std::vector<std::shared_ptr<TrackInterface>> vec;
    for (auto &&result : results) {
        vec.push_back(TrackInterface::fromDiscogs(
            std::make_shared<Discogs::SearchResult>(result)));
    }

    trackPanel->MergeTracks({});
    trackPanel->MergeTracks(vec);

    // Downloader::SearchResult result = downloader->fetchResource(
    //     _searchText.ToStdString(), {Downloader::SearchCategory::Video});
    // showSearchResults(std::move(result));
}