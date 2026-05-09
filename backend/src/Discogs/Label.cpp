#include "Discogs/Label.h"

namespace Discogs {

Label::Label(int _id, const std::string &_name, const std::string &_imageUrl)
    : QueryObject(std::to_string(_id), _name, "label", _imageUrl) {}

} // namespace Discogs
