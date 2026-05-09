#include "Discogs/SearchResult.h"

namespace Discogs {

SearchResult::SearchResult(int _id, const std::string &_name,
                           const std::string &_imageUrl, int _masterId,
                           const std::vector<std::string> &_genres,
                           const std::vector<std::string> &_styles,
                           const std::string &_year)
    : QueryObject(std::to_string(_id), _name, "searchResult", _imageUrl),
      masterId(_masterId), year(_year) {

    // Ignore Discogs genres as they are too generic
    // for (auto &&genre : _genres) {
    //     if (!genres.empty())
    //         genres += "\n";
    //     genres += genre;
    // }
    for (auto &&style : _styles) {
        if (!genres.empty())
            genres += "\n";
        genres += style;
    }
}

} // namespace Discogs