
#ifndef __EFFEKSEERRENDERER_RING_RENDERER_BASE_H__
#define __EFFEKSEERRENDERER_RING_RENDERER_BASE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <Effekseer.h>
#include <assert.h>
#include <cmath>
#include <math.h>
#include <string.h>

#include "EffekseerRenderer.CommonUtils.h"
#include "EffekseerRenderer.StandardRenderer.h"
#include "EffekseerRendererCommon/EffekseerRenderer.RenderStateBase.h"

#define __ZOFFSET__

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
typedef ::Effekseer::SIMD::Vec3f efkVector3D;

template <typename RENDERER, bool FLIP_RGB_FLAG>
class RingRendererBase : public ::Effekseer::RingRenderer, public ::Effekseer::SIMD::AlignedAllocationPolicy<16>
{
protected:
	struct RingRenderState
	{
		int32_t singleVertexCount = 0;
		float invVertexCount = 0.0f;

		float circleAngleDegree = 0.0f;
		float stepAngleDegree = 0.0f;
		float stepAngle = 0.0f;
		float beginAngle = 0.0f;

		float outerRadius = 0.0f;
		float innerRadius = 0.0f;
		float centerRadius = 0.0f;

		float outerHeight = 0.0f;
		float innerHeight = 0.0f;
		float centerHeight = 0.0f;

		::Effekseer::Color outerColor;
		::Effekseer::Color innerColor;
		::Effekseer::Color centerColor;

		float uv0Current = 0.0f;
		float uv0Step = 0.0f;
		float uv0v1 = 0.0f;
		float uv0v2 = 0.0f;
		float uv0v3 = 0.0f;

		float uv1Current = 0.0f;
		float uv1Step = 0.0f;
		float uv1v1 = 0.0f;
		float uv1v2 = 0.0f;
		float uv1v3 = 0.0f;

		static const int32_t AdvancedUVNum = 5;
		float advancedUVCurrent[AdvancedUVNum]{};
		float advancedUVStep[AdvancedUVNum]{};
		float advancedUVv1[AdvancedUVNum]{};
		float advancedUVv2[AdvancedUVNum]{};
		float advancedUVv3[AdvancedUVNum]{};

		float currentAngleDegree = 0.0f;
		float fadeStartAngle = 0.0f;
		float fadeEndingAngle = 0.0f;

		float stepC = 0.0f;
		float stepS = 0.0f;
		float cos_ = 0.0f;
		float sin_ = 0.0f;

		::Effekseer::SIMD::Vec3f outerCurrent{};
		::Effekseer::SIMD::Vec3f innerCurrent{};
		::Effekseer::SIMD::Vec3f centerCurrent{};

		ZFixedTransformBlock outerTransform;
		ZFixedTransformBlock innerTransform;
		ZFixedTransformBlock centerTransform;

		RingRenderState(const ::Effekseer::SIMD::Mat43f& mat43, float outerHeight_, float innerHeight_, float centerHeight_)
			: outerTransform(mat43, outerHeight_)
			, innerTransform(mat43, innerHeight_)
			, centerTransform(mat43, centerHeight_)
		{
		}
	};

	static inline float SafeAlpha(float alpha)
	{
		if (!std::isfinite(alpha))
		{
			return 0.0f;
		}
		return Effekseer::Clamp(alpha, 1.0f, 0.0f);
	}

	struct KeyValue
	{
		float Key;
		efkRingInstanceParam Value;
	};
	std::vector<KeyValue> instances_;

	RENDERER* renderer_;
	int32_t ringBufferOffset_;
	uint8_t* ringBufferData_;

	int32_t spriteCount_;
	int32_t instanceCount_;
	::Effekseer::SIMD::Mat44f singleRenderingMatrix_;
	::Effekseer::RendererMaterialType materialType_ = ::Effekseer::RendererMaterialType::Default;

	int32_t vertexCount_ = 0;
	int32_t stride_ = 0;
	int32_t customData1Count_ = 0;
	int32_t customData2Count_ = 0;

public:
	RingRendererBase(RENDERER* renderer)
		: renderer_(renderer)
		, ringBufferOffset_(0)
		, ringBufferData_(nullptr)
		, spriteCount_(0)
		, instanceCount_(0)
	{
	}

