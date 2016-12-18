
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#include "EffekseerRendererGL.Shader.h"
#include "EffekseerRendererGL.Renderer.h"
#include "EffekseerRendererGL.GLExtension.h"

#if _WIN32
#include <windows.h>
#define LOG(s)	OutputDebugStringA(s)
#else
#define LOG(s)	printf("%s", s)
#endif

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{

static const char g_header_vs_gl3_src [] =
"#version 330\n" \
"#define lowp\n" \
"#define mediump\n" \
"#define highp\n" \
"#define IN in\n" \
"#define OUT out\n";

static const char g_header_fs_gl3_src [] =
"#version 330\n" \
"#define lowp\n" \
"#define mediump\n" \
"#define highp\n" \
"#define IN in\n" \
"#define TEX2D texture\n" \
"layout (location = 0) out vec4 FRAGCOLOR;\n";

static const char g_header_vs_gles3_src [] =
"#version 300 es\n" \
"precision mediump float;\n" \
"#define IN in\n" \
"#define OUT out\n";

static const char g_header_fs_gles3_src [] =
"#version 300 es\n" \
"precision mediump float;\n" \
"#define IN in\n" \
"#define TEX2D texture\n" \
"layout (location = 0) out vec4 FRAGCOLOR;\n";

static const char g_header_vs_gles2_src [] =
"precision mediump float;\n" \
"#define IN attribute\n" \
"#define OUT varying\n";

static const char g_header_fs_gles2_src [] =
"precision mediump float;\n" \
"#define IN varying\n" \
"#define TEX2D texture2D\n" \
"#define FRAGCOLOR gl_FragColor\n";

static const char g_header_vs_gl2_src [] =
"#version 110\n" \
"#define lowp\n" \
"#define mediump\n" \
"#define highp\n" \
"#define IN attribute\n" \
"#define OUT varying\n";

static const char g_header_fs_gl2_src [] =
"#version 110\n" \
"#define lowp\n" \
"#define mediump\n" \
"#define highp\n" \
"#define IN varying\n" \
"#define TEX2D texture2D\n" \
"#define FRAGCOLOR gl_FragColor\n";

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
bool Shader::CompileShader(
	RendererImplemented* renderer,
	GLuint& program,
	const char* vs_src,
	int32_t vertexShaderSize,
	const char* fs_src,
	int32_t pixelShaderSize,
	const char* name)
{
	const char* src_data[2];
	GLint src_size[2];

	GLuint vert_shader, frag_shader;
	GLint res_vs, res_fs, res_link;
	

	// バーテックスシェーダをコンパイル
	if (renderer->GetDeviceType() == OpenGLDeviceType::OpenGL3) src_data[0] = g_header_vs_gl3_src;
	if (renderer->GetDeviceType() == OpenGLDeviceType::OpenGL2) src_data[0] = g_header_vs_gl2_src;
	if (renderer->GetDeviceType() == OpenGLDeviceType::OpenGLES3) src_data[0] = g_header_vs_gles3_src;
	if (renderer->GetDeviceType() == OpenGLDeviceType::OpenGLES2 || renderer->GetDeviceType() == OpenGLDeviceType::Emscripten) src_data[0] = g_header_vs_gles2_src;

	src_size[0] = (GLint) strlen(src_data[0]);
	src_data[1] = vs_src;
	src_size[1] = (GLint)strlen(vs_src);
	
	vert_shader = GLExt::glCreateShader(GL_VERTEX_SHADER);
	GLExt::glShaderSource(vert_shader, 2, src_data, src_size);
	GLExt::glCompileShader(vert_shader);
	GLExt::glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &res_vs);

	// フラグメントシェーダをコンパイル
	if (renderer->GetDeviceType() == OpenGLDeviceType::OpenGL3) src_data[0] = g_header_fs_gl3_src;
	if (renderer->GetDeviceType() == OpenGLDeviceType::OpenGL2) src_data[0] = g_header_fs_gl2_src;
	if (renderer->GetDeviceType() == OpenGLDeviceType::OpenGLES3) src_data[0] = g_header_fs_gles3_src;
	if (renderer->GetDeviceType() == OpenGLDeviceType::OpenGLES2 || renderer->GetDeviceType() == OpenGLDeviceType::Emscripten) src_data[0] = g_header_fs_gles2_src;

	src_size[0] = (GLint) strlen(src_data[0]);
	src_data[1] = fs_src;
	src_size[1] = strlen(fs_src);

	frag_shader = GLExt::glCreateShader(GL_FRAGMENT_SHADER);
	GLExt::glShaderSource(frag_shader, 2, src_data, src_size);
	GLExt::glCompileShader(frag_shader);
	GLExt::glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &res_fs);
	
	// シェーダプログラムの作成
	program = GLExt::glCreateProgram();
	GLExt::glAttachShader(program, vert_shader);
	GLExt::glAttachShader(program, frag_shader);
	
	// シェーダプログラムのリンク
	GLExt::glLinkProgram(program);
	GLExt::glGetProgramiv(program, GL_LINK_STATUS, &res_link);

