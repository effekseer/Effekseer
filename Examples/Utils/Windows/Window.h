#pragma once

#include "../Window.h"
#include <windows.h>
#include <string>

	namespace Utils
	{

class WindowWin : public Window
{
private:
	HWND hwnd_ = nullptr;
	HINSTANCE hInstance_ = nullptr;
	std::string title_;
	Vec2I windowSize_;

public:
	WindowWin() = default;

	virtual ~WindowWin();

	bool Initialize(const char* title, const Vec2I& windowSize);

	bool DoEvent();

	void Terminate();

	bool OnNewFrame() override;

	void* GetNativePtr(int32_t index) override;

	Vec2I GetWindowSize() const;

	HWND GetHandle() const { return hwnd_; }

	HINSTANCE GetInstance() const { return hInstance_; }
};

} // namespace LLGI