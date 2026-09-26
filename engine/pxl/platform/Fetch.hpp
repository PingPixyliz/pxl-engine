#pragma once

#include <cstddef>
#include <functional>
#include <span>
#include <string>

namespace pxl::platform
{
    using FetchCallback = std::function<void(bool success, std::span<const std::byte> data)>;

    void FetchFile(const std::string& url, FetchCallback callback);
}
