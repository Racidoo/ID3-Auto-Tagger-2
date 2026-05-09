#include "Discogs/Artist.h"

namespace Discogs {

Artist::Artist(int _id, const std::string &_name, const std::string &_imageUrl)
    : QueryObject(std::to_string(_id), _name, "artist", _imageUrl) {}

} // namespace Discogs
