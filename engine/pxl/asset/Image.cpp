#include <pxl/asset/Image.hpp>

#include <cstddef>
#include <cstdio>
#include <cstring>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#define STBI_ONLY_PNG
#define STBI_ONLY_JPEG
#include <stb_image.h>

namespace pxl::asset
{
    Image DecodeImage(std::span<const std::byte> file)
    {
        int width = 0;
        int height = 0;
        int channels = 0;

        stbi_uc* pixels = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(file.data()),
            static_cast<int>(file.size()), &width, &height, &channels, static_cast<int>(Image::k_BytesPerPixel));
        if (!pixels)
        {
            std::fprintf(stderr, "[pxl] DecodeImage: %s\n", stbi_failure_reason());
            return {};
        }

        Image image{};
        image.width = static_cast<uint32_t>(width);
        image.height = static_cast<uint32_t>(height);
        image.pixels.resize(static_cast<size_t>(width) * static_cast<size_t>(height) * Image::k_BytesPerPixel);
        std::memcpy(image.pixels.data(), pixels, image.pixels.size());
        stbi_image_free(pixels);
        return image;
    }
}
