#include "Overlay.h"

#include <dwmapi.h>
#include <windowsx.h>
#include <stdio.h>

#include <string>
#include "../cheat/config/Config.h"
#include <d3dcompiler.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK window_procedure(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(window, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_NCHITTEST:
	{
		const LONG borderWidth = GetSystemMetrics(SM_CXSIZEFRAME);
		const LONG titleBarHeight = GetSystemMetrics(SM_CYCAPTION);

		POINT cursorPos = { GET_X_LPARAM(wParam), GET_Y_LPARAM(lParam) };
		RECT windowRect;
		GetWindowRect(window, &windowRect);

		if (cursorPos.y >= windowRect.top && cursorPos.y < windowRect.top + titleBarHeight)
			return HTCAPTION;

		break;
	}
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_CLOSE:
		return 0;
	}

	return DefWindowProc(window, msg, wParam, lParam);
}

void CheatOverlay::Initialize(const char* windowTitle)
{
	this->CreateOverlay(windowTitle);
	this->CreateDevice();
	this->CreateImGui();

	this->g_Running = true;
}

void CheatOverlay::StartRender()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (GetAsyncKeyState(VK_INSERT) & 1) {
		this->m_PrevMenuRenderState = this->m_ShowMenu;
		this->m_ShowMenu = !this->m_ShowMenu;
		this->UpdateWindowStyle();
	}
}

DWORD colorEditFlags = ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreview;

