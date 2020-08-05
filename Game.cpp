//
// Game.cpp
//

#include "pch.h"
#include "Game.h"


extern void ExitGame() noexcept;

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

ESPOverlay::ESPOverlay() noexcept :
    m_window(nullptr),
    m_targetWindow(nullptr),
    m_outputWidth(800),
    m_outputHeight(600),
    m_featureLevel(D3D_FEATURE_LEVEL_9_1)
{
}

// Initialize the Direct3D resources required to run.
void ESPOverlay::Initialize(HWND window)
{
    m_window = window;
 /*   m_outputWidth = std::max(width, 1);
    m_outputHeight = std::max(height, 1);*/

    CreateDevice();

 /*   RECT rc;
    GetWindowRect(m_targetWindow, &rc);*/
    CreateResources();

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60.f);
}

void ESPOverlay::AttachWindow(std::wstring targetWindowName)
{
   m_targetWindow = FindWindowW(NULL, targetWindowName.c_str());
   std::wstring ErrorMessage = targetWindowName + std::wstring(L" has not been found. Retry?");
   
   while (m_targetWindow == NULL)
   {
        auto choice = MessageBox(m_window, ErrorMessage.c_str(), L"ERROR", MB_RETRYCANCEL);
        if (choice == IDCANCEL) exit(0);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        m_targetWindow = FindWindow(NULL, targetWindowName.c_str());
   }

   {
       RECT rect;
       GetClientRect(m_targetWindow, &rect);
       OnWindowSizeChanged(rect.right, rect.bottom);
       int clientwidth, clientheight;
       clientwidth = rect.right;
       clientheight = rect.bottom;
       GetWindowRect(m_targetWindow, &rect);
       MoveWindow(this->m_window, rect.left, rect.top, clientwidth, clientheight, false);
   }

   AttachedWindow = targetWindowName;
   b_isWindowAttached = true;
}

// Executes the basic game loop.
void ESPOverlay::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });
   
    Render();
}

// Updates the world.
void ESPOverlay::Update(DX::StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());

    // TODO: Add your game logic here.
    elapsedTime;
}

// Draws the scene.
void ESPOverlay::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        Present();
        return;
    }

    Clear(Colors::Black);
    if (!b_isWindowAttached) AttachWindow(std::wstring(GAME_NAME));

    // TODO: Add your rendering code here.

    SetAsOverlay();
   /* if (!isTargetWindowOnForeground())
        return;*/

    m_spriteBatch->Begin(SpriteSortMode_Deferred, m_states->NonPremultiplied());
 
    m_spriteBatch->End();

    Present();
}

// Helper method to clear the back buffers.
void ESPOverlay::Clear(DirectX::XMVECTORF32 BkgCol )
{
    // Clear the views.
    m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), BkgCol);
    m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

    // Set the viewport.
    CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(m_outputWidth), static_cast<float>(m_outputHeight));
    m_d3dContext->RSSetViewports(1, &viewport);
}

// Presents the back buffer contents to the screen.
void ESPOverlay::Present()
{
    // The first argument instructs DXGI to block until VSync, putting the application
    // to sleep until the next VSync. This ensures we don't waste any cycles rendering
    // frames that will never be displayed to the screen.
    HRESULT hr = m_swapChain->Present(1, 0);

    // If the device was reset we must completely reinitialize the renderer.
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        OnDeviceLost();
    }
    else
    {
        DX::ThrowIfFailed(hr);
    }
}

// Message handlers
void ESPOverlay::OnActivated()
{
    // TODO: Game is becoming active window.
}

void ESPOverlay::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void ESPOverlay::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void ESPOverlay::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void ESPOverlay::OnWindowSizeChanged(int width, int height)
{
    m_outputWidth = std::max(width, 1);
    m_outputHeight = std::max(height, 1);

    CreateResources();

    // TODO: Game window is being resized.
}

void ESPOverlay::SetAsOverlay()
{
    RECT overlay, target;
    GetWindowRect(m_window, &overlay);
    GetWindowRect(m_targetWindow, &target);
    //if (IsWindowVisible(m_targetWindow) == false) exit(0);
    if (overlay != target)
    {
        MoveWindow(m_window, target.left, target.top, target.right - target.left, target.bottom - target.top, FALSE);
    }
}

// Properties
void ESPOverlay::GetDefaultSize(int& width, int& height) const noexcept
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = 800;
    height = 600;
}

void ESPOverlay::GetSize(int& width, int& height) const noexcept
{
    width = m_outputWidth;
    height = m_outputHeight;
}

inline bool ESPOverlay::isTargetWindowOnForeground()
{
    return GetForegroundWindow() == m_targetWindow;
}

