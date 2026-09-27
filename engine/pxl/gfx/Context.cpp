#include <pxl/gfx/Context.hpp>

#include <string_view>
#include <utility>

#include <pxl/log/Log.hpp>

namespace pxl::gfx
{
    namespace
    {
        const char* ToString(wgpu::ErrorType type)
        {
            switch (type)
            {
                case wgpu::ErrorType::NoError:
                    return "no error";
                case wgpu::ErrorType::Validation:
                    return "validation";
                case wgpu::ErrorType::OutOfMemory:
                    return "out of memory";
                case wgpu::ErrorType::Internal:
                    return "internal";
                case wgpu::ErrorType::Unknown:
                    return "unknown";
            }
            return "unrecognised";
        }
    }

    void Context::Initialize(const std::string& canvasSelector, ReadyCallback onReady)
    {
        m_OnReady = std::move(onReady);

        m_Instance = wgpu::CreateInstance();
        m_Surface = Surface(m_Instance, canvasSelector);

        RequestAdapter();
    }

    void Context::RequestAdapter()
    {
        wgpu::RequestAdapterOptions adapterOptions{};
        adapterOptions.compatibleSurface = m_Surface.GetHandle();
        adapterOptions.powerPreference = wgpu::PowerPreference::HighPerformance;

        m_Instance.RequestAdapter(&adapterOptions, wgpu::CallbackMode::AllowSpontaneous,
            [this](wgpu::RequestAdapterStatus status, wgpu::Adapter adapter, wgpu::StringView message)
        {
            if (status != wgpu::RequestAdapterStatus::Success)
            {
                Finish(false, message);
                return;
            }
            m_Adapter = std::move(adapter);
            RequestDevice();
        });
    }

    void Context::RequestDevice()
    {
        wgpu::DeviceDescriptor deviceDesc{};
        deviceDesc.label = "Main Device";
        deviceDesc.SetDeviceLostCallback(wgpu::CallbackMode::AllowSpontaneous,
            [](const wgpu::Device&, wgpu::DeviceLostReason reason, wgpu::StringView message)
        {
            if (reason == wgpu::DeviceLostReason::Destroyed)
            {
                return;
            }
            log::Error("device lost: {}", std::string_view(message));
        });
        deviceDesc.SetUncapturedErrorCallback([](const wgpu::Device&, wgpu::ErrorType type, wgpu::StringView message)
        {
            log::Error("{}: {}", ToString(type), std::string_view(message));
        });

        m_Adapter.RequestDevice(&deviceDesc, wgpu::CallbackMode::AllowSpontaneous,
            [this](wgpu::RequestDeviceStatus status, wgpu::Device device, wgpu::StringView message)
        {
            if (status != wgpu::RequestDeviceStatus::Success)
            {
                Finish(false, message);
                return;
            }
            m_Device = std::move(device);
            m_Queue = m_Device.GetQueue();
            m_Surface.Initialize(m_Adapter, m_Device);
            Finish(true, {});
        });
    }

    void Context::Finish(bool success, std::string_view error)
    {
        ReadyCallback onReady = std::move(m_OnReady);
        m_OnReady = nullptr;
        if (onReady)
        {
            onReady(success, error);
        }
    }
}
