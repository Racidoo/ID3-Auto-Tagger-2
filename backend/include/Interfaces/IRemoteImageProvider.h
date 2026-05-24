#pragma once

#include <string>

#include "Interfaces/IImageProvider.hpp"

class IRemoteImageProvider : public IImageProvider {
  public:
    explicit IRemoteImageProvider(
        const std::string &_imageURL,
        const std::optional<std::vector<std::byte>> _imageData = {});

    std::optional<std::vector<std::byte>> get_image() override;
    const std::string &get_imageUrl() const;
    void
    set_image(const std::optional<std::vector<std::byte>> &_imageData) override;

  private:
    std::string imageURL;
    std::optional<std::vector<std::byte>> cachedImage;
};