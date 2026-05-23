#include "Interfaces/MediaEntityBase.h"
#include "Services/IMediaService.hpp"

MediaEntityBase::MediaEntityBase(const std::string &_id, State _state,
                                 IMediaService *_mediaService)
    : id(_id),  state(_state), mediaService(_mediaService) {}

const std::string &MediaEntityBase::get_id() const { return id; }
// const std::string &MediaEntityBase::get_name() const { return name; }
MediaEntityBase::State MediaEntityBase::get_state() const { return state; }

// void MediaEntityBase::set_name(const std::string &_name) { name = _name; }
void MediaEntityBase::set_state(State _state) { state = _state; }

void MediaEntityBase::ensureLoaded() {
    if (!mediaService || state == State::Full)
        return;

    if (mediaService->load(shared_from_this())) {
        state = State::Full;
    }
}
IMediaService::MediaSourceId MediaEntityBase::get_mediaSourceId() const {
    assert(mediaService && "No valid reference to mediaService!");
    return mediaService->get_id();
}