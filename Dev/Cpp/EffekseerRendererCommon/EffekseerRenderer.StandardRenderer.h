
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

enum class StandardRendererShaderType
{
	Unlit,
	Lit,
	BackDistortion,
	AdvancedUnlit,
	AdvancedLit,
	AdvancedBackDistortion,
};

struct StandardRendererState
{
	bool DepthTest;
	bool DepthWrite;
	bool Distortion;
	float DistortionIntensity;
	bool Wireframe;
	bool Refraction;

	::Effekseer::AlphaBlendType AlphaBlend;
	::Effekseer::CullingType CullingType;
	::Effekseer::TextureFilterType TextureFilter1;
	::Effekseer::TextureWrapType TextureWrap1;
	::Effekseer::TextureFilterType TextureFilter2;
	::Effekseer::TextureWrapType TextureWrap2;
	::Effekseer::TextureFilterType TextureFilter3;
	::Effekseer::TextureWrapType TextureWrap3;
	::Effekseer::TextureFilterType TextureFilter4;
	::Effekseer::TextureWrapType TextureWrap4;
	::Effekseer::TextureFilterType TextureFilter5;
	::Effekseer::TextureWrapType TextureWrap5;
	::Effekseer::TextureFilterType TextureFilter6;
	::Effekseer::TextureWrapType TextureWrap6;
	::Effekseer::TextureFilterType TextureFilter7;
	::Effekseer::TextureWrapType TextureWrap7;

	::Effekseer::TextureData* TexturePtr;
	::Effekseer::TextureData* NormalTexturePtr;
	::Effekseer::TextureData* AlphaTexturePtr;
	::Effekseer::TextureData* UVDistortionTexturePtr;
	::Effekseer::TextureData* BlendTexturePtr;
	::Effekseer::TextureData* BlendAlphaTexturePtr;
	::Effekseer::TextureData* BlendUVDistortionTexturePtr;

	int32_t EnableInterpolation;
	int32_t UVLoopType;
	int32_t InterpolationType;
	int32_t FlipbookDivideX;
	int32_t FlipbookDivideY;

	float UVDistortionIntensity;

	int32_t TextureBlendType;

	float BlendUVDistortionIntensity;

	int32_t EmissiveScaling;

	float EdgeThreshold;
	uint8_t EdgeColor[4];
	int32_t EdgeColorScaling;
	bool IsAlphaCuttoffEnabled = false;

	::Effekseer::RendererMaterialType MaterialType;
	::Effekseer::MaterialData* MaterialPtr;
	int32_t MaterialUniformCount = 0;
	std::array<std::array<float, 4>, 16> MaterialUniforms;
	int32_t MaterialTextureCount = 0;
	std::array<::Effekseer::TextureData*, 16> MaterialTextures;
	int32_t CustomData1Count = 0;
	int32_t CustomData2Count = 0;

	StandardRendererState()
	{
		DepthTest = false;
		DepthWrite = false;
		Distortion = false;
		DistortionIntensity = 1.0f;
		Wireframe = true;
		Refraction = false;

		AlphaBlend = ::Effekseer::AlphaBlendType::Blend;
		CullingType = ::Effekseer::CullingType::Front;
		TextureFilter1 = ::Effekseer::TextureFilterType::Nearest;
		TextureWrap1 = ::Effekseer::TextureWrapType::Repeat;
		TextureFilter2 = ::Effekseer::TextureFilterType::Nearest;
		TextureWrap2 = ::Effekseer::TextureWrapType::Repeat;
		TextureFilter3 = ::Effekseer::TextureFilterType::Nearest;
		TextureWrap3 = ::Effekseer::TextureWrapType::Repeat;
		TextureFilter4 = ::Effekseer::TextureFilterType::Nearest;
		TextureWrap4 = ::Effekseer::TextureWrapType::Repeat;
		TextureFilter5 = ::Effekseer::TextureFilterType::Nearest;
		TextureWrap5 = ::Effekseer::TextureWrapType::Repeat;
		TextureFilter6 = ::Effekseer::TextureFilterType::Nearest;
		TextureWrap6 = ::Effekseer::TextureWrapType::Repeat;
		TextureFilter7 = ::Effekseer::TextureFilterType::Nearest;
		TextureWrap7 = ::Effekseer::TextureWrapType::Repeat;

		TexturePtr = nullptr;
		NormalTexturePtr = nullptr;
		AlphaTexturePtr = nullptr;
		UVDistortionTexturePtr = nullptr;
		BlendTexturePtr = nullptr;
		BlendAlphaTexturePtr = nullptr;
		BlendUVDistortionTexturePtr = nullptr;

		EnableInterpolation = 0;
		UVLoopType = 0;
		InterpolationType = 0;
		FlipbookDivideX = 0;
		FlipbookDivideY = 0;

		UVDistortionIntensity = 1.0f;

		TextureBlendType = 0;

		BlendUVDistortionIntensity = 1.0f;

		EmissiveScaling = 1;

		EdgeThreshold = 0.0f;
		EdgeColor[0] = EdgeColor[1] = EdgeColor[2] = EdgeColor[3] = 0;
		EdgeColorScaling = 1;

		MaterialPtr = nullptr;

		MaterialType = ::Effekseer::RendererMaterialType::Default;
		MaterialPtr = nullptr;
		MaterialUniformCount = 0;
		MaterialTextureCount = 0;
		CustomData1Count = 0;
		CustomData2Count = 0;
	}

