#if !defined(TRACK_EDIT_WINDOW_H)
#define TRACK_EDIT_WINDOW_H

#include <functional> // for std::hash
#include <string>     // for std::string
#include <unordered_set>
#include <wx/wx.h>

#include "MediaLabel.h"
#include "TrackInterface.h"
#include "TrackLabel.h"

class LabeledTextCtrl;
// class TrackLabel;

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

    std::unordered_set<TrackLabel *> activeSongs;

  public:
    TrackEditWindow(wxWindow *_parent, wxWindowID _winid = wxID_ANY,
                    const wxPoint &_pos = wxDefaultPosition,
                    const wxSize &_size = wxDefaultSize);
    ~TrackEditWindow();

    inline std::unordered_set<TrackLabel *> &get_activeSongs() {
        return activeSongs;
    }
    inline const std::unordered_set<TrackLabel *> &get_activeSongs() const {
        return activeSongs;
    }

    void show();

    static std::size_t bitmapHash(const wxBitmap &bmp);
    static bool bitmapsEqual(const wxBitmap &a, const wxBitmap &b);

  private:
    /**
     * @brief Returns the common attribute across all files.
     *        If any file is missing the attribute or values differ,
     *        an empty string is returned.
     *
     * @tparam Container Container of TagLib::MPEG::File*
     * @tparam Getter Callable: (TagLib::ID3v2::Tag*) -> std::string
     */
    template <typename Getter> wxString getCommonAttribute(Getter getter) {
        if (activeSongs.empty())
            return {};

        std::string commonValue;
        bool initialized = false;

        for (auto activeSong : get_activeSongs()) {
            if (!activeSong || !activeSong->get_data())
                continue;

            std::string value = getter(activeSong->get_data()); // call lambda

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
        if (activeSongs.empty())
            return wxBitmap(_size);

        // Access the first file, and check it has the expected tag
        auto firstLabel = *get_activeSongs().begin();

        if (!firstLabel || !firstLabel->get_data()) {
            return wxBitmap(_size);
        }

        // Get the common value from the first file
        wxBitmap commonBitmap =
            MediaLabel::loadImage(getAttribute(firstLabel->get_data()), _size);

        for (auto activeSong : get_activeSongs()) {
            if (!activeSong || !activeSong->get_data())
                continue;
            if (!bitmapsEqual(MediaLabel::loadImage(
                                  getAttribute(activeSong->get_data()), _size),
                              commonBitmap))
                return wxBitmap(_size);
        }
        return commonBitmap;
    }
};

#endif // TRACK_EDIT_WINDOW_H
