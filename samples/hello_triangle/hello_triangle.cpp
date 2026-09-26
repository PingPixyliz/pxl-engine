#include <memory>

#include <pxl/pxl.hpp>
#include "shaders/triangle.wgsl.hpp"

namespace
{
    class HelloTriangle : public pxl::Application
    {
        protected:
            void OnInit() override
            {
                const wgpu::Device& device = GetContext().GetDevice();

                wgpu::ShaderModule shaderModule =
                    pxl::gfx::CreateShaderModule(device, shaders::triangle_wgsl, "Triangle Shader");

                wgpu::ColorTargetState colorTarget{};
                colorTarget.format = GetContext().GetSurface().GetFormat();

                wgpu::FragmentState fragmentState{};
                fragmentState.module = shaderModule;
                fragmentState.entryPoint = "fs_main";
                fragmentState.targetCount = 1;
                fragmentState.targets = &colorTarget;

                wgpu::RenderPipelineDescriptor pipelineDesc{};
                pipelineDesc.label = "Triangle Pipeline";
                pipelineDesc.vertex.module = shaderModule;
                pipelineDesc.vertex.entryPoint = "vs_main";
                pipelineDesc.fragment = &fragmentState;
                pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
                m_Pipeline = device.CreateRenderPipeline(&pipelineDesc);
            }

            void OnRender(pxl::gfx::Frame& frame) override
            {
                wgpu::RenderPassColorAttachment color{};
                color.view = frame.target;
                color.loadOp = wgpu::LoadOp::Clear;
                color.storeOp = wgpu::StoreOp::Store;
                color.clearValue = {0.05, 0.05, 0.07, 1.0};

                wgpu::RenderPassDescriptor passDesc{};
                passDesc.label = "Triangle Pass";
                passDesc.colorAttachmentCount = 1;
                passDesc.colorAttachments = &color;

                wgpu::RenderPassEncoder pass = frame.encoder.BeginRenderPass(&passDesc);
                pass.SetPipeline(m_Pipeline);
                pass.Draw(3);
                pass.End();
            }

        private:
            wgpu::RenderPipeline m_Pipeline;
    };
}

int main()
{
    pxl::Run(std::make_unique<HelloTriangle>());
    return 0;
}
