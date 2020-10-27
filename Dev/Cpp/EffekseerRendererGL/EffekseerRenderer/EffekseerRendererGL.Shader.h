
#ifndef __EFFEKSEERRENDERER_GL_SHADER_H__
#define __EFFEKSEERRENDERER_GL_SHADER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererGL.DeviceObject.h"
#include "EffekseerRendererGL.RendererImplemented.h"

#include "../../EffekseerRendererCommon/EffekseerRenderer.ShaderBase.h"

#include <string>
#include <vector>

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
	const char* name;
	GLenum type;
	uint16_t count;
	uint16_t offset;
	bool normalized;
};

struct ShaderUniformInfo
{
	const char* name;
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
	: public DeviceObject,
	  public ::EffekseerRenderer::ShaderBase
{
private:
	struct Layout
	{
		GLenum type;
		uint16_t count;
		uint16_t offset;
		bool normalized;
	};

	struct ShaderAttribInfoInternal
	{
		std::string name;
		GLenum type;
		uint16_t count;
		uint16_t offset;
		bool normalized;
	};

	struct ShaderUniformInfoInternal
	{
		std::string name;
	};

	struct ConstantLayout
	{
		eConstantType Type;
		GLint ID;
		int32_t Offset;
	};

	OpenGLDeviceType deviceType_;
	GLuint m_program;

	std::vector<GLint> m_aid;
	std::vector<Layout> m_layout;

	int32_t m_vertexSize;

	uint8_t* m_vertexConstantBuffer;
	uint8_t* m_pixelConstantBuffer;

	std::vector<ConstantLayout> m_vertexConstantLayout;
	std::vector<ConstantLayout> m_pixelConstantLayout;

	std::array<GLuint, Effekseer::TextureSlotMax> m_textureSlots;
	std::array<bool, Effekseer::TextureSlotMax> m_textureSlotEnables;

	std::vector<char> m_vsSrc;
	std::vector<char> m_psSrc;
	std::string m_name;

	std::vector<ShaderAttribInfoInternal> attribs;
	std::vector<ShaderUniformInfoInternal> uniforms;

	static bool CompileShader(
		OpenGLDeviceType deviceType,
		GLuint& program,
		const char* vs_src,
		size_t vertexShaderSize,
		const char* fs_src,
		size_t pixelShaderSize,
		const char* name);

	Shader(OpenGLDeviceType deviceType,
		   Renderer* renderer,
		   DeviceObjectCollection* deviceObjectCollection,
		   GLuint program,
		   const char* vs_src,
		   size_t vertexShaderSize,
		   const char* fs_src,
		   size_t pixelShaderSize,
		   const char* name,
		   bool hasRefCount);

	GLint GetAttribId(const char* name) const;

public:
	GLint GetUniformId(const char* name) const;

public:
	virtual ~Shader();

	static Shader* Create(
		OpenGLDeviceType deviceType,
		DeviceObjectCollection* deviceObjectCollection,
		const char* vs_src,
		size_t vertexShaderSize,
		const char* fs_src,
		size_t pixelShaderSize,
		const char* name,
		bool hasRefCount);

	static Shader* Create(
		Renderer* renderer,
		const char* vs_src,
		size_t vertexShaderSize,
		const char* fs_src,
		size_t pixelShaderSize,
		const char* name);

public:
	virtual void OnLostDevice() override;
	virtual void OnResetDevice() override;
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

	void SetVertexConstantBufferSize(int32_t size) override;
	void SetPixelConstantBufferSize(int32_t size) override;

	void* GetVertexConstantBuffer() override
	{
		return m_vertexConstantBuffer;
	}
	void* GetPixelConstantBuffer() override
	{
		return m_pixelConstantBuffer;
	}

	void AddVertexConstantLayout(eConstantType type, GLint id, int32_t offset);
	void AddPixelConstantLayout(eConstantType type, GLint id, int32_t offset);

	void SetConstantBuffer() override;

	void SetTextureSlot(int32_t index, GLuint value);
	GLuint GetTextureSlot(int32_t index);
	bool GetTextureSlotEnable(int32_t index);

	bool IsValid() const;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererGL

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_GL_SHADER_H__
