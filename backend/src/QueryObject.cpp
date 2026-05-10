#include "../include/QueryObject.h"
#include "../include/Query.h"

QueryObject::QueryObject(const std::string &_id, const std::string &_name,
                         MetadataState _state, const std::string &_imageURL)
    : id(_id), name(_name), state(_state), imageURL(_imageURL) {}

QueryObject::~QueryObject() {}

const std::string &QueryObject::get_id() const { return id; }
const std::string &QueryObject::get_name() const { return name; }
QueryObject::MetadataState QueryObject::get_state() const { return state; }
const std::string &QueryObject::get_imageUrl() const { return imageURL; }
std::vector<std::byte> QueryObject::get_image() const {
    return Query::downloadImage(imageURL);
}

void QueryObject::set_state(MetadataState _state) { state = _state; }