void ESPOverlay::drawText(float ScreenPosX, float ScreenPosY, wchar_t const* FontName, wchar_t const* TextInput, DirectX::XMVECTOR color, std::size_t SizeInPt)
{
    std::unique_ptr<DirectX::SpriteFont> fontPtr;
    fontPtr = std::make_unique<DirectX::SpriteFont>(m_d3dDevice.Get(), FontName);
    Vector2 ScreenPos, Origin;
    ScreenPos = { ScreenPosX, ScreenPosY };
    Origin = fontPtr->MeasureString(TextInput);
    fontPtr->DrawString(this->m_spriteBatch.get(), TextInput, ScreenPos, color, 0.f, Origin);
}
/// <summary>
/// Draws a filled box on the screen
/// </summary>
/// <param name="ScreenPosX">Screen pos(X) in pixels at wich the box upper left corner is to be drawn</param>
/// <param name="ScreenPosY">Screen pos(Y) in pixels at wich the box upper left corner is to be drawn</param>
/// <param name="width">width in pixels</param>
/// <param name="height">height in pixels</param>
/// <param name="color">color from Colors::</param>
void ESPOverlay::drawFilledBox(float ScreenPosX, float ScreenPosY, float width, float height, DirectX::XMVECTOR color)
{
    if (ScreenPosX + delta < 0 || ScreenPosY + delta < 0 || width < 0 || height < 0) /*throw(std::logic_error("Invalid coordinates! 0<ScreenPosX<ScreenWidth and 0<ScreenPosY<ScreenHeight etc. "));*/
        return;
    /// <summary>
    /// Setting states and applying input layout / blend state and depth stencil.
    /// </summary>

    m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(m_d3dContext.Get());

    m_d3dContext->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
    m_d3dContext->OMSetDepthStencilState(m_states->DepthNone(), 0);
    m_d3dContext->RSSetState(m_states->CullNone());
    m_d3dContext->IASetInputLayout(m_inputLayout.Get());
   
    m_effect->Apply(m_d3dContext.Get());

    m_batch->Begin();

    VertexPositionColor v1(Vector3(ScreenPosX, ScreenPosY, 0.5f), color);
    VertexPositionColor v2(Vector3(ScreenPosX+width, ScreenPosY, 0.5f), color);
    VertexPositionColor v3(Vector3(ScreenPosX + width, ScreenPosY + height , 0.5f), color);
    VertexPositionColor v4(Vector3(ScreenPosX, ScreenPosY + height, 0.5f), color);

    //m_batch->DrawTriangle(v1, v2, v3);
    m_batch->DrawQuad(v1, v2, v3, v4);

    m_batch->End();

}

/// <summary>
/// Draws a filled box on the screen
/// </summary>
/// <param name="ScreenPosX">Screen pos(X) in pixels at wich the box upper left corner is to be drawn</param>
/// <param name="ScreenPosY">Screen pos(Y) in pixels at wich the box upper left corner is to be drawn</param>
/// <param name="width">width in pixels</param>
/// <param name="height">height in pixels</param>
/// <param name="Thickness">thickness in pixels</param>
/// <param name="color">color from Colors::</param>
void ESPOverlay::drawEmptyBox(float ScreenPosX, float ScreenPosY, float width, float height, size_t Thickness, DirectX::XMVECTOR color)
{
    drawFilledBox(ScreenPosX, ScreenPosY, width, Thickness, color);
    drawFilledBox(ScreenPosX, ScreenPosY, Thickness, height, color);
    drawFilledBox(ScreenPosX, ScreenPosY + height, width + Thickness, Thickness, color);
    drawFilledBox(ScreenPosX + width, ScreenPosY, Thickness, height, color);
}

// These are the resources that depend on the device.
void ESPOverlay::CreateDevice()
{
    UINT creationFlags = 0;

#ifdef _DEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    static const D3D_FEATURE_LEVEL featureLevels [] =
    {
        // TODO: Modify for supported Direct3D feature levels
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1,
    };

    // Create the DX11 API device object, and get a corresponding context.
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    DX::ThrowIfFailed(D3D11CreateDevice(
        nullptr,                            // specify nullptr to use the default adapter
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        creationFlags,
        featureLevels,
        _countof(featureLevels),
        D3D11_SDK_VERSION,
        device.ReleaseAndGetAddressOf(),    // returns the Direct3D device created
        &m_featureLevel,                    // returns feature level of device created
        context.ReleaseAndGetAddressOf()    // returns the device immediate context
        ));

#ifndef NDEBUG
    ComPtr<ID3D11Debug> d3dDebug;
    if (SUCCEEDED(device.As(&d3dDebug)))
    {
        ComPtr<ID3D11InfoQueue> d3dInfoQueue;
        if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue)))
        {
#ifdef _DEBUG
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
            D3D11_MESSAGE_ID hide [] =
            {
                D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
                // TODO: Add more message IDs here as needed.
            };
            D3D11_INFO_QUEUE_FILTER filter = {};
            filter.DenyList.NumIDs = _countof(hide);
            filter.DenyList.pIDList = hide;
            d3dInfoQueue->AddStorageFilterEntries(&filter);
        }
    }
