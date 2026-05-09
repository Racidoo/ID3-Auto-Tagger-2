#pragma once

#include <string>

#include "QueryObject.h"

namespace Discogs {

class Artist : public QueryObject {
  public:
    Artist(int _id, const std::string &_name, const std::string &_imageUrl);
    ~Artist() = default;

  private:
    /* data */
};

} // namespace Discogs
