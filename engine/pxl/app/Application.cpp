#include <pxl/app/Application.hpp>

#include <algorithm>
#include <string>
#include <utility>

#include <pxl/platform/MainLoop.hpp>
#include <pxl/platform/Page.hpp>

namespace pxl
{
    namespace
    {
        std::unique_ptr<Application> g_App;
    }

    void Run(std::unique_ptr<Application> app, AppConfig config)
    {
        g_App = std::move(app);
        g_App->Start(config);
    }

    void Application::Start(const AppConfig& config)
    {
        m_Canvas = platform::Canvas(config.canvasSelector);

        m_Context.Initialize(config.canvasSelector, [this](bool success, std::string_view error)
        {
            if (!success)
            {
                platform::ShowFatalError("WebGPU could not be initialised.\n" + std::string(error));
                return;
            }
            OnInit();
            platform::StartMainLoop([this](double timeMs)
            {
                return Tick(timeMs);
            });
        });
    }

    bool Application::Tick(double timeMs)
    {
        constexpr float k_MaxDeltaSeconds = 0.1f;
        const float rawDelta = m_LastTimeMs < 0.0 ? 0.0f : static_cast<float>((timeMs - m_LastTimeMs) / 1000.0);
        const float deltaSeconds = std::min(rawDelta, k_MaxDeltaSeconds);
        m_LastTimeMs = timeMs;

        gfx::Surface& surface = m_Context.GetSurface();
        if (m_Canvas.UpdateSize())
        {
            surface.Configure(m_Canvas.GetWidth(), m_Canvas.GetHeight());
            m_DepthBuffer.Resize(m_Context.GetDevice(), m_Canvas.GetWidth(), m_Canvas.GetHeight());
            OnResize(m_Canvas.GetWidth(), m_Canvas.GetHeight());
        }

        OnUpdate(deltaSeconds);

        wgpu::TextureView target = surface.AcquireNextView();
        if (!target)
        {
            return true;
        }

        wgpu::CommandEncoderDescriptor encoderDesc{};
        encoderDesc.label = "Frame Encoder";

        gfx::Frame frame{};
        frame.encoder = m_Context.GetDevice().CreateCommandEncoder(&encoderDesc);
        frame.target = std::move(target);
        frame.depth = m_DepthBuffer.GetTextureView();
        frame.format = surface.GetFormat();
        frame.width = surface.GetWidth();
        frame.height = surface.GetHeight();

        OnRender(frame);

        wgpu::CommandBuffer commands = frame.encoder.Finish();
        m_Context.GetQueue().Submit(1, &commands);
        return true;
    }
}
