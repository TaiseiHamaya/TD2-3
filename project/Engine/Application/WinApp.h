#pragma once

#include <windows.h>

#include <Library/Utility/Template/SingletonInterface.h>

enum WindowStyle {
	Window = WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
	Fullscreen,//= WS_ & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
	Borderless,
};

class WinApp final : public SingletonInterface<WinApp> {
	friend class SingletonInterface<WinApp>;

private:
	WinApp() = default;
	~WinApp();

public:
	WinApp(const WinApp&) = delete;
	WinApp& operator=(const WinApp&) = delete;
	WinApp(WinApp&&) = delete;
	WinApp& operator=(WinApp&&) = delete;

public:
	static void Initialize(DWORD windowConfig = WindowStyle::Window);
	static void BeginFrame();
	static void EndFrame();
	static void Finalize();

	static void ShowAppWindow();
	static bool IsEndApp();
	static void ProcessMessage();

public:
	static HWND GetWndHandle() noexcept { return GetInstance().hWnd; };
	static HANDLE GetProcessHandle() noexcept { return GetInstance().hProcess; };
	static HINSTANCE GetInstanceHandle() noexcept { return GetInstance().hInstance; };

private:
	void initialize_application(DWORD windowConfig);

	void wait_frame();

private:
	static inline bool isInitialized{ false };

private:
	bool isEndApp{ false };
	HWND hWnd{ nullptr };
	HINSTANCE hInstance{ nullptr };
	HANDLE hProcess{ nullptr };

	MSG msg{};

	bool isFullscreen{ false };

#ifdef DEBUG_FEATURES_ENABLE
public:
	static bool IsStopUpdate();

private:
	bool isStopUpdate{ false };
	bool isPassedPause{ false };
#endif // _DEBUG
};

