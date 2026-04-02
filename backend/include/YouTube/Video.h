#if !defined(VIDEO_H)
#define VIDEO_H

#include <cmath>
#include <string>
#include <vector>

#include "QueryObject.h"

namespace YouTube {
class Video : public QueryObject {
  private:
    std::string channelTitle;
    std::string uploadDate;
    unsigned int duration;
    bool licensed;
    unsigned int views;

  public:
    Video(const std::string &_id, const std::string &_name,
          const std::string &_channelTitle, const std::string &_uploadDate,
          const std::string &_imageUrl, unsigned int _duration,
          bool _licensed);
    ~Video();

    inline std::string get_channelTitle() const { return channelTitle; }
    inline std::string get_uploadDate() const { return uploadDate; }
    inline unsigned int get_duration() const { return duration; }
    inline bool get_licensed() const { return licensed; }
};

} // namespace YouTube
#endif // VIDEO_H
