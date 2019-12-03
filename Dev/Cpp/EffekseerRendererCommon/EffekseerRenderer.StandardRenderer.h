﻿
#ifndef __EFFEKSEERRENDERER_STANDARD_RENDERER_BASE_H__
#define __EFFEKSEERRENDERER_STANDARD_RENDERER_BASE_H__

#include <Effekseer.h>
#include <vector>
#include <algorithm>
#include <functional>

#include "EffekseerRenderer.CommonUtils.h"
#include "EffekseerRenderer.Renderer.h"
#include "EffekseerRenderer.VertexBufferBase.h"
#include "EffekseerRenderer.Renderer_Impl.h"

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
	bool Wireframe;
	bool Refraction;

	::Effekseer::AlphaBlendType AlphaBlend;
	::Effekseer::CullingType CullingType;
	::Effekseer::TextureFilterType TextureFilter1;
	::Effekseer::TextureWrapType TextureWrap1;
	::Effekseer::TextureFilterType TextureFilter2;
	::Effekseer::TextureWrapType TextureWrap2;
	::Effekseer::TextureData* TexturePtr;
	::Effekseer::TextureData* NormalTexturePtr;

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
		TexturePtr = nullptr;
		NormalTexturePtr = nullptr;

		MaterialPtr = nullptr;

		MaterialType = ::Effekseer::RendererMaterialType::Default; 
		MaterialPtr = nullptr;
		MaterialUniformCount = 0;
		MaterialTextureCount = 0;
		CustomData1Count = 0;
		CustomData2Count = 0;
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
		if (TexturePtr != state.TexturePtr)
			return true;
		if (NormalTexturePtr != state.TexturePtr)
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

	void CopyMaterialFromParameterToState(Effekseer::Effect* effect, Effekseer::MaterialParameter* materialParam, int32_t colorTextureIndex, int32_t texture2Index)
	{
		if (materialParam != nullptr)
		{
			if (materialParam->MaterialIndex >= 0 && effect->GetMaterial(materialParam->MaterialIndex) != nullptr)
			{
				MaterialPtr = effect->GetMaterial(materialParam->MaterialIndex);

				CustomData1Count = MaterialPtr->CustomData1;
				CustomData2Count = MaterialPtr->CustomData2;

				MaterialUniformCount = static_cast<int32_t>(Effekseer::Min(materialParam->MaterialUniforms.size(), MaterialUniforms.size()));
				for (size_t i = 0; i < MaterialUniformCount; i++)
				{
					MaterialUniforms[i] = materialParam->MaterialUniforms[i];
				}

				MaterialTextureCount = static_cast<int32_t>(Effekseer::Min(materialParam->MaterialTextures.size(), MaterialTextures.size()));
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

			Refraction = false;
			CustomData1Count = 0;
			CustomData2Count = 0;
		}
	}
};

