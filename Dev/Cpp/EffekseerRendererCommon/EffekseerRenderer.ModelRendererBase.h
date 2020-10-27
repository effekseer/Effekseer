
#ifndef __EFFEKSEERRENDERER_MODEL_RENDERER_BASE_H__
#define __EFFEKSEERRENDERER_MODEL_RENDERER_BASE_H__

#include <Effekseer.Internal.h>
#include <Effekseer.h>
#include <algorithm>
#include <assert.h>
#include <string.h>
#include <vector>

#include "EffekseerRenderer.CommonUtils.h"
#include "EffekseerRenderer.IndexBufferBase.h"
#include "EffekseerRenderer.RenderStateBase.h"
#include "EffekseerRenderer.Renderer.h"
#include "EffekseerRenderer.VertexBufferBase.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
typedef ::Effekseer::ModelRenderer::NodeParameter efkModelNodeParam;
typedef ::Effekseer::ModelRenderer::InstanceParameter efkModelInstanceParam;
typedef ::Effekseer::Vec3f efkVector3D;

template <int MODEL_COUNT>
struct ModelRendererVertexConstantBuffer
{
	Effekseer::Matrix44 CameraMatrix;
	Effekseer::Matrix44 ModelMatrix[MODEL_COUNT];
	float ModelUV[MODEL_COUNT][4];
#ifdef __EFFEKSEER_BUILD_VERSION16__
	float ModelAlphaUV[MODEL_COUNT][4];

	struct
	{
		union {
			float Buffer[4];

			struct
			{
				float EnableInterpolation;
				float LoopType;
				float DivideX;
				float DivideY;
			};
		};
	} ModelFlipbookParameter;

	float ModelFlipbookIndexAndNextRate[MODEL_COUNT][4];

	float ModelAlphaThreshold[MODEL_COUNT][4];

#endif
	float ModelColor[MODEL_COUNT][4];

	float LightDirection[4];
	float LightColor[4];
	float LightAmbientColor[4];
	float UVInversed[4];
};

struct ModelRendererPixelConstantBuffer
{
	float LightDirection[4];
	float LightColor[4];
	float LightAmbientColor[4];

#ifdef __EFFEKSEER_BUILD_VERSION16__
	struct
	{
		union {
			float Buffer[4];

			struct
			{
				float EnableInterpolation;
				float InterpolationType;
			};
		};
	} ModelFlipbookParameter;
#endif
};

class ModelRendererBase
	: public ::Effekseer::ModelRenderer,
	  public ::Effekseer::AlignedAllocationPolicy<16>
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
#ifdef __EFFEKSEER_BUILD_VERSION16__
	std::vector<Effekseer::RectF> alphaUVSorted_;
	std::vector<float> flipbookIndexAndNextRateSorted_;
	std::vector<float> alphaThresholdSorted_;
#endif
	std::vector<Effekseer::Color> colorsSorted_;
	std::vector<int32_t> timesSorted_;
	std::vector<std::array<float, 4>> customData1Sorted_;
	std::vector<std::array<float, 4>> customData2Sorted_;

	std::vector<Effekseer::Matrix44> m_matrixes;
	std::vector<Effekseer::RectF> m_uv;
#ifdef __EFFEKSEER_BUILD_VERSION16__
	std::vector<Effekseer::RectF> m_alphaUV;
	std::vector<float> m_flipbookIndexAndNextRate;
	std::vector<float> m_alphaThreshold;
