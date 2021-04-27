
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

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
bool Shader::CompileShader(OpenGLDeviceType deviceType,
						   GLuint& program,
						   const ShaderCodeView* vsData,
						   size_t vsDataCount,
						   const ShaderCodeView* psData,
						   size_t psDataCount,
						   const char* name,
						   bool addHeader)
{
	std::array<const char*, 16> src_data;
	std::array<GLint, 16> src_size;

	if (vsDataCount + 1 > src_data.size() || psDataCount + 1 > src_data.size())
	{
		assert(0);
		return false;
	}

	GLuint vert_shader, frag_shader;
	GLint res_vs, res_fs, res_link;

	size_t vsOffset = 0;
	size_t psOffset = 0;

	// compile a vertex shader
	if (addHeader)
	{
		if (deviceType == OpenGLDeviceType::OpenGL3)
			src_data[0] = g_header_vs_gl3_src;
		if (deviceType == OpenGLDeviceType::OpenGL2)
			src_data[0] = g_header_vs_gl2_src;
		if (deviceType == OpenGLDeviceType::OpenGLES3)
			src_data[0] = g_header_vs_gles3_src;
		if (deviceType == OpenGLDeviceType::OpenGLES2)
			src_data[0] = g_header_vs_gles2_src;

		src_size[0] = (GLint)strlen(src_data[0]);
		vsOffset += 1;
	}

	for (int i = 0; i < vsDataCount; i++)
	{
		src_data[i + vsOffset] = vsData[i].Data;
		src_size[i + vsOffset] = (GLint)strlen(src_data[i + vsOffset]);
	}

	vsOffset += vsDataCount;

	vert_shader = GLExt::glCreateShader(GL_VERTEX_SHADER);
	GLExt::glShaderSource(vert_shader, (GLsizei)vsOffset, src_data.data(), src_size.data());
	GLExt::glCompileShader(vert_shader);
	GLExt::glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &res_vs);

	// compile a fragment shader
	if (addHeader)
	{
		if (deviceType == OpenGLDeviceType::OpenGL3)
			src_data[0] = g_header_fs_gl3_src;
		if (deviceType == OpenGLDeviceType::OpenGL2)
			src_data[0] = g_header_fs_gl2_src;
		if (deviceType == OpenGLDeviceType::OpenGLES3)
			src_data[0] = g_header_fs_gles3_src;
		if (deviceType == OpenGLDeviceType::OpenGLES2)
			src_data[0] = g_header_fs_gles2_src;

		src_size[0] = (GLint)strlen(src_data[0]);
		psOffset += 1;
	}

	for (int i = 0; i < psDataCount; i++)
	{
		src_data[i + psOffset] = psData[i].Data;
		src_size[i + psOffset] = (GLint)strlen(src_data[i + psOffset]);
	}

	psOffset += psDataCount;

	frag_shader = GLExt::glCreateShader(GL_FRAGMENT_SHADER);
	GLExt::glShaderSource(frag_shader, (GLsizei)psOffset, src_data.data(), src_size.data());
	GLExt::glCompileShader(frag_shader);
	GLExt::glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &res_fs);

	// create shader program
	program = GLExt::glCreateProgram();
	GLExt::glAttachShader(program, vert_shader);
	GLExt::glAttachShader(program, frag_shader);

	// link shaders
	GLExt::glLinkProgram(program);
	GLExt::glGetProgramiv(program, GL_LINK_STATUS, &res_link);

#ifndef NDEBUG
	if (res_link == GL_FALSE)
	{
		// output errors
		char log[512];
		int32_t log_size;
		GLExt::glGetShaderInfoLog(vert_shader, sizeof(log), &log_size, log);
		if (log_size > 0)
		{
			LOG(name);
			LOG(": Vertex Shader error.\n");
			LOG((std::string("DeviceType=") + std::to_string(static_cast<int>(deviceType)) + "\n").c_str());
			LOG(log);
		}
		GLExt::glGetShaderInfoLog(frag_shader, sizeof(log), &log_size, log);
		if (log_size > 0)
		{
			LOG(name);
			LOG(": Fragment Shader error.\n");
			LOG((std::string("DeviceType=") + std::to_string(static_cast<int>(deviceType)) + "\n").c_str());
			LOG(log);
		}
		GLExt::glGetProgramInfoLog(program, sizeof(log), &log_size, log);
		if (log_size > 0)
		{
			LOG(name);
			LOG(": Shader Link error.\n");
			LOG(log);
		}
	}
#endif
	// dispose shader objects
	GLExt::glDeleteShader(frag_shader);
	GLExt::glDeleteShader(vert_shader);

	if (res_link == GL_FALSE)
	{
		GLExt::glDeleteProgram(program);
		return false;
	}

	return true;
}

