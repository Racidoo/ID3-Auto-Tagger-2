#include "../include/YouTube/Video.h"

namespace YouTube {

Video::Video(const std::string &_id, const std::string &_name,
             MetadataState _state, const std::string &_channelTitle,
             const std::string &_uploadDate, const std::string &_imageUrl,
             unsigned int _duration, bool _licensed)
    : QueryObject(_id, _name, _state, _imageUrl), channelTitle(_channelTitle),
      uploadDate(_uploadDate), duration(_duration), licensed(_licensed) {}

const std::string &Video::get_channelTitle() const { return channelTitle; }
const std::string &Video::get_uploadDate() const { return uploadDate; }
unsigned int Video::get_duration() const { return duration; }
bool Video::get_licensed() const { return licensed; }

} // namespace YouTube
