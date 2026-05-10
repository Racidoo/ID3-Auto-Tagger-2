#pragma once

#include <string>

#include "QueryObject.h"

namespace Discogs {

class Label : public QueryObject {
  public:
    Label(int _id, const std::string &_name, MetadataState _state,
          const std::string &_imageUrl);
    ~Label() = default;

};

} // namespace Discogs
