#include "Discogs/Artist.h"

namespace Discogs {

Artist::Artist(int _id, const std::string &_name, State _state,
               const std::string &_imageUrl, IMediaService *_mediaService)
    : IArtist(std::to_string(_id), _state, _mediaService), name(_name),
      imageProvider(_imageUrl) {}

const std::string &Artist::get_name() const { return name; }
std::vector<std::byte> Artist::get_image() { return imageProvider.get_image(); }

} // namespace Discogs
