
#ifndef __EFFEKSEERRENDERER_STANDARD_RENDERER_BASE_H__
#define __EFFEKSEERRENDERER_STANDARD_RENDERER_BASE_H__

#include <Effekseer.h>
#include <algorithm>
#include <functional>
#include <vector>

#include "EffekseerRenderer.CommonUtils.h"
#include "EffekseerRenderer.Renderer.h"
#include "EffekseerRenderer.Renderer_Impl.h"
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
	bool DepthTest;
	bool DepthWrite;
	bool Distortion;
	float DistortionIntensity;
	bool Refraction;

	::Effekseer::AlphaBlendType AlphaBlend;
	::Effekseer::CullingType CullingType;

	int32_t EnableInterpolation;
	int32_t UVLoopType;
	int32_t InterpolationType;
	int32_t FlipbookDivideX;
	int32_t FlipbookDivideY;

	float UVDistortionIntensity;

	int32_t TextureBlendType;

	float BlendUVDistortionIntensity;

	float EmissiveScaling;

	float EdgeThreshold;
	uint8_t EdgeColor[4];
	float EdgeColorScaling;
	bool IsAlphaCuttoffEnabled = false;
	float SoftParticleDistanceFar = 0.0f;
	float SoftParticleDistanceNear = 0.0f;
	float SoftParticleDistanceNearOffset = 0.0f;
	float Maginification = 1.0f;

	::Effekseer::RendererMaterialType MaterialType;
	int32_t MaterialUniformCount = 0;
	std::array<std::array<float, 4>, 16> MaterialUniforms;

	int32_t CustomData1Count = 0;
	int32_t CustomData2Count = 0;

	ShaderParameterCollector Collector{};

	Effekseer::RefPtr<Effekseer::RenderingUserData> RenderingUserData{};
	void* HandleUserData;

	StandardRendererState()
	{
		DepthTest = false;
		DepthWrite = false;
		Distortion = false;
		DistortionIntensity = 1.0f;
		Refraction = false;

		AlphaBlend = ::Effekseer::AlphaBlendType::Blend;
		CullingType = ::Effekseer::CullingType::Front;

		EnableInterpolation = 0;
		UVLoopType = 0;
		InterpolationType = 0;
		FlipbookDivideX = 0;
		FlipbookDivideY = 0;

		UVDistortionIntensity = 1.0f;

		TextureBlendType = 0;

		BlendUVDistortionIntensity = 1.0f;

		EmissiveScaling = 1.0f;

		EdgeThreshold = 0.0f;
		EdgeColor[0] = EdgeColor[1] = EdgeColor[2] = EdgeColor[3] = 0;
		EdgeColorScaling = 1;

		MaterialType = ::Effekseer::RendererMaterialType::Default;
		MaterialUniformCount = 0;
		CustomData1Count = 0;
		CustomData2Count = 0;

		RenderingUserData.Reset();
		HandleUserData = nullptr;
	}

	bool operator!=(const StandardRendererState state)
	{
		if (Collector != state.Collector)
			return true;

		if (DepthTest != state.DepthTest)
			return true;
		if (DepthWrite != state.DepthWrite)
			return true;
		if (Distortion != state.Distortion)
			return true;
		if (DistortionIntensity != state.DistortionIntensity)
			return true;
		if (AlphaBlend != state.AlphaBlend)
			return true;
		if (CullingType != state.CullingType)
			return true;

		if (EnableInterpolation != state.EnableInterpolation)
			return true;
		if (UVLoopType != state.UVLoopType)
			return true;
		if (InterpolationType != state.InterpolationType)
			return true;
		if (FlipbookDivideX != state.FlipbookDivideX)
			return true;
		if (FlipbookDivideY != state.FlipbookDivideY)
			return true;

		if (UVDistortionIntensity != state.UVDistortionIntensity)
			return true;
		if (TextureBlendType != state.TextureBlendType)
			return true;
		if (BlendUVDistortionIntensity != state.BlendUVDistortionIntensity)
			return true;
		if (EmissiveScaling != state.EmissiveScaling)
			return true;

		if (EdgeThreshold != state.EdgeThreshold)
			return true;
		if (EdgeColor[0] != state.EdgeColor[0] ||
			EdgeColor[1] != state.EdgeColor[1] ||
			EdgeColor[2] != state.EdgeColor[2] ||
			EdgeColor[3] != state.EdgeColor[3])
			return true;
		if (EdgeColorScaling != state.EdgeColorScaling)
			return true;

		if (IsAlphaCuttoffEnabled != state.IsAlphaCuttoffEnabled)
			return true;

		if (SoftParticleDistanceFar != state.SoftParticleDistanceFar)
			return true;

		if (SoftParticleDistanceNear != state.SoftParticleDistanceNear)
			return true;

		if (SoftParticleDistanceNearOffset != state.SoftParticleDistanceNearOffset)
			return true;

		if (Maginification != state.Maginification)
			return true;

		if (MaterialType != state.MaterialType)
			return true;
		if (MaterialUniformCount != state.MaterialUniformCount)
			return true;
		if (Refraction != state.Refraction)
			return true;

		for (int32_t i = 0; i < state.MaterialUniformCount; i++)
		{
			if (MaterialUniforms[i] != state.MaterialUniforms[i])
				return true;
		}

		if (CustomData1Count != state.CustomData1Count)
			return true;

		if (CustomData1Count != state.CustomData1Count)
			return true;

		if (RenderingUserData == nullptr && state.RenderingUserData != nullptr)
			return true;

		if (RenderingUserData != nullptr && state.RenderingUserData == nullptr)
			return true;

		if (RenderingUserData != nullptr && state.RenderingUserData != nullptr && !RenderingUserData->Equal(state.RenderingUserData.Get()))
			return true;

		if (HandleUserData != state.HandleUserData)
			return true;

		return false;
	}

	void CopyMaterialFromParameterToState(
		EffekseerRenderer::Renderer* renderer,
		Effekseer::Effect* effect,
		Effekseer::NodeRendererBasicParameter* basicParam)
	{
		AlphaBlend = basicParam->AlphaBlend;

		// TODO : refactor in 1.7
		if (renderer->GetExternalShaderSettings() != nullptr)
		{
			AlphaBlend = renderer->GetExternalShaderSettings()->Blend;		
		}
		else if (renderer->GetRenderMode() == ::Effekseer::RenderMode::Wireframe)
		{
			AlphaBlend = ::Effekseer::AlphaBlendType::Opacity;
		}

		Collector = ShaderParameterCollector();
		Collector.Collect(renderer, effect, basicParam, false, renderer->GetImpl()->isSoftParticleEnabled);

		SoftParticleDistanceFar = basicParam->SoftParticleDistanceFar;
		SoftParticleDistanceNear = basicParam->SoftParticleDistanceNear;
		SoftParticleDistanceNearOffset = basicParam->SoftParticleDistanceNearOffset;

		if (Collector.MaterialRenderDataPtr != nullptr && Collector.MaterialDataPtr != nullptr)
		{
			CustomData1Count = Collector.MaterialDataPtr->CustomData1;
			CustomData2Count = Collector.MaterialDataPtr->CustomData2;

			MaterialUniformCount =
				static_cast<int32_t>(Effekseer::Min(Collector.MaterialRenderDataPtr->MaterialUniforms.size(), MaterialUniforms.size()));
			for (size_t i = 0; i < MaterialUniformCount; i++)
			{
				MaterialUniforms[i] = Collector.MaterialRenderDataPtr->MaterialUniforms[i];
			}
		}
		else
		{
			Refraction = false;
			CustomData1Count = 0;
			CustomData2Count = 0;
		}
	}
};

