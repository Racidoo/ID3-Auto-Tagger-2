#pragma once

#include "Interfaces/IArtist.h"
#include "Interfaces/IRemoteImageProvider.h"

namespace Discogs {

class Artist : public IArtist {
  public:
    Artist(int _id, const std::optional<std::string> &_name, State _state,
           const std::string &_imageUrl, IMediaService *_mediaService);
    ~Artist() = default;

    std::optional<std::string> get_name() const override;
    std::optional<std::vector<std::byte>> get_image() override;

  private:
    std::optional<std::string> name;
    IRemoteImageProvider imageProvider;
};

} // namespace Discogs
