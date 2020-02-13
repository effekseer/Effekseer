#include "EffectPlatformGL.h"
#include "../../3rdParty/stb/stb_image_write.h"

class DistortingCallbackGL : public EffekseerRenderer::DistortingCallback
{
	::EffekseerRendererGL::Renderer* renderer = nullptr;
	GLuint texture = 0;
	int32_t width_ = 0;
	int32_t height_ = 0;

public:
	DistortingCallbackGL(::EffekseerRendererGL::Renderer* renderer, int width, int height)
		: renderer(renderer), width_(width), height_(height)
	{
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	virtual ~DistortingCallbackGL() { glDeleteTextures(1, &texture); }

	virtual bool OnDistorting() override
	{
		glBindTexture(GL_TEXTURE_2D, texture);
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width_, height_);
		glBindTexture(GL_TEXTURE_2D, 0);

		renderer->SetBackground(texture);

		return true;
	}
};

EffekseerRenderer::Renderer* EffectPlatformGL::CreateRenderer()
{
	auto ret = EffekseerRendererGL::Renderer::Create(2000, EffekseerRendererGL::OpenGLDeviceType::OpenGL3);

	ret->SetDistortingCallback(new DistortingCallbackGL(ret, 1280, 720));

	return ret;
}

EffectPlatformGL::~EffectPlatformGL() {}

void EffectPlatformGL::InitializeDevice(const EffectPlatformInitializingParameter& param)
{
	// flip checker pattern
	for (size_t y = 0; y < 720 / 2; y++)
	{
		for (size_t x = 0; x < 1280; x++)
		{
			std::swap(checkeredPattern_[x + y * 1280], checkeredPattern_[x + (720 - 1 - y) * 1280]);
		}
	}
}

void EffectPlatformGL::BeginRendering()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawPixels(1280, 720, GL_RGBA, GL_UNSIGNED_BYTE, checkeredPattern_.data());
	glFlush();
	glFinish();
}

void EffectPlatformGL::EndRendering() {}

bool EffectPlatformGL::TakeScreenshot(const char* path)
{
	glFlush();
	glFinish();

	glViewport(0, 0, 1280, 720);
	glReadBuffer(GL_BACK);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	std::vector<uint8_t> data;
	data.resize(1280 * 720 * 4);
	glReadPixels(0, 0, 1280, 720, GL_RGBA, GL_UNSIGNED_BYTE, (void*)data.data());

	std::vector<uint8_t> temp = data;

	for (int32_t y = 0; y < 720; y++)
	{
		for (int32_t x = 0; x < 1280 * 4; x++)
		{
			data[x + y * 1280 * 4] = temp[x + (720 - 1 - y) * 1280 * 4];
		}
	}

	// HACK for Geforce
	for (int32_t i = 0; i < 1280 * 720; i++)
	{
		data[i * 4 + 3] = 255;
	}

	stbi_write_png(path, 1280, 720, 4, data.data(), 1280 * 4);

	return true;
}
