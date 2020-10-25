#pragma once

#include "RenderingWindow.h"
#include <array>
#include <stdint.h>

struct GLFWwindow;

class RenderingWindowGL : public RenderingWindow
{
private:
public:
	RenderingWindowGL(std::array<int32_t, 2> windowSize, const char* title);
	~RenderingWindowGL() override = default;
};