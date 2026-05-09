#pragma once

#include <string>
#include <vector>

#include "QueryObject.h"

namespace Discogs {

class SearchResult : public QueryObject {
  public:
    SearchResult(int _id, const std::string &_name,
                 const std::string &_imageUrl, int _masterId,
                 const std::vector<std::string> &_genres,
                 const std::vector<std::string> &_styles, const std::string& _year);
    int masterId;
    std::string genres;
    std::string year;
};

} // namespace Discogs
