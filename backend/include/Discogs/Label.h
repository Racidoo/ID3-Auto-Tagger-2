#pragma once

#include "Interfaces/IRemoteImageProvider.h"
#include "Interfaces/MediaEntityBase.h"

namespace Discogs {

class Label : public MediaEntityBase {
  public:
    Label(int _id, const std::optional<std::string> &_name, State _state,
          const std::string &_imageUrl, IMediaService *_mediaService);
    ~Label() = default;

    std::optional<std::string> get_name() const override;
    std::optional<std::vector<std::byte>> get_image() override;

  private:
    std::optional<std::string> name;
    IRemoteImageProvider imageProvider;
};
} // namespace Discogs