void CheatOverlay::RenderFrame()
{
	ImGui::SetNextWindowSize({ 600, 500 });

	std::string fps = std::to_string(ImGui::GetIO().Framerate) + " fps";

	ImGui::GetForegroundDrawList()->AddText(
		{5, 5}, ImColor(1.f, 1.f, 1.f, 1.f), fps.c_str()
	);
	
	if (this->m_ShowMenu) {
		const bool drawingMenu = ImGui::Begin("Huntsman Menu", &this->m_ShowMenu, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

		if (drawingMenu != this->m_PrevMenuRenderState)
			this->UpdateWindowStyle();

		if (ImGui::BeginTabBar("Categorys"))
		{
			if (ImGui::BeginTabItem("Aim"))
			{
				ImGui::Text("Aim Bot");
				ImGui::Checkbox("Enabled", &config::aim::aimbot::b_Enabled);
				ImGui::SliderFloat("Smoothing", &config::aim::aimbot::f_Smoothing, 1.f, 20.f, "%.1f");

				ImGui::Checkbox("FOV", &config::aim::aimbot::b_FoV);
				ImGui::SliderFloat("Radius", &config::aim::aimbot::f_Radius, 2.f, 1000.f, "%.1f");

				ImGui::Checkbox("LockTarget", &config::aim::aimbot::b_LockTarget);
				ImGui::Checkbox("Dormant Check", &config::aim::aimbot::b_DormantCheck);

				ImGui::Combo("Aim Bone", &config::aim::aimbot::i_TargetBoneIndex, config::aim::aimbot::a_TargetBoneModeItems, IM_ARRAYSIZE(config::aim::aimbot::a_TargetBoneModeItems), 5);

				ImGui::Checkbox("Draw Circle", &config::aim::aimbot::render::b_DrawCircle);
				ImGui::SliderFloat("Circle Thickness", &config::aim::aimbot::render::f_CircleThickness, 1.f, 5.f, "%.1f");
				ImGui::ColorEdit4("Circle Color", config::aim::aimbot::render::f_CircleColor, colorEditFlags);

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Visuals"))
			{
				ImGui::Text("Enemy ESP");
				ImGui::Checkbox("ESP ##xx Enemy", &config::visual::esp::enemy::b_Enabled);
				ImGui::ColorEdit4("Box Color ##xx Enemy", config::visual::esp::enemy::f_BoxColor, colorEditFlags);

				ImGui::Separator();

				ImGui::Text("Team ESP");
				ImGui::Checkbox("ESP ##xx Team", &config::visual::esp::team::b_Enabled);
				ImGui::ColorEdit4("Box Color ##xx Team", config::visual::esp::team::f_BoxColor, colorEditFlags);

				ImGui::Separator();

				ImGui::SliderFloat("Box Thickness (Global)", &config::visual::esp::f_BoxThickness, 1.f, 10.f, "%.1f");

				ImGui::Separator();

				ImGui::Text("Skeleton ESP");
				ImGui::Checkbox("Enabled ##xx Skeleton", &config::visual::esp::skeleton::b_Enabled);
				ImGui::Checkbox("Head Circle ##xx Skeleton", &config::visual::esp::skeleton::b_HeadCircle);
				ImGui::ColorEdit4("Color ##xx Skeleton", config::visual::esp::skeleton::f_Color, colorEditFlags);

				ImGui::Checkbox("Skeleton Outline ##xx Skeleton", &config::visual::esp::skeleton::outline::b_Enabled);
				ImGui::SliderFloat("Outline Thickness ##xx Skeleton", &config::visual::esp::skeleton::f_Thickness, 1.f, 6.f, "%.1f");
				ImGui::ColorEdit4("Outline Color ##xx Skeleton", config::visual::esp::skeleton::outline::f_Color, colorEditFlags);

				ImGui::Separator();

				ImGui::Text("Box Outline");
				ImGui::Checkbox("Enabled ##xx Box Outline", &config::visual::esp::outline::b_Enabled);
				ImGui::ColorEdit4("Color ##xx Box Outline", config::visual::esp::outline::f_Color, colorEditFlags);
				ImGui::SliderFloat("Thickness", &config::visual::esp::outline::f_Thickness, 1.5, 10.5f, "%.1f");

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		//ImGui::Checkbox("Team Check", &config::visual::b_TeamCheck);
		//ImGui::Checkbox("Fill Box", &config::visual::b_FillBox);
		//ImGui::ColorEdit4("Box Color", config::visual::i_BoxColor);
		//ImGui::Checkbox("Skeletons", &config::visual::b_Skeletons);
		//ImGui::ColorEdit4("Skeleton Color", config::visual::i_SkeletonsColor);

		ImGui::End();
	}
}

void CheatOverlay::EndRender()
{
	ImGui::Render();

	float color[4]{ 0, 0, 0, 0 };

	this->m_DeviceContext->OMSetRenderTargets(1, &this->m_RenderTargetView, nullptr);
	this->m_DeviceContext->ClearRenderTargetView(this->m_RenderTargetView, color);

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	this->m_SwapChain->Present(0U, 0U);
}

bool CheatOverlay::CreateDevice()
{
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));

	sd.BufferCount = 2;

	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;

	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	sd.BufferDesc.RefreshRate.Numerator = 240; // TODO: Change in the future
	sd.BufferDesc.RefreshRate.Denominator = 1;

	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	sd.OutputWindow = this->m_HWND;

	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;

	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

	UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if _DEBUG
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT result = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		deviceFlags,
		featureLevelArray,
		2,
		D3D11_SDK_VERSION,
		&sd,
		&this->m_SwapChain,
		&m_Device,
		&featureLevel,
		&this->m_DeviceContext
	);

	if (result == DXGI_ERROR_UNSUPPORTED) {
		result = D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_WARP,
			nullptr,
			0U,
			featureLevelArray,
			2, D3D11_SDK_VERSION,
			&sd,
			&this->m_SwapChain,
			&m_Device,
			&featureLevel,
			&this->m_DeviceContext);
	}

	if (result != S_OK) {
		return false;
	}

	ID3D11Texture2D* back_buffer{ nullptr };
	this->m_SwapChain->GetBuffer(0U, IID_PPV_ARGS(&back_buffer));

	if (back_buffer)
	{
		m_Device->CreateRenderTargetView(back_buffer, nullptr, &this->m_RenderTargetView);
		back_buffer->Release();
		return true;
	}

	printf("[-] Failed to create Device\n");
	return false;
}

