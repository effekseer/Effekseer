
#ifndef	__EFFEKSEERRENDERER_TRACK_RENDERER_BASE_H__
#define	__EFFEKSEERRENDERER_TRACK_RENDERER_BASE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <Effekseer.h>
#include <assert.h>
#include <string.h>

#include "EffekseerRenderer.VertexBufferBase.h"
#include "EffekseerRenderer.IndexBufferBase.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
typedef ::Effekseer::TrackRenderer::NodeParameter efkTrackNodeParam;
typedef ::Effekseer::TrackRenderer::InstanceParameter efkTrackInstanceParam;
typedef ::Effekseer::Vector3D efkVector3D;

class TrackRendererBase
	: public ::Effekseer::TrackRenderer
{
protected:
	int32_t							m_ribbonCount;

	int32_t							m_ringBufferOffset;
	uint8_t*						m_ringBufferData;

	TrackRendererBase();
public:

	virtual ~TrackRendererBase();

protected:

	template<typename VERTEX, typename RENDERER>
	void BeginRendering_( RENDERER* renderer, const efkTrackNodeParam& parameter, int32_t count, void* userData )
	{
		m_ribbonCount = 0;
		
		int32_t vertexCount = (count - 1) * 8;
	
		if( ! renderer->GetVertexBuffer()->RingBufferLock( sizeof(VERTEX) * vertexCount, m_ringBufferOffset, (void*&)m_ringBufferData ) )
		{
			m_ringBufferOffset = 0;
			m_ringBufferData = NULL;
		}
	}

	template<typename VERTEX>
	void Rendering_( const efkTrackNodeParam& parameter, const efkTrackInstanceParam& instanceParameter, void* userData, const ::Effekseer::Matrix44& camera )
	{
		if( m_ringBufferData == NULL ) return;
		if( instanceParameter.InstanceCount < 2 ) return;

		const efkTrackInstanceParam& param = instanceParameter;
		
		bool isFirst = param.InstanceIndex == 0;
		bool isLast = param.InstanceIndex == (param.InstanceCount - 1);

		Vertex* verteies = (Vertex*)m_ringBufferData;

		float size = 0.0f;
		::Effekseer::Color leftColor;
		::Effekseer::Color centerColor;
		::Effekseer::Color rightColor;

		if( param.InstanceIndex < param.InstanceCount / 2 )
		{
			float l = (float)param.InstanceIndex / (float)param.InstanceCount;
			l = l * 2.0f;
			size = param.SizeFor + (param.SizeMiddle-param.SizeFor) * l;
			
			leftColor.R = (uint8_t)Effekseer::Clamp( param.ColorLeft.R + (param.ColorLeftMiddle.R-param.ColorLeft.R) * l, 255, 0 );
			leftColor.G = (uint8_t)Effekseer::Clamp( param.ColorLeft.G + (param.ColorLeftMiddle.G-param.ColorLeft.G) * l, 255, 0 );
			leftColor.B = (uint8_t)Effekseer::Clamp( param.ColorLeft.B + (param.ColorLeftMiddle.B-param.ColorLeft.B) * l, 255, 0 );
			leftColor.A = (uint8_t)Effekseer::Clamp( param.ColorLeft.A + (param.ColorLeftMiddle.A-param.ColorLeft.A) * l, 255, 0 );

			centerColor.R = (uint8_t)Effekseer::Clamp( param.ColorCenter.R + (param.ColorCenterMiddle.R-param.ColorCenter.R) * l, 255, 0 );
			centerColor.G = (uint8_t)Effekseer::Clamp( param.ColorCenter.G + (param.ColorCenterMiddle.G-param.ColorCenter.G) * l, 255, 0 );
			centerColor.B = (uint8_t)Effekseer::Clamp( param.ColorCenter.B + (param.ColorCenterMiddle.B-param.ColorCenter.B) * l, 255, 0 );
			centerColor.A = (uint8_t)Effekseer::Clamp( param.ColorCenter.A + (param.ColorCenterMiddle.A-param.ColorCenter.A) * l, 255, 0 );

			rightColor.R = (uint8_t)Effekseer::Clamp( param.ColorRight.R + (param.ColorRightMiddle.R-param.ColorRight.R) * l, 255, 0 );
			rightColor.G = (uint8_t)Effekseer::Clamp( param.ColorRight.G + (param.ColorRightMiddle.G-param.ColorRight.G) * l, 255, 0 );
			rightColor.B = (uint8_t)Effekseer::Clamp( param.ColorRight.B + (param.ColorRightMiddle.B-param.ColorRight.B) * l, 255, 0 );
			rightColor.A = (uint8_t)Effekseer::Clamp( param.ColorRight.A + (param.ColorRightMiddle.A-param.ColorRight.A) * l, 255, 0 );
		}
		else
		{
			float l = (float)param.InstanceIndex / (float)param.InstanceCount;
			l = 1.0f - (l * 2.0f - 1.0f);
			size = param.SizeBack + (param.SizeMiddle-param.SizeBack) * l;
			
			leftColor.R = (uint8_t)Effekseer::Clamp( param.ColorLeft.R + (param.ColorLeftMiddle.R-param.ColorLeft.R) * l, 255, 0 );
			leftColor.G = (uint8_t)Effekseer::Clamp( param.ColorLeft.G + (param.ColorLeftMiddle.G-param.ColorLeft.G) * l, 255, 0 );
			leftColor.B = (uint8_t)Effekseer::Clamp( param.ColorLeft.B + (param.ColorLeftMiddle.B-param.ColorLeft.B) * l, 255, 0 );
			leftColor.A = (uint8_t)Effekseer::Clamp( param.ColorLeft.A + (param.ColorLeftMiddle.A-param.ColorLeft.A) * l, 255, 0 );

			centerColor.R = (uint8_t)Effekseer::Clamp( param.ColorCenter.R + (param.ColorCenterMiddle.R-param.ColorCenter.R) * l, 255, 0 );
			centerColor.G = (uint8_t)Effekseer::Clamp( param.ColorCenter.G + (param.ColorCenterMiddle.G-param.ColorCenter.G) * l, 255, 0 );
			centerColor.B = (uint8_t)Effekseer::Clamp( param.ColorCenter.B + (param.ColorCenterMiddle.B-param.ColorCenter.B) * l, 255, 0 );
			centerColor.A = (uint8_t)Effekseer::Clamp( param.ColorCenter.A + (param.ColorCenterMiddle.A-param.ColorCenter.A) * l, 255, 0 );

			rightColor.R = (uint8_t)Effekseer::Clamp( param.ColorRight.R + (param.ColorRightMiddle.R-param.ColorRight.R) * l, 255, 0 );
			rightColor.G = (uint8_t)Effekseer::Clamp( param.ColorRight.G + (param.ColorRightMiddle.G-param.ColorRight.G) * l, 255, 0 );
			rightColor.B = (uint8_t)Effekseer::Clamp( param.ColorRight.B + (param.ColorRightMiddle.B-param.ColorRight.B) * l, 255, 0 );
			rightColor.A = (uint8_t)Effekseer::Clamp( param.ColorRight.A + (param.ColorRightMiddle.A-param.ColorRight.A) * l, 255, 0 );
		}

		Vertex v[3];

		v[0].Pos.X = - size / 2.0f;
		v[0].Pos.Y = 0.0f;
		v[0].Pos.Z = 0.0f;
		v[0].SetColor( leftColor );

		v[1].Pos.X = 0.0f;
		v[1].Pos.Y = 0.0f;
		v[1].Pos.Z = 0.0f;
		v[1].SetColor( centerColor );

		v[2].Pos.X = size / 2.0f;
		v[2].Pos.Y = 0.0f;
		v[2].Pos.Z = 0.0f;
		v[2].SetColor( rightColor );

		v[0].UV[0] = 0.0f;
		v[0].UV[1] = (float)param.InstanceIndex / (float)param.InstanceCount;

		v[1].UV[0] = 0.5f;
		v[1].UV[1] = (float)( param.InstanceIndex ) / (float)param.InstanceCount;

		v[2].UV[0] = 1.0f;
		v[2].UV[1] = (float)( param.InstanceIndex ) / (float)param.InstanceCount;

		v[1].Pos.X = param.SRTMatrix43.Value[3][0];
		v[1].Pos.Y = param.SRTMatrix43.Value[3][1];
		v[1].Pos.Z = param.SRTMatrix43.Value[3][2];

		if( isFirst )
		{
			verteies[0] = v[0];
			verteies[1] = v[1];
			verteies[4] = v[1];
			verteies[5] = v[2];
			m_ringBufferData += sizeof(Vertex) * 2;
			
		}
		else if( isLast )
		{
			verteies[0] = v[0];
			verteies[1] = v[1];
			verteies[4] = v[1];
			verteies[5] = v[2];
			m_ringBufferData += sizeof(Vertex) * 6;
			m_ribbonCount += 2;
		}
		else
		{
			verteies[0] = v[0];
			verteies[1] = v[1];
			verteies[4] = v[1];
			verteies[5] = v[2];

			verteies[6] = v[0];
			verteies[7] = v[1];
			verteies[10] = v[1];
			verteies[11] = v[2];

			m_ringBufferData += sizeof(Vertex) * 8;
			m_ribbonCount += 2;
		}

		/* À•W•ÏŠ· */
		if( isLast )
		{
			Vertex* vs_ = (Vertex*)(m_ringBufferData - sizeof(Vertex) * 8 * (param.InstanceCount-1) );
			
			Effekseer::Vector3D axisBefore;

			for( int32_t i = 0; i < param.InstanceCount; i++ )
			{
				bool isFirst_ = (i == 0);
				bool isLast_ = (i == (param.InstanceCount-1));
				Effekseer::Vector3D axis;
				Effekseer::Vector3D pos;

				if( isFirst_ )
				{
					axis = (vs_[3].Pos - vs_[1].Pos);
					Effekseer::Vector3D::Normal( axis, axis );
					axisBefore = axis;
				}
				else if( isLast_ )
				{
					 axis = axisBefore;
				}
				else
				{
					Effekseer::Vector3D axisOld = axisBefore;
					axis = vs_[9].Pos - vs_[7].Pos;
					Effekseer::Vector3D::Normal( axis, axis );
					axisBefore = axis;

					axis = (axisBefore + axisOld) / 2.0f;
				}

				pos = vs_[1].Pos;

				Vertex vl = vs_[0];
				Vertex vm = vs_[1];
				Vertex vr = vs_[5];

				vm.Pos.X = 0.0f;
				vm.Pos.Y = 0.0f;
				vm.Pos.Z = 0.0f;

				::Effekseer::Vector3D F;
				::Effekseer::Vector3D R;
				::Effekseer::Vector3D U;

				U = axis;

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
				mat_rot.Value[3][0] = pos.X;
				mat_rot.Value[3][1] = pos.Y;
				mat_rot.Value[3][2] = pos.Z;

				::Effekseer::Vector3D::Transform(
					vl.Pos,
					vl.Pos,
					mat_rot );

				::Effekseer::Vector3D::Transform(
					vm.Pos,
					vm.Pos,
					mat_rot );

				::Effekseer::Vector3D::Transform(
					vr.Pos,
					vr.Pos,
					mat_rot );


				if( isFirst_ )
				{
					vs_[0] = vl;
					vs_[1] = vm;
					vs_[4] = vm;
					vs_[5] = vr;
					vs_ += 2;
					
				}
				else if( isLast_ )
				{
					vs_[0] = vl;
					vs_[1] = vm;
					vs_[4] = vm;
					vs_[5] = vr;
					vs_ += 6;
				}
				else
				{
					vs_[0] = vl;
					vs_[1] = vm;
					vs_[4] = vm;
					vs_[5] = vr;
					
					vs_[6] = vl;
					vs_[7] = vm;
					vs_[10] = vm;
					vs_[11] = vr;

					vs_ += 8;
				}
			}
		}
	}

	template<typename RENDERER, typename SHADER, typename TEXTURE>
	void EndRendering_(RENDERER* renderer, SHADER* shader, SHADER* shader_no_texture, const efkTrackNodeParam& param)
	{
		RenderStateBase::State& state = renderer->GetRenderState()->Push();
		state.DepthTest = param.ZTest;
		state.DepthWrite = param.ZWrite;
		state.CullingType = ::Effekseer::CULLING_DOUBLE;

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
			TEXTURE texture = (TEXTURE) param.EffectPointer->GetImage(param.ColorTextureIndex);
			renderer->SetTextures(shader_, &texture, 1);
		}
		else
		{
			TEXTURE texture = NULL;
			renderer->SetTextures(shader_, &texture, 1);
		}

		((Effekseer::Matrix44*)(shader_->GetVertexConstantBuffer()))[0] = renderer->GetCameraProjectionMatrix();
		shader_->SetConstantBuffer();


		state.AlphaBlend = param.AlphaBlend;
		state.TextureFilterTypes[0] = param.TextureFilter;
		state.TextureWrapTypes[0] = param.TextureWrap;

		renderer->GetRenderState()->Update(false);

		renderer->SetVertexBuffer(renderer->GetVertexBuffer(), sizeof(Vertex));
		renderer->SetIndexBuffer(renderer->GetIndexBuffer());
		renderer->SetLayout(shader_);
		renderer->DrawSprites(m_ribbonCount, m_ringBufferOffset / sizeof(Vertex));

		renderer->EndShader(shader_);

		renderer->GetRenderState()->Pop();
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