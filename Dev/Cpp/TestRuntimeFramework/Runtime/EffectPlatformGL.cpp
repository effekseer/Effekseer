#include "EffectPlatformGL.h"
#include "../../3rdParty/stb/stb_image_write.h"
#include <EffekseerRendererGL.h>
#include <EffekseerRendererGL/EffekseerRendererGL.GLExtension.h>
#include <OpenGLExtensions.h>
namespace GL = EffekseerRendererGL::GLExt;

namespace
{

const auto ground_vs_gl = R"(
#version 330

layout(location = 0) in vec4 input_Position;
layout(location = 1) in vec2 input_WorldXZ;
out vec2 output_WorldXZ;

void main()
{
	gl_Position = input_Position;
	output_WorldXZ = input_WorldXZ;
}
)";

const auto ground_ps_gl = R"(
#version 330

in vec2 output_WorldXZ;
layout(location = 0) out vec4 output_Color;

void main()
{
	float checker = fract((floor(output_WorldXZ.x) + floor(output_WorldXZ.y)) * 0.5);
	vec3 darkColor = vec3(0.24, 0.32, 0.27);
	vec3 brightColor = vec3(0.39, 0.50, 0.42);
	vec3 color = mix(darkColor, brightColor, step(0.5, checker));
	float distanceFade = clamp(length(output_WorldXZ) * 0.025, 0.0, 1.0);
	color *= 1.0 - distanceFade * 0.35;
	output_Color = vec4(color, 1.0);
}
)";

const auto ground_depth_ps_gl = R"(
#version 330

in vec2 output_WorldXZ;
layout(location = 0) out vec4 output_Color;

void main()
{
	output_Color = vec4(gl_FragCoord.z, 1.0, 1.0, 1.0);
}
)";

GLuint CompileShader(GLenum type, const char* code)
{
	auto shader = GL::glCreateShader(type);
	GL::glShaderSource(shader, 1, &code, nullptr);
	GL::glCompileShader(shader);

	GLint status = 0;
	GL::glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == 0)
	{
		GL::glDeleteShader(shader);
		return 0;
	}

	return shader;
}

GLuint CreateProgram(const char* vsCode, const char* psCode)
{
	auto vs = CompileShader(GL_VERTEX_SHADER, vsCode);
	auto ps = CompileShader(GL_FRAGMENT_SHADER, psCode);
	if (vs == 0 || ps == 0)
	{
		if (vs != 0)
		{
			GL::glDeleteShader(vs);
		}
		if (ps != 0)
		{
			GL::glDeleteShader(ps);
		}
		return 0;
	}

	auto program = GL::glCreateProgram();
	GL::glAttachShader(program, vs);
	GL::glAttachShader(program, ps);
	GL::glLinkProgram(program);
	GL::glDeleteShader(vs);
	GL::glDeleteShader(ps);

	GLint status = 0;
	GL::glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == 0)
	{
		GL::glDeleteProgram(program);
		return 0;
	}

	return program;
}

} // namespace

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
		Effekseer::OpenGLHelper::glCopyTexSubImage2D(
			GL_TEXTURE_2D, 0, 0, 0, 0, 0, width_, height_);
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
	ReleaseGroundResources();
	GL::glDeleteFramebuffers(1, &frameBuffer_);
}

void EffectPlatformGL::InitializeDevice(const EffectPlatformInitializingParameter& param)
{
	graphicsDevice_ = EffekseerRendererGL::CreateGraphicsDevice(EffekseerRendererGL::OpenGLDeviceType::OpenGL3);

	UpdateBackgroundTexture();

	Effekseer::OpenGLHelper::Initialize();

	GL::glGenFramebuffers(1, &frameBuffer_);
}

bool EffectPlatformGL::CreateGroundResources()
{
	if (groundDepthColorTexture_ != 0)
	{
		return true;
	}

	groundProgram_ = CreateProgram(ground_vs_gl, ground_ps_gl);
	groundDepthProgram_ = CreateProgram(ground_vs_gl, ground_depth_ps_gl);
	if (groundProgram_ == 0 || groundDepthProgram_ == 0)
	{
		ReleaseGroundResources();
		return false;
	}

	GL::glGenBuffers(1, &groundVertexBuffer_);
	GL::glGenBuffers(1, &groundIndexBuffer_);
	const auto indices = CreateGroundPlaneIndices();
	GL::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, groundIndexBuffer_);
	GL::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);
	GL::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenTextures(1, &groundDepthColorTexture_);
	glBindTexture(GL_TEXTURE_2D, groundDepthColorTexture_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, initParam_.WindowSize[0], initParam_.WindowSize[1], 0, GL_RGBA, GL_FLOAT, nullptr);

	glGenTextures(1, &groundDepthTexture_);
	glBindTexture(GL_TEXTURE_2D, groundDepthTexture_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, initParam_.WindowSize[0], initParam_.WindowSize[1], 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	GL::glGenFramebuffers(1, &groundDepthFrameBuffer_);
	GL::glBindFramebuffer(GL_FRAMEBUFFER, groundDepthFrameBuffer_);
	GL::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, groundDepthColorTexture_, 0);
	GL::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, groundDepthTexture_, 0);
	const GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
	GL::glDrawBuffers(1, drawBuffers);
	GL::glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