struct StandardRendererVertexBuffer
{
	Effekseer::Matrix44 constantVSBuffer[2];
	float uvInversed[4];

	struct
	{
		union
		{
			float Buffer[4];

			struct
			{
				float enableInterpolation;
				float loopType;
				float divideX;
				float divideY;
			};
		};
	} flipbookParameter;
};

template <typename RENDERER, typename SHADER>
class StandardRenderer
{
private:
	RENDERER* m_renderer;

	struct RenderInfo
	{
		StandardRendererState state;
		int offset;
		int size;
		int stride;
		bool isLargeSize;
		bool hasDistortion;
	};

	//! WebAssembly requires a much time to resize std::vector (in a profiler at least) so reduce to call resize
	std::vector<uint8_t> vertexCaches_;
	int32_t vertexCacheMaxSize_ = 0;
	int32_t vertexCacheOffset_ = 0;
	EffekseerRenderer::VertexBufferBase* lastVb_ = nullptr;

	Effekseer::CustomAlignedVector<RenderInfo> renderInfos_;

	void ColorToFloat4(::Effekseer::Color color, float fc[4])
	{
		fc[0] = color.R / 255.0f;
		fc[1] = color.G / 255.0f;
		fc[2] = color.B / 255.0f;
		fc[3] = color.A / 255.0f;
	}

