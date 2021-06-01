
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
	, m_vertexConstantBuffer(nullptr)
	, m_pixelConstantBuffer(nullptr)
{
	m_textureSlots.fill(0);
	m_textureSlotEnables.fill(false);

	name_ = name;

	GetAttribIdList(0, nullptr);
	baseInstance_ = GLExt::glGetUniformLocation(shader_->GetProgram(), "SPIRV_Cross_BaseInstance");
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
GLint Shader::GetAttribId(const char* name) const
{
	auto ret = GLExt::glGetAttribLocation(shader_->GetProgram(), name);

#ifdef __INTERNAL_DEBUG__
	if (ret < 0)
	{
		std::string message = "Unused : " + name_ + " : " + std::string(name) + "\n";
		LOG(message.c_str());
	}
#endif

	return ret;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
GLint Shader::GetUniformId(const char* name) const
{
	auto ret = GLExt::glGetUniformLocation(shader_->GetProgram(), name);

#ifdef __INTERNAL_DEBUG__
	if (ret < 0)
	{
		std::string message = "Unused : " + name_ + " : " + std::string(name) + "\n";
		LOG(message.c_str());
	}
#endif

	return ret;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
Shader::~Shader()
{
	ES_SAFE_DELETE_ARRAY(m_vertexConstantBuffer);
	ES_SAFE_DELETE_ARRAY(m_pixelConstantBuffer);
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
	GetAttribIdList(0, nullptr);
	baseInstance_ = GLExt::glGetUniformLocation(shader_->GetProgram(), "SPIRV_Cross_BaseInstance");
}

GLuint Shader::GetInterface() const
{
	return shader_->GetProgram();
}

void Shader::GetAttribIdList(int count, const ShaderAttribInfo* info)
{
	// TODO : refactoring

	m_aid.clear();

	m_vertexSize = 0;

	if (info != nullptr)
	{
		for (int i = 0; i < count; i++)
		{
			m_aid.push_back(GLExt::glGetAttribLocation(shader_->GetProgram(), info[i].name));
			Layout layout;

			layout.normalized = info[i].normalized;
			layout.type = info[i].type;
			layout.offset = info[i].offset;
			layout.count = info[i].count;

			m_layout.push_back(layout);
		}

		attribs.resize(count);

		for (int i = 0; i < count; i++)
		{
			attribs[i].name = info[i].name;
			attribs[i].normalized = info[i].normalized;
			attribs[i].type = info[i].type;
			attribs[i].offset = info[i].offset;
			attribs[i].count = info[i].count;

			int elementSize = 0;
			if (attribs[i].type == GL_FLOAT)
			{
				elementSize = sizeof(float);
			}
			if (attribs[i].type == GL_UNSIGNED_BYTE)
			{
				elementSize = sizeof(uint8_t);
			}

			m_vertexSize = Effekseer::Max(m_vertexSize, attribs[i].offset + elementSize * attribs[i].count);
		}
	}
	else
	{
		for (int i = 0; i < (int)attribs.size(); i++)
		{
			m_aid.push_back(GLExt::glGetAttribLocation(shader_->GetProgram(), attribs[i].name.c_str()));
			Layout layout;

			layout.normalized = attribs[i].normalized;
			layout.type = attribs[i].type;
			layout.offset = attribs[i].offset;
			layout.count = attribs[i].count;

			m_layout.push_back(layout);
		}
	}
}

void Shader::GetUniformIdList(int count, const ShaderUniformInfo* info, GLint* uid_list) const
{
	for (int i = 0; i < count; i++)
	{
		uid_list[i] = GLExt::glGetUniformLocation(shader_->GetProgram(), info[i].name);
	}
}

void Shader::BeginScene()
{
	GLExt::glUseProgram(shader_->GetProgram());
}

void Shader::EndScene()
{
	GLExt::glUseProgram(0);
}

void Shader::EnableAttribs()
{
	GLCheckError();
	for (size_t i = 0; i < m_aid.size(); i++)
	{
		if (m_aid[i] >= 0)
		{
			GLExt::glEnableVertexAttribArray(m_aid[i]);
		}
	}
	GLCheckError();
}

void Shader::DisableAttribs()
{
	GLCheckError();

	for (size_t i = 0; i < m_aid.size(); i++)
	{
		if (m_aid[i] >= 0)
		{
			GLExt::glDisableVertexAttribArray(m_aid[i]);
		}
	}
	GLCheckError();
}

void Shader::SetVertex()
{
	for (size_t i = 0; i < m_aid.size(); i++)
	{
		if (m_aid[i] >= 0)
		{
			GLExt::glVertexAttribPointer(m_aid[i],
										 m_layout[i].count,
										 m_layout[i].type,
										 m_layout[i].normalized,
										 m_vertexSize,
										 reinterpret_cast<GLvoid*>(m_layout[i].offset));
		}
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void Shader::SetVertexConstantBufferSize(int32_t size)
{
	ES_SAFE_DELETE_ARRAY(m_vertexConstantBuffer);
	m_vertexConstantBuffer = new uint8_t[size];
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void Shader::SetPixelConstantBufferSize(int32_t size)
{
	ES_SAFE_DELETE_ARRAY(m_pixelConstantBuffer);
	m_pixelConstantBuffer = new uint8_t[size];
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void Shader::AddVertexConstantLayout(eConstantType type, GLint id, int32_t offset, int32_t count)
{
	ConstantLayout l;
	l.Type = type;
	l.ID = id;
	l.Offset = offset;
	l.Count = count;
	m_vertexConstantLayout.push_back(l);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void Shader::AddPixelConstantLayout(eConstantType type, GLint id, int32_t offset, int32_t count)
{
	ConstantLayout l;
	l.Type = type;
	l.ID = id;
	l.Offset = offset;
	l.Count = count;
	m_pixelConstantLayout.push_back(l);
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

	for (size_t i = 0; i < m_vertexConstantLayout.size(); i++)
	{
		if (m_vertexConstantLayout[i].Type == CONSTANT_TYPE_MATRIX44)
		{
			uint8_t* data = (uint8_t*)m_vertexConstantBuffer;
			data += m_vertexConstantLayout[i].Offset;
			GLExt::glUniformMatrix4fv(m_vertexConstantLayout[i].ID, m_vertexConstantLayout[i].Count, isTransposeEnabled_ ? GL_TRUE : GL_FALSE, (const GLfloat*)data);
		}

		else if (m_vertexConstantLayout[i].Type == CONSTANT_TYPE_VECTOR4)
		{
			uint8_t* data = (uint8_t*)m_vertexConstantBuffer;
			data += m_vertexConstantLayout[i].Offset;
			GLExt::glUniform4fv(m_vertexConstantLayout[i].ID, m_vertexConstantLayout[i].Count, (const GLfloat*)data);
		}
	}

	for (size_t i = 0; i < m_pixelConstantLayout.size(); i++)
	{
		if (m_pixelConstantLayout[i].Type == CONSTANT_TYPE_MATRIX44)
		{
			uint8_t* data = (uint8_t*)m_pixelConstantBuffer;
			data += m_pixelConstantLayout[i].Offset;
			GLExt::glUniformMatrix4fv(m_pixelConstantLayout[i].ID, m_pixelConstantLayout[i].Count, isTransposeEnabled_ ? GL_TRUE : GL_FALSE, (const GLfloat*)data);
		}

		else if (m_pixelConstantLayout[i].Type == CONSTANT_TYPE_VECTOR4)
		{
			uint8_t* data = (uint8_t*)m_pixelConstantBuffer;
			data += m_pixelConstantLayout[i].Offset;
			GLExt::glUniform4fv(m_pixelConstantLayout[i].ID, m_pixelConstantLayout[i].Count, (const GLfloat*)data);
		}
	}

	GLCheckError();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void Shader::SetTextureSlot(int32_t index, GLint value)
{
	if (value >= 0)
	{
		m_textureSlots[index] = value;
		m_textureSlotEnables[index] = true;
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
GLint Shader::GetTextureSlot(int32_t index)
{
	return m_textureSlots[index];
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
bool Shader::GetTextureSlotEnable(int32_t index)
{
	return m_textureSlotEnables[index];
}

bool Shader::IsValid() const
{
	return shader_->GetProgram() != 0;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerRendererGL
