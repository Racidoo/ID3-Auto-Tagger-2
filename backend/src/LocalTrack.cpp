#include "LocalTrack.h"

LocalTrack::LocalTrack(const std::filesystem::path &_path)
    : filepath(_path), id(filepath.stem()), state(IMediaEntity::State::None),
      length(0) {
    ensurePreviewTagsLoaded();
}

LocalTrack::~LocalTrack() {}

const std::string &LocalTrack::get_title() const { return title; }
const std::string &LocalTrack::get_artist() const { return artist; }
const std::string &LocalTrack::get_album() const { return album; }
const std::string &LocalTrack::get_genre() const { return genre; }
const std::string &LocalTrack::get_year() const { return year; }
const std::string &LocalTrack::get_trackNumber() const { return trackNumber; }
std::size_t LocalTrack::get_length() const { return length; }
const std::string &LocalTrack::get_albumArtist() {
    ensureFullTagsLoaded();
    return albumArtist;
}
const std::string &LocalTrack::get_copyright() {
    ensureFullTagsLoaded();
    return copyright;
}
const std::string &LocalTrack::get_label() {
    ensureFullTagsLoaded();
    return label;
}
const std::string &LocalTrack::get_discNumber() {
    ensureFullTagsLoaded();
    return discNumber;
}
std::string LocalTrack::get_bitrate() {
    ensureFullTagsLoaded();
    return std::to_string(bitrate);
}
std::string LocalTrack::get_sampleRate() {
    ensureFullTagsLoaded();
    return std::to_string(sampleRate);
}
std::string LocalTrack::get_channels() {
    ensureFullTagsLoaded();
    return std::to_string(channels);
}

IMediaEntity::State LocalTrack::get_state() const { return state; }

const std::vector<std::byte> &LocalTrack::get_cover() {
    if (!cachedImage.empty()) {
        return cachedImage;
    }

    TagLib::MPEG::File _track(get_filepath().c_str());

    if (!_track.hasID3v2Tag()) {
        return cachedImage;
    }
    // Get ID3v2 tag
    auto *tag = _track.ID3v2Tag();
    if (!tag) {
        std::cerr << "No ID3v2 tag found in: " << _track.name() << std::endl;
        return cachedImage;
    }

    // Search for an APIC (Attached Picture) frame
    auto frames = tag->frameListMap()["APIC"];
    if (frames.isEmpty()) {
        std::cerr << "No album cover found in: " << _track.name() << std::endl;
        return cachedImage;
    }

    // Extract image data
    auto *apic =
        dynamic_cast<TagLib::ID3v2::AttachedPictureFrame *>(frames.front());
    if (!apic) {
        std::cerr << "Invalid APIC frame in: " << _track.name() << std::endl;
        return cachedImage;
    }

    // Get raw image data
    const auto &imageData = apic->picture();
    if (imageData.isEmpty()) {
        std::cerr << "APIC frame contains no image data in: " << std::endl;
        return cachedImage;
    }

    cachedImage.reserve(imageData.size());

    for (auto c : imageData)
        cachedImage.push_back(static_cast<std::byte>(c));

    return cachedImage;
}

const std::filesystem::path &LocalTrack::get_filepath() const {
    return filepath;
}
const std::string &LocalTrack::get_id() const { return id; }

void LocalTrack::set_title(const std::string &_title) {
    TagLib::MPEG::File file(filepath.c_str());

    if (file.tag()) {
        file.tag()->setTitle(TagLib::String(_title, TagLib::String::UTF8));
        file.save();
    }

    title = _title;
}

void LocalTrack::set_artist(const std::string &_artist) {
    TagLib::MPEG::File file(filepath.c_str());

    if (file.tag()) {
        file.tag()->setArtist(TagLib::String(_artist, TagLib::String::UTF8));
        file.save();
    }

    artist = _artist;
}

void LocalTrack::set_album(const std::string &_album) {
    TagLib::MPEG::File file(filepath.c_str());

    if (file.tag()) {
        file.tag()->setAlbum(TagLib::String(_album, TagLib::String::UTF8));
        file.save();
    }

    album = _album;
}

