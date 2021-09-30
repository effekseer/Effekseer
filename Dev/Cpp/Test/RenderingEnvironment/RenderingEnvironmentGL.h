#pragma once

#include "RenderingEnvironment.h"
#include <array>
#include <stdint.h>

struct GLFWwindow;

class RenderingEnvironmentGL : public RenderingEnvironment
{
private:
public:
	RenderingEnvironmentGL(std::array<int32_t, 2> windowSize, const char* title);
	~RenderingEnvironmentGL() override = default;
};