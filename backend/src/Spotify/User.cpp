#include "Spotify/User.h"

namespace Spotify {
User::User(const std::string &_id, const std::string &_name,
          State _state, const std::string &_imageURL)
    : MediaEntityBase(_id, _name, _state, _imageURL) {}

// void User::loadAdditionalData(std::weak_ptr<IMediaService> _service) {
//     if (state == MetadataState::Full)
//         return;

//     _service.lock()->load(*this);
//     state = MetadataState::Full;
// }

} // namespace Spotify
