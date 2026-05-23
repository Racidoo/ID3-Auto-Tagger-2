#pragma once

#include <memory>
#include <sstream>
#include <string>
#include <vector>

class IMediaEntity : public std::enable_shared_from_this<IMediaEntity> {
  public:
    enum class State { None, Preview, Partial, Full };

    virtual ~IMediaEntity() = default;

    virtual const std::string &get_id() const = 0;
    virtual const std::string &get_name() const = 0;
    virtual std::vector<std::byte> get_image() = 0;
    virtual void ensureLoaded() = 0;

    template <typename T>
    static std::string vecToStr(const std::vector<T> &_objects,
                                const std::string &_sep = ", ") {
        using RawT = typename std::remove_reference<decltype(deref(
            std::declval<T>()))>::type;

        static_assert(std::is_base_of<IMediaEntity, RawT>::value,
                      "T must derive from IMediaEntity");

        std::stringstream ss;

        for (size_t i = 0; i < _objects.size(); ++i) {
            if (i != 0)
                ss << _sep;

            ss << deref(_objects[i]).get_name();
        }

        return ss.str();
    }

  private:
    template <typename T> static const T &deref(const T &_obj) { return _obj; }

    template <typename T>
    static const T &deref(const std::shared_ptr<T> &_obj) {
        return *_obj;
    }
};