	bool IsAdvanced() const
	{
		// TODO : merge with IsAdvanced in ModelRenderer
		if (MaterialType == ::Effekseer::RendererMaterialType::File)
		{
			return false;
		}

		if (AlphaTexturePtr != nullptr && AlphaTexturePtr != reinterpret_cast<void*>(0x1))
			return true;

		if (UVDistortionTexturePtr != nullptr && UVDistortionTexturePtr != reinterpret_cast<void*>(0x1))
			return true;

		if (BlendTexturePtr != nullptr && BlendTexturePtr != reinterpret_cast<void*>(0x1))
			return true;

		if (BlendAlphaTexturePtr != nullptr && BlendAlphaTexturePtr != reinterpret_cast<void*>(0x1))
			return true;

		if (BlendUVDistortionTexturePtr != nullptr && BlendUVDistortionTexturePtr != reinterpret_cast<void*>(0x1))
			return true;

		if (EnableInterpolation != 0 ||
			TextureBlendType != -1 ||
			EdgeThreshold != 0 ||
			EmissiveScaling != 1.0f ||
			IsAlphaCuttoffEnabled)
		{
			return true;
		}

		return false;
	}

	bool operator!=(const StandardRendererState state)
	{
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
		if (TextureFilter1 != state.TextureFilter1)
			return true;
		if (TextureWrap1 != state.TextureWrap1)
			return true;
		if (TextureFilter2 != state.TextureFilter2)
			return true;
		if (TextureWrap2 != state.TextureWrap2)
			return true;
		if (TextureFilter3 != state.TextureFilter3)
			return true;
		if (TextureWrap3 != state.TextureWrap3)
			return true;
		if (TextureFilter4 != state.TextureFilter4)
			return true;
		if (TextureWrap4 != state.TextureWrap4)
			return true;
		if (TextureFilter5 != state.TextureFilter5)
			return true;
		if (TextureWrap5 != state.TextureWrap5)
			return true;
		if (TextureFilter6 != state.TextureFilter6)
			return true;
		if (TextureWrap6 != state.TextureWrap6)
			return true;
		if (TextureFilter7 != state.TextureFilter7)
			return true;
		if (TextureWrap7 != state.TextureWrap7)
			return true;
		if (TexturePtr != state.TexturePtr)
			return true;
		if (NormalTexturePtr != state.NormalTexturePtr)
			return true;
		if (AlphaTexturePtr != state.AlphaTexturePtr)
			return true;
		if (UVDistortionTexturePtr != state.UVDistortionTexturePtr)
			return true;
		if (BlendTexturePtr != state.BlendTexturePtr)
			return true;
		if (BlendAlphaTexturePtr != state.BlendAlphaTexturePtr)
			return true;
		if (BlendUVDistortionTexturePtr != state.BlendUVDistortionTexturePtr)
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
		if (MaterialType != state.MaterialType)
			return true;
		if (MaterialPtr != state.MaterialPtr)
			return true;
		if (MaterialUniformCount != state.MaterialUniformCount)
			return true;
		if (MaterialTextureCount != state.MaterialTextureCount)
			return true;
		if (Refraction != state.Refraction)
			return true;

		for (int32_t i = 0; i < state.MaterialUniformCount; i++)
		{
			if (MaterialUniforms[i] != state.MaterialUniforms[i])
				return true;
		}

		for (int32_t i = 0; i < state.MaterialTextureCount; i++)
		{
			if (MaterialTextures[i] != state.MaterialTextures[i])
				return true;
		}

		if (CustomData1Count != state.CustomData1Count)
			return true;

		if (CustomData1Count != state.CustomData1Count)
			return true;

		return false;
	}

