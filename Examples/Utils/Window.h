#pragma once

#include <memory>
#include <stdint.h>
#include <stdio.h>

namespace Utils
{

struct Vec2I
{
	int32_t X;
	int32_t Y;
};

class Window
{
public:
	Window() = default;

	virtual ~Window() = default;

	/**
		@brief	called when platform calls newframe, if it need to exit, return false
	*/
	virtual bool OnNewFrame() = 0;

	/**
		@brief return native window ptr
	*/
	virtual void* GetNativePtr(int32_t index) = 0;

	/**
	 * @brief get window size
	 */
	virtual Vec2I GetWindowSize() const = 0;

	/**
	 @brief Create an window
	*/
	static std::shared_ptr<Window> Create(const char* title, const Vec2I& windowSize);
};

} // namespace Utils