#endif
	std::vector<Effekseer::Color> m_colors;
	std::vector<int32_t> m_times;
	std::vector<std::array<float, 4>> customData1_;
	std::vector<std::array<float, 4>> customData2_;

	int32_t customData1Count_ = 0;
	int32_t customData2Count_ = 0;

	void ColorToFloat4(::Effekseer::Color color, float fc[4])
	{
		fc[0] = color.R / 255.0f;
		fc[1] = color.G / 255.0f;
		fc[2] = color.B / 255.0f;
		fc[3] = color.A / 255.0f;
	}

	void VectorToFloat4(const ::Effekseer::Vec3f& v, float fc[4])
	{
		::Effekseer::SIMD4f::Store3(fc, v.s);
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
			keyValues_.resize(m_matrixes.size());
			for (size_t i = 0; i < keyValues_.size(); i++)
			{
				efkVector3D t(
					m_matrixes[i].Values[3][0],
					m_matrixes[i].Values[3][1],
					m_matrixes[i].Values[3][2]);

				auto frontDirection = renderer->GetCameraFrontDirection();
				if (!param.IsRightHand)
				{
					frontDirection.Z = -frontDirection.Z;
				}

				keyValues_[i].Key = Effekseer::Vec3f::Dot(t, frontDirection);
				keyValues_[i].Value = static_cast<int32_t>(i);
			}

			if (param.DepthParameterPtr->ZSort == Effekseer::ZSortType::NormalOrder)
			{
				std::sort(keyValues_.begin(), keyValues_.end(), [](const KeyValue& a, const KeyValue& b) -> bool { return a.Key < b.Key; });
			}
			else
			{
				std::sort(keyValues_.begin(), keyValues_.end(), [](const KeyValue& a, const KeyValue& b) -> bool { return a.Key > b.Key; });
			}

			matrixesSorted_.resize(m_matrixes.size());
			uvSorted_.resize(m_matrixes.size());
#ifdef __EFFEKSEER_BUILD_VERSION16__
			alphaUVSorted_.resize(m_matrixes.size());
			flipbookIndexAndNextRateSorted_.resize(m_matrixes.size());
#endif
			colorsSorted_.resize(m_matrixes.size());
			timesSorted_.resize(m_matrixes.size());

			if (customData1Count_ > 0)
			{
				customData1Sorted_.resize(m_matrixes.size());
			}

			if (customData2Count_ > 0)
			{
				customData2Sorted_.resize(m_matrixes.size());
			}

			for (size_t i = 0; i < keyValues_.size(); i++)
			{
				matrixesSorted_[keyValues_[i].Value] = m_matrixes[i];
				uvSorted_[keyValues_[i].Value] = m_uv[i];
#ifdef __EFFEKSEER_BUILD_VERSION16__
				alphaUVSorted_[keyValues_[i].Value] = m_alphaUV[i];
				flipbookIndexAndNextRateSorted_[keyValues_[i].Value] = m_flipbookIndexAndNextRate[i];
				alphaThresholdSorted_[keyValues_[i].Value] = m_alphaThreshold[i];
#endif
				colorsSorted_[keyValues_[i].Value] = m_colors[i];
				timesSorted_[keyValues_[i].Value] = m_times[i];
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

			m_matrixes = matrixesSorted_;
			m_uv = uvSorted_;
#ifdef __EFFEKSEER_BUILD_VERSION16__
			m_alphaUV = alphaUVSorted_;
			m_flipbookIndexAndNextRate = flipbookIndexAndNextRateSorted_;
			m_alphaThreshold = alphaThresholdSorted_;
#endif
			m_colors = colorsSorted_;
			m_times = timesSorted_;
			customData1_ = customData1Sorted_;
			customData2_ = customData2Sorted_;
		}
	}

	template <typename RENDERER, typename SHADER, int InstanceCount>
	void StoreFileUniform(RENDERER* renderer,
						  SHADER* shader_,
						  Effekseer::MaterialData* material,
						  Effekseer::MaterialParameter* materialParam,
						  const efkModelNodeParam& param,
						  int32_t renderPassInd,
						  float*& cutomData1Ptr,
						  float*& cutomData2Ptr)
	{
		std::array<float, 4> uvInversed;
		std::array<float, 4> uvInversedBack;
		cutomData1Ptr = nullptr;
		cutomData2Ptr = nullptr;

		GetInversedFlags(renderer, uvInversed, uvInversedBack);

		std::array<float, 4> uvInversedMaterial;
		uvInversedMaterial[0] = uvInversed[0];
		uvInversedMaterial[1] = uvInversed[1];
		uvInversedMaterial[2] = uvInversedBack[0];
		uvInversedMaterial[3] = uvInversedBack[1];

		// camera
		float cameraPosition[4];
		::Effekseer::Vec3f cameraPosition3 = renderer->GetCameraPosition();
		VectorToFloat4(cameraPosition3, cameraPosition);

		// time
		std::array<float, 4> predefined_uniforms;
		predefined_uniforms.fill(0.5f);
		predefined_uniforms[0] = renderer->GetTime();

		// vs
		int32_t vsOffset = sizeof(Effekseer::Matrix44) + (sizeof(Effekseer::Matrix44) + sizeof(float) * 4 * 2) * InstanceCount;

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

		for (size_t i = 0; i < materialParam->MaterialUniforms.size(); i++)
		{
			renderer->SetVertexBufferToShader(materialParam->MaterialUniforms[i].data(), sizeof(float) * 4, vsOffset);
			vsOffset += (sizeof(float) * 4);
		}

		// ps
		int32_t psOffset = 0;
		renderer->SetPixelBufferToShader(uvInversedMaterial.data(), sizeof(float) * 4, psOffset);
		psOffset += (sizeof(float) * 4);

		renderer->SetPixelBufferToShader(predefined_uniforms.data(), sizeof(float) * 4, psOffset);
		psOffset += (sizeof(float) * 4);

		renderer->SetPixelBufferToShader(cameraPosition, sizeof(float) * 4, psOffset);
		psOffset += (sizeof(float) * 4);

		// shader model
		material = param.EffectPointer->GetMaterial(materialParam->MaterialIndex);

		if (material->ShadingModel == ::Effekseer::ShadingModelType::Lit)
		{
			float lightDirection[4];
			float lightColor[4];
			float lightAmbientColor[4];

			::Effekseer::Vec3f lightDirection3 = renderer->GetLightDirection();
			lightDirection3 = lightDirection3.Normalize();

			VectorToFloat4(lightDirection3, lightDirection);
			ColorToFloat4(renderer->GetLightColor(), lightColor);
			ColorToFloat4(renderer->GetLightAmbientColor(), lightAmbientColor);

			renderer->SetPixelBufferToShader(lightDirection, sizeof(float) * 4, psOffset);
			psOffset += (sizeof(float) * 4);

			renderer->SetPixelBufferToShader(lightColor, sizeof(float) * 4, psOffset);
			psOffset += (sizeof(float) * 4);

			renderer->SetPixelBufferToShader(lightAmbientColor, sizeof(float) * 4, psOffset);
			psOffset += (sizeof(float) * 4);
		}

		// refraction
		if (material->RefractionModelUserPtr != nullptr && renderPassInd == 0)
		{
			auto mat = renderer->GetCameraMatrix();
			renderer->SetPixelBufferToShader(&mat, sizeof(float) * 16, psOffset);
			psOffset += (sizeof(float) * 16);
		}

		for (size_t i = 0; i < materialParam->MaterialUniforms.size(); i++)
		{
			renderer->SetPixelBufferToShader(materialParam->MaterialUniforms[i].data(), sizeof(float) * 4, psOffset);
			psOffset += (sizeof(float) * 4);
		}
	}

public:
	virtual ~ModelRendererBase()
	{
	}

	template <typename RENDERER>
	void BeginRendering_(RENDERER* renderer, const efkModelNodeParam& parameter, int32_t count, void* userData)
	{
		keyValues_.clear();

		m_matrixes.clear();
		m_uv.clear();
#ifdef __EFFEKSEER_BUILD_VERSION16__
		m_alphaUV.clear();
		m_flipbookIndexAndNextRate.clear();
		m_alphaThreshold.clear();
#endif
		m_colors.clear();
		m_times.clear();
		customData1_.clear();
		customData2_.clear();

		matrixesSorted_.clear();
		uvSorted_.clear();
#ifdef __EFFEKSEER_BUILD_VERSION16__
		alphaUVSorted_.clear();
		flipbookIndexAndNextRateSorted_.clear();
		alphaThresholdSorted_.clear();
#endif
		colorsSorted_.clear();
		timesSorted_.clear();
		customData1Sorted_.clear();
		customData2Sorted_.clear();

		if (parameter.BasicParameterPtr->MaterialType == ::Effekseer::RendererMaterialType::File &&
			parameter.BasicParameterPtr->MaterialParameterPtr != nullptr &&
			parameter.BasicParameterPtr->MaterialParameterPtr->MaterialIndex >= 0 &&
			parameter.EffectPointer->GetMaterial(parameter.BasicParameterPtr->MaterialParameterPtr->MaterialIndex) != nullptr)
		{
			auto material = parameter.EffectPointer->GetMaterial(parameter.BasicParameterPtr->MaterialParameterPtr->MaterialIndex);
			customData1Count_ = material->CustomData1;
			customData2Count_ = material->CustomData2;
		}
		else
		{
			customData1Count_ = 0;
			customData2Count_ = 0;
		}

		renderer->GetStandardRenderer()->ResetAndRenderingIfRequired();
	}

	template <typename RENDERER>
	void Rendering_(RENDERER* renderer, const efkModelNodeParam& parameter, const efkModelInstanceParam& instanceParameter, void* userData)
	{
		::Effekseer::BillboardType btype = parameter.Billboard;
		Effekseer::Mat44f mat44;

		if (btype == ::Effekseer::BillboardType::Fixed)
		{
			mat44 = instanceParameter.SRTMatrix43;
		}
		else
		{
			Effekseer::Mat43f mat43;
			Effekseer::Vec3f s;
			Effekseer::Vec3f R;
			Effekseer::Vec3f F;

			CalcBillboard(btype, mat43, s, R, F, instanceParameter.SRTMatrix43, renderer->GetCameraFrontDirection());

			mat44 = ::Effekseer::Mat43f::Scaling(s) * mat43;
		}

		if (parameter.Magnification != 1.0f)
		{
			mat44 = Effekseer::Mat44f::Scaling(::Effekseer::Vec3f(parameter.Magnification)) * mat44;
		}

		if (!parameter.IsRightHand)
		{
			mat44 = Effekseer::Mat44f::Scaling(1.0f, 1.0f, -1.0f) * mat44;
		}

		m_matrixes.push_back(ToStruct(mat44));
		m_uv.push_back(instanceParameter.UV);
#ifdef __EFFEKSEER_BUILD_VERSION16__
		m_alphaUV.push_back(instanceParameter.AlphaUV);
		m_flipbookIndexAndNextRate.push_back(instanceParameter.FlipbookIndexAndNextRate);
		m_alphaThreshold.push_back(instanceParameter.AlphaThreshold);
#endif
		m_colors.push_back(instanceParameter.AllColor);
		m_times.push_back(instanceParameter.Time);

		if (customData1Count_ > 0)
		{
			customData1_.push_back(instanceParameter.CustomData1);
		}

		if (customData2Count_ > 0)
		{
			customData2_.push_back(instanceParameter.CustomData2);
		}
	}

	template <typename RENDERER, typename SHADER, typename MODEL, bool Instancing, int InstanceCount>
	void EndRendering_(RENDERER* renderer,
					   SHADER* shader_lighting_texture_normal,
					   SHADER* shader_texture,
					   SHADER* shader_distortion_texture,
					   const efkModelNodeParam& param)
	{
		if (m_matrixes.size() == 0)
			return;
		if (param.ModelIndex < 0)
			return;

		int32_t renderPassCount = 1;

		if (param.BasicParameterPtr->MaterialParameterPtr != nullptr && param.BasicParameterPtr->MaterialParameterPtr->MaterialIndex >= 0)
		{
			auto materialData = param.EffectPointer->GetMaterial(param.BasicParameterPtr->MaterialParameterPtr->MaterialIndex);
			if (materialData != nullptr && materialData->IsRefractionRequired)
			{
				// refraction, standard
				renderPassCount = 2;
			}
		}

		// sort
		SortTemporaryValues(renderer, param);

		for (int32_t renderPassInd = 0; renderPassInd < renderPassCount; renderPassInd++)
		{
			RenderPass<RENDERER, SHADER, MODEL, Instancing, InstanceCount>(renderer,
																		   shader_lighting_texture_normal,
																		   //shader_lighting_normal,
																		   //shader_lighting_texture,
																		   //shader_lighting,
																		   shader_texture,
																		   //shader,
																		   shader_distortion_texture,
																		   //shader_distortion,
																		   param,
																		   renderPassInd);
		}
	}

	template <typename RENDERER, typename SHADER, typename MODEL, bool Instancing, int InstanceCount>
	void RenderPass(
		RENDERER* renderer,
		SHADER* shader_lighting_texture_normal,
		//SHADER* shader_lighting_normal,
		//SHADER* shader_lighting_texture,
		//SHADER* shader_lighting,
		SHADER* shader_texture,
		//SHADER* shader,
		SHADER* shader_distortion_texture,
		//SHADER* shader_distortion,
		const efkModelNodeParam& param,
		int32_t renderPassInd)
	{
		if (m_matrixes.size() == 0)
			return;
		if (param.ModelIndex < 0)
			return;

		MODEL* model = (MODEL*)param.EffectPointer->GetModel(param.ModelIndex);
		if (model == NULL)
			return;

		bool isBackgroundRequired = false;

		isBackgroundRequired |= (param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::BackDistortion);

		if (param.BasicParameterPtr->MaterialParameterPtr != nullptr && param.BasicParameterPtr->MaterialParameterPtr->MaterialIndex >= 0)
		{
			auto materialData = param.EffectPointer->GetMaterial(param.BasicParameterPtr->MaterialParameterPtr->MaterialIndex);
			if (materialData != nullptr && materialData->IsRefractionRequired && renderPassInd == 0)
			{
				isBackgroundRequired = true;
			}
		}

		if (isBackgroundRequired)
		{
			auto callback = renderer->GetDistortingCallback();
			if (callback != nullptr)
			{
				if (!callback->OnDistorting())
				{
					return;
				}
			}
		}

		auto distortion = param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::BackDistortion;

		if (isBackgroundRequired && renderer->GetBackground() == 0)
			return;

		// select shader
		Effekseer::MaterialParameter* materialParam = param.BasicParameterPtr->MaterialParameterPtr;
		//materialParam = nullptr;
		Effekseer::MaterialData* material = nullptr;
		SHADER* shader_ = nullptr;
		bool renderDistortedBackground = false;

		if (materialParam != nullptr && materialParam->MaterialIndex >= 0 &&
			param.EffectPointer->GetMaterial(materialParam->MaterialIndex) != nullptr)
		{
			material = param.EffectPointer->GetMaterial(materialParam->MaterialIndex);

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

			// validate
			if (shader_ == nullptr)
			{
				return;
			}

			if (material != nullptr && (material->TextureCount != materialParam->MaterialTextures.size() ||
										material->UniformCount != materialParam->MaterialUniforms.size()))
			{
				return;
			}
		}
		else
		{
			if (distortion)
			{
				//if (param.BasicParameterPtr->Texture1Index >= 0)
				//{
				shader_ = shader_distortion_texture;
				//}
				//else
				//{
				//	shader_ = shader_distortion;
				//}
			}
			else if (param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::Lighting)
			{
				//if (param.BasicParameterPtr->Texture2Index >= 0)
				//{
				//	if (param.BasicParameterPtr->Texture1Index >= 0)
				//	{
				shader_ = shader_lighting_texture_normal;
				//	}
				//	else
				//	{
				//		shader_ = shader_lighting_normal;
				//	}
				//}
				//else
				//{
				//	if (param.BasicParameterPtr->Texture1Index >= 0)
				//	{
				//		shader_ = shader_lighting_texture;
				//	}
				//	else
				//	{
				//		shader_ = shader_lighting;
				//	}
				//}
			}
			else
			{
				//if (param.BasicParameterPtr->Texture1Index >= 0)
				//{
				shader_ = shader_texture;
				//}
				//else
				//{
				//	shader_ = shader;
				//}
			}
		}

		RenderStateBase::State& state = renderer->GetRenderState()->Push();
		state.DepthTest = param.ZTest;
		state.DepthWrite = param.ZWrite;
		state.AlphaBlend = param.BasicParameterPtr->AlphaBlend;
		state.CullingType = param.Culling;

		if (renderDistortedBackground)
		{
			state.AlphaBlend = ::Effekseer::AlphaBlendType::Blend;
		}

		renderer->BeginShader(shader_);

		// Select texture
		if (materialParam != nullptr && material != nullptr)
		{
			int32_t textureCount = 0;
			std::array<Effekseer::TextureData*, ::Effekseer::TextureSlotMax> textures;

			if (materialParam->MaterialTextures.size() > 0)
			{
				textureCount = Effekseer::Min(materialParam->MaterialTextures.size(), ::Effekseer::UserTextureSlotMax);

				auto effect = param.EffectPointer;

				for (size_t i = 0; i < textureCount; i++)
				{
					if (materialParam->MaterialTextures[i].Type == 1)
					{
						if (materialParam->MaterialTextures[i].Index >= 0)
						{
							textures[i] = effect->GetNormalImage(materialParam->MaterialTextures[i].Index);
						}
						else
						{
							textures[i] = nullptr;
						}
					}
					else
					{
						if (materialParam->MaterialTextures[i].Index >= 0)
						{
							textures[i] = effect->GetColorImage(materialParam->MaterialTextures[i].Index);
						}
						else
						{
							textures[i] = nullptr;
						}
					}

					state.TextureFilterTypes[i] = Effekseer::TextureFilterType::Linear;
					state.TextureWrapTypes[i] = material->TextureWrapTypes[i];
				}
			}

			if (renderer->GetBackground() != 0)
			{
				textures[textureCount] = renderer->GetBackground();
				state.TextureFilterTypes[textureCount] = Effekseer::TextureFilterType::Linear;
				state.TextureWrapTypes[textureCount] = Effekseer::TextureWrapType::Clamp;
				textureCount += 1;
			}

			if (textureCount > 0)
			{
				renderer->SetTextures(shader_, textures.data(), textureCount);
			}
		}
		else
		{
#ifdef __EFFEKSEER_BUILD_VERSION16__
			Effekseer::TextureData* textures[3];
			textures[0] = nullptr;
			textures[1] = nullptr;
			textures[2] = nullptr;
#else
			Effekseer::TextureData* textures[2];
			textures[0] = nullptr;
			textures[1] = nullptr;
#endif

			if (distortion)
			{
				if (param.BasicParameterPtr->Texture1Index >= 0)
				{
					textures[0] = param.EffectPointer->GetDistortionImage(param.BasicParameterPtr->Texture1Index);
				}
				else
				{
					textures[0] = renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
				}

				textures[1] = renderer->GetBackground();

#ifdef __EFFEKSEER_BUILD_VERSION16__
				if (param.BasicParameterPtr->Texture3Index >= 0)
				{
					textures[2] = param.EffectPointer->GetDistortionImage(param.BasicParameterPtr->Texture3Index);
				}
				else
				{
					textures[2] = renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
				}
#endif
			}
			else
			{
				if (param.BasicParameterPtr->Texture1Index >= 0)
				{
					textures[0] = param.EffectPointer->GetColorImage(param.BasicParameterPtr->Texture1Index);
				}

				if (textures[0] == nullptr)
				{
					textures[0] = renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
				}

				if (param.BasicParameterPtr->Texture2Index >= 0)
				{
					textures[1] = param.EffectPointer->GetNormalImage(param.BasicParameterPtr->Texture2Index);
				}

				if (textures[1] == nullptr)
				{
					textures[1] = renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::Normal);
				}

#ifdef __EFFEKSEER_BUILD_VERSION16__
				if (param.BasicParameterPtr->Texture3Index >= 0)
				{
					textures[2] = param.EffectPointer->GetColorImage(param.BasicParameterPtr->Texture3Index);
				}

				if (textures[2] == nullptr)
				{
					textures[2] = renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
				}
#endif
			}

			state.TextureFilterTypes[0] = param.BasicParameterPtr->TextureFilter1;
			state.TextureWrapTypes[0] = param.BasicParameterPtr->TextureWrap1;

			if (distortion)
			{
				state.TextureFilterTypes[1] = Effekseer::TextureFilterType::Linear;
				state.TextureWrapTypes[1] = Effekseer::TextureWrapType::Clamp;
			}
			else
			{
				state.TextureFilterTypes[1] = param.BasicParameterPtr->TextureFilter2;
				state.TextureWrapTypes[1] = param.BasicParameterPtr->TextureWrap2;
			}

#ifdef __EFFEKSEER_BUILD_VERSION16__
			state.TextureFilterTypes[2] = param.BasicParameterPtr->TextureFilter3;
			state.TextureWrapTypes[2] = param.BasicParameterPtr->TextureWrap3;
#endif

#ifdef __EFFEKSEER_BUILD_VERSION16__
			renderer->SetTextures(shader_, textures, 3);
#else
			renderer->SetTextures(shader_, textures, 2);
#endif
		}

		renderer->GetRenderState()->Update(distortion);

		ModelRendererVertexConstantBuffer<InstanceCount>* vcb =
			(ModelRendererVertexConstantBuffer<InstanceCount>*)shader_->GetVertexConstantBuffer();

		float* cutomData1Ptr = nullptr;
		float* cutomData2Ptr = nullptr;

		if (materialParam != nullptr && material != nullptr)
		{
			StoreFileUniform<RENDERER, SHADER, InstanceCount>(renderer, shader_, material, materialParam, param, renderPassInd, cutomData1Ptr, cutomData2Ptr);
		}
		else
		{
			std::array<float, 4> uvInversed;
			std::array<float, 4> uvInversedBack;

			GetInversedFlags(renderer, uvInversed, uvInversedBack);

			vcb->UVInversed[0] = uvInversed[0];
			vcb->UVInversed[1] = uvInversed[1];

			if (distortion)
			{
				float* pcb = (float*)shader_->GetPixelConstantBuffer();
				pcb[4 * 0 + 0] = param.BasicParameterPtr->DistortionIntensity;

				pcb[4 * 1 + 0] = uvInversedBack[0];
				pcb[4 * 1 + 1] = uvInversedBack[1];

#ifdef __EFFEKSEER_BUILD_VERSION16__
				pcb[4 * 2 + 0] = param.BasicParameterPtr->EnableInterpolation;
				pcb[4 * 2 + 1] = param.BasicParameterPtr->InterpolationType;
#endif
			}
			else
			{
				ModelRendererPixelConstantBuffer* pcb = (ModelRendererPixelConstantBuffer*)shader_->GetPixelConstantBuffer();

				// specify predefined parameters
				if (param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::Lighting)
				{
					::Effekseer::Vec3f lightDirection = renderer->GetLightDirection();
					lightDirection = lightDirection.Normalize();
					VectorToFloat4(lightDirection, vcb->LightDirection);
					VectorToFloat4(lightDirection, pcb->LightDirection);
				}

				{
					ColorToFloat4(renderer->GetLightColor(), vcb->LightColor);
					ColorToFloat4(renderer->GetLightColor(), pcb->LightColor);
				}

				{
					ColorToFloat4(renderer->GetLightAmbientColor(), vcb->LightAmbientColor);
					ColorToFloat4(renderer->GetLightAmbientColor(), pcb->LightAmbientColor);
				}

#ifdef __EFFEKSEER_BUILD_VERSION16__
				pcb->ModelFlipbookParameter.EnableInterpolation = param.BasicParameterPtr->EnableInterpolation;
				pcb->ModelFlipbookParameter.InterpolationType = param.BasicParameterPtr->InterpolationType;
#endif
			}
		}

		vcb->CameraMatrix = renderer->GetCameraProjectionMatrix();

#ifdef __EFFEKSEER_BUILD_VERSION16__
		vcb->ModelFlipbookParameter.EnableInterpolation = param.BasicParameterPtr->EnableInterpolation;
		vcb->ModelFlipbookParameter.LoopType = param.BasicParameterPtr->UVLoopType;
		vcb->ModelFlipbookParameter.DivideX = param.BasicParameterPtr->FlipbookDivideX;
		vcb->ModelFlipbookParameter.DivideY = param.BasicParameterPtr->FlipbookDivideY;
#endif

		// Check time
		auto stTime0 = m_times[0] % model->GetFrameCount();
		auto isTimeSame = true;

		for (auto t : m_times)
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
			auto& imodel = model->InternalModels[stTime0];

			// Invalid unless layout is set after buffer
			renderer->SetVertexBuffer(imodel.VertexBuffer, model->GetVertexSize());
			renderer->SetIndexBuffer(imodel.IndexBuffer);
			renderer->SetLayout(shader_);

			for (size_t loop = 0; loop < m_matrixes.size();)
			{
				int32_t modelCount = Effekseer::Min(static_cast<int32_t>(m_matrixes.size()) - loop, model->ModelCount);

				for (int32_t num = 0; num < modelCount; num++)
				{
					vcb->ModelMatrix[num] = m_matrixes[loop + num];

					// DepthParameter
					::Effekseer::Mat44f modelMatrix = vcb->ModelMatrix[num];
					ApplyDepthParameters(modelMatrix,
										 renderer->GetCameraFrontDirection(),
										 renderer->GetCameraPosition(),
										 param.DepthParameterPtr,
										 param.IsRightHand);
					vcb->ModelMatrix[num] = ToStruct(modelMatrix);

					vcb->ModelUV[num][0] = m_uv[loop + num].X;
					vcb->ModelUV[num][1] = m_uv[loop + num].Y;
					vcb->ModelUV[num][2] = m_uv[loop + num].Width;
					vcb->ModelUV[num][3] = m_uv[loop + num].Height;

#ifdef __EFFEKSEER_BUILD_VERSION16__
					vcb->ModelAlphaUV[num][0] = m_alphaUV[loop + num].X;
					vcb->ModelAlphaUV[num][1] = m_alphaUV[loop + num].Y;
					vcb->ModelAlphaUV[num][2] = m_alphaUV[loop + num].Width;
					vcb->ModelAlphaUV[num][3] = m_alphaUV[loop + num].Height;

					vcb->ModelFlipbookIndexAndNextRate[num][0] = m_flipbookIndexAndNextRate[loop + num];

					vcb->ModelAlphaThreshold[num][0] = m_alphaThreshold[loop + num];
#endif

					ColorToFloat4(m_colors[loop + num], vcb->ModelColor[num]);

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

				renderer->DrawPolygon(imodel.VertexCount * modelCount, imodel.IndexCount * modelCount);

				loop += modelCount;
			}
		}
		else
		{
			for (size_t loop = 0; loop < m_matrixes.size();)
			{
				auto stTime = m_times[loop] % model->GetFrameCount();
				auto& imodel = model->InternalModels[stTime];

				// Invalid unless layout is set after buffer
				renderer->SetVertexBuffer(imodel.VertexBuffer, model->GetVertexSize());
				renderer->SetIndexBuffer(imodel.IndexBuffer);
				renderer->SetLayout(shader_);

				vcb->ModelMatrix[0] = m_matrixes[loop];
				vcb->ModelUV[0][0] = m_uv[loop].X;
				vcb->ModelUV[0][1] = m_uv[loop].Y;
				vcb->ModelUV[0][2] = m_uv[loop].Width;
				vcb->ModelUV[0][3] = m_uv[loop].Height;

#ifdef __EFFEKSEER_BUILD_VERSION16__
				vcb->ModelAlphaUV[0][0] = m_alphaUV[loop].X;
				vcb->ModelAlphaUV[0][1] = m_alphaUV[loop].Y;
				vcb->ModelAlphaUV[0][2] = m_alphaUV[loop].Width;
				vcb->ModelAlphaUV[0][3] = m_alphaUV[loop].Height;

				vcb->ModelFlipbookIndexAndNextRate[0][0] = m_flipbookIndexAndNextRate[loop];

				vcb->ModelAlphaThreshold[0][0] = m_alphaThreshold[loop];
#endif

				// DepthParameters
				::Effekseer::Mat44f modelMatrix = vcb->ModelMatrix[0];
				ApplyDepthParameters(modelMatrix, renderer->GetCameraFrontDirection(), renderer->GetCameraPosition(), param.DepthParameterPtr, param.IsRightHand);
				vcb->ModelMatrix[0] = ToStruct(modelMatrix);
				ColorToFloat4(m_colors[loop], vcb->ModelColor[0]);

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
				renderer->DrawPolygon(imodel.VertexCount, imodel.IndexCount);

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