#endif

    DX::ThrowIfFailed(device.As(&m_d3dDevice));
    DX::ThrowIfFailed(context.As(&m_d3dContext));

    // TODO: Initialize device dependent objects here (independent of window size).
    m_spriteBatch = std::make_unique<SpriteBatch>(m_d3dContext.Get());


    /// <summary>
    /// Initializing VS, PS, input layout etc
    /// </summary>
    m_states = std::make_unique<CommonStates>(m_d3dDevice.Get());
    m_effect = std::make_unique<BasicEffect>(m_d3dDevice.Get());
    m_effect->SetVertexColorEnabled(true);

    void const* shaderByteCode;
    size_t byteCodeLength;

    m_effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
    DX::ThrowIfFailed(
        m_d3dDevice->CreateInputLayout(VertexPositionColor::InputElements,
            VertexPositionColor::InputElementCount,
            shaderByteCode, byteCodeLength,
            m_inputLayout.ReleaseAndGetAddressOf()));


}

// Allocate all memory resources that change on a window SizeChanged event.
void ESPOverlay::CreateResources()
{
    // Clear the previous window size specific context.
    ID3D11RenderTargetView* nullViews [] = { nullptr };
    m_d3dContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
    m_renderTargetView.Reset();
    m_depthStencilView.Reset();
    m_d3dContext->Flush();

    const UINT backBufferWidth = static_cast<UINT>(m_outputWidth);
    const UINT backBufferHeight = static_cast<UINT>(m_outputHeight);
    const DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
    const DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    constexpr UINT backBufferCount = 2;

    // If the swap chain already exists, resize it, otherwise create one.
    if (m_swapChain)
    {
        HRESULT hr = m_swapChain->ResizeBuffers(backBufferCount, backBufferWidth, backBufferHeight, backBufferFormat, 0);

        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
            // If the device was removed for any reason, a new device and swap chain will need to be created.
            OnDeviceLost();

            // Everything is set up now. Do not continue execution of this method. OnDeviceLost will reenter this method 
            // and correctly set up the new device.
            return;
        }
        else
        {
            DX::ThrowIfFailed(hr);
        }
    }
    else
    {
        // First, retrieve the underlying DXGI Device from the D3D Device.
        ComPtr<IDXGIDevice1> dxgiDevice;
        DX::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

        // Identify the physical adapter (GPU or card) this device is running on.
        ComPtr<IDXGIAdapter> dxgiAdapter;
        DX::ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

        // And obtain the factory object that created it.
        ComPtr<IDXGIFactory2> dxgiFactory;
        DX::ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

        // Create a descriptor for the swap chain.
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = backBufferWidth;
        swapChainDesc.Height = backBufferHeight;
        swapChainDesc.Format = backBufferFormat;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = backBufferCount;

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
        fsSwapChainDesc.Windowed = TRUE;

        // Create a SwapChain from a Win32 window.
        DX::ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
            m_d3dDevice.Get(),
            m_window,
            &swapChainDesc,
            &fsSwapChainDesc,
            nullptr,
            m_swapChain.ReleaseAndGetAddressOf()
            ));

        // This template does not support exclusive fullscreen mode and prevents DXGI from responding to the ALT+ENTER shortcut.
        DX::ThrowIfFailed(dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER));
    }

    // Obtain the backbuffer for this window which will be the final 3D rendertarget.
    ComPtr<ID3D11Texture2D> backBuffer;
    DX::ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));

    // Create a view interface on the rendertarget to use on bind.
    DX::ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.ReleaseAndGetAddressOf()));

    // Allocate a 2-D surface as the depth/stencil buffer and
    // create a DepthStencil view on this surface to use on bind.
    CD3D11_TEXTURE2D_DESC depthStencilDesc(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL);

    ComPtr<ID3D11Texture2D> depthStencil;
    DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf()));

    CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
    DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, m_depthStencilView.ReleaseAndGetAddressOf()));

    // TODO: Initialize windows-size dependent objects here.
    m_HealthTextureScreenPos.x = backBufferWidth / 2.f;
    m_HealthTextureScreenPos.y = backBufferHeight / 2.f;
    m_fontPos.x = backBufferWidth / 2.f;
    m_fontPos.y = backBufferHeight / 2.f;
    Matrix proj = Matrix::CreateScale(2.f / float(backBufferWidth),
        -2.f / float(backBufferHeight), 1.f)
        * Matrix::CreateTranslation(-1.f, 1.f, 0.f);
    m_effect->SetProjection(proj);
}

void ESPOverlay::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.

    m_depthStencilView.Reset();
    m_renderTargetView.Reset();
    m_swapChain.Reset();
    m_d3dContext.Reset();
    m_d3dDevice.Reset();
    m_healthTexture.Reset();
    m_spriteBatch.reset();
    m_states.reset();
    m_LatoFont.reset();

    CreateDevice();

    CreateResources();
}
