#include "LocalTrack.h"

LocalTrack::LocalTrack(const std::filesystem::path &_path)
    : filepath(_path), tagsLoaded(false), length(0) {}

LocalTrack::~LocalTrack() {}

std::string LocalTrack::get_title() {
    ensureTagsLoaded();
    return title;
}
std::string LocalTrack::get_artist() {
    ensureTagsLoaded();
    return artist;
}
std::string LocalTrack::get_album() {
    ensureTagsLoaded();
    return album;
}
std::string LocalTrack::get_albumArtist() {
    ensureTagsLoaded();
    return albumArtist;
}
std::string LocalTrack::get_copyright() {
    ensureTagsLoaded();
    return copyright;
}
std::string LocalTrack::get_genre() {
    ensureTagsLoaded();
    return genre;
}
std::string LocalTrack::get_year() {
    ensureTagsLoaded();
    return year;
}
std::string LocalTrack::get_label() {
    ensureTagsLoaded();
    return label;
}
std::string LocalTrack::get_trackNumber() {
    ensureTagsLoaded();
    return trackNumber;
}
std::string LocalTrack::get_discNumber() {
    ensureTagsLoaded();
    return discNumber;
}
std::size_t LocalTrack::get_length() {
    ensureTagsLoaded();
    return length;
}

std::vector<std::byte> LocalTrack::get_cover() const {
    TagLib::MPEG::File _track(get_filepath().c_str());

    if (!_track.hasID3v2Tag()) {
        return {};
    }
    // Get ID3v2 tag
    auto *tag = _track.ID3v2Tag();
    if (!tag) {
        std::cerr << "No ID3v2 tag found in: " << _track.name() << std::endl;
        return {};
    }

    // Search for an APIC (Attached Picture) frame
    auto frames = tag->frameListMap()["APIC"];
    if (frames.isEmpty()) {
        std::cerr << "No album cover found in: " << _track.name() << std::endl;
        return {};
    }

    // Extract image data
    auto *apic =
        dynamic_cast<TagLib::ID3v2::AttachedPictureFrame *>(frames.front());
    if (!apic) {
        std::cerr << "Invalid APIC frame in: " << _track.name() << std::endl;
        return {};
    }

    // Get raw image data
    const auto &imageData = apic->picture();
    if (imageData.isEmpty()) {
        std::cerr << "APIC frame contains no image data in: " << std::endl;
        return {};
    }

    std::vector<std::byte> result;
    result.reserve(imageData.size());

    for (auto c : imageData)
        result.push_back(static_cast<std::byte>(c));

    return result;
}

const std::filesystem::path &LocalTrack::get_filepath() const {
    return filepath;
}
std::string LocalTrack::get_filename() const { return filepath.stem(); }

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
}

void LocalTrack::set_filepath(const std::filesystem::path &_filepath) {
    filepath = _filepath;
    tagsLoaded = false;
}

void LocalTrack::ensureTagsLoaded() {
    if (tagsLoaded)
        return;

    if (filepath.empty()) {
        tagsLoaded = true;
        return;
    }

    TagLib::MPEG::File file(filepath.c_str());
    if (!file.isValid() || !file.tag()) {
        tagsLoaded = true;
        return;
    }

    TagLib::Tag *tag = file.tag();
    auto id3Tag = file.ID3v2Tag();
    TagLib::FileRef fr(TagLib::FileName(filepath.c_str()), true,
                       TagLib::AudioProperties::Accurate);

    title = tag->title().to8Bit(true);
    artist = tag->artist().to8Bit(true);
    album = tag->album().to8Bit(true);
    genre = tag->genre().to8Bit(true);
    year = tag->year() == 0 ? "" : std::to_string(tag->year());
    trackNumber = tag->track() == 0 ? "" : std::to_string(tag->track());
    albumArtist = getFrameText(id3Tag, "TPE2");
    label = getFrameText(id3Tag, "TPUB");
    discNumber = getFrameText(id3Tag, "TPOS");
    copyright = getFrameText(id3Tag, "TCOP");
    length = fr.audioProperties() ? fr.audioProperties()->length() : 0;
    tagsLoaded = true;
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
