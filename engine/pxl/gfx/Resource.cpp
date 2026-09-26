#include <pxl/gfx/Resource.hpp>

#include <cstring>

namespace pxl::gfx
{
    namespace
    {
        constexpr uint64_t k_BufferSizeAlignment = 4;

        uint64_t AlignTo(uint64_t value, uint64_t alignment)
        {
            return (value + alignment - 1) & ~(alignment - 1);
        }
    }

    wgpu::ShaderModule CreateShaderModule(const wgpu::Device& device, std::string_view wgsl, std::string_view label)
    {
        wgpu::ShaderSourceWGSL source{};
        source.code = wgsl;

        wgpu::ShaderModuleDescriptor descriptor{};
        descriptor.nextInChain = &source;
        descriptor.label = label;
        return device.CreateShaderModule(&descriptor);
    }

    wgpu::Buffer CreateBuffer(const wgpu::Device& device,
        wgpu::BufferUsage usage,
        std::span<const std::byte> data,
        std::string_view label)
    {
        wgpu::BufferDescriptor descriptor{};
        descriptor.label = label;
        descriptor.usage = usage;
        descriptor.size = AlignTo(data.size(), k_BufferSizeAlignment);
        descriptor.mappedAtCreation = true;

        wgpu::Buffer buffer = device.CreateBuffer(&descriptor);
        void* mapped = buffer.GetMappedRange();
        if (mapped && !data.empty())
        {
            std::memcpy(mapped, data.data(), data.size());
        }
        buffer.Unmap();

        return buffer;
    }

    wgpu::Buffer CreateBuffer(const wgpu::Device& device,
        wgpu::BufferUsage usage,
        uint64_t size,
        std::string_view label)
    {
        wgpu::BufferDescriptor descriptor{};
        descriptor.usage = usage;
        descriptor.size = AlignTo(size, k_BufferSizeAlignment);
        descriptor.label = label;
        return device.CreateBuffer(&descriptor);
    }
}
