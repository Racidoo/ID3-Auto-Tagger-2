#include "Interfaces/IRemoteImageProvider.h"
#include "Query.h"

IRemoteImageProvider::IRemoteImageProvider(
    const std::string &_imageURL, const std::vector<std::byte> _imageData)
    : imageURL(_imageURL), cachedImage(_imageData) {}

const std::string &IRemoteImageProvider::get_imageUrl() const {
    return imageURL;
}
std::vector<std::byte> IRemoteImageProvider::get_image() {
    if (cachedImage.empty() && !imageURL.empty()) {
        cachedImage = Query::downloadImage(imageURL);
    }
    return cachedImage;
}
void IRemoteImageProvider::set_image(const std::vector<std::byte> &_imageData) {
    cachedImage = _imageData;
}