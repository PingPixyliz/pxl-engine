#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include <pxl/gfx/Context.hpp>
#include <pxl/gfx/DepthBuffer.hpp>
#include <pxl/gfx/Frame.hpp>
#include <pxl/platform/Canvas.hpp>

namespace pxl
{
    struct AppConfig
    {
            std::string canvasSelector = "#canvas";
    };

    class Application;

    void Run(std::unique_ptr<Application> app, AppConfig config = {});

    class Application
    {
        public:
            Application() = default;
            virtual ~Application() = default;
            Application(const Application&) = delete;
            Application& operator=(const Application&) = delete;

        protected:
            virtual void OnInit() {}
            virtual void OnResize([[maybe_unused]] uint32_t width, [[maybe_unused]] uint32_t height) {}
            virtual void OnUpdate([[maybe_unused]] float deltaSeconds) {}
            virtual void OnRender([[maybe_unused]] gfx::Frame& frame) {}

            gfx::Context& GetContext() { return m_Context; }
            const platform::Canvas& GetCanvas() const { return m_Canvas; }

        private:
            friend void Run(std::unique_ptr<Application> app, AppConfig config);

            void Start(const AppConfig& config);
            bool Tick(double timeMs);

            gfx::Context m_Context;
            gfx::DepthBuffer m_DepthBuffer;
            platform::Canvas m_Canvas;
            double m_LastTimeMs = -1.0;
    };
}
