#include "Spotify/Playlist.h"

namespace Spotify {

Playlist::Playlist(const std::string &_id, const std::string &_name,
                   State _state, const std::string &_imageURL,
                   const User &_creator)
    : MediaEntityBase(_id, _name, _state, _imageURL), creator(_creator) {}

// void Playlist::loadAdditionalData(std::weak_ptr<IMediaService> _service) {
//     if (state == MetadataState::Full)
//         return;

//     _service.lock()->load(*this);
//     state = MetadataState::Full;
// }
} // namespace Spotify
