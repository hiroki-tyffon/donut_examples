#include <donut/app/ApplicationBase.h>
#include <donut/engine/ShaderFactory.h>
#include <donut/app/DeviceManager.h>
#include <donut/core/log.h>
#include <donut/core/vfs/VFS.h>
#include <nvrhi/utils.h>

#include "RenderTargets.h"
#include "SLWrapper.h"
#include "UIData.h"
#include "donut/engine/BindingCache.h"


using namespace donut;

static const char* g_WindowTitle = "Donut Example: Basic Triangle";



using namespace donut::math;
using namespace donut::app;
using namespace donut::vfs;
using namespace donut::engine;
using namespace donut::render;

struct ScriptingConfig {

    // Control at start behavior
    int maxFrames = -1;
    int DLSS_mode = -1;
    int Reflex_mode = -1;
    int Reflex_fpsCap = -1;
#ifdef DLSSG_ALLOWED // NDA ONLY DLSS-G DLSS_G Release
    int DLSSG_on = -1;
#endif
    ScriptingConfig() = default;

    ScriptingConfig(int argc, const char* const* argv)
    {

        for (int i = 1; i < argc; i++)
        {
            //MaxFrames
            if (!strcmp(argv[i], "-maxFrames"))
            {
                maxFrames = std::stoi(argv[++i]);
            }

            // DLSS
            else if (!strcmp(argv[i], "-DLSS_mode"))
            {
                DLSS_mode = std::stoi(argv[++i]);
            }

            // Reflex
            else if (!strcmp(argv[i], "-Reflex_mode"))
            {
                Reflex_mode = std::stoi(argv[++i]);
            }
            else if (!strcmp(argv[i], "-Reflex_fpsCap"))
            {
                Reflex_fpsCap = std::stoi(argv[++i]);
            }

#ifdef DLSSG_ALLOWED // NDA ONLY DLSS-G DLSS_G Release
            // DLSSG
            else if (!strcmp(argv[i], "-DLSSG_on"))
            {
                DLSSG_on = 1;
            }
#endif
        }
    }
};

class BasicTriangle : public app::IRenderPass
{
private:
    nvrhi::ShaderHandle m_VertexShader;
    nvrhi::ShaderHandle m_PixelShader;
    nvrhi::GraphicsPipelineHandle m_Pipeline;
    nvrhi::CommandListHandle m_CommandList;

    BindingCache m_BindingCache;
    // RenderTargets
    std::unique_ptr<RenderTargets> m_RenderTargets;

    // UI
    UIData* m_ui;

    // For Streamline
    int2 m_RenderingRectSize = {0, 0};
    int2 m_DisplaySize;

    // Scripting Behavior
    ScriptingConfig                                 m_ScriptingConfig;

public:
    using IRenderPass::IRenderPass;

