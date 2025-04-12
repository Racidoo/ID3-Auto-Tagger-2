#if !defined(TRACK_EDIT_WINDOW_H)
#define TRACK_EDIT_WINDOW_H

#include <functional> // for std::hash
#include <optional>
#include <string> // for std::string
#include <taglib/mpegfile.h>
#include <wx/wx.h>

#include "LabeledTextCtrl.h"

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

  public:
    TrackEditWindow(wxWindow *_parent, wxWindowID _winid = wxID_ANY,
                    const wxPoint &_pos = wxDefaultPosition,
                    const wxSize &_size = wxDefaultSize);
    ~TrackEditWindow();

    void show(const std::vector<TagLib::MPEG::File *> &_tracks);

    static std::size_t bitmapHash(const wxBitmap &bmp);
    static bool bitmapsEqual(const wxBitmap &a, const wxBitmap &b);

  private:
    /**
     * @brief This function iterates over the container and compares the
     * attribute extracted by getAttribute() from each file's ID3v2 tag. If all
     * extracted attributes are equal, it returns that value; otherwise, it
     * returns an empty string.
     *
     * @tparam Container
     * @tparam Getter
     * @param files
     * @param getAttribute
     * @return std::string
     */
    template <typename Container, typename Getter>
    std::string getCommonAttribute(const Container &files,
                                   Getter getAttribute) {
        if (files.empty())
            return "";

        // Access the first file, and check it has the expected tag
        auto *firstFile = files.front();
        if (!firstFile->hasID3v2Tag())
            return "";

        // Get the common value from the first file
        std::string commonValue = getAttribute(firstFile->ID3v2Tag());

        for (auto *file : files) {
            if (!file->hasID3v2Tag())
                return "";
            if (getAttribute(file->ID3v2Tag()) != commonValue)
                return "";
        }
        return commonValue;
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
    template <typename Container, typename Getter>
    wxBitmap getCommonBitmap(const Container &files, const wxSize &_size,
                             Getter getAttribute) {
        if (files.empty())
            return wxBitmap(_size);

        // Access the first file, and check it has the expected tag
        auto *firstFile = files.front();
        if (!firstFile->hasID3v2Tag())
            return wxBitmap(_size);

        // Get the common value from the first file
        wxBitmap commonBitmap = getAttribute(firstFile);

        for (auto *file : files) {
            if (!bitmapsEqual(getAttribute(file), commonBitmap))
                return wxBitmap(_size);
        }
        return commonBitmap;
    }
};

#endif // TRACK_EDIT_WINDOW_H
