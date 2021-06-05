
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

using ShaderCodeView = Effekseer::StringView<char>;

const char* GetVertexShaderHeader(OpenGLDeviceType deviceType);

const char* GetFragmentShaderHeader(OpenGLDeviceType deviceType);

class Shader : public DeviceObject, public ::EffekseerRenderer::ShaderBase
{
private:
	OpenGLDeviceType m_deviceType;
	Backend::GraphicsDeviceRef graphicsDevice_;
	Backend::ShaderRef shader_;
	Backend::ShaderRef shaderOverride_;
	Backend::VertexLayoutRef vertexLayout_;

	int32_t m_vertexSize;

	Backend::UniformBufferRef vertexConstantBuffer_;
	Backend::UniformBufferRef pixelConstantBuffer_;

	std::string name_;

	Effekseer::CustomVector<GLint> attribs_;

	bool isTransposeEnabled_ = false;

	GLint baseInstance_ = -1;

	Backend::ShaderRef& GetCurrentShader();

	const Backend::ShaderRef& GetCurrentShader() const;

	void AssignAttribs();

	Shader(const Backend::GraphicsDeviceRef& graphicsDevice,
		   Backend::ShaderRef shader,
		   const char* name);

public:
	virtual ~Shader() override = default;

	static Shader* Create(const Backend::GraphicsDeviceRef& graphicsDevice,
						  Backend::ShaderRef shader,
						  const char* name);

	static Shader* Create(const Backend::GraphicsDeviceRef& graphicsDevice, const Effekseer::CustomVector<Effekseer::StringView<char>>& vsCodes, const Effekseer::CustomVector<Effekseer::StringView<char>>& psCodes, Effekseer::Backend::UniformLayoutRef layout, const char* name)
	{
		auto shader = graphicsDevice->CreateShaderFromCodes(vsCodes, psCodes, layout).DownCast<Backend::Shader>();
		return Create(graphicsDevice, shader, name);
	}

	static Shader* CreateWithHeader(const Backend::GraphicsDeviceRef& graphicsDevice, const Effekseer::StringView<char>& vsCode, const Effekseer::StringView<char>& psCode, Effekseer::Backend::UniformLayoutRef layout, const char* name)
	{

		auto shader = graphicsDevice->CreateShaderFromCodes(
										{{GetVertexShaderHeader(graphicsDevice->GetDeviceType())}, vsCode},
										{{GetFragmentShaderHeader(graphicsDevice->GetDeviceType())}, psCode},
										layout)
						  .DownCast<Backend::Shader>();
		return Create(graphicsDevice, shader, name);
	}

public:
	virtual void OnLostDevice() override
	{
	}
	virtual void OnResetDevice() override;

public:
	void OverrideShader(::Effekseer::Backend::ShaderRef shader) override;

	GLuint GetInterface() const;

	void SetVertexLayout(Backend::VertexLayoutRef vertexLayout);

	void BeginScene();
	void EndScene();
	void EnableAttribs();
	void DisableAttribs();

	void SetVertexConstantBufferSize(int32_t size) override;
	void SetPixelConstantBufferSize(int32_t size) override;

	void* GetVertexConstantBuffer() override
	{
		return vertexConstantBuffer_ != nullptr ? vertexConstantBuffer_->GetBuffer().data() : nullptr;
	}
	void* GetPixelConstantBuffer() override
	{
		return pixelConstantBuffer_ != nullptr ? pixelConstantBuffer_->GetBuffer().data() : nullptr;
	}

	void SetConstantBuffer() override;

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
