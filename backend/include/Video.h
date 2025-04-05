#if !defined(VIDEO_H)
#define VIDEO_H

#include <cmath>
#include <string>
#include <vector>


class Video {
  private:
    std::string title;
    std::string id;
    std::string channelTitle;

 

  public:
    Video(const std::string &_title, const std::string &_id,
          const std::string &_channelTitle);
    ~Video();

    inline std::string get_title() const { return title; }
    inline std::string get_id() const { return id; }
    inline std::string get_channelTitle() const { return channelTitle; }

    
};

#endif // VIDEO_H
