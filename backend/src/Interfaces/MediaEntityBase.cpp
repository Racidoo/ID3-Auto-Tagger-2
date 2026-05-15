#include "Interfaces/MediaEntityBase.h"
#include "IMediaService.hpp"
#include "Query.h"

MediaEntityBase::MediaEntityBase(const std::string &_id,
                                 const std::string &_name, State _state,
                                 const std::string &_imageURL)
    : id(_id), name(_name), state(_state), imageURL(_imageURL) {}

const std::string &MediaEntityBase::get_id() const { return id; }
const std::string &MediaEntityBase::get_name() const { return name; }
MediaEntityBase::State MediaEntityBase::get_state() const { return state; }
const std::string &MediaEntityBase::get_imageUrl() const { return imageURL; }
const std::vector<std::byte> &MediaEntityBase::get_image() {
    if (chachedImage.empty()) {
        chachedImage = Query::downloadImage(imageURL);
    }
    return chachedImage;
}

void MediaEntityBase::set_name(const std::string &_name) { name = _name; }
void MediaEntityBase::set_state(State _state) { state = _state; }
void MediaEntityBase::set_image(const std::vector<std::byte> &_imageData) {
    chachedImage = _imageData;
}
void MediaEntityBase::ensureLoaded(IMediaService &_service) {
    if (state == State::Full)
        return;

    _service.load(*this);
    state = State::Full;
}