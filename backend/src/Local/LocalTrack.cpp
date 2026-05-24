#include "Local/LocalTrack.h"

LocalTrack::LocalTrack(const std::filesystem::path &_path,
                       IMediaService *_mediaService)
    : ITrackMutable(_path.stem(), IMediaEntity::State::None, _mediaService),
      filepath(_path), imageProvider(_path), length(0) {
    ensurePreviewTagsLoaded();
}

LocalTrack::~LocalTrack() {}

std::optional<std::string> LocalTrack::get_name() const { return name; }
std::optional<std::string> LocalTrack::get_artist() const { return artist; }
std::optional<std::string> LocalTrack::get_albumName() const { return album; }
std::optional<std::string> LocalTrack::get_genre() const { return genre; }
std::optional<size_t> LocalTrack::get_year() const { return year; }
std::optional<size_t> LocalTrack::get_trackNumber() const {
    return trackNumber;
}
std::size_t LocalTrack::get_length() const { return length; }
std::optional<std::string> LocalTrack::get_albumArtist() const {
    return albumArtist;
}
std::optional<std::string> LocalTrack::get_copyright() const {
    return copyright;
}
std::optional<std::string> LocalTrack::get_label() const { return label; }
std::optional<size_t> LocalTrack::get_discNumber() const { return discNumber; }
std::string LocalTrack::get_bitrate() const { return std::to_string(bitrate); }
std::string LocalTrack::get_sampleRate() const {
    return std::to_string(sampleRate);
}
std::string LocalTrack::get_channels() const {
    return std::to_string(channels);
}

std::optional<std::vector<std::byte>> LocalTrack::get_image() {
    return imageProvider.get_image();
}

bool LocalTrack::is_verified() const { return verified; }

const std::filesystem::path &LocalTrack::get_filepath() const {
    return filepath;
}

void LocalTrack::set_name(const std::optional<std::string> &_name) {
    if (!_name.has_value()) {
        return;
    }
    TagLib::MPEG::File file(filepath.c_str());

    if (file.tag()) {
        file.tag()->setTitle(
            TagLib::String(_name.value_or(""), TagLib::String::UTF8));
        file.save();
    }

    name = _name;
}

void LocalTrack::set_artist(const std::optional<std::string> &_artist) {
    if (!_artist.has_value()) {
        return;
    }
    TagLib::MPEG::File file(filepath.c_str());

    if (file.tag()) {
        file.tag()->setArtist(
            TagLib::String(_artist.value(), TagLib::String::UTF8));
        file.save();
    }

    artist = _artist;
}

void LocalTrack::set_albumName(const std::optional<std::string> &_album) {
    if (!_album.has_value()) {
        return;
    }
    TagLib::MPEG::File file(filepath.c_str());

    if (file.tag()) {
        file.tag()->setAlbum(
            TagLib::String(_album.value(), TagLib::String::UTF8));
        file.save();
    }

    album = _album;
}

void LocalTrack::set_albumArtist(
    const std::optional<std::string> &_albumArtist) {
    if (!_albumArtist.has_value()) {
        return;
    }
    setTagValue(filepath, "TPE2", _albumArtist.value());
    albumArtist = _albumArtist;
}

void LocalTrack::set_copyright(const std::optional<std::string> &_copyright) {
    if (!_copyright.has_value()) {
        return;
    }
    setTagValue(filepath, "TCOP", _copyright.value());
    copyright = _copyright;
}

void LocalTrack::set_genre(const std::optional<std::string> &_genre) {
    if (!_genre.has_value()) {
        return;
    }
    TagLib::MPEG::File file(filepath.c_str());

    if (file.tag()) {
        file.tag()->setGenre(
            TagLib::String(_genre.value(), TagLib::String::UTF8));
        file.save();
    }

    genre = _genre;
}

void LocalTrack::set_year(std::optional<size_t> _year) {
    TagLib::MPEG::File file(filepath.c_str());

    if (file.tag()) {
        if (!_year.has_value()) {
            file.tag()->setYear(0);
        } else {
            file.tag()->setYear(_year.value());
            file.save();
        }
        year = _year;
    }
}

void LocalTrack::set_label(const std::optional<std::string> &_label) {
    if (!_label.has_value()) {
        return;
    }
    setTagValue(filepath, "TPUB", _label.value());
    label = _label;
}

void LocalTrack::set_trackNumber(std::optional<size_t> _trackNumber) {
    TagLib::MPEG::File file(filepath.c_str());

    if (file.tag()) {
        if (!_trackNumber.has_value()) {
            file.tag()->setTrack(0);
        } else {
            file.tag()->setTrack(_trackNumber.value());
            file.save();
        }
        trackNumber = _trackNumber;
    }
}

void LocalTrack::set_discNumber(std::optional<size_t> _discNumber) {
    TagLib::MPEG::File file(filepath.c_str());
    if (!_discNumber.has_value()) {
        setTagValue(filepath, "TPOS", "");
    } else {
        setTagValue(filepath, "TPOS", std::to_string(_discNumber.value()));
    }
    discNumber = _discNumber;
}

void LocalTrack::set_image(
    const std::optional<std::vector<std::byte>> &_imageData) {
    if (!_imageData.has_value()) {
        return;
    }
    imageProvider.set_image(_imageData.value());
}

void LocalTrack::set_verified(bool _verified) { verified = _verified; }

void LocalTrack::set_filepath(const std::filesystem::path &_filepath) {
    filepath = _filepath;
    imageProvider.set_filepath(filepath);
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

    name = tag->title().to8Bit(true);
    artist = tag->artist().to8Bit(true);
    album = tag->album().to8Bit(true);
    genre = tag->genre().to8Bit(true);
    year = tag->year() == 0 ? std::optional<std::size_t>{} : tag->year();
    trackNumber =
        tag->track() == 0 ? std::optional<std::size_t>{} : tag->track();

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
    discNumber = (getFrameText(id3Tag, "TPOS").empty()
                      ? std::optional<std::size_t>{}
                      : std::stoul(getFrameText(id3Tag, "TPOS")));
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
