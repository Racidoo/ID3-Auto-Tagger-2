#pragma once

#include "Interfaces/IMediaEntity.hpp"
#include "Services/IMediaService.hpp"

class MediaEntityBase : public IMediaEntity {
  public:
    explicit MediaEntityBase(const std::string &_id, State _state,
                             IMediaService *_mediaService);

    const std::string &get_id() const override;
    State get_state() const;
    void set_state(State _state);

    void ensureLoaded() override;
    IMediaService::MediaSourceId get_mediaSourceId() const;

  protected:
    std::string id;
    State state = State::Preview;

    IMediaService *mediaService;
};