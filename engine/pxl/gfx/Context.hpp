#pragma once

#include <functional>
#include <string>
#include <string_view>

#include <webgpu/webgpu_cpp.h>

#include <pxl/gfx/Surface.hpp>

namespace pxl::gfx
{
    class Context
    {
        public:
            using ReadyCallback = std::function<void(bool success, std::string_view error)>;

            Context() = default;
            Context(const Context&) = delete;
            Context& operator=(const Context&) = delete;

            void Initialize(const std::string& canvasSelector, ReadyCallback onReady);

            const wgpu::Instance& GetInstance() const { return m_Instance; }
            Surface& GetSurface() { return m_Surface; }
            const wgpu::Adapter& GetAdapter() const { return m_Adapter; }
            const wgpu::Device& GetDevice() const { return m_Device; }
            const wgpu::Queue& GetQueue() const { return m_Queue; }

        private:
            void RequestAdapter();
            void RequestDevice();
            void Finish(bool success, std::string_view error);

            ReadyCallback m_OnReady;

            wgpu::Instance m_Instance;
            Surface m_Surface;
            wgpu::Adapter m_Adapter;
            wgpu::Device m_Device;
            wgpu::Queue m_Queue;
    };
}
