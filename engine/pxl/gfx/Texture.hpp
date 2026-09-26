#pragma once

#include <cstdint>
#include <string_view>

#include <webgpu/webgpu_cpp.h>

#include <pxl/asset/Image.hpp>

namespace pxl::gfx
{
    wgpu::Texture CreateTexture2D(const wgpu::Device& device,
        uint32_t width,
        uint32_t height,
        wgpu::TextureFormat format,
        wgpu::TextureUsage usage,
        std::string_view label = "");

    wgpu::Texture CreateTexture(const wgpu::Device& device, const asset::Image& image, std::string_view label = "");
}
