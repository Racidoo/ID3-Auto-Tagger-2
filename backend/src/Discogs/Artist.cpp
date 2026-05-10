#include "Discogs/Artist.h"

namespace Discogs {

Artist::Artist(int _id, const std::string &_name, MetadataState _state,
               const std::string &_imageUrl)
    : QueryObject(std::to_string(_id), _name, _state, _imageUrl) {}

} // namespace Discogs
