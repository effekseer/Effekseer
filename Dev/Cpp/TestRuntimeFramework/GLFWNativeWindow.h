#pragma once

#include <cstdint>

struct GLFWwindow;

// Keep platform headers (and their macros) out of the test framework interface.
// Index 0 is the window on Windows/macOS and the display on X11.
// Index 1 is the module handle on Windows and the window on X11.
void* GetGLFWNativeWindowPointer(GLFWwindow* window, int32_t index);