#ifndef NDEBUG
	{
		// エラー出力
		char log[512];
		int32_t log_size;
		GLExt::glGetShaderInfoLog(vert_shader, sizeof(log), &log_size, log);
		if (log_size > 0) {
			LOG(name);
			LOG(": Vertex Shader error.\n");
			LOG(log);
		}
		GLExt::glGetShaderInfoLog(frag_shader, sizeof(log), &log_size, log);
		if (log_size > 0) {
			LOG(name);
			LOG(": Fragment Shader error.\n");
			LOG(log);
		}
		GLExt::glGetProgramInfoLog(program, sizeof(log), &log_size, log);
		if (log_size > 0) {
			LOG(name);
			LOG(": Shader Link error.\n");
			LOG(log);
		}
	}
#endif
	// シェーダオブジェクトの削除
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
	RendererImplemented* renderer,
	GLuint program,
	const char* vs_src,
	int32_t vertexShaderSize,
	const char* fs_src,
	int32_t pixelShaderSize,
	const char* name)
	: DeviceObject		( renderer )
	, m_program			( program )
	, m_vertexSize		( 0 )
	, m_vertexConstantBuffer	( NULL )
	, m_pixelConstantBuffer		( NULL )
{
	for (int32_t i = 0; i < 4; i++)
	{
		m_textureSlots[i] = 0;
		m_textureSlotEnables[i] = false;
	}

	m_vsSrc.resize(vertexShaderSize);
	memcpy( &(m_vsSrc[0]), vs_src, vertexShaderSize );

	m_psSrc.resize(pixelShaderSize);
	memcpy( &(m_psSrc[0]), fs_src, pixelShaderSize );

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
	ES_SAFE_DELETE_ARRAY( m_vertexConstantBuffer );
	ES_SAFE_DELETE_ARRAY( m_pixelConstantBuffer );
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
Shader* Shader::Create(
	RendererImplemented* renderer,
		const char* vs_src,
		int32_t vertexShaderSize,
		const char* fs_src,
		int32_t pixelShaderSize,
		const char* name)
{
	GLuint program;
	
	assert( renderer != NULL );

	if(CompileShader(
		renderer,
		program,
		vs_src,
		vertexShaderSize,
		fs_src,
		pixelShaderSize,
		name))
	{
		return new Shader( 
			renderer, 
			program,
			vs_src,
			vertexShaderSize,
			fs_src,
			pixelShaderSize, 
			name);
	}
	else
	{
		return NULL;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Shader::OnLostDevice()
{
	GLExt::glDeleteProgram(m_program);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Shader::OnResetDevice()
{
	GLuint program;
	
	if(CompileShader(
		GetRenderer(),
		program,
		(const char*)&(m_vsSrc[0]),
		m_vsSrc.size(),
		(const char*)&(m_psSrc[0]),
		m_psSrc.size(),
		m_name.c_str()))
	{
		m_program = program;
	}
	else
	{

	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Shader::OnChangeDevice()
{
	GLExt::glDeleteProgram(m_program);

	GLuint program;
	
	if(CompileShader(
		GetRenderer(),
		program,
		(const char*)&(m_vsSrc[0]),
		m_vsSrc.size(),
		(const char*)&(m_psSrc[0]),
		m_psSrc.size(),
		m_name.c_str()))
	{
		m_program = program;
	}
	else
	{

	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
GLuint Shader::GetInterface() const
{
	return m_program;
}

void Shader::GetAttribIdList(int count, const ShaderAttribInfo* info )
{
	m_aid.clear();

	for (int i = 0; i < count; i++)
	{
		m_aid.push_back( GLExt::glGetAttribLocation(m_program, info[i].name) );
		Layout layout;

		layout.normalized = info[i].normalized;
		layout.type = info[i].type;
		layout.offset = info[i].offset;
		layout.count = info[i].count;

		m_layout.push_back(layout);
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
	for( size_t i = 0; i < m_aid.size(); i++ )
	{
		if ( m_aid[i] >= 0 ) 
		{
			GLExt::glEnableVertexAttribArray( m_aid[i] );
		}
	}
	GLCheckError();
}

void Shader::DisableAttribs()
{
	GLCheckError();

	for( size_t i = 0; i < m_aid.size(); i++ )
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
	const void* vertices = NULL;

	for( size_t i = 0; i < m_aid.size(); i++ )
	{
		if ( m_aid[i] >= 0) 
		{
			GLExt::glVertexAttribPointer(m_aid[i], m_layout[i].count, m_layout[i].type, m_layout[i].normalized, m_vertexSize, (uint8_t*) vertices + m_layout[i].offset);
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
			uint8_t* data = (uint8_t*) m_vertexConstantBuffer;
			data += m_vertexConstantLayout[i].Offset;
			GLExt::glUniformMatrix4fv(
				m_vertexConstantLayout[i].ID,
				1,
				GL_FALSE,
				(const GLfloat*)data);
		}
		else if( 
			CONSTANT_TYPE_MATRIX44_ARRAY_END > m_vertexConstantLayout[i].Type &&
			m_vertexConstantLayout[i].Type >= CONSTANT_TYPE_MATRIX44_ARRAY)
		{
			uint8_t* data = (uint8_t*) m_vertexConstantBuffer;
			data += m_vertexConstantLayout[i].Offset;
			GLExt::glUniformMatrix4fv(
				m_vertexConstantLayout[i].ID,
				m_vertexConstantLayout[i].Type - CONSTANT_TYPE_MATRIX44_ARRAY,
				GL_FALSE,
				(const GLfloat*)data);
		}

		else if (m_vertexConstantLayout[i].Type == CONSTANT_TYPE_VECTOR4)
		{
			uint8_t* data = (uint8_t*) m_vertexConstantBuffer;
			data += m_vertexConstantLayout[i].Offset;
			GLExt::glUniform4fv(
				m_vertexConstantLayout[i].ID,
				1,
				(const GLfloat*)data);
		}
		else if( 
			CONSTANT_TYPE_VECTOR4_ARRAY_END > m_vertexConstantLayout[i].Type &&
			m_vertexConstantLayout[i].Type >= CONSTANT_TYPE_VECTOR4_ARRAY)
		{
			uint8_t* data = (uint8_t*) m_vertexConstantBuffer;
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
			uint8_t* data = (uint8_t*) m_pixelConstantBuffer;
			data += m_pixelConstantLayout[i].Offset;
			GLExt::glUniformMatrix4fv(
				m_pixelConstantLayout[i].ID,
				1,
				GL_FALSE,
				(const GLfloat*) data);
		}
		else if( 
			CONSTANT_TYPE_MATRIX44_ARRAY_END > m_pixelConstantLayout[i].Type &&
			m_pixelConstantLayout[i].Type >= CONSTANT_TYPE_MATRIX44_ARRAY)
		{
			uint8_t* data = (uint8_t*) m_pixelConstantBuffer;
			data += m_pixelConstantLayout[i].Offset;
			GLExt::glUniformMatrix4fv(
				m_pixelConstantLayout[i].ID,
				m_pixelConstantLayout[i].Type - CONSTANT_TYPE_MATRIX44_ARRAY,
				GL_FALSE,
				(const GLfloat*)data);
		}

		else if (m_pixelConstantLayout[i].Type == CONSTANT_TYPE_VECTOR4)
		{
			uint8_t* data = (uint8_t*) m_pixelConstantBuffer;
			data += m_pixelConstantLayout[i].Offset;
			GLExt::glUniform4fv(
				m_pixelConstantLayout[i].ID,
				1,
				(const GLfloat*)data);
		}
		else if( 
			CONSTANT_TYPE_VECTOR4_ARRAY_END > m_pixelConstantLayout[i].Type &&
			m_pixelConstantLayout[i].Type >= CONSTANT_TYPE_VECTOR4_ARRAY)
		{
			uint8_t* data = (uint8_t*) m_pixelConstantBuffer;
			data += m_pixelConstantLayout[i].Offset;
			GLExt::glUniform4fv(
				m_pixelConstantLayout[i].ID,
				m_pixelConstantLayout[i].Type - CONSTANT_TYPE_VECTOR4_ARRAY,
				(const GLfloat*)data);
		}
	}
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

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
}