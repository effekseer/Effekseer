
#ifndef __EFFEKSEERRENDERER_STANDARD_RENDERER_BASE_H__
#define __EFFEKSEERRENDERER_STANDARD_RENDERER_BASE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <Effekseer.h>
#include <vector>

#include "EffekseerRenderer.CommonUtils.h"
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
	::Effekseer::TextureFilterType TextureFilterType;
	::Effekseer::TextureWrapType TextureWrapType;
	::Effekseer::TextureData* TexturePtr;

	::Effekseer::MaterialData* MaterialPtr;
	uint32_t MaterialUniformCount;
	std::array<std::array<float, 4>, 16> MaterialUniforms;
	uint32_t MaterialTextureCount;
	std::array<::Effekseer::TextureData*, 16> MaterialTextures;

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
		TextureFilterType = ::Effekseer::TextureFilterType::Nearest;
		TextureWrapType = ::Effekseer::TextureWrapType::Repeat;
		TexturePtr = nullptr;
		MaterialPtr = nullptr;

		MaterialPtr = nullptr;
		MaterialUniformCount = 0;
		MaterialTextureCount = 0;
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
		if (TextureFilterType != state.TextureFilterType)
			return true;
		if (TextureWrapType != state.TextureWrapType)
			return true;
		if (TexturePtr != state.TexturePtr)
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

		return false;
	}

	void CopyMaterialFromParameterToState(Effekseer::Effect* effect, Effekseer::MaterialParameter* materialParam, int32_t colorTextureIndex)
	{
		if (materialParam != nullptr)
		{
			if (materialParam->MaterialIndex >= 0)
			{
				MaterialPtr = effect->GetMaterial(materialParam->MaterialIndex);

				MaterialUniformCount = Effekseer::Min(materialParam->MaterialUniforms.size(), MaterialUniforms.size());
				for (size_t i = 0; i < MaterialUniformCount; i++)
				{
					MaterialUniforms[i] = materialParam->MaterialUniforms[i];
				}

				MaterialTextureCount = Effekseer::Min(materialParam->MaterialTextures.size(), MaterialTextures.size());
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

			Refraction = false;
		}
	}
};

