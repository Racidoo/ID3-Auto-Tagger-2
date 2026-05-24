#include "Interfaces/IFileImageProvider.h"

IFileImageProvider::IFileImageProvider(const std::filesystem::path &_path)
    : filepath(_path) {}

void IFileImageProvider::set_filepath(const std::filesystem::path &_filepath) {
    filepath = _filepath;
}

std::optional<std::vector<std::byte>> IFileImageProvider::get_image() {
    if (cachedImage.has_value()) {
        return cachedImage;
    }

    TagLib::MPEG::File _track(filepath.c_str());

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
    cachedImage.emplace(imageData.size());

    std::transform(imageData.begin(), imageData.end(), cachedImage->begin(),
                   [](char c) { return static_cast<std::byte>(c); });
    return cachedImage;
}

void IFileImageProvider::set_image(
    const std::optional<std::vector<std::byte>> &_imageData) {

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

    if (_imageData.value().empty()) {
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
    pictureFrame->setMimeType(detectMime(_imageData.value()));
    pictureFrame->setType(TagLib::ID3v2::AttachedPictureFrame::FrontCover);
    pictureFrame->setPicture(TagLib::ByteVector(
        reinterpret_cast<const char *>(_imageData.value().data()),
        static_cast<unsigned int>(_imageData.value().size())));

    tag->addFrame(pictureFrame);

    // Save the changes
    if (!file.save()) {
        std::cerr << "Failed to save changes!" << std::endl;
        return;
    }
    cachedImage = _imageData;
}