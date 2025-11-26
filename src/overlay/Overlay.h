#pragma once

#include <d3d11.h>
#include <dxgi.h>

#include <thread>
#include <atomic>
#include <functional>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

class CheatOverlay
{
private:
    bool CreateDevice();
    void CreateOverlay(const char* windowTitle);
    bool CreateImGui();

    void DestroyDevice();
    void DestroyOverlay();
    void DestroyImGui();

    void UpdateWindowStyle();

    void RenderThreadLoop();
public:
    ~CheatOverlay() {
        StopRenderThread();
        DestroyDevice();
        DestroyOverlay();
        DestroyImGui();
    }

    void Initialize(const char* windowTitle);

    void StartRender();
    void RenderFrame();
    void EndRender();

    bool IsWindowInForeground(HWND window) { return GetForegroundWindow() == window; }
    void SetForeground(HWND window);

    bool IsRunning() { return this->g_Running.load(); }
    bool IsRenderingMenu() { return this->m_ShowMenu; }

    void StartRenderThread();
    void StopRenderThread();

    std::function<void()> m_RenderCallback;
private:
    std::atomic<bool> g_Running = false;
    std::atomic<bool> g_RenderThreadRunning = false;

    std::thread m_RenderThread;

    bool m_ShowMenu = false;
    bool m_PrevMenuRenderState = false;

    HWND m_HWND;
    WNDCLASSEX m_WindowClassEx;

    ID3D11Device* m_Device = nullptr;
    ID3D11DeviceContext* m_DeviceContext = nullptr;
    IDXGISwapChain* m_SwapChain = nullptr;
    ID3D11RenderTargetView* m_RenderTargetView = nullptr;

    ID3D11VertexShader* m_LineVS = nullptr;
    ID3D11PixelShader* m_LinePS = nullptr;
    ID3D11InputLayout* m_LineLayout = nullptr;
    ID3D11Buffer* m_LineVertexBuffer = nullptr;
};