	void CopyMaterialFromParameterToState(Effekseer::Effect* effect,
										  Effekseer::MaterialParameter* materialParam,
										  int32_t colorTextureIndex,
										  int32_t texture2Index
										  ,
										  int32_t texture3Index,
										  int32_t texture4Index,
										  int32_t texture5Index,
										  int32_t texture6Index,
										  int32_t texture7Index
	)
	{
		if (materialParam != nullptr)
		{
			if (materialParam->MaterialIndex >= 0 && effect->GetMaterial(materialParam->MaterialIndex) != nullptr)
			{
				MaterialPtr = effect->GetMaterial(materialParam->MaterialIndex);

				CustomData1Count = MaterialPtr->CustomData1;
				CustomData2Count = MaterialPtr->CustomData2;

				MaterialUniformCount =
					static_cast<int32_t>(Effekseer::Min(materialParam->MaterialUniforms.size(), MaterialUniforms.size()));
				for (size_t i = 0; i < MaterialUniformCount; i++)
				{
					MaterialUniforms[i] = materialParam->MaterialUniforms[i];
				}

				MaterialTextureCount =
					static_cast<int32_t>(Effekseer::Min(materialParam->MaterialTextures.size(), MaterialTextures.size()));
				for (size_t i = 0; i < MaterialTextureCount; i++)
				{
					if (materialParam->MaterialTextures[i].Type == 1)
					{
						if (materialParam->MaterialTextures[i].Index >= 0)
						{
							MaterialTextures[i] = effect->GetNormalImage(materialParam->MaterialTextures[i].Index);
						}
						else
						{
							MaterialTextures[i] = nullptr;
						}
					}
					else
					{
						if (materialParam->MaterialTextures[i].Index >= 0)
						{
							MaterialTextures[i] = effect->GetColorImage(materialParam->MaterialTextures[i].Index);
						}
						else
						{
							MaterialTextures[i] = nullptr;
						}
					}
				}
			}
		}
		else
		{
			if (colorTextureIndex >= 0)
			{
				if (Distortion)
				{
					TexturePtr = effect->GetDistortionImage(colorTextureIndex);
				}
				else
				{
					TexturePtr = effect->GetColorImage(colorTextureIndex);
				}
			}
			else
			{
				TexturePtr = nullptr;
			}

			if (texture2Index >= 0)
			{
				NormalTexturePtr = effect->GetNormalImage(texture2Index);
			}
			else
			{
				NormalTexturePtr = nullptr;
			}

			if (texture3Index >= 0)
			{
				if (Distortion)
				{
					AlphaTexturePtr = effect->GetDistortionImage(texture3Index);
				}
				else
				{
					AlphaTexturePtr = effect->GetColorImage(texture3Index);
				}
			}
			else
			{
				AlphaTexturePtr = nullptr;
			}

			if (texture4Index >= 0)
			{
				if (Distortion)
				{
					UVDistortionTexturePtr = effect->GetDistortionImage(texture4Index);
				}
				else
				{
					UVDistortionTexturePtr = effect->GetColorImage(texture4Index);
				}
			}
			else
			{
				UVDistortionTexturePtr = nullptr;
			}

			if (texture5Index >= 0)
			{
				if (Distortion)
				{
					BlendTexturePtr = effect->GetDistortionImage(texture5Index);
				}
				else
				{
					BlendTexturePtr = effect->GetColorImage(texture5Index);
				}
			}
			else
			{
				BlendTexturePtr = nullptr;
			}

			if (texture6Index >= 0)
			{
				if (Distortion)
				{
					BlendAlphaTexturePtr = effect->GetDistortionImage(texture6Index);
				}
				else
				{
					BlendAlphaTexturePtr = effect->GetColorImage(texture6Index);
				}
			}
			else
			{
				BlendAlphaTexturePtr = nullptr;
			}

			if (texture7Index >= 0)
			{
				if (Distortion)
				{
					BlendUVDistortionTexturePtr = effect->GetDistortionImage(texture7Index);
				}
				else
				{
					BlendUVDistortionTexturePtr = effect->GetColorImage(texture7Index);
				}
			}
			else
			{
				BlendUVDistortionTexturePtr = nullptr;
			}

			Refraction = false;
			CustomData1Count = 0;
			CustomData2Count = 0;
		}
	}
};

template <typename RENDERER, typename SHADER>
class StandardRenderer
{
private:
	enum class RenderingMode
	{
		Unlit,
		Lit,
		BackDistortion,
		AdvancedUnlit,
		AdvancedLit,
		AdvancedBackDistortion,
		Material,
	};

	RENDERER* m_renderer;

	Effekseer::TextureData* m_texture;

	StandardRendererState m_state;

	std::vector<uint8_t> vertexCaches;
	int32_t squareMaxSize_ = 0;
	RenderingMode renderingMode_ = RenderingMode::Unlit;

	struct VertexConstantBuffer
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

	struct PixelConstantBuffer
	{
		struct
		{
			union
			{
				float Buffer[4];

				struct
				{
					float enableInterpolation;
					float interpolationType;
				};
			};
		} flipbookParameter;

		struct
		{
			union
			{
				float Buffer[4];

				struct
				{
					float intensity;
					float blendIntensity;
					float uvInversed[2];
				};
			};
		} uvDistortionParameter;

		struct
		{
			union
			{
				float Buffer[4];

				struct
				{
					float blendType;
				};
			};
		} blendTextureParameter;

		struct
		{
			union
			{
				float Buffer[4];

				struct
				{
					float emissiveScaling;
				};
			};
		};

		struct
		{
			float Color[4];

			union
			{
				float Buffer[4];

				struct
				{
					float Threshold;
					float ColorScaling;
				};
			};
		} edgeParameter;
	};

	struct DistortionPixelConstantBuffer
	{
		float scale[4];
		float uvInversed[4];

		struct
		{
			union
			{
				float Buffer[4];

				struct
				{
					float enableInterpolation;
					float interpolationType;
				};
			};
		} flipbookParameter;

		struct
		{
			union
			{
				float Buffer[4];

				struct
				{
					float intensity;
					float blendIntensity;
					float uvInversed[2];
				};
			};
		} uvDistortionParameter;

		struct
		{
			union
			{
				float Buffer[4];

				struct
				{
					float blendType;
				};
			};
		} blendTextureParameter;
	};

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

public:
	StandardRenderer(RENDERER* renderer)
		: squareMaxSize_(renderer->GetSquareMaxCount())
	{
		m_renderer = renderer;
		vertexCaches.reserve(m_renderer->GetVertexBuffer()->GetMaxSize());
	}

	virtual ~StandardRenderer()
	{
	}

