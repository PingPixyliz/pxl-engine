#include <pxl/gfx/Texture.hpp>

#include <pxl/log/Log.hpp>

namespace pxl::gfx
{
    wgpu::Texture CreateTexture2D(const wgpu::Device& device,
        uint32_t width,
        uint32_t height,
        wgpu::TextureFormat format,
        wgpu::TextureUsage usage,
        std::string_view label)
    {
        wgpu::TextureDescriptor descriptor{};
        descriptor.label = label;
        descriptor.usage = usage;
        descriptor.dimension = wgpu::TextureDimension::e2D;
        descriptor.size = {width, height, 1};
        descriptor.format = format;
        return device.CreateTexture(&descriptor);
    }

    wgpu::Texture CreateTexture(const wgpu::Device& device, const asset::Image& image, std::string_view label)
    {
        if (image.IsEmpty())
        {
            log::Error("CreateTexture: no image data for '{}'", label);
            return nullptr;
        }

        wgpu::Texture texture = CreateTexture2D(device, image.width, image.height, wgpu::TextureFormat::RGBA8Unorm,
            wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst, label);

        wgpu::TexelCopyTextureInfo destination{};
        destination.texture = texture;

        wgpu::TexelCopyBufferLayout layout{};
        layout.bytesPerRow = image.width * asset::Image::k_BytesPerPixel;
        layout.rowsPerImage = image.height;

        const wgpu::Extent3D size{image.width, image.height, 1};
        device.GetQueue().WriteTexture(&destination, image.pixels.data(), image.pixels.size(), &layout, &size);
        return texture;
    }
}