template <typename RENDERER, typename SHADER, typename VERTEX, typename VERTEX_DISTORTION> class StandardRenderer
{

private:
	RENDERER* m_renderer;
	SHADER* m_shader;
	//SHADER* m_shader_no_texture;

	SHADER* m_shader_distortion;
	//SHADER* m_shader_no_texture_distortion;

	Effekseer::TextureData* m_texture;

	StandardRendererState m_state;

	std::vector<uint8_t> vertexCaches;
	int32_t renderVertexMaxSize;

	bool isDistortionMode_;
	bool isDynamicVertexMode_ = false;

	struct VertexConstantBuffer
	{
		Effekseer::Matrix44 constantVSBuffer[2];
		float uvInversed[4];
	};

	struct DistortionPixelConstantBuffer
	{
		float scale[4];
		float uvInversed[4];
	};

	
	void ColorToFloat4(::Effekseer::Color color, float fc[4])
	{
		fc[0] = color.R / 255.0f;
		fc[1] = color.G / 255.0f;
		fc[2] = color.B / 255.0f;
		fc[3] = color.A / 255.0f;
	}

	void VectorToFloat4(::Effekseer::Vector3D v, float fc[4])
	{
		fc[0] = v.X;
		fc[1] = v.Y;
		fc[2] = v.Z;
		fc[3] = 1.0f;
	}

public:
	StandardRenderer(
		RENDERER* renderer, SHADER* shader, SHADER* shader_distortion)
		: renderVertexMaxSize(0), isDistortionMode_(false)
	{
		m_renderer = renderer;
		m_shader = shader;
		//m_shader_no_texture = shader_no_texture;
		m_shader_distortion = shader_distortion;
		//m_shader_no_texture_distortion = shader_no_texture_distortion;

		vertexCaches.reserve(m_renderer->GetVertexBuffer()->GetMaxSize());
		renderVertexMaxSize = m_renderer->GetVertexBuffer()->GetMaxSize();
	}

	virtual ~StandardRenderer()
	{
	}

	int32_t CalculateCurrentStride() const
	{
		int32_t stride = 0;
		if (isDynamicVertexMode_)
		{
			stride = (int32_t)sizeof(DynamicVertex);
			stride += (m_state.CustomData1Count + m_state.CustomData2Count) * sizeof(float);
		}
		else if (isDistortionMode_)
		{
			stride = (int32_t)sizeof(VERTEX_DISTORTION);
		}
		else
		{
			stride = (int32_t)sizeof(VERTEX);
		}
		return stride;
	}

	void UpdateStateAndRenderingIfRequired(StandardRendererState state)
	{
		if (m_state != state)
		{
			Rendering();
		}

		m_state = state;

		isDynamicVertexMode_ = (m_state.MaterialPtr != nullptr && !m_state.MaterialPtr->IsSimpleVertex) || 
			m_state.MaterialType == ::Effekseer::RendererMaterialType::Lighting;
		isDistortionMode_ = m_state.Distortion;
	}

	void BeginRenderingAndRenderingIfRequired(int32_t count, int& stride, void*& data)
	{
		stride = CalculateCurrentStride();

		{
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
	}

	const StandardRendererState& GetState() { return m_state; }

	void Rendering(const Effekseer::Matrix44& mCamera, const Effekseer::Matrix44& mProj)
	{
		if (vertexCaches.size() == 0)
			return;

		int32_t stride = CalculateCurrentStride();

		int32_t passNum = 1;

		if (m_state.MaterialPtr != nullptr)
		{
			if(m_state.MaterialPtr->RefractionUserPtr != nullptr)
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

	void Rendering_(const Effekseer::Matrix44& mCamera, const Effekseer::Matrix44& mProj, int32_t bufferOffset, int32_t bufferSize, int32_t stride, int32_t renderPass)
	{
		bool isBackgroundRequired = false;
		
		isBackgroundRequired |= m_state.Distortion;
		isBackgroundRequired |=
			(m_state.MaterialPtr != nullptr && m_state.MaterialPtr->IsRefractionRequired && renderPass == 0);

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
			shader_ = m_renderer->GetShader(true, m_state.MaterialType);
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
			std::array<Effekseer::TextureData*, 2> textures;
			textures.fill(nullptr);

			if (m_state.TexturePtr != nullptr && m_state.TexturePtr != (Effekseer::TextureData*)0x01 && m_renderer->GetRenderMode() == Effekseer::RenderMode::Normal)
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

				m_renderer->SetTextures(shader_, textures.data(), 2);
			}
			else if (distortion)
			{
				textures[1] = m_renderer->GetBackground();
				m_renderer->SetTextures(shader_, textures.data(), 2);
			}
			else
			{
				m_renderer->SetTextures(shader_, textures.data(), 1);
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
			// time
			std::array<float, 4> predefined_uniforms;
			predefined_uniforms.fill(0.5f);
			predefined_uniforms[0] = m_renderer->GetTime();

			// vs
			int32_t vsOffset = 0;
			m_renderer->SetVertexBufferToShader(&mCamera, sizeof(Effekseer::Matrix44), vsOffset);
			vsOffset += sizeof(Effekseer::Matrix44);

			m_renderer->SetVertexBufferToShader(&mProj, sizeof(Effekseer::Matrix44), vsOffset);
			vsOffset += sizeof(Effekseer::Matrix44);

			m_renderer->SetVertexBufferToShader(uvInversedMaterial.data(), sizeof(float) * 4, vsOffset);
			vsOffset += (sizeof(float) * 4);

			m_renderer->SetVertexBufferToShader(predefined_uniforms.data(), sizeof(float) * 4, vsOffset);
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

			// shader model
			if (m_state.MaterialPtr->ShadingModel == ::Effekseer::ShadingModelType::Lit)
			{
				float cameraPosition[4];
				float lightDirection[4];
				float lightColor[4];
				float lightAmbientColor[4];

				::Effekseer::Vector3D cameraPosition3 = m_renderer->GetCameraPosition();
				::Effekseer::Vector3D lightDirection3 = m_renderer->GetLightDirection();
				::Effekseer::Vector3D::Normal(lightDirection3, lightDirection3);
				VectorToFloat4(cameraPosition3, cameraPosition);
				VectorToFloat4(lightDirection3, lightDirection);
				ColorToFloat4(m_renderer->GetLightColor(), lightColor);
				ColorToFloat4(m_renderer->GetLightAmbientColor(), lightAmbientColor);

				m_renderer->SetPixelBufferToShader(cameraPosition, sizeof(float) * 4, psOffset);
				psOffset += (sizeof(float) * 4);

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
			vcb.constantVSBuffer[0] = mCamera;
			vcb.constantVSBuffer[1] = mProj;
			vcb.uvInversed[0] = uvInversed[0];
			vcb.uvInversed[1] = uvInversed[1];

			m_renderer->SetVertexBufferToShader(&vcb, sizeof(VertexConstantBuffer), 0);

			// ps
			int32_t psOffset = 0;
			float lightDirection[4];
			float lightColor[4];
			float lightAmbientColor[4];

			::Effekseer::Vector3D lightDirection3 = m_renderer->GetLightDirection();
			::Effekseer::Vector3D::Normal(lightDirection3, lightDirection3);
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
		else
		{
			VertexConstantBuffer vcb;
			vcb.constantVSBuffer[0] = mCamera;
			vcb.constantVSBuffer[1] = mProj;
			vcb.uvInversed[0] = uvInversed[0];
			vcb.uvInversed[1] = uvInversed[1];

			m_renderer->SetVertexBufferToShader(&vcb, sizeof(VertexConstantBuffer), 0);

			if (distortion)
			{
				DistortionPixelConstantBuffer pcb;
				pcb.scale[0] = m_state.DistortionIntensity;
				pcb.uvInversed[0] = uvInversedBack[0];
				pcb.uvInversed[1] = uvInversedBack[1];

				m_renderer->SetPixelBufferToShader(&pcb, sizeof(DistortionPixelConstantBuffer), 0);
			}
		}

		shader_->SetConstantBuffer();

		if (m_state.MaterialPtr == nullptr)
		{
			state.TextureFilterTypes[0] = m_state.TextureFilter1;
			state.TextureWrapTypes[0] = m_state.TextureWrap1;

			if (distortion)
			{
				state.TextureFilterTypes[1] = Effekseer::TextureFilterType::Linear;
				state.TextureWrapTypes[1] = Effekseer::TextureWrapType::Clamp;
			}
			else
			{
				state.TextureFilterTypes[1] = m_state.TextureFilter2;
				state.TextureWrapTypes[1] = m_state.TextureWrap2;
			}
		}

		m_renderer->GetRenderState()->Update(distortion);

		assert(vbOffset % stride == 0);

		m_renderer->SetVertexBuffer(m_renderer->GetVertexBuffer(), stride);
		m_renderer->SetIndexBuffer(m_renderer->GetIndexBuffer());
		m_renderer->SetLayout(shader_);
		m_renderer->DrawSprites(vertexSize / stride / 4, vbOffset / stride);

		m_renderer->EndShader(shader_);

		m_renderer->GetRenderState()->Pop();
	}

	void Rendering() { Rendering(m_renderer->GetCameraMatrix(), m_renderer->GetProjectionMatrix()); }
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRenderer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_STANDARD_RENDERER_H__