	int32_t CalculateCurrentStride() const
	{
		size_t stride = 0;
		if (renderingMode_ == RenderingMode::Material)
		{
			stride = sizeof(DynamicVertex);
			stride += (m_state.CustomData1Count + m_state.CustomData2Count) * sizeof(float);
		}
		else if (renderingMode_ == RenderingMode::Lit)
		{
			stride = sizeof(LightingVertex);
		}
		else if (renderingMode_ == RenderingMode::BackDistortion)
		{
			stride = sizeof(VertexDistortion);
		}
		else if (renderingMode_ == RenderingMode::Unlit)
		{
			stride = sizeof(SimpleVertex);
		}
		else if (renderingMode_ == RenderingMode::AdvancedLit)
		{
			stride = sizeof(AdvancedLightingVertex);
		}
		else if (renderingMode_ == RenderingMode::AdvancedBackDistortion)
		{
			stride = sizeof(AdvancedVertexDistortion);
		}
		else if (renderingMode_ == RenderingMode::AdvancedUnlit)
		{
			stride = sizeof(AdvancedSimpleVertex);
		}

		return static_cast<int32_t>(stride);
	}

	void UpdateStateAndRenderingIfRequired(StandardRendererState state)
	{
		if (m_state != state)
		{
			Rendering();
		}

		m_state = state;

		bool isAdvanced = m_state.IsAdvanced();

		if (m_state.MaterialPtr != nullptr && !m_state.MaterialPtr->IsSimpleVertex)
		{
			renderingMode_ = RenderingMode::Material;
		}
		else if (m_state.MaterialType == ::Effekseer::RendererMaterialType::Lighting && isAdvanced)
		{
			renderingMode_ = RenderingMode::AdvancedLit;
		}
		else if (m_state.MaterialType == ::Effekseer::RendererMaterialType::BackDistortion && isAdvanced)
		{
			renderingMode_ = RenderingMode::AdvancedBackDistortion;
		}
		else if (m_state.MaterialType == ::Effekseer::RendererMaterialType::Default && isAdvanced)
		{
			renderingMode_ = RenderingMode::AdvancedUnlit;
		}
		else if (m_state.MaterialType == ::Effekseer::RendererMaterialType::Lighting)
		{
			renderingMode_ = RenderingMode::Lit;
		}
		else if (m_state.MaterialType == ::Effekseer::RendererMaterialType::BackDistortion)
		{
			renderingMode_ = RenderingMode::BackDistortion;
		}
		else if (m_state.MaterialType == ::Effekseer::RendererMaterialType::Default)
		{
			renderingMode_ = RenderingMode::Unlit;
		}
		else
		{
			assert(0);
		}
	}

	void BeginRenderingAndRenderingIfRequired(int32_t count, int& stride, void*& data)
	{
		stride = CalculateCurrentStride();

		{
			int32_t renderVertexMaxSize = squareMaxSize_ * stride * 4;

			if (count * stride + (int32_t)vertexCaches.size() > renderVertexMaxSize)
			{
				Rendering();
			}

			auto old = vertexCaches.size();
			vertexCaches.resize(count * stride + vertexCaches.size());
			data = (vertexCaches.data() + old);
		}
	}

	void ResetAndRenderingIfRequired()
	{
		Rendering();

		// It is always initialized with the next drawing.
		m_state.TexturePtr = (Effekseer::TextureData*)0x1;
		m_state.NormalTexturePtr = (Effekseer::TextureData*)0x1;
		m_state.AlphaTexturePtr = (Effekseer::TextureData*)0x1;
		m_state.UVDistortionTexturePtr = (Effekseer::TextureData*)0x1;
		m_state.BlendTexturePtr = (Effekseer::TextureData*)0x1;
		m_state.BlendAlphaTexturePtr = (Effekseer::TextureData*)0x1;
		m_state.BlendUVDistortionTexturePtr = (Effekseer::TextureData*)0x1;
	}

	const StandardRendererState& GetState()
	{
		return m_state;
	}

	void Rendering(const Effekseer::Mat44f& mCamera, const Effekseer::Mat44f& mProj)
	{
		if (vertexCaches.size() == 0)
			return;

		int32_t stride = CalculateCurrentStride();

		int32_t passNum = 1;

		if (m_state.MaterialPtr != nullptr)
		{
			if (m_state.MaterialPtr->RefractionUserPtr != nullptr)
			{
				// refraction and standard
				passNum = 2;
			}
		}

		for (int32_t passInd = 0; passInd < passNum; passInd++)
		{
			int32_t offset = 0;

			while (true)
			{
				// only sprite
				int32_t renderBufferSize = (int32_t)vertexCaches.size() - offset;

				int32_t renderVertexMaxSize = squareMaxSize_ * stride * 4;

				if (renderBufferSize > renderVertexMaxSize)
				{
					renderBufferSize =
						(Effekseer::Min(renderVertexMaxSize, (int32_t)vertexCaches.size() - offset) / (stride * 4)) * (stride * 4);
				}

				Rendering_(mCamera, mProj, offset, renderBufferSize, stride, passInd);

				offset += renderBufferSize;

				if (offset == vertexCaches.size())
					break;
			}
		}

		vertexCaches.clear();
	}

