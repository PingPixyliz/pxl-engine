#pragma once

#include <functional>
#include <string>
#include <vector>

#include <pxl/asset/Image.hpp>

namespace pxl::asset
{
    using ImageCallback = std::function<void(Image image)>;
    void LoadImage(const std::string& url, ImageCallback callback);

    using ImagesCallback = std::function<void(std::vector<Image> images)>;
    void LoadImages(const std::vector<std::string>& urls, ImagesCallback callback);
}
