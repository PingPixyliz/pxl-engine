#include <pxl/gfx/Surface.hpp>

#include <pxl/log/Log.hpp>

namespace pxl::gfx
{
    namespace
    {
        const char* ToString(wgpu::SurfaceGetCurrentTextureStatus status)
        {
            switch (status)
            {
                case wgpu::SurfaceGetCurrentTextureStatus::SuccessOptimal:
                    return "optimal";
                case wgpu::SurfaceGetCurrentTextureStatus::SuccessSuboptimal:
                    return "suboptimal";
                case wgpu::SurfaceGetCurrentTextureStatus::Timeout:
                    return "timed out";
                case wgpu::SurfaceGetCurrentTextureStatus::Outdated:
                    return "outdated";
                case wgpu::SurfaceGetCurrentTextureStatus::Lost:
                    return "lost";
                case wgpu::SurfaceGetCurrentTextureStatus::Error:
                    return "error";
            }
            return "unrecognised";
        }
    }

    Surface::Surface(const wgpu::Instance& instance, const std::string& canvasSelector)
    {
        wgpu::EmscriptenSurfaceSourceCanvasHTMLSelector canvasSource{};
        canvasSource.selector = canvasSelector.c_str();

        wgpu::SurfaceDescriptor surfaceDesc{};
        surfaceDesc.nextInChain = &canvasSource;
        surfaceDesc.label = "Canvas Surface";
        m_Surface = instance.CreateSurface(&surfaceDesc);
    }

    void Surface::Initialize(const wgpu::Adapter& adapter, const wgpu::Device& device)
    {
        m_Device = device;

        wgpu::SurfaceCapabilities capabilities;
        m_Surface.GetCapabilities(adapter, &capabilities);
        m_Format = capabilities.formatCount > 0 ? capabilities.formats[0] : wgpu::TextureFormat::BGRA8Unorm;
    }

    void Surface::Configure(uint32_t width, uint32_t height)
    {
        m_Width = width;
        m_Height = height;

        wgpu::SurfaceConfiguration config{};
        config.device = m_Device;
        config.format = m_Format;
        config.usage = wgpu::TextureUsage::RenderAttachment;
        config.width = width;
        config.height = height;
        config.alphaMode = wgpu::CompositeAlphaMode::Opaque;
        config.presentMode = wgpu::PresentMode::Fifo;
        m_Surface.Configure(&config);
    }

    wgpu::TextureView Surface::AcquireNextView()
    {
        wgpu::SurfaceTexture surfaceTexture{};
        m_Surface.GetCurrentTexture(&surfaceTexture);

        switch (surfaceTexture.status)
        {
            case wgpu::SurfaceGetCurrentTextureStatus::SuccessOptimal:
            case wgpu::SurfaceGetCurrentTextureStatus::SuccessSuboptimal:
                break;
            case wgpu::SurfaceGetCurrentTextureStatus::Timeout:
            case wgpu::SurfaceGetCurrentTextureStatus::Outdated:
            case wgpu::SurfaceGetCurrentTextureStatus::Lost:
                log::Warn("Surface::AcquireNextView: surface {}, skipping the frame", ToString(surfaceTexture.status));
                if (m_Width > 0 && m_Height > 0)
                {
                    Configure(m_Width, m_Height);
                }
                return nullptr;
            case wgpu::SurfaceGetCurrentTextureStatus::Error:
                log::Error("Surface::AcquireNextView: GetCurrentTexture failed ({})", ToString(surfaceTexture.status));
                return nullptr;
        }

        wgpu::TextureViewDescriptor viewDesc{};
        viewDesc.label = "Canvas View";
        return surfaceTexture.texture.CreateView(&viewDesc);
    }
}
