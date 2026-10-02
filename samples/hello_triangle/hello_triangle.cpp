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

                pxl::gfx::RenderPipelineConfig config{};
                config.shader = pxl::gfx::CreateShaderModule(device, shaders::triangle_wgsl, "Triangle Shader");
                config.colorFormat = GetContext().GetSurface().GetFormat();
                config.depthFormat = wgpu::TextureFormat::Undefined; // No need depthFormat on this sample
                m_Pipeline = pxl::gfx::CreateRenderPipeline(device, config, "Triangle Pipeline");
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
