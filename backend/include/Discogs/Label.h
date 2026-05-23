#pragma once

#include "Interfaces/IRemoteImageProvider.h"
#include "Interfaces/MediaEntityBase.h"

namespace Discogs {

class Label : public MediaEntityBase {
  public:
    Label(int _id, const std::string &_name, State _state,
          const std::string &_imageUrl, IMediaService *_mediaService);
    ~Label() = default;

    const std::string &get_name() const override;
    std::vector<std::byte> get_image() override;

  private:
    std::string name;
    IRemoteImageProvider imageProvider;
};
} // namespace Discogs
