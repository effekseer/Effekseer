#include "Window.h"

#if defined(_WIN32)
#include "Windows/Window.h"
#endif

namespace Utils
{

std::shared_ptr<Window> Window::Create(const char* title, const Vec2I& windowSize)
{

#if defined(_WIN32)

	auto window = std::make_shared<WindowWin>();
	if (window->Initialize(title, windowSize))
	{
		return window;
	}
#endif

	return nullptr;
}

} // namespace Utils