
#ifndef __EFFEKSEERRENDERER_RIBBON_RENDERER_BASE_H__
#define __EFFEKSEERRENDERER_RIBBON_RENDERER_BASE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <Effekseer.h>
#include <assert.h>
#include <string.h>

#include "EffekseerRenderer.CommonUtils.h"
#include "EffekseerRenderer.RenderStateBase.h"
#include "EffekseerRenderer.StandardRenderer.h"
#include "TrailRenderer.h"

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
typedef ::Effekseer::SIMD::Vec3f efkVector3D;

template <typename RENDERER, bool FLIP_RGB_FLAG>
class RibbonRendererBase : public ::Effekseer::RibbonRenderer, public ::Effekseer::SIMD::AlignedAllocationPolicy<16>
{
private:
protected:
	RENDERER* renderer_;
	int32_t ribbonCount_;

	int32_t ringBufferOffset_;
	uint8_t* ringBufferData_;

	Effekseer::CustomAlignedVector<efkRibbonInstanceParam> instances;
	Effekseer::SplineGenerator spline_left;
	Effekseer::SplineGenerator spline_right;

	int32_t vertexCount_ = 0;
	int32_t stride_ = 0;

	int32_t customData1Count_ = 0;
	int32_t customData2Count_ = 0;

	template <typename VERTEX, bool FLIP_RGB>
	void RenderSplines(const efkRibbonNodeParam& parameter, const ::Effekseer::SIMD::Mat44f& camera)
	{
		if (instances.size() == 0)
		{
			return;
		}

		// Calculate spline
		if (parameter.SplineDivision > 1)
		{
			spline_left.Reset();
			spline_right.Reset();

			for (size_t loop = 0; loop < instances.size(); loop++)
			{
				auto& param = instances[loop];

				efkVector3D pl(param.Positions[0], 0.0f, 0.0f);
				efkVector3D pr(param.Positions[1], 0.0f, 0.0f);

				if (parameter.ViewpointDependent)
				{
					::Effekseer::SIMD::Mat43f mat = param.SRTMatrix43;

					if (parameter.EnableViewOffset)
					{
						ApplyViewOffset(mat, camera, param.ViewOffsetDistance);
					}

					::Effekseer::SIMD::Vec3f s;
					::Effekseer::SIMD::Mat43f r;
					::Effekseer::SIMD::Vec3f t;
					mat.GetSRT(s, r, t);

					ApplyDepthParameters(r,
										 t,
										 s,
										 renderer_->GetCameraFrontDirection(),
										 renderer_->GetCameraPosition(),
										 parameter.DepthParameterPtr,
										 parameter.IsRightHand);

					// extend
					pl.SetX(pl.GetX() * s.GetX());
					pr.SetX(pr.GetX() * s.GetX());

					::Effekseer::SIMD::Vec3f F;
					::Effekseer::SIMD::Vec3f R;
					::Effekseer::SIMD::Vec3f U;

					U = ::Effekseer::SIMD::Vec3f(r.X.GetY(), r.Y.GetY(), r.X.GetY());
					F = ::Effekseer::SIMD::Vec3f(-renderer_->GetCameraFrontDirection()).GetNormal();
					R = ::Effekseer::SIMD::Vec3f::Cross(U, F).GetNormal();
					F = ::Effekseer::SIMD::Vec3f::Cross(R, U).GetNormal();

					::Effekseer::SIMD::Mat43f mat_rot(-R.GetX(),
													  -R.GetY(),
													  -R.GetZ(),
													  U.GetX(),
													  U.GetY(),
													  U.GetZ(),
													  F.GetX(),
													  F.GetY(),
													  F.GetZ(),
													  t.GetX(),
													  t.GetY(),
													  t.GetZ());

					pl = ::Effekseer::SIMD::Vec3f::Transform(pl, mat_rot);
					pr = ::Effekseer::SIMD::Vec3f::Transform(pr, mat_rot);

					spline_left.AddVertex(pl);
					spline_right.AddVertex(pr);
				}
				else
				{
					::Effekseer::SIMD::Mat43f mat = param.SRTMatrix43;

					if (parameter.EnableViewOffset == true)
					{
						ApplyViewOffset(mat, camera, param.ViewOffsetDistance);
					}

					ApplyDepthParameters(mat,
										 renderer_->GetCameraFrontDirection(),
										 renderer_->GetCameraPosition(),
										 // s,
										 parameter.DepthParameterPtr,
										 parameter.IsRightHand);

					pl = ::Effekseer::SIMD::Vec3f::Transform(pl, mat);
					pr = ::Effekseer::SIMD::Vec3f::Transform(pr, mat);

					spline_left.AddVertex(pl);
					spline_right.AddVertex(pr);
				}
			}

			spline_left.Calculate();
			spline_right.Calculate();
		}

		StrideView<VERTEX> verteies(ringBufferData_, stride_, vertexCount_);
		for (size_t loop = 0; loop < instances.size(); loop++)
		{
			auto& param = instances[loop];

			for (auto sploop = 0; sploop < parameter.SplineDivision; sploop++)
			{
				bool isFirst = param.InstanceIndex == 0 && sploop == 0;
				bool isLast = param.InstanceIndex == (param.InstanceCount - 1);

				float percent_instance = sploop / (float)parameter.SplineDivision;

				if (parameter.SplineDivision > 1)
				{
					verteies[0].Pos = ToStruct(spline_left.GetValue(param.InstanceIndex + sploop / (float)parameter.SplineDivision));
					verteies[5].Pos = ToStruct(spline_right.GetValue(param.InstanceIndex + sploop / (float)parameter.SplineDivision));

					verteies[0].SetColor(Effekseer::Color::Lerp(param.Colors[0], param.Colors[2], percent_instance), FLIP_RGB);
					verteies[5].SetColor(Effekseer::Color::Lerp(param.Colors[1], param.Colors[3], percent_instance), FLIP_RGB);

					verteies[1].Pos = (verteies[0].Pos + verteies[5].Pos) / 2.0f;
					verteies[1].Col = Effekseer::Color::Lerp(verteies[0].Col, verteies[5].Col, 0.5f);
					verteies[4] = verteies[1];
				}
				else
				{
					{
						verteies[0].Pos.X = param.Positions[0];
						verteies[0].Pos.Y = 0.0f;
						verteies[0].Pos.Z = 0.0f;
						verteies[0].SetColor(param.Colors[0], FLIP_RGB);
						verteies[0].SetFlipbookIndexAndNextRate(param.FlipbookIndexAndNextRate);
						verteies[0].SetAlphaThreshold(param.AlphaThreshold);
					}

					{
						verteies[5].Pos.X = param.Positions[1];
						verteies[5].Pos.Y = 0.0f;
						verteies[5].Pos.Z = 0.0f;
						verteies[5].SetColor(param.Colors[1], FLIP_RGB);
						verteies[5].SetFlipbookIndexAndNextRate(param.FlipbookIndexAndNextRate);
						verteies[5].SetAlphaThreshold(param.AlphaThreshold);
					}

					verteies[1].Pos = (verteies[0].Pos + verteies[5].Pos) / 2.0f;
					verteies[1].Col = Effekseer::Color::Lerp(verteies[0].Col, verteies[5].Col, 0.5f);
					verteies[1].SetFlipbookIndexAndNextRate(param.FlipbookIndexAndNextRate);
					verteies[1].SetAlphaThreshold(param.AlphaThreshold);
					verteies[4] = verteies[1];
				}

				if (parameter.ViewpointDependent)
				{
					::Effekseer::SIMD::Mat43f mat = param.SRTMatrix43;

					if (parameter.EnableViewOffset)
					{
						ApplyViewOffset(mat, camera, param.ViewOffsetDistance);
					}

					::Effekseer::SIMD::Vec3f s;
					::Effekseer::SIMD::Mat43f r;
					::Effekseer::SIMD::Vec3f t;
					mat.GetSRT(s, r, t);

					ApplyDepthParameters(r,
										 t,
										 s,
										 renderer_->GetCameraFrontDirection(),
										 renderer_->GetCameraPosition(),
										 parameter.DepthParameterPtr,
										 parameter.IsRightHand);

					if (parameter.SplineDivision > 1)
					{
					}
					else
					{
						for (int i : {0, 1, 4, 5})
						{
							verteies[i].Pos.X = verteies[i].Pos.X * s.GetX();
						}

						::Effekseer::SIMD::Vec3f F;
						::Effekseer::SIMD::Vec3f R;
						::Effekseer::SIMD::Vec3f U;

						U = ::Effekseer::SIMD::Vec3f(r.X.GetY(), r.Y.GetY(), r.Z.GetY());

						F = ::Effekseer::SIMD::Vec3f(-renderer_->GetCameraFrontDirection()).GetNormal();
						R = ::Effekseer::SIMD::Vec3f::Cross(U, F).GetNormal();
						F = ::Effekseer::SIMD::Vec3f::Cross(R, U).GetNormal();

						::Effekseer::SIMD::Mat43f mat_rot(-R.GetX(),
														  -R.GetY(),
														  -R.GetZ(),
														  U.GetX(),
														  U.GetY(),
														  U.GetZ(),
														  F.GetX(),
														  F.GetY(),
														  F.GetZ(),
														  t.GetX(),
														  t.GetY(),
														  t.GetZ());

						for (int i : {0, 1, 4, 5})
						{
							verteies[i].Pos = ToStruct(::Effekseer::SIMD::Vec3f::Transform(verteies[i].Pos, mat_rot));
						}
					}
				}
				else
				{
					if (parameter.SplineDivision > 1)
					{
					}
					else
					{
						::Effekseer::SIMD::Mat43f mat = param.SRTMatrix43;

						if (parameter.EnableViewOffset == true)
						{
							ApplyViewOffset(mat, camera, param.ViewOffsetDistance);
						}

						ApplyDepthParameters(mat,
											 renderer_->GetCameraFrontDirection(),
											 renderer_->GetCameraPosition(),
											 // s,
											 parameter.DepthParameterPtr,
											 parameter.IsRightHand);

						for (int i : {0, 1, 4, 5})
						{
							verteies[i].Pos = ToStruct(::Effekseer::SIMD::Vec3f::Transform(verteies[i].Pos, mat));
						}
					}
				}

				for (int idx : {0, 1, 4, 5})
				{
					verteies[idx].SetParticleTime(param.ParticleTimes[0], param.ParticleTimes[1]);
				}

				if (isFirst || isLast)
				{
					verteies += 2;
				}
				else
				{
					verteies[6] = verteies[0];
					verteies[7] = verteies[1];
					verteies[10] = verteies[4];
					verteies[11] = verteies[5];
					verteies += 8;
				}

				if (!isFirst)
				{
					ribbonCount_++;
				}

				if (isLast)
				{
					break;
				}
			}
		}

		if (VertexNormalRequired<VERTEX>())
		{
			StrideView<VERTEX> vs_(ringBufferData_, stride_, vertexCount_);
			Effekseer::SIMD::Vec3f axisBefore{};

			for (size_t i = 0; i < (instances.size() - 1) * parameter.SplineDivision + 1; i++)
			{
				bool isFirst_ = (i == 0);
				bool isLast_ = (i == ((instances.size() - 1) * parameter.SplineDivision));

				Effekseer::SIMD::Vec3f axis;

				if (isFirst_)
				{
					axis = (vs_[3].Pos - vs_[1].Pos);
					axis = SafeNormalize(axis);
					axisBefore = axis;
				}
				else if (isLast_)
				{
					axis = axisBefore;
				}
				else
				{
					Effekseer::SIMD::Vec3f axisOld = axisBefore;
					axis = (vs_[9].Pos - vs_[7].Pos);
					axis = SafeNormalize(axis);
					axisBefore = axis;

					axis = (axisBefore + axisOld) / 2.0f;
					axis = SafeNormalize(axis);
				}

				Effekseer::SIMD::Vec3f tangent = vs_[1].Pos - vs_[0].Pos;
				tangent = SafeNormalize(tangent);

				Effekseer::SIMD::Vec3f normal = Effekseer::SIMD::Vec3f::Cross(axis, tangent);
				normal = SafeNormalize(normal);

				if (!parameter.IsRightHand)
				{
					normal = -normal;
				}

				if (isFirst_)
				{
					const auto packedNormal = PackVector3DF(normal);
					const auto packedTangent = PackVector3DF(tangent);
					vs_[0].SetPackedNormal(packedNormal, FLIP_RGB);
					vs_[0].SetPackedTangent(packedTangent, FLIP_RGB);
					vs_[1].SetPackedNormal(packedNormal, FLIP_RGB);
					vs_[1].SetPackedTangent(packedTangent, FLIP_RGB);
					vs_[4].SetPackedNormal(packedNormal, FLIP_RGB);
					vs_[4].SetPackedTangent(packedTangent, FLIP_RGB);
					vs_[5].SetPackedNormal(packedNormal, FLIP_RGB);
					vs_[5].SetPackedTangent(packedTangent, FLIP_RGB);

					vs_ += 2;
				}
				else if (isLast_)
				{
					const auto packedNormal = PackVector3DF(normal);
					const auto packedTangent = PackVector3DF(tangent);
					vs_[0].SetPackedNormal(packedNormal, FLIP_RGB);
					vs_[0].SetPackedTangent(packedTangent, FLIP_RGB);
					vs_[1].SetPackedNormal(packedNormal, FLIP_RGB);
					vs_[1].SetPackedTangent(packedTangent, FLIP_RGB);
					vs_[4].SetPackedNormal(packedNormal, FLIP_RGB);
					vs_[4].SetPackedTangent(packedTangent, FLIP_RGB);
					vs_[5].SetPackedNormal(packedNormal, FLIP_RGB);
					vs_[5].SetPackedTangent(packedTangent, FLIP_RGB);

					vs_ += 2;
				}
				else
				{
					const auto packedNormal = PackVector3DF(normal);
					const auto packedTangent = PackVector3DF(tangent);
					for (int offset : {0, 6})
					{
						vs_[0 + offset].SetPackedNormal(packedNormal, FLIP_RGB);
						vs_[0 + offset].SetPackedTangent(packedTangent, FLIP_RGB);
						vs_[1 + offset].SetPackedNormal(packedNormal, FLIP_RGB);
						vs_[1 + offset].SetPackedTangent(packedTangent, FLIP_RGB);
						vs_[4 + offset].SetPackedNormal(packedNormal, FLIP_RGB);
						vs_[4 + offset].SetPackedTangent(packedTangent, FLIP_RGB);
						vs_[5 + offset].SetPackedNormal(packedNormal, FLIP_RGB);
						vs_[5 + offset].SetPackedTangent(packedTangent, FLIP_RGB);
					}
					vs_ += 8;
				}
			}
		}

		// calculate UV
		const auto global_scale = parameter.GlobalScale * parameter.Maginification;

		TrailRendererUtils::AssignUVs<VERTEX, efkRibbonInstanceParam, 0>(*parameter.TextureUVTypeParameterPtr, instances, verteies, parameter.SplineDivision, global_scale);

		if (VertexUV2Required<VERTEX>())
		{
			TrailRendererUtils::AssignUVs<VERTEX, efkRibbonInstanceParam, 1>(*parameter.TextureUVTypeParameterPtr, instances, verteies, parameter.SplineDivision, global_scale);
		}

		TrailRendererUtils::AssignUVs<VERTEX, efkRibbonInstanceParam, 2>(*parameter.TextureUVTypeParameterPtr, instances, verteies, parameter.SplineDivision, global_scale);
		TrailRendererUtils::AssignUVs<VERTEX, efkRibbonInstanceParam, 3>(*parameter.TextureUVTypeParameterPtr, instances, verteies, parameter.SplineDivision, global_scale);
		TrailRendererUtils::AssignUVs<VERTEX, efkRibbonInstanceParam, 4>(*parameter.TextureUVTypeParameterPtr, instances, verteies, parameter.SplineDivision, global_scale);
		TrailRendererUtils::AssignUVs<VERTEX, efkRibbonInstanceParam, 5>(*parameter.TextureUVTypeParameterPtr, instances, verteies, parameter.SplineDivision, global_scale);
		TrailRendererUtils::AssignUVs<VERTEX, efkRibbonInstanceParam, 6>(*parameter.TextureUVTypeParameterPtr, instances, verteies, parameter.SplineDivision, global_scale);

		// custom parameter
		if (customData1Count_ > 0)
		{
			StrideView<float> custom(ringBufferData_ + sizeof(DynamicVertex), stride_, vertexCount_);
			for (size_t loop = 0; loop < instances.size() - 1; loop++)
			{
				auto& param = instances[loop];

				for (int32_t sploop = 0; sploop < parameter.SplineDivision; sploop++)
				{
					for (size_t i = 0; i < 8; i++)
					{
						auto c = (float*)(&custom[0]);
						memcpy(c, param.CustomData1.data(), sizeof(float) * customData1Count_);
						custom += 1;
					}
				}
			}
		}

		if (customData2Count_ > 0)
		{
			StrideView<float> custom(ringBufferData_ + sizeof(DynamicVertex) + sizeof(float) * customData1Count_, stride_, vertexCount_);
			for (size_t loop = 0; loop < instances.size() - 1; loop++)
			{
				auto& param = instances[loop];

				for (int32_t sploop = 0; sploop < parameter.SplineDivision; sploop++)
				{
					for (size_t i = 0; i < 8; i++)
					{
						auto c = (float*)(&custom[0]);
						memcpy(c, param.CustomData2.data(), sizeof(float) * customData2Count_);
						custom += 1;
					}
				}
			}
		}
	}

public:
	RibbonRendererBase(RENDERER* renderer)
		: renderer_(renderer)
		, ribbonCount_(0)
		, ringBufferOffset_(0)
		, ringBufferData_(nullptr)
	{
	}

	virtual ~RibbonRendererBase()
	{
	}

protected:
	void Rendering_(const efkRibbonNodeParam& parameter,
					const efkRibbonInstanceParam& instanceParameter,
					const ::Effekseer::SIMD::Mat44f& camera)
	{
		if (ringBufferData_ == nullptr)
			return;
		if (instanceParameter.InstanceCount <= 1)
			return;

		const auto& state = renderer_->GetStandardRenderer()->GetState();
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

	template <typename VERTEX, bool FLIP_RGB>
	void Rendering_Internal(const efkRibbonNodeParam& parameter,
							const efkRibbonInstanceParam& instanceParameter,
							const ::Effekseer::SIMD::Mat44f& camera)
	{
		if (ringBufferData_ == nullptr)
			return;
		if (instanceParameter.InstanceCount < 2)
			return;

		bool isFirst = instanceParameter.InstanceIndex == 0;
		bool isLast = instanceParameter.InstanceIndex == (instanceParameter.InstanceCount - 1);

		auto& param = instanceParameter;

		if (isFirst)
		{
			instances.reserve(param.InstanceCount);
			instances.resize(0);
		}

		instances.push_back(param);

		if (isLast)
		{
			RenderSplines<VERTEX, FLIP_RGB>(parameter, camera);
		}
	}

public:
	void BeginRenderingGroup(const efkRibbonNodeParam& param, int32_t count, void* userData) override
	{
		ribbonCount_ = 0;
		int32_t vertexCount = ((count - 1) * param.SplineDivision) * 8;
		if (vertexCount <= 0)
			return;

		EffekseerRenderer::StandardRendererState state;
		state.CullingType = ::Effekseer::CullingType::Double;
		state.DepthTest = param.ZTest;
		state.DepthWrite = param.ZWrite;
		/*
		state.TextureFilter1 = param.BasicParameterPtr->TextureFilter1;
		state.TextureWrap1 = param.BasicParameterPtr->TextureWrap1;
		state.TextureFilter2 = param.BasicParameterPtr->TextureFilter2;
		state.TextureWrap2 = param.BasicParameterPtr->TextureWrap2;
		state.TextureFilter3 = param.BasicParameterPtr->TextureFilter3;
		state.TextureWrap3 = param.BasicParameterPtr->TextureWrap3;
		state.TextureFilter4 = param.BasicParameterPtr->TextureFilter4;
		state.TextureWrap4 = param.BasicParameterPtr->TextureWrap4;
		state.TextureFilter5 = param.BasicParameterPtr->TextureFilter5;
		state.TextureWrap5 = param.BasicParameterPtr->TextureWrap5;
		state.TextureFilter6 = param.BasicParameterPtr->TextureFilter6;
		state.TextureWrap6 = param.BasicParameterPtr->TextureWrap6;
		state.TextureFilter7 = param.BasicParameterPtr->TextureFilter7;
		state.TextureWrap7 = param.BasicParameterPtr->TextureWrap7;
		*/

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

		renderer_->GetStandardRenderer()->BeginRenderingAndRenderingIfRequired(state, vertexCount, stride_, (void*&)ringBufferData_);
		vertexCount_ = vertexCount;
	}

	void Rendering(const efkRibbonNodeParam& parameter, const efkRibbonInstanceParam& instanceParameter, void* userData) override
	{
		Rendering_(parameter, instanceParameter, renderer_->GetCameraMatrix());
	}

	void EndRendering(const efkRibbonNodeParam& parameter, void* userData) override
	{
		renderer_->GetStandardRenderer()->EndRenderingAndRenderingIfRequired();
	}
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRenderer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_RIBBON_RENDERER_H__
