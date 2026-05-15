#pragma once

#include <sstream>
#include <string>
#include <vector>

class IMediaEntity {
  public:
    enum class State { None, Preview, Partial, Full };

    virtual ~IMediaEntity() = default;

    virtual const std::string &get_id() const = 0;
    virtual const std::string &get_name() const = 0;
    virtual const std::vector<std::byte> &get_image() = 0;
    virtual void ensureLoaded(class IMediaService &_service) = 0;

  public:
    template <typename T>
    static std::string vecToStr(const std::vector<T> &_object,
                                const std::string &_sep = ", ") {
        static_assert(std::is_base_of<IMediaEntity, T>::value,
                      "T must derive from IMediaEntity");

        std::stringstream ss;
        for (size_t i = 0; i < _object.size(); ++i) {
            if (i != 0)
                ss << _sep;
            ss << _object[i].get_name();
        }
        return ss.str();
    }
};