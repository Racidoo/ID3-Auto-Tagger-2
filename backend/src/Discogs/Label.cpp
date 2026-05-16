#include "Discogs/Label.h"

namespace Discogs {

Label::Label(int _id, const std::string &_name, State _state,
             const std::string &_imageUrl)
    : MediaEntityBase(std::to_string(_id), _name, _state, _imageUrl) {}

} // namespace Discogs
