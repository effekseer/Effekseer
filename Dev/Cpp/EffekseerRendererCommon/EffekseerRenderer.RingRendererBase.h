
#ifndef	__EFFEKSEERRENDERER_RING_RENDERER_BASE_H__
#define	__EFFEKSEERRENDERER_RING_RENDERER_BASE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <Effekseer.h>
#include <assert.h>
#include <string.h>
#include <math.h>

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
typedef ::Effekseer::RingRenderer::NodeParameter efkRingNodeParam;
typedef ::Effekseer::RingRenderer::InstanceParameter efkRingInstanceParam;
typedef ::Effekseer::Vector3D efkVector3D;

template<typename RENDERER, typename VERTEX_NORMAL, typename VERTEX_DISTORTION>
class RingRendererBase
	: public ::Effekseer::RingRenderer
{
protected:
	RENDERER*						m_renderer;
	int32_t							m_ringBufferOffset;
	uint8_t*						m_ringBufferData;

	int32_t							m_spriteCount;
	int32_t							m_instanceCount;
	::Effekseer::Matrix44			m_singleRenderingMatrix;
	::Effekseer::RendererMaterialType materialType_ = ::Effekseer::RendererMaterialType::Default;

	int32_t vertexCount_ = 0;
	int32_t stride_ = 0;
	int32_t customData1Count_ = 0;
	int32_t customData2Count_ = 0;

public:

	RingRendererBase(RENDERER* renderer)
		: m_renderer(renderer)
		, m_ringBufferOffset(0)
		, m_ringBufferData(NULL)
		, m_spriteCount(0)
		, m_instanceCount(0)
	{
	}

	virtual ~RingRendererBase()
	{
	}

protected:

	void BeginRendering_(RENDERER* renderer, int32_t count, const efkRingNodeParam& param)
	{
		m_spriteCount = 0;
		int32_t singleVertexCount = param.VertexCount * 8;
		m_instanceCount = count;

		if (count == 1)
		{
			renderer->GetStandardRenderer()->ResetAndRenderingIfRequired();
		}
		
		EffekseerRenderer::StandardRendererState state;
		state.AlphaBlend = param.BasicParameterPtr->AlphaBlend;
		state.CullingType = ::Effekseer::CullingType::Double;
		state.DepthTest = param.ZTest;
		state.DepthWrite = param.ZWrite;
		state.TextureFilter1 = param.BasicParameterPtr->TextureFilter1;
		state.TextureWrap1 = param.BasicParameterPtr->TextureWrap1;
		state.TextureFilter2 = param.BasicParameterPtr->TextureFilter2;
		state.TextureWrap2 = param.BasicParameterPtr->TextureWrap2;

		state.Distortion = param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::BackDistortion;
		state.DistortionIntensity = param.BasicParameterPtr->DistortionIntensity;
		state.MaterialType = param.BasicParameterPtr->MaterialType;

		state.CopyMaterialFromParameterToState(param.EffectPointer,
											   param.BasicParameterPtr->MaterialParameterPtr,
											   param.BasicParameterPtr->Texture1Index,
											   param.BasicParameterPtr->Texture2Index);

		customData1Count_ = state.CustomData1Count;
		customData2Count_ = state.CustomData2Count;

		materialType_ = param.BasicParameterPtr->MaterialType;

		renderer->GetStandardRenderer()->UpdateStateAndRenderingIfRequired(state);
		renderer->GetStandardRenderer()->BeginRenderingAndRenderingIfRequired(count * singleVertexCount, stride_, (void*&)m_ringBufferData);

		vertexCount_ = count * singleVertexCount;
	}

	void Rendering_(const efkRingNodeParam& parameter, const efkRingInstanceParam& instanceParameter, void* userData, const ::Effekseer::Matrix44& camera)
	{
		const auto& state = m_renderer->GetStandardRenderer()->GetState();

		if ((state.MaterialPtr != nullptr && !state.MaterialPtr->IsSimpleVertex) || 
			parameter.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::Lighting)
		{
			Rendering_Internal<DynamicVertex>(parameter, instanceParameter, userData, camera);
		}
		else if (parameter.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::BackDistortion)
		{
			Rendering_Internal<VERTEX_DISTORTION>(parameter, instanceParameter, userData, camera);
		}
		else
		{
			Rendering_Internal<VERTEX_NORMAL>(parameter, instanceParameter, userData, camera);
		}
	}

	enum class VertexType
	{
		Normal,
		Distortion,
		Dynamic,
	};

	VertexType GetVertexType(const VERTEX_NORMAL* v) { return VertexType::Normal; }

	VertexType GetVertexType(const VERTEX_DISTORTION* v) { return VertexType::Distortion; }

	VertexType GetVertexType(const DynamicVertex* v) { return VertexType::Dynamic; }

	bool CanSingleRendering()
	{ 
		return m_instanceCount <= 1 && materialType_ == ::Effekseer::RendererMaterialType::Default;
	}

	template<typename VERTEX>
	void Rendering_Internal( const efkRingNodeParam& parameter, const efkRingInstanceParam& instanceParameter, void* userData, const ::Effekseer::Matrix44& camera )
	{
		::Effekseer::Matrix43 mat_rot;

		if (parameter.Billboard == ::Effekseer::BillboardType::Billboard ||
			parameter.Billboard == ::Effekseer::BillboardType::RotatedBillboard ||
			parameter.Billboard == ::Effekseer::BillboardType::YAxisFixed)
		{
			Effekseer::Vector3D s;
			Effekseer::Vector3D R;
			Effekseer::Vector3D F;

			CalcBillboard(parameter.Billboard, mat_rot, s, R, F, instanceParameter.SRTMatrix43, m_renderer->GetCameraFrontDirection());

			ApplyDepthParameters(mat_rot,
								 m_renderer->GetCameraFrontDirection(),
								 m_renderer->GetCameraPosition(),
								 s,
								 parameter.DepthParameterPtr,
								 parameter.IsRightHand);

			if (CanSingleRendering())
			{
				::Effekseer::Matrix43 mat_scale;
				mat_scale.Scaling(s.X, s.Y, s.Z);
				::Effekseer::Matrix43::Multiple(mat_rot, mat_scale, mat_rot);
			}
			else
			{
				::Effekseer::Matrix43 mat_scale;
				mat_scale.Scaling(s.X, s.Y, s.Z);
				::Effekseer::Matrix43::Multiple(mat_rot, mat_scale, mat_rot);
			}
		}
		else if (parameter.Billboard == ::Effekseer::BillboardType::Fixed)
		{
			mat_rot = instanceParameter.SRTMatrix43;

			ApplyDepthParameters(mat_rot,
								 m_renderer->GetCameraFrontDirection(),
								 m_renderer->GetCameraPosition(),
								 parameter.DepthParameterPtr,
								 parameter.IsRightHand);
		}

		int32_t singleVertexCount = parameter.VertexCount * 8;
		//Vertex* verteies = (Vertex*)m_renderer->GetVertexBuffer()->GetBufferDirect( sizeof(Vertex) * vertexCount );
		
		StrideView<VERTEX> verteies(m_ringBufferData, stride_, singleVertexCount);
		auto vertexType = GetVertexType((VERTEX*)m_ringBufferData);

		const float circleAngleDegree = (instanceParameter.ViewingAngleEnd - instanceParameter.ViewingAngleStart);
		const float stepAngleDegree = circleAngleDegree / (parameter.VertexCount);
		const float stepAngle = (stepAngleDegree) / 180.0f * 3.141592f;
		const float beginAngle = (instanceParameter.ViewingAngleStart + 90) / 180.0f * 3.141592f;
		
		const float outerRadius = instanceParameter.OuterLocation.X;
		const float innerRadius = instanceParameter.InnerLocation.X;
		const float centerRadius = innerRadius + (outerRadius - innerRadius) * instanceParameter.CenterRatio;

		const float outerHeight = instanceParameter.OuterLocation.Y;
		const float innerHeight = instanceParameter.InnerLocation.Y;
		const float centerHeight = innerHeight + (outerHeight - innerHeight) * instanceParameter.CenterRatio;
		
		::Effekseer::Color outerColor = instanceParameter.OuterColor;
		::Effekseer::Color innerColor = instanceParameter.InnerColor;
		::Effekseer::Color centerColor = instanceParameter.CenterColor;
		::Effekseer::Color outerColorNext = instanceParameter.OuterColor;
		::Effekseer::Color innerColorNext = instanceParameter.InnerColor;
		::Effekseer::Color centerColorNext = instanceParameter.CenterColor;

		if (parameter.StartingFade > 0)
		{
			outerColor.A = 0;
			innerColor.A = 0;
			centerColor.A = 0;
		}

		const float stepC = cosf(stepAngle);
		const float stepS = sinf(stepAngle);
		float cos_ = cosf(beginAngle);
		float sin_ = sinf(beginAngle);
		::Effekseer::Vector3D outerCurrent( cos_ * outerRadius, sin_ * outerRadius, outerHeight );
		::Effekseer::Vector3D innerCurrent( cos_ * innerRadius, sin_ * innerRadius, innerHeight );
		::Effekseer::Vector3D centerCurrent( cos_ * centerRadius, sin_ * centerRadius, centerHeight );

		float uv0Current = instanceParameter.UV.X;
		const float uv0Step = instanceParameter.UV.Width / parameter.VertexCount;
		const float uv0v1 = instanceParameter.UV.Y;
		const float uv0v2 = uv0v1 + instanceParameter.UV.Height * 0.5f;
		const float uv0v3 = uv0v1 + instanceParameter.UV.Height;
		float uv0texNext = 0.0f;
		
		float uv1Current = 0.0f;
		const float uv1Step = 1.0f / parameter.VertexCount;
		const float uv1v1 = 0.0f;
		const float uv1v2 = uv1v1 + 0.5f;
		const float uv1v3 = uv1v1 + 1.0f;
		float uv1texNext = 0.0f;

		::Effekseer::Vector3D outerNext, innerNext, centerNext;

		float currentAngleDegree = 0;
		float fadeStartAngle = parameter.StartingFade;
		float fadeEndingAngle = parameter.EndingFade;

		for( int i = 0; i < singleVertexCount; i += 8 )
		{
			float old_c = cos_;
			float old_s = sin_;

			float t;
			t = cos_ * stepC - sin_ * stepS;
			sin_ = sin_ * stepC + cos_ * stepS;
			cos_ = t;

			outerNext.X = cos_ * outerRadius;
			outerNext.Y = sin_ * outerRadius;
			outerNext.Z = outerHeight;
			innerNext.X = cos_ * innerRadius;
			innerNext.Y = sin_ * innerRadius;
			innerNext.Z = innerHeight;
			centerNext.X = cos_ * centerRadius;
			centerNext.Y = sin_ * centerRadius;
			centerNext.Z = centerHeight;

			currentAngleDegree += stepAngleDegree;

			// for floating decimal point error
			currentAngleDegree = Effekseer::Min(currentAngleDegree, circleAngleDegree);
			float alpha = 1.0f;
			if (currentAngleDegree < fadeStartAngle)
			{
				alpha = currentAngleDegree / fadeStartAngle;
			}
			else if (currentAngleDegree > circleAngleDegree - fadeEndingAngle)
			{
				alpha = 1.0f - (currentAngleDegree - (circleAngleDegree - fadeEndingAngle)) / fadeEndingAngle;
			}

			outerColorNext = instanceParameter.OuterColor;
			innerColorNext = instanceParameter.InnerColor;
			centerColorNext = instanceParameter.CenterColor;

			if (alpha != 1.0f)
			{
				outerColorNext.A *= alpha;
				innerColorNext.A *= alpha;
				centerColorNext.A *= alpha;
			}

			uv0texNext = uv0Current + uv0Step;

			VERTEX* v = &verteies[i];
			v[0].Pos = outerCurrent;
			v[0].SetColor( outerColor );
			v[0].UV[0] = uv0Current;
			v[0].UV[1] = uv0v1;

			v[1].Pos = centerCurrent;
			v[1].SetColor( centerColor );
			v[1].UV[0] = uv0Current;
			v[1].UV[1] = uv0v2;

			v[2].Pos = outerNext;
			v[2].SetColor( outerColorNext );
			v[2].UV[0] = uv0texNext;
			v[2].UV[1] = uv0v1;
			
			v[3].Pos = centerNext;
			v[3].SetColor( centerColorNext );
			v[3].UV[0] = uv0texNext;
			v[3].UV[1] = uv0v2;

			v[4] = v[1];

			v[5].Pos = innerCurrent;
			v[5].SetColor( innerColor );
			v[5].UV[0] = uv0Current;
			v[5].UV[1] = uv0v3;

			v[6] = v[3];

			v[7].Pos = innerNext;
			v[7].SetColor( innerColorNext );
			v[7].UV[0] = uv0texNext;
			v[7].UV[1] = uv0v3;

			// distortion
			if (vertexType == VertexType::Distortion)
			{
				auto vs = (VERTEX_DISTORTION*) &verteies[i];
				auto binormalCurrent = v[5].Pos - v[0].Pos;
				auto binormalNext = v[7].Pos - v[2].Pos;

				// return back
				float t_b;
				t_b = old_c * (stepC) - old_s * (-stepS);
				auto s_b = old_s * (stepC) + old_c * (-stepS);
				auto c_b = t_b;

				::Effekseer::Vector3D outerBefore;
				outerBefore.X = c_b * outerRadius;
				outerBefore.Y = s_b * outerRadius;
				outerBefore.Z = outerHeight;

				// next
				auto t_n = cos_ * stepC - sin_ * stepS;
				auto s_n = sin_ * stepC + cos_ * stepS;
				auto c_n = t_n;

				::Effekseer::Vector3D outerNN;
				outerNN.X = c_n * outerRadius;
				outerNN.Y = s_n * outerRadius;
				outerNN.Z = outerHeight;

				::Effekseer::Vector3D tangent0, tangent1, tangent2;
				::Effekseer::Vector3D::Normal(tangent0, outerCurrent - outerBefore);
				::Effekseer::Vector3D::Normal(tangent1, outerNext - outerCurrent);
				::Effekseer::Vector3D::Normal(tangent2, outerNN - outerNext);

				auto tangentCurrent = (tangent0 + tangent1) / 2.0f;
				auto tangentNext = (tangent1 + tangent2) / 2.0f;

				vs[0].Tangent = tangentCurrent;
				vs[0].Binormal = binormalCurrent;
				vs[1].Tangent = tangentCurrent;
				vs[1].Binormal = binormalCurrent;
				vs[2].Tangent = tangentNext;
				vs[2].Binormal = binormalNext;
				vs[3].Tangent = tangentNext;
				vs[3].Binormal = binormalNext;

				vs[4].Tangent = tangentCurrent;
				vs[4].Binormal = binormalCurrent;
				vs[5].Tangent = tangentCurrent;
				vs[5].Binormal = binormalCurrent;
				vs[6].Tangent = tangentNext;
				vs[6].Binormal = binormalNext;
				vs[7].Tangent = tangentNext;
				vs[7].Binormal = binormalNext;
			}
			else if (vertexType == VertexType::Dynamic)
			{
				auto vs = (DynamicVertex*)&verteies[i];

				// return back
				float t_b;
				t_b = old_c * (stepC)-old_s * (-stepS);
				auto s_b = old_s * (stepC) + old_c * (-stepS);
				auto c_b = t_b;

				::Effekseer::Vector3D outerBefore;
				outerBefore.X = c_b * outerRadius;
				outerBefore.Y = s_b * outerRadius;
				outerBefore.Z = outerHeight;

				// next
				auto t_n = cos_ * stepC - sin_ * stepS;
				auto s_n = sin_ * stepC + cos_ * stepS;
				auto c_n = t_n;

				::Effekseer::Vector3D outerNN;
				outerNN.X = c_n * outerRadius;
				outerNN.Y = s_n * outerRadius;
				outerNN.Z = outerHeight;

				::Effekseer::Vector3D tangent0, tangent1, tangent2;
				::Effekseer::Vector3D::Normal(tangent0, outerCurrent - outerBefore);
				::Effekseer::Vector3D::Normal(tangent1, outerNext - outerCurrent);
				::Effekseer::Vector3D::Normal(tangent2, outerNN - outerNext);

				auto tangentCurrent = (tangent0 + tangent1) / 2.0f;
				auto tangentNext = (tangent1 + tangent2) / 2.0f;

				auto binormalCurrent = v[5].Pos - v[0].Pos;
				auto binormalNext = v[7].Pos - v[2].Pos;

				::Effekseer::Vector3D normalCurrent;
				::Effekseer::Vector3D normalNext;

				::Effekseer::Vector3D::Cross(normalCurrent, tangentCurrent, binormalCurrent);
				::Effekseer::Vector3D::Cross(normalNext, tangentNext, binormalNext);
				::Effekseer::Vector3D::Normal(normalCurrent, normalCurrent);
				::Effekseer::Vector3D::Normal(normalNext, normalNext);

				// rotate directions
				::Effekseer::Vector3D::Transform(normalCurrent, normalCurrent, mat_rot);
				::Effekseer::Vector3D::Transform(normalNext, normalNext, mat_rot);
				::Effekseer::Vector3D::Transform(tangentCurrent, tangentCurrent, mat_rot);
				::Effekseer::Vector3D::Transform(tangentNext, tangentNext, mat_rot);

				vs[0].Normal = PackVector3DF(normalCurrent);
				vs[1].Normal = vs[0].Normal;
				vs[2].Normal = PackVector3DF(normalNext);
				vs[3].Normal = vs[2].Normal;

				vs[4].Normal = vs[0].Normal;
				vs[5].Normal = vs[0].Normal;
				vs[6].Normal = vs[2].Normal;
				vs[7].Normal = vs[2].Normal;

				vs[0].Tangent = PackVector3DF(tangentCurrent);
				vs[1].Tangent = vs[0].Tangent;
				vs[2].Tangent = PackVector3DF(tangentNext);
				vs[3].Tangent = vs[2].Tangent;

				vs[4].Tangent = vs[0].Tangent;
				vs[5].Tangent = vs[0].Tangent;
				vs[6].Tangent = vs[2].Tangent;
				vs[7].Tangent = vs[2].Tangent;

				// uv1
				uv1texNext = uv1Current + uv1Step;

				vs[0].UV2[0] = uv1Current;
				vs[0].UV2[1] = uv1v1;
				 
				vs[1].UV2[0] = uv1Current;
				vs[1].UV2[1] = uv1v2;
				 
				vs[2].UV2[0] = uv1texNext;
				vs[2].UV2[1] = uv1v1;
				 
				vs[3].UV2[0] = uv1texNext;
				vs[3].UV2[1] = uv1v2;
				 
				vs[4].UV2[0] = vs[1].UV2[0];
				vs[4].UV2[1] = vs[1].UV2[1];
				 
				vs[5].UV2[0] = uv1Current;
				vs[5].UV2[1] = uv1v3;
				 
				vs[6].UV2[0] = vs[3].UV2[0];
				vs[6].UV2[1] = vs[3].UV2[1];
				 
				vs[7].UV2[0] = uv1texNext;
				vs[7].UV2[1] = uv1v3;
			}

			outerCurrent = outerNext;
			innerCurrent = innerNext;
			centerCurrent = centerNext;
			uv0Current = uv0texNext;
			uv1Current = uv1texNext;
			outerColor = outerColorNext;
			innerColor = innerColorNext;
			centerColor = centerColorNext;
		}

		if (CanSingleRendering())
		{
			for (int32_t i = 0; i < 4; i++)
			{
				m_singleRenderingMatrix.Values[i][0] = mat_rot.Value[i][0];
				m_singleRenderingMatrix.Values[i][1] = mat_rot.Value[i][1];
				m_singleRenderingMatrix.Values[i][2] = mat_rot.Value[i][2];
			}
		}
		else
		{
			TransformVertexes(verteies, singleVertexCount, mat_rot);
		}

		// custom parameter
		if (customData1Count_ > 0)
		{
			StrideView<float> custom(m_ringBufferData + sizeof(DynamicVertex), stride_, singleVertexCount);
			for (int i = 0; i < singleVertexCount; i++)
			{
				auto c = (float*)(&custom[i]);
				memcpy(c, instanceParameter.CustomData1.data(), sizeof(float) * customData1Count_);
			}
		}

		if (customData2Count_ > 0)
		{
			StrideView<float> custom(
				m_ringBufferData + sizeof(DynamicVertex) + sizeof(float) * customData1Count_, stride_, singleVertexCount);
			for (int i = 0; i < singleVertexCount; i++)
			{
				auto c = (float*)(&custom[i]);
				memcpy(c, instanceParameter.CustomData2.data(), sizeof(float) * customData2Count_);
			}
		}

		m_spriteCount += 2 * parameter.VertexCount;
		m_ringBufferData += stride_ * singleVertexCount;

	}

	void EndRendering_(RENDERER* renderer, const efkRingNodeParam& param)
	{
		if (CanSingleRendering())
		{

			::Effekseer::Matrix44 mat;
			::Effekseer::Matrix44::Mul(mat, m_singleRenderingMatrix, renderer->GetCameraMatrix());

			renderer->GetStandardRenderer()->Rendering(mat, renderer->GetProjectionMatrix());
		}
	}

public:
	void BeginRendering(const efkRingNodeParam& parameter, int32_t count, void* userData)
	{
		BeginRendering_(m_renderer, count, parameter);
	}

	void Rendering(const efkRingNodeParam& parameter, const efkRingInstanceParam& instanceParameter, void* userData)
	{
		if (m_spriteCount == m_renderer->GetSquareMaxCount()) return;
		Rendering_(parameter, instanceParameter, userData, m_renderer->GetCameraMatrix());
	}

	void EndRendering(const efkRingNodeParam& parameter, void* userData)
	{
		if (m_ringBufferData == NULL) return;

		if (m_spriteCount == 0) return;

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
#endif	// __EFFEKSEERRENDERER_RING_RENDERER_H__