
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#include "EffekseerRendererGL.VertexArray.h"
#include "EffekseerRendererGL.Shader.h"
#include "EffekseerRendererGL.VertexBuffer.h"
#include "EffekseerRendererGL.IndexBuffer.h"
#include "EffekseerRendererGL.GLExtension.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
VertexArray::VertexArray( RendererImplemented* renderer, Shader* shader, 
	VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer )
	: DeviceObject		( renderer )
	, m_shader			( shader )
	, m_vertexBuffer	( vertexBuffer )
	, m_indexBuffer		( indexBuffer )
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
VertexArray* VertexArray::Create( RendererImplemented* renderer, Shader* shader, 
	VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer )
{
	if (GLExt::IsSupportedVertexArray())
	{
		return new VertexArray( renderer, shader, vertexBuffer, indexBuffer );
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
	GLExt::glGenVertexArrays(1, &m_vertexArray);

	GLExt::glBindVertexArray(m_vertexArray);
	
	if (m_vertexBuffer != nullptr)
	{
		GLExt::glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer->GetInterface());
	}
	
	if (m_indexBuffer != nullptr)
	{
		GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer->GetInterface());
	}

	m_shader->EnableAttribs();
	m_shader->SetVertex();

	GLExt::glBindVertexArray(0);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexArray::Release()
{
	if (m_vertexArray != 0)
	{
		GLExt::glDeleteVertexArrays(1, &m_vertexArray);
		m_vertexArray = 0;
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------