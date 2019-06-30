
#ifndef	__WINDOW_H__
#define	__WINDOW_H__

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

#include "common.h"

#if _WIN32
#include <windows.h>
HWND GetHandle();

#elif __APPLE__

#else
#include <X11/Xlib.h>
#include <X11/Xutil.h>
Display* GetDisplay();
Window* GetWindow();
#endif

void InitWindow(int width, int height);
bool DoWindowEvent();
void ExitWindow();

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

#endif
