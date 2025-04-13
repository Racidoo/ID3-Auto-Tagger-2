#if !defined(MEDIA_WINDOW_H)
#define MEDIA_WINDOW_H

#include <unordered_map>
#include <wx/wx.h>

template <typename LabelT, typename ModelT>
class MediaWindow : public wxPanel {
  public:
    MediaWindow(wxWindow *_parent) : wxPanel(_parent, wxID_ANY) {
        // SetScrollRate(15, 15);
        SetSizer(new wxBoxSizer(wxHORIZONTAL));
    }

    void deleteChildren() {
        GetSizer()->Clear(true);
        labelMap.clear();
    }

    void appendChildren(LabelT *_label) {
        GetSizer()->Add(_label, 0, wxSHRINK, 5);
        const Spotify::SpotifyObject &obj = _label->getSpotifyObject();
        labelMap[obj.get_id()] = _label;
    }

    LabelT *getLabel(const std::string &id) {
        auto it = labelMap.find(id);
        return it != labelMap.end() ? it->second : nullptr;
    }

  private:
    std::unordered_map<std::string, LabelT *> labelMap;
};

#endif // MEDIA_WINDOW_H