	void Rendering_(const Effekseer::Mat44f& mCamera,
					const Effekseer::Mat44f& mProj,
					int32_t bufferOffset,
					int32_t bufferSize,
					int32_t stride,
					int32_t renderPass)
	{
		bool isBackgroundRequired = false;

		isBackgroundRequired |= m_state.Distortion;
		isBackgroundRequired |= (m_state.MaterialPtr != nullptr && m_state.MaterialPtr->IsRefractionRequired && renderPass == 0);

		if (isBackgroundRequired)
		{
			auto callback = m_renderer->GetDistortingCallback();
			if (callback != nullptr)
			{
				if (!callback->OnDistorting())
				{
					return;
				}
			}
		}

		if (isBackgroundRequired && m_renderer->GetBackground() == 0)
		{
			return;
		}

		int32_t vertexSize = bufferSize;
		int32_t vbOffset = 0;
		{
			VertexBufferBase* vb = m_renderer->GetVertexBuffer();

			void* vbData = nullptr;

			if (vb->RingBufferLock(vertexSize, vbOffset, vbData, stride * 4))
			{
				assert(vbData != nullptr);
				memcpy(vbData, vertexCaches.data() + bufferOffset, vertexSize);
				vb->Unlock();
			}
			else
			{
				return;
			}
		}

		SHADER* shader_ = nullptr;

		bool distortion = m_state.Distortion;
		bool renderDistortedBackground = false;

		if (m_state.MaterialPtr != nullptr)
		{
			if (m_state.MaterialPtr->IsRefractionRequired)
			{
				if (renderPass == 0)
				{
					if (m_renderer->GetBackground() == 0)
					{
						return;
					}

					shader_ = (SHADER*)m_state.MaterialPtr->RefractionUserPtr;
					renderDistortedBackground = true;
				}
				else
				{
					shader_ = (SHADER*)m_state.MaterialPtr->UserPtr;
				}
			}
			else
			{
				shader_ = (SHADER*)m_state.MaterialPtr->UserPtr;
			}

			// validate
			if (shader_ == nullptr)
				return;

			if (m_state.MaterialPtr->UniformCount != m_state.MaterialUniformCount)
				return;

			if (m_state.MaterialPtr->TextureCount != m_state.MaterialTextureCount)
				return;
		}
		else
		{
			auto isAdvanced = m_state.IsAdvanced();

			StandardRendererShaderType type;
			if (m_state.MaterialType == Effekseer::RendererMaterialType::Default && isAdvanced)
			{
				type = StandardRendererShaderType::AdvancedUnlit;
			}
			else if (m_state.MaterialType == Effekseer::RendererMaterialType::Lighting && isAdvanced)
			{
				type = StandardRendererShaderType::AdvancedLit;
			}
			else if (m_state.MaterialType == Effekseer::RendererMaterialType::BackDistortion && isAdvanced)
			{
				type = StandardRendererShaderType::AdvancedBackDistortion;
			}
			else if (m_state.MaterialType == Effekseer::RendererMaterialType::Default)
			{
				type = StandardRendererShaderType::Unlit;
			}
			else if (m_state.MaterialType == Effekseer::RendererMaterialType::Lighting)
			{
				type = StandardRendererShaderType::Lit;
			}
			else if (m_state.MaterialType == Effekseer::RendererMaterialType::BackDistortion)
			{
				type = StandardRendererShaderType::BackDistortion;
			}

			shader_ = m_renderer->GetShader(type);
		}

		RenderStateBase::State& state = m_renderer->GetRenderState()->Push();
		state.DepthTest = m_state.DepthTest;
		state.DepthWrite = m_state.DepthWrite;
		state.CullingType = m_state.CullingType;
		state.AlphaBlend = m_state.AlphaBlend;

		if (renderDistortedBackground)
		{
			state.AlphaBlend = ::Effekseer::AlphaBlendType::Blend;
		}

		m_renderer->BeginShader(shader_);

		if (m_state.MaterialPtr != nullptr)
		{
			std::array<Effekseer::TextureData*, 16> textures;
			textures.fill(nullptr);
			int32_t textureCount = Effekseer::Min(m_state.MaterialTextureCount, textures.size() - 1);

			if (m_state.MaterialTextureCount > 0)
			{
				auto textureSize = static_cast<int32_t>(textures.size());

				for (size_t i = 0; i < Effekseer::Min(m_state.MaterialTextureCount, textureSize); i++)
				{
					textures[i] = m_state.MaterialTextures[i];
					state.TextureFilterTypes[i] = Effekseer::TextureFilterType::Linear;
					state.TextureWrapTypes[i] = m_state.MaterialPtr->TextureWrapTypes[i];
				}
			}

			if (m_renderer->GetBackground() != 0)
			{
				textures[textureCount] = m_renderer->GetBackground();
				state.TextureFilterTypes[textureCount] = Effekseer::TextureFilterType::Linear;
				state.TextureWrapTypes[textureCount] = Effekseer::TextureWrapType::Clamp;
				textureCount += 1;
			}

			m_renderer->SetTextures(shader_, textures.data(), textureCount);
		}
		else
		{
			state.TextureFilterTypes[0] = m_state.TextureFilter1;
			state.TextureWrapTypes[0] = m_state.TextureWrap1;

			if (distortion)
			{
				state.TextureFilterTypes[1] = Effekseer::TextureFilterType::Linear;
				state.TextureWrapTypes[1] = Effekseer::TextureWrapType::Clamp;

				state.TextureFilterTypes[2] = m_state.TextureFilter3;
				state.TextureWrapTypes[2] = m_state.TextureWrap3;

				state.TextureFilterTypes[3] = m_state.TextureFilter4;
				state.TextureWrapTypes[3] = m_state.TextureWrap4;

				state.TextureFilterTypes[4] = m_state.TextureFilter5;
				state.TextureWrapTypes[4] = m_state.TextureWrap5;

				state.TextureFilterTypes[5] = m_state.TextureFilter6;
				state.TextureWrapTypes[5] = m_state.TextureWrap6;

				state.TextureFilterTypes[6] = m_state.TextureFilter7;
				state.TextureWrapTypes[6] = m_state.TextureWrap7;
			}
			else
			{
				if (m_state.MaterialType == ::Effekseer::RendererMaterialType::Lighting)
				{
					state.TextureFilterTypes[1] = m_state.TextureFilter2;
					state.TextureWrapTypes[1] = m_state.TextureWrap2;

					state.TextureFilterTypes[2] = m_state.TextureFilter3;
					state.TextureWrapTypes[2] = m_state.TextureWrap3;

					state.TextureFilterTypes[3] = m_state.TextureFilter4;
					state.TextureWrapTypes[3] = m_state.TextureWrap4;

					state.TextureFilterTypes[4] = m_state.TextureFilter5;
					state.TextureWrapTypes[4] = m_state.TextureWrap5;

					state.TextureFilterTypes[5] = m_state.TextureFilter6;
					state.TextureWrapTypes[5] = m_state.TextureWrap6;

					state.TextureFilterTypes[6] = m_state.TextureFilter7;
					state.TextureWrapTypes[6] = m_state.TextureWrap7;
				}
				else
				{
					state.TextureFilterTypes[1] = m_state.TextureFilter3;
					state.TextureWrapTypes[1] = m_state.TextureWrap3;

					state.TextureFilterTypes[2] = m_state.TextureFilter4;
					state.TextureWrapTypes[2] = m_state.TextureWrap4;

					state.TextureFilterTypes[3] = m_state.TextureFilter5;
					state.TextureWrapTypes[3] = m_state.TextureWrap5;

					state.TextureFilterTypes[4] = m_state.TextureFilter6;
					state.TextureWrapTypes[4] = m_state.TextureWrap6;

					state.TextureFilterTypes[5] = m_state.TextureFilter7;
					state.TextureWrapTypes[5] = m_state.TextureWrap7;
				}
			}

			std::array<Effekseer::TextureData*, 7> textures;
			textures.fill(nullptr);

			if (m_state.TexturePtr != nullptr && m_state.TexturePtr != (Effekseer::TextureData*)0x01 &&
				m_renderer->GetRenderMode() == Effekseer::RenderMode::Normal)
			{
				textures[0] = m_state.TexturePtr;
			}
			else
			{
				textures[0] = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
			}

			if (m_state.MaterialType == ::Effekseer::RendererMaterialType::Lighting)
			{
				textures[1] = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::Normal);

				if (m_state.NormalTexturePtr != nullptr && m_state.NormalTexturePtr != (Effekseer::TextureData*)0x01)
				{
					textures[1] = m_state.NormalTexturePtr;
				}
				if (m_state.AlphaTexturePtr != nullptr && m_state.AlphaTexturePtr != (Effekseer::TextureData*)0x01)
				{
					textures[2] = m_state.AlphaTexturePtr;
				}
				else
				{
					textures[2] = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
				}

				if (m_state.UVDistortionTexturePtr != nullptr && m_state.UVDistortionTexturePtr != (Effekseer::TextureData*)0x01)
				{
					textures[3] = m_state.UVDistortionTexturePtr;
				}
				else
				{
					textures[3] = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::Normal);
				}

				if (m_state.BlendTexturePtr != nullptr && m_state.BlendTexturePtr != (Effekseer::TextureData*)0x01)
				{
					textures[4] = m_state.BlendTexturePtr;
				}
				else
				{
					textures[4] = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
				}

				if (m_state.BlendAlphaTexturePtr != nullptr && m_state.BlendAlphaTexturePtr != (Effekseer::TextureData*)0x01)
				{
					textures[5] = m_state.BlendAlphaTexturePtr;
				}
				else
				{
					textures[5] = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
				}

				if (m_state.BlendUVDistortionTexturePtr != nullptr && m_state.BlendUVDistortionTexturePtr != (Effekseer::TextureData*)0x01)
				{
					textures[6] = m_state.BlendUVDistortionTexturePtr;
				}
				else
				{
					textures[6] = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::Normal);
				}

