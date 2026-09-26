#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace pxl::asset
{
    struct Image
    {
            static constexpr uint32_t k_BytesPerPixel = 4;

            uint32_t width = 0;
            uint32_t height = 0;
            std::vector<std::byte> pixels;

            bool IsEmpty() const { return pixels.empty(); }
    };

    Image DecodeImage(std::span<const std::byte> file);
}