void LocalTrack::set_albumArtist(const std::string &_albumArtist) {
    setTagValue(filepath, "TPE2", _albumArtist);
    albumArtist = _albumArtist;
}

void LocalTrack::set_copyright(const std::string &_copyright) {
    setTagValue(filepath, "TCOP", _copyright);
    copyright = _copyright;
}

void LocalTrack::set_genre(const std::string &_genre) {
    TagLib::MPEG::File file(filepath.c_str());

    if (file.tag()) {
        file.tag()->setGenre(TagLib::String(_genre, TagLib::String::UTF8));
        file.save();
    }

    genre = _genre;
}

void LocalTrack::set_year(const std::string &_year) {
    TagLib::MPEG::File file(filepath.c_str());

    if (file.tag()) {
        file.tag()->setYear(std::stoi(_year));
        file.save();
    }

    year = _year;
}

void LocalTrack::set_label(const std::string &_label) {
    setTagValue(filepath, "TPUB", _label);
    label = _label;
}

void LocalTrack::set_trackNumber(const std::string &_trackNumber) {
    TagLib::MPEG::File file(filepath.c_str());

    if (file.tag()) {
        file.tag()->setTrack(std::stoi(_trackNumber));
        file.save();
    }
    trackNumber = _trackNumber;
}

void LocalTrack::set_discNumber(const std::string &_discNumber) {
    TagLib::MPEG::File file(filepath.c_str());
    setTagValue(filepath, "TPOS", _discNumber);
    discNumber = _discNumber;
}

void LocalTrack::set_cover(const std::vector<std::byte> &_imageData) {

    auto detectMime = [](const std::vector<std::byte> &data) -> const char * {
        if (data.size() >= 4) {
            // JPEG
            if ((unsigned char)data[0] == 0xFF &&
                (unsigned char)data[1] == 0xD8)
                return "image/jpeg";

            // PNG
            if ((unsigned char)data[0] == 0x89 &&
                (unsigned char)data[1] == 0x50 &&
                (unsigned char)data[2] == 0x4E &&
                (unsigned char)data[3] == 0x47)
                return "image/png";
        }
        return "image/jpeg"; // fallback
    };

    if (_imageData.empty()) {
        std::cerr << "Image is empty" << std::endl;
        return;
    }

    TagLib::MPEG::File file(filepath.c_str());

    if (!file.isValid()) {
        std::cerr << "Invalid MP3 file: " << filepath << std::endl;
        return;
    }

    // Get the ID3v2 tag (create one if it doesn’t exist)
    TagLib::ID3v2::Tag *tag = file.ID3v2Tag(true);
    if (!tag) {
        std::cerr << "Failed to get ID3v2 tag!" << std::endl;
        return;
    }

    // Remove existing album art
    auto frames = tag->frameListMap()["APIC"];
    for (auto *frame : frames) {
        tag->removeFrame(frame, true);
    }

    // Create new APIC frame
    auto *pictureFrame = new TagLib::ID3v2::AttachedPictureFrame();
    pictureFrame->setMimeType(detectMime(_imageData));
    pictureFrame->setType(TagLib::ID3v2::AttachedPictureFrame::FrontCover);
    pictureFrame->setPicture(
        TagLib::ByteVector(reinterpret_cast<const char *>(_imageData.data()),
                           static_cast<unsigned int>(_imageData.size())));

    tag->addFrame(pictureFrame);

    // Save the changes
    if (!file.save()) {
        std::cerr << "Failed to save changes!" << std::endl;
        return;
    }
    cachedImage = _imageData;
}

void LocalTrack::set_state(IMediaEntity::State _state) { state = _state; }

void LocalTrack::set_filepath(const std::filesystem::path &_filepath) {
    filepath = _filepath;
    id = filepath.stem();
    state = IMediaEntity::State::None;
    ensurePreviewTagsLoaded();
}

