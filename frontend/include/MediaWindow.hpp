#if !defined(MEDIA_WINDOW_H)
#define MEDIA_WINDOW_H

#include "IconProvider.h"
#include "Spotify/SpotifyObject.h"
#include <unordered_map>
#include <wx/wx.h>

wxDEFINE_EVENT(EVT_MEDIA_WINDOW_EXPAND_CLICKED, wxCommandEvent);

template <typename LabelT> class MediaWindow : public wxPanel {
  public:
    MediaWindow(wxWindow *_parent) : wxPanel(_parent, wxID_ANY) {
        // SetScrollRate(15, 15);
        mainSizer = new wxBoxSizer(wxVERTICAL);
        contentSizer = new wxBoxSizer(wxHORIZONTAL);
        header = new wxStaticText(this, wxID_ANY, wxT("Header"));
        mainSizer->Add(header);
        mainSizer->Add(contentSizer);
        SetSizer(mainSizer);
    }

    void deleteChildren() {
        contentSizer->Clear(true);
        labelMap.clear();
    }

    void appendChildren(LabelT *_label) {
        if (contentSizer->IsEmpty()) {
            auto loadMore = new wxStaticBitmap(
                this, wxID_ANY, wxArtProvider::GetBitmap(wxART_ARROW_RIGHT));
            loadMore->Bind(wxEVT_LEFT_DOWN, [this, _label](wxMouseEvent event) {
                wxCommandEvent notifyEvent(EVT_MEDIA_WINDOW_EXPAND_CLICKED,
                                           GetId());
                notifyEvent.SetEventObject(this);
                notifyEvent.SetString(_label->getSpotifyObject()->get_type());
                wxPostEvent(GetParent(), notifyEvent);
            });
            contentSizer->Add(loadMore, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM,
                              5);
            header->SetLabel(_label->getSpotifyObject()->get_type() + 's');
        }
        contentSizer->Prepend(_label, 0, wxSHRINK, 5);
        const Spotify::SpotifyObject *obj = _label->getSpotifyObject();
        labelMap[obj->get_id()] = _label;
    }

    LabelT *getLabel(const std::string &id) {
        auto it = labelMap.find(id);
        return it != labelMap.end() ? it->second : nullptr;
    }

  private:
    std::unordered_map<std::string, LabelT *> labelMap;
    // Spotify::SpotifyObject SpotifyObject;
    wxBoxSizer *mainSizer;
    wxBoxSizer *contentSizer;
    wxStaticText *header;
};

#endif // MEDIA_WINDOW_H