template <typename RENDERER, typename SHADER, typename VERTEX, typename VERTEX_DISTORTION> class StandardRenderer
{

private:
	RENDERER* m_renderer;
	SHADER* m_shader;
	SHADER* m_shader_no_texture;

	SHADER* m_shader_distortion;
	SHADER* m_shader_no_texture_distortion;

	Effekseer::TextureData* m_texture;

	StandardRendererState m_state;

	std::vector<uint8_t> vertexCaches;
	int32_t renderVertexMaxSize;

	bool m_isDistortionMode;
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
		RENDERER* renderer, SHADER* shader, SHADER* shader_no_texture, SHADER* shader_distortion, SHADER* shader_no_texture_distortion)
		: renderVertexMaxSize(0), m_isDistortionMode(false)
	{
		m_renderer = renderer;
		m_shader = shader;
		m_shader_no_texture = shader_no_texture;
		m_shader_distortion = shader_distortion;
		m_shader_no_texture_distortion = shader_no_texture_distortion;

		vertexCaches.reserve(m_renderer->GetVertexBuffer()->GetMaxSize());
		renderVertexMaxSize = m_renderer->GetVertexBuffer()->GetMaxSize();
	}

	void UpdateStateAndRenderingIfRequired(StandardRendererState state)
	{
		if (m_state != state)
		{
			Rendering();
		}

		m_state = state;

		isDynamicVertexMode_ = m_state.MaterialPtr != nullptr && !m_state.MaterialPtr->IsSimpleVertex;
		m_isDistortionMode = m_state.Distortion;
	}

	void BeginRenderingAndRenderingIfRequired(int32_t count, int32_t& offset, void*& data)
	{
		if (isDynamicVertexMode_)
		{
			if (count * (int32_t)sizeof(DynamicVertex) + (int32_t)vertexCaches.size() > renderVertexMaxSize)
			{
				Rendering();
			}

			auto old = vertexCaches.size();
			vertexCaches.resize(count * sizeof(DynamicVertex) + vertexCaches.size());
			offset = (int32_t)old;
			data = (vertexCaches.data() + old);

		}
		else if (m_isDistortionMode)
		{
			if (count * (int32_t)sizeof(VERTEX_DISTORTION) + (int32_t)vertexCaches.size() > renderVertexMaxSize)
			{
				Rendering();
			}

			auto old = vertexCaches.size();
			vertexCaches.resize(count * sizeof(VERTEX_DISTORTION) + vertexCaches.size());
			offset = (int32_t)old;
			data = (vertexCaches.data() + old);
		}
		else
		{
			if (count * (int32_t)sizeof(VERTEX) + (int32_t)vertexCaches.size() > renderVertexMaxSize)
			{
				Rendering();
			}

			auto old = vertexCaches.size();
			vertexCaches.resize(count * sizeof(VERTEX) + vertexCaches.size());
			offset = (int32_t)old;
			data = (vertexCaches.data() + old);
		}
	}

	void ResetAndRenderingIfRequired()
	{
		Rendering();

		// It is always initialized with the next drawing.
		m_state.TexturePtr = (Effekseer::TextureData*)0x1;
	}

	const StandardRendererState& GetState() { return m_state; }

	void Rendering(const Effekseer::Matrix44& mCamera, const Effekseer::Matrix44& mProj)
	{
		if (vertexCaches.size() == 0)
			return;

		auto vsize = 0;

		if (m_state.MaterialPtr != nullptr && !m_state.MaterialPtr->IsSimpleVertex)
		{
			vsize = sizeof(DynamicVertex);
		}
		else if (m_state.Distortion)
		{
			vsize = sizeof(VERTEX_DISTORTION);
		}
		else
		{
			vsize = sizeof(VERTEX);
		}


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
						(Effekseer::Min(renderVertexMaxSize, (int32_t)vertexCaches.size() - offset) / (vsize * 4)) * (vsize * 4);
				}

				Rendering_(mCamera, mProj, offset, renderBufferSize, vsize, passInd);

				offset += renderBufferSize;

				if (offset == vertexCaches.size())
					break;
			}
		}

		vertexCaches.clear();
	}

	void Rendering_(const Effekseer::Matrix44& mCamera, const Effekseer::Matrix44& mProj, int32_t bufferOffset, int32_t bufferSize, int32_t vertexSize_, int32_t renderPass)
	{
		if (m_state.Distortion)
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

		if (m_state.Distortion && m_renderer->GetBackground() == 0)
		{
			return;
		}

		int32_t vertexSize = bufferSize;
		int32_t offsetSize = 0;
		{
			VertexBufferBase* vb = m_renderer->GetVertexBuffer();

			void* data = nullptr;

			// TODO : improve performance
			if (vertexSize_ != sizeof(VERTEX))
			{
				// For OpenGL ES(Because OpenGL ES 3.2 and later can only realize a vertex layout variable ring buffer)
				vb->Lock();
				data = vb->GetBufferDirect(vertexSize);
				if (data == nullptr)
				{
					return;
				}
				memcpy(data, vertexCaches.data() + bufferOffset, vertexSize);
				vb->Unlock();
			}
			else if (vb->RingBufferLock(vertexSize, offsetSize, data))
			{
				assert(data != nullptr);
				memcpy(data, vertexCaches.data() + bufferOffset, vertexSize);
				vb->Unlock();
			}
			else
			{
				return;
			}
		}

		RenderStateBase::State& state = m_renderer->GetRenderState()->Push();
		state.DepthTest = m_state.DepthTest;
		state.DepthWrite = m_state.DepthWrite;
		state.CullingType = m_state.CullingType;
		state.AlphaBlend = m_state.AlphaBlend;

		SHADER* shader_ = nullptr;

		bool distortion = m_state.Distortion;

		if (m_state.MaterialPtr != nullptr)
		{
			if (m_state.MaterialPtr->RefractionUserPtr != nullptr)
			{
				if (renderPass == 0)
				{
					if (m_renderer->GetBackground() == 0)
					{
						return;
					}

					shader_ = (SHADER*)m_state.MaterialPtr->RefractionUserPtr;
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
			if (m_state.MaterialPtr->UniformCount != m_state.MaterialUniformCount)
				return;

			if (m_state.MaterialPtr->TextureCount != m_state.MaterialTextureCount)
				return;
		}
		else
		{
			shader_ = m_renderer->GetShader(m_state.TexturePtr != nullptr, distortion);
		}

		m_renderer->BeginShader(shader_);

		if (m_state.MaterialPtr != nullptr)
		{
			std::array<Effekseer::TextureData*, 16> textures;

			if (m_state.MaterialTextureCount > 0)
			{
				for (size_t i = 0; i < Effekseer::Min(m_state.MaterialTextureCount, textures.size()); i++)
				{
					textures[i] = m_state.MaterialTextures[i];
				}
			}

			if (m_renderer->GetBackground() == 0)
			{
				textures[m_state.MaterialTextureCount] = m_renderer->GetBackground();
			}

			m_renderer->SetTextures(shader_, textures.data(), Effekseer::Min(m_state.MaterialTextureCount + 1, textures.size()));
		}
		else
		{
			Effekseer::TextureData* textures[2];

			if (m_state.TexturePtr != nullptr && m_state.TexturePtr != (Effekseer::TextureData*)0x01)
			{
				textures[0] = m_state.TexturePtr;
			}
			else
			{
				textures[0] = nullptr;
			}

			if (distortion)
			{
				textures[1] = m_renderer->GetBackground();
				m_renderer->SetTextures(shader_, textures, 2);
			}
			else
			{
				m_renderer->SetTextures(shader_, textures, 1);
			}
		}

		std::array<float, 4> uvInversed;
		std::array<float, 4> uvInversedBack;

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

			m_renderer->SetVertexBufferToShader(uvInversed.data(), sizeof(float) * 4, vsOffset);
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
			m_renderer->SetPixelBufferToShader(uvInversedBack.data(), sizeof(float) * 4, psOffset);
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

			for (size_t i = 0; i < m_state.MaterialUniformCount; i++)
			{
				m_renderer->SetPixelBufferToShader(m_state.MaterialUniforms[i].data(), sizeof(float) * 4, psOffset);
				psOffset += (sizeof(float) * 4);
			}
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
				pcb.uvInversed[0] = uvInversed[0];
				pcb.uvInversed[1] = uvInversed[1];

				m_renderer->SetPixelBufferToShader(&pcb, sizeof(DistortionPixelConstantBuffer), 0);
			}
		}

		shader_->SetConstantBuffer();

		state.TextureFilterTypes[0] = m_state.TextureFilterType;
		state.TextureWrapTypes[0] = m_state.TextureWrapType;

		if (distortion)
		{
			state.TextureFilterTypes[1] = Effekseer::TextureFilterType::Nearest;
			state.TextureWrapTypes[1] = Effekseer::TextureWrapType::Clamp;
		}

		m_renderer->GetRenderState()->Update(distortion);

		m_renderer->SetVertexBuffer(m_renderer->GetVertexBuffer(), vertexSize_);
		m_renderer->SetIndexBuffer(m_renderer->GetIndexBuffer());
		m_renderer->SetLayout(shader_);
		m_renderer->DrawSprites(vertexSize / vertexSize_ / 4, offsetSize / vertexSize_);

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
