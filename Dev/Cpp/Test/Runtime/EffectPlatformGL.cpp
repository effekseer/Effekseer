#include "EffectPlatformGL.h"
#include "../../3rdParty/stb/stb_image_write.h"

class DistortingCallbackGL : public EffekseerRenderer::DistortingCallback
{
	GLuint texture = 0;
	int32_t width_ = 0;
	int32_t height_ = 0;

public:
	DistortingCallbackGL(int width, int height)
		: width_(width)
		, height_(height)
	{
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	virtual ~DistortingCallbackGL()
	{
		glDeleteTextures(1, &texture);
	}

	virtual bool OnDistorting(EffekseerRenderer::Renderer* renderer) override
	{
		glBindTexture(GL_TEXTURE_2D, texture);
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width_, height_);
		glBindTexture(GL_TEXTURE_2D, 0);

		reinterpret_cast<::EffekseerRendererGL::Renderer*>(renderer)->SetBackground(texture);

		return true;
	}
};

EffekseerRenderer::RendererRef EffectPlatformGL::CreateRenderer()
{
	auto ret = EffekseerRendererGL::Renderer::Create(initParam_.SpriteCount, EffekseerRendererGL::OpenGLDeviceType::OpenGL3);

	ret->SetDistortingCallback(new DistortingCallbackGL(initParam_.WindowSize[0], initParam_.WindowSize[1]));

	return ret;
}

EffectPlatformGL::~EffectPlatformGL()
{
}

void EffectPlatformGL::InitializeDevice(const EffectPlatformInitializingParameter& param)
{
}

void EffectPlatformGL::BeginRendering()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawPixels(initParam_.WindowSize[0], initParam_.WindowSize[1], GL_RGBA, GL_UNSIGNED_BYTE, checkeredPattern_.data());
	glFlush();
	glFinish();
}

void EffectPlatformGL::EndRendering()
{
}

bool EffectPlatformGL::TakeScreenshot(const char* path)
{
	glFlush();
	glFinish();

	glViewport(0, 0, initParam_.WindowSize[0], initParam_.WindowSize[1]);
	glReadBuffer(GL_BACK);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	std::vector<uint8_t> data;
	data.resize(initParam_.WindowSize[0] * initParam_.WindowSize[1] * 4);
	glReadPixels(0, 0, initParam_.WindowSize[0], initParam_.WindowSize[1], GL_RGBA, GL_UNSIGNED_BYTE, (void*)data.data());

	std::vector<uint8_t> temp = data;

	for (int32_t y = 0; y < initParam_.WindowSize[1]; y++)
	{
		for (int32_t x = 0; x < initParam_.WindowSize[0] * 4; x++)
		{
			data[x + y * initParam_.WindowSize[0] * 4] = temp[x + (initParam_.WindowSize[1] - 1 - y) * initParam_.WindowSize[0] * 4];
		}
	}

	// HACK for Geforce
	for (int32_t i = 0; i < initParam_.WindowSize[0] * initParam_.WindowSize[1]; i++)
	{
		data[i * 4 + 3] = 255;
	}

	stbi_write_png(path, initParam_.WindowSize[0], initParam_.WindowSize[1], 4, data.data(), initParam_.WindowSize[0] * 4);

	return true;
}