	void VectorToFloat4(const ::Effekseer::SIMD::Vec3f& v, float fc[4])
	{
		::Effekseer::SIMD::Float4::Store3(fc, v.s);
		fc[3] = 1.0f;
	}

public:
	StandardRenderer(RENDERER* renderer)
	{
		m_renderer = renderer;
		vertexCacheMaxSize_ = m_renderer->GetVertexBuffer()->GetMaxSize();
		vertexCaches_.reserve(m_renderer->GetVertexBuffer()->GetMaxSize());
	}

	virtual ~StandardRenderer()
	{
	}

	static int32_t CalculateCurrentStride(const StandardRendererState& state)
	{
		const auto renderingMode = state.Collector.ShaderType;
		size_t stride = 0;
		if (renderingMode == RendererShaderType::Material)
		{
			stride = sizeof(DynamicVertex);
			stride += (state.CustomData1Count + state.CustomData2Count) * sizeof(float);
		}
		else if (renderingMode == RendererShaderType::Lit)
		{
			stride = sizeof(LightingVertex);
		}
		else if (renderingMode == RendererShaderType::BackDistortion)
		{
			stride = sizeof(LightingVertex);
		}
		else if (renderingMode == RendererShaderType::Unlit)
		{
			stride = sizeof(SimpleVertex);
		}
		else if (renderingMode == RendererShaderType::AdvancedLit)
		{
			stride = sizeof(AdvancedLightingVertex);
		}
		else if (renderingMode == RendererShaderType::AdvancedBackDistortion)
		{
			stride = sizeof(AdvancedLightingVertex);
		}
		else if (renderingMode == RendererShaderType::AdvancedUnlit)
		{
			stride = sizeof(AdvancedSimpleVertex);
		}

		return static_cast<int32_t>(stride);
	}

	void BeginRenderingAndRenderingIfRequired(StandardRendererState state, int32_t count, int& stride, void*& data)
	{
		if (renderInfos_.size() > 0 && renderInfos_[renderInfos_.size() - 1].state != state)
		{
			//Rendering();
		}

		if (renderInfos_.size() > 0 && (renderInfos_[renderInfos_.size() - 1].isLargeSize || renderInfos_[renderInfos_.size() - 1].hasDistortion))
		{
			Rendering();
		}

		stride = CalculateCurrentStride(state);

		const int32_t requiredSize = count * stride;

		if (requiredSize + EffekseerRenderer::VertexBufferBase::GetNextAliginedVertexRingOffset(vertexCacheOffset_, stride * 4) > vertexCacheMaxSize_)
		{
			Rendering();
		}

		if (renderInfos_.size() == 0)
		{
			EffekseerRenderer::VertexBufferBase* vb = m_renderer->GetVertexBuffer();
			vertexCacheOffset_ = vb->GetVertexRingOffset();
			lastVb_ = vb;

			// reset
			if (!vb->GetIsRingEnabled())
			{
				vertexCacheOffset_ = 0;
			}

			if (requiredSize + EffekseerRenderer::VertexBufferBase::GetNextAliginedVertexRingOffset(vertexCacheOffset_, stride * 4) > vertexCacheMaxSize_)
			{
				vertexCacheOffset_ = 0;
			}
		}

		vertexCacheOffset_ = EffekseerRenderer::VertexBufferBase::GetNextAliginedVertexRingOffset(vertexCacheOffset_, stride * 4);

		const auto oldOffset = vertexCacheOffset_;
		vertexCacheOffset_ += requiredSize;
		if (vertexCaches_.size() < vertexCacheOffset_)
		{
			vertexCaches_.resize(vertexCacheOffset_);
		}

		data = (vertexCaches_.data() + oldOffset);

		RenderInfo renderInfo;
		renderInfo.state = state;
		renderInfo.size = requiredSize;
		renderInfo.offset = oldOffset;
		renderInfo.stride = stride;
		renderInfo.isLargeSize = requiredSize > vertexCacheMaxSize_;
		renderInfo.hasDistortion = state.Collector.IsBackgroundRequiredOnFirstPass && m_renderer->GetDistortingCallback() != nullptr;
		renderInfos_.emplace_back(renderInfo);
	}

