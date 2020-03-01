#pragma once

#include "../Window.h"
#include <string>
#include <memory>

namespace Utils
{

struct WindowMac_Impl;

class WindowMac : public Window
{
private:
	std::shared_ptr<WindowMac_Impl> impl = nullptr;
    Vec2I windowSize_;
    
public:
	WindowMac() = default;

	virtual ~WindowMac() = default;

	bool Initialize(const char* title, const Vec2I& windowSize);

	bool DoEvent();

	void Terminate();

	void* GetNSWindowAsVoidPtr();
    
    bool OnNewFrame() override;
    
    void* GetNativePtr(int32_t index) override;
    
    Vec2I GetWindowSize() const override;
};

} // namespace LLGI
