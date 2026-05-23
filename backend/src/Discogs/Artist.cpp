#include "Discogs/Artist.h"

namespace Discogs {

Artist::Artist(int _id, const std::string &_name, State _state,
               const std::string &_imageUrl, IMediaService *_mediaService)
    : IArtist(std::to_string(_id), _name, _state, _imageUrl, _mediaService) {}

} // namespace Discogs
