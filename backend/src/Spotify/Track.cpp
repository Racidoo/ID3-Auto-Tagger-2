#include "../include/Spotify/Track.h"

namespace Spotify {

Track::Track(const std::string &_id, const std::string &_name,
             const unsigned int _discNumber, const unsigned long _durationMs,
             const bool _explicitContent, const unsigned int _trackNumber,
             const Album &_album, const std::vector<Artist> &_artists)
    : SpotifyObject(_id, _name, "track", ""), discNumber(_discNumber),
      durationMs(_durationMs), explicitContent(_explicitContent),
      trackNumber(_trackNumber), album(_album), artists(_artists),
      downloaded(false) {}

Track::~Track() {}

bool Track::writeID3V2Tags(TagLib::MPEG::File _file) const {

    bool suc(true);

    _file.tag()->setTitle(this->name);
    _file.tag()->setAlbum(this->album.get_name());
    _file.tag()->setArtist(this->get_stringArtists());
    _file.tag()->setTrack(this->trackNumber);
    _file.tag()->setYear(
        std::stoi(this->get_album().get_releaseDate().substr(0, 4)));
    // file.tag()->setGenre();
    // file.tag()->setProperties();

    suc = suc && setTagValue(_file, "TPE2", this->album.get_artists());
    suc = suc && setTagValue(_file, "TPUB", this->album.get_label());
    suc = suc && setTagValue(_file, "TCOP", this->album.get_copyright());
    suc = suc && setTagValue(_file, "TPOS", std::to_string(this->discNumber));
    suc = suc && _file.save();
    return suc;
}

bool Track::setAlbumCover(const std::string &_mp3Path,
                          const std::vector<char> &_imageData) const {
    return setAPICTag(_mp3Path, _imageData);
}

bool Track::setAlbumCover(const std::string &_mp3Path,
                          const std::string &_imagePath) const {
    std::ifstream imageFile(_imagePath, std::ios::binary);
    if (!imageFile) {
        std::cerr << "Could not open image file: " << _imagePath << std::endl;
        return false;
    }

    std::vector<char> imageData((std::istreambuf_iterator<char>(imageFile)),
                                std::istreambuf_iterator<char>());
    imageFile.close();
    return setAPICTag(_mp3Path, imageData);
}

bool Track::setAPICTag(const std::string &_mp3Path,
                       const std::vector<char> &_imageData) const {

    if (_imageData.empty()) {
        std::cerr << "Image is empty" << std::endl;
        return false;
    }

    TagLib::MPEG::File file(_mp3Path.c_str());

    if (!file.isValid()) {
        std::cerr << "Invalid MP3 file: " << _mp3Path << std::endl;
        return false;
    }

    // Get the ID3v2 tag (create one if it doesn’t exist)
    TagLib::ID3v2::Tag *tag = file.ID3v2Tag(true);
    if (!tag) {
        std::cerr << "Failed to get ID3v2 tag!" << std::endl;
        return false;
    }

    // Remove existing album art
    auto frames = tag->frameListMap()["APIC"];
    for (auto *frame : frames) {
        tag->removeFrame(frame, true);
    }

    // Create new APIC frame
    auto *pictureFrame = new TagLib::ID3v2::AttachedPictureFrame();
    pictureFrame->setMimeType("image/jpeg");
    pictureFrame->setType(TagLib::ID3v2::AttachedPictureFrame::FrontCover);
    pictureFrame->setPicture(
        TagLib::ByteVector(_imageData.data(), _imageData.size()));

    tag->addFrame(pictureFrame);

    // Save the changes
    if (!file.save()) {
        std::cerr << "Failed to save changes!" << std::endl;
        return false;
    }
    // std::cout << "Album cover added successfully!" << std::endl;
    return true;
}

void Track::printID3V2Tags(TagLib::MPEG::File _file) {
    // std::cout << "title: " _file.tag()->title() << "\n"
}

bool Track::setTagValue(TagLib::MPEG::File &file, const char *frameID,
                        const std::string &value) {
    if (!file.isValid() || !file.ID3v2Tag()) {
        return false;
    }
    TagLib::ID3v2::Tag *id3v2tag = file.ID3v2Tag();
    TagLib::ID3v2::FrameList frameList = id3v2tag->frameList(frameID);

    if (!frameList.isEmpty()) {
        frameList.front()->setText(value);
    } else {
        TagLib::ID3v2::TextIdentificationFrame *newFrame =
            new TagLib::ID3v2::TextIdentificationFrame(frameID,
                                                       TagLib::String::UTF8);
        newFrame->setText(value);
        id3v2tag->addFrame(newFrame);
    }
    return true;
}

} // namespace Spotify