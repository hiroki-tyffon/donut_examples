#pragma once

#include "SLWrapper.h"
#include "RenderTargets.h"
#include "UIData.h"
#include <random>
#include <chrono>

// From Donut
#include <donut/core/vfs/VFS.h>
#include <donut/core/log.h>
#include <donut/engine/CommonRenderPasses.h>
#include <donut/engine/FramebufferFactory.h>
#include <donut/engine/Scene.h>
#include <donut/engine/ShaderFactory.h>
#include <donut/engine/TextureCache.h>
#include <donut/render/BloomPass.h>
#include <donut/render/CascadedShadowMap.h>
#include <donut/render/DeferredLightingPass.h>
#include <donut/render/DepthPass.h>
#include <donut/render/DrawStrategy.h>
#include <donut/render/ForwardShadingPass.h>
#include <donut/render/GBufferFillPass.h>
#include <donut/render/LightProbeProcessingPass.h>
#include <donut/render/PixelReadbackPass.h>
#include <donut/render/SkyPass.h>
#include <donut/render/SsaoPass.h>
#include <donut/render/TemporalAntiAliasingPass.h>
#include <donut/render/ToneMappingPasses.h>
#include <donut/app/ApplicationBase.h>
#include <donut/app/Camera.h>
#include <donut/app/DeviceManager.h>
#include <nvrhi/utils.h>

#include "StreamlineSample.h"

using namespace donut::math;
using namespace donut::app;
using namespace donut::vfs;
using namespace donut::engine;
using namespace donut::render;

class StreamlineApp : public ApplicationBase
{
    nvrhi::GraphicsPipelineHandle m_Pipeline;
    nvrhi::CommandListHandle m_CommandList;
    BindingCache m_BindingCache;

    // RenderTargets
    std::unique_ptr<RenderTargets> m_RenderTargets;

    // For Streamline
    int2 m_RenderingRectSize = {0, 0};
    int2 m_DisplaySize;

    nvrhi::ShaderHandle m_VertexShader;
    nvrhi::ShaderHandle m_PixelShader;

public:
    StreamlineApp(DeviceManager* deviceManager, UIData& ui, const std::string& sceneName,
                  ScriptingConfig scriptingConfig)
        : ApplicationBase(deviceManager), m_BindingCache(deviceManager->GetDevice())
    {
        std::filesystem::path appShaderPath = GetDirectoryWithExecutable() / "shaders/basic_triangle" /
            GetShaderTypeName(GetDevice()->getGraphicsAPI());

        auto nativeFS = std::make_shared<NativeFileSystem>();
        ShaderFactory shaderFactory(GetDevice(), nativeFS, appShaderPath);

        m_VertexShader = shaderFactory.CreateShader("shaders.hlsl", "main_vs", nullptr, nvrhi::ShaderType::Vertex);
        m_PixelShader = shaderFactory.CreateShader("shaders.hlsl", "main_ps", nullptr, nvrhi::ShaderType::Pixel);

        if (!m_VertexShader || !m_PixelShader)
        {
            return;
        }

        m_CommandList = GetDevice()->createCommandList();

        // これをしないとRenderSceneが呼ばれない
        ApplicationBase::SceneLoaded();
    }

    bool LoadScene(std::shared_ptr<IFileSystem> fs, const std::filesystem::path& fileName) override
    {
        return true;
    }

    void RenderScene(nvrhi::IFramebuffer* framebuffer) override
    {
        // INITIALISE

        int windowWidth, windowHeight;
        GetDeviceManager()->GetWindowDimensions(windowWidth, windowHeight);
        nvrhi::Viewport windowViewport = nvrhi::Viewport((float)windowWidth, (float)windowHeight);

        // m_Scene->RefreshSceneGraph(GetFrameIndex());

        bool exposureResetRequired = false;
        bool needNewPasses = false;

        m_DisplaySize = int2(windowWidth, windowHeight);
        float lodBias = 0.f;
        
        // PASS SETUP
        {
            bool needNewPasses = false;

            // Here, we intentionally leave the renderTargets oversized: (displaySize, displaySize) instead of (m_RenderingRectSize, displaySize), to show the power of sl::Extent
            // bool useFullSizeRenderingBuffers = m_ui.DLSS_always_use_extents || (m_ui.DLSS_Resolution_Mode ==
            //     RenderingResolutionMode::DYNAMIC);
            bool useFullSizeRenderingBuffers = true;

            donut::math::int2 renderSize = useFullSizeRenderingBuffers ? m_DisplaySize : m_RenderingRectSize;

            if (!m_RenderTargets || m_RenderTargets->IsUpdateRequired(renderSize, m_DisplaySize))
            {
                m_BindingCache.Clear();

                m_RenderTargets = nullptr;
                m_RenderTargets = std::make_unique<RenderTargets>();
                m_RenderTargets->Init(GetDevice(), renderSize, m_DisplaySize,
                                      framebuffer->getDesc().colorAttachments[0].texture->getDesc().format);

                needNewPasses = true;
            }
        }

        if (!m_Pipeline)
        {
            nvrhi::GraphicsPipelineDesc psoDesc;
            psoDesc.VS = m_VertexShader;
            psoDesc.PS = m_PixelShader;
            psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
            psoDesc.renderState.depthStencilState.depthTestEnable = false;

            m_Pipeline = GetDevice()->createGraphicsPipeline(psoDesc, framebuffer);
        }

        m_CommandList->open();

        nvrhi::utils::ClearColorAttachment(m_CommandList, framebuffer, 0, nvrhi::Color(0.f));

        nvrhi::GraphicsState state;
        state.pipeline = m_Pipeline;
        state.framebuffer = framebuffer;
        state.viewport.addViewportAndScissorRect(framebuffer->getFramebufferInfo().getViewport());

        m_CommandList->setGraphicsState(state);

        nvrhi::DrawArguments args;
        args.vertexCount = 3;
        m_CommandList->draw(args);

        m_CommandList->close();
        GetDevice()->executeCommandList(m_CommandList);
    }
};
