#if !defined(TRACK_EDIT_WINDOW_H)
#define TRACK_EDIT_WINDOW_H

#include <functional> // for std::hash
#include <string>     // for std::string
#include <vector>
#include <wx/wx.h>

#include "MediaLabel.h"

class LabeledTextCtrl;
class TrackInterface;

class TrackEditWindow : public wxPanel {
  private:
    wxStaticBitmap *albumCover;
    LabeledTextCtrl *titleText;
    LabeledTextCtrl *artistText;
    LabeledTextCtrl *albumText;
    LabeledTextCtrl *albumArtistsText;
    LabeledTextCtrl *genreText;
    LabeledTextCtrl *labelText;
    LabeledTextCtrl *yearText;
    LabeledTextCtrl *trackNumberText;
    LabeledTextCtrl *discNumberText;
    LabeledTextCtrl *copyrightText;
    LabeledTextCtrl *filenameText;

    std::vector<std::shared_ptr<TrackInterface>> selected;

  public:
    TrackEditWindow(wxWindow *_parent, wxWindowID _winid = wxID_ANY,
                    const wxPoint &_pos = wxDefaultPosition,
                    const wxSize &_size = wxDefaultSize);
    ~TrackEditWindow();

    void
    set_selected(const std::vector<std::shared_ptr<TrackInterface>> &_selected);

    void Update();

    static std::size_t bitmapHash(const wxBitmap &bmp);
    static bool bitmapsEqual(const wxBitmap &a, const wxBitmap &b);

  private:
    void OnShow(wxShowEvent &_event);

    /**
     * @brief Returns the common attribute across all files.
     *        If any file is missing the attribute or values differ,
     *        an empty string is returned.
     *
     * @tparam Container Container of TagLib::MPEG::File*
     * @tparam Getter Callable: (TagLib::ID3v2::Tag*) -> std::string
     */
    template <typename Getter> wxString getCommonAttribute(Getter getter) {
        if (selected.empty())
            return {};

        std::string commonValue;
        bool initialized = false;

        for (auto active : selected) {
            if (!active)
                continue;

            std::string value = getter(active); // call lambda

            // Treat empty string as "not present"
            if (value.empty())
                return {};

            if (!initialized) {
                commonValue = std::move(value);
                initialized = true;
            } else if (value != commonValue) {
                return {}; // mismatch found
            }
        }

        return wxString::FromUTF8(commonValue);
    }

    /**
     * @brief Get the Common Bitmap object
     *
     * @tparam Container
     * @tparam Getter
     * @param files
     * @param _size
     * @param getAttribute
     * @return wxBitmap
     */
    template <typename Getter>
    wxBitmap getCommonBitmap(const wxSize &_size, Getter getAttribute) {
        if (selected.empty())
            return wxBitmap(_size);

        // Access the first file, and check it has the expected tag
        auto first = *selected.begin();

        if (!first) {
            return wxBitmap(_size);
        }

        // Get the common value from the first file
        wxBitmap commonBitmap =
            MediaLabel::loadImage(getAttribute(first), _size);

        for (auto active : selected) {
            if (!active)
                continue;
            if (!bitmapsEqual(
                    MediaLabel::loadImage(getAttribute(active), _size),
                    commonBitmap))
                return wxBitmap(_size);
        }
        return commonBitmap;
    }
};

#endif // TRACK_EDIT_WINDOW_H
