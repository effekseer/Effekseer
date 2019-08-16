﻿
#ifndef	__EFFEKSEERRENDERER_SPRITE_RENDERER_BASE_H__
#define	__EFFEKSEERRENDERER_SPRITE_RENDERER_BASE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <Effekseer.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <algorithm>

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
typedef ::Effekseer::SpriteRenderer::NodeParameter efkSpriteNodeParam;
typedef ::Effekseer::SpriteRenderer::InstanceParameter efkSpriteInstanceParam;
typedef ::Effekseer::Vector3D efkVector3D;

template<typename RENDERER, typename VERTEX_NORMAL, typename VERTEX_DISTORTION>
class SpriteRendererBase
	: public ::Effekseer::SpriteRenderer
{
protected:
	RENDERER*						m_renderer;
	int32_t							m_spriteCount;
	int32_t							m_ringBufferOffset;
	uint8_t*						m_ringBufferData;

	struct KeyValue
	{
		float Key;
		efkSpriteInstanceParam	Value;
	};

	std::vector<KeyValue>				instances;

public:

	SpriteRendererBase(RENDERER* renderer)
		: m_renderer(renderer)
		, m_spriteCount(0)
		, m_ringBufferOffset(0)
		, m_ringBufferData(nullptr)
	{
		// reserve buffers
		instances.reserve(m_renderer->GetSquareMaxCount());
	}

	virtual ~SpriteRendererBase()
	{
	}

protected:

	void BeginRendering_(RENDERER* renderer, int32_t count, const efkSpriteNodeParam& param)
	{
		EffekseerRenderer::StandardRendererState state;
		state.AlphaBlend = param.AlphaBlend;
		state.CullingType = ::Effekseer::CullingType::Double;
		state.DepthTest = param.ZTest;
		state.DepthWrite = param.ZWrite;
		state.TextureFilterType = param.TextureFilter;
		state.TextureWrapType = param.TextureWrap;

		state.Distortion = param.Distortion;
		state.DistortionIntensity = param.DistortionIntensity;

		state.CopyMaterialFromParameterToState(param.EffectPointer, param.MaterialParameterPtr, param.ColorTextureIndex);
		/*
		if (param.MaterialParameterPtr != nullptr)
		{
			if (param.MaterialParameterPtr->MaterialIndex >= 0)
			{
				state.MaterialPtr = param.EffectPointer->GetMaterial(param.MaterialParameterPtr->MaterialIndex);

				state.MaterialUniformCount =
					Effekseer::Min(param.MaterialParameterPtr->MaterialUniforms.size(), state.MaterialUniforms.size());
				for (size_t i = 0; i < state.MaterialUniformCount; i++)
				{
					state.MaterialUniforms[i] = param.MaterialParameterPtr->MaterialUniforms[i];
				}

				state.MaterialTextureCount =
					Effekseer::Min(param.MaterialParameterPtr->MaterialTextures.size(), state.MaterialTextures.size());
				for (size_t i = 0; i < state.MaterialTextureCount; i++)
				{
					if (param.MaterialParameterPtr->MaterialTextures[i].Type == 1)
					{
						if (param.MaterialParameterPtr->MaterialTextures[i].Index >= 0)
						{
							state.MaterialTextures[i] =
								param.EffectPointer->GetNormalImage(param.MaterialParameterPtr->MaterialTextures[i].Index);
						}
						else
						{
							state.MaterialTextures[i] = nullptr;
						}

					}
					else
					{
						if (param.MaterialParameterPtr->MaterialTextures[i].Index >= 0)
						{
							state.MaterialTextures[i] =
								param.EffectPointer->GetColorImage(param.MaterialParameterPtr->MaterialTextures[i].Index);
						}
						else
						{
							state.MaterialTextures[i] = nullptr;
						}
					}
				}
			}
		}
		else
		{
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
		}
		*/
		renderer->GetStandardRenderer()->UpdateStateAndRenderingIfRequired(state);

		renderer->GetStandardRenderer()->BeginRenderingAndRenderingIfRequired(count * 4, m_ringBufferOffset, (void*&) m_ringBufferData);
		m_spriteCount = 0;

		instances.clear();
	}

	void Rendering_(const efkSpriteNodeParam& parameter, const efkSpriteInstanceParam& instanceParameter, void* userData, const ::Effekseer::Matrix44& camera)
	{
		if (parameter.ZSort == Effekseer::ZSortType::None)
		{
			auto camera = m_renderer->GetCameraMatrix();
			const auto& state = m_renderer->GetStandardRenderer()->GetState();

			if (state.MaterialPtr != nullptr && !state.MaterialPtr->IsSimpleVertex)
			{
				Rendering_Internal<DynamicVertex>(parameter, instanceParameter, userData, camera);
			}
			else if (parameter.Distortion)
			{
				Rendering_Internal<VERTEX_DISTORTION>(parameter, instanceParameter, userData, camera);
			}
			else
			{
				Rendering_Internal<VERTEX_NORMAL>(parameter, instanceParameter, userData, camera);
			}
		}
		else
		{
			KeyValue kv;
			kv.Value = instanceParameter;
			instances.push_back(kv);
		}
	}

	enum class VertexType
	{
		Normal,
		Distortion,
		Dynamic,
	};

	template <typename V> VertexType GetVertexType(const V* v) { return VertexType::Normal; }

	template <> VertexType GetVertexType(const VERTEX_NORMAL* v) { return VertexType::Normal; }

	template <> VertexType GetVertexType(const VERTEX_DISTORTION* v) { return VertexType::Distortion; }

	template <> VertexType GetVertexType(const DynamicVertex* v) { return VertexType::Dynamic; }

	template<typename VERTEX>
	void Rendering_Internal( const efkSpriteNodeParam& parameter, const efkSpriteInstanceParam& instanceParameter, void* userData, const ::Effekseer::Matrix44& camera )
	{
		if( m_ringBufferData == nullptr ) return;

		VERTEX* verteies = (VERTEX*)m_ringBufferData;
		m_ringBufferData += (sizeof(VERTEX) * 4);

		auto vertexType = GetVertexType(verteies);

		for( int i = 0; i < 4; i++ )
		{
			verteies[i].Pos.X = instanceParameter.Positions[i].X;
			verteies[i].Pos.Y = instanceParameter.Positions[i].Y;
			verteies[i].Pos.Z = 0.0f;
	
			verteies[i].SetColor( instanceParameter.Colors[i] );
		}
		
		verteies[0].UV[0] = instanceParameter.UV.X;
		verteies[0].UV[1] = instanceParameter.UV.Y + instanceParameter.UV.Height;
	
		verteies[1].UV[0] = instanceParameter.UV.X + instanceParameter.UV.Width;
		verteies[1].UV[1] = instanceParameter.UV.Y + instanceParameter.UV.Height;
		
		verteies[2].UV[0] = instanceParameter.UV.X;
		verteies[2].UV[1] = instanceParameter.UV.Y;
	
		verteies[3].UV[0] = instanceParameter.UV.X + instanceParameter.UV.Width;
		verteies[3].UV[1] = instanceParameter.UV.Y;

		// distortion
		if (vertexType == VertexType::Distortion)
		{
			auto vs = (VERTEX_DISTORTION*) verteies;
			for (auto i = 0; i < 4; i++)
			{
				vs[i].Tangent.X = 1.0f;
				vs[i].Tangent.Y = 0.0f;
				vs[i].Tangent.Z = 0.0f;
				vs[i].Binormal.X = 1.0f;
				vs[i].Binormal.Y = 0.0f;
				vs[i].Binormal.Z = 0.0f;
			}
		}
		else if (vertexType == VertexType::Dynamic)
		{
			auto vs = (DynamicVertex*)verteies;
			for (auto i = 0; i < 4; i++)
			{
				vs[i].Normal.R = static_cast<uint8_t>(0.0f * 255);
				vs[i].Normal.G = static_cast<uint8_t>(0.0f * 255);
				vs[i].Normal.B = static_cast<uint8_t>(1.0f * 255);

				vs[i].Tangent.R = static_cast<uint8_t>(1.0f * 255);
				vs[i].Tangent.G = static_cast<uint8_t>(0.0f * 255);
				vs[i].Tangent.B = static_cast<uint8_t>(0.0f * 255);
			}

			vs[0].UV2[0] = 0.0f;
			vs[0].UV2[1] = 1.0f;
			vs[1].UV2[0] = 1.0f;
			vs[1].UV2[1] = 1.0f;
			vs[2].UV2[0] = 0.0f;
			vs[2].UV2[1] = 0.0f;
			vs[3].UV2[0] = 1.0f;
			vs[3].UV2[1] = 0.0f;
		}
		
		if( parameter.Billboard == ::Effekseer::BillboardType::Billboard ||
			parameter.Billboard == ::Effekseer::BillboardType::RotatedBillboard ||
			parameter.Billboard == ::Effekseer::BillboardType::YAxisFixed)
		{
			const ::Effekseer::Matrix43& mat = instanceParameter.SRTMatrix43;
			::Effekseer::Vector3D s;
			::Effekseer::Matrix43 r;
			::Effekseer::Vector3D t;
			mat.GetSRT( s, r, t );
	
			// 拡大
			for( int i = 0; i < 4; i++ )
			{
				verteies[i].Pos.X = verteies[i].Pos.X * s.X;
				verteies[i].Pos.Y = verteies[i].Pos.Y * s.Y;
			}
	
			::Effekseer::Vector3D F;
			::Effekseer::Vector3D R;
			::Effekseer::Vector3D U;
			
			
			if( parameter.Billboard == ::Effekseer::BillboardType::Billboard )
			{
				::Effekseer::Vector3D Up( 0.0f, 1.0f, 0.0f );
	
				::Effekseer::Vector3D::Normal( F, -m_renderer->GetCameraFrontDirection());
	
				::Effekseer::Vector3D::Normal( R, ::Effekseer::Vector3D::Cross( R, Up, F ) );
				::Effekseer::Vector3D::Normal( U, ::Effekseer::Vector3D::Cross( U, F, R ) );
			}
			else if( parameter.Billboard == ::Effekseer::BillboardType::RotatedBillboard )
			{
				::Effekseer::Vector3D Up( 0.0f, 1.0f, 0.0f );
	
				::Effekseer::Vector3D::Normal( F, -m_renderer->GetCameraFrontDirection());
	
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
			else if( parameter.Billboard == ::Effekseer::BillboardType::YAxisFixed )
			{
				U = ::Effekseer::Vector3D( r.Value[1][0], r.Value[1][1], r.Value[1][2] );
	
				::Effekseer::Vector3D::Normal( F, -m_renderer->GetCameraFrontDirection());
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
	
			ApplyDepthParameters(mat_rot,
								 m_renderer->GetCameraFrontDirection(),
								 m_renderer->GetCameraPosition(),
								 s,
								 parameter.DepthOffset,
								 parameter.IsDepthOffsetScaledWithCamera,
								 parameter.IsDepthOffsetScaledWithParticleScale,
								 parameter.DepthParameterPtr,
								 parameter.IsRightHand);

			TransformVertexes( verteies, 4, mat_rot );
		}
		else if( parameter.Billboard == ::Effekseer::BillboardType::Fixed )
		{
			auto mat = instanceParameter.SRTMatrix43;

			ApplyDepthParameters(mat,
							 m_renderer->GetCameraFrontDirection(),
							 m_renderer->GetCameraPosition(),
							 parameter.DepthOffset,
							 parameter.IsDepthOffsetScaledWithCamera,
							 parameter.IsDepthOffsetScaledWithParticleScale,
							 parameter.DepthParameterPtr,
							 parameter.IsRightHand);

			for( int i = 0; i < 4; i++ )
			{
				::Effekseer::Vector3D::Transform(
					verteies[i].Pos,
					verteies[i].Pos,
					mat);

				// distortion
				if (vertexType == VertexType::Distortion)
				{
					auto vs = (VERTEX_DISTORTION*) & verteies[i];

					::Effekseer::Vector3D::Transform(
						vs->Tangent,
						vs->Tangent,
						mat);

					::Effekseer::Vector3D::Transform(
						vs->Binormal,
						vs->Binormal,
						mat);

					Effekseer::Vector3D zero;
					::Effekseer::Vector3D::Transform(
						zero,
						zero,
						mat);

					::Effekseer::Vector3D::Normal(vs->Tangent, vs->Tangent - zero);
					::Effekseer::Vector3D::Normal(vs->Binormal, vs->Binormal - zero);
				}
				else if (vertexType == VertexType::Dynamic)
				{
					auto vs = (DynamicVertex*)&verteies[i];
					auto tangentX = efkVector3D(mat.Value[0][0], mat.Value[0][1], mat.Value[0][2]);
					auto tangentZ = efkVector3D(mat.Value[2][0], mat.Value[2][1], mat.Value[2][2]);
					vs->Normal.R = static_cast<uint8_t>(tangentZ.X * 255.0f);
					vs->Normal.G = static_cast<uint8_t>(tangentZ.Y * 255.0f);
					vs->Normal.B = static_cast<uint8_t>(tangentZ.Z * 255.0f);
					vs->Tangent.R = static_cast<uint8_t>(tangentX.X * 255.0f);
					vs->Tangent.G = static_cast<uint8_t>(tangentX.Y * 255.0f);
					vs->Tangent.B = static_cast<uint8_t>(tangentX.Z * 255.0f);
				}
			}
		}
		
		m_spriteCount++;
	}

	void EndRendering_(RENDERER* renderer, const efkSpriteNodeParam& param)
	{
		if (param.ZSort != Effekseer::ZSortType::None)
		{
			for (auto& kv : instances)
			{
				efkVector3D t;
				t.X = kv.Value.SRTMatrix43.Value[3][0];
				t.Y = kv.Value.SRTMatrix43.Value[3][1];
				t.Z = kv.Value.SRTMatrix43.Value[3][2];

				auto frontDirection = m_renderer->GetCameraFrontDirection();
				if (!param.IsRightHand)
				{
					frontDirection.Z = -frontDirection.Z;
				}

				kv.Key = Effekseer::Vector3D::Dot(t, frontDirection);
			}

			if (param.ZSort == Effekseer::ZSortType::NormalOrder)
			{
				std::sort(instances.begin(), instances.end(), [](const KeyValue& a, const KeyValue& b) -> bool { return a.Key < b.Key; });
			}
			else
			{
				std::sort(instances.begin(), instances.end(), [](const KeyValue& a, const KeyValue& b) -> bool { return a.Key > b.Key; });
			}
			

			for (auto& kv : instances)
			{
				auto camera = m_renderer->GetCameraMatrix();
				const auto& state = renderer->GetStandardRenderer()->GetState();

				if (state.MaterialPtr != nullptr && !state.MaterialPtr->IsSimpleVertex)
				{
					Rendering_Internal<DynamicVertex>(param, kv.Value, nullptr, camera);
				}
				else if (param.Distortion)
				{
					Rendering_Internal<VERTEX_DISTORTION>(param, kv.Value, nullptr, camera);
				}
				else
				{
					Rendering_Internal<VERTEX_NORMAL>(param, kv.Value, nullptr, camera);
				}
			}
		}
	}

public:
	void BeginRendering(const efkSpriteNodeParam& parameter, int32_t count, void* userData) override
	{
		BeginRendering_(m_renderer, count, parameter);
	}

	void Rendering(const efkSpriteNodeParam& parameter, const efkSpriteInstanceParam& instanceParameter, void* userData) override
	{
		if (m_spriteCount == m_renderer->GetSquareMaxCount()) return;
		Rendering_(parameter, instanceParameter, userData, m_renderer->GetCameraMatrix());
	}

	void EndRendering(const efkSpriteNodeParam& parameter, void* userData) override
	{
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
#endif	// __EFFEKSEERRENDERER_SPRITE_RENDERER_H__
