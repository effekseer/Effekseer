
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#include "EffekseerRendererGL.Shader.h"
#include "EffekseerRendererGL.GLExtension.h"
#include "EffekseerRendererGL.Renderer.h"

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

static const char g_header_vs_gl3_src[] =
	"#version 330\n"
	"#define lowp\n"
	"#define mediump\n"
	"#define highp\n"
	"#define IN in\n"
	"#define CENTROID centroid\n"
	"#define TEX2D textureLod\n"
	"#define OUT out\n";

static const char g_header_fs_gl3_src[] =
	"#version 330\n"
	"#define lowp\n"
	"#define mediump\n"
	"#define highp\n"
	"#define IN in\n"
	"#define CENTROID centroid\n"
	"#define TEX2D texture\n"
	"layout (location = 0) out vec4 FRAGCOLOR;\n";

static const char g_header_vs_gles3_src[] =
	"#version 300 es\n"
	"precision mediump float;\n"
	"#define IN in\n"
	"#define CENTROID\n"
	"#define TEX2D textureLod\n"
	"#define OUT out\n";

static const char g_header_fs_gles3_src[] =
	"#version 300 es\n"
	"precision mediump float;\n"
	"#define IN in\n"
	"#define CENTROID\n"
	"#define TEX2D texture\n"
	"layout (location = 0) out vec4 FRAGCOLOR;\n";

static const char g_header_vs_gles2_src[] =
	"precision mediump float;\n"
	"#define IN attribute\n"
	"#define CENTROID\n"
	"#define TEX2D texture2DLod\n"
	"#define OUT varying\n";

static const char g_header_fs_gles2_src[] =
	"precision mediump float;\n"
	"#define IN varying\n"
	"#define CENTROID\n"
	"#define TEX2D texture2D\n"
	"#define FRAGCOLOR gl_FragColor\n";

static const char g_header_vs_gl2_src[] =
	"#version 120\n"
	"#define lowp\n"
	"#define mediump\n"
	"#define highp\n"
	"#define IN attribute\n"
	"#define CENTROID\n"
	"#define TEX2D texture2DLod\n"
	"#define OUT varying\n";

