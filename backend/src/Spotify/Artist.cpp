#include "../include/Spotify/Artist.h"

namespace Spotify {

Artist::Artist(const std::string &_id, const std::string &_name,
               MetadataState _state, const std::string &_imageURL)
    : QueryObject(_id, _name, _state, _imageURL) {}

} // namespace Spotify