				m_renderer->SetTextures(shader_, textures.data(), 7);
			}
			else if (distortion)
			{
				textures[1] = m_renderer->GetBackground();

				if (m_state.AlphaTexturePtr != nullptr && m_state.AlphaTexturePtr != (Effekseer::TextureData*)0x01)
				{
					textures[2] = m_state.AlphaTexturePtr;
				}
				else
				{
					textures[2] = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
				}

				if (m_state.UVDistortionTexturePtr != nullptr && m_state.UVDistortionTexturePtr != (Effekseer::TextureData*)0x01)
				{
					textures[3] = m_state.UVDistortionTexturePtr;
				}
				else
				{
					textures[3] = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::Normal);
				}

				if (m_state.BlendTexturePtr != nullptr && m_state.BlendTexturePtr != (Effekseer::TextureData*)0x01)
				{
					textures[4] = m_state.BlendTexturePtr;
				}
				else
				{
					textures[4] = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
				}

				if (m_state.BlendAlphaTexturePtr != nullptr && m_state.BlendAlphaTexturePtr != (Effekseer::TextureData*)0x01)
				{
					textures[5] = m_state.BlendAlphaTexturePtr;
				}
				else
				{
					textures[5] = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
				}

				if (m_state.BlendUVDistortionTexturePtr != nullptr && m_state.BlendUVDistortionTexturePtr != (Effekseer::TextureData*)0x01)
				{
					textures[6] = m_state.BlendUVDistortionTexturePtr;
				}
				else
				{
					textures[6] = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::Normal);
				}

				m_renderer->SetTextures(shader_, textures.data(), 7);
			}
			else
			{
				if (m_state.AlphaTexturePtr != nullptr && m_state.AlphaTexturePtr != (Effekseer::TextureData*)0x01)
				{
					textures[1] = m_state.AlphaTexturePtr;
				}
				else
				{
					textures[1] = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
				}

				if (m_state.UVDistortionTexturePtr != nullptr && m_state.UVDistortionTexturePtr != (Effekseer::TextureData*)0x01)
				{
					textures[2] = m_state.UVDistortionTexturePtr;
				}
				else
				{
					textures[2] = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::Normal);
				}

				if (m_state.BlendTexturePtr != nullptr && m_state.BlendTexturePtr != (Effekseer::TextureData*)0x01)
				{
					textures[3] = m_state.BlendTexturePtr;
				}
				else
				{
					textures[3] = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
				}

				if (m_state.BlendAlphaTexturePtr != nullptr && m_state.BlendAlphaTexturePtr != (Effekseer::TextureData*)0x01)
				{
					textures[4] = m_state.BlendAlphaTexturePtr;
				}
				else
				{
					textures[4] = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::White);
				}

				if (m_state.BlendUVDistortionTexturePtr != nullptr && m_state.BlendUVDistortionTexturePtr != (Effekseer::TextureData*)0x01)
				{
					textures[5] = m_state.BlendUVDistortionTexturePtr;
				}
				else
				{
					textures[5] = m_renderer->GetImpl()->GetProxyTexture(EffekseerRenderer::ProxyTextureType::Normal);
				}

				m_renderer->SetTextures(shader_, textures.data(), 6);
			}
		}

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

		if (m_state.MaterialPtr != nullptr)
		{
			Effekseer::Matrix44 mstCamera = ToStruct(mCamera);
			Effekseer::Matrix44 mstProj = ToStruct(mProj);

			// camera
			float cameraPosition[4];
			::Effekseer::Vec3f cameraPosition3 = m_renderer->GetCameraPosition();
			VectorToFloat4(cameraPosition3, cameraPosition);
			// time
			std::array<float, 4> predefined_uniforms;
			predefined_uniforms.fill(0.5f);
			predefined_uniforms[0] = m_renderer->GetTime();

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

			for (size_t i = 0; i < m_state.MaterialUniformCount; i++)
			{
				m_renderer->SetVertexBufferToShader(m_state.MaterialUniforms[i].data(), sizeof(float) * 4, vsOffset);
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

			// shader model
			if (m_state.MaterialPtr->ShadingModel == ::Effekseer::ShadingModelType::Lit)
			{

				float lightDirection[4];
				float lightColor[4];
				float lightAmbientColor[4];

				::Effekseer::Vec3f lightDirection3 = m_renderer->GetLightDirection();
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
			if (m_state.MaterialPtr->RefractionUserPtr != nullptr && renderPass == 0)
			{
				auto mat = m_renderer->GetCameraMatrix();
				m_renderer->SetPixelBufferToShader(&mat, sizeof(float) * 16, psOffset);
				psOffset += (sizeof(float) * 16);
			}

			for (size_t i = 0; i < m_state.MaterialUniformCount; i++)
			{
				m_renderer->SetPixelBufferToShader(m_state.MaterialUniforms[i].data(), sizeof(float) * 4, psOffset);
				psOffset += (sizeof(float) * 4);
			}
		}
		else if (m_state.MaterialType == ::Effekseer::RendererMaterialType::Lighting)
		{
			VertexConstantBuffer vcb;
			vcb.constantVSBuffer[0] = ToStruct(mCamera);
			vcb.constantVSBuffer[1] = ToStruct(mProj);
			vcb.uvInversed[0] = uvInversed[0];
			vcb.uvInversed[1] = uvInversed[1];

			vcb.flipbookParameter.enableInterpolation = static_cast<float>(m_state.EnableInterpolation);
			vcb.flipbookParameter.loopType = static_cast<float>(m_state.UVLoopType);
			vcb.flipbookParameter.divideX = static_cast<float>(m_state.FlipbookDivideX);
			vcb.flipbookParameter.divideY = static_cast<float>(m_state.FlipbookDivideY);

			m_renderer->SetVertexBufferToShader(&vcb, sizeof(VertexConstantBuffer), 0);

			// ps
			int32_t psOffset = 0;
			float lightDirection[4];
			float lightColor[4];
			float lightAmbientColor[4];

			::Effekseer::Vec3f lightDirection3 = m_renderer->GetLightDirection();
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

			PixelConstantBuffer pcb;
			pcb.flipbookParameter.enableInterpolation = static_cast<float>(m_state.EnableInterpolation);
			pcb.flipbookParameter.interpolationType = static_cast<float>(m_state.InterpolationType);

			pcb.uvDistortionParameter.intensity = m_state.UVDistortionIntensity;
			pcb.uvDistortionParameter.blendIntensity = m_state.BlendUVDistortionIntensity;
			pcb.uvDistortionParameter.uvInversed[0] = uvInversed[0];
			pcb.uvDistortionParameter.uvInversed[1] = uvInversed[1];

			pcb.blendTextureParameter.blendType = static_cast<float>(m_state.TextureBlendType);

			pcb.emissiveScaling = static_cast<float>(m_state.EmissiveScaling);

			ColorToFloat4(Effekseer::Color(m_state.EdgeColor[0], m_state.EdgeColor[1], m_state.EdgeColor[2], m_state.EdgeColor[3]), pcb.edgeParameter.Color);
			pcb.edgeParameter.Threshold = m_state.EdgeThreshold;
			pcb.edgeParameter.ColorScaling = static_cast<float>(m_state.EdgeColorScaling);

			m_renderer->SetPixelBufferToShader(&pcb.flipbookParameter, sizeof(PixelConstantBuffer), psOffset);
		}
		else
		{
			VertexConstantBuffer vcb;
			vcb.constantVSBuffer[0] = ToStruct(mCamera);
			vcb.constantVSBuffer[1] = ToStruct(mProj);
			vcb.uvInversed[0] = uvInversed[0];
			vcb.uvInversed[1] = uvInversed[1];
			vcb.uvInversed[2] = 0.0f;
			vcb.uvInversed[3] = 0.0f;

			vcb.flipbookParameter.enableInterpolation = static_cast<float>(m_state.EnableInterpolation);
			vcb.flipbookParameter.loopType = static_cast<float>(m_state.UVLoopType);
			vcb.flipbookParameter.divideX = static_cast<float>(m_state.FlipbookDivideX);
			vcb.flipbookParameter.divideY = static_cast<float>(m_state.FlipbookDivideY);

			m_renderer->SetVertexBufferToShader(&vcb, sizeof(VertexConstantBuffer), 0);

			if (distortion)
			{
				DistortionPixelConstantBuffer pcb;
				pcb.scale[0] = m_state.DistortionIntensity;
				pcb.uvInversed[0] = uvInversedBack[0];
				pcb.uvInversed[1] = uvInversedBack[1];

				pcb.flipbookParameter.enableInterpolation = static_cast<float>(m_state.EnableInterpolation);
				pcb.flipbookParameter.interpolationType = static_cast<float>(m_state.InterpolationType);

				pcb.uvDistortionParameter.intensity = m_state.UVDistortionIntensity;
				pcb.uvDistortionParameter.blendIntensity = m_state.BlendUVDistortionIntensity;
				pcb.uvDistortionParameter.uvInversed[0] = uvInversed[0];
				pcb.uvDistortionParameter.uvInversed[1] = uvInversed[1];

				pcb.blendTextureParameter.blendType = static_cast<float>(m_state.TextureBlendType);

				m_renderer->SetPixelBufferToShader(&pcb, sizeof(DistortionPixelConstantBuffer), 0);
			}
			else
			{
				PixelConstantBuffer pcb;
				pcb.flipbookParameter.enableInterpolation = static_cast<float>(m_state.EnableInterpolation);
				pcb.flipbookParameter.interpolationType = static_cast<float>(m_state.InterpolationType);

				pcb.uvDistortionParameter.intensity = m_state.UVDistortionIntensity;
				pcb.uvDistortionParameter.blendIntensity = m_state.BlendUVDistortionIntensity;
				pcb.uvDistortionParameter.uvInversed[0] = uvInversed[0];
				pcb.uvDistortionParameter.uvInversed[1] = uvInversed[1];

				pcb.blendTextureParameter.blendType = static_cast<float>(m_state.TextureBlendType);

				pcb.emissiveScaling = static_cast<float>(m_state.EmissiveScaling);

				ColorToFloat4(Effekseer::Color(m_state.EdgeColor[0], m_state.EdgeColor[1], m_state.EdgeColor[2], m_state.EdgeColor[3]), pcb.edgeParameter.Color);
				pcb.edgeParameter.Threshold = m_state.EdgeThreshold;
				pcb.edgeParameter.ColorScaling = static_cast<float>(m_state.EdgeColorScaling);

				m_renderer->SetPixelBufferToShader(&pcb, sizeof(PixelConstantBuffer), 0);
			}
		}

		shader_->SetConstantBuffer();

		m_renderer->GetRenderState()->Update(distortion);

		assert(vbOffset % stride == 0);

		m_renderer->SetVertexBuffer(m_renderer->GetVertexBuffer(), stride);
		m_renderer->SetIndexBuffer(m_renderer->GetIndexBuffer());
		m_renderer->SetLayout(shader_);
		m_renderer->DrawSprites(vertexSize / stride / 4, vbOffset / stride);

		m_renderer->EndShader(shader_);

		m_renderer->GetRenderState()->Pop();
	}

	void Rendering()
	{
		Rendering(m_renderer->GetCameraMatrix(), m_renderer->GetProjectionMatrix());
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
