#pragma once

#include <cstdint>

#include <webgpu/webgpu_cpp.h>

namespace pxl::gfx
{
    class DepthBuffer
    {
        public:
            static constexpr wgpu::TextureFormat k_Format = wgpu::TextureFormat::Depth24Plus;

            void Resize(const wgpu::Device& device, uint32_t width, uint32_t height);

            const wgpu::Texture& GetTexture() const { return m_Texture; }
            const wgpu::TextureView& GetTextureView() const { return m_TextureView; }

        private:
            wgpu::Texture m_Texture;
            wgpu::TextureView m_TextureView;
    };
}
