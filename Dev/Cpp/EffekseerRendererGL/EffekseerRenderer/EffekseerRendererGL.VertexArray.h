
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
class VertexArray
	: public DeviceObject
{
private:
	GLuint m_vertexArray;

	Shader* m_shader;
	VertexBuffer* m_vertexBuffer;
	IndexBuffer* m_indexBuffer;

	VertexArray(RendererImplemented* renderer, Shader* shader, VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer, bool hasRefCount);

public:
	virtual ~VertexArray();

	static VertexArray*
	Create(RendererImplemented* renderer, Shader* shader, VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer, bool hasRefCount);

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

public: // デバイス復旧用
	virtual void OnLostDevice();
	virtual void OnResetDevice();

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