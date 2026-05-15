#include "Discogs/Artist.h"

namespace Discogs {

Artist::Artist(int _id, const std::string &_name, State _state,
               const std::string &_imageUrl)
    : MediaEntityBase(std::to_string(_id), _name, _state, _imageUrl) {}

// void Artist::loadAdditionalData(std::weak_ptr<IMediaService> _service) {
//     if (state == MetadataState::Full)
//         return;

//     _service.lock()->load(*this);
//     state = MetadataState::Full;
// }
} // namespace Discogs
