#pragma once

#include <cassert>
#include <cstdint>
#include <span>
#include <string_view>
#include <type_traits>

#include <webgpu/webgpu_cpp.h>

#include <pxl/gfx/Buffer.hpp>

namespace pxl::gfx
{
    template <typename T>
    class StorageBuffer
    {
            static_assert(std::is_trivially_copyable_v<T> && std::is_standard_layout_v<T>, "T is copied to the GPU byte for byte");
            static_assert(sizeof(T) % 4 == 0, "Storage buffer element size must be a multiple of 4 bytes.");

        public:
            StorageBuffer() = default;
            explicit StorageBuffer(const wgpu::Device& device,
                uint32_t count,
                wgpu::BufferUsage extraUsage = wgpu::BufferUsage::None,
                std::string_view label = "")
                : m_Buffer(CreateBuffer(device,
                      wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopyDst | extraUsage,
                      sizeof(T) * count,
                      label)),
                  m_Count(count)
            {
            }
            void Write(const wgpu::Queue& queue, std::span<const T> values, uint32_t first = 0)
            {
                assert(first + values.size() <= m_Count);
                queue.WriteBuffer(m_Buffer, sizeof(T) * first, values.data(), values.size_bytes());
            }

            wgpu::BindGroupEntry GetBindGroupEntry(uint32_t binding) const
            {
                wgpu::BindGroupEntry entry{};
                entry.binding = binding;
                entry.buffer = m_Buffer;
                entry.size = sizeof(T) * m_Count;
                return entry;
            }
            const wgpu::Buffer& GetHandle() const { return m_Buffer; }
            uint32_t GetCount() const { return m_Count; }

        private:
            wgpu::Buffer m_Buffer;
            uint32_t m_Count = 0;
    };
}
