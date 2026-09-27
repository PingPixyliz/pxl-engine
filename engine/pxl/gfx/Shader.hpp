#pragma once

#include <string_view>

#include <webgpu/webgpu_cpp.h>

namespace pxl::gfx
{
    wgpu::ShaderModule CreateShaderModule(const wgpu::Device& device,
        std::string_view wgsl,
        std::string_view label = "");
}
