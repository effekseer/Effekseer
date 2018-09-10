
#ifndef	__EFFEKSEERRENDERER_MODEL_RENDERER_BASE_H__
#define	__EFFEKSEERRENDERER_MODEL_RENDERER_BASE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <Effekseer.h>
#include <assert.h>
#include <string.h>
#include <vector>

#include "EffekseerRenderer.CommonUtils.h"
#include "EffekseerRenderer.RenderStateBase.h"
#include "EffekseerRenderer.VertexBufferBase.h"
#include "EffekseerRenderer.IndexBufferBase.h"

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
typedef ::Effekseer::Vector3D efkVector3D;

template<int MODEL_COUNT>
struct ModelRendererVertexConstantBuffer
{
	Effekseer::Matrix44		CameraMatrix;
	Effekseer::Matrix44		ModelMatrix[MODEL_COUNT];
	float	ModelUV[MODEL_COUNT][4];
	float	ModelColor[MODEL_COUNT][4];

	float	LightDirection[4];
	float	LightColor[4];
	float	LightAmbientColor[4];
};

struct ModelRendererPixelConstantBuffer
{
	float	LightDirection[4];
	float	LightColor[4];
	float	LightAmbientColor[4];
};

class ModelRendererBase
	: public ::Effekseer::ModelRenderer
{
protected:
	std::vector<Effekseer::Matrix44>	m_matrixes;
	std::vector<Effekseer::RectF>		m_uv;
	std::vector<Effekseer::Color>		m_colors;
	std::vector<int32_t>				m_times;

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

	ModelRendererBase()
	{
	}

public:

	virtual ~ModelRendererBase() {}

	template<typename RENDERER>
	void BeginRendering_(RENDERER* renderer, const efkModelNodeParam& parameter, int32_t count, void* userData)
	{
		m_matrixes.clear();
		m_uv.clear();
		m_colors.clear();
		m_times.clear();

		renderer->GetStandardRenderer()->ResetAndRenderingIfRequired();
	}

	template<typename RENDERER>
	void Rendering_(RENDERER* renderer, const efkModelNodeParam& parameter, const efkModelInstanceParam& instanceParameter, void* userData)
	{
		auto camera = renderer->GetCameraMatrix();
		::Effekseer::BillboardType btype = parameter.Billboard;
		Effekseer::Matrix44 mat44;

		if (btype == ::Effekseer::BillboardType::Billboard ||
			btype == ::Effekseer::BillboardType::RotatedBillboard ||
			btype == ::Effekseer::BillboardType::YAxisFixed)
		{
			const ::Effekseer::Matrix43& mat = instanceParameter.SRTMatrix43;
			::Effekseer::Vector3D s;
			::Effekseer::Matrix43 r;
			::Effekseer::Vector3D t;
			mat.GetSRT(s, r, t);

			::Effekseer::Vector3D F;
			::Effekseer::Vector3D R;
			::Effekseer::Vector3D U;

			if (btype == ::Effekseer::BillboardType::Billboard)
			{
				::Effekseer::Vector3D Up(0.0f, 1.0f, 0.0f);

				::Effekseer::Vector3D::Normal(F, -renderer->GetCameraFrontDirection());
				::Effekseer::Vector3D::Normal(R, ::Effekseer::Vector3D::Cross(R, Up, F));
				::Effekseer::Vector3D::Normal(U, ::Effekseer::Vector3D::Cross(U, F, R));
			}
			else if (btype == ::Effekseer::BillboardType::RotatedBillboard)
			{
				::Effekseer::Vector3D Up(0.0f, 1.0f, 0.0f);

				::Effekseer::Vector3D::Normal(F, -renderer->GetCameraFrontDirection());

				::Effekseer::Vector3D::Normal(R, ::Effekseer::Vector3D::Cross(R, Up, F));
				::Effekseer::Vector3D::Normal(U, ::Effekseer::Vector3D::Cross(U, F, R));

				float c_zx = sqrt(1.0f - r.Value[2][1] * r.Value[2][1]);
				float s_z = 0.0f;
				float c_z = 0.0f;

				if (fabsf(c_zx) > 0.05f)
				{
					s_z = -r.Value[0][1] / c_zx;
					c_z = sqrt(1.0f - s_z * s_z);
					if (r.Value[1][1] < 0.0f) c_z = -c_z;
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
			else if (btype == ::Effekseer::BillboardType::YAxisFixed)
			{
				U = ::Effekseer::Vector3D(r.Value[1][0], r.Value[1][1], r.Value[1][2]);

				::Effekseer::Vector3D::Normal(F, -renderer->GetCameraFrontDirection());

				::Effekseer::Vector3D::Normal(R, ::Effekseer::Vector3D::Cross(R, U, F));
				::Effekseer::Vector3D::Normal(F, ::Effekseer::Vector3D::Cross(F, R, U));
			}

			::Effekseer::Matrix43 mat_rot;

			mat_rot.Value[0][0] = -R.X;
			mat_rot.Value[0][1] = -R.Y;
			mat_rot.Value[0][2] = -R.Z;
			mat_rot.Value[1][0] = U.X;
			mat_rot.Value[1][1] = U.Y;
			mat_rot.Value[1][2] = U.Z;
			mat_rot.Value[2][0] = F.X;
			mat_rot.Value[2][1] = F.Y;
			mat_rot.Value[2][2] = F.Z;
			mat_rot.Value[3][0] = t.X;
			mat_rot.Value[3][1] = t.Y;
			mat_rot.Value[3][2] = t.Z;

			::Effekseer::Matrix43 mat_scale;
			mat_scale.Scaling(s.X, s.Y, s.Z);
			::Effekseer::Matrix43::Multiple(mat_rot, mat_scale, mat_rot);

			for (int32_t r_ = 0; r_ < 4; r_++)
			{
				for (int32_t c_ = 0; c_ < 3; c_++)
				{
					mat44.Values[r_][c_] = mat_rot.Value[r_][c_];
				}
			}
		}
		else if (btype == ::Effekseer::BillboardType::Fixed)
		{
			for (int32_t r_ = 0; r_ < 4; r_++)
			{
				for (int32_t c_ = 0; c_ < 3; c_++)
				{
					mat44.Values[r_][c_] = instanceParameter.SRTMatrix43.Value[r_][c_];
				}
			}
		}

		if (parameter.Magnification != 1.0f)
		{
			Effekseer::Matrix44 mat_scale;
			mat_scale.Values[0][0] = parameter.Magnification;
			mat_scale.Values[1][1] = parameter.Magnification;
			mat_scale.Values[2][2] = parameter.Magnification;

			Effekseer::Matrix44::Mul(mat44, mat_scale, mat44);
		}

		if (!parameter.IsRightHand)
		{
			Effekseer::Matrix44 mat_scale;
			mat_scale.Values[0][0] = 1.0f;
			mat_scale.Values[1][1] = 1.0f;
			mat_scale.Values[2][2] = -1.0f;

			Effekseer::Matrix44::Mul(mat44, mat_scale, mat44);
		}

		m_matrixes.push_back(mat44);
		m_uv.push_back(instanceParameter.UV);
		m_colors.push_back(instanceParameter.AllColor);
		m_times.push_back(instanceParameter.Time);
	}

	template<typename RENDERER, typename SHADER, typename MODEL, bool Instancing, int InstanceCount>
	void EndRendering_(
		RENDERER* renderer, 
		SHADER* shader_lighting_texture_normal,
		SHADER* shader_lighting_normal,
		SHADER* shader_lighting_texture,
		SHADER* shader_lighting,
		SHADER* shader_texture,
		SHADER* shader,
		SHADER* shader_distortion_texture,
		SHADER* shader_distortion,
		const efkModelNodeParam& param)
	{
		if (m_matrixes.size() == 0) return;
		if (param.ModelIndex < 0) return;

		auto camera = renderer->GetCameraMatrix();

		MODEL* model = (MODEL*) param.EffectPointer->GetModel(param.ModelIndex);
		if (model == NULL) return;
		
		auto distortion = param.Distortion;
		if (distortion && renderer->GetBackground() == 0) return;

		if (distortion)
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

		RenderStateBase::State& state = renderer->GetRenderState()->Push();
		state.DepthTest = param.ZTest;
		state.DepthWrite = param.ZWrite;
		state.AlphaBlend = param.AlphaBlend;
		state.CullingType = param.Culling;

		/*シェーダー選択*/
		SHADER* shader_ = NULL;
		if (distortion)
		{
			if (param.ColorTextureIndex >= 0)
			{
				shader_ = shader_distortion_texture;
			}
			else
			{
				shader_ = shader_distortion;
			}
		}
		else if (param.Lighting)
		{
			if (param.NormalTextureIndex >= 0)
			{
				if (param.ColorTextureIndex >= 0)
				{
					shader_ = shader_lighting_texture_normal;
				}
				else
				{
					shader_ = shader_lighting_normal;
				}
			}
			else
			{
				if (param.ColorTextureIndex >= 0)
				{
					shader_ = shader_lighting_texture;
				}
				else
				{
					shader_ = shader_lighting;
				}
			}
		}
		else
		{
			if (param.ColorTextureIndex >= 0)
			{
				shader_ = shader_texture;
			}
			else
			{
				shader_ = shader;
			}
		}

		renderer->BeginShader(shader_);

		// Select texture
		Effekseer::TextureData* textures[2];
		textures[0] = nullptr;
		textures[1] = nullptr;

		if (distortion)
		{
			if (param.ColorTextureIndex >= 0)
			{
				textures[0] = param.EffectPointer->GetDistortionImage(param.ColorTextureIndex);
			}

			textures[1] = renderer->GetBackground();
		}
		else
		{
			if (param.ColorTextureIndex >= 0)
			{
				textures[0] = param.EffectPointer->GetColorImage(param.ColorTextureIndex);
			}

			if (param.NormalTextureIndex >= 0)
			{
				textures[1] = param.EffectPointer->GetNormalImage(param.NormalTextureIndex);
			}
		}
		
		
		renderer->SetTextures(shader_, textures, 2);

		state.TextureFilterTypes[0] = param.TextureFilter;
		state.TextureWrapTypes[0] = param.TextureWrap;
		state.TextureFilterTypes[1] = param.TextureFilter;
		state.TextureWrapTypes[1] = param.TextureWrap;

		renderer->GetRenderState()->Update(distortion);

		ModelRendererVertexConstantBuffer<InstanceCount>* vcb = (ModelRendererVertexConstantBuffer<InstanceCount>*)shader_->GetVertexConstantBuffer();

		if (distortion)
		{
			float* pcb = (float*) shader_->GetPixelConstantBuffer();
			pcb[0] = param.DistortionIntensity;
		}
		else
		{
			ModelRendererPixelConstantBuffer* pcb = (ModelRendererPixelConstantBuffer*) shader_->GetPixelConstantBuffer();

			// 固定値設定
			if (param.Lighting)
			{
				{
					::Effekseer::Vector3D lightDirection = renderer->GetLightDirection();
					::Effekseer::Vector3D::Normal(lightDirection, lightDirection);
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
			}
		}
		
		vcb->CameraMatrix = renderer->GetCameraProjectionMatrix();

		// Check time
		auto stTime0 = m_times[0] % model->GetFrameCount();
		auto isTimeSame = true;

		for (auto t : m_times)
		{
			t = t % model->GetFrameCount();
			if(t != stTime0)
			{
				isTimeSame = false;
				break;
			}
		}

		if(Instancing && isTimeSame)
		{
			auto& imodel = model->InternalModels[stTime0];

			// Invalid unless layout is set after buffer
			renderer->SetVertexBuffer(imodel.VertexBuffer, model->GetVertexSize());
			renderer->SetIndexBuffer(imodel.IndexBuffer);
			renderer->SetLayout(shader_);

			for( size_t loop = 0; loop < m_matrixes.size(); )
			{
				int32_t modelCount = Effekseer::Min( m_matrixes.size() - loop, model->ModelCount );
				
				for( int32_t num = 0; num < modelCount; num++ )
				{
					vcb->ModelMatrix[num] = m_matrixes[loop+num];

					// DepthOffset
					ApplyDepthOffset(vcb->ModelMatrix[num], renderer->GetCameraFrontDirection(), renderer->GetCameraPosition(), param.DepthOffset, param.IsDepthOffsetScaledWithCamera, param.IsDepthOffsetScaledWithParticleScale, param.IsRightHand);
	
					vcb->ModelUV[num][0] = m_uv[loop+num].X;
					vcb->ModelUV[num][1] = m_uv[loop+num].Y;
					vcb->ModelUV[num][2] = m_uv[loop+num].Width;
					vcb->ModelUV[num][3] = m_uv[loop+num].Height;

					ColorToFloat4(m_colors[loop+num],vcb->ModelColor[num]);
				}

				shader_->SetConstantBuffer();

				renderer->DrawPolygon(imodel.VertexCount * modelCount, imodel.IndexCount * modelCount);

				loop += modelCount;
			}
		}
		else
		{
			for( size_t loop = 0; loop < m_matrixes.size(); )
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

				// DepthOffset
				ApplyDepthOffset(vcb->ModelMatrix[0], renderer->GetCameraFrontDirection(), renderer->GetCameraPosition(), param.DepthOffset, param.IsDepthOffsetScaledWithCamera, param.IsDepthOffsetScaledWithParticleScale, param.IsRightHand);
				
				ColorToFloat4( m_colors[loop], vcb->ModelColor[0] );
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
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_MODEL_RENDERER_H__