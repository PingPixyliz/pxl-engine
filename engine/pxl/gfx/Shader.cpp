#include <pxl/gfx/Shader.hpp>

namespace pxl::gfx
{
    wgpu::ShaderModule CreateShaderModule(const wgpu::Device& device, std::string_view wgsl, std::string_view label)
    {
        wgpu::ShaderSourceWGSL source{};
        source.code = wgsl;

        wgpu::ShaderModuleDescriptor descriptor{};
        descriptor.nextInChain = &source;
        descriptor.label = label;
        return device.CreateShaderModule(&descriptor);
    }
}