void LocalTrack::ensurePreviewTagsLoaded() {
    if (state != IMediaEntity::State::None)
        return;

    if (filepath.empty()) {
        state = IMediaEntity::State::Preview;
        return;
    }

    TagLib::MPEG::File file(filepath.c_str());
    if (!file.isValid() || !file.tag()) {
        state = IMediaEntity::State::Preview;
        return;
    }

    TagLib::Tag *tag = file.tag();
    TagLib::FileRef fr(TagLib::FileName(filepath.c_str()), true,
                       TagLib::AudioProperties::Accurate);

    title = tag->title().to8Bit(true);
    artist = tag->artist().to8Bit(true);
    album = tag->album().to8Bit(true);
    genre = tag->genre().to8Bit(true);
    year = tag->year() == 0 ? "" : std::to_string(tag->year());
    trackNumber = tag->track() == 0 ? "" : std::to_string(tag->track());

    length = fr.audioProperties() ? fr.audioProperties()->length() : 0;
    state = IMediaEntity::State::Preview;
}

void LocalTrack::ensureFullTagsLoaded() {
    if (state == IMediaEntity::State::Full)
        return;

    if (filepath.empty()) {
        state = IMediaEntity::State::Full;
        return;
    }

    TagLib::MPEG::File file(filepath.c_str());
    TagLib::FileRef fr(TagLib::FileName(filepath.c_str()), true,
                       TagLib::AudioProperties::Accurate);
    if (!file.isValid()) {
        state = IMediaEntity::State::Full;
        return;
    }

    auto id3Tag = file.ID3v2Tag();

    albumArtist = getFrameText(id3Tag, "TPE2");
    label = getFrameText(id3Tag, "TPUB");
    discNumber = getFrameText(id3Tag, "TPOS");
    copyright = getFrameText(id3Tag, "TCOP");

    bitrate = file.audioProperties()->bitrate();
    sampleRate = file.audioProperties()->sampleRate();
    channels = file.audioProperties()->channels();
    ensurePreviewTagsLoaded();
    state = IMediaEntity::State::Full;
}

std::string LocalTrack::getFrameText(TagLib::ID3v2::Tag *_tag,
                                     const char *_id) {

    auto toStdString = [](const TagLib::String &_str) {
        return _str.isEmpty() ? std::string{} : _str.to8Bit(true);
    };

    if (!_tag)
        return std::string{};

    const auto &map = _tag->frameListMap();
    auto it = map.find(_id);

    if (it == map.end() || it->second.isEmpty())
        return std::string{};

    auto *frame = it->second.front();
    if (!frame)
        return std::string{};

    return toStdString(frame->toString());
};

bool LocalTrack::setTagValue(const std::filesystem::path &_filepath,
                             const char *frameID, const std::string &value) {
    TagLib::MPEG::File file(_filepath.c_str());
    if (!file.isValid() || !file.ID3v2Tag()) {
        return false;
    }
    TagLib::ID3v2::Tag *id3v2tag = file.ID3v2Tag();
    TagLib::ID3v2::FrameList frameList = id3v2tag->frameList(frameID);

    if (!frameList.isEmpty()) {
        frameList.front()->setText(TagLib::String(value, TagLib::String::UTF8));
    } else {
        TagLib::ID3v2::TextIdentificationFrame *newFrame =
            new TagLib::ID3v2::TextIdentificationFrame(frameID,
                                                       TagLib::String::UTF8);
        newFrame->setText(TagLib::String(value, TagLib::String::UTF8));
        id3v2tag->addFrame(newFrame);
    }
    if (!file.save()) {
        return false;
    }

    return true;
}

bool LocalTrack::renameLocalTrack(const std::string &_filename) {
    std::filesystem::path newPath =
        filepath.parent_path() / (_filename + filepath.extension().string());
    try {
        std::filesystem::rename(filepath, newPath);
    } catch (const std::filesystem::filesystem_error &e) {
        std::cerr << "Rename failed: " << e.what() << std::endl;
        return false;
    }
    filepath = newPath;
    return true;
}

/**
 * @brief only deletes the file on disc. Deleting `this` is the user's
 * responsibility!
 *
 * @return true
 * @return false
 */
bool LocalTrack::deleteLocalTrack() {
    if (!std::filesystem::exists(filepath)) {
        std::cerr << "Could not remove " << filepath
                  << ": File does not exist.\n";
        return false;
    }
    if (!std::filesystem::remove(filepath)) {
        std::cerr << "Could not remove " << filepath
                  << ":Failed to delete the file.\n";
        return false;
    }
    std::cout << "File deleted successfully.\n";
    return true;
}