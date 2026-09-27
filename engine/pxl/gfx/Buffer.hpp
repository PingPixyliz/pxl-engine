#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>

#include <webgpu/webgpu_cpp.h>

namespace pxl::gfx
{
    wgpu::Buffer CreateBuffer(const wgpu::Device& device,
        wgpu::BufferUsage usage,
        std::span<const std::byte> data,
        std::string_view label = "");

    wgpu::Buffer CreateBuffer(const wgpu::Device& device,
        wgpu::BufferUsage usage,
        uint64_t size,
        std::string_view label = "");
}
