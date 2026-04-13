#include "../include/QueryObject.h"
#include "../include/Query.h"

QueryObject::QueryObject(const std::string &_id, const std::string &_name,
                         const std::string &_type, const std::string &_imageURL)
    : id(_id), name(_name), type(_type), imageURL(_imageURL) {}

QueryObject::~QueryObject() {}

std::vector<std::byte> QueryObject::get_image() const {
    return Query::downloadImage(imageURL);
}