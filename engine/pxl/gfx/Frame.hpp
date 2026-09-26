#pragma once

#include <cstdint>

#include <webgpu/webgpu_cpp.h>

namespace pxl::gfx
{
    struct Frame
    {
            wgpu::CommandEncoder encoder;
            wgpu::TextureView target;
            wgpu::TextureView depth;
            wgpu::TextureFormat format = wgpu::TextureFormat::Undefined;
            uint32_t width = 0;
            uint32_t height = 0;
    };
}
