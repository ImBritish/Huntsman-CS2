#include <iostream>

#include <thread>

#include "overlay/Overlay.h"
#include "cheat/sdk/CS2Game.h"

#define SLEEP(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms));

INT APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT showConsole) {
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	freopen_s(&f, "CONOUT$", "w", stderr);

	ShowWindow(GetConsoleWindow(), SW_SHOW);

	Memory mem { "cs2.exe" };

	if (!mem.ProcessIsOpen("cs2.exe")) {
		printf("Waiting for cs2.exe to start...\n");
	}

	while (!mem.ProcessIsOpen("cs2.exe"))
	{
		// Blocking Main Thread
	}

	printf("CS2 detected! Starting overlay...\n");

	//SLEEP(1250);

	uintptr_t client = mem.GetBase("client.dll");

	//SLEEP(3000);

	//ShowWindow(GetConsoleWindow(), SW_HIDE);

	g_Instance = std::make_unique<CS2Game>(&mem, &client);

	CheatOverlay overlay;

	overlay.Initialize("Huntsman");

	overlay.m_RenderCallback = [&]()
		{
			if (mem.InForeground("Counter-Strike 2") ||
				(mem.InForeground("Huntsman") && overlay.IsRenderingMenu()))
			{
				g_Instance->Render();
				overlay.RenderFrame();
			}
		};

	overlay.StartRenderThread();

	g_Instance->StartEntityCollectionThread();

	while (overlay.IsRunning())
	{
		MSG msg;
		while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		timeBeginPeriod(1);

		g_Instance->Tick();

		if (!mem.ProcessIsOpen("cs2.exe"))
			break;

		std::this_thread::sleep_for(std::chrono::microseconds(200));

		timeEndPeriod(1);
	}

	overlay.StopRenderThread();

	g_Instance->GetCheatManager()->Clear();

	return 0;
}