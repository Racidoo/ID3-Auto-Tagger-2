#include "Interfaces/MediaEntityBase.h"
#include "Query.h"
#include "Services/IMediaService.hpp"

MediaEntityBase::MediaEntityBase(const std::string &_id,
                                 const std::string &_name, State _state,
                                 const std::string &_imageURL,
                                 IMediaService *_mediaService)
    : id(_id), name(_name), state(_state), imageURL(_imageURL),
      mediaService(_mediaService) {}

const std::string &MediaEntityBase::get_id() const { return id; }
const std::string &MediaEntityBase::get_name() const { return name; }
MediaEntityBase::State MediaEntityBase::get_state() const { return state; }
const std::string &MediaEntityBase::get_imageUrl() const { return imageURL; }
const std::vector<std::byte> &MediaEntityBase::get_image() {
    if (cachedImage.empty() && !imageURL.empty()) {
        cachedImage = Query::downloadImage(imageURL);
    }
    return cachedImage;
}

void MediaEntityBase::set_name(const std::string &_name) { name = _name; }
void MediaEntityBase::set_state(State _state) { state = _state; }
void MediaEntityBase::set_image(const std::vector<std::byte> &_imageData) {
    cachedImage = _imageData;
}
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