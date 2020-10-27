
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#include "EffekseerRendererGL.VertexArray.h"
#include "EffekseerRendererGL.GLExtension.h"
#include "EffekseerRendererGL.IndexBuffer.h"
#include "EffekseerRendererGL.Shader.h"
#include "EffekseerRendererGL.VertexBuffer.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
VertexArray::VertexArray(RendererImplemented* renderer, Shader* shader, VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer, bool hasRefCount)
	: DeviceObject(renderer, renderer->GetDeviceObjectCollection(), hasRefCount)
	, m_shader(shader)
	, m_vertexBuffer(vertexBuffer)
	, m_indexBuffer(indexBuffer)
{
	Init();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
VertexArray::~VertexArray()
{
	Release();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
VertexArray* VertexArray::Create(RendererImplemented* renderer, Shader* shader, VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer, bool hasRefCount)
{
	if (GLExt::IsSupportedVertexArray())
	{
		return new VertexArray(renderer, shader, vertexBuffer, indexBuffer, hasRefCount);
	}
	return nullptr;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexArray::OnLostDevice()
{
	Release();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexArray::OnResetDevice()
{
	Init();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexArray::Init()
{
	if (!m_shader->IsValid())
	{
		m_shader->OnResetDevice();
	}

	GLCheckError();

	GLExt::glGenVertexArrays(1, &m_vertexArray);

	GLExt::glBindVertexArray(m_vertexArray);

	if (m_vertexBuffer != nullptr)
	{
		if (!m_vertexBuffer->IsValid())
		{
			m_vertexBuffer->OnResetDevice();
		}

		GLExt::glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer->GetInterface());
	}

	if (m_indexBuffer != nullptr)
	{
		if (!m_indexBuffer->IsValid())
		{
			m_indexBuffer->OnResetDevice();
		}

		GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer->GetInterface());
	}

	if (m_vertexBuffer != nullptr)
	{
		m_shader->EnableAttribs();
		m_shader->SetVertex();
	}

	GLExt::glBindVertexArray(0);

	GLCheckError();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexArray::Release()
{
	GLCheckError();

	if (m_vertexArray != 0)
	{
		GLExt::glDeleteVertexArrays(1, &m_vertexArray);
		m_vertexArray = 0;
	}

	GLCheckError();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerRendererGL
  //-----------------------------------------------------------------------------------
  //
  //-----------------------------------------------------------------------------------