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

    // Filesystem and scene
    std::shared_ptr<RootFileSystem> m_RootFs;

    // Render Passes                                
    std::shared_ptr<ShaderFactory> m_ShaderFactory;

    // RenderTargets
    std::unique_ptr<RenderTargets> m_RenderTargets;

    //Views
    std::shared_ptr<IView> m_View;
    bool m_PreviousViewsValid = false;
    std::shared_ptr<IView> m_ViewPrevious;
    // std::shared_ptr<IView>                          m_TonemappingView;

    // For Streamline
    int2 m_RenderingRectSize = {100, 100};
    int2 m_DisplaySize;

    nvrhi::ShaderHandle m_VertexShader;
    nvrhi::ShaderHandle m_PixelShader;

public:
    StreamlineApp(DeviceManager* deviceManager, UIData& ui, const std::string& sceneName,
                  ScriptingConfig scriptingConfig)
        : ApplicationBase(deviceManager), m_BindingCache(deviceManager->GetDevice())
    {
        auto nativeFS = std::make_shared<NativeFileSystem>();

        auto mediaPath = GetDirectoryWithExecutable().parent_path() / "media";
        auto frameworkShaderPath = GetDirectoryWithExecutable() / "shaders/framework" /
            GetShaderTypeName(GetDevice()->getGraphicsAPI());

        m_RootFs = std::make_shared<RootFileSystem>();
        m_RootFs->mount("/media", mediaPath);
        m_RootFs->mount("/shaders/donut", frameworkShaderPath);
        m_RootFs->mount("/native", nativeFS);

        m_ShaderFactory = std::make_shared<ShaderFactory>(GetDevice(), m_RootFs, "/shaders");
        m_CommonPasses = std::make_shared<CommonRenderPasses>(GetDevice(), m_ShaderFactory);

        m_CommandList = GetDevice()->createCommandList();

        std::filesystem::path appShaderPath = GetDirectoryWithExecutable() / "shaders/basic_triangle" /
            GetShaderTypeName(GetDevice()->getGraphicsAPI());
        ShaderFactory shaderFactory(GetDevice(), nativeFS, appShaderPath);
        m_VertexShader = shaderFactory.CreateShader("shaders.hlsl", "main_vs", nullptr, nvrhi::ShaderType::Vertex);
        m_PixelShader = shaderFactory.CreateShader("shaders.hlsl", "main_ps", nullptr, nvrhi::ShaderType::Pixel);

        if (!m_VertexShader || !m_PixelShader)
        {
            return;
        }

        // これをしないとRenderSceneが呼ばれない
        ApplicationBase::SceneLoaded();
    }

    bool SetupView()
    {
        // if (m_TemporalAntiAliasingPass) m_TemporalAntiAliasingPass->SetJitter(m_ui.TemporalAntiAliasingJitter);
        //
        // float2 pixelOffset = m_ui.AAMode != AntiAliasingMode::NONE && m_TemporalAntiAliasingPass
        //                          ? m_TemporalAntiAliasingPass->GetCurrentPixelOffset()
        //                          : float2(0.f);
        //
        std::shared_ptr<PlanarView> planarView = std::dynamic_pointer_cast<PlanarView, IView>(m_View);
        //
        // dm::affine3 viewMatrix;
        // float verticalFov = dm::radians(m_CameraVerticalFov);
        float zNear = 0.01f;
        // viewMatrix = m_FirstPersonCamera.GetWorldToViewMatrix();

        bool topologyChanged = false;

        // Render View
        {
            if (!planarView)
            {
                m_View = planarView = std::make_shared<PlanarView>();
                m_ViewPrevious = std::make_shared<PlanarView>();
                topologyChanged = true;
            }

            // float4x4 projection = perspProjD3DStyleReverse(verticalFov,
            //                                                float(m_RenderingRectSize.x) / m_RenderingRectSize.y, zNear);
            //
            // planarView->SetViewport(nvrhi::Viewport((float)m_RenderingRectSize.x, (float)m_RenderingRectSize.y));
            // planarView->SetPixelOffset(pixelOffset);
            //
            // planarView->SetMatrices(viewMatrix, projection);
            // planarView->UpdateCache();
            //
            // if (topologyChanged)
            // {
            //     *std::static_pointer_cast<PlanarView>(m_ViewPrevious) = *std::static_pointer_cast<PlanarView>(m_View);
            // }
        }

        // ToneMappingView
        // {
        //     std::shared_ptr<PlanarView> tonemappingPlanarView = std::dynamic_pointer_cast<PlanarView, IView>(
        //         m_TonemappingView);
        //
        //     if (!tonemappingPlanarView)
        //     {
        //         m_TonemappingView = tonemappingPlanarView = std::make_shared<PlanarView>();
        //         topologyChanged = true;
        //     }
        //
        //     float4x4 projection = perspProjD3DStyleReverse(verticalFov,
        //                                                    float(m_RenderingRectSize.x) / m_RenderingRectSize.y, zNear);
        //
        //     tonemappingPlanarView->SetViewport(nvrhi::Viewport((float)m_DisplaySize.x, (float)m_DisplaySize.y));
        //     tonemappingPlanarView->SetMatrices(viewMatrix, projection);
        //     tonemappingPlanarView->UpdateCache();
        // }

        return topologyChanged;
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
            bool useFullSizeRenderingBuffers = false;

            int2 renderSize = useFullSizeRenderingBuffers ? m_DisplaySize : m_RenderingRectSize;

            if (!m_RenderTargets || m_RenderTargets->IsUpdateRequired(renderSize, m_DisplaySize))
            {
                m_BindingCache.Clear();

                m_RenderTargets = nullptr;
                m_RenderTargets = std::make_unique<RenderTargets>();
                m_RenderTargets->Init(GetDevice(), renderSize, m_DisplaySize,
                                      framebuffer->getDesc().colorAttachments[0].texture->getDesc().format);

                needNewPasses = true;
            }

            // Render scene, change bias
            // if (m_ui.DLSS_lodbias_useoveride) lodBias = m_ui.DLSS_lodbias_overide;
            // if (m_PreviousLodBias != lodBias)
            // {
            //     needNewPasses = true;
            //     m_PreviousLodBias = lodBias;
            // }

            if (SetupView())
            {
                needNewPasses = true;
            }

            // if (needNewPasses)
            // {
            //     CreateRenderPasses(exposureResetRequired, lodBias);
            // }
        }

        nvrhi::IFramebuffer* hdrFramebuffer = m_RenderTargets->HdrFramebuffer->GetFramebuffer(*m_View);
        if (!m_Pipeline)
        {
            nvrhi::GraphicsPipelineDesc psoDesc;
            psoDesc.VS = m_VertexShader;
            psoDesc.PS = m_PixelShader;
            psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
            psoDesc.renderState.depthStencilState.depthTestEnable = false;
#if 0
            m_Pipeline = GetDevice()->createGraphicsPipeline(psoDesc, framebuffer);
#else
            m_Pipeline = GetDevice()->createGraphicsPipeline(psoDesc, hdrFramebuffer);
#endif
        }

        m_CommandList->open();

        // draw
        {
            auto drawFramebuffer = hdrFramebuffer;
            auto info = drawFramebuffer->getFramebufferInfo();

            nvrhi::utils::ClearColorAttachment(m_CommandList, drawFramebuffer, 0, nvrhi::Color(0.f));

            nvrhi::GraphicsState state;
            state.pipeline = m_Pipeline;
            state.framebuffer = drawFramebuffer;
            state.viewport.addViewportAndScissorRect(drawFramebuffer->getFramebufferInfo().getViewport());

            m_CommandList->setGraphicsState(state);

            nvrhi::DrawArguments args;
            args.vertexCount = 3;
            m_CommandList->draw(args);
        }

        // nvrhi::ITexture* framebufferTexture = framebuffer->getDesc().colorAttachments[0].texture;
        // nvrhi::TextureSlice slice;
        // slice.width = 640;
        // slice.height = 480;
        // m_CommandList->copyTexture(framebufferTexture, slice, m_RenderTargets->HdrColor, nvrhi::TextureSlice());

        // DEBUG OVERLAY
        {
            static constexpr int SubWindowNumber = 2;
            static constexpr float SubWindowSpacing = 5.f;

            // If we want to, we can overlay the other textures onto the screen for comparative inspection
            auto displayDebugPiP = [&](nvrhi::TextureHandle texture, int2 pos, float scale)
            {
                // This snippet is by Manuel Kraemer

                dm::float2 size = dm::float2(float(windowWidth), float(windowHeight - 2.f * SubWindowSpacing)) * scale;

                nvrhi::Viewport viewport = nvrhi::Viewport(
                    SubWindowSpacing * (pos.x + 1) + size.x * pos.x,
                    SubWindowSpacing * (pos.x + 1) + size.x * (pos.x + 1),
                    windowViewport.maxY - SubWindowSpacing * (pos.y + 1) - size.y * (pos.y + 1),
                    windowViewport.maxY - SubWindowSpacing * (pos.y + 1) - size.y * pos.y, 0.f, 1.f
                );

                BlitParameters blitParams;
                blitParams.targetFramebuffer = framebuffer;
                blitParams.targetViewport = viewport;
                blitParams.sourceTexture = texture;
                m_CommonPasses->BlitTexture(m_CommandList, blitParams, &m_BindingCache);
            };

            int counter = 0;
            // displayDebugPiP(m_RenderTargets->MotionVectors, int2(counter % SubWindowNumber, counter++ / SubWindowNumber), 1 / float(SubWindowNumber));
            displayDebugPiP(m_RenderTargets->HdrColor, int2(counter % SubWindowNumber, counter++ / SubWindowNumber),
                            1 / float(SubWindowNumber));
            displayDebugPiP(m_RenderTargets->Depth, int2(counter % SubWindowNumber, counter++ / SubWindowNumber),
                            1 / float(SubWindowNumber));
        }

        m_CommandList->close();
        GetDevice()->executeCommandList(m_CommandList);
    }
};
