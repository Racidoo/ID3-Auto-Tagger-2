#pragma once

#include "Interfaces/IMediaEntity.hpp"
#include "Services/IMediaService.hpp"

class MediaEntityBase : public IMediaEntity {
  public:
    explicit MediaEntityBase(const std::string &_id, const std::string &_name,
                             State _state, const std::string &_imageURL,
                             IMediaService *_mediaService);

    const std::string &get_id() const override;
    const std::string &get_name() const override;
    State get_state() const;
    const std::string &get_imageUrl() const;
    const std::vector<std::byte> &get_image() override;

    void set_name(const std::string &_name);
    void set_state(State _state);
    void set_image(const std::vector<std::byte> &_imageData);

    void ensureLoaded() override;
    IMediaService::MediaSourceId get_mediaSourceId() const;

  protected:
    std::string id;
    State state = State::Preview;
    std::string name;
    std::string imageURL;
    std::vector<std::byte> cachedImage;

    IMediaService *mediaService;
};