#pragma once

#include <cstdint>
#include <string>

#include <webgpu/webgpu_cpp.h>

namespace pxl::gfx
{
    class Surface
    {
        public:
            Surface() = default;
            Surface(const wgpu::Instance& instance, const std::string& canvasSelector);

            void Initialize(const wgpu::Adapter& adapter, const wgpu::Device& device);
            void Configure(uint32_t width, uint32_t height);
            wgpu::TextureView AcquireNextView();

            const wgpu::Surface& GetHandle() const { return m_Surface; }
            wgpu::TextureFormat GetFormat() const { return m_Format; }
            uint32_t GetWidth() const { return m_Width; }
            uint32_t GetHeight() const { return m_Height; }

        private:
            wgpu::Surface m_Surface;
            wgpu::Device m_Device;
            wgpu::TextureFormat m_Format = wgpu::TextureFormat::Undefined;
            uint32_t m_Width = 0;
            uint32_t m_Height = 0;
    };
}
