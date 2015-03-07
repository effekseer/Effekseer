
#ifndef	__EFFEKSEERRENDERER_STANDARD_RENDERER_BASE_H__
#define	__EFFEKSEERRENDERER_STANDARD_RENDERER_BASE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <Effekseer.h>

#include "EffekseerRenderer.VertexBufferBase.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

struct StandardRendererState
{
	bool								DepthTest;
	bool								DepthWrite;
	::Effekseer::AlphaBlendType			AlphaBlend;
	::Effekseer::CullingType			CullingType;
	::Effekseer::TextureFilterType		TextureFilterType;
	::Effekseer::TextureWrapType		TextureWrapType;
	void*								TexturePtr;

	StandardRendererState()
	{
		DepthTest = false;
		DepthWrite = false;
		AlphaBlend = ::Effekseer::AlphaBlendType::Blend;
		CullingType = ::Effekseer::CullingType::Front;
		TextureFilterType = ::Effekseer::TextureFilterType::Nearest;
		TextureWrapType = ::Effekseer::TextureWrapType::Repeat;
		TexturePtr = NULL;
	}

	bool operator != (const StandardRendererState state)
	{
		if (DepthTest != state.DepthTest) return false;
		if (DepthWrite != state.DepthWrite) return false;
		if (AlphaBlend != state.AlphaBlend) return false;
		if (CullingType != state.CullingType) return false;
		if (TextureFilterType != state.TextureFilterType) return false;
		if (TextureWrapType != state.TextureWrapType) return false;
		if (TexturePtr != state.TexturePtr) return false;
		return true;
	}
};

template<typename RENDERER, typename SHADER, typename TEXTURE, typename VERTEX>
class StandardRenderer
{

private:
	RENDERER*	m_renderer;
	SHADER*		m_shader;
	SHADER*		m_shader_no_texture;
	TEXTURE*	m_texture;

	StandardRendererState		m_state;

	int32_t		vertexCount = 0;
	int32_t		vertexOffsetSize = -1;

public:

	StandardRenderer(RENDERER* renderer, SHADER* shader, SHADER* shader_no_texture)
	{
		m_renderer = renderer;
		m_shader = shader;
		m_shader_no_texture = shader_no_texture;
	}

	void UpdateStateAndRenderingIfRequired(StandardRendererState state)
	{
		if(m_state != state)
		{
			Rendering();
		}

		m_state = state;
	}

	void BeginRenderingAndRenderingIfRequired(int32_t count, int32_t& offset, void*& data)
	{
		VertexBufferBase* vb = m_renderer->GetVertexBuffer();

		if (vb->TryRingBufferLock(count * sizeof(VERTEX), offset, data))
		{
			
		}
		else
		{
			Rendering();

			vb->RingBufferLock(count * sizeof(VERTEX), offset, data);
		}

		vertexCount += count;
		
		if (vertexOffsetSize < 0)
		{
			vertexOffsetSize = offset;
		}
	}

	void ResetAndRenderingIfRequired()
	{
		Rendering();

		// •K‚¸ŽŸ‚Ì•`‰æ‚Å‰Šú‰»‚³‚ê‚éB
		m_state.TexturePtr = (void*)0x1;
	}

	void Rendering()
	{
		if (vertexCount == 0) return;

		m_renderer->BeginShader(m_shader);

		RenderStateBase::State& state = m_renderer->GetRenderState()->Push();
		state.DepthTest = m_state.DepthTest;
		state.DepthWrite = m_state.DepthWrite;
		state.CullingType = m_state.CullingType;

		SHADER* shader_ = nullptr;
		if (m_state.TexturePtr != nullptr)
		{
			shader_ = m_shader;
		}
		else
		{
			shader_ = m_shader_no_texture;
		}

		if (m_state.TexturePtr != nullptr)
		{
			TEXTURE texture = TexturePointerToTexture<TEXTURE>(m_state.TexturePtr);
			m_renderer->SetTextures(shader_, &texture, 1);
		}
		else
		{
			TEXTURE texture = 0;
			m_renderer->SetTextures(shader_, &texture, 1);
		}

		((Effekseer::Matrix44*)(shader_->GetVertexConstantBuffer()))[0] = m_renderer->GetCameraProjectionMatrix();
		shader_->SetConstantBuffer();

		state.AlphaBlend = m_state.AlphaBlend;
		state.TextureFilterTypes[0] = m_state.TextureFilterType;
		state.TextureWrapTypes[0] = m_state.TextureWrapType;

		m_renderer->GetRenderState()->Update(false);

		m_renderer->SetVertexBuffer(m_renderer->GetVertexBuffer(), sizeof(VERTEX));
		m_renderer->SetIndexBuffer(m_renderer->GetIndexBuffer());
		m_renderer->SetLayout(shader_);
		m_renderer->DrawSprites(vertexCount, vertexOffsetSize / sizeof(VERTEX));

		m_renderer->EndShader(shader_);

		m_renderer->GetRenderState()->Pop();

		vertexCount = 0;
		vertexOffsetSize = -1;
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_STANDARD_RENDERER_H__