#pragma once

#include <string>

class IAudioFileInfo {
public:
    virtual std::size_t bitrate() const = 0;
    virtual std::size_t sampleRate() const = 0;
    virtual std::size_t channels() const = 0;
};