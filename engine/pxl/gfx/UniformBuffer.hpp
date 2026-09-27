#pragma once

#include <cstdint>
#include <string_view>
#include <type_traits>

#include <webgpu/webgpu_cpp.h>

#include <pxl/gfx/Buffer.hpp>

namespace pxl::gfx
{
    template <typename T>
    class UniformBuffer
    {
            static_assert(std::is_trivially_copyable_v<T> && std::is_standard_layout_v<T>, "T is copied to the GPU byte for byte");
            static_assert(sizeof(T) % 4 == 0, "Uniform buffer size must be a multiple of 4 bytes.");

        public:
            UniformBuffer() = default;
            explicit UniformBuffer(const wgpu::Device& device, std::string_view label = "")
                : m_Buffer(
                      CreateBuffer(device, wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst, sizeof(T), label))
            {
            }

            void Write(const wgpu::Queue& queue, const T& value) { queue.WriteBuffer(m_Buffer, 0, &value, sizeof(T)); }

            wgpu::BindGroupEntry GetBindGroupEntry(uint32_t binding) const
            {
                wgpu::BindGroupEntry entry{};
                entry.binding = binding;
                entry.buffer = m_Buffer;
                entry.size = sizeof(T);
                return entry;
            }
            const wgpu::Buffer& GetHandle() const { return m_Buffer; }

        private:
            wgpu::Buffer m_Buffer;
    };
}