static const char g_header_fs_gl2_src[] =
	"#version 120\n"
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
bool Shader::CompileShader(
	OpenGLDeviceType deviceType,
	GLuint& program,
	const char* vs_src,
	size_t vertexShaderSize,
	const char* fs_src,
	size_t pixelShaderSize,
	const char* name)
{
	const char* src_data[2];
	GLint src_size[2];

	GLuint vert_shader, frag_shader;
	GLint res_vs, res_fs, res_link;

	// compile a vertex shader
	if (deviceType == OpenGLDeviceType::OpenGL3)
		src_data[0] = g_header_vs_gl3_src;
	if (deviceType == OpenGLDeviceType::OpenGL2)
		src_data[0] = g_header_vs_gl2_src;
	if (deviceType == OpenGLDeviceType::OpenGLES3)
		src_data[0] = g_header_vs_gles3_src;
	if (deviceType == OpenGLDeviceType::OpenGLES2 || deviceType == OpenGLDeviceType::Emscripten)
		src_data[0] = g_header_vs_gles2_src;

	src_size[0] = (GLint)strlen(src_data[0]);
	src_data[1] = vs_src;
	src_size[1] = (GLint)strlen(vs_src);

	vert_shader = GLExt::glCreateShader(GL_VERTEX_SHADER);
	GLExt::glShaderSource(vert_shader, 2, src_data, src_size);
	GLExt::glCompileShader(vert_shader);
	GLExt::glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &res_vs);

	// compile a fragment shader
	if (deviceType == OpenGLDeviceType::OpenGL3)
		src_data[0] = g_header_fs_gl3_src;
	if (deviceType == OpenGLDeviceType::OpenGL2)
		src_data[0] = g_header_fs_gl2_src;
	if (deviceType == OpenGLDeviceType::OpenGLES3)
		src_data[0] = g_header_fs_gles3_src;
	if (deviceType == OpenGLDeviceType::OpenGLES2 || deviceType == OpenGLDeviceType::Emscripten)
		src_data[0] = g_header_fs_gles2_src;

	src_size[0] = (GLint)strlen(src_data[0]);
	src_data[1] = fs_src;
	src_size[1] = (GLint)strlen(fs_src);

	frag_shader = GLExt::glCreateShader(GL_FRAGMENT_SHADER);
	GLExt::glShaderSource(frag_shader, 2, src_data, src_size);
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
			LOG(log);
		}
		GLExt::glGetShaderInfoLog(frag_shader, sizeof(log), &log_size, log);
		if (log_size > 0)
		{
			LOG(name);
			LOG(": Fragment Shader error.\n");
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

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
Shader::Shader(
	OpenGLDeviceType deviceType,
	Renderer* renderer,
	DeviceObjectCollection* deviceObjectCollection,
	GLuint program,
	const char* vs_src,
	size_t vertexShaderSize,
	const char* fs_src,
	size_t pixelShaderSize,
	const char* name,
	bool hasRefCount)
	: DeviceObject(static_cast<RendererImplemented*>(renderer), deviceObjectCollection, hasRefCount)
	, deviceType_(deviceType)
	, m_program(program)
	, m_vertexSize(0)
	, m_vertexConstantBuffer(NULL)
	, m_pixelConstantBuffer(NULL)
{
	m_textureSlots.fill(0);
	m_textureSlotEnables.fill(false);

	m_vsSrc.resize(vertexShaderSize);
	memcpy(m_vsSrc.data(), vs_src, vertexShaderSize);
	m_vsSrc.push_back(0);

	m_psSrc.resize(pixelShaderSize);
	memcpy(m_psSrc.data(), fs_src, pixelShaderSize);
	m_psSrc.push_back(0);

	m_name = name;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
GLint Shader::GetAttribId(const char* name) const
{
	return GLExt::glGetAttribLocation(m_program, name);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
GLint Shader::GetUniformId(const char* name) const
{
	return GLExt::glGetUniformLocation(m_program, name);
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

Shader* Shader::Create(OpenGLDeviceType deviceType,

					   DeviceObjectCollection* deviceObjectCollection,
					   const char* vs_src,
					   size_t vertexShaderSize,
					   const char* fs_src,
					   size_t pixelShaderSize,
					   const char* name,
					   bool hasRefCount)
{
	GLuint program;

	if (CompileShader(deviceType, program, vs_src, vertexShaderSize, fs_src, pixelShaderSize, name))
	{
		return new Shader(deviceType, nullptr, deviceObjectCollection, program, vs_src, vertexShaderSize, fs_src, pixelShaderSize, name, hasRefCount);
	}
	else
	{
		return NULL;
	}
}

Shader* Shader::Create(
	Renderer* renderer, const char* vs_src, size_t vertexShaderSize, const char* fs_src, size_t pixelShaderSize, const char* name)
{
	GLuint program;

	auto r = static_cast<RendererImplemented*>(renderer);

	if (CompileShader(r->GetDeviceType(), program, vs_src, vertexShaderSize, fs_src, pixelShaderSize, name))
	{
		return new Shader(r->GetDeviceType(),
						  renderer,
						  r->GetDeviceObjectCollection(),
						  program,
						  vs_src,
						  vertexShaderSize,
						  fs_src,
						  pixelShaderSize,
						  name,
						  true);
	}
	else
	{
		return NULL;
	}
}

void Shader::OnLostDevice()
{
	GLExt::glDeleteProgram(m_program);
	m_program = 0;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Shader::OnResetDevice()
{
	if (IsValid())
		return;

	GLuint program;

	if (CompileShader(
			deviceType_,
			program,
			(const char*)(m_vsSrc.data()),
			m_vsSrc.size(),
			(const char*)(m_psSrc.data()),
			m_psSrc.size(),
			m_name.c_str()))
	{
		m_program = program;
		GetAttribIdList(0, nullptr);
	}
	else
	{
		printf("Failed to reset device.\n");
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Shader::OnChangeDevice()
{
	GLExt::glDeleteProgram(m_program);

	GLuint program;

	if (CompileShader(
			deviceType_,
			program,
			(const char*)&(m_vsSrc[0]),
			m_vsSrc.size(),
			(const char*)&(m_psSrc[0]),
			m_psSrc.size(),
			m_name.c_str()))
	{
		m_program = program;
		GetAttribIdList(0, nullptr);
	}
	else
	{
		printf("Failed to change device.\n");
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
GLuint Shader::GetInterface() const
{
	return m_program;
}

void Shader::GetAttribIdList(int count, const ShaderAttribInfo* info)
{
	// TODO : refactoring

	m_aid.clear();

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
			GLExt::glVertexAttribPointer(m_aid[i], m_layout[i].count, m_layout[i].type, m_layout[i].normalized, m_vertexSize, reinterpret_cast<GLvoid*>(m_layout[i].offset));
		}
	}
}

void Shader::SetVertexSize(int32_t vertexSize)
{
	m_vertexSize = vertexSize;
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
void Shader::AddVertexConstantLayout(eConstantType type, GLint id, int32_t offset)
{
	ConstantLayout l;
	l.Type = type;
	l.ID = id;
	l.Offset = offset;
	m_vertexConstantLayout.push_back(l);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void Shader::AddPixelConstantLayout(eConstantType type, GLint id, int32_t offset)
{
	ConstantLayout l;
	l.Type = type;
	l.ID = id;
	l.Offset = offset;
	m_pixelConstantLayout.push_back(l);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void Shader::SetConstantBuffer()
{
	for (size_t i = 0; i < m_vertexConstantLayout.size(); i++)
	{
		if (m_vertexConstantLayout[i].Type == CONSTANT_TYPE_MATRIX44)
		{
			uint8_t* data = (uint8_t*)m_vertexConstantBuffer;
			data += m_vertexConstantLayout[i].Offset;
			GLExt::glUniformMatrix4fv(
				m_vertexConstantLayout[i].ID,
				1,
				GL_FALSE,
				(const GLfloat*)data);
		}
		else if (
			CONSTANT_TYPE_MATRIX44_ARRAY_END > m_vertexConstantLayout[i].Type &&
			m_vertexConstantLayout[i].Type >= CONSTANT_TYPE_MATRIX44_ARRAY)
		{
			uint8_t* data = (uint8_t*)m_vertexConstantBuffer;
			data += m_vertexConstantLayout[i].Offset;
			GLExt::glUniformMatrix4fv(
				m_vertexConstantLayout[i].ID,
				m_vertexConstantLayout[i].Type - CONSTANT_TYPE_MATRIX44_ARRAY,
				GL_FALSE,
				(const GLfloat*)data);
		}

		else if (m_vertexConstantLayout[i].Type == CONSTANT_TYPE_VECTOR4)
		{
			uint8_t* data = (uint8_t*)m_vertexConstantBuffer;
			data += m_vertexConstantLayout[i].Offset;
			GLExt::glUniform4fv(
				m_vertexConstantLayout[i].ID,
				1,
				(const GLfloat*)data);
		}
		else if (
			CONSTANT_TYPE_VECTOR4_ARRAY_END > m_vertexConstantLayout[i].Type &&
			m_vertexConstantLayout[i].Type >= CONSTANT_TYPE_VECTOR4_ARRAY)
		{
			uint8_t* data = (uint8_t*)m_vertexConstantBuffer;
			data += m_vertexConstantLayout[i].Offset;
			GLExt::glUniform4fv(
				m_vertexConstantLayout[i].ID,
				m_vertexConstantLayout[i].Type - CONSTANT_TYPE_VECTOR4_ARRAY,
				(const GLfloat*)data);
		}
	}

	for (size_t i = 0; i < m_pixelConstantLayout.size(); i++)
	{
		if (m_pixelConstantLayout[i].Type == CONSTANT_TYPE_MATRIX44)
		{
			uint8_t* data = (uint8_t*)m_pixelConstantBuffer;
			data += m_pixelConstantLayout[i].Offset;
			GLExt::glUniformMatrix4fv(
				m_pixelConstantLayout[i].ID,
				1,
				GL_FALSE,
				(const GLfloat*)data);
		}
		else if (
			CONSTANT_TYPE_MATRIX44_ARRAY_END > m_pixelConstantLayout[i].Type &&
			m_pixelConstantLayout[i].Type >= CONSTANT_TYPE_MATRIX44_ARRAY)
		{
			uint8_t* data = (uint8_t*)m_pixelConstantBuffer;
			data += m_pixelConstantLayout[i].Offset;
			GLExt::glUniformMatrix4fv(
				m_pixelConstantLayout[i].ID,
				m_pixelConstantLayout[i].Type - CONSTANT_TYPE_MATRIX44_ARRAY,
				GL_FALSE,
				(const GLfloat*)data);
		}

		else if (m_pixelConstantLayout[i].Type == CONSTANT_TYPE_VECTOR4)
		{
			uint8_t* data = (uint8_t*)m_pixelConstantBuffer;
			data += m_pixelConstantLayout[i].Offset;
			GLExt::glUniform4fv(
				m_pixelConstantLayout[i].ID,
				1,
				(const GLfloat*)data);
		}
		else if (
			CONSTANT_TYPE_VECTOR4_ARRAY_END > m_pixelConstantLayout[i].Type &&
			m_pixelConstantLayout[i].Type >= CONSTANT_TYPE_VECTOR4_ARRAY)
		{
			uint8_t* data = (uint8_t*)m_pixelConstantBuffer;
			data += m_pixelConstantLayout[i].Offset;
			GLExt::glUniform4fv(
				m_pixelConstantLayout[i].ID,
				m_pixelConstantLayout[i].Type - CONSTANT_TYPE_VECTOR4_ARRAY,
				(const GLfloat*)data);
		}
	}

	GLCheckError();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void Shader::SetTextureSlot(int32_t index, GLuint value)
{
	m_textureSlots[index] = value;
	m_textureSlotEnables[index] = true;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
GLuint Shader::GetTextureSlot(int32_t index)
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
