
#ifndef	__EFFEKSEERRENDERER_RIBBON_RENDERER_BASE_H__
#define	__EFFEKSEERRENDERER_RIBBON_RENDERER_BASE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <Effekseer.h>
#include <assert.h>
#include <string.h>

#include "EffekseerRenderer.CommonUtils.h"
#include "EffekseerRenderer.RenderStateBase.h"
#include "EffekseerRenderer.VertexBufferBase.h"
#include "EffekseerRenderer.IndexBufferBase.h"
#include "EffekseerRenderer.StandardRenderer.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
typedef ::Effekseer::RibbonRenderer::NodeParameter efkRibbonNodeParam;
typedef ::Effekseer::RibbonRenderer::InstanceParameter efkRibbonInstanceParam;
typedef ::Effekseer::Vector3D efkVector3D;

template<typename RENDERER, typename VERTEX_NORMAL, typename VERTEX_DISTORTION>
class RibbonRendererBase
	: public ::Effekseer::RibbonRenderer
{
protected:
	RENDERER*						m_renderer;
	int32_t							m_ribbonCount;

	int32_t							m_ringBufferOffset;
	uint8_t*						m_ringBufferData;

public:

	RibbonRendererBase(RENDERER* renderer)
		: m_renderer(renderer)
		, m_ribbonCount(0)
		, m_ringBufferOffset(0)
		, m_ringBufferData(NULL)
	{
	}

	virtual ~RibbonRendererBase()
	{
	}


protected:

	void BeginRendering_(RENDERER* renderer, int32_t count, const efkRibbonNodeParam& param)
	{
		m_ribbonCount = 0;
		int32_t vertexCount = (count - 1) * 4;
		if (vertexCount <= 0) return;

		EffekseerRenderer::StandardRendererState state;
		state.AlphaBlend = param.AlphaBlend;
		state.CullingType = ::Effekseer::CullingType::Double;
		state.DepthTest = param.ZTest;
		state.DepthWrite = param.ZWrite;
		state.TextureFilterType = param.TextureFilter;
		state.TextureWrapType = param.TextureWrap;

		state.Distortion = param.Distortion;
		state.DistortionIntensity = param.DistortionIntensity;

		if (param.ColorTextureIndex >= 0)
		{
			if (state.Distortion)
			{
				state.TexturePtr = param.EffectPointer->GetDistortionImage(param.ColorTextureIndex);
			}
			else
			{
				state.TexturePtr = param.EffectPointer->GetColorImage(param.ColorTextureIndex);
			}
		}
		else
		{
			state.TexturePtr = nullptr;
		}

		renderer->GetStandardRenderer()->UpdateStateAndRenderingIfRequired(state);

		renderer->GetStandardRenderer()->BeginRenderingAndRenderingIfRequired(vertexCount, m_ringBufferOffset, (void*&) m_ringBufferData);
	}

	void Rendering_(const efkRibbonNodeParam& parameter, const efkRibbonInstanceParam& instanceParameter, void* userData, const ::Effekseer::Matrix44& camera)
	{
		if (parameter.Distortion)
		{
			Rendering_Internal<VERTEX_DISTORTION>(parameter, instanceParameter, userData, camera);
		}
		else
		{
			Rendering_Internal<VERTEX_NORMAL>(parameter, instanceParameter, userData, camera);
		}
	}

	template<typename VERTEX>
	void Rendering_Internal( const efkRibbonNodeParam& parameter, const efkRibbonInstanceParam& instanceParameter, void* userData, const ::Effekseer::Matrix44& camera )
	{
		if( m_ringBufferData == NULL ) return;
		if( instanceParameter.InstanceCount < 2 ) return;
	
		bool isFirst = instanceParameter.InstanceIndex == 0;
		bool isLast = instanceParameter.InstanceIndex == (instanceParameter.InstanceCount - 1);
	
		VERTEX* verteies = (VERTEX*)m_ringBufferData;
	
		for( int i = 0; i < 2; i++ )
		{
			verteies[i].Pos.X = instanceParameter.Positions[i];
			verteies[i].Pos.Y = 0.0f;
			verteies[i].Pos.Z = 0.0f;
			verteies[i].SetColor( instanceParameter.Colors[i] );
		}

		float percent = (float) instanceParameter.InstanceIndex / (float) (instanceParameter.InstanceCount - 1);
		
		verteies[0].UV[0] = instanceParameter.UV.X;
		verteies[0].UV[1] = instanceParameter.UV.Y + percent * instanceParameter.UV.Height;
	
		verteies[1].UV[0] = instanceParameter.UV.X + instanceParameter.UV.Width;
		verteies[1].UV[1] = instanceParameter.UV.Y + percent * instanceParameter.UV.Height;
		
		if( parameter.ViewpointDependent)
		{
			const ::Effekseer::Matrix43& mat = instanceParameter.SRTMatrix43;
			::Effekseer::Vector3D s;
			::Effekseer::Matrix43 r;
			::Effekseer::Vector3D t;
			mat.GetSRT( s, r, t );
	
			// 拡大
			for( int i = 0; i < 2; i++ )
			{
				verteies[i].Pos.X = verteies[i].Pos.X * s.X;
			}
	
			::Effekseer::Vector3D F;
			::Effekseer::Vector3D R;
			::Effekseer::Vector3D U;
	
			U = ::Effekseer::Vector3D( r.Value[1][0], r.Value[1][1], r.Value[1][2] );
	
			::Effekseer::Vector3D::Normal( F, ::Effekseer::Vector3D( - camera.Values[0][2], - camera.Values[1][2], - camera.Values[2][2] ) );
	
			::Effekseer::Vector3D::Normal( R, ::Effekseer::Vector3D::Cross( R, U, F ) );
			::Effekseer::Vector3D::Normal( F, ::Effekseer::Vector3D::Cross( F, R, U ) );
			
			::Effekseer::Matrix43 mat_rot;
	
			mat_rot.Value[0][0] = - R.X;
			mat_rot.Value[0][1] = - R.Y;
			mat_rot.Value[0][2] = - R.Z;
			mat_rot.Value[1][0] = U.X;
			mat_rot.Value[1][1] = U.Y;
			mat_rot.Value[1][2] = U.Z;
			mat_rot.Value[2][0] = F.X;
			mat_rot.Value[2][1] = F.Y;
			mat_rot.Value[2][2] = F.Z;
			mat_rot.Value[3][0] = t.X;
			mat_rot.Value[3][1] = t.Y;
			mat_rot.Value[3][2] = t.Z;
	
			for( int i = 0; i < 2; i++ )
			{
				::Effekseer::Vector3D::Transform(
					verteies[i].Pos,
					verteies[i].Pos,
					mat_rot );
			}
		}
		else
		{
			for( int i = 0; i < 2; i++ )
			{
				::Effekseer::Vector3D::Transform(
					verteies[i].Pos,
					verteies[i].Pos,
					instanceParameter.SRTMatrix43 );
			}
		}
	
		if( isFirst || isLast )
		{
			m_ringBufferData += sizeof(VERTEX) * 2;
		}
		else
		{
			verteies[2] = verteies[0];
			verteies[3] = verteies[1];
			m_ringBufferData += sizeof(VERTEX) * 4;
		}
	
		if( !isFirst )
		{
			m_ribbonCount++;
		}

		/* 歪みを適用 */
		if (isLast && sizeof(VERTEX) == sizeof(VERTEX_DISTORTION))
		{
			VERTEX_DISTORTION* vs_ = (VERTEX_DISTORTION*) (m_ringBufferData - sizeof(VERTEX_DISTORTION) * (instanceParameter.InstanceCount - 1) * 4);

			Effekseer::Vector3D axisBefore;

			for (int32_t i = 0; i < instanceParameter.InstanceCount; i++)
			{
				bool isFirst_ = (i == 0);
				bool isLast_ = (i == (instanceParameter.InstanceCount - 1));
		
				Effekseer::Vector3D axis;
				Effekseer::Vector3D pos;

				if (isFirst_)
				{
					axis = (vs_[3].Pos - vs_[1].Pos);
					Effekseer::Vector3D::Normal(axis, axis);
					axisBefore = axis;
				}
				else if (isLast_)
				{
					axis = axisBefore;
				}
				else
				{
					Effekseer::Vector3D axisOld = axisBefore;
					axis = (vs_[5].Pos - vs_[3].Pos);
					Effekseer::Vector3D::Normal(axis, axis);
					axisBefore = axis;

					axis = (axisBefore + axisOld) / 2.0f;
					Effekseer::Vector3D::Normal(axis, axis);
				}

				auto tangent = vs_[1].Pos - vs_[0].Pos;
				Effekseer::Vector3D::Normal(tangent, tangent);

				if (isFirst_)
				{
					vs_[0].Binormal = axis;
					vs_[1].Binormal = axis;

					vs_[0].Tangent = tangent;
					vs_[1].Tangent = tangent;

					vs_ += 2;

				}
				else if (isLast_)
				{
					vs_[0].Binormal = axis;
					vs_[1].Binormal = axis;

					vs_[0].Tangent = tangent;
					vs_[1].Tangent = tangent;

					vs_ += 2;
				}
				else
				{
					vs_[0].Binormal = axis;
					vs_[1].Binormal = axis;
					vs_[2].Binormal = axis;
					vs_[3].Binormal = axis;

					vs_[0].Tangent = tangent;
					vs_[1].Tangent = tangent;
					vs_[2].Tangent = tangent;
					vs_[3].Tangent = tangent;

					vs_ += 4;
				}
			}
		}
	}

	void EndRendering_(RENDERER* renderer, const efkRibbonNodeParam& param)
	{
		/*
		RenderStateBase::State& state = renderer->GetRenderState()->Push();
		state.DepthTest = param.ZTest;
		state.DepthWrite = param.ZWrite;
		state.CullingType = ::Effekseer::CullingType::Double;

		SHADER* shader_ = NULL;
		if (param.ColorTextureIndex >= 0)
		{
			shader_ = shader;
		}
		else
		{
			shader_ = shader_no_texture;
		}

		renderer->BeginShader(shader_);

		if (param.ColorTextureIndex >= 0)
		{
			TEXTURE texture = TexturePointerToTexture<TEXTURE>(param.EffectPointer->GetColorImage(param.ColorTextureIndex));
			renderer->SetTextures(shader_, &texture, 1);
		}
		else
		{
			TEXTURE texture = (TEXTURE)NULL;
			renderer->SetTextures(shader_, &texture, 1);
		}

		((Effekseer::Matrix44*)(shader_->GetVertexConstantBuffer()))[0] = renderer->GetCameraProjectionMatrix();
		shader_->SetConstantBuffer();


		state.AlphaBlend = param.AlphaBlend;
		state.TextureFilterTypes[0] = param.TextureFilter;
		state.TextureWrapTypes[0] = param.TextureWrap;

		renderer->GetRenderState()->Update(false);

		renderer->SetVertexBuffer(renderer->GetVertexBuffer(), sizeof(VERTEX));
		renderer->SetIndexBuffer(renderer->GetIndexBuffer());
		renderer->SetLayout(shader_);
		renderer->DrawSprites(m_ribbonCount, m_ringBufferOffset / sizeof(VERTEX));

		renderer->EndShader(shader_);

		renderer->GetRenderState()->Pop();
		*/
	}

public:

	void BeginRendering(const efkRibbonNodeParam& parameter, int32_t count, void* userData) override
	{
		BeginRendering_(m_renderer, count, parameter);
	}

	void Rendering(const efkRibbonNodeParam& parameter, const efkRibbonInstanceParam& instanceParameter, void* userData) override
	{
		Rendering_(parameter, instanceParameter, userData, m_renderer->GetCameraMatrix());
	}

	void EndRendering(const efkRibbonNodeParam& parameter, void* userData) override
	{
		if (m_ringBufferData == NULL) return;

		if (m_ribbonCount <= 1) return;

		EndRendering_(m_renderer, parameter);
	}

};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_RIBBON_RENDERER_H__
