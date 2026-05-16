#include "Interfaces/MediaEntityBase.h"
#include "Query.h"
#include "Services/IMediaService.hpp"

MediaEntityBase::MediaEntityBase(const std::string &_id,
                                 const std::string &_name, State _state,
                                 const std::string &_imageURL)
    : id(_id), name(_name), state(_state), imageURL(_imageURL) {}

const std::string &MediaEntityBase::get_id() const { return id; }
const std::string &MediaEntityBase::get_name() const { return name; }
MediaEntityBase::State MediaEntityBase::get_state() const { return state; }
const std::string &MediaEntityBase::get_imageUrl() const { return imageURL; }
const std::vector<std::byte> &MediaEntityBase::get_image() {
    if (cachedImage.empty()) {
        cachedImage = Query::downloadImage(imageURL);
    }
    return cachedImage;
}

void MediaEntityBase::set_name(const std::string &_name) { name = _name; }
void MediaEntityBase::set_state(State _state) { state = _state; }
void MediaEntityBase::set_image(const std::vector<std::byte> &_imageData) {
    cachedImage = _imageData;
}
void MediaEntityBase::ensureLoaded(IMediaService &_service) {
    if (state == State::Full)
        return;

    _service.load(shared_from_this());
    state = State::Full;
}