void CheatOverlay::CreateOverlay(const char* windowTitle)
{
	this->m_WindowClassEx.cbSize = sizeof(this->m_WindowClassEx);
	this->m_WindowClassEx.style = CS_CLASSDC;
	this->m_WindowClassEx.lpfnWndProc = window_procedure;
	this->m_WindowClassEx.hInstance = GetModuleHandleA(0);
	this->m_WindowClassEx.lpszClassName = "Huntsman";

	RegisterClassEx(&this->m_WindowClassEx);

	//const std::string title{ windowTitle };

	//int requiredSize = MultiByteToWideChar(
	//	CP_UTF8, 0,
	//	title.data(), (int)title.size(),
	//	nullptr, 0
	//);

	//std::wstring wide(requiredSize, 0);

	//MultiByteToWideChar(
	//	CP_UTF8, 0,
	//	title.data(), (int)title.size(),
	//	wide.data(), requiredSize
	//);

	this->m_HWND = CreateWindowEx(
		WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW,
		this->m_WindowClassEx.lpszClassName,
		windowTitle,
		WS_POPUP,
		0,
		0,
		GetSystemMetrics(SM_CXSCREEN),
		GetSystemMetrics(SM_CYSCREEN),
		NULL,
		NULL,
		this->m_WindowClassEx.hInstance,
		NULL
	);

	SetLayeredWindowAttributes(this->m_HWND, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);

	RECT client_area{};
	RECT window_area{};

	GetClientRect(this->m_HWND, &client_area);
	GetWindowRect(this->m_HWND, &window_area);

	POINT diff{};
	ClientToScreen(this->m_HWND, &diff);

	const MARGINS margins{
		window_area.left + (diff.x - window_area.left),
		window_area.top + (diff.y - window_area.top),
		client_area.right,
		client_area.bottom
	};

	DwmExtendFrameIntoClientArea(this->m_HWND, &margins);

	ShowWindow(this->m_HWND, SW_SHOW);
	UpdateWindow(this->m_HWND);

	printf("[+] Overlay Created\n");
}

bool CheatOverlay::CreateImGui()
{
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

	if (!ImGui_ImplWin32_Init(this->m_HWND)) {
		printf("[-] Failed ImGui_ImplWin32_Init\n");
		return false;
	}

	if (!ImGui_ImplDX11_Init(this->m_Device, this->m_DeviceContext)) {
		printf("[-] Failed ImGui_ImplDX11_Init\n");
		return false;
	}

	printf("[+] ImGui Initialized\n");
	return true;
}

void CheatOverlay::DestroyDevice()
{
	if (this->m_Device)
	{
		this->m_Device->Release();
		this->m_DeviceContext->Release();
		this->m_SwapChain->Release();
		this->m_RenderTargetView->Release();
	}
}

void CheatOverlay::DestroyOverlay()
{
	DestroyWindow(this->m_HWND);
	UnregisterClass(this->m_WindowClassEx.lpszClassName, this->m_WindowClassEx.hInstance);
}

void CheatOverlay::DestroyImGui()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void CheatOverlay::UpdateWindowStyle()
{
	if (!this->m_HWND)
		return;

	LONG style = WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT;

	if (!this->m_ShowMenu)
		style |= WS_EX_TOPMOST | WS_EX_LAYERED;

	SetWindowLong(this->m_HWND, GWL_EXSTYLE, style);
}

void CheatOverlay::RenderThreadLoop()
{
	timeBeginPeriod(1);

	while (g_RenderThreadRunning)
    {
        StartRender();
        
		if (this->m_RenderCallback)
			this->m_RenderCallback();

        EndRender();

        std::this_thread::sleep_for(std::chrono::microseconds(200));
    }

    timeEndPeriod(1);
}

void CheatOverlay::SetForeground(HWND window)
{
}

void CheatOverlay::StartRenderThread()
{
	if (g_RenderThreadRunning)
		return;

	g_RenderThreadRunning = true;
	g_Running = true;

	m_RenderThread = std::thread(&CheatOverlay::RenderThreadLoop, this);
}

void CheatOverlay::StopRenderThread()
{
	g_Running = false;
	g_RenderThreadRunning = false;

	if (m_RenderThread.joinable())
		m_RenderThread.join();
}
