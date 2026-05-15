#include "Discogs/Label.h"

namespace Discogs {

Label::Label(int _id, const std::string &_name, State _state,
             const std::string &_imageUrl)
    : MediaEntityBase(std::to_string(_id), _name, _state, _imageUrl) {}

// void Label::loadAdditionalData(std::weak_ptr<IMediaService> _service) {
//     if (state == MetadataState::Full)
//         return;

//     _service.lock()->load(*this);
//     state = MetadataState::Full;
// }
} // namespace Discogs