    BasicTriangle(app::DeviceManager* deviceManager, UIData* pui, const std::string& sceneName,
                  ScriptingConfig scriptingConfig)
        : IRenderPass(deviceManager),
          // Super(deviceManager),
          m_ui(pui)
          , m_BindingCache(deviceManager->GetDevice())
          , m_ScriptingConfig(scriptingConfig)
    {
        m_ui->DLSS_Supported = SLWrapper::Get().GetDLSSAvailable();
        m_ui->REFLEX_Supported = SLWrapper::Get().GetReflexAvailable();
        m_ui->NIS_Supported = SLWrapper::Get().GetNISAvailable();
#ifdef DLSSG_ALLOWED // NDA ONLY DLSS-G DLSS_G Release
    m_ui.DLSSG_Supported = SLWrapper::Get().GetDLSSGAvailable();
#endif // DLSSG_ALLOWED END NDA ONLY DLSS-G DLSS_G Release

        std::shared_ptr<NativeFileSystem> nativeFS = std::make_shared<NativeFileSystem>();

        std::filesystem::path mediaPath = app::GetDirectoryWithExecutable().parent_path() / "media";
        std::filesystem::path frameworkShaderPath = app::GetDirectoryWithExecutable() / "shaders/framework" /
            app::GetShaderTypeName(GetDevice()->getGraphicsAPI());

        // m_RootFs = std::make_shared<RootFileSystem>();
        // m_RootFs->mount("/media", mediaPath);
        // m_RootFs->mount("/shaders/donut", frameworkShaderPath);
        // m_RootFs->mount("/native", nativeFS);
        //
        // m_TextureCache = std::make_shared<TextureCache>(GetDevice(), m_RootFs, nullptr);
        //
        // m_ShaderFactory = std::make_shared<ShaderFactory>(GetDevice(), m_RootFs, "/shaders");
        // m_CommonPasses = std::make_shared<CommonRenderPasses>(GetDevice(), m_ShaderFactory);
        //
        // m_OpaqueDrawStrategy = std::make_shared<InstancedOpaqueDrawStrategy>();

        const nvrhi::Format shadowMapFormats[] = {
            nvrhi::Format::D24S8,
            nvrhi::Format::D32,
            nvrhi::Format::D16,
            nvrhi::Format::D32S8
        };

        const nvrhi::FormatSupport shadowMapFeatures =
            nvrhi::FormatSupport::Texture |
            nvrhi::FormatSupport::DepthStencil |
            nvrhi::FormatSupport::ShaderLoad;

        // nvrhi::Format shadowMapFormat = nvrhi::utils::ChooseFormat(GetDevice(), shadowMapFeatures, shadowMapFormats,
        //                                                            std::size(shadowMapFormats));
        //
        // m_ShadowMap = std::make_shared<CascadedShadowMap>(GetDevice(), 2048, 4, 0, shadowMapFormat);
        // m_ShadowMap->SetupProxyViews();
        //
        // m_ShadowFramebuffer = std::make_shared<FramebufferFactory>(GetDevice());
        // m_ShadowFramebuffer->DepthTarget = m_ShadowMap->GetTexture();
        //
        // DepthPass::CreateParameters shadowDepthParams;
        // shadowDepthParams.slopeScaledDepthBias = 4.f;
        // shadowDepthParams.depthBias = 100;
        // m_ShadowDepthPass = std::make_shared<DepthPass>(GetDevice(), m_CommonPasses);
        // m_ShadowDepthPass->Init(*m_ShaderFactory, shadowDepthParams);
        //
        // m_CommandList = GetDevice()->createCommandList();
        //
        // m_FirstPersonCamera.SetMoveSpeed(3.0f);
        //
        // SetAsynchronousLoadingEnabled(false);
        //
        // if (sceneName.empty())
        //     SetCurrentSceneName("/media/sponza-plus.scene.json");
        // else
        //     SetCurrentSceneName("/native/" + sceneName);

        // Set the callbacks for Reflex
        deviceManager->m_callbacks.beforeFrame = SLWrapper::Callback_FrameCount_Reflex_Sleep_Input_SimStart;
        deviceManager->m_callbacks.afterAnimate = SLWrapper::ReflexCallback_SimEnd;
        deviceManager->m_callbacks.beforeRender = SLWrapper::ReflexCallback_RenderStart;
        deviceManager->m_callbacks.afterRender = SLWrapper::ReflexCallback_RenderEnd;
        deviceManager->m_callbacks.beforePresent = SLWrapper::ReflexCallback_PresentStart;
        deviceManager->m_callbacks.afterPresent = SLWrapper::ReflexCallback_PresentEnd;

        if (m_ScriptingConfig.Reflex_mode != -1 && SLWrapper::Get().GetReflexAvailable())
        {
            static constexpr std::array<int, 3> ValidReflexIndices{0, 1, 2};
            if (std::find(ValidReflexIndices.begin(), ValidReflexIndices.end(), m_ScriptingConfig.Reflex_mode) !=
                ValidReflexIndices.end())
            {
                // CHECK IF THE DLSS MODE IS VALID
                m_ui->REFLEX_Mode = m_ScriptingConfig.Reflex_mode;
            }
        }

        if (m_ScriptingConfig.Reflex_fpsCap > 0 && SLWrapper::Get().GetReflexAvailable())
            m_ui->REFLEX_CapedFPS = m_ScriptingConfig.Reflex_fpsCap;

        if (m_ScriptingConfig.DLSS_mode != -1 && SLWrapper::Get().GetDLSSAvailable())
        {
            static constexpr std::array<int, 6> ValidDLLSIndices{0, 1, 2, 3, 4, 6};
            if (std::find(ValidDLLSIndices.begin(), ValidDLLSIndices.end(), m_ScriptingConfig.DLSS_mode) !=
                ValidDLLSIndices.end())
            {
                // CHECK IF THE DLSS MODE IS VALID
                m_ui->AAMode = AntiAliasingMode::DLSS;
                m_ui->DLSS_Mode = static_cast<sl::DLSSMode>(m_ScriptingConfig.DLSS_mode);
            }
        }
        m_ui->DLSSPresetsReset();

#ifdef DLSSG_ALLOWED // NDA ONLY DLSS-G DLSS_G Release
    if (m_ScriptingConfig.DLSSG_on != -1 && SLWrapper::Get().GetDLSSGAvailable() && SLWrapper::Get().GetReflexAvailable()) {
        if (m_ui.REFLEX_Mode == 0) 
            m_ui.REFLEX_Mode = 1;
        m_ui.DLSSG_mode = sl::DLSSGMode::eOn;
    }
#endif
    };

