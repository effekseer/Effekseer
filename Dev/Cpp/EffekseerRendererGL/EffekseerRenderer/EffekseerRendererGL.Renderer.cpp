
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererGL.Renderer.h"
#include "EffekseerRendererGL.RendererImplemented.h"
#include "EffekseerRendererGL.RenderState.h"

#include "EffekseerRendererGL.Shader.h"
#include "EffekseerRendererGL.VertexBuffer.h"
#include "EffekseerRendererGL.IndexBuffer.h"
#include "EffekseerRendererGL.DeviceObject.h"
#include "EffekseerRendererGL.SpriteRenderer.h"
#include "EffekseerRendererGL.RibbonRenderer.h"
#include "EffekseerRendererGL.RingRenderer.h"
#include "EffekseerRendererGL.TrackRenderer.h"
#include "EffekseerRendererGL.ModelRenderer.h"
#include "EffekseerRendererGL.TextureLoader.h"
#include "EffekseerRendererGL.ModelLoader.h"

#include "EffekseerRendererGL.GLExtension.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Renderer* Renderer::Create( int32_t squareMaxCount )
{
	GLExt::Initialize();

	RendererImplemented* renderer = new RendererImplemented( squareMaxCount );
	if( renderer->Initialize() )
	{
		return renderer;
	}
	return NULL;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RendererImplemented::RendererImplemented( int32_t squareMaxCount )
	: m_reference	( 1 )
	, m_vertexBuffer( NULL )
	, m_indexBuffer	( NULL )
	, m_squareMaxCount	( squareMaxCount )
	, m_renderState		( NULL )
	, m_restorationOfStates(true)
{
	::Effekseer::Vector3D direction( 1.0f, 1.0f, 1.0f );
	SetLightDirection( direction );
	::Effekseer::Color lightColor( 255, 255, 255, 255 );
	SetLightColor( lightColor );
	::Effekseer::Color lightAmbient( 40, 40, 40, 255 );
	SetLightAmbientColor( lightAmbient );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RendererImplemented::~RendererImplemented()
{
	assert( m_reference == 0 );
	ES_SAFE_DELETE( m_renderState );
	ES_SAFE_DELETE( m_vertexBuffer );
	ES_SAFE_DELETE( m_indexBuffer );
	assert( m_reference == -2 );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::OnLostDevice()
{
	std::set<DeviceObject*>::iterator it = m_deviceObjects.begin();
	std::set<DeviceObject*>::iterator it_end = m_deviceObjects.end();
	while( it != it_end )
	{
		(*it)->OnLostDevice();
		it++;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::OnResetDevice()
{
	std::set<DeviceObject*>::iterator it = m_deviceObjects.begin();
	std::set<DeviceObject*>::iterator it_end = m_deviceObjects.end();
	while( it != it_end )
	{
		(*it)->OnResetDevice();
		it++;
	}

	{
		m_indexBuffer->Lock();

		// ( 標準設定で　DirectX 時計周りが表, OpenGLは反時計回りが表 )
		for( int i = 0; i < m_squareMaxCount; i++ )
		{
			uint16_t* buf = (uint16_t*)m_indexBuffer->GetBufferDirect( 6 );
			buf[0] = 3 + 4 * i;
			buf[1] = 1 + 4 * i;
			buf[2] = 0 + 4 * i;
			buf[3] = 3 + 4 * i;
			buf[4] = 0 + 4 * i;
			buf[5] = 2 + 4 * i;
		}

		m_indexBuffer->Unlock();
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool RendererImplemented::Initialize()
{
	// 頂点の生成
	{
		// 最大でfloat * 10 と仮定
		m_vertexBuffer = VertexBuffer::Create( this, sizeof(float) * 10 * m_squareMaxCount * 4, true );
		if( m_vertexBuffer == NULL ) return false;
	}

	// インデックスの生成
	{
		m_indexBuffer = IndexBuffer::Create( this, m_squareMaxCount * 6, false );
		if( m_indexBuffer == NULL ) return false;

		m_indexBuffer->Lock();

		// ( 標準設定で　DirectX 時計周りが表, OpenGLは反時計回りが表 )
		for( int i = 0; i < m_squareMaxCount; i++ )
		{
			uint16_t* buf = (uint16_t*)m_indexBuffer->GetBufferDirect( 6 );
			buf[0] = (uint16_t)(3 + 4 * i);
			buf[1] = (uint16_t)(1 + 4 * i);
			buf[2] = (uint16_t)(0 + 4 * i);
			buf[3] = (uint16_t)(3 + 4 * i);
			buf[4] = (uint16_t)(0 + 4 * i);
			buf[5] = (uint16_t)(2 + 4 * i);
		}

		m_indexBuffer->Unlock();
	}

	m_renderState = new RenderState( this );

	// 参照カウントの調整
	// m_vertexBufferの参照カウンタ
	Release();
	// m_indexBufferの参照カウンタ
	Release();

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int RendererImplemented::AddRef()
{
	m_reference++;
	return m_reference;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int RendererImplemented::Release()
{
	m_reference--;
	int count = m_reference;
	if ( count == 0 )
	{
		delete this;
	}
	return count;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::Destory()
{
	Release();
}

void RendererImplemented::SetRestorationOfStatesFlag(bool flag)
{
	m_restorationOfStates = flag;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool RendererImplemented::BeginRendering()
{
	GLCheckError();

	::Effekseer::Matrix44::Mul( m_cameraProj, m_camera, m_proj );

	// ステートを保存する
	if(m_restorationOfStates)
	{
		m_originalState.blend = glIsEnabled(GL_BLEND);
		m_originalState.cullFace = glIsEnabled(GL_CULL_FACE);
		m_originalState.depthTest = glIsEnabled(GL_DEPTH_TEST);
		m_originalState.texture = glIsEnabled(GL_TEXTURE_2D);
		glGetBooleanv(GL_DEPTH_WRITEMASK, &m_originalState.depthWrite);
		glGetIntegerv(GL_BLEND_SRC_RGB, &m_originalState.blendSrc);
		glGetIntegerv(GL_BLEND_DST_RGB, &m_originalState.blendDst);
		glGetIntegerv(GL_BLEND_EQUATION, &m_originalState.blendEquation);
	}

	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);

	m_renderState->GetActiveState().Reset();
	m_renderState->Update( true );
	m_currentTextures.clear();

	GLCheckError();

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool RendererImplemented::EndRendering()
{
	GLCheckError();

	// ステートを復元する
	if(m_restorationOfStates)
	{
		if (m_originalState.blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
		if (m_originalState.cullFace) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
		if (m_originalState.depthTest) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
		if (m_originalState.texture) glEnable(GL_TEXTURE_2D); else glDisable(GL_TEXTURE_2D);
		
		glDepthMask(m_originalState.depthWrite);
		glBlendFunc(m_originalState.blendSrc, m_originalState.blendDst);
		GLExt::glBlendEquation(m_originalState.blendEquation);
	}

	GLCheckError();

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
VertexBuffer* RendererImplemented::GetVertexBuffer()
{
	return m_vertexBuffer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
IndexBuffer* RendererImplemented::GetIndexBuffer()
{
	return m_indexBuffer;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int32_t RendererImplemented::GetSquareMaxCount() const
{
	return m_squareMaxCount;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::EffekseerRenderer::RenderStateBase* RendererImplemented::GetRenderState()
{
	return m_renderState;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
const ::Effekseer::Vector3D& RendererImplemented::GetLightDirection() const
{
	return m_lightDirection;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetLightDirection( ::Effekseer::Vector3D& direction )
{
	m_lightDirection = direction;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
const ::Effekseer::Color& RendererImplemented::GetLightColor() const
{
	return m_lightColor;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetLightColor( ::Effekseer::Color& color )
{
	m_lightColor = color;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
const ::Effekseer::Color& RendererImplemented::GetLightAmbientColor() const
{
	return m_lightAmbient;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetLightAmbientColor( ::Effekseer::Color& color )
{
	m_lightAmbient = color;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
const ::Effekseer::Matrix44& RendererImplemented::GetProjectionMatrix() const
{
	return m_proj;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetProjectionMatrix( const ::Effekseer::Matrix44& mat )
{
	m_proj = mat;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
const ::Effekseer::Matrix44& RendererImplemented::GetCameraMatrix() const
{
	return m_camera;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetCameraMatrix( const ::Effekseer::Matrix44& mat )
{
	m_camera = mat;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::Matrix44& RendererImplemented::GetCameraProjectionMatrix()
{
	return m_cameraProj;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::SpriteRenderer* RendererImplemented::CreateSpriteRenderer()
{
	return SpriteRenderer::Create( this );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::RibbonRenderer* RendererImplemented::CreateRibbonRenderer()
{
	return RibbonRenderer::Create( this );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::RingRenderer* RendererImplemented::CreateRingRenderer()
{
	return RingRenderer::Create( this );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::ModelRenderer* RendererImplemented::CreateModelRenderer()
{
	return ModelRenderer::Create( this );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::TrackRenderer* RendererImplemented::CreateTrackRenderer()
{
	return TrackRenderer::Create( this );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::TextureLoader* RendererImplemented::CreateTextureLoader( ::Effekseer::FileInterface* fileInterface )
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	return new TextureLoader( fileInterface );
#else
	return NULL;
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::ModelLoader* RendererImplemented::CreateModelLoader( ::Effekseer::FileInterface* fileInterface )
{
#ifdef __EFFEKSEER_RENDERER_INTERNAL_LOADER__
	return new ModelLoader( fileInterface );
#else
	return NULL;
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetVertexBuffer( VertexBuffer* vertexBuffer, int32_t size )
{
	GLExt::glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->GetInterface() );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetVertexBuffer(GLuint vertexBuffer, int32_t size)
{
	GLExt::glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetIndexBuffer( IndexBuffer* indexBuffer )
{
	GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->GetInterface() );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetIndexBuffer(GLuint indexBuffer)
{
	GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetLayout(Shader* shader)
{
	GLCheckError();

	shader->EnableAttribs();
	shader->SetVertex();

	GLCheckError();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::DrawSprites( int32_t spriteCount, int32_t vertexOffset )
{
	GLCheckError();

	assert( vertexOffset == 0 );
	glDrawElements(GL_TRIANGLES, spriteCount * 6, GL_UNSIGNED_SHORT, NULL);

	GLCheckError();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::DrawPolygon( int32_t vertexCount, int32_t indexCount)
{
	GLCheckError();

	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, NULL);

	GLCheckError();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::BeginShader(Shader* shader)
{
	GLCheckError();

	shader->BeginScene();

	GLCheckError();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::EndShader(Shader* shader)
{
	GLCheckError();

	shader->DisableAttribs();
	GLCheckError();

	GLExt::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	GLCheckError();

	GLExt::glBindBuffer(GL_ARRAY_BUFFER, 0);
	GLCheckError();

	shader->EndScene();
	GLCheckError();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetTextures(Shader* shader, GLuint* textures, int32_t count)
{
	GLCheckError();

	m_currentTextures.clear();
	m_currentTextures.resize(count);

	for (int32_t i = 0; i < count; i++)
	{
		GLExt::glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		
		m_currentTextures[i] = textures[i];

		if (shader->GetTextureSlotEnable(i))
		{
			GLExt::glUniform1i(shader->GetTextureSlot(i), i);
		}
	}
	GLExt::glActiveTexture(GL_TEXTURE0);

	GLCheckError();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::ResetRenderState()
{
	m_renderState->GetActiveState().Reset();
	m_renderState->Update( true );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Model::Model( ::Effekseer::Model::Vertex vertexData[], int32_t vertexCount, 
	::Effekseer::Model::Face faceData[], int32_t faceCount )
	: VertexBuffer	( 0 )
	, IndexBuffer	( 0 )
	, VertexCount	( vertexCount )
	, IndexCount	( faceCount * 3 )
	, ModelCount	( 1 )
{
	GLExt::glGenBuffers( 1, &VertexBuffer );
	GLExt::glBindBuffer( GL_ARRAY_BUFFER, VertexBuffer );
	size_t vertexSize = vertexCount * sizeof(::Effekseer::Model::Vertex);
	GLExt::glBufferData( GL_ARRAY_BUFFER, vertexSize, vertexData, GL_STATIC_DRAW );

	GLExt::glGenBuffers( 1, &IndexBuffer );
	GLExt::glBindBuffer( GL_ARRAY_BUFFER, IndexBuffer );
	size_t indexSize = faceCount * sizeof(::Effekseer::Model::Face);
	GLExt::glBufferData( GL_ARRAY_BUFFER, indexSize, faceData, GL_STATIC_DRAW );
	
	GLExt::glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
Model::~Model()
{
	GLExt::glDeleteBuffers( 1, &IndexBuffer );
	GLExt::glDeleteBuffers( 1, &VertexBuffer );
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