	virtual ~RingRendererBase()
	{
	}

protected:
	void RenderingInstance(const efkRingInstanceParam& instanceParameter,
						   const efkRingNodeParam& parameter,
						   const StandardRendererState& state,
						   const ::Effekseer::SIMD::Mat44f& camera)
	{
		const ShaderParameterCollector& collector = state.Collector;
		if (collector.ShaderType == RendererShaderType::Material)
		{
			Rendering_Internal<DynamicVertex, FLIP_RGB_FLAG>(parameter, instanceParameter, camera);
		}
		else if (collector.ShaderType == RendererShaderType::AdvancedLit)
		{
			Rendering_Internal<AdvancedLightingVertex, FLIP_RGB_FLAG>(parameter, instanceParameter, camera);
		}
		else if (collector.ShaderType == RendererShaderType::AdvancedBackDistortion)
		{
			Rendering_Internal<AdvancedLightingVertex, FLIP_RGB_FLAG>(parameter, instanceParameter, camera);
		}
		else if (collector.ShaderType == RendererShaderType::AdvancedUnlit)
		{
			Rendering_Internal<AdvancedSimpleVertex, FLIP_RGB_FLAG>(parameter, instanceParameter, camera);
		}
		else if (collector.ShaderType == RendererShaderType::Lit)
		{
			Rendering_Internal<LightingVertex, FLIP_RGB_FLAG>(parameter, instanceParameter, camera);
		}
		else if (collector.ShaderType == RendererShaderType::BackDistortion)
		{
			Rendering_Internal<LightingVertex, FLIP_RGB_FLAG>(parameter, instanceParameter, camera);
		}
		else
		{
			Rendering_Internal<SimpleVertex, FLIP_RGB_FLAG>(parameter, instanceParameter, camera);
		}
	}

	void BeginRendering_(RENDERER* renderer, int32_t count, const efkRingNodeParam& param, void* userData)
	{
		spriteCount_ = 0;
		const auto singleVertexCount = param.VertexCount * 8;
		const auto singleSpriteCount = param.VertexCount * 2;

		count = (std::min)(count, renderer->GetSquareMaxCount() / singleSpriteCount);

		instanceCount_ = count;

		instances_.clear();

		if (param.DepthParameterPtr->ZSort != Effekseer::ZSortType::None)
		{
			instances_.reserve(count);
		}

		EffekseerRenderer::StandardRendererState state;
		state.CullingType = ::Effekseer::CullingType::Double;
		state.DepthTest = param.ZTest;
		state.DepthWrite = param.ZWrite;

		state.Flipbook = ToState(param.BasicParameterPtr->Flipbook);

		state.UVDistortionIntensity = param.BasicParameterPtr->UVDistortionIntensity;

		state.TextureBlendType = param.BasicParameterPtr->TextureBlendType;

		state.BlendUVDistortionIntensity = param.BasicParameterPtr->BlendUVDistortionIntensity;

		state.EmissiveScaling = param.BasicParameterPtr->EmissiveScaling;

		state.EdgeThreshold = param.BasicParameterPtr->EdgeThreshold;
		state.EdgeColor[0] = param.BasicParameterPtr->EdgeColor[0];
		state.EdgeColor[1] = param.BasicParameterPtr->EdgeColor[1];
		state.EdgeColor[2] = param.BasicParameterPtr->EdgeColor[2];
		state.EdgeColor[3] = param.BasicParameterPtr->EdgeColor[3];
		state.EdgeColorScaling = param.BasicParameterPtr->EdgeColorScaling;
		state.IsAlphaCuttoffEnabled = param.BasicParameterPtr->IsAlphaCutoffEnabled;
		state.Maginification = param.Maginification;

		state.Distortion = param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::BackDistortion;
		state.DistortionIntensity = param.BasicParameterPtr->DistortionIntensity;
		state.MaterialType = param.BasicParameterPtr->MaterialType;

		state.RenderingUserData = param.UserData;
		state.HandleUserData = userData;

		state.LocalTime = param.LocalTime;

		state.CopyMaterialFromParameterToState(
			renderer_,
			param.EffectPointer,
			param.BasicParameterPtr);

		customData1Count_ = state.CustomData1Count;
		customData2Count_ = state.CustomData2Count;

		materialType_ = param.BasicParameterPtr->MaterialType;

		renderer->GetStandardRenderer()->BeginRenderingAndRenderingIfRequired(state, count * singleVertexCount, stride_, (void*&)ringBufferData_);

		vertexCount_ = count * singleVertexCount;
	}

	void Rendering_(const efkRingNodeParam& parameter,
					const efkRingInstanceParam& instanceParameter,
					const ::Effekseer::SIMD::Mat44f& camera)
	{
		if (parameter.DepthParameterPtr->ZSort == Effekseer::ZSortType::None)
		{
			const auto& state = renderer_->GetStandardRenderer()->GetState();

			RenderingInstance(instanceParameter, parameter, state, camera);
		}
		else
		{
			if (instances_.size() >= instanceCount_)
			{
				return;
			}

			KeyValue kv;
			kv.Value = instanceParameter;
			instances_.push_back(kv);
		}
	}

