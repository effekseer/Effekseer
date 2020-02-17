#include "Window.h"

namespace Utils
{

bool WindowLinux::Initialize(const char* title, const Vec2I& windowSize)
{

	display_ = XOpenDisplay(NULL);
	if (display_ == nullptr)
		return false;

	unsigned long background = WhitePixel(display_, 0);
	unsigned long foreground = BlackPixel(display_, 0);

	window_ = XCreateSimpleWindow(display_, DefaultRootWindow(display_), 0, 0, windowSize.X, windowSize.Y, 0, 0, background);

	// specify a title
	const char* titles_ptr[] = {title};

	XTextProperty title_prop;
	setlocale(LC_ALL, "");
	XmbTextListToTextProperty(display_, const_cast<char**>(titles_ptr), 1, XCompoundTextStyle, &title_prop);
	XSetWMName(display_, window_, &title_prop);

	XmbTextListToTextProperty(display_, const_cast<char**>(titles_ptr), 1, XCompoundTextStyle, &title_prop);
	XSetWMIconName(display_, window_, &title_prop);

	gc_ = XCreateGC(display_, window_, 0, 0);

	XSetBackground(display_, gc_, background);
	XSetForeground(display_, gc_, foreground);

	XMapRaised(display_, window_);

	XSelectInput(display_, window_, StructureNotifyMask | ButtonPressMask);

	wm_delete_window_ = XInternAtom(display_, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(display_, window_, &wm_delete_window_, 1);

	windowSize_ = windowSize;
	return true;
}

WindowLinux::~WindowLinux() { Terminate(); }

bool WindowLinux::DoEvent()
{

	if (closed_)
		return false;

	while (XPending(display_))
	{
		XEvent event;
		XNextEvent(display_, &event);

		switch (event.type)
		{
		case ClientMessage:
		{
			if ((Atom)event.xclient.data.l[0] == wm_delete_window_)
				return false;
		}
		break;

		case (DestroyNotify):
		{
			closed_ = true;
			return false;
		}
		break;
		}
	}

	return true;
}

void WindowLinux::Terminate()
{
	if (!closed_)
	{
		XDestroyWindow(display_, window_);
		closed_ = true;
	}

	XCloseDisplay(display_);
}

bool WindowLinux::OnNewFrame() { return DoEvent(); }

void* WindowLinux::GetNativePtr(int32_t index)
{
	if (index == 0)
		return GetDisplay();
	if (index == 1)
		return reinterpret_cast<void*>(GetWindow());
	return nullptr;
}

Vec2I WindowLinux::GetWindowSize() const { return windowSize_; }

} // namespace LLGI