	void ResetAndRenderingIfRequired()
	{
		Rendering();
	}

	StandardRendererState& GetState()
	{
		assert(renderInfos_.size() > 0);
		return renderInfos_.back().state;
	}

	void Rendering()
	{
		if (vertexCacheOffset_ == 0)
			return;

		int cpuBufStart = INT_MAX;
		int cpuBufEnd = 0;

		for (auto& info : renderInfos_)
		{
			cpuBufStart = Effekseer::Min(cpuBufStart, info.offset);
			cpuBufEnd = Effekseer::Max(cpuBufEnd, info.offset + info.size);
		}

		const int cpuBufSize = cpuBufEnd - cpuBufStart;

		{
			VertexBufferBase* vb = m_renderer->GetVertexBuffer();
			assert(vb == lastVb_);

			void* vbData = nullptr;
			int32_t vbOffset = 0;

			if (vb->RingBufferLock(cpuBufSize, vbOffset, vbData, renderInfos_.begin()->stride * 4))
			{
				assert(vbData != nullptr);
				assert(vbOffset == cpuBufStart);

				const auto dst = (reinterpret_cast<uint8_t*>(vbData));
				memcpy(dst, vertexCaches_.data() + cpuBufStart, cpuBufSize);
				vb->Unlock();
			}
			else
			{
				m_renderer->GetImpl()->CurrentRingBufferIndex++;
				m_renderer->GetImpl()->CurrentRingBufferIndex %= m_renderer->GetImpl()->RingBufferCount;
				return;
			}
		}

		for (auto& info : renderInfos_)
		{
			const auto& state = info.state;

			const auto& mProj = m_renderer->GetProjectionMatrix();

			int32_t stride = CalculateCurrentStride(state);

			int32_t passNum = 1;

			if (state.Collector.ShaderType == RendererShaderType::Material)
			{
				if (state.Collector.MaterialDataPtr->RefractionUserPtr != nullptr)
				{
					// refraction and standard
					passNum = 2;
				}
			}

			for (int32_t passInd = 0; passInd < passNum; passInd++)
			{
				int32_t offset = 0;

				// only sprite
				int32_t renderBufferSize = info.size;

				if (renderBufferSize > vertexCacheMaxSize_)
				{
					assert(renderInfos_.size() == 1 && renderInfos_[0].offset == 0);
					renderBufferSize = (vertexCacheMaxSize_ / (stride * 4)) * (stride * 4);
				}

				Rendering_(m_renderer->GetCameraMatrix(), mProj, info.offset, renderBufferSize, info.stride, passInd, state);
			}
		}

		vertexCacheOffset_ = 0;
		renderInfos_.clear();

		m_renderer->GetImpl()->CurrentRingBufferIndex++;
		m_renderer->GetImpl()->CurrentRingBufferIndex %= m_renderer->GetImpl()->RingBufferCount;
	}

	void Rendering_(const Effekseer::SIMD::Mat44f& mCamera,
					const Effekseer::SIMD::Mat44f& mProj,
					int32_t vbOffset,
					int32_t bufferSize,
					int32_t stride,
					int32_t renderPass,
					const StandardRendererState& renderState)
	{
		bool isBackgroundRequired = renderState.Collector.IsBackgroundRequiredOnFirstPass && renderPass == 0;

		if (isBackgroundRequired)
		{
			auto callback = m_renderer->GetDistortingCallback();
			if (callback != nullptr)
			{
				if (!callback->OnDistorting(m_renderer))
				{
					return;
				}
			}
		}

		if (isBackgroundRequired && m_renderer->GetBackground() == 0)
		{
			return;
		}

		auto textures = renderState.Collector.Textures;
		if (isBackgroundRequired)
		{
			textures[renderState.Collector.BackgroundIndex] = m_renderer->GetBackground();
		}

		::Effekseer::Backend::TextureRef depthTexture = nullptr;
		::EffekseerRenderer::DepthReconstructionParameter reconstructionParam;
		m_renderer->GetImpl()->GetDepth(depthTexture, reconstructionParam);

		if (renderState.Collector.IsDepthRequired)
		{
			if (depthTexture == nullptr || (renderState.SoftParticleDistanceFar == 0.0f &&
											renderState.SoftParticleDistanceNear == 0.0f &&
											renderState.SoftParticleDistanceNearOffset == 0.0f &&
											renderState.Collector.ShaderType != RendererShaderType::Material))
			{
				depthTexture = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
			}

			textures[renderState.Collector.DepthIndex] = depthTexture;
		}

		SHADER* shader_ = nullptr;

		bool distortion = renderState.Distortion;
		bool renderDistortedBackground = false;

		if (renderState.Collector.ShaderType == RendererShaderType::Material)
		{
			if (renderState.Collector.MaterialDataPtr->IsRefractionRequired)
			{
				if (renderPass == 0)
				{
					if (m_renderer->GetBackground() == 0)
					{
						return;
					}

					shader_ = (SHADER*)renderState.Collector.MaterialDataPtr->RefractionUserPtr;
					renderDistortedBackground = true;
				}
				else
				{
					shader_ = (SHADER*)renderState.Collector.MaterialDataPtr->UserPtr;
				}
			}
			else
			{
				shader_ = (SHADER*)renderState.Collector.MaterialDataPtr->UserPtr;
			}

			// validate
			if (shader_ == nullptr)
				return;
		}
		else
		{
			shader_ = m_renderer->GetShader(renderState.Collector.ShaderType);
		}

		RenderStateBase::State& state = m_renderer->GetRenderState()->Push();
		state.DepthTest = renderState.DepthTest;
		state.DepthWrite = renderState.DepthWrite;
		state.CullingType = renderState.CullingType;
		state.AlphaBlend = renderState.AlphaBlend;

		if (renderDistortedBackground)
		{
			state.AlphaBlend = ::Effekseer::AlphaBlendType::Blend;
		}

		m_renderer->BeginShader(shader_);

		for (int32_t i = 0; i < renderState.Collector.TextureCount; i++)
		{
			state.TextureFilterTypes[i] = renderState.Collector.TextureFilterTypes[i];
			state.TextureWrapTypes[i] = renderState.Collector.TextureWrapTypes[i];
		}

		m_renderer->SetTextures(shader_, textures.data(), renderState.Collector.TextureCount);

		std::array<float, 4> uvInversed;
		std::array<float, 4> uvInversedBack;
		std::array<float, 4> uvInversedMaterial;

		if (m_renderer->GetTextureUVStyle() == UVStyle::VerticalFlipped)
		{
			uvInversed[0] = 1.0f;
			uvInversed[1] = -1.0f;
		}
		else
		{
			uvInversed[0] = 0.0f;
			uvInversed[1] = 1.0f;
		}

		if (m_renderer->GetBackgroundTextureUVStyle() == UVStyle::VerticalFlipped)
		{
			uvInversedBack[0] = 1.0f;
			uvInversedBack[1] = -1.0f;
		}
		else
		{
			uvInversedBack[0] = 0.0f;
			uvInversedBack[1] = 1.0f;
		}

		uvInversedMaterial[0] = uvInversed[0];
		uvInversedMaterial[1] = uvInversed[1];
		uvInversedMaterial[2] = uvInversedBack[0];
		uvInversedMaterial[3] = uvInversedBack[1];

		if (renderState.Collector.ShaderType == RendererShaderType::Material)
		{
			Effekseer::Matrix44 mstCamera = ToStruct(mCamera);
			Effekseer::Matrix44 mstProj = ToStruct(mProj);

			// camera
			float cameraPosition[4];
			::Effekseer::SIMD::Vec3f cameraPosition3 = m_renderer->GetCameraPosition();
			VectorToFloat4(cameraPosition3, cameraPosition);
			// time
			std::array<float, 4> predefined_uniforms;
			predefined_uniforms.fill(0.5f);
			predefined_uniforms[0] = m_renderer->GetTime();
			predefined_uniforms[1] = renderState.Maginification;

			// vs
			int32_t vsOffset = 0;
			m_renderer->SetVertexBufferToShader(&mstCamera, sizeof(Effekseer::Matrix44), vsOffset);
			vsOffset += sizeof(Effekseer::Matrix44);

			m_renderer->SetVertexBufferToShader(&mstProj, sizeof(Effekseer::Matrix44), vsOffset);
			vsOffset += sizeof(Effekseer::Matrix44);

			m_renderer->SetVertexBufferToShader(uvInversedMaterial.data(), sizeof(float) * 4, vsOffset);
			vsOffset += (sizeof(float) * 4);

			m_renderer->SetVertexBufferToShader(predefined_uniforms.data(), sizeof(float) * 4, vsOffset);
			vsOffset += (sizeof(float) * 4);

			m_renderer->SetVertexBufferToShader(cameraPosition, sizeof(float) * 4, vsOffset);
			vsOffset += (sizeof(float) * 4);

			for (size_t i = 0; i < renderState.MaterialUniformCount; i++)
			{
				m_renderer->SetVertexBufferToShader(renderState.MaterialUniforms[i].data(), sizeof(float) * 4, vsOffset);
				vsOffset += (sizeof(float) * 4);
			}

			// ps
			int32_t psOffset = 0;
			m_renderer->SetPixelBufferToShader(uvInversedMaterial.data(), sizeof(float) * 4, psOffset);
			psOffset += (sizeof(float) * 4);

			m_renderer->SetPixelBufferToShader(predefined_uniforms.data(), sizeof(float) * 4, psOffset);
			psOffset += (sizeof(float) * 4);

			m_renderer->SetPixelBufferToShader(cameraPosition, sizeof(float) * 4, psOffset);
			psOffset += (sizeof(float) * 4);

			SoftParticleParameter softParticleParam;

			softParticleParam.SetParam(
				0.0f,
				0.0f,
				0.0f,
				0.0f,
				reconstructionParam.DepthBufferScale,
				reconstructionParam.DepthBufferOffset,
				reconstructionParam.ProjectionMatrix33,
				reconstructionParam.ProjectionMatrix34,
				reconstructionParam.ProjectionMatrix43,
				reconstructionParam.ProjectionMatrix44);

			m_renderer->SetPixelBufferToShader(softParticleParam.reconstructionParam1.data(), sizeof(float) * 4, psOffset);
			psOffset += (sizeof(float) * 4);

			m_renderer->SetPixelBufferToShader(softParticleParam.reconstructionParam2.data(), sizeof(float) * 4, psOffset);
			psOffset += (sizeof(float) * 4);

			// shader model
			if (renderState.Collector.MaterialDataPtr->ShadingModel == ::Effekseer::ShadingModelType::Lit)
			{

				float lightDirection[4];
				float lightColor[4];
				float lightAmbientColor[4];

				::Effekseer::SIMD::Vec3f lightDirection3 = m_renderer->GetLightDirection();
				lightDirection3 = lightDirection3.Normalize();
				VectorToFloat4(lightDirection3, lightDirection);
				ColorToFloat4(m_renderer->GetLightColor(), lightColor);
				ColorToFloat4(m_renderer->GetLightAmbientColor(), lightAmbientColor);

				m_renderer->SetPixelBufferToShader(lightDirection, sizeof(float) * 4, psOffset);
				psOffset += (sizeof(float) * 4);

				m_renderer->SetPixelBufferToShader(lightColor, sizeof(float) * 4, psOffset);
				psOffset += (sizeof(float) * 4);

				m_renderer->SetPixelBufferToShader(lightAmbientColor, sizeof(float) * 4, psOffset);
				psOffset += (sizeof(float) * 4);
			}

			// refraction
			if (renderState.Collector.MaterialDataPtr->RefractionUserPtr != nullptr && renderPass == 0)
			{
				auto mat = m_renderer->GetCameraMatrix();
				m_renderer->SetPixelBufferToShader(&mat, sizeof(float) * 16, psOffset);
				psOffset += (sizeof(float) * 16);
			}

			for (size_t i = 0; i < renderState.MaterialUniformCount; i++)
			{
				m_renderer->SetPixelBufferToShader(renderState.MaterialUniforms[i].data(), sizeof(float) * 4, psOffset);
				psOffset += (sizeof(float) * 4);
			}
		}
		else if (renderState.MaterialType == ::Effekseer::RendererMaterialType::Lighting)
		{
			StandardRendererVertexBuffer vcb;
			vcb.constantVSBuffer[0] = ToStruct(mCamera);
			vcb.constantVSBuffer[1] = ToStruct(mCamera * mProj);
			vcb.uvInversed[0] = uvInversed[0];
			vcb.uvInversed[1] = uvInversed[1];

			vcb.flipbookParameter.enableInterpolation = static_cast<float>(renderState.EnableInterpolation);
			vcb.flipbookParameter.loopType = static_cast<float>(renderState.UVLoopType);
			vcb.flipbookParameter.divideX = static_cast<float>(renderState.FlipbookDivideX);
			vcb.flipbookParameter.divideY = static_cast<float>(renderState.FlipbookDivideY);

			m_renderer->SetVertexBufferToShader(&vcb, sizeof(StandardRendererVertexBuffer), 0);

			// ps
			PixelConstantBuffer pcb{};

			pcb.FalloffParam.Enable = 0;

			auto lightDirection3 = m_renderer->GetLightDirection();
			Effekseer::Vector3D::Normal(lightDirection3, lightDirection3);
			pcb.LightDirection = lightDirection3.ToFloat4();
			pcb.LightColor = m_renderer->GetLightColor().ToFloat4();
			pcb.LightAmbientColor = m_renderer->GetLightAmbientColor().ToFloat4();

			pcb.FlipbookParam.EnableInterpolation = static_cast<float>(renderState.EnableInterpolation);
			pcb.FlipbookParam.InterpolationType = static_cast<float>(renderState.InterpolationType);

			pcb.UVDistortionParam.Intensity = renderState.UVDistortionIntensity;
			pcb.UVDistortionParam.BlendIntensity = renderState.BlendUVDistortionIntensity;
			pcb.UVDistortionParam.UVInversed[0] = uvInversed[0];
			pcb.UVDistortionParam.UVInversed[1] = uvInversed[1];

			pcb.BlendTextureParam.BlendType = static_cast<float>(renderState.TextureBlendType);

			pcb.EmmisiveParam.EmissiveScaling = renderState.EmissiveScaling;

			pcb.EdgeParam.EdgeColor = Effekseer::Color(renderState.EdgeColor[0], renderState.EdgeColor[1], renderState.EdgeColor[2], renderState.EdgeColor[3]).ToFloat4();
			pcb.EdgeParam.Threshold = renderState.EdgeThreshold;
			pcb.EdgeParam.ColorScaling = static_cast<float>(renderState.EdgeColorScaling);

			pcb.SoftParticleParam.SetParam(
				renderState.SoftParticleDistanceFar,
				renderState.SoftParticleDistanceNear,
				renderState.SoftParticleDistanceNearOffset,
				renderState.Maginification,
				reconstructionParam.DepthBufferScale,
				reconstructionParam.DepthBufferOffset,
				reconstructionParam.ProjectionMatrix33,
				reconstructionParam.ProjectionMatrix34,
				reconstructionParam.ProjectionMatrix43,
				reconstructionParam.ProjectionMatrix44);

			pcb.UVInversedBack[0] = uvInversedBack[0];
			pcb.UVInversedBack[1] = uvInversedBack[1];

			m_renderer->SetPixelBufferToShader(&pcb, sizeof(PixelConstantBuffer), 0);
		}
		else
		{
			StandardRendererVertexBuffer vcb;
			vcb.constantVSBuffer[0] = ToStruct(mCamera);
			vcb.constantVSBuffer[1] = ToStruct(mCamera * mProj);
			vcb.uvInversed[0] = uvInversed[0];
			vcb.uvInversed[1] = uvInversed[1];
			vcb.uvInversed[2] = 0.0f;
			vcb.uvInversed[3] = 0.0f;

			vcb.flipbookParameter.enableInterpolation = static_cast<float>(renderState.EnableInterpolation);
			vcb.flipbookParameter.loopType = static_cast<float>(renderState.UVLoopType);
			vcb.flipbookParameter.divideX = static_cast<float>(renderState.FlipbookDivideX);
			vcb.flipbookParameter.divideY = static_cast<float>(renderState.FlipbookDivideY);

			m_renderer->SetVertexBufferToShader(&vcb, sizeof(StandardRendererVertexBuffer), 0);

			if (distortion)
			{
				PixelConstantBufferDistortion pcb;
				pcb.DistortionIntencity[0] = renderState.DistortionIntensity;
				pcb.UVInversedBack[0] = uvInversedBack[0];
				pcb.UVInversedBack[1] = uvInversedBack[1];

				pcb.FlipbookParam.EnableInterpolation = static_cast<float>(renderState.EnableInterpolation);
				pcb.FlipbookParam.InterpolationType = static_cast<float>(renderState.InterpolationType);

				pcb.UVDistortionParam.Intensity = renderState.UVDistortionIntensity;
				pcb.UVDistortionParam.BlendIntensity = renderState.BlendUVDistortionIntensity;
				pcb.UVDistortionParam.UVInversed[0] = uvInversed[0];
				pcb.UVDistortionParam.UVInversed[1] = uvInversed[1];

				pcb.BlendTextureParam.BlendType = static_cast<float>(renderState.TextureBlendType);

				pcb.SoftParticleParam.SetParam(
					renderState.SoftParticleDistanceFar,
					renderState.SoftParticleDistanceNear,
					renderState.SoftParticleDistanceNearOffset,
					renderState.Maginification,
					reconstructionParam.DepthBufferScale,
					reconstructionParam.DepthBufferOffset,
					reconstructionParam.ProjectionMatrix33,
					reconstructionParam.ProjectionMatrix34,
					reconstructionParam.ProjectionMatrix43,
					reconstructionParam.ProjectionMatrix44);

				m_renderer->SetPixelBufferToShader(&pcb, sizeof(PixelConstantBufferDistortion), 0);
			}
			else
			{
				PixelConstantBuffer pcb;
				pcb.FalloffParam.Enable = 0;
				pcb.FlipbookParam.EnableInterpolation = static_cast<float>(renderState.EnableInterpolation);
				pcb.FlipbookParam.InterpolationType = static_cast<float>(renderState.InterpolationType);

				pcb.UVDistortionParam.Intensity = renderState.UVDistortionIntensity;
				pcb.UVDistortionParam.BlendIntensity = renderState.BlendUVDistortionIntensity;
				pcb.UVDistortionParam.UVInversed[0] = uvInversed[0];
				pcb.UVDistortionParam.UVInversed[1] = uvInversed[1];

				pcb.BlendTextureParam.BlendType = static_cast<float>(renderState.TextureBlendType);

				pcb.EmmisiveParam.EmissiveScaling = renderState.EmissiveScaling;

				pcb.EdgeParam.EdgeColor = Effekseer::Color(renderState.EdgeColor[0], renderState.EdgeColor[1], renderState.EdgeColor[2], renderState.EdgeColor[3]).ToFloat4();
				pcb.EdgeParam.Threshold = renderState.EdgeThreshold;
				pcb.EdgeParam.ColorScaling = static_cast<float>(renderState.EdgeColorScaling);

				pcb.SoftParticleParam.SetParam(
					renderState.SoftParticleDistanceFar,
					renderState.SoftParticleDistanceNear,
					renderState.SoftParticleDistanceNearOffset,
					renderState.Maginification,
					reconstructionParam.DepthBufferScale,
					reconstructionParam.DepthBufferOffset,
					reconstructionParam.ProjectionMatrix33,
					reconstructionParam.ProjectionMatrix34,
					reconstructionParam.ProjectionMatrix43,
					reconstructionParam.ProjectionMatrix44);

				pcb.UVInversedBack[0] = uvInversedBack[0];
				pcb.UVInversedBack[1] = uvInversedBack[1];

				m_renderer->SetPixelBufferToShader(&pcb, sizeof(PixelConstantBuffer), 0);
			}
		}

		shader_->SetConstantBuffer();

		m_renderer->GetRenderState()->Update(distortion);

		assert(vbOffset % stride == 0);

		m_renderer->SetVertexBuffer(m_renderer->GetVertexBuffer(), stride);
		m_renderer->SetIndexBuffer(m_renderer->GetIndexBuffer());
		m_renderer->SetLayout(shader_);
		m_renderer->GetImpl()->CurrentRenderingUserData = renderState.RenderingUserData;
		m_renderer->GetImpl()->CurrentHandleUserData = renderState.HandleUserData;
		m_renderer->DrawSprites(bufferSize / stride / 4, vbOffset / stride);

		m_renderer->EndShader(shader_);

		m_renderer->GetRenderState()->Pop();
	}
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRenderer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_STANDARD_RENDERER_H__
