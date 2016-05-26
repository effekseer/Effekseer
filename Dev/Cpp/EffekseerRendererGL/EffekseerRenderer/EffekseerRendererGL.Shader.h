
#ifndef	__EFFEKSEERRENDERER_GL_SHADER_H__
#define	__EFFEKSEERRENDERER_GL_SHADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererGL.RendererImplemented.h"
#include "EffekseerRendererGL.DeviceObject.h"

#include <vector>
#include <string>

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
struct ShaderAttribInfo
{
	const char*	name;
	GLenum		type;
	uint16_t	count;
	uint16_t	offset;
	bool		normalized;
};

struct ShaderUniformInfo
{
	const char*	name;
};

enum eConstantType
{
	CONSTANT_TYPE_MATRIX44 = 0,
	CONSTANT_TYPE_MATRIX44_ARRAY = 10,
	CONSTANT_TYPE_MATRIX44_ARRAY_END = 99,

	CONSTANT_TYPE_VECTOR4 = 100,
	CONSTANT_TYPE_VECTOR4_ARRAY = 110,
	CONSTANT_TYPE_VECTOR4_ARRAY_END = 199,
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class Shader
	: public DeviceObject
{
private:
	struct Layout
	{
		GLenum		type;
		uint16_t	count;
		uint16_t	offset;
		bool		normalized;
	};

	struct ConstantLayout
	{
		eConstantType	Type;
		GLint			ID;
		int32_t			Offset;
	};

	GLuint m_program;

	std::vector<GLint>		m_aid;
	std::vector<Layout>		m_layout;

	int32_t					m_vertexSize;

	uint8_t*					m_vertexConstantBuffer;
	uint8_t*					m_pixelConstantBuffer;

	std::vector<ConstantLayout>	m_vertexConstantLayout;
	std::vector<ConstantLayout>	m_pixelConstantLayout;

	GLuint	m_textureSlots[4];
	bool	m_textureSlotEnables[4];

	std::vector<uint8_t>	m_vsSrc;
	std::vector<uint8_t>	m_psSrc;
	std::string				m_name;

	static bool CompileShader(
		GLuint& program,
		const char* vs_src,
		int32_t vertexShaderSize,
		const char* fs_src,
		int32_t pixelShaderSize,
		const char* name);

	Shader(
		RendererImplemented* renderer, 
		GLuint program,
		const char* vs_src,
		int32_t vertexShaderSize,
		const char* fs_src,
		int32_t pixelShaderSize,
		const char* name);

	GLint GetAttribId(const char* name) const;

public:
	GLint GetUniformId(const char* name) const;

public:
	virtual ~Shader();

	static Shader* Create(
		RendererImplemented* renderer,
		const char* vs_src,
		int32_t vertexShaderSize,
		const char* fs_src,
		int32_t pixelShaderSize,
		const char* name);

public:	// デバイス復旧用
	virtual void OnLostDevice();
	virtual void OnResetDevice();
	virtual void OnChangeDevice();

public:
	GLuint GetInterface() const;

	void GetAttribIdList(int count, const ShaderAttribInfo* info);
	void GetUniformIdList(int count, const ShaderUniformInfo* info, GLint* uid_list) const;

	void BeginScene();
	void EndScene();
	void EnableAttribs();
	void DisableAttribs();
	void SetVertex();

	void SetVertexSize(int32_t vertexSize);

	void SetVertexConstantBufferSize(int32_t size);
	void SetPixelConstantBufferSize(int32_t size);

	void* GetVertexConstantBuffer() { return m_vertexConstantBuffer; }
	void* GetPixelConstantBuffer() { return m_pixelConstantBuffer; }

	void AddVertexConstantLayout(eConstantType type, GLint id, int32_t offset);
	void AddPixelConstantLayout(eConstantType type, GLint id, int32_t offset);

	void SetConstantBuffer();

	void SetTextureSlot(int32_t index, GLuint value);
	GLuint GetTextureSlot(int32_t index);
	bool GetTextureSlotEnable(int32_t index);
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

#if  defined(__EFFEKSEER_RENDERER_GL3__) 
#define EFFEKSEER_VERTEX_SHADER_HEADER \
R"(#version 330
#define lowp
#define mediump
#define highp
#define IN in
#define OUT out
)"
#define EFFEKSEER_FRAGMENT_SHADER_HEADER \
R"(#version 330
#define lowp
#define mediump
#define highp
#define IN in
#define TEX2D texture
layout (location = 0) out vec4 FRAGCOLOR;
)"
#elif defined(__EFFEKSEER_RENDERER_GLES3__)
#define EFFEKSEER_VERTEX_SHADER_HEADER \
"#version 300 es\n" \
"precision mediump float;\n" \
"#define IN in\n" \
"#define OUT out\n"
#define EFFEKSEER_FRAGMENT_SHADER_HEADER \
"#version 300 es\n" \
"precision mediump float;\n" \
"#define IN in\n" \
"#define TEX2D texture\n" \
"layout (location = 0) out vec4 FRAGCOLOR;\n"
#elif defined(__EFFEKSEER_RENDERER_GLES2__)
#define EFFEKSEER_VERTEX_SHADER_HEADER \
R"(
precision mediump float;
#define IN attribute
#define OUT varying
)"
#define EFFEKSEER_FRAGMENT_SHADER_HEADER \
R"(
precision mediump float;
#define IN varying
#define TEX2D texture2D
#define FRAGCOLOR gl_FragColor
)"
#else
#define EFFEKSEER_VERTEX_SHADER_HEADER \
R"(#version 110
#define lowp
#define mediump
#define highp
#define IN attribute
#define OUT varying
)"
#define EFFEKSEER_FRAGMENT_SHADER_HEADER \
R"(#version 110
#define lowp
#define mediump
#define highp
#define IN varying
#define TEX2D texture2D
#define FRAGCOLOR gl_FragColor
)"
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_GL_SHADER_H__