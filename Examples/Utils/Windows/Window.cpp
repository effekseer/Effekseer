#include "Window.h"

namespace Utils
{

LRESULT WndProc_Win(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

WindowWin::~WindowWin()
{
	Terminate();
}

bool WindowWin::Initialize(const char* title, const Vec2I& windowSize)
{

	WNDCLASSEXA wcex;
	memset(&wcex, 0, sizeof(WNDCLASSEXA));

	wcex.cbSize = sizeof(WNDCLASSEXA);
	wcex.style = CS_CLASSDC;
	wcex.lpfnWndProc = (WNDPROC)WndProc_Win;
	wcex.lpszClassName = title;
	wcex.hInstance = GetModuleHandle(NULL);
	hInstance_ = wcex.hInstance;
	RegisterClassExA(&wcex);

	auto wflags = WS_OVERLAPPEDWINDOW;
	RECT rect;
	rect.left = rect.top = 0;
	rect.right = windowSize.X;
	rect.bottom = windowSize.Y;
	::AdjustWindowRect(&rect, wflags, false);

	hwnd_ = CreateWindowA(title, title, wflags, 100, 100, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, wcex.hInstance, NULL);

	ShowWindow(hwnd_, SW_SHOWDEFAULT);
	UpdateWindow(hwnd_);

	title_ = title;
	windowSize_ = windowSize;

	// TODO : check many things

	return true;
}

bool WindowWin::DoEvent()
{
	return false;
}

void WindowWin::Terminate()
{

#ifdef _WIN32
	if (hwnd_ != nullptr)
	{
		// DestroyWindow(hwnd_);
		UnregisterClassA(title_.c_str(), GetModuleHandle(NULL));
	}
	hwnd_ = nullptr;
#endif
}

bool WindowWin::OnNewFrame()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);

			if (msg.message == WM_QUIT)
				return false;

			DispatchMessage(&msg);

			continue;
		}
		else
		{
			break;
		}
	}

	return true;
}

void* WindowWin::GetNativePtr(int32_t index)
{
	if (index == 0)
		return hwnd_;
	if (index == 1)
		return hInstance_;

	return nullptr;
}

Vec2I WindowWin::GetWindowSize() const
{
	return windowSize_;
}

} // namespace Utils
