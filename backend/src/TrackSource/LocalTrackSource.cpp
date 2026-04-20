#include "TrackSource/LocalTrackSource.h"

std::string LocalTrackSource::get_id() const { return track->get_filename(); }
std::string LocalTrackSource::get_title() { return track->get_title(); }
std::string LocalTrackSource::get_artist() { return track->get_artist(); }
std::string LocalTrackSource::get_album() { return track->get_album(); }
std::string LocalTrackSource::get_albumArtist() {
    return track->get_albumArtist();
}
std::string LocalTrackSource::get_copyright() { return track->get_copyright(); }
std::string LocalTrackSource::get_genre() { return track->get_genre(); }
std::string LocalTrackSource::get_year() { return track->get_year(); }
std::string LocalTrackSource::get_label() { return track->get_label(); }
std::string LocalTrackSource::get_trackNumber() {
    return track->get_trackNumber();
}
std::string LocalTrackSource::get_discNumber() {
    return track->get_discNumber();
}
std::size_t LocalTrackSource::get_length() { return track->get_length(); }
std::vector<std::byte> LocalTrackSource::get_cover() {
    return track->get_cover();
}

void LocalTrackSource::set_title(const std::string &_title) {
    track->set_title(_title);
}
void LocalTrackSource::set_artist(const std::string &_artist) {
    track->set_artist(_artist);
}
void LocalTrackSource::set_album(const std::string &_album) {
    track->set_album(_album);
}
void LocalTrackSource::set_albumArtist(const std::string &_albumArtist) {
    track->set_albumArtist(_albumArtist);
}
void LocalTrackSource::set_copyright(const std::string &_copyright) {
    track->set_copyright(_copyright);
}
void LocalTrackSource::set_genre(const std::string &_genre) {
    track->set_genre(_genre);
}
void LocalTrackSource::set_year(const std::string &_year) {
    track->set_year(_year);
}
void LocalTrackSource::set_label(const std::string &_label) {
    track->set_label(_label);
}
void LocalTrackSource::set_trackNumber(const std::string &_track) {
    track->set_trackNumber(_track);
}
void LocalTrackSource::set_discNumber(const std::string &_disc) {
    track->set_discNumber(_disc);
}
void LocalTrackSource::set_cover(const std::vector<std::byte> &_imageData) {
    track->set_cover(_imageData);
}

std::shared_ptr<LocalTrack> LocalTrackSource::get_track() const {
    return track;
}
