#pragma once

#include <Effekseer.h>
#include <array>
#include <stdint.h>
#include <string>
#include <vector>

struct GLFWwindow;

class ShaderContainer
{
public:
	std::vector<std::string> VertexCodes;
	std::vector<std::string> PixelCodes;
	std::vector<uint8_t> VertexData;
	std::vector<uint8_t> PixelData;

	void InitAsBinary(const void* vsData, size_t vsSize, const void* psData, size_t psSize)
	{
		const uint8_t* vp = static_cast<const uint8_t*>(vsData);
		const uint8_t* pp = static_cast<const uint8_t*>(psData);

		VertexData.assign(vp, vp + vsSize);
		PixelData.assign(pp, pp + psSize);
	}
};

class RenderingEnvironment
{
private:
	GLFWwindow* glfwWindow_ = nullptr;
	bool isOpenGLMode_ = false;

protected:
	Effekseer::Backend::GraphicsDeviceRef graphicsDevice_;
	Effekseer::Backend::RenderPassRef renderPass_;

public:
	RenderingEnvironment(bool isOpenGLMode, std::array<int32_t, 2> windowSize, const char* title);
	virtual ~RenderingEnvironment();
	virtual void Present();
	virtual bool DoEvent();

	Effekseer::Backend::ShaderRef CreateShader(std::map<std::string, ShaderContainer> shaders, Effekseer::Backend::UniformLayoutRef layout);

	Effekseer::Backend::GraphicsDeviceRef GetGraphicsDevice()
	{
		return graphicsDevice_;
	}

	Effekseer::Backend::RenderPassRef GetScreenRenderPass()
	{
		return renderPass_;
	}

	void* GetNativePtr(int32_t index);
};