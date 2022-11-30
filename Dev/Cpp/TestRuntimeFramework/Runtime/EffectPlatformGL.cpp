#if !defined(__APPLE__)
#define GLEW_STATIC
typedef char GLchar;
#include <GL/glew.h>
#endif

#include "EffectPlatformGL.h"
#include "../../3rdParty/stb/stb_image_write.h"
#include <EffekseerRendererGL.h>

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
	glDeleteFramebuffers(1, &frameBuffer_);
}

void EffectPlatformGL::InitializeDevice(const EffectPlatformInitializingParameter& param)
{
	graphicsDevice_ = EffekseerRendererGL::CreateGraphicsDevice(EffekseerRendererGL::OpenGLDeviceType::OpenGL3);
	Effekseer::Backend::TextureParameter textureParam;
	textureParam.Dimension = 2;
	textureParam.Format = Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM;
	textureParam.Size[0] = initParam_.WindowSize[0];
	textureParam.Size[1] = initParam_.WindowSize[1];

	Effekseer::CustomVector<uint8_t> data;
	data.resize(checkeredPattern_.size() * 4);

	data.assign(reinterpret_cast<uint8_t*>(checkeredPattern_.data()), reinterpret_cast<uint8_t*>(checkeredPattern_.data() + checkeredPattern_.size()));

	checkedTexture_ = graphicsDevice_->CreateTexture(textureParam, data);

	glGenFramebuffers(1, &frameBuffer_);
}

void EffectPlatformGL::BeginRendering()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const auto prop = EffekseerRendererGL::GetTextureProperty(checkedTexture_);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer_);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, prop.Buffer, 0);
	glBlitFramebuffer(0, 0, initParam_.WindowSize[0], initParam_.WindowSize[1], 0, 0, initParam_.WindowSize[0], initParam_.WindowSize[1], GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
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
