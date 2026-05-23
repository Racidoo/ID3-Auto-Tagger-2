#include "Spotify/Artist.h"

namespace Spotify {

Artist::Artist(const std::string &_id, const std::string &_name, State _state,
               const std::string &_imageURL, IMediaService *_mediaService)
    : IArtist(_id, _name, _state, _imageURL, _mediaService) {}

} // namespace Spotify