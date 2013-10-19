
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#include "EffekseerRenderer.Shader.h"
#include "EffekseerRenderer.Renderer.h"

#if _WIN32
#include <windows.h>
#define LOG(s)	OutputDebugStringA(s)
#else
#define LOG(s)	printf(s)
#endif

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
bool Shader::CompileShader(
	GLuint& program,
	const char* vs_src,
	int32_t vertexShaderSize,
	const char* fs_src,
	int32_t pixelShaderSize,
	const char* name)
{
	const char* src_data[1];
	GLint src_size[1];

	GLuint vert_shader, frag_shader;
	GLint res_vs, res_fs, res_link;
	

	// バーテックスシェーダをコンパイル
	src_data[0] = vs_src;
	src_size[0] = (GLint)strlen(vs_src);
	vert_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert_shader, 1, src_data, src_size);
	glCompileShader(vert_shader);
	glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &res_vs);

	// フラグメントシェーダをコンパイル
	src_data[0] = fs_src;
	src_size[0] = strlen(fs_src);
	frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader, 1, src_data, src_size);
	glCompileShader(frag_shader);
	glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &res_fs);
	
	// シェーダプログラムの作成
	program = glCreateProgram();
	glAttachShader(program, vert_shader);
	glAttachShader(program, frag_shader);
	
	// シェーダオブジェクトの削除
	glDeleteShader(frag_shader);
	glDeleteShader(vert_shader);

	// シェーダプログラムのリンク
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &res_link);

	#ifndef NDEBUG
	{
		// エラー出力
		char log[512];
		int32_t log_size;
		glGetShaderInfoLog(vert_shader, sizeof(log), &log_size, log);
		if (log_size > 0) {
			LOG("Vertex Shader:\n");
			LOG(log);
		}
		glGetShaderInfoLog(frag_shader, sizeof(log), &log_size, log);
		if (log_size > 0) {
			LOG("Fragment Shader:\n");
			LOG(log);
		}
		glGetProgramInfoLog(program, sizeof(log), &log_size, log);
		if (log_size > 0) {
			LOG("Shader Link:\n");
			LOG(log);
		}
	}
#endif

	if (res_link == GL_FALSE) {
		glDeleteProgram(program);
		printf("Failed to compile shader \"\".", name);
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
	return glGetAttribLocation(m_program, name);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
GLint Shader::GetUniformId(const char* name) const
{
	return glGetUniformLocation(m_program, name);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
Shader::~Shader()
{
	glDeleteProgram(m_program);
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
		return nullptr;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Shader::OnLostDevice()
{
	glDeleteProgram(m_program);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Shader::OnResetDevice()
{
	GLuint program;
	
	if(CompileShader(
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
	glDeleteProgram(m_program);

	GLuint program;
	
	if(CompileShader(
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
		m_aid.push_back( glGetAttribLocation(m_program, info[i].name) );
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
		uid_list[i] = glGetUniformLocation(m_program, info[i].name);
	}
}

void Shader::BeginScene()
{
	glUseProgram(m_program);
}

void Shader::EndScene()
{
	glUseProgram(0);
}

void Shader::EnableAttribs()
{
	for( size_t i = 0; i < m_aid.size(); i++ )
	{
		if ( m_aid[i] >= 0 ) 
		{
			glEnableVertexAttribArray( m_aid[i] );
		}
	}
}

void Shader::DisableAttribs()
{
	for( size_t i = 0; i < m_aid.size(); i++ )
	{
		glDisableVertexAttribArray( m_aid[i] );
	}
}

void Shader::SetVertex()
{
	const void* vertices = NULL;

	for( size_t i = 0; i < m_aid.size(); i++ )
	{
		if ( m_aid[i] >= 0) 
		{
			glVertexAttribPointer(m_aid[i], m_layout[i].count, m_layout[i].type, m_layout[i].normalized, m_vertexSize, (uint8_t*) vertices + m_layout[i].offset);
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
			glUniformMatrix4fv(
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
			glUniformMatrix4fv(
				m_vertexConstantLayout[i].ID,
				m_vertexConstantLayout[i].Type - CONSTANT_TYPE_MATRIX44_ARRAY,
				GL_FALSE,
				(const GLfloat*)data);
		}

		else if (m_vertexConstantLayout[i].Type == CONSTANT_TYPE_VECTOR4)
		{
			uint8_t* data = (uint8_t*) m_vertexConstantBuffer;
			data += m_vertexConstantLayout[i].Offset;
			glUniform4fv(
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
			glUniform4fv(
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
			glUniformMatrix4fv(
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
			glUniformMatrix4fv(
				m_pixelConstantLayout[i].ID,
				m_pixelConstantLayout[i].Type - CONSTANT_TYPE_MATRIX44_ARRAY,
				GL_FALSE,
				(const GLfloat*)data);
		}

		else if (m_pixelConstantLayout[i].Type == CONSTANT_TYPE_VECTOR4)
		{
			uint8_t* data = (uint8_t*) m_pixelConstantBuffer;
			data += m_pixelConstantLayout[i].Offset;
			glUniform4fv(
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
			glUniform4fv(
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