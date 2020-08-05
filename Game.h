//
// Game.h
//

#pragma once
#include "StepTimer.h"
#include "ESP.h"
#include "ENTITY.h"
#include "Memory.h"
#include "Offsets.h"
#include "Defines.h"


// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class ESPOverlay
{
public:

    int                                             m_outputWidth;
    int                                             m_outputHeight;
    bool b_isWindowAttached = false;
    std::wstring AttachedWindow=L"";
    std::vector<ENTITY> players;
    ESPOverlay() noexcept;
    ~ESPOverlay() = default;

    ESPOverlay(ESPOverlay&&) = default;
    ESPOverlay& operator= (ESPOverlay&&) = default;

    ESPOverlay(ESPOverlay const&) = delete;
    ESPOverlay& operator= (ESPOverlay const&) = delete;

    // Initialization and management
    void Initialize(HWND window);
    void AttachWindow(std::wstring targetWindowName);
    // Basic game loop
    void Tick();

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowSizeChanged(int width, int height);
    void SetAsOverlay();

    // Properties
    void GetDefaultSize( int& width, int& height ) const noexcept;
    void GetSize( int& width, int& height ) const noexcept;
    bool isTargetWindowOnForeground();

    //Window Figures and Text drawing
    void drawText(float ScreenPosX, float ScreenPosY, wchar_t const* FontName, wchar_t const* TextInput, DirectX::XMVECTOR color, std::size_t SizeInPt);
    void drawFilledBox(float ScreenPosX, float ScreenPosY, float width, float height, DirectX::XMVECTOR color);
    void drawEmptyBox(float ScreenPosX, float ScreenPosY, float width, float height,size_t Thickness, DirectX::XMVECTOR color);
private:

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear(DirectX::XMVECTORF32 = DirectX::Colors::Black);
    void Present();

    void CreateDevice();
    void CreateResources();

    void OnDeviceLost();

    // Device resources.
    HWND                                            m_window;
    HWND                                            m_targetWindow;

    D3D_FEATURE_LEVEL                               m_featureLevel;
    Microsoft::WRL::ComPtr<ID3D11Device1>           m_d3dDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext1>    m_d3dContext;

    Microsoft::WRL::ComPtr<IDXGISwapChain1>         m_swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_depthStencilView;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
    std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;


    std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
    DirectX::SimpleMath::Vector2 m_HealthTextureScreenPos;
    DirectX::SimpleMath::Vector2 m_HealthTextureOrigin;
    Microsoft::WRL::ComPtr< ID3D11ShaderResourceView> m_healthTexture;
    std::unique_ptr<DirectX::CommonStates> m_states;
    std::unique_ptr<DirectX::SpriteFont> m_LatoFont;
    std::unique_ptr<DirectX::BasicEffect> m_effect;
    constexpr static float delta = 3;
    DirectX::SimpleMath::Vector2 m_fontPos;

    // Rendering loop timer.
    DX::StepTimer                                   m_timer;
};
