#pragma once

#include <span>
#include <string_view>

#include <webgpu/webgpu_cpp.h>

#include <pxl/gfx/DepthBuffer.hpp>

namespace pxl::gfx
{
    struct RenderPipelineConfig
    {
            wgpu::ShaderModule shader;
            std::string_view vertexEntry = "vs_main";
            std::string_view fragmentEntry = "fs_main";
            std::span<const wgpu::VertexBufferLayout> vertexBuffers;
            wgpu::TextureFormat colorFormat = wgpu::TextureFormat::Undefined;
            wgpu::TextureFormat depthFormat = DepthBuffer::k_Format;
            wgpu::PrimitiveTopology primitiveTopology = wgpu::PrimitiveTopology::TriangleList;
            wgpu::CullMode cullMode = wgpu::CullMode::None;
            wgpu::PipelineLayout layout;
    };

    wgpu::RenderPipeline CreateRenderPipeline(const wgpu::Device& device,
        const RenderPipelineConfig& config,
        std::string_view label = "");

    wgpu::ComputePipeline CreateComputePipeline(const wgpu::Device& device,
        const wgpu::ShaderModule& shader,
        std::string_view entry = "cs_main",
        const wgpu::PipelineLayout& layout = nullptr,
        std::string_view label = "");
}
