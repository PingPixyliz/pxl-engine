#include <pxl/gfx/DepthBuffer.hpp>

namespace pxl::gfx
{
    void DepthBuffer::Resize(const wgpu::Device& device, uint32_t width, uint32_t height)
    {
        if (m_Texture)
        {
            m_Texture.Destroy();
            m_TextureView = nullptr;
        }

        wgpu::TextureDescriptor textureDesc{};
        textureDesc.label = "Depth Buffer";
        textureDesc.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::TextureBinding;
        textureDesc.dimension = wgpu::TextureDimension::e2D;
        textureDesc.size = {width, height, 1};
        textureDesc.format = k_Format;
        m_Texture = device.CreateTexture(&textureDesc);

        wgpu::TextureViewDescriptor viewDesc{};
        viewDesc.label = "Depth Buffer View";
        m_TextureView = m_Texture.CreateView(&viewDesc);
    }
}
