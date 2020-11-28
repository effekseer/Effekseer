
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
	CONSTANT_TYPE_VECTOR4 = 100,
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

struct ShaderCodeView
{
	const char* Data;
	int32_t Length;

	ShaderCodeView()
		: Data(nullptr)
		, Length(0)
	{
	}

	ShaderCodeView(const char* data)
		: Data(data)
		, Length(static_cast<int32_t>(strlen(data)))
	{
	}
};

class Shader : public DeviceObject, public ::EffekseerRenderer::ShaderBase
{
private:
	struct ShaderCode
	{
		std::vector<char> Code;
	};

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
		int32_t Count;
	};

	OpenGLDeviceType m_deviceType;
	GLuint m_program;

	std::vector<GLint> m_aid;
	std::vector<Layout> m_layout;

	int32_t m_vertexSize;

	uint8_t* m_vertexConstantBuffer;
	uint8_t* m_pixelConstantBuffer;
	bool addHeader_ = true;

	std::vector<ConstantLayout> m_vertexConstantLayout;
	std::vector<ConstantLayout> m_pixelConstantLayout;

	std::array<GLint, Effekseer::TextureSlotMax> m_textureSlots;
	std::array<bool, Effekseer::TextureSlotMax> m_textureSlotEnables;

	std::string name_;
	std::vector<ShaderCode> vsCodes_;
	std::vector<ShaderCode> psCodes_;

	std::vector<ShaderAttribInfoInternal> attribs;
	std::vector<ShaderUniformInfoInternal> uniforms;

	bool isTransposeEnabled_ = false;

	GLint baseInstance_ = -1;

	static bool CompileShader(OpenGLDeviceType deviceType,
							  GLuint& program,
							  const ShaderCodeView* vsData,
							  size_t vsDataCount,
							  const ShaderCodeView* psData,
							  size_t psDataCount,
							  const char* name,
							  bool addHeader);

	bool ReloadShader();

	Shader(const Backend::GraphicsDeviceRef& graphicsDevice,
		   GLuint program,
		   const ShaderCodeView* vsData,
		   size_t vsDataCount,
		   const ShaderCodeView* psData,
		   size_t psDataCount,
		   const char* name,
		   bool hasRefCount,
		   bool addHeader);

	GLint GetAttribId(const char* name) const;

public:
	GLint GetUniformId(const char* name) const;

public:
	virtual ~Shader();

	static Shader* Create(const Backend::GraphicsDeviceRef& graphicsDevice,
						  const ShaderCodeView* vsData,
						  size_t vsDataCount,
						  const ShaderCodeView* psData,
						  size_t psDataCount,
						  const char* name,
						  bool hasRefCount = true,
						  bool addHeader = true);

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

	void AddVertexConstantLayout(eConstantType type, GLint id, int32_t offset, int32_t count = 1);
	void AddPixelConstantLayout(eConstantType type, GLint id, int32_t offset, int32_t count = 1);

	void SetConstantBuffer() override;

	void SetTextureSlot(int32_t index, GLint value);
	GLint GetTextureSlot(int32_t index);
	bool GetTextureSlotEnable(int32_t index);

	void SetIsTransposeEnabled(bool isTransposeEnabled)
	{
		isTransposeEnabled_ = isTransposeEnabled;
	}

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