	template <typename VERTEX, bool FLIP_RGB>
	void Rendering_Internal(const efkRingNodeParam& parameter,
							const efkRingInstanceParam& instanceParameter,
							const ::Effekseer::SIMD::Mat44f& camera)
	{
		::Effekseer::SIMD::Mat43f mat43{};

		PrepareMatrix(parameter, instanceParameter, camera, mat43);

		auto state = InitializeRingState(parameter, instanceParameter, mat43);

		RenderRingVertices<VERTEX, FLIP_RGB>(parameter, instanceParameter, state);

		WriteCustomData(instanceParameter, state.singleVertexCount);

		spriteCount_ += 2 * parameter.VertexCount;
		ringBufferData_ += stride_ * state.singleVertexCount;
	}

	void PrepareMatrix(const efkRingNodeParam& parameter,
					   const efkRingInstanceParam& instanceParameter,
					   const ::Effekseer::SIMD::Mat44f& camera,
					   ::Effekseer::SIMD::Mat43f& mat43)
	{
		if (parameter.Billboard != ::Effekseer::BillboardType::Fixed)
		{
			Effekseer::SIMD::Vec3f s;
			Effekseer::SIMD::Vec3f R;
			Effekseer::SIMD::Vec3f F;

			if (parameter.EnableViewOffset)
			{
				Effekseer::SIMD::Mat43f instMat = instanceParameter.SRTMatrix43;

				ApplyViewOffset(instMat, camera, instanceParameter.ViewOffsetDistance);

				CalcBillboard(parameter.Billboard, mat43, s, R, F, instMat, renderer_->GetCameraFrontDirection(), instanceParameter.Direction);
			}
			else
			{
				CalcBillboard(parameter.Billboard, mat43, s, R, F, instanceParameter.SRTMatrix43, renderer_->GetCameraFrontDirection(), instanceParameter.Direction);
			}

			ApplyDepthParameters(mat43,
								 renderer_->GetCameraFrontDirection(),
								 renderer_->GetCameraPosition(),
								 s,
								 parameter.DepthParameterPtr,
								 parameter.IsRightHand);

			mat43 = ::Effekseer::SIMD::Mat43f::Scaling(s) * mat43;
		}
		else if (parameter.Billboard == ::Effekseer::BillboardType::Fixed)
		{
			mat43 = instanceParameter.SRTMatrix43;

			if (parameter.EnableViewOffset)
			{
				ApplyViewOffset(mat43, camera, instanceParameter.ViewOffsetDistance);
			}

			ApplyDepthParameters(mat43,
								 renderer_->GetCameraFrontDirection(),
								 renderer_->GetCameraPosition(),
								 parameter.DepthParameterPtr,
								 parameter.IsRightHand);
		}
	}

