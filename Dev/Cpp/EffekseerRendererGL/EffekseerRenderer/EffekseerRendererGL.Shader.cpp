
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#include "EffekseerRendererGL.Shader.h"
#include "EffekseerRendererGL.GLExtension.h"
#include "EffekseerRendererGL.Renderer.h"

// #define __INTERNAL_DEBUG__

#ifdef __ANDROID__

#ifdef __ANDROID__DEBUG__
#include "android/log.h"
#define LOG(s) __android_log_print(ANDROID_LOG_DEBUG, "Tag", "%s", s)
#else
#define LOG(s) printf("%s", s)
#endif

#elif defined(_WIN32)
#include <windows.h>
#define LOG(s) OutputDebugStringA(s)
#else
#define LOG(s) printf("%s", s)
#endif

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{

static const char g_header_vs_gl3_src[] = "#version 330\n"
										  R"(
)"
										  "#define lowp\n"
										  "#define mediump\n"
										  "#define highp\n"
										  "#define IN in\n"
										  "#define CENTROID centroid\n"
										  "#define TEX2D textureLod\n"
										  "#define OUT out\n";

static const char g_header_fs_gl3_src[] = "#version 330\n"
										  R"(
)"
										  "#define lowp\n"
										  "#define mediump\n"
										  "#define highp\n"
										  "#define IN in\n"
										  "#define CENTROID centroid\n"
										  "#define TEX2D texture\n"
										  "layout (location = 0) out vec4 FRAGCOLOR;\n";

static const char g_header_vs_gles3_src[] = "#version 300 es\n"
											R"(
)"
											"#define IN in\n"
											"#define CENTROID centroid\n"
											"#define TEX2D textureLod\n"
											"#define OUT out\n";

static const char g_header_fs_gles3_src[] = "#version 300 es\n"
											R"(
)"
											"precision highp float;\n"
											"#define IN in\n"
											"#define CENTROID centroid\n"
											"#define TEX2D texture\n"
											"layout (location = 0) out vec4 FRAGCOLOR;\n";

static const char g_header_vs_gles2_src[] =
	R"(
#define EFK__INSTANCING_DISABLED__ 1
)"
	"#define IN attribute\n"
	"#define CENTROID\n"
	"#define TEX2D texture2DLod\n"
	"#define OUT varying\n";

static const char g_header_fs_gles2_src[] = "precision highp float;\n"
											R"(
)"
											"#define IN varying\n"
											"#define CENTROID\n"
											"#define TEX2D texture2D\n"
											"#define FRAGCOLOR gl_FragColor\n";

static const char g_header_vs_gl2_src[] = "#version 120\n"
										  R"(
#define EFK__INSTANCING_DISABLED__ 1
)"
										  "#define lowp\n"
										  "#define mediump\n"
										  "#define highp\n"
										  "#define IN attribute\n"
										  "#define CENTROID\n"
										  "#define TEX2D texture2DLod\n"
										  "#define OUT varying\n";

static const char g_header_fs_gl2_src[] = "#version 120\n"
										  R"(
)"
										  "#define lowp\n"
										  "#define mediump\n"
										  "#define highp\n"
										  "#define IN varying\n"
										  "#define CENTROID\n"
										  "#define TEX2D texture2D\n"
										  "#define FRAGCOLOR gl_FragColor\n";

const char* GetVertexShaderHeader(OpenGLDeviceType deviceType)
{
	if (deviceType == OpenGLDeviceType::OpenGL3)
		return g_header_vs_gl3_src;
	if (deviceType == OpenGLDeviceType::OpenGL2)
		return g_header_vs_gl2_src;
	if (deviceType == OpenGLDeviceType::OpenGLES3)
		return g_header_vs_gles3_src;
	if (deviceType == OpenGLDeviceType::OpenGLES2)
		return g_header_vs_gles2_src;
	return nullptr;
}

const char* GetFragmentShaderHeader(OpenGLDeviceType deviceType)
{
	if (deviceType == OpenGLDeviceType::OpenGL3)
		return g_header_fs_gl3_src;
	if (deviceType == OpenGLDeviceType::OpenGL2)
		return g_header_fs_gl2_src;
	if (deviceType == OpenGLDeviceType::OpenGLES3)
		return g_header_fs_gles3_src;
	if (deviceType == OpenGLDeviceType::OpenGLES2)
		return g_header_fs_gles2_src;
	return nullptr;
}

Shader::Shader(const Backend::GraphicsDeviceRef& graphicsDevice,
			   Backend::ShaderRef shader,
			   const char* name)
	: DeviceObject(graphicsDevice.Get())
	, m_deviceType(graphicsDevice->GetDeviceType())
	, shader_(shader)
	, m_vertexSize(0)
{
	name_ = name;

	graphicsDevice_ = graphicsDevice;
	baseInstance_ = GLExt::glGetUniformLocation(shader_->GetProgram(), "SPIRV_Cross_BaseInstance");
}

Shader* Shader::Create(const Backend::GraphicsDeviceRef& graphicsDevice,
					   Backend::ShaderRef shader,
					   const char* name)
{
	if (shader == nullptr)
		return nullptr;

	return new Shader(graphicsDevice, shader, name);
}

void Shader::OnResetDevice()
{
	if (shaderOverride_ != nullptr)
	{
		attribs_ = Backend::GetVertexAttribLocations(vertexLayout_, shaderOverride_);
		baseInstance_ = GLExt::glGetUniformLocation(shaderOverride_->GetProgram(), "SPIRV_Cross_BaseInstance");
	}
	else
	{
		attribs_ = Backend::GetVertexAttribLocations(vertexLayout_, shader_);
		baseInstance_ = GLExt::glGetUniformLocation(shader_->GetProgram(), "SPIRV_Cross_BaseInstance");
	}
}

GLuint Shader::GetInterface() const
{
	if (shaderOverride_ != nullptr)
	{
		return shaderOverride_->GetProgram();
	}

	return shader_->GetProgram();
}

void Shader::OverrideShader(::Effekseer::Backend::ShaderRef shader)
{
	shaderOverride_ = shader.DownCast<Backend::Shader>();

	if (shaderOverride_ != nullptr)
	{
		attribs_ = Backend::GetVertexAttribLocations(vertexLayout_, shaderOverride_);
		baseInstance_ = GLExt::glGetUniformLocation(shaderOverride_->GetProgram(), "SPIRV_Cross_BaseInstance");
	}
	else
	{
		attribs_ = Backend::GetVertexAttribLocations(vertexLayout_, shader_);
		baseInstance_ = GLExt::glGetUniformLocation(shader_->GetProgram(), "SPIRV_Cross_BaseInstance");
	}
}

void Shader::SetVertexLayout(Backend::VertexLayoutRef vertexLayout)
{
	vertexLayout_ = vertexLayout;
	attribs_ = Backend::GetVertexAttribLocations(vertexLayout_, shader_);
}

void Shader::BeginScene()
{
	if (shaderOverride_ != nullptr)
	{
		GLExt::glUseProgram(shaderOverride_->GetProgram());
	}
	else
	{
		GLExt::glUseProgram(shader_->GetProgram());	
	}
}

void Shader::EndScene()
{
	GLExt::glUseProgram(0);
}

void Shader::EnableAttribs()
{
	GLCheckError();
	Backend::EnableLayouts(vertexLayout_, attribs_);
	GLCheckError();
}

void Shader::DisableAttribs()
{
	GLCheckError();
	Backend::DisableLayouts(attribs_);
	GLCheckError();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void Shader::SetVertexConstantBufferSize(int32_t size)
{
	vertexConstantBuffer_ = graphicsDevice_->CreateUniformBuffer(size, nullptr).DownCast<Backend::UniformBuffer>();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void Shader::SetPixelConstantBufferSize(int32_t size)
{
	pixelConstantBuffer_ = graphicsDevice_->CreateUniformBuffer(size, nullptr).DownCast<Backend::UniformBuffer>();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void Shader::SetConstantBuffer()
{
	// baseInstance_
	if (baseInstance_ >= 0)
	{
		GLExt::glUniform1i(baseInstance_, 0);
	}

	if (shaderOverride_ != nullptr)
	{
		Backend::StoreUniforms(shaderOverride_, vertexConstantBuffer_, pixelConstantBuffer_, isTransposeEnabled_);
	}
	else
	{
		Backend::StoreUniforms(shader_, vertexConstantBuffer_, pixelConstantBuffer_, isTransposeEnabled_);
	}

	GLCheckError();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
GLint Shader::GetTextureSlot(int32_t index)
{
	if (shaderOverride_ != nullptr)
	{
		return shaderOverride_->GetTextureLocations()[index];
	}

	return shader_->GetTextureLocations()[index];
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
bool Shader::GetTextureSlotEnable(int32_t index)
{
	if (shaderOverride_ != nullptr)
	{
		return index < shaderOverride_->GetTextureLocations().size();
	}

	return index < shader_->GetTextureLocations().size();
}

bool Shader::IsValid() const
{
	if (shaderOverride_ != nullptr)
	{
		return shaderOverride_->GetProgram() != 0;
	}

	return shader_->GetProgram() != 0;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerRendererGL
