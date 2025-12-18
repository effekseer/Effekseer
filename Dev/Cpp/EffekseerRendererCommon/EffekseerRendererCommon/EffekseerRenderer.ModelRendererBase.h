
#ifndef __EFFEKSEERRENDERER_MODEL_RENDERER_BASE_H__
#define __EFFEKSEERRENDERER_MODEL_RENDERER_BASE_H__

#include <Effekseer.h>
#include <algorithm>
#include <assert.h>
#include <string.h>
#include <vector>

#include "EffekseerRenderer.CommonUtils.h"
#include "EffekseerRenderer.RenderStateBase.h"
#include "EffekseerRenderer.Renderer.h"

namespace EffekseerRenderer
{

typedef ::Effekseer::ModelRenderer::NodeParameter efkModelNodeParam;
typedef ::Effekseer::ModelRenderer::InstanceParameter efkModelInstanceParam;
typedef ::Effekseer::SIMD::Vec3f efkVector3D;

template <int MODEL_COUNT>
struct ModelRendererVertexConstantBuffer
{
	Effekseer::Matrix44 CameraMatrix;
	Effekseer::Matrix44 ModelMatrix[MODEL_COUNT];
	float ModelUV[MODEL_COUNT][4];

	void SetModelFlipbookParameter(const Effekseer::NodeRendererFlipbookParameter& param)
	{
	}

	void SetModelAlphaUV(int32_t index, float x, float y, float w, float h)
	{
	}

	void SetModelUVDistortionUV(int32_t index, float x, float y, float w, float h)
	{
	}

	void SetModelBlendUV(int32_t index, float x, float y, float w, float h)
	{
	}

	void SetModelBlendAlphaUV(int32_t index, float x, float y, float w, float h)
	{
	}

	void SetModelBlendUVDistortionUV(int32_t index, float x, float y, float w, float h)
	{
	}

	void SetModelFlipbookIndexAndNextRate(int32_t index, float value)
	{
	}

	void SetModelAlphaThreshold(int32_t index, float value)
	{
	}

	void SetModelParticleTime(int32_t index, float normalized, float seconds)
	{
	}

	float ModelColor[MODEL_COUNT][4];

	float LightDirection[4];
	float LightColor[4];
	float LightAmbientColor[4];
	float UVInversed[4];
};

template <int MODEL_COUNT>
struct ModelRendererAdvancedVertexConstantBuffer
{
	Effekseer::Matrix44 CameraMatrix;
	Effekseer::Matrix44 ModelMatrix[MODEL_COUNT];
	float ModelUV[MODEL_COUNT][4];

	float ModelAlphaUV[MODEL_COUNT][4];

	float ModelUVDistortionUV[MODEL_COUNT][4];

	float ModelBlendUV[MODEL_COUNT][4];

	float ModelBlendAlphaUV[MODEL_COUNT][4];

	float ModelBlendUVDistortionUV[MODEL_COUNT][4];

	FlipbookVertexBuffer ModelFlipbookParameter;

	float ModelFlipbookIndexAndNextRate[MODEL_COUNT][4];

	float ModelAlphaThreshold[MODEL_COUNT][4];

	void SetModelFlipbookParameter(const Effekseer::NodeRendererFlipbookParameter& param)
	{
		const auto state = ToState(param);
		ModelFlipbookParameter = ToVertexBuffer(state);
	}

	void SetModelAlphaUV(int32_t index, float x, float y, float w, float h)
	{
		ModelAlphaUV[index][0] = x;
		ModelAlphaUV[index][1] = y;
		ModelAlphaUV[index][2] = w;
		ModelAlphaUV[index][3] = h;
	}

	void SetModelUVDistortionUV(int32_t index, float x, float y, float w, float h)
	{
		ModelUVDistortionUV[index][0] = x;
		ModelUVDistortionUV[index][1] = y;
		ModelUVDistortionUV[index][2] = w;
		ModelUVDistortionUV[index][3] = h;
	}

	void SetModelBlendUV(int32_t index, float x, float y, float w, float h)
	{
		ModelBlendUV[index][0] = x;
		ModelBlendUV[index][1] = y;
		ModelBlendUV[index][2] = w;
		ModelBlendUV[index][3] = h;
	}

	void SetModelBlendAlphaUV(int32_t index, float x, float y, float w, float h)
	{
		ModelBlendAlphaUV[index][0] = x;
		ModelBlendAlphaUV[index][1] = y;
		ModelBlendAlphaUV[index][2] = w;
		ModelBlendAlphaUV[index][3] = h;
	}

	void SetModelBlendUVDistortionUV(int32_t index, float x, float y, float w, float h)
	{
		ModelBlendUVDistortionUV[index][0] = x;
		ModelBlendUVDistortionUV[index][1] = y;
		ModelBlendUVDistortionUV[index][2] = w;
		ModelBlendUVDistortionUV[index][3] = h;
	}

	void SetModelFlipbookIndexAndNextRate(int32_t index, float value)
	{
		ModelFlipbookIndexAndNextRate[index][0] = value;
	}

	void SetModelAlphaThreshold(int32_t index, float value)
	{
		ModelAlphaThreshold[index][0] = value;
	}

	void SetModelParticleTime(int32_t index, float normalized, float seconds)
	{
	}

	float ModelColor[MODEL_COUNT][4];

	float LightDirection[4];
	float LightColor[4];
	float LightAmbientColor[4];
	float UVInversed[4];
};

template <int MODEL_COUNT>
struct ModelRendererMaterialVertexConstantBuffer
{
	Effekseer::Matrix44 CameraMatrix;
	Effekseer::Matrix44 ModelMatrix[MODEL_COUNT];
	float ModelUV[MODEL_COUNT][4];
	float ModelColor[MODEL_COUNT][4];
	float ModelParticleTime[MODEL_COUNT][4];

	float LightDirection[4];
	float LightColor[4];
	float LightAmbientColor[4];
	float UVInversed[4];

	void SetModelFlipbookParameter(const Effekseer::NodeRendererFlipbookParameter& param)
	{
	}

	void SetModelAlphaUV(int32_t index, float x, float y, float w, float h)
	{
	}

	void SetModelUVDistortionUV(int32_t index, float x, float y, float w, float h)
	{
	}

	void SetModelBlendUV(int32_t iondex, float x, float y, float w, float h)
	{
	}

	void SetModelBlendAlphaUV(int32_t iondex, float x, float y, float w, float h)
	{
	}

	void SetModelBlendUVDistortionUV(int32_t iondex, float x, float y, float w, float h)
	{
	}

	void SetModelFlipbookIndexAndNextRate(int32_t index, float value)
	{
	}

	void SetModelAlphaThreshold(int32_t index, float value)
	{
	}

	void SetModelParticleTime(int32_t index, float normalized, float seconds)
	{
		ModelParticleTime[index][0] = normalized;
		ModelParticleTime[index][1] = seconds;
		ModelParticleTime[index][2] = 0.0f;
		ModelParticleTime[index][3] = 0.0f;
	}
};

enum class ModelRendererVertexType
{
	Instancing,
	Single,
};

class ModelRendererBase : public ::Effekseer::ModelRenderer, public ::Effekseer::SIMD::AlignedAllocationPolicy<16>
{
protected:
	struct KeyValue
	{
		float Key;
		int Value;
	};

	std::vector<KeyValue> keyValues_;

	std::vector<Effekseer::Matrix44> matrixesSorted_;
	std::vector<Effekseer::RectF> uvSorted_;
	std::vector<Effekseer::RectF> alphaUVSorted_;
	std::vector<Effekseer::RectF> uvDistortionUVSorted_;
	std::vector<Effekseer::RectF> blendUVSorted_;
	std::vector<Effekseer::RectF> blendAlphaUVSorted_;
	std::vector<Effekseer::RectF> blendUVDistortionUVSorted_;
	std::vector<float> flipbookIndexAndNextRateSorted_;
	std::vector<float> alphaThresholdSorted_;
	std::vector<float> viewOffsetDistanceSorted_;

	std::vector<Effekseer::Color> colorsSorted_;
	std::vector<int32_t> timesSorted_;
	std::vector<std::array<float, 2>> particleTimesSorted_;
	std::vector<std::array<float, 4>> customData1Sorted_;
	std::vector<std::array<float, 4>> customData2Sorted_;

	std::vector<Effekseer::Matrix44> matrixes_;
	std::vector<Effekseer::RectF> uv_;

	std::vector<Effekseer::RectF> alphaUv_;
	std::vector<Effekseer::RectF> uvDistortionUv_;
	std::vector<Effekseer::RectF> blendUv_;
	std::vector<Effekseer::RectF> blendAlphaUv_;
	std::vector<Effekseer::RectF> blendUvDistortionUv_;
	std::vector<float> flipbookIndexAndNextRate_;
	std::vector<float> alphaThreshold_;
	std::vector<float> viewOffsetDistance_;

	std::vector<Effekseer::Color> colors_;
	std::vector<int32_t> times_;
	std::vector<std::array<float, 2>> particleTimes_;
	std::vector<std::array<float, 4>> customData1_;
	std::vector<std::array<float, 4>> customData2_;

	int32_t customData1Count_ = 0;
	int32_t customData2Count_ = 0;

	ShaderParameterCollector collector_;

	void ColorToFloat4(::Effekseer::Color color, float fc[4])
	{
		fc[0] = color.R / 255.0f;
		fc[1] = color.G / 255.0f;
		fc[2] = color.B / 255.0f;
		fc[3] = color.A / 255.0f;
	}

	std::array<float, 4> ColorToFloat4(::Effekseer::Color color)
	{
		std::array<float, 4> fc;
		fc[0] = color.R / 255.0f;
		fc[1] = color.G / 255.0f;
		fc[2] = color.B / 255.0f;
		fc[3] = color.A / 255.0f;
		return fc;
	}

	void VectorToFloat4(const ::Effekseer::SIMD::Vec3f& v, float fc[4])
	{
		::Effekseer::SIMD::Float4::Store3(fc, v.s);
		fc[3] = 1.0f;
	}

	void VectorToFloat4(const ::Effekseer::SIMD::Vec3f& v, std::array<float, 4>& fc)
	{
		::Effekseer::SIMD::Float4::Store3(fc.data(), v.s);
		fc[3] = 1.0f;
	}

	ModelRendererBase()
	{
	}

	template <typename RENDERER>
	void GetInversedFlags(RENDERER* renderer, std::array<float, 4>& uvInversed, std::array<float, 4>& uvInversedBack)
	{
		if (renderer->GetTextureUVStyle() == UVStyle::VerticalFlipped)
		{
			uvInversed[0] = 1.0f;
			uvInversed[1] = -1.0f;
		}
		else
		{
			uvInversed[0] = 0.0f;
			uvInversed[1] = 1.0f;
		}

		if (renderer->GetBackgroundTextureUVStyle() == UVStyle::VerticalFlipped)
		{
			uvInversedBack[0] = 1.0f;
			uvInversedBack[1] = -1.0f;
		}
		else
		{
			uvInversedBack[0] = 0.0f;
			uvInversedBack[1] = 1.0f;
		}
	}

	template <typename RENDERER>
	void SortTemporaryValues(RENDERER* renderer, const efkModelNodeParam& param)
	{
		if (param.DepthParameterPtr->ZSort != Effekseer::ZSortType::None)
		{
			keyValues_.resize(matrixes_.size());
			for (size_t i = 0; i < keyValues_.size(); i++)
			{
				efkVector3D t(matrixes_[i].Values[3][0], matrixes_[i].Values[3][1], matrixes_[i].Values[3][2]);

				auto frontDirection = renderer->GetCameraFrontDirection();
				if (!param.IsRightHand)
				{
					frontDirection = -frontDirection;
				}

				keyValues_[i].Key = Effekseer::SIMD::Vec3f::Dot(t, frontDirection);
				keyValues_[i].Value = static_cast<int32_t>(i);
			}

			if (param.DepthParameterPtr->ZSort == Effekseer::ZSortType::NormalOrder)
			{
				std::sort(keyValues_.begin(), keyValues_.end(), [](const KeyValue& a, const KeyValue& b) -> bool
						  { return a.Key < b.Key; });
			}
			else
			{
				std::sort(keyValues_.begin(), keyValues_.end(), [](const KeyValue& a, const KeyValue& b) -> bool
						  { return a.Key > b.Key; });
			}

			matrixesSorted_.resize(matrixes_.size());
			uvSorted_.resize(matrixes_.size());
			alphaUVSorted_.resize(matrixes_.size());
			uvDistortionUVSorted_.resize(matrixes_.size());
			blendUVSorted_.resize(matrixes_.size());
			blendAlphaUVSorted_.resize(matrixes_.size());
			blendUVDistortionUVSorted_.resize(matrixes_.size());
			flipbookIndexAndNextRateSorted_.resize(matrixes_.size());
			alphaThresholdSorted_.resize(matrixes_.size());
			viewOffsetDistanceSorted_.resize(matrixes_.size());
			colorsSorted_.resize(matrixes_.size());
			timesSorted_.resize(matrixes_.size());
			particleTimesSorted_.resize(matrixes_.size());

			if (customData1Count_ > 0)
			{
				customData1Sorted_.resize(matrixes_.size());
			}

			if (customData2Count_ > 0)
			{
				customData2Sorted_.resize(matrixes_.size());
			}

			for (size_t i = 0; i < keyValues_.size(); i++)
			{
				matrixesSorted_[keyValues_[i].Value] = matrixes_[i];
				uvSorted_[keyValues_[i].Value] = uv_[i];
				alphaUVSorted_[keyValues_[i].Value] = alphaUv_[i];
				uvDistortionUVSorted_[keyValues_[i].Value] = uvDistortionUv_[i];
				blendUVSorted_[keyValues_[i].Value] = blendUv_[i];
				blendAlphaUVSorted_[keyValues_[i].Value] = blendAlphaUv_[i];
				blendUVDistortionUVSorted_[keyValues_[i].Value] = blendUvDistortionUv_[i];
				flipbookIndexAndNextRateSorted_[keyValues_[i].Value] = flipbookIndexAndNextRate_[i];
				alphaThresholdSorted_[keyValues_[i].Value] = alphaThreshold_[i];
				viewOffsetDistanceSorted_[keyValues_[i].Value] = viewOffsetDistance_[i];
				colorsSorted_[keyValues_[i].Value] = colors_[i];
				timesSorted_[keyValues_[i].Value] = times_[i];
				particleTimesSorted_[keyValues_[i].Value] = particleTimes_[i];
			}

			if (customData1Count_ > 0)
			{
				for (size_t i = 0; i < keyValues_.size(); i++)
				{
					customData1Sorted_[keyValues_[i].Value] = customData1_[i];
				}
			}

			if (customData2Count_ > 0)
			{
				for (size_t i = 0; i < keyValues_.size(); i++)
				{
					customData2Sorted_[keyValues_[i].Value] = customData2_[i];
				}
			}

			matrixes_ = matrixesSorted_;
			uv_ = uvSorted_;
			alphaUv_ = alphaUVSorted_;
			uvDistortionUv_ = uvDistortionUVSorted_;
			blendUv_ = blendUVSorted_;
			blendAlphaUv_ = blendAlphaUVSorted_;
			blendUvDistortionUv_ = blendUVDistortionUVSorted_;
			flipbookIndexAndNextRate_ = flipbookIndexAndNextRateSorted_;
			alphaThreshold_ = alphaThresholdSorted_;
			viewOffsetDistance_ = viewOffsetDistanceSorted_;
			colors_ = colorsSorted_;
			times_ = timesSorted_;
			particleTimes_ = particleTimesSorted_;
			customData1_ = customData1Sorted_;
			customData2_ = customData2Sorted_;
		}
	}

	template <typename RENDERER, typename SHADER, int InstanceCount>
	void StoreFileUniform(RENDERER* renderer,
						  SHADER* shader_,
						  Effekseer::MaterialRef material,
						  Effekseer::MaterialRenderData* materialRenderData,
						  const efkModelNodeParam& param,
						  int32_t renderPassInd,
						  float*& cutomData1Ptr,
						  float*& cutomData2Ptr)
	{
		std::array<float, 4> uvInversed;
		std::array<float, 4> uvInversedBack;
		cutomData1Ptr = nullptr;
		cutomData2Ptr = nullptr;

		const auto materialGradientCount = static_cast<int32_t>(Effekseer::Min(materialRenderData->MaterialGradients.size(), Effekseer::UserGradientSlotMax));

		GetInversedFlags(renderer, uvInversed, uvInversedBack);

		std::array<float, 4> uvInversedMaterial;
		uvInversedMaterial[0] = uvInversed[0];
		uvInversedMaterial[1] = uvInversed[1];
		uvInversedMaterial[2] = uvInversedBack[0];
		uvInversedMaterial[3] = uvInversedBack[1];

		// camera
		float cameraPosition[4];
		::Effekseer::SIMD::Vec3f cameraPosition3 = renderer->GetCameraPosition();
		VectorToFloat4(cameraPosition3, cameraPosition);

		// time
		std::array<float, 4> predefined_uniforms;
		predefined_uniforms.fill(0.5f);
		predefined_uniforms[0] = renderer->GetTime();
		predefined_uniforms[1] = param.Magnification;
		predefined_uniforms[2] = renderer->GetImpl()->MaintainGammaColorInLinearColorSpace ? 1.0f : 0.0f;
		predefined_uniforms[3] = param.LocalTime;

		// vs
		int32_t vsOffset = sizeof(Effekseer::Matrix44) + (sizeof(Effekseer::Matrix44) + sizeof(float) * 4 * 3) * InstanceCount;

		renderer->SetVertexBufferToShader(uvInversedMaterial.data(), sizeof(float) * 4, vsOffset);
		vsOffset += (sizeof(float) * 4);

		renderer->SetVertexBufferToShader(predefined_uniforms.data(), sizeof(float) * 4, vsOffset);
		vsOffset += (sizeof(float) * 4);

		renderer->SetVertexBufferToShader(cameraPosition, sizeof(float) * 4, vsOffset);
		vsOffset += (sizeof(float) * 4);

		// vs - custom data
		if (customData1Count_ > 0)
		{
			cutomData1Ptr = (float*)((uint8_t*)shader_->GetVertexConstantBuffer() + vsOffset);
			vsOffset += (sizeof(float) * 4) * InstanceCount;
		}

		if (customData2Count_ > 0)
		{
			cutomData2Ptr = (float*)((uint8_t*)shader_->GetVertexConstantBuffer() + vsOffset);
			vsOffset += (sizeof(float) * 4) * InstanceCount;
		}

		for (size_t i = 0; i < materialRenderData->MaterialUniforms.size(); i++)
		{
			renderer->SetVertexBufferToShader(materialRenderData->MaterialUniforms[i].data(), sizeof(float) * 4, vsOffset);
			vsOffset += (sizeof(float) * 4);
		}

		for (size_t i = 0; i < materialGradientCount; i++)
		{
			auto data = ToUniform(*materialRenderData->MaterialGradients[i]);
			renderer->SetVertexBufferToShader(data.data(), sizeof(float) * 4 * 13, vsOffset);
			vsOffset += (sizeof(float) * 4) * 13;
		}

		// ps
		int32_t psOffset = 0;
		renderer->SetPixelBufferToShader(uvInversedMaterial.data(), sizeof(float) * 4, psOffset);
		psOffset += (sizeof(float) * 4);

		renderer->SetPixelBufferToShader(predefined_uniforms.data(), sizeof(float) * 4, psOffset);
		psOffset += (sizeof(float) * 4);

		renderer->SetPixelBufferToShader(cameraPosition, sizeof(float) * 4, psOffset);
		psOffset += (sizeof(float) * 4);

		::Effekseer::Backend::TextureRef depthTexture = nullptr;
		::EffekseerRenderer::DepthReconstructionParameter reconstructionParam;
		renderer->GetImpl()->GetDepth(depthTexture, reconstructionParam);

		SoftParticleParameter softParticleParam;

		softParticleParam.SetParam(
			0.0f,
			0.0f,
			0.0f,
			param.Maginification,
			reconstructionParam.DepthBufferScale,
			reconstructionParam.DepthBufferOffset,
			reconstructionParam.ProjectionMatrix33,
			reconstructionParam.ProjectionMatrix34,
			reconstructionParam.ProjectionMatrix43,
			reconstructionParam.ProjectionMatrix44);

		renderer->SetPixelBufferToShader(softParticleParam.reconstructionParam1.data(), sizeof(float) * 4, psOffset);
		psOffset += (sizeof(float) * 4);

		renderer->SetPixelBufferToShader(softParticleParam.reconstructionParam2.data(), sizeof(float) * 4, psOffset);
		psOffset += (sizeof(float) * 4);

		// shader model
		material = param.EffectPointer->GetMaterial(materialRenderData->MaterialIndex);

		float lightDirection[4];
		float lightColor[4];
		float lightAmbientColor[4];

		::Effekseer::SIMD::Vec3f lightDirection3 = renderer->GetLightDirection();
		lightDirection3 = lightDirection3.GetNormal();

		VectorToFloat4(lightDirection3, lightDirection);
		ColorToFloat4(renderer->GetLightColor(), lightColor);
		ColorToFloat4(renderer->GetLightAmbientColor(), lightAmbientColor);

		renderer->SetPixelBufferToShader(lightDirection, sizeof(float) * 4, psOffset);
		psOffset += (sizeof(float) * 4);

		renderer->SetPixelBufferToShader(lightColor, sizeof(float) * 4, psOffset);
		psOffset += (sizeof(float) * 4);

		renderer->SetPixelBufferToShader(lightAmbientColor, sizeof(float) * 4, psOffset);
		psOffset += (sizeof(float) * 4);

		// refraction
		if (material->RefractionModelUserPtr != nullptr && renderPassInd == 0)
		{
			auto mat = renderer->GetCameraMatrix();
			renderer->SetPixelBufferToShader(&mat, sizeof(float) * 16, psOffset);
			psOffset += (sizeof(float) * 16);
		}

		for (size_t i = 0; i < materialRenderData->MaterialUniforms.size(); i++)
		{
			renderer->SetPixelBufferToShader(materialRenderData->MaterialUniforms[i].data(), sizeof(float) * 4, psOffset);
			psOffset += (sizeof(float) * 4);
		}

		for (int32_t i = 0; i < materialGradientCount; i++)
		{
			auto data = ToUniform(*materialRenderData->MaterialGradients[i]);
			renderer->SetPixelBufferToShader(data.data(), sizeof(float) * 4 * 13, psOffset);
			psOffset += (sizeof(float) * 4) * 13;
		}
	}

	template <typename RENDERER, typename SHADER, int InstanceCount, typename VertexConstantBufferType, bool REQUIRE_ADVANCED_DATA, bool DISTORTION>
	void StoreFixedUniforms(RENDERER* renderer,
							SHADER* shader_,
							const efkModelNodeParam& param)
	{
		VertexConstantBufferType* vcb = (VertexConstantBufferType*)shader_->GetVertexConstantBuffer();
		std::array<float, 4> uvInversed;
		std::array<float, 4> uvInversedBack;

		GetInversedFlags(renderer, uvInversed, uvInversedBack);

		vcb->UVInversed[0] = uvInversed[0];
		vcb->UVInversed[1] = uvInversed[1];

		::Effekseer::Backend::TextureRef depthTexture = nullptr;
		::EffekseerRenderer::DepthReconstructionParameter reconstructionParam;
		renderer->GetImpl()->GetDepth(depthTexture, reconstructionParam);

		if (DISTORTION)
		{
			auto pcb = (PixelConstantBufferDistortion*)shader_->GetPixelConstantBuffer();
			pcb->DistortionIntencity[0] = param.BasicParameterPtr->DistortionIntensity;

			pcb->UVInversedBack[0] = uvInversedBack[0];
			pcb->UVInversedBack[1] = uvInversedBack[1];

			pcb->FlipbookParam.EnableInterpolation = static_cast<float>(param.BasicParameterPtr->Flipbook.EnableInterpolation);
			pcb->FlipbookParam.InterpolationType = static_cast<float>(param.BasicParameterPtr->Flipbook.InterpolationType);

			pcb->UVDistortionParam.Intensity = param.BasicParameterPtr->UVDistortionIntensity;
			pcb->UVDistortionParam.BlendIntensity = param.BasicParameterPtr->BlendUVDistortionIntensity;
			pcb->UVDistortionParam.UVInversed[0] = uvInversed[0];
			pcb->UVDistortionParam.UVInversed[1] = uvInversed[1];

			pcb->BlendTextureParam.BlendType = static_cast<float>(param.BasicParameterPtr->TextureBlendType);

			pcb->SoftParticleParam.SetParam(
				param.BasicParameterPtr->SoftParticleDistanceFar,
				param.BasicParameterPtr->SoftParticleDistanceNear,
				param.BasicParameterPtr->SoftParticleDistanceNearOffset,
				param.Maginification,
				reconstructionParam.DepthBufferScale,
				reconstructionParam.DepthBufferOffset,
				reconstructionParam.ProjectionMatrix33,
				reconstructionParam.ProjectionMatrix34,
				reconstructionParam.ProjectionMatrix43,
				reconstructionParam.ProjectionMatrix44);
		}
		else
		{
			auto pcb = (PixelConstantBuffer*)shader_->GetPixelConstantBuffer();
			pcb->MiscFlags.fill(0.0f);
			pcb->MiscFlags[0] = renderer->GetImpl()->MaintainGammaColorInLinearColorSpace ? 1.0f : 0.0f;

			// specify predefined parameters
			if (param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::Lighting)
			{
				::Effekseer::SIMD::Vec3f lightDirection = renderer->GetLightDirection();
				lightDirection = lightDirection.GetNormal();
				VectorToFloat4(lightDirection, vcb->LightDirection);
				VectorToFloat4(lightDirection, pcb->LightDirection);
			}

			{
				ColorToFloat4(renderer->GetLightColor(), vcb->LightColor);
				pcb->LightColor = ColorToFloat4(renderer->GetLightColor());
			}

			{
				ColorToFloat4(renderer->GetLightAmbientColor(), vcb->LightAmbientColor);
				pcb->LightAmbientColor = ColorToFloat4(renderer->GetLightAmbientColor());
			}

			pcb->SetEmissiveScaling(static_cast<float>(param.BasicParameterPtr->EmissiveScaling));

			if (REQUIRE_ADVANCED_DATA)
			{
				pcb->SetModelFlipbookParameter(param.BasicParameterPtr->Flipbook.EnableInterpolation, static_cast<float>(param.BasicParameterPtr->Flipbook.InterpolationType));
				pcb->SetModelUVDistortionParameter(param.BasicParameterPtr->UVDistortionIntensity, param.BasicParameterPtr->BlendUVDistortionIntensity, {uvInversed[0], uvInversed[1]});
				pcb->SetModelBlendTextureParameter(static_cast<float>(param.BasicParameterPtr->TextureBlendType));

				::Effekseer::Vector3D CameraFront = renderer->GetCameraFrontDirection();

				if (!param.IsRightHand)
				{
					CameraFront = -CameraFront;
				}

				pcb->SetCameraFrontDirection(-CameraFront.X, -CameraFront.Y, -CameraFront.Z);
				pcb->SetFalloffParameter(
					static_cast<float>(param.EnableFalloff),
					static_cast<float>(param.FalloffParam.ColorBlendType),
					static_cast<float>(param.FalloffParam.Pow),
					ColorToFloat4(param.FalloffParam.BeginColor),
					ColorToFloat4(param.FalloffParam.EndColor));

				pcb->SetEdgeParameter(ColorToFloat4(Effekseer::Color(
										  param.BasicParameterPtr->EdgeColor[0],
										  param.BasicParameterPtr->EdgeColor[1],
										  param.BasicParameterPtr->EdgeColor[2],
										  param.BasicParameterPtr->EdgeColor[3])),
									  param.BasicParameterPtr->EdgeThreshold,
									  static_cast<float>(param.BasicParameterPtr->EdgeColorScaling));
			}

			pcb->SoftParticleParam.SetParam(
				param.BasicParameterPtr->SoftParticleDistanceFar,
				param.BasicParameterPtr->SoftParticleDistanceNear,
				param.BasicParameterPtr->SoftParticleDistanceNearOffset,
				param.Maginification,
				reconstructionParam.DepthBufferScale,
				reconstructionParam.DepthBufferOffset,
				reconstructionParam.ProjectionMatrix33,
				reconstructionParam.ProjectionMatrix34,
				reconstructionParam.ProjectionMatrix43,
				reconstructionParam.ProjectionMatrix44);

			pcb->UVInversedBack[0] = uvInversedBack[0];
			pcb->UVInversedBack[1] = uvInversedBack[1];
		}

		vcb->CameraMatrix = renderer->GetCameraProjectionMatrix();

		vcb->SetModelFlipbookParameter(param.BasicParameterPtr->Flipbook);
	}

public:
	ModelRendererVertexType VertexType = ModelRendererVertexType::Single;

	virtual ~ModelRendererBase()
	{
	}

	template <typename RENDERER>
	void BeginRendering_(RENDERER* renderer, const efkModelNodeParam& parameter, int32_t count, void* userData)
	{
		keyValues_.clear();

		matrixes_.clear();
		uv_.clear();
		alphaUv_.clear();
		uvDistortionUv_.clear();
		blendUv_.clear();
		blendAlphaUv_.clear();
		blendUvDistortionUv_.clear();
		flipbookIndexAndNextRate_.clear();
		alphaThreshold_.clear();
		viewOffsetDistance_.clear();
		colors_.clear();
		times_.clear();
		particleTimes_.clear();
		customData1_.clear();
		customData2_.clear();

		matrixesSorted_.clear();
		uvSorted_.clear();
		alphaUVSorted_.clear();
		uvDistortionUVSorted_.clear();
		blendUVSorted_.clear();
		blendAlphaUVSorted_.clear();
		blendUVDistortionUVSorted_.clear();
		flipbookIndexAndNextRateSorted_.clear();
		alphaThresholdSorted_.clear();
		viewOffsetDistanceSorted_.clear();
		colorsSorted_.clear();
		timesSorted_.clear();
		particleTimesSorted_.clear();
		customData1Sorted_.clear();
		customData2Sorted_.clear();

		if (parameter.BasicParameterPtr->MaterialType == ::Effekseer::RendererMaterialType::File &&
			parameter.BasicParameterPtr->MaterialRenderDataPtr != nullptr &&
			parameter.BasicParameterPtr->MaterialRenderDataPtr->MaterialIndex >= 0 &&
			parameter.EffectPointer->GetMaterial(parameter.BasicParameterPtr->MaterialRenderDataPtr->MaterialIndex) != nullptr)
		{
			auto material = parameter.EffectPointer->GetMaterial(parameter.BasicParameterPtr->MaterialRenderDataPtr->MaterialIndex);
			customData1Count_ = material->CustomData1;
			customData2Count_ = material->CustomData2;
		}
		else
		{
			customData1Count_ = 0;
			customData2Count_ = 0;
		}

		renderer->GetStandardRenderer()->ResetAndRenderingIfRequired();

		collector_ = ShaderParameterCollector();
		collector_.Collect(renderer, parameter.EffectPointer, parameter.BasicParameterPtr, parameter.EnableFalloff, renderer->GetImpl()->isSoftParticleEnabled);
	}

	template <typename RENDERER>
	void Rendering_(RENDERER* renderer, const efkModelNodeParam& parameter, const efkModelInstanceParam& instanceParameter, void* userData)
	{
		::Effekseer::BillboardType btype = parameter.Billboard;
		Effekseer::SIMD::Mat44f mat44;
		auto baseMatrix = instanceParameter.SRTMatrix43;

		if (parameter.IsExternalMode)
		{
			baseMatrix *= parameter.ExternalModelTransform;
		}

		if (btype == ::Effekseer::BillboardType::Fixed)
		{
			mat44 = baseMatrix;
		}
		else
		{
			Effekseer::SIMD::Mat43f mat43;
			Effekseer::SIMD::Vec3f s;
			Effekseer::SIMD::Vec3f R;
			Effekseer::SIMD::Vec3f F;

			CalcBillboard(btype, mat43, s, R, F, baseMatrix, renderer->GetCameraFrontDirection(), instanceParameter.Direction);

			mat44 = ::Effekseer::SIMD::Mat43f::Scaling(s) * mat43;
		}

		if (parameter.Magnification != 1.0f)
		{
			mat44 = Effekseer::SIMD::Mat44f::Scaling(::Effekseer::SIMD::Vec3f(parameter.Magnification)) * mat44;
		}

		if (!parameter.IsRightHand)
		{
			mat44 = Effekseer::SIMD::Mat44f::Scaling(1.0f, 1.0f, -1.0f) * mat44;
		}

		matrixes_.push_back(ToStruct(mat44));
		uv_.push_back(instanceParameter.UV);
		alphaUv_.push_back(instanceParameter.AlphaUV);
		uvDistortionUv_.push_back(instanceParameter.UVDistortionUV);
		blendUv_.push_back(instanceParameter.BlendUV);
		blendAlphaUv_.push_back(instanceParameter.BlendAlphaUV);
		blendUvDistortionUv_.push_back(instanceParameter.BlendUVDistortionUV);
		flipbookIndexAndNextRate_.push_back(instanceParameter.FlipbookIndexAndNextRate);
		alphaThreshold_.push_back(instanceParameter.AlphaThreshold);
		viewOffsetDistance_.push_back(instanceParameter.ViewOffsetDistance);
		colors_.push_back(instanceParameter.AllColor);
		times_.push_back(instanceParameter.Time);
		particleTimes_.push_back({instanceParameter.ParticleTimes[0], instanceParameter.ParticleTimes[1]});

		if (customData1Count_ > 0)
		{
			customData1_.push_back(instanceParameter.CustomData1);
		}

		if (customData2Count_ > 0)
		{
			customData2_.push_back(instanceParameter.CustomData2);
		}

		// parameter.BasicParameterPtr
	}

	template <typename RENDERER, typename SHADER, typename MODEL, bool Instancing, int InstanceCount>
	void EndRendering_(RENDERER* renderer,
					   SHADER* advanced_shader_lit,
					   SHADER* advanced_shader_unlit,
					   SHADER* advanced_shader_distortion,
					   SHADER* shader_lit,
					   SHADER* shader_unlit,
					   SHADER* shader_distortion,
					   const efkModelNodeParam& param,
					   void* userData)
	{
		if (matrixes_.size() == 0)
			return;
		if (!param.IsExternalMode && param.ModelIndex < 0)
			return;

		if (renderer->GetExternalShaderSettings() == nullptr)
		{
			shader_unlit->OverrideShader(nullptr);
		}
		else
		{
			shader_unlit->OverrideShader(renderer->GetExternalShaderSettings()->ModelShader);
		}

		int32_t renderPassCount = 1;

		if (param.BasicParameterPtr->MaterialRenderDataPtr != nullptr && param.BasicParameterPtr->MaterialRenderDataPtr->MaterialIndex >= 0)
		{
			auto material = param.EffectPointer->GetMaterial(param.BasicParameterPtr->MaterialRenderDataPtr->MaterialIndex);
			if (material != nullptr && material->IsRefractionRequired)
			{
				// refraction, standard
				renderPassCount = 2;
			}
		}

		// sort
		SortTemporaryValues(renderer, param);

		for (int32_t renderPassInd = 0; renderPassInd < renderPassCount; renderPassInd++)
		{
			Effekseer::MaterialRenderData* materialRenderData = param.BasicParameterPtr->MaterialRenderDataPtr;

			if (materialRenderData != nullptr && materialRenderData->MaterialIndex >= 0 &&
				param.EffectPointer->GetMaterial(materialRenderData->MaterialIndex) != nullptr)
			{
				RenderPass<RENDERER, SHADER, MODEL, Instancing, InstanceCount, ModelRendererMaterialVertexConstantBuffer<InstanceCount>, false>(
					renderer, advanced_shader_lit, advanced_shader_unlit, advanced_shader_distortion, shader_lit, shader_unlit, shader_distortion, param, renderPassInd, userData);
			}
			else
			{
				if (collector_.DoRequireAdvancedRenderer())
				{
					RenderPass<RENDERER, SHADER, MODEL, Instancing, InstanceCount, ModelRendererAdvancedVertexConstantBuffer<InstanceCount>, true>(
						renderer, advanced_shader_lit, advanced_shader_unlit, advanced_shader_distortion, shader_lit, shader_unlit, shader_distortion, param, renderPassInd, userData);
				}
				else
				{
					RenderPass<RENDERER, SHADER, MODEL, Instancing, InstanceCount, ModelRendererVertexConstantBuffer<InstanceCount>, false>(
						renderer, advanced_shader_lit, advanced_shader_unlit, advanced_shader_distortion, shader_lit, shader_unlit, shader_distortion, param, renderPassInd, userData);
				}
			}
		}
	}

	template <typename RENDERER, typename SHADER, typename MODEL, bool Instancing, int InstanceCount, typename VertexConstantBufferType, bool REQUIRE_ADVANCED_DATA>
	void RenderPass(RENDERER* renderer,
					SHADER* advanced_shader_lit,
					SHADER* advanced_shader_unlit,
					SHADER* advanced_shader_distortion,
					SHADER* shader_lit,
					SHADER* shader_unlit,
					SHADER* shader_distortion,
					const efkModelNodeParam& param,
					int32_t renderPassInd,
					void* userData)
	{
		if (matrixes_.size() == 0)
			return;

		if (!param.IsExternalMode && param.ModelIndex < 0)
			return;

		::Effekseer::RefPtr<MODEL> model;

		if (param.IsExternalMode)
		{
			model = param.ExternalModel;
		}
		else if (param.IsProceduralMode)
		{
			model = param.EffectPointer->GetProceduralModel(param.ModelIndex);
		}
		else
		{
			model = param.EffectPointer->GetModel(param.ModelIndex);
		}

		if (model == nullptr)
			return;

		auto isBackgroundRequired = collector_.IsBackgroundRequiredOnFirstPass && renderPassInd == 0;

		if (isBackgroundRequired)
		{
			auto callback = renderer->GetDistortingCallback();
			if (callback != nullptr)
			{
				if (!callback->OnDistorting(renderer))
				{
					return;
				}
			}
		}

		auto distortion = collector_.ShaderType == EffekseerRenderer::RendererShaderType::BackDistortion ||
						  collector_.ShaderType == EffekseerRenderer::RendererShaderType::AdvancedBackDistortion;

		if (isBackgroundRequired && renderer->GetBackground() == 0)
			return;

		if (isBackgroundRequired)
		{
			collector_.Textures[collector_.BackgroundIndex] = renderer->GetBackground();
		}

		::Effekseer::Backend::TextureRef depthTexture = nullptr;
		::EffekseerRenderer::DepthReconstructionParameter reconstructionParam;
		renderer->GetImpl()->GetDepth(depthTexture, reconstructionParam);

		if (collector_.IsDepthRequired)
		{
			if (depthTexture == nullptr || (param.BasicParameterPtr->SoftParticleDistanceFar == 0.0f &&
											param.BasicParameterPtr->SoftParticleDistanceNear == 0.0f &&
											param.BasicParameterPtr->SoftParticleDistanceNearOffset == 0.0f &&
											collector_.ShaderType != RendererShaderType::Material))
			{
				depthTexture = renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
			}

			collector_.Textures[collector_.DepthIndex] = depthTexture;
		}

		// select shader
		Effekseer::MaterialRenderData* materialRenderData = param.BasicParameterPtr->MaterialRenderDataPtr;
		// materialRenderData = nullptr;
		Effekseer::MaterialRef material = nullptr;
		SHADER* shader_ = nullptr;
		bool renderDistortedBackground = false;

		if (collector_.ShaderType == EffekseerRenderer::RendererShaderType::Material &&
			materialRenderData != nullptr && materialRenderData->MaterialIndex >= 0 &&
			param.EffectPointer->GetMaterial(materialRenderData->MaterialIndex) != nullptr)
		{
			material = param.EffectPointer->GetMaterial(materialRenderData->MaterialIndex);

			if (material != nullptr && material->IsRefractionRequired)
			{
				if (renderPassInd == 0)
				{
					shader_ = (SHADER*)material->RefractionModelUserPtr;
					renderDistortedBackground = true;
				}
				else
				{
					shader_ = (SHADER*)material->ModelUserPtr;
				}
			}
			else
			{
				shader_ = (SHADER*)material->ModelUserPtr;
			}
		}
		else
		{
			if (collector_.DoRequireAdvancedRenderer())
			{
				if (distortion)
				{
					shader_ = advanced_shader_distortion;
				}
				else if (collector_.ShaderType == EffekseerRenderer::RendererShaderType::AdvancedLit)
				{
					shader_ = advanced_shader_lit;
				}
				else
				{
					shader_ = advanced_shader_unlit;
				}
			}
			else
			{
				if (distortion)
				{
					shader_ = shader_distortion;
				}
				else if (collector_.ShaderType == EffekseerRenderer::RendererShaderType::Lit)
				{
					shader_ = shader_lit;
				}
				else
				{
					shader_ = shader_unlit;
				}
			}
		}
		// validate
		if (shader_ == nullptr)
		{
			return;
		}

		RenderStateBase::State& state = renderer->GetRenderState()->Push();
		state.DepthTest = param.ZTest;
		state.DepthWrite = param.ZWrite;
		state.AlphaBlend = param.BasicParameterPtr->AlphaBlend;
		state.CullingType = param.Culling;

		// TODO : refactor in 1.7
		if (renderer->GetExternalShaderSettings() != nullptr)
		{
			state.AlphaBlend = renderer->GetExternalShaderSettings()->Blend;
		}
		else if (renderer->GetRenderMode() == Effekseer::RenderMode::Wireframe)
		{
			state.AlphaBlend = ::Effekseer::AlphaBlendType::Opacity;
		}

		if (renderDistortedBackground)
		{
			state.AlphaBlend = ::Effekseer::AlphaBlendType::Blend;
		}

		renderer->BeginShader(shader_);

		for (int32_t i = 0; i < collector_.TextureCount; i++)
		{
			state.TextureFilterTypes[i] = collector_.TextureFilterTypes[i];
			state.TextureWrapTypes[i] = collector_.TextureWrapTypes[i];
		}

		renderer->SetTextures(shader_, collector_.Textures.data(), collector_.TextureCount);

		renderer->GetRenderState()->Update(distortion);

		VertexConstantBufferType* vcb = (VertexConstantBufferType*)shader_->GetVertexConstantBuffer();

		float* cutomData1Ptr = nullptr;
		float* cutomData2Ptr = nullptr;

		if (materialRenderData != nullptr && material != nullptr)
		{
			StoreFileUniform<RENDERER, SHADER, InstanceCount>(
				renderer, shader_, material, materialRenderData, param, renderPassInd, cutomData1Ptr, cutomData2Ptr);

			vcb->CameraMatrix = renderer->GetCameraProjectionMatrix();
		}
		else
		{
			if (distortion)
			{
				StoreFixedUniforms<RENDERER, SHADER, InstanceCount, VertexConstantBufferType, REQUIRE_ADVANCED_DATA, true>(renderer, shader_, param);
			}
			else
			{
				StoreFixedUniforms<RENDERER, SHADER, InstanceCount, VertexConstantBufferType, REQUIRE_ADVANCED_DATA, false>(renderer, shader_, param);
			}
		}

		renderer->GetImpl()->CurrentRenderingUserData = param.UserData;
		renderer->GetImpl()->CurrentHandleUserData = userData;

		// Check time
		auto stTime0 = times_[0] % model->GetFrameCount();
		auto isTimeSame = true;

		for (auto t : times_)
		{
			t = t % model->GetFrameCount();
			if (t != stTime0)
			{
				isTimeSame = false;
				break;
			}
		}

		if (Instancing && isTimeSame)
		{
			// auto& imodel = model->InternalModels[stTime0];

			// Invalid unless layout is set after buffer
			renderer->SetVertexBuffer(model->GetVertexBuffer(stTime0), sizeof(Effekseer::Model::Vertex));

			int32_t indexPerFace = 3;
			if (renderer->GetRenderMode() == Effekseer::RenderMode::Wireframe)
			{
				renderer->SetIndexBuffer(model->GetWireIndexBuffer(stTime0));
				indexPerFace = 6;
			}
			else
			{
				renderer->SetIndexBuffer(model->GetIndexBuffer(stTime0));
			}

			renderer->SetLayout(shader_);

			for (size_t loop = 0; loop < matrixes_.size();)
			{
				int32_t modelCount = Effekseer::Min(static_cast<int32_t>(matrixes_.size()) - (int32_t)loop, InstanceCount);

				for (int32_t num = 0; num < modelCount; num++)
				{
					vcb->ModelMatrix[num] = matrixes_[loop + num];

					// DepthParameter
					::Effekseer::SIMD::Mat44f modelMatrix = vcb->ModelMatrix[num];

					if (param.EnableViewOffset)
					{
						ApplyViewOffset(modelMatrix, renderer->GetCameraMatrix(), viewOffsetDistance_[loop + num]);
					}

					ApplyDepthParameters(modelMatrix,
										 renderer->GetCameraFrontDirection(),
										 renderer->GetCameraPosition(),
										 param.DepthParameterPtr,
										 param.IsRightHand);
					vcb->ModelMatrix[num] = ToStruct(modelMatrix);

					vcb->ModelUV[num][0] = uv_[loop + num].X;
					vcb->ModelUV[num][1] = uv_[loop + num].Y;
					vcb->ModelUV[num][2] = uv_[loop + num].Width;
					vcb->ModelUV[num][3] = uv_[loop + num].Height;

					vcb->SetModelAlphaUV(
						num, alphaUv_[loop + num].X, alphaUv_[loop + num].Y, alphaUv_[loop + num].Width, alphaUv_[loop + num].Height);
					vcb->SetModelUVDistortionUV(num,
												uvDistortionUv_[loop + num].X,
												uvDistortionUv_[loop + num].Y,
												uvDistortionUv_[loop + num].Width,
												uvDistortionUv_[loop + num].Height);
					vcb->SetModelBlendUV(
						num, blendUv_[loop + num].X, blendUv_[loop + num].Y, blendUv_[loop + num].Width, blendUv_[loop + num].Height);
					vcb->SetModelBlendAlphaUV(num,
											  blendAlphaUv_[loop + num].X,
											  blendAlphaUv_[loop + num].Y,
											  blendAlphaUv_[loop + num].Width,
											  blendAlphaUv_[loop + num].Height);
					vcb->SetModelBlendUVDistortionUV(num,
													 blendUvDistortionUv_[loop + num].X,
													 blendUvDistortionUv_[loop + num].Y,
													 blendUvDistortionUv_[loop + num].Width,
													 blendUvDistortionUv_[loop + num].Height);
					vcb->SetModelFlipbookIndexAndNextRate(num, flipbookIndexAndNextRate_[loop + num]);
					vcb->SetModelAlphaThreshold(num, alphaThreshold_[loop + num]);
					vcb->SetModelParticleTime(num, particleTimes_[loop + num][0], particleTimes_[loop + num][1]);

					ColorToFloat4(colors_[loop + num], vcb->ModelColor[num]);

					if (cutomData1Ptr != nullptr)
					{
						cutomData1Ptr[num * 4 + 0] = customData1_[loop + num][0];
						cutomData1Ptr[num * 4 + 1] = customData1_[loop + num][1];
						cutomData1Ptr[num * 4 + 2] = customData1_[loop + num][2];
						cutomData1Ptr[num * 4 + 3] = customData1_[loop + num][3];
					}

					if (cutomData2Ptr != nullptr)
					{
						cutomData2Ptr[num * 4 + 0] = customData2_[loop + num][0];
						cutomData2Ptr[num * 4 + 1] = customData2_[loop + num][1];
						cutomData2Ptr[num * 4 + 2] = customData2_[loop + num][2];
						cutomData2Ptr[num * 4 + 3] = customData2_[loop + num][3];
					}
				}

				shader_->SetConstantBuffer();

				if (VertexType == ModelRendererVertexType::Instancing)
				{
					renderer->DrawPolygonInstanced(model->GetVertexCount(stTime0), model->GetFaceCount(stTime0) * indexPerFace, modelCount);
				}
				else
				{
					assert(0);
				}

				loop += modelCount;
			}
		}
		else
		{
			for (size_t loop = 0; loop < matrixes_.size();)
			{
				auto stTime = times_[loop] % model->GetFrameCount();
				// auto& imodel = model->InternalModels[stTime];

				// Invalid unless layout is set after buffer
				renderer->SetVertexBuffer(model->GetVertexBuffer(stTime), sizeof(Effekseer::Model::Vertex));

				int32_t indexPerFace = 3;
				if (renderer->GetRenderMode() == Effekseer::RenderMode::Wireframe)
				{
					renderer->SetIndexBuffer(model->GetWireIndexBuffer(stTime));
					indexPerFace = 6;
				}
				else
				{
					renderer->SetIndexBuffer(model->GetIndexBuffer(stTime));
				}

				renderer->SetLayout(shader_);

				vcb->ModelMatrix[0] = matrixes_[loop];
				vcb->ModelUV[0][0] = uv_[loop].X;
				vcb->ModelUV[0][1] = uv_[loop].Y;
				vcb->ModelUV[0][2] = uv_[loop].Width;
				vcb->ModelUV[0][3] = uv_[loop].Height;
				vcb->SetModelParticleTime(0, particleTimes_[loop][0], particleTimes_[loop][1]);

				vcb->SetModelAlphaUV(0, alphaUv_[loop].X, alphaUv_[loop].Y, alphaUv_[loop].Width, alphaUv_[loop].Height);
				vcb->SetModelUVDistortionUV(
					0, uvDistortionUv_[loop].X, uvDistortionUv_[loop].Y, uvDistortionUv_[loop].Width, uvDistortionUv_[loop].Height);
				vcb->SetModelBlendUV(0, blendUv_[loop].X, blendUv_[loop].Y, blendUv_[loop].Width, blendUv_[loop].Height);
				vcb->SetModelBlendAlphaUV(
					0, blendAlphaUv_[loop].X, blendAlphaUv_[loop].Y, blendAlphaUv_[loop].Width, blendAlphaUv_[loop].Height);
				vcb->SetModelBlendUVDistortionUV(
					0, blendUvDistortionUv_[loop].X, blendUvDistortionUv_[loop].Y, blendUvDistortionUv_[loop].Width, blendUvDistortionUv_[loop].Height);
				vcb->SetModelFlipbookIndexAndNextRate(0, flipbookIndexAndNextRate_[loop]);
				vcb->SetModelAlphaThreshold(0, alphaThreshold_[loop]);

				// DepthParameters
				::Effekseer::SIMD::Mat44f modelMatrix = vcb->ModelMatrix[0];
				if (param.EnableViewOffset == true)
				{
					ApplyViewOffset(modelMatrix, renderer->GetCameraMatrix(), viewOffsetDistance_[0]);
				}

				ApplyDepthParameters(modelMatrix,
									 renderer->GetCameraFrontDirection(),
									 renderer->GetCameraPosition(),
									 param.DepthParameterPtr,
									 param.IsRightHand);
				vcb->ModelMatrix[0] = ToStruct(modelMatrix);
				ColorToFloat4(colors_[loop], vcb->ModelColor[0]);

				if (cutomData1Ptr != nullptr)
				{
					cutomData1Ptr[0] = customData1_[loop][0];
					cutomData1Ptr[1] = customData1_[loop][1];
					cutomData1Ptr[2] = customData1_[loop][2];
					cutomData1Ptr[3] = customData1_[loop][3];
				}

				if (cutomData2Ptr != nullptr)
				{
					cutomData2Ptr[0] = customData2_[loop][0];
					cutomData2Ptr[1] = customData2_[loop][1];
					cutomData2Ptr[2] = customData2_[loop][2];
					cutomData2Ptr[3] = customData2_[loop][3];
				}

				shader_->SetConstantBuffer();
				renderer->DrawPolygon(model->GetVertexCount(stTime), model->GetFaceCount(stTime) * indexPerFace);

				loop += 1;
			}
		}

		renderer->EndShader(shader_);

		renderer->GetRenderState()->Pop();
	}
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRenderer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_MODEL_RENDERER_H__