bool Shader::ReloadShader()
{
	assert(m_program == 0);

	GLuint program;

	std::vector<ShaderCodeView> vsData;
	std::vector<ShaderCodeView> psData;

	vsData.resize(vsCodes_.size());
	psData.resize(psCodes_.size());

	for (size_t i = 0; i < vsData.size(); i++)
	{
		vsData[i].Data = vsCodes_[i].Code.data();
		vsData[i].Length = (int32_t)vsCodes_[i].Code.size();
	}

	for (size_t i = 0; i < psData.size(); i++)
	{
		psData[i].Data = psCodes_[i].Code.data();
		psData[i].Length = (int32_t)psCodes_[i].Code.size();
	}

	if (CompileShader(m_deviceType, program, vsData.data(), vsData.size(), psData.data(), psData.size(), name_.c_str(), addHeader_))
	{
		m_program = program;
		GetAttribIdList(0, nullptr);
	}
	else
	{
		return false;
	}

	return true;
}

Shader::Shader(const Backend::GraphicsDeviceRef& graphicsDevice,
			   GLuint program,
			   const ShaderCodeView* vsData,
			   size_t vsDataCount,
			   const ShaderCodeView* psData,
			   size_t psDataCount,
			   const char* name,
			   bool hasRefCount,
			   bool addHeader)
	: DeviceObject(graphicsDevice.Get())
	, m_deviceType(graphicsDevice->GetDeviceType())
	, m_program(program)
	, m_vertexSize(0)
	, m_vertexConstantBuffer(nullptr)
	, m_pixelConstantBuffer(nullptr)
	, addHeader_(addHeader)
{
	m_textureSlots.fill(0);
	m_textureSlotEnables.fill(false);

	vsCodes_.resize(vsDataCount);
	psCodes_.resize(psDataCount);

	for (size_t i = 0; i < vsDataCount; i++)
	{
		vsCodes_[i].Code.resize(vsData[i].Length);
		memcpy(vsCodes_[i].Code.data(), vsData[i].Data, vsData[i].Length);
		vsCodes_[i].Code.push_back(0);
	}

	for (size_t i = 0; i < psDataCount; i++)
	{
		psCodes_[i].Code.resize(psData[i].Length);
		memcpy(psCodes_[i].Code.data(), psData[i].Data, psData[i].Length);
		psCodes_[i].Code.push_back(0);
	}

	name_ = name;

	baseInstance_ = GLExt::glGetUniformLocation(m_program, "SPIRV_Cross_BaseInstance");
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
GLint Shader::GetAttribId(const char* name) const
{
	auto ret = GLExt::glGetAttribLocation(m_program, name);

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
	auto ret = GLExt::glGetUniformLocation(m_program, name);

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
	GLExt::glDeleteProgram(m_program);
	ES_SAFE_DELETE_ARRAY(m_vertexConstantBuffer);
	ES_SAFE_DELETE_ARRAY(m_pixelConstantBuffer);
}

Shader* Shader::Create(const Backend::GraphicsDeviceRef& graphicsDevice,
					   const ShaderCodeView* vsData,
					   size_t vsDataCount,
					   const ShaderCodeView* psData,
					   size_t psDataCount,
					   const char* name,
					   bool hasRefCount,
					   bool addHeader)
{
	GLuint program;

	if (CompileShader(graphicsDevice->GetDeviceType(), program, vsData, vsDataCount, psData, psDataCount, name, addHeader))
	{
		return new Shader(graphicsDevice, program, vsData, vsDataCount, psData, psDataCount, name, hasRefCount, addHeader);
	}
	else
	{
		return nullptr;
	}
}

void Shader::OnLostDevice()
{
	GLExt::glDeleteProgram(m_program);
	m_program = 0;
}

void Shader::OnResetDevice()
{
	if (IsValid())
		return;

	if (!ReloadShader())
	{
		printf("Failed to reset device.\n");
	}
}

void Shader::OnChangeDevice()
{
	GLExt::glDeleteProgram(m_program);
	m_program = 0;

	if (!ReloadShader())
	{
		printf("Failed to change device.\n");
	}
}

GLuint Shader::GetInterface() const
{
	return m_program;
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
			m_aid.push_back(GLExt::glGetAttribLocation(m_program, info[i].name));
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
			m_aid.push_back(GLExt::glGetAttribLocation(m_program, attribs[i].name.c_str()));
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
		uid_list[i] = GLExt::glGetUniformLocation(m_program, info[i].name);
	}
}

void Shader::BeginScene()
{
	GLExt::glUseProgram(m_program);
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
	return m_program != 0;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerRendererGL
