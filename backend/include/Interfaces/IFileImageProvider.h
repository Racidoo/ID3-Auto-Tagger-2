#pragma once

#include <filesystem>
#include <string>
#include <taglib/apetag.h>
#include <taglib/attachedpictureframe.h>
#include <taglib/fileref.h>
#include <taglib/id3v2frame.h>
#include <taglib/id3v2header.h>
#include <taglib/id3v2tag.h>
#include <taglib/mpegfile.h>
#include <taglib/textidentificationframe.h>

#include "Interfaces/IImageProvider.hpp"

class IFileImageProvider : public IImageProvider {
  public:
    explicit IFileImageProvider(const std::filesystem::path &_path);

    std::optional<std::vector<std::byte>> get_image() override;

    void set_filepath(const std::filesystem::path &_filepath);
    void
    set_image(const std::optional<std::vector<std::byte>> &_imageData) override;

  private:
    std::filesystem::path filepath;
    std::optional<std::vector<std::byte>> cachedImage;
};