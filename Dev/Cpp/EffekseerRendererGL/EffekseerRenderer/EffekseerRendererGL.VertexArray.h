
#pragma once

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererGL.DeviceObject.h"
#include "EffekseerRendererGL.RendererImplemented.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
class VertexArray : public DeviceObject
{
private:
	GLuint m_vertexArray;

	Shader* m_shader;
	VertexBuffer* m_vertexBuffer;
	IndexBuffer* m_indexBuffer;

	VertexArray(const Backend::GraphicsDeviceRef& graphicsDevice, Shader* shader, VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer);

public:
	virtual ~VertexArray();

	static VertexArray*
	Create(const Backend::GraphicsDeviceRef& graphicsDevice, Shader* shader, VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer);

	GLuint GetInterface() const
	{
		return m_vertexArray;
	}

	VertexBuffer* GetVertexBuffer()
	{
		return m_vertexBuffer;
	}
	IndexBuffer* GetIndexBuffer()
	{
		return m_indexBuffer;
	}

public:
	void OnLostDevice() override;
	void OnResetDevice() override;

private:
	void Init();
	void Release();
};

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerRendererGL
  //-----------------------------------------------------------------------------------
  //
  //-----------------------------------------------------------------------------------