	RingRenderState InitializeRingState(const efkRingNodeParam& parameter,
										const efkRingInstanceParam& instanceParameter,
										const ::Effekseer::SIMD::Mat43f& mat43)
	{
		const int32_t singleVertexCount = parameter.VertexCount * 8;
		const float vertexCount = Effekseer::AvoidZero(static_cast<float>(parameter.VertexCount));
		const float invVertexCount = 1.0f / vertexCount;

		const float circleAngleDegree = (instanceParameter.ViewingAngleEnd - instanceParameter.ViewingAngleStart);
		const float stepAngleDegree = circleAngleDegree * invVertexCount;
		const float stepAngle = (stepAngleDegree) / 180.0f * 3.141592f;
		const float beginAngle = (instanceParameter.ViewingAngleStart + 90) / 180.0f * 3.141592f;

		const float outerRadius = instanceParameter.OuterLocation.GetX();
		const float innerRadius = instanceParameter.InnerLocation.GetX();
		const float centerRadius = innerRadius + (outerRadius - innerRadius) * instanceParameter.CenterRatio;

		const float outerHeight = instanceParameter.OuterLocation.GetY();
		const float innerHeight = instanceParameter.InnerLocation.GetY();
		const float centerHeight = innerHeight + (outerHeight - innerHeight) * instanceParameter.CenterRatio;

		RingRenderState state(mat43, outerHeight, innerHeight, centerHeight);

		state.singleVertexCount = singleVertexCount;
		state.invVertexCount = invVertexCount;
		state.circleAngleDegree = circleAngleDegree;
		state.stepAngleDegree = stepAngleDegree;
		state.stepAngle = stepAngle;
		state.beginAngle = beginAngle;

		state.outerRadius = outerRadius;
		state.innerRadius = innerRadius;
		state.centerRadius = centerRadius;

		state.outerHeight = outerHeight;
		state.innerHeight = innerHeight;
		state.centerHeight = centerHeight;

		state.outerColor = instanceParameter.OuterColor;
		state.innerColor = instanceParameter.InnerColor;
		state.centerColor = instanceParameter.CenterColor;

		if (parameter.StartingFade > 0)
		{
			state.outerColor.A = 0;
			state.innerColor.A = 0;
			state.centerColor.A = 0;
		}

		state.stepC = cosf(stepAngle);
		state.stepS = sinf(stepAngle);
		state.cos_ = cosf(beginAngle);
		state.sin_ = sinf(beginAngle);

		state.uv0Current = instanceParameter.UV.X;
		state.uv0Step = instanceParameter.UV.Width * invVertexCount;
		state.uv0v1 = instanceParameter.UV.Y;
		state.uv0v2 = state.uv0v1 + instanceParameter.UV.Height * 0.5f;
		state.uv0v3 = state.uv0v1 + instanceParameter.UV.Height;

		state.uv1Current = 0.0f;
		state.uv1Step = invVertexCount;
		state.uv1v1 = 0.0f;
		state.uv1v2 = state.uv1v1 + 0.5f;
		state.uv1v3 = state.uv1v1 + 1.0f;

		state.advancedUVCurrent[0] = instanceParameter.AlphaUV.X;
		state.advancedUVCurrent[1] = instanceParameter.UVDistortionUV.X;
		state.advancedUVCurrent[2] = instanceParameter.BlendUV.X;
		state.advancedUVCurrent[3] = instanceParameter.BlendAlphaUV.X;
		state.advancedUVCurrent[4] = instanceParameter.BlendUVDistortionUV.X;

		state.advancedUVStep[0] = instanceParameter.AlphaUV.Width * invVertexCount;
		state.advancedUVStep[1] = instanceParameter.UVDistortionUV.Width * invVertexCount;
		state.advancedUVStep[2] = instanceParameter.BlendUV.Width * invVertexCount;
		state.advancedUVStep[3] = instanceParameter.BlendAlphaUV.Width * invVertexCount;
		state.advancedUVStep[4] = instanceParameter.BlendUVDistortionUV.Width * invVertexCount;

		state.advancedUVv1[0] = instanceParameter.AlphaUV.Y;
		state.advancedUVv1[1] = instanceParameter.UVDistortionUV.Y;
		state.advancedUVv1[2] = instanceParameter.BlendUV.Y;
		state.advancedUVv1[3] = instanceParameter.BlendAlphaUV.Y;
		state.advancedUVv1[4] = instanceParameter.BlendUVDistortionUV.Y;

		state.advancedUVv2[0] = state.advancedUVv1[0] + instanceParameter.AlphaUV.Height * 0.5f;
		state.advancedUVv2[1] = state.advancedUVv1[1] + instanceParameter.UVDistortionUV.Height * 0.5f;
		state.advancedUVv2[2] = state.advancedUVv1[2] + instanceParameter.BlendUV.Height * 0.5f;
		state.advancedUVv2[3] = state.advancedUVv1[3] + instanceParameter.BlendAlphaUV.Height * 0.5f;
		state.advancedUVv2[4] = state.advancedUVv1[4] + instanceParameter.BlendUVDistortionUV.Height * 0.5f;

		state.advancedUVv3[0] = state.advancedUVv1[0] + instanceParameter.AlphaUV.Height;
		state.advancedUVv3[1] = state.advancedUVv1[1] + instanceParameter.UVDistortionUV.Height;
		state.advancedUVv3[2] = state.advancedUVv1[2] + instanceParameter.BlendUV.Height;
		state.advancedUVv3[3] = state.advancedUVv1[3] + instanceParameter.BlendAlphaUV.Height;
		state.advancedUVv3[4] = state.advancedUVv1[4] + instanceParameter.BlendUVDistortionUV.Height;

		state.fadeStartAngle = Effekseer::AvoidZero(parameter.StartingFade);
		state.fadeEndingAngle = Effekseer::AvoidZero(parameter.EndingFade);

		state.outerCurrent = ::Effekseer::SIMD::Vec3f(state.cos_ * state.outerRadius, state.sin_ * state.outerRadius, 0.0f);
		state.innerCurrent = ::Effekseer::SIMD::Vec3f(state.cos_ * state.innerRadius, state.sin_ * state.innerRadius, 0.0f);
		state.centerCurrent = ::Effekseer::SIMD::Vec3f(state.cos_ * state.centerRadius, state.sin_ * state.centerRadius, 0.0f);

		state.outerTransform.Transform(state.outerCurrent);
		state.innerTransform.Transform(state.innerCurrent);
		state.centerTransform.Transform(state.centerCurrent);

		return state;
	}

	template <typename VERTEX, bool FLIP_RGB>
	void RenderRingVertices(const efkRingNodeParam& parameter,
							const efkRingInstanceParam& instanceParameter,
							RingRenderState& state)
	{
		StrideView<VERTEX> verteies(ringBufferData_, stride_, state.singleVertexCount);

		::Effekseer::Color outerColor = state.outerColor;
		::Effekseer::Color innerColor = state.innerColor;
		::Effekseer::Color centerColor = state.centerColor;

		::Effekseer::Color outerColorNext = instanceParameter.OuterColor;
		::Effekseer::Color innerColorNext = instanceParameter.InnerColor;
		::Effekseer::Color centerColorNext = instanceParameter.CenterColor;

		float uv0texNext = 0.0f;
		float uv1texNext = 0.0f;
		float advancedUVtexNext[RingRenderState::AdvancedUVNum] = {0.0f};

		::Effekseer::SIMD::Vec3f outerNext, innerNext, centerNext;

		for (int i = 0; i < state.singleVertexCount; i += 8)
		{
			float old_c = state.cos_;
			float old_s = state.sin_;

			float t;
			t = state.cos_ * state.stepC - state.sin_ * state.stepS;
			state.sin_ = state.sin_ * state.stepC + state.cos_ * state.stepS;
			state.cos_ = t;

			outerNext = ::Effekseer::SIMD::Vec3f{state.cos_ * state.outerRadius, state.sin_ * state.outerRadius, 0};
			innerNext = ::Effekseer::SIMD::Vec3f{state.cos_ * state.innerRadius, state.sin_ * state.innerRadius, 0};
			centerNext = ::Effekseer::SIMD::Vec3f{state.cos_ * state.centerRadius, state.sin_ * state.centerRadius, 0};

			state.outerTransform.Transform(outerNext);
			state.innerTransform.Transform(innerNext);
			state.centerTransform.Transform(centerNext);

			state.currentAngleDegree += state.stepAngleDegree;

			// for floating decimal point error
			state.currentAngleDegree = Effekseer::Min(state.currentAngleDegree, state.circleAngleDegree);
			float alpha = 1.0f;
			if (state.currentAngleDegree < state.fadeStartAngle)
			{
				alpha = state.currentAngleDegree / state.fadeStartAngle;
			}
			else if (state.currentAngleDegree > state.circleAngleDegree - state.fadeEndingAngle)
			{
				alpha = 1.0f - (state.currentAngleDegree - (state.circleAngleDegree - state.fadeEndingAngle)) / state.fadeEndingAngle;
			}

			alpha = SafeAlpha(alpha);

			outerColorNext = instanceParameter.OuterColor;
			innerColorNext = instanceParameter.InnerColor;
			centerColorNext = instanceParameter.CenterColor;

			if (alpha != 1.0f)
			{
				outerColorNext.A = static_cast<uint8_t>(outerColorNext.A * alpha);
				innerColorNext.A = static_cast<uint8_t>(innerColorNext.A * alpha);
				centerColorNext.A = static_cast<uint8_t>(centerColorNext.A * alpha);
			}

			uv0texNext = state.uv0Current + state.uv0Step;

			StrideView<VERTEX> v(&verteies[i], stride_, 8);
			v[0].Pos = ToStruct(state.outerCurrent);
			v[0].SetColor(outerColor, FLIP_RGB);
			v[0].UV[0] = state.uv0Current;
			v[0].UV[1] = state.uv0v1;

			v[1].Pos = ToStruct(state.centerCurrent);
			v[1].SetColor(centerColor, FLIP_RGB);
			v[1].UV[0] = state.uv0Current;
			v[1].UV[1] = state.uv0v2;

			v[2].Pos = ToStruct(outerNext);
			v[2].SetColor(outerColorNext, FLIP_RGB);
			v[2].UV[0] = uv0texNext;
			v[2].UV[1] = state.uv0v1;

			v[3].Pos = ToStruct(centerNext);
			v[3].SetColor(centerColorNext, FLIP_RGB);
			v[3].UV[0] = uv0texNext;
			v[3].UV[1] = state.uv0v2;

			v[4] = v[1];

			v[5].Pos = ToStruct(state.innerCurrent);
			v[5].SetColor(innerColor, FLIP_RGB);
			v[5].UV[0] = state.uv0Current;
			v[5].UV[1] = state.uv0v3;

			v[6] = v[3];

			v[7].Pos = ToStruct(innerNext);
			v[7].SetColor(innerColorNext, FLIP_RGB);
			v[7].UV[0] = uv0texNext;
			v[7].UV[1] = state.uv0v3;

			for (int32_t uvi = 0; uvi < RingRenderState::AdvancedUVNum; uvi++)
			{
				advancedUVtexNext[uvi] = state.advancedUVCurrent[uvi] + state.advancedUVStep[uvi];
			}

			SetVertexAlphaUV(v[0], state.advancedUVCurrent[0], 0);
			SetVertexAlphaUV(v[0], state.advancedUVv1[0], 1);

			SetVertexUVDistortionUV(v[0], state.advancedUVCurrent[1], 0);
			SetVertexUVDistortionUV(v[0], state.advancedUVv1[1], 1);

			SetVertexBlendUV(v[0], state.advancedUVCurrent[2], 0);
			SetVertexBlendUV(v[0], state.advancedUVv1[2], 1);

			SetVertexBlendAlphaUV(v[0], state.advancedUVCurrent[3], 0);
			SetVertexBlendAlphaUV(v[0], state.advancedUVv1[3], 1);

			SetVertexBlendUVDistortionUV(v[0], state.advancedUVCurrent[4], 0);
			SetVertexBlendUVDistortionUV(v[0], state.advancedUVv1[4], 1);

			SetVertexAlphaUV(v[1], state.advancedUVCurrent[0], 0);
			SetVertexAlphaUV(v[1], state.advancedUVv2[0], 1);

			SetVertexUVDistortionUV(v[1], state.advancedUVCurrent[1], 0);
			SetVertexUVDistortionUV(v[1], state.advancedUVv2[1], 1);

			SetVertexBlendUV(v[1], state.advancedUVCurrent[2], 0);
			SetVertexBlendUV(v[1], state.advancedUVv2[2], 1);

			SetVertexBlendAlphaUV(v[1], state.advancedUVCurrent[3], 0);
			SetVertexBlendAlphaUV(v[1], state.advancedUVv2[3], 1);

			SetVertexBlendUVDistortionUV(v[1], state.advancedUVCurrent[4], 0);
			SetVertexBlendUVDistortionUV(v[1], state.advancedUVv2[4], 1);

			SetVertexAlphaUV(v[2], advancedUVtexNext[0], 0);
			SetVertexAlphaUV(v[2], state.advancedUVv1[0], 1);

			SetVertexUVDistortionUV(v[2], advancedUVtexNext[1], 0);
			SetVertexUVDistortionUV(v[2], state.advancedUVv1[1], 1);

			SetVertexBlendUV(v[2], advancedUVtexNext[2], 0);
			SetVertexBlendUV(v[2], state.advancedUVv1[2], 1);

			SetVertexBlendAlphaUV(v[2], advancedUVtexNext[3], 0);
			SetVertexBlendAlphaUV(v[2], state.advancedUVv1[3], 1);

			SetVertexBlendUVDistortionUV(v[2], advancedUVtexNext[4], 0);
			SetVertexBlendUVDistortionUV(v[2], state.advancedUVv1[4], 1);

			SetVertexAlphaUV(v[3], advancedUVtexNext[0], 0);
			SetVertexAlphaUV(v[3], state.advancedUVv2[0], 1);

			SetVertexUVDistortionUV(v[3], advancedUVtexNext[1], 0);
			SetVertexUVDistortionUV(v[3], state.advancedUVv2[1], 1);

			SetVertexBlendUV(v[3], advancedUVtexNext[2], 0);
			SetVertexBlendUV(v[3], state.advancedUVv2[2], 1);

			SetVertexBlendAlphaUV(v[3], advancedUVtexNext[3], 0);
			SetVertexBlendAlphaUV(v[3], state.advancedUVv2[3], 1);

			SetVertexBlendUVDistortionUV(v[3], advancedUVtexNext[4], 0);
			SetVertexBlendUVDistortionUV(v[3], state.advancedUVv2[4], 1);

			v[4] = v[1];

			SetVertexAlphaUV(v[5], state.advancedUVCurrent[0], 0);
			SetVertexAlphaUV(v[5], state.advancedUVv3[0], 1);

			SetVertexUVDistortionUV(v[5], state.advancedUVCurrent[1], 0);
			SetVertexUVDistortionUV(v[5], state.advancedUVv3[1], 1);

			SetVertexBlendUV(v[5], state.advancedUVCurrent[2], 0);
			SetVertexBlendUV(v[5], state.advancedUVv3[2], 1);

			SetVertexBlendAlphaUV(v[5], state.advancedUVCurrent[3], 0);
			SetVertexBlendAlphaUV(v[5], state.advancedUVv3[3], 1);

			SetVertexBlendUVDistortionUV(v[5], state.advancedUVCurrent[4], 0);
			SetVertexBlendUVDistortionUV(v[5], state.advancedUVv3[4], 1);

			v[6] = v[3];

			SetVertexAlphaUV(v[7], advancedUVtexNext[0], 0);
			SetVertexAlphaUV(v[7], state.advancedUVv3[0], 1);

			SetVertexUVDistortionUV(v[7], advancedUVtexNext[1], 0);
			SetVertexUVDistortionUV(v[7], state.advancedUVv3[1], 1);

			SetVertexBlendUV(v[7], advancedUVtexNext[2], 0);
			SetVertexBlendUV(v[7], state.advancedUVv3[2], 1);

			SetVertexBlendAlphaUV(v[7], advancedUVtexNext[3], 0);
			SetVertexBlendAlphaUV(v[7], state.advancedUVv3[3], 1);

			SetVertexBlendUVDistortionUV(v[7], advancedUVtexNext[4], 0);
			SetVertexBlendUVDistortionUV(v[7], state.advancedUVv3[4], 1);

			for (int32_t vi = 0; vi < 8; vi++)
			{
				v[vi].SetFlipbookIndexAndNextRate(instanceParameter.FlipbookIndexAndNextRate);
				v[vi].SetAlphaThreshold(instanceParameter.AlphaThreshold);
				v[vi].SetParticleTime(instanceParameter.ParticleTimes[0], instanceParameter.ParticleTimes[1]);
			}

			if (VertexNormalRequired<VERTEX>())
			{
				StrideView<VERTEX> vs(&verteies[i], stride_, 8);

				// return back
				float t_b;
				t_b = old_c * (state.stepC)-old_s * (-state.stepS);
				auto s_b = old_s * (state.stepC) + old_c * (-state.stepS);
				auto c_b = t_b;

				::Effekseer::SIMD::Vec3f outerBefore{c_b * state.outerRadius, s_b * state.outerRadius, 0.0f};
				state.outerTransform.Transform(outerBefore);

				// next
				auto t_n = state.cos_ * state.stepC - state.sin_ * state.stepS;
				auto s_n = state.sin_ * state.stepC + state.cos_ * state.stepS;
				auto c_n = t_n;

				::Effekseer::SIMD::Vec3f outerNN{c_n * state.outerRadius, s_n * state.outerRadius, 0.0f};
				state.outerTransform.Transform(outerNN);

				::Effekseer::SIMD::Vec3f tangent0 = (state.outerCurrent - outerBefore).GetNormal();
				::Effekseer::SIMD::Vec3f tangent1 = (outerNext - state.outerCurrent).GetNormal();
				::Effekseer::SIMD::Vec3f tangent2 = (outerNN - outerNext).GetNormal();

				auto tangentCurrent = (tangent0 + tangent1) / 2.0f;
				auto tangentNext = (tangent1 + tangent2) / 2.0f;

				auto binormalCurrent = v[5].Pos - v[0].Pos;
				auto binormalNext = v[7].Pos - v[2].Pos;

				::Effekseer::SIMD::Vec3f normalCurrent;
				::Effekseer::SIMD::Vec3f normalNext;

				normalCurrent = ::Effekseer::SIMD::Vec3f::Cross(tangentCurrent, binormalCurrent);
				normalNext = ::Effekseer::SIMD::Vec3f::Cross(tangentNext, binormalNext);

				if (!parameter.IsRightHand)
				{
					normalCurrent = -normalCurrent;
					normalNext = -normalNext;
				}

				normalCurrent = normalCurrent.GetNormal();
				normalNext = normalNext.GetNormal();

				tangentCurrent = tangentCurrent.GetNormal();
				tangentNext = tangentNext.GetNormal();

				const auto packedNormalCurrent = PackVector3DF(normalCurrent);
				const auto packedNormalNext = PackVector3DF(normalNext);
				const auto packedTangentCurrent = PackVector3DF(tangentCurrent);
				const auto packedTangentNext = PackVector3DF(tangentNext);

				vs[0].SetPackedNormal(packedNormalCurrent, FLIP_RGB);
				vs[1].SetPackedNormal(packedNormalCurrent, FLIP_RGB);
				vs[2].SetPackedNormal(packedNormalNext, FLIP_RGB);
				vs[3].SetPackedNormal(packedNormalNext, FLIP_RGB);

				vs[4].SetPackedNormal(packedNormalCurrent, FLIP_RGB);
				vs[5].SetPackedNormal(packedNormalCurrent, FLIP_RGB);
				vs[6].SetPackedNormal(packedNormalNext, FLIP_RGB);
				vs[7].SetPackedNormal(packedNormalNext, FLIP_RGB);

				vs[0].SetPackedTangent(packedTangentCurrent, FLIP_RGB);
				vs[1].SetPackedTangent(packedTangentCurrent, FLIP_RGB);
				vs[2].SetPackedTangent(packedTangentNext, FLIP_RGB);
				vs[3].SetPackedTangent(packedTangentNext, FLIP_RGB);

				vs[4].SetPackedTangent(packedTangentCurrent, FLIP_RGB);
				vs[5].SetPackedTangent(packedTangentCurrent, FLIP_RGB);
				vs[6].SetPackedTangent(packedTangentNext, FLIP_RGB);
				vs[7].SetPackedTangent(packedTangentNext, FLIP_RGB);

				// uv1
				uv1texNext = state.uv1Current + state.uv1Step;

				vs[0].SetUV2(state.uv1Current, state.uv1v1);
				vs[1].SetUV2(state.uv1Current, state.uv1v2);
				vs[2].SetUV2(uv1texNext, state.uv1v1);
				vs[3].SetUV2(uv1texNext, state.uv1v2);

				vs[4].SetUV2(state.uv1Current, state.uv1v2);

				vs[5].SetUV2(state.uv1Current, state.uv1v3);

				vs[6].SetUV2(uv1texNext, state.uv1v2);

				vs[7].SetUV2(uv1texNext, state.uv1v3);
			}

			state.outerCurrent = outerNext;
			state.innerCurrent = innerNext;
			state.centerCurrent = centerNext;
			state.uv0Current = uv0texNext;
			state.uv1Current = uv1texNext;
			for (int32_t uvi = 0; uvi < RingRenderState::AdvancedUVNum; uvi++)
			{
				state.advancedUVCurrent[uvi] = advancedUVtexNext[uvi];
			}

			outerColor = outerColorNext;
			innerColor = innerColorNext;
			centerColor = centerColorNext;
		}
	}