    bool Init()
    {
        std::filesystem::path appShaderPath = app::GetDirectoryWithExecutable() / "shaders/basic_triangle" /
            app::GetShaderTypeName(GetDevice()->getGraphicsAPI());

        auto nativeFS = std::make_shared<vfs::NativeFileSystem>();
        engine::ShaderFactory shaderFactory(GetDevice(), nativeFS, appShaderPath);

        m_VertexShader = shaderFactory.CreateShader("shaders.hlsl", "main_vs", nullptr, nvrhi::ShaderType::Vertex);
        m_PixelShader = shaderFactory.CreateShader("shaders.hlsl", "main_ps", nullptr, nvrhi::ShaderType::Pixel);

        if (!m_VertexShader || !m_PixelShader)
        {
            return false;
        }

        m_CommandList = GetDevice()->createCommandList();

        return true;
    }

    void BackBufferResizing() override
    {
        m_Pipeline = nullptr;
    }

    void Animate(float fElapsedTimeSeconds) override
    {
        GetDeviceManager()->SetInformativeWindowTitle(g_WindowTitle);
    }

    void Render(nvrhi::IFramebuffer* framebuffer) override
    {
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
        
        RenderScene(framebuffer);
    }

    void RenderScene(nvrhi::IFramebuffer* framebuffer)
    {
        // Pass セットアップ
        {
            bool needNewPasses = false;
            bool useFullSizeRenderingBuffers = m_ui->DLSS_always_use_extents || (m_ui->DLSS_Resolution_Mode ==
                RenderingResolutionMode::DYNAMIC);
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
    }
};


#ifdef WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
// ビルドの設定、サブシステムがコンソールの場合
// WIN32がDefineされているとmainが無いとリンカーエラーがでる
int main(int __argc, const char** __argv)
#endif
{
    nvrhi::GraphicsAPI api = app::GetGraphicsAPIFromCommandLine(__argc, __argv);

    app::DeviceCreationParameters deviceParams;
#ifdef _DEBUG
    deviceParams.enableDebugRuntime = true;
    deviceParams.enableNvrhiValidationLayer = true;
#endif

    bool checkSig = true;
#ifdef _DEBUG
    checkSig = false;
#endif

    // Initialise Streamline before creating the device and swapchain.
    auto success = SLWrapper::Get().Initialize_preDevice(api, checkSig, true);

#if USE_DX11 || USE_DX12
    // We choose the best adapter that supports as many of our features as possible. This will be used for device creation.
    if (api == nvrhi::GraphicsAPI::D3D11 || api == nvrhi::GraphicsAPI::D3D12)
        SLWrapper::Get().FindAdapter((void*&) (deviceParams.adapter));
#endif

    if (!success)
        return 0;

    
    app::DeviceManager* deviceManager = app::DeviceManager::Create(api);
    

    if (!deviceManager->CreateWindowDeviceAndSwapChain(deviceParams, g_WindowTitle))
    {
        log::fatal("Cannot initialize a graphics device with the requested parameters");
        return 1;
    }

    SLWrapper::Get().SetDevice_nvrhi(deviceManager->GetDevice());

    SLWrapper::Get().Initialize_postDevice();

    SLWrapper::Get().UpdateFeatureAvailable(deviceManager);

    {
        UIData uiData;
        uiData.EnableVsync = deviceParams.vsyncEnabled;
        uiData.Resolution = donut::math::int2{ (int)deviceParams.backBufferWidth, (int)deviceParams.backBufferHeight };

        ScriptingConfig scriptingConfig;
        BasicTriangle example(deviceManager, &uiData, "sample", scriptingConfig);
        if (example.Init())
        {
            deviceManager->AddRenderPassToBack(&example);
            deviceManager->RunMessageLoop();
            deviceManager->RemoveRenderPass(&example);
        }
    }

    deviceManager->Shutdown();

    delete deviceManager;

    return 0;
}