void EffectPlatformGL::ReleaseGroundResources()
{
	groundDepthTextureForEffekseer_.Reset();
	if (groundIndexBuffer_ != 0)
	{
		GL::glDeleteBuffers(1, &groundIndexBuffer_);
		groundIndexBuffer_ = 0;
	}
	if (groundVertexBuffer_ != 0)
	{
		GL::glDeleteBuffers(1, &groundVertexBuffer_);
		groundVertexBuffer_ = 0;
	}
	if (groundProgram_ != 0)
	{
		GL::glDeleteProgram(groundProgram_);
		groundProgram_ = 0;
	}
	if (groundDepthProgram_ != 0)
	{
		GL::glDeleteProgram(groundDepthProgram_);
		groundDepthProgram_ = 0;
	}
	if (groundDepthTexture_ != 0)
	{
		glDeleteTextures(1, &groundDepthTexture_);
		groundDepthTexture_ = 0;
	}
	if (groundDepthColorTexture_ != 0)
	{
		glDeleteTextures(1, &groundDepthColorTexture_);
		groundDepthColorTexture_ = 0;
	}
	if (groundDepthFrameBuffer_ != 0)
	{
		GL::glDeleteFramebuffers(1, &groundDepthFrameBuffer_);
		groundDepthFrameBuffer_ = 0;
	}
	usesGpuGroundDepth_ = false;
}

void EffectPlatformGL::UpdateGroundVertexBuffer()
{
	const auto vertices = CreateGroundPlaneVertices();

	GL::glBindBuffer(GL_ARRAY_BUFFER, groundVertexBuffer_);
	GL::glBufferData(GL_ARRAY_BUFFER, sizeof(GroundPlaneVertex) * vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);
	GL::glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void EffectPlatformGL::DrawGround(GLuint program)
{
	GL::glUseProgram(program);
	GL::glBindBuffer(GL_ARRAY_BUFFER, groundVertexBuffer_);
	GL::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, groundIndexBuffer_);
	GL::glEnableVertexAttribArray(0);
	GL::glEnableVertexAttribArray(1);
	GL::glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GroundPlaneVertex), reinterpret_cast<const void*>(0));
	GL::glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GroundPlaneVertex), reinterpret_cast<const void*>(sizeof(float) * 4));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
	GL::glDisableVertexAttribArray(1);
	GL::glDisableVertexAttribArray(0);
	GL::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	GL::glBindBuffer(GL_ARRAY_BUFFER, 0);
	GL::glUseProgram(0);
}

void EffectPlatformGL::UpdateBackgroundTexture()
{
	Effekseer::Backend::TextureParameter textureParam;
	textureParam.Dimension = 2;
	textureParam.Format = Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM;
	textureParam.Size[0] = initParam_.WindowSize[0];
	textureParam.Size[1] = initParam_.WindowSize[1];

	Effekseer::CustomVector<uint8_t> data;
	data.resize(checkeredPattern_.size() * 4);

	data.assign(reinterpret_cast<uint8_t*>(checkeredPattern_.data()), reinterpret_cast<uint8_t*>(checkeredPattern_.data() + checkeredPattern_.size()));

	checkedTexture_ = graphicsDevice_->CreateTexture(textureParam, data);
}

void EffectPlatformGL::BeginRendering()
{
	glViewport(0, 0, initParam_.WindowSize[0], initParam_.WindowSize[1]);

	if (usesGpuGroundDepth_)
	{
		UpdateGroundVertexBuffer();

		GL::glBindFramebuffer(GL_FRAMEBUFFER, groundDepthFrameBuffer_);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClearDepth(1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		DrawGround(groundDepthProgram_);

		GL::glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(22.0f / 255.0f, 34.0f / 255.0f, 48.0f / 255.0f, 1.0f);
		glClearDepth(1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		DrawGround(groundProgram_);
	}
	else
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		const auto prop = EffekseerRendererGL::GetTextureProperty(checkedTexture_);

		GL::glBindFramebuffer(GL_FRAMEBUFFER, 0);
		GL::glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer_);
		GL::glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, prop.Buffer, 0);
		Effekseer::OpenGLHelper::glBlitFramebuffer(
			0,
			0,
			initParam_.WindowSize[0],
			initParam_.WindowSize[1],
			0,
			0,
			initParam_.WindowSize[0],
			initParam_.WindowSize[1],
			GL_COLOR_BUFFER_BIT,
			GL_NEAREST);
		GL::glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glFlush();
		glFinish();
	}
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

void EffectPlatformGL::ResetBackgroundPattern()
{
	usesGpuGroundDepth_ = false;
	EffectPlatform::ResetBackgroundPattern();
}

void EffectPlatformGL::GenerateGroundDepth()
{
	isGroundDepthEnabled_ = true;
	usesGpuGroundDepth_ = false;

	if (!CreateGroundResources())
	{
		ReleaseGroundResources();
		EffectPlatform::GenerateGroundDepth();
		return;
	}

	if (groundDepthTextureForEffekseer_ == nullptr)
	{
		groundDepthTextureForEffekseer_ = EffekseerRendererGL::CreateTexture(graphicsDevice_, groundDepthColorTexture_, false, nullptr);
	}

	if (groundDepthTextureForEffekseer_ == nullptr)
	{
		ReleaseGroundResources();
		EffectPlatform::GenerateGroundDepth();
		return;
	}

	usesGpuGroundDepth_ = true;
	GetRenderer()->SetDepth(groundDepthTextureForEffekseer_, CreateGroundDepthReconstructionParameter(2.0f, -1.0f));
}
