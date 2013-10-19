
#ifndef	__EFFEKSEERRENDERER_RING_RENDERER_BASE_H__
#define	__EFFEKSEERRENDERER_RING_RENDERER_BASE_H__

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
typedef ::Effekseer::RingRenderer::NodeParameter efkRingNodeParam;
typedef ::Effekseer::RingRenderer::InstanceParameter efkRingInstanceParam;
typedef ::Effekseer::Vector3D efkVector3D;

class RingRendererBase
	: public ::Effekseer::RingRenderer
{
protected:
	int32_t							m_ringBufferOffset;
	uint8_t*						m_ringBufferData;

	int32_t							m_spriteCount;
	int32_t							m_instanceCount;
	::Effekseer::Matrix44			m_singleRenderingMatrix;

	RingRendererBase();
public:

	virtual ~RingRendererBase();


protected:

	template<typename VERTEX>
	void Rendering_( const efkRingNodeParam& parameter, const efkRingInstanceParam& instanceParameter, void* userData, const ::Effekseer::Matrix44& camera )
	{
		int32_t vertexCount = parameter.VertexCount * 8;
		//Vertex* verteies = (Vertex*)m_renderer->GetVertexBuffer()->GetBufferDirect( sizeof(Vertex) * vertexCount );
		
		Vertex* verteies = (Vertex*)m_ringBufferData;
		m_ringBufferData += sizeof(Vertex) * vertexCount;

		const float radian = instanceParameter.ViewingAngle / 180.0f * 3.141592f;
		const float stepAngle = radian / (parameter.VertexCount);
		const float beginAngle = -radian / 2.0f;
		
		const float outerRadius = instanceParameter.OuterLocation.X;
		const float innerRadius = instanceParameter.InnerLocation.X;
		const float centerRadius = innerRadius + (outerRadius - innerRadius) * instanceParameter.CenterRatio;

		const float outerHeight = instanceParameter.OuterLocation.Y;
		const float innerHeight = instanceParameter.InnerLocation.Y;
		const float centerHeight = innerHeight + (outerHeight - innerHeight) * instanceParameter.CenterRatio;
		
		::Effekseer::Color outerColor = instanceParameter.OuterColor;
		::Effekseer::Color innerColor = instanceParameter.InnerColor;
		::Effekseer::Color centerColor = instanceParameter.CenterColor;
		
		const float stepC = cosf(stepAngle);
		const float stepS = sinf(stepAngle);
		float c = cosf(beginAngle);
		float s = sinf(beginAngle);
		::Effekseer::Vector3D outerCurrent( c * outerRadius, s * outerRadius, outerHeight );
		::Effekseer::Vector3D innerCurrent( c * innerRadius, s * innerRadius, innerHeight );
		::Effekseer::Vector3D centerCurrent( c * centerRadius, s * centerRadius, centerHeight );
		float texCurrent = instanceParameter.UV.X;
		const float texStep = instanceParameter.UV.Width / parameter.VertexCount;
		const float v1 = instanceParameter.UV.Y;
		const float v2 = v1 + instanceParameter.UV.Height * 0.5f;
		const float v3 = v1 + instanceParameter.UV.Height;
		
		::Effekseer::Vector3D outerNext, innerNext, centerNext;
		float texNext;

		for( int i = 0; i < vertexCount; i += 8 )
		{
			float t;
			t = c * stepC - s * stepS;
			s = s * stepC + c * stepS;
			c = t;

			outerNext.X = c * outerRadius;
			outerNext.Y = s * outerRadius;
			outerNext.Z = outerHeight;
			innerNext.X = c * innerRadius;
			innerNext.Y = s * innerRadius;
			innerNext.Z = innerHeight;
			centerNext.X = c * centerRadius;
			centerNext.Y = s * centerRadius;
			centerNext.Z = centerHeight;

			texNext = texCurrent + texStep;
			
			Vertex* v = &verteies[i];
			v[0].Pos = outerCurrent;
			v[0].SetColor( outerColor );
			v[0].UV[0] = texCurrent;
			v[0].UV[1] = v1;

			v[1].Pos = centerCurrent;
			v[1].SetColor( centerColor );
			v[1].UV[0] = texCurrent;
			v[1].UV[1] = v2;

			v[2].Pos = outerNext;
			v[2].SetColor( outerColor );
			v[2].UV[0] = texNext;
			v[2].UV[1] = v1;
			
			v[3].Pos = centerNext;
			v[3].SetColor( centerColor );
			v[3].UV[0] = texNext;
			v[3].UV[1] = v2;

			v[4].Pos = centerCurrent;
			v[4].SetColor( centerColor );
			v[4].UV[0] = texCurrent;
			v[4].UV[1] = v2;

			v[5].Pos = innerCurrent;
			v[5].SetColor( innerColor );
			v[5].UV[0] = texCurrent;
			v[5].UV[1] = v3;

			v[6].Pos = centerNext;
			v[6].SetColor( centerColor );
			v[6].UV[0] = texNext;
			v[6].UV[1] = v2;

			v[7].Pos = innerNext;
			v[7].SetColor( innerColor );
			v[7].UV[0] = texNext;
			v[7].UV[1] = v3;

			outerCurrent = outerNext;
			innerCurrent = innerNext;
			centerCurrent = centerNext;
			texCurrent = texNext;
		}

		if( parameter.Billboard == ::Effekseer::BillboardType_Billboard ||
			parameter.Billboard == ::Effekseer::BillboardType_RotatedBillboard ||
			parameter.Billboard == ::Effekseer::BillboardType_YAxisFixed)
		{
			const ::Effekseer::Matrix43& mat = instanceParameter.SRTMatrix43;
			::Effekseer::Vector3D s;
			::Effekseer::Matrix43 r;
			::Effekseer::Vector3D t;

			mat.GetSRT( s, r, t );

			::Effekseer::Vector3D F;
			::Effekseer::Vector3D R;
			::Effekseer::Vector3D U;

			
			if( parameter.Billboard == ::Effekseer::BillboardType_Billboard )
			{
				::Effekseer::Vector3D Up( 0.0f, 1.0f, 0.0f );

				::Effekseer::Vector3D::Normal( F, ::Effekseer::Vector3D( - camera.Values[0][2], - camera.Values[1][2], - camera.Values[2][2] ) );

				::Effekseer::Vector3D::Normal( R, ::Effekseer::Vector3D::Cross( R, Up, F ) );
				::Effekseer::Vector3D::Normal( U, ::Effekseer::Vector3D::Cross( U, F, R ) );
			}
			else if( parameter.Billboard == ::Effekseer::BillboardType_RotatedBillboard )
			{
				::Effekseer::Vector3D Up( 0.0f, 1.0f, 0.0f );

				::Effekseer::Vector3D::Normal( F, ::Effekseer::Vector3D( - camera.Values[0][2], - camera.Values[1][2], - camera.Values[2][2] ) );

				::Effekseer::Vector3D::Normal( R, ::Effekseer::Vector3D::Cross( R, Up, F ) );
				::Effekseer::Vector3D::Normal( U, ::Effekseer::Vector3D::Cross( U, F, R ) );

				float c_zx = sqrt( 1.0f - r.Value[2][1] * r.Value[2][1] );
				float s_z = 0.0f;
				float c_z = 0.0f;

				if( fabsf( c_zx ) > 0.05f )
				{
					s_z = - r.Value[0][1] / c_zx;
					c_z = sqrt( 1.0f - s_z * s_z );
					if( r.Value[1][1] < 0.0f ) c_z = - c_z;
				}
				else
				{
					s_z = 0.0f;
					c_z = 1.0f;
				}
				
				::Effekseer::Vector3D r_temp = R;
				::Effekseer::Vector3D u_temp = U;

				R.X = r_temp.X * c_z + u_temp.X * s_z;
				R.Y = r_temp.Y * c_z + u_temp.Y * s_z;
				R.Z = r_temp.Z * c_z + u_temp.Z * s_z;

				U.X = u_temp.X * c_z - r_temp.X * s_z;
				U.Y = u_temp.Y * c_z - r_temp.Y * s_z;
				U.Z = u_temp.Z * c_z - r_temp.Z * s_z;
			}
			else if( parameter.Billboard == ::Effekseer::BillboardType_YAxisFixed )
			{
				U = ::Effekseer::Vector3D( r.Value[1][0], r.Value[1][1], r.Value[1][2] );

				::Effekseer::Vector3D::Normal( F, ::Effekseer::Vector3D( - camera.Values[0][2], - camera.Values[1][2], - camera.Values[2][2] ) );

				::Effekseer::Vector3D::Normal( R, ::Effekseer::Vector3D::Cross( R, U, F ) );
				::Effekseer::Vector3D::Normal( F, ::Effekseer::Vector3D::Cross( F, R, U ) );
			}
			
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
			
			if( m_instanceCount > 1 )
			{
				::Effekseer::Matrix43 mat_scale;
				mat_scale.Scaling( s.X, s.Y, s.Z );
				::Effekseer::Matrix43::Multiple( mat_rot, mat_scale, mat_rot );

				TransformVertexes( verteies, vertexCount, mat_rot );
			}
			else
			{
				::Effekseer::Matrix43 mat_scale;
				mat_scale.Scaling( s.X, s.Y, s.Z );
				::Effekseer::Matrix43::Multiple( mat_rot, mat_scale, mat_rot );

				for( int32_t i = 0; i < 4; i++ )
				{
					m_singleRenderingMatrix.Values[i][0] = mat_rot.Value[i][0];
					m_singleRenderingMatrix.Values[i][1] = mat_rot.Value[i][1];
					m_singleRenderingMatrix.Values[i][2] = mat_rot.Value[i][2];
				}
			}
		}
		else if( parameter.Billboard == ::Effekseer::BillboardType_Fixed )
		{
			if( m_instanceCount > 1 )
			{
				TransformVertexes( verteies, vertexCount, instanceParameter.SRTMatrix43 );
			}
			else
			{
				for( int32_t i = 0; i < 4; i++ )
				{
					m_singleRenderingMatrix.Values[i][0] = instanceParameter.SRTMatrix43.Value[i][0];
					m_singleRenderingMatrix.Values[i][1] = instanceParameter.SRTMatrix43.Value[i][1];
					m_singleRenderingMatrix.Values[i][2] = instanceParameter.SRTMatrix43.Value[i][2];
				}
			}
		}

		m_spriteCount += 2 * parameter.VertexCount;
	}

	template<typename RENDERER, typename SHADER, typename TEXTURE>
	void EndRendering_(RENDERER* renderer, SHADER* shader, SHADER* shader_no_texture, const efkRingNodeParam& param)
	{
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

		RenderStateBase::State& state = renderer->GetRenderState()->Push();
		state.DepthTest = param.ZTest;
		state.DepthWrite = param.ZWrite;
		state.CullingType = ::Effekseer::CULLING_DOUBLE;

		if (param.ColorTextureIndex >= 0)
		{
			TEXTURE texture = (TEXTURE) param.EffectPointer->GetImage(param.ColorTextureIndex);
			renderer->SetTextures(shader_, &texture, 1);
		}
		else
		{
			TEXTURE texture = 0;
			renderer->SetTextures(shader_, &texture, 1);
		}

		if (m_instanceCount > 1)
		{
			((Effekseer::Matrix44*)(shader_->GetVertexConstantBuffer()))[0] = renderer->GetCameraProjectionMatrix();
		}
		else
		{
			::Effekseer::Matrix44 mat;
			::Effekseer::Matrix44::Mul(mat, m_singleRenderingMatrix, renderer->GetCameraProjectionMatrix());
			((Effekseer::Matrix44*)(shader_->GetVertexConstantBuffer()))[0] = mat;
		}
		shader_->SetConstantBuffer();

		state.AlphaBlend = param.AlphaBlend;
		state.TextureFilterTypes[0] = param.TextureFilter;
		state.TextureWrapTypes[0] = param.TextureWrap;

		renderer->GetRenderState()->Update(false);

		renderer->SetVertexBuffer(renderer->GetVertexBuffer(), sizeof(Vertex));
		renderer->SetIndexBuffer(renderer->GetIndexBuffer());
		renderer->SetLayout(shader_);
		renderer->DrawSprites(m_spriteCount, m_ringBufferOffset / sizeof(Vertex));

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
#endif	// __EFFEKSEERRENDERER_RING_RENDERER_H__