#include <pxl/gfx/Pipeline.hpp>

#include <pxl/log/Log.hpp>

namespace pxl::gfx
{
    wgpu::RenderPipeline CreateRenderPipeline(const wgpu::Device& device,
        const RenderPipelineConfig& config,
        std::string_view label)
    {
        if (!config.shader)
        {
            log::Error("CreateRenderPipeline: no shader module for '{}'", label);
            return nullptr;
        }

        wgpu::ColorTargetState colorTargetState{};
        colorTargetState.format = config.colorFormat;

        wgpu::FragmentState fragmentState{};
        fragmentState.module = config.shader;
        fragmentState.entryPoint = config.fragmentEntry;
        fragmentState.targetCount = 1;
        fragmentState.targets = &colorTargetState;

        wgpu::DepthStencilState depthStencilState{};
        depthStencilState.format = config.depthFormat;
        depthStencilState.depthWriteEnabled = wgpu::OptionalBool::True;
        depthStencilState.depthCompare = wgpu::CompareFunction::Less;

        wgpu::RenderPipelineDescriptor pipelineDesc{};
        pipelineDesc.label = label;
        pipelineDesc.layout = config.layout;
        pipelineDesc.vertex.module = config.shader;
        pipelineDesc.vertex.entryPoint = config.vertexEntry;
        pipelineDesc.vertex.bufferCount = config.vertexBuffers.size();
        pipelineDesc.vertex.buffers = config.vertexBuffers.data();
        pipelineDesc.fragment = &fragmentState;
        pipelineDesc.primitive.topology = config.primitiveTopology;
        pipelineDesc.primitive.cullMode = config.cullMode;

        if (config.depthFormat != wgpu::TextureFormat::Undefined)
        {
            pipelineDesc.depthStencil = &depthStencilState;
        }
        return device.CreateRenderPipeline(&pipelineDesc);
    }

    wgpu::ComputePipeline CreateComputePipeline(const wgpu::Device& device,
        const wgpu::ShaderModule& shader,
        std::string_view entry,
        const wgpu::PipelineLayout& layout,
        std::string_view label)
    {
        if (!shader)
        {
            log::Error("CreateComputePipeline: no shader module for '{}'", label);
            return nullptr;
        }

        wgpu::ComputePipelineDescriptor pipelineDesc{};
        pipelineDesc.label = label;
        pipelineDesc.layout = layout;
        pipelineDesc.compute.module = shader;
        pipelineDesc.compute.entryPoint = entry;

        return device.CreateComputePipeline(&pipelineDesc);
    }
}
