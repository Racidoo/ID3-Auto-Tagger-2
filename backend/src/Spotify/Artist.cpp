#include "Spotify/Artist.h"

namespace Spotify {

Artist::Artist(const std::string &_id, const std::string &_name,
               State _state, const std::string &_imageURL)
    : MediaEntityBase(_id, _name, _state, _imageURL) {}

// void Artist::loadAdditionalData(std::weak_ptr<IMediaService> _service) {
//     if (state == MetadataState::Full)
//         return;

//     _service.lock()->load(*this);
//     state = MetadataState::Full;
// }

} // namespace Spotify