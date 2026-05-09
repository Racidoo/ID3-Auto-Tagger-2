#pragma once

#include <string>

#include "QueryObject.h"

namespace Discogs {

class Label : public QueryObject {
  public:
    Label(int _id, const std::string &_name, const std::string &_imageUrl);
    ~Label() = default;

  private:
    /* data */
};

} // namespace Discogs