	void WriteCustomData(const efkRingInstanceParam& instanceParameter, int32_t singleVertexCount)
	{
		// custom parameter
		if (customData1Count_ > 0)
		{
			StrideView<float> custom(ringBufferData_ + sizeof(DynamicVertex), stride_, singleVertexCount);
			for (int i = 0; i < singleVertexCount; i++)
			{
				auto c = (float*)(&custom[i]);
				memcpy(c, instanceParameter.CustomData1.data(), sizeof(float) * customData1Count_);
			}
		}

		if (customData2Count_ > 0)
		{
			StrideView<float> custom(
				ringBufferData_ + sizeof(DynamicVertex) + sizeof(float) * customData1Count_, stride_, singleVertexCount);
			for (int i = 0; i < singleVertexCount; i++)
			{
				auto c = (float*)(&custom[i]);
				memcpy(c, instanceParameter.CustomData2.data(), sizeof(float) * customData2Count_);
			}
		}
	}

	void EndRendering_(RENDERER* renderer, const efkRingNodeParam& param, const ::Effekseer::SIMD::Mat44f& camera)
	{
		if (param.DepthParameterPtr->ZSort != Effekseer::ZSortType::None)
		{
			for (auto& kv : instances_)
			{
				efkVector3D t = kv.Value.SRTMatrix43.GetTranslation();

				Effekseer::SIMD::Vec3f frontDirection = renderer_->GetCameraFrontDirection();
				if (!param.IsRightHand)
				{
					frontDirection = -frontDirection;
				}

				kv.Key = Effekseer::SIMD::Vec3f::Dot(t, frontDirection);
			}

			if (param.DepthParameterPtr->ZSort == Effekseer::ZSortType::NormalOrder)
			{
				std::sort(instances_.begin(), instances_.end(), [](const KeyValue& a, const KeyValue& b) -> bool
						  { return a.Key < b.Key; });
			}
			else
			{
				std::sort(instances_.begin(), instances_.end(), [](const KeyValue& a, const KeyValue& b) -> bool
						  { return a.Key > b.Key; });
			}

			const auto& state = renderer_->GetStandardRenderer()->GetState();

			for (auto& kv : instances_)
			{
				RenderingInstance(kv.Value, param, state, camera);
			}
		}

		renderer->GetStandardRenderer()->EndRenderingAndRenderingIfRequired();
	}

public:
	void BeginRendering(const efkRingNodeParam& parameter, int32_t count, void* userData)
	{
		BeginRendering_(renderer_, count, parameter, userData);
	}

	void Rendering(const efkRingNodeParam& parameter, const efkRingInstanceParam& instanceParameter, void* userData)
	{
		if (ringBufferData_ == nullptr)
			return;
		if (spriteCount_ + 2 * parameter.VertexCount > renderer_->GetSquareMaxCount())
			return;
		Rendering_(parameter, instanceParameter, renderer_->GetCameraMatrix());
	}

	void EndRendering(const efkRingNodeParam& parameter, void* userData)
	{
		if (ringBufferData_ == nullptr)
			return;
		if (spriteCount_ == 0 && parameter.DepthParameterPtr->ZSort == Effekseer::ZSortType::None)
			return;

		EndRendering_(renderer_, parameter, renderer_->GetCameraMatrix());
	}
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRenderer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_RING_RENDERER_H__
