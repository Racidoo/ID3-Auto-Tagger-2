#include "../include/SettingsWindow.h"
#include "../include/Downloader.h"
#include "../include/IconProvider.h"
#include "../include/MultiInputDialog.h"

SettingsWindow::SettingsWindow(wxWindow *_parent, Downloader *_downloader,
                               wxWindowID _winid, const wxPoint &_pos,
                               const wxSize &_size)
    : wxScrolledWindow(_parent, _winid, _pos, _size), downloader(_downloader) {

    SetBackgroundColour(*wxLIGHT_GREY);
    auto mainSizer(new wxBoxSizer(wxVERTICAL));

    auto googleAuthSizer = new wxBoxSizer(wxHORIZONTAL);
    auto googleAuthIcon = new wxStaticBitmap(
        this, wxID_ANY,
        wxBitmap(downloader->initializeYouTube()
                     ? wxArtProvider::GetBitmap(wxART_CIRCLE_CHECKMARK)
                     : wxArtProvider::GetBitmap(wxART_CIRCLE_XMARK)));
    auto googleAuthTokenDialog =
        new MultiInputDialog(this, wxT("Google API Authentication"),
                             {{"authToken", wxT("Google Auth Token"), true}});
    auto googleAuthTokenButton =
        new wxButton(this, wxID_ANY, wxT("Google API Authentication"));
    googleAuthTokenButton->Bind(wxEVT_BUTTON, [googleAuthTokenDialog,
                                               googleAuthIcon,
                                               this](wxCommandEvent &event) {
        if (googleAuthTokenDialog->ShowModal() == wxID_OK) {
            googleAuthIcon->SetBitmap(
                downloader->initializeYouTube(googleAuthTokenDialog->GetValues()
                                                  .at("authToken")
                                                  .ToStdString())
                    ? wxArtProvider::GetBitmap(wxART_CIRCLE_CHECKMARK)
                    : wxArtProvider::GetBitmap(wxART_CIRCLE_XMARK));
        }
    });

    auto spotifyCredentialsSizer = new wxBoxSizer(wxHORIZONTAL);
    auto spotifyCredentialsIcon = new wxStaticBitmap(
        this, wxID_ANY,
        wxBitmap(downloader->initializeSpotify()
                     ? wxArtProvider::GetBitmap(wxART_CIRCLE_CHECKMARK)
                     : wxArtProvider::GetBitmap(wxART_CIRCLE_XMARK)));
    auto spotifyCredentialsDialog = new MultiInputDialog(
        this, wxT("Spotify API Authentication"),
        {{"clientId", wxT("Spotify Client ID"), false},
         {"clientSecret", wxT("Spotify Client Secret"), true}});
    auto spotifyCredentialsButton =
        new wxButton(this, wxID_ANY, wxT("Spotify API Authentication"));
    spotifyCredentialsButton->Bind(wxEVT_BUTTON, [spotifyCredentialsDialog,
                                                  spotifyCredentialsIcon,
                                                  this](wxCommandEvent &event) {
        if (spotifyCredentialsDialog->ShowModal() == wxID_OK) {
            spotifyCredentialsIcon->SetBitmap(
                downloader->initializeSpotify(
                    spotifyCredentialsDialog->GetValues()
                        .at("clientId")
                        .ToStdString(),
                    spotifyCredentialsDialog->GetValues()
                        .at("clientSecret")
                        .ToStdString())
                    ? wxArtProvider::GetBitmap(wxART_CIRCLE_CHECKMARK)
                    : wxArtProvider::GetBitmap(wxART_CIRCLE_XMARK));
        }
    });

    auto musicPathSizer = new wxBoxSizer(wxHORIZONTAL);
    auto musicPathText = new wxStaticText(
        this, wxID_ANY, wxString(downloader->get_trackPath().string()));
    auto musicPathButton =
        new wxButton(this, wxID_ANY, wxT("Select save folder"));
    musicPathButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent &event) {
        wxDirDialog dlg(this, "Select folder", "",
                        wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

        if (dlg.ShowModal() == wxID_OK) {
            auto path = dlg.GetPath();
            wxLogDebug("Set music folder to " + path);
            downloader->set_trackPath(path.ToStdString());
        }
    });
    // auto soundcloudCredentialsSizer = new wxBoxSizer(wxHORIZONTAL);
    // auto soundcloudCredentialsIcon = new wxStaticBitmap(
    //     this, wxID_ANY,
    //     wxBitmap(downloader->initializeSoundcloud()
    //                  ? wxArtProvider::GetBitmap(wxART_CIRCLE_CHECKMARK)
    //                  : wxArtProvider::GetBitmap(wxART_CIRCLE_XMARK)));
    // auto soundcloudCredentialsDialog = new MultiInputDialog(
    //     this, wxT("Soundcloud API Authentication"),
    //     {{"clientId", wxT("Soundcloud Client ID"), false},
    //      {"clientSecret", wxT("Soundcloud Client Secret"), true}});
    // auto soundcloudCredentialsButton =
    //     new wxButton(this, wxID_ANY, wxT("Soundcloud API Authentication"));
    // soundcloudCredentialsButton->Bind(
    //     wxEVT_BUTTON, [soundcloudCredentialsDialog, soundcloudCredentialsIcon,
    //                    this](wxCommandEvent &event) {
    //         if (soundcloudCredentialsDialog->ShowModal() == wxID_OK) {
    //             soundcloudCredentialsIcon->SetBitmap(
    //                 downloader->initializeSoundcloud(
    //                     soundcloudCredentialsDialog->GetValues()
    //                         .at("clientId")
    //                         .ToStdString(),
    //                     soundcloudCredentialsDialog->GetValues()
    //                         .at("clientSecret")
    //                         .ToStdString())
    //                     ? wxArtProvider::GetBitmap(wxART_CIRCLE_CHECKMARK)
    //                     : wxArtProvider::GetBitmap(wxART_CIRCLE_XMARK));
    //         }
    //     });

    googleAuthSizer->Add(googleAuthTokenButton, 0, wxALL, 5);
    googleAuthSizer->Add(googleAuthIcon, 0, wxALL, 5);
    spotifyCredentialsSizer->Add(spotifyCredentialsButton, 0, wxALL, 5);
    spotifyCredentialsSizer->Add(spotifyCredentialsIcon, 0, wxALL, 5);
    musicPathSizer->Add(musicPathButton, 0, wxALL, 5);
    musicPathSizer->Add(musicPathText, 0, wxALL, 5);

    mainSizer->Add(googleAuthSizer, 0, wxALL, 5);
    mainSizer->Add(spotifyCredentialsSizer, 0, wxALL, 5);
    mainSizer->Add(musicPathSizer, 0, wxALL, 5);
    // soundcloudCredentialsSizer->Add(soundcloudCredentialsButton, 0, wxALL, 5);
    // soundcloudCredentialsSizer->Add(soundcloudCredentialsIcon, 0, wxALL, 5);

    mainSizer->Add(googleAuthSizer, 0, wxALL, 5);
    mainSizer->Add(spotifyCredentialsSizer, 0, wxALL, 5);
    // mainSizer->Add(soundcloudCredentialsSizer, 0, wxALL, 5);

    this->SetSizerAndFit(mainSizer);
}