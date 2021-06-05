#include "EffekseerRendererGL.Shader.h"
#include "EffekseerRendererGL.GLExtension.h"
#include "EffekseerRendererGL.Renderer.h"

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

Backend::ShaderRef& Shader::GetCurrentShader()
{
	if (shaderOverride_ != nullptr)
	{
		return shaderOverride_;
	}

	return shader_;
}

const Backend::ShaderRef& Shader::GetCurrentShader() const
{
	if (shaderOverride_ != nullptr)
	{
		return shaderOverride_;
	}

	return shader_;
}

void Shader::AssignAttribs()
{
	auto& shader = GetCurrentShader();

	if (vertexLayout_ != nullptr)
	{
		attribs_ = Backend::GetVertexAttribLocations(vertexLayout_, shader);
	}

	baseInstance_ = GLExt::glGetUniformLocation(shader->GetProgram(), "SPIRV_Cross_BaseInstance");
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
	AssignAttribs();
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
	AssignAttribs();
}

GLuint Shader::GetInterface() const
{
	return GetCurrentShader()->GetProgram();
}

void Shader::OverrideShader(::Effekseer::Backend::ShaderRef shader)
{
	shaderOverride_ = shader.DownCast<Backend::Shader>();

	AssignAttribs();
}

void Shader::SetVertexLayout(Backend::VertexLayoutRef vertexLayout)
{
	vertexLayout_ = vertexLayout;
	AssignAttribs();
}

void Shader::BeginScene()
{
	GLExt::glUseProgram(GetCurrentShader()->GetProgram());
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

void Shader::SetVertexConstantBufferSize(int32_t size)
{
	vertexConstantBuffer_ = graphicsDevice_->CreateUniformBuffer(size, nullptr).DownCast<Backend::UniformBuffer>();
}

void Shader::SetPixelConstantBufferSize(int32_t size)
{
	pixelConstantBuffer_ = graphicsDevice_->CreateUniformBuffer(size, nullptr).DownCast<Backend::UniformBuffer>();
}

void Shader::SetConstantBuffer()
{
	if (baseInstance_ >= 0)
	{
		GLExt::glUniform1i(baseInstance_, 0);
	}

	Backend::StoreUniforms(GetCurrentShader(), vertexConstantBuffer_, pixelConstantBuffer_, isTransposeEnabled_);

	GLCheckError();
}

GLint Shader::GetTextureSlot(int32_t index)
{
	return GetCurrentShader()->GetTextureLocations()[index];
}

bool Shader::GetTextureSlotEnable(int32_t index)
{
	return index < GetCurrentShader()->GetTextureLocations().size();
}

bool Shader::IsValid() const
{
	return GetCurrentShader()->GetProgram() != 0;
}

} // namespace EffekseerRendererGL
