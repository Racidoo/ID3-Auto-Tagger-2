#include "../include/YouTube/Video.h"

namespace YouTube {

Video::Video(const std::string &_id, const std::string &_name,
             const std::string &_channelTitle, const std::string &_uploadDate,
             const std::string &_imageUrl, unsigned int _duration,
             bool _licensed)
    : QueryObject(_id, _name, "video", _imageUrl), channelTitle(_channelTitle),
      uploadDate(_uploadDate), duration(_duration), licensed(_licensed) {}

Video::~Video() {}

} // namespace YouTube
