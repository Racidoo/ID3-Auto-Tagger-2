#pragma once

#include "Interfaces/IArtist.h"
#include "Interfaces/IRemoteImageProvider.h"

namespace Spotify {
class User : public IArtist {
  public:
    User(const std::string &_id, const std::string &_name, State _state,
         const std::string &_imageURL, IMediaService *_mediaService);
    ~User() = default;

    std::optional<std::string> get_name() const override;
    std::optional<std::vector<std::byte>> get_image() override;

  private:
    std::optional<std::string> name;
    IRemoteImageProvider imageProvider;
};

} // namespace Spotify
