
#ifndef	__EFFEKSEERRENDERER_TRACK_RENDERER_BASE_H__
#define	__EFFEKSEERRENDERER_TRACK_RENDERER_BASE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <Effekseer.h>
#include <Effekseer.Internal.h>
#include <assert.h>
#include <string.h>

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
	typedef ::Effekseer::TrackRenderer::NodeParameter efkTrackNodeParam;
	typedef ::Effekseer::TrackRenderer::InstanceParameter efkTrackInstanceParam;
	typedef ::Effekseer::Vector3D efkVector3D;

	template<typename RENDERER, typename VERTEX_NORMAL, typename VERTEX_DISTORTION>
	class TrackRendererBase
		: public ::Effekseer::TrackRenderer
	{
	protected:
		RENDERER*						m_renderer;
		int32_t							m_ribbonCount;

		int32_t							m_ringBufferOffset;
		uint8_t*						m_ringBufferData;

		efkTrackNodeParam					innstancesNodeParam;
		std::vector<efkTrackInstanceParam>	instances;
		SplineGenerator spline;

		int32_t vertexCount_ = 0;
		int32_t stride_ = 0;

		enum class VertexType
		{
			Normal,
			Distortion,
			Dynamic,
		};

		VertexType GetVertexType(const VERTEX_NORMAL* v) { return VertexType::Normal; }

		VertexType GetVertexType(const VERTEX_DISTORTION* v) { return VertexType::Distortion; }

		VertexType GetVertexType(const DynamicVertex* v) { return VertexType::Dynamic; }

		template <typename VERTEX> void AssignUV(StrideView<VERTEX>& v, float uvX1, float uvX2, float uvX3, float uvY1, float uvY2)
		{
			v[0].UV[0] = uvX1;
			v[0].UV[1] = uvY1;

			v[1].UV[0] = uvX2;
			v[1].UV[1] = uvY1;

			v[4].UV[0] = uvX2;
			v[4].UV[1] = uvY1;

			v[5].UV[0] = uvX3;
			v[5].UV[1] = uvY1;

			v[2].UV[0] = uvX1;
			v[2].UV[1] = uvY2;

			v[3].UV[0] = uvX2;
			v[3].UV[1] = uvY2;

			v[6].UV[0] = uvX2;
			v[6].UV[1] = uvY2;

			v[7].UV[0] = uvX3;
			v[7].UV[1] = uvY2;
		}
		template<typename VERTEX>
		void RenderSplines(const ::Effekseer::Matrix44& camera)
		{
			if (instances.size() == 0)
			{
				return;
			}

			auto& parameter = innstancesNodeParam;

			auto vertexType = GetVertexType((VERTEX*)m_ringBufferData);

			// Calculate spline
			if (parameter.SplineDivision > 1)
			{
				spline.Reset();

				for (size_t loop = 0; loop < instances.size(); loop++)
				{
					auto p = efkVector3D();
					auto& param = instances[loop];

					p.X = param.SRTMatrix43.Value[3][0];
					p.Y = param.SRTMatrix43.Value[3][1];
					p.Z = param.SRTMatrix43.Value[3][2];

					spline.AddVertex(p);
				}

				spline.Calculate();
			}

			StrideView<VERTEX> verteies(m_ringBufferData, stride_, vertexCount_);

			for (size_t loop = 0; loop < instances.size(); loop++)
			{
				auto& param = instances[loop];

				for (int32_t sploop = 0; sploop < parameter.SplineDivision; sploop++)
				{
					bool isFirst = param.InstanceIndex == 0 && sploop == 0;
					bool isLast = param.InstanceIndex == (param.InstanceCount - 1);

					float size = 0.0f;
					::Effekseer::Color leftColor;
					::Effekseer::Color centerColor;
					::Effekseer::Color rightColor;

					float percent = (float)(param.InstanceIndex  * parameter.SplineDivision + sploop) / (float)((param.InstanceCount - 1) * parameter.SplineDivision);

					if (param.InstanceIndex < param.InstanceCount / 2)
					{
						float l = percent;
						l = l * 2.0f;
						size = param.SizeFor + (param.SizeMiddle - param.SizeFor) * l;

						leftColor.R = (uint8_t)Effekseer::Clamp(param.ColorLeft.R + (param.ColorLeftMiddle.R - param.ColorLeft.R) * l, 255, 0);
						leftColor.G = (uint8_t)Effekseer::Clamp(param.ColorLeft.G + (param.ColorLeftMiddle.G - param.ColorLeft.G) * l, 255, 0);
						leftColor.B = (uint8_t)Effekseer::Clamp(param.ColorLeft.B + (param.ColorLeftMiddle.B - param.ColorLeft.B) * l, 255, 0);
						leftColor.A = (uint8_t)Effekseer::Clamp(param.ColorLeft.A + (param.ColorLeftMiddle.A - param.ColorLeft.A) * l, 255, 0);

						centerColor.R = (uint8_t)Effekseer::Clamp(param.ColorCenter.R + (param.ColorCenterMiddle.R - param.ColorCenter.R) * l, 255, 0);
						centerColor.G = (uint8_t)Effekseer::Clamp(param.ColorCenter.G + (param.ColorCenterMiddle.G - param.ColorCenter.G) * l, 255, 0);
						centerColor.B = (uint8_t)Effekseer::Clamp(param.ColorCenter.B + (param.ColorCenterMiddle.B - param.ColorCenter.B) * l, 255, 0);
						centerColor.A = (uint8_t)Effekseer::Clamp(param.ColorCenter.A + (param.ColorCenterMiddle.A - param.ColorCenter.A) * l, 255, 0);

						rightColor.R = (uint8_t)Effekseer::Clamp(param.ColorRight.R + (param.ColorRightMiddle.R - param.ColorRight.R) * l, 255, 0);
						rightColor.G = (uint8_t)Effekseer::Clamp(param.ColorRight.G + (param.ColorRightMiddle.G - param.ColorRight.G) * l, 255, 0);
						rightColor.B = (uint8_t)Effekseer::Clamp(param.ColorRight.B + (param.ColorRightMiddle.B - param.ColorRight.B) * l, 255, 0);
						rightColor.A = (uint8_t)Effekseer::Clamp(param.ColorRight.A + (param.ColorRightMiddle.A - param.ColorRight.A) * l, 255, 0);
					}
					else
					{
						float l = percent;
						l = 1.0f - (l * 2.0f - 1.0f);
						size = param.SizeBack + (param.SizeMiddle - param.SizeBack) * l;

						leftColor.R = (uint8_t)Effekseer::Clamp(param.ColorLeft.R + (param.ColorLeftMiddle.R - param.ColorLeft.R) * l, 255, 0);
						leftColor.G = (uint8_t)Effekseer::Clamp(param.ColorLeft.G + (param.ColorLeftMiddle.G - param.ColorLeft.G) * l, 255, 0);
						leftColor.B = (uint8_t)Effekseer::Clamp(param.ColorLeft.B + (param.ColorLeftMiddle.B - param.ColorLeft.B) * l, 255, 0);
						leftColor.A = (uint8_t)Effekseer::Clamp(param.ColorLeft.A + (param.ColorLeftMiddle.A - param.ColorLeft.A) * l, 255, 0);

						centerColor.R = (uint8_t)Effekseer::Clamp(param.ColorCenter.R + (param.ColorCenterMiddle.R - param.ColorCenter.R) * l, 255, 0);
						centerColor.G = (uint8_t)Effekseer::Clamp(param.ColorCenter.G + (param.ColorCenterMiddle.G - param.ColorCenter.G) * l, 255, 0);
						centerColor.B = (uint8_t)Effekseer::Clamp(param.ColorCenter.B + (param.ColorCenterMiddle.B - param.ColorCenter.B) * l, 255, 0);
						centerColor.A = (uint8_t)Effekseer::Clamp(param.ColorCenter.A + (param.ColorCenterMiddle.A - param.ColorCenter.A) * l, 255, 0);

						rightColor.R = (uint8_t)Effekseer::Clamp(param.ColorRight.R + (param.ColorRightMiddle.R - param.ColorRight.R) * l, 255, 0);
						rightColor.G = (uint8_t)Effekseer::Clamp(param.ColorRight.G + (param.ColorRightMiddle.G - param.ColorRight.G) * l, 255, 0);
						rightColor.B = (uint8_t)Effekseer::Clamp(param.ColorRight.B + (param.ColorRightMiddle.B - param.ColorRight.B) * l, 255, 0);
						rightColor.A = (uint8_t)Effekseer::Clamp(param.ColorRight.A + (param.ColorRightMiddle.A - param.ColorRight.A) * l, 255, 0);
					}

					const ::Effekseer::Matrix43& mat = param.SRTMatrix43;
					::Effekseer::Vector3D s;
					::Effekseer::Matrix43 r;
					::Effekseer::Vector3D t;
					mat.GetSRT(s, r, t);

					VERTEX v[3];

					v[0].Pos.X = (-size / 2.0f) * s.X;
					v[0].Pos.Y = 0.0f;
					v[0].Pos.Z = 0.0f;
					v[0].SetColor(leftColor);

					v[1].Pos.X = 0.0f;
					v[1].Pos.Y = 0.0f;
					v[1].Pos.Z = 0.0f;
					v[1].SetColor(centerColor);

					v[2].Pos.X = (size / 2.0f) * s.X;
					v[2].Pos.Y = 0.0f;
					v[2].Pos.Z = 0.0f;
					v[2].SetColor(rightColor);

					if (parameter.SplineDivision > 1)
					{
						v[1].Pos = spline.GetValue(param.InstanceIndex + sploop / (float)parameter.SplineDivision);
					}
					else
					{
						v[1].Pos.X = param.SRTMatrix43.Value[3][0];
						v[1].Pos.Y = param.SRTMatrix43.Value[3][1];
						v[1].Pos.Z = param.SRTMatrix43.Value[3][2];
					}

					if (isFirst)
					{
						verteies[0] = v[0];
						verteies[1] = v[1];
						verteies[4] = v[1];
						verteies[5] = v[2];
						verteies += 2;

					}
					else if (isLast)
					{
						verteies[0] = v[0];
						verteies[1] = v[1];
						verteies[4] = v[1];
						verteies[5] = v[2];
						verteies += 6;
						m_ribbonCount += 2;
					}
					else
					{
						verteies[0] = v[0];
						verteies[1] = v[1];
						verteies[4] = v[1];
						verteies[5] = v[2];

						verteies[6] = v[0];
						verteies[7] = v[1];
						verteies[10] = v[1];
						verteies[11] = v[2];

						verteies += 8;
						m_ribbonCount += 2;
					}

					if (isLast)
					{
						break;
					}
				}
			}


			// transform all vertecies
			{
				StrideView<VERTEX> vs_(m_ringBufferData, stride_, vertexCount_);
				Effekseer::Vector3D axisBefore;

				for (size_t i = 0; i < (instances.size() - 1) * parameter.SplineDivision + 1; i++)
				{
					bool isFirst_ = (i == 0);
					bool isLast_ = (i == ((instances.size() - 1) * parameter.SplineDivision));
					Effekseer::Vector3D axis;
					Effekseer::Vector3D pos;

					if (isFirst_)
					{
						axis = (vs_[3].Pos - vs_[1].Pos);
						Effekseer::Vector3D::Normal(axis, axis);
						axisBefore = axis;
					}
					else if (isLast_)
					{
						axis = axisBefore;
					}
					else
					{
						Effekseer::Vector3D axisOld = axisBefore;
						axis = vs_[9].Pos - vs_[7].Pos;
						Effekseer::Vector3D::Normal(axis, axis);
						axisBefore = axis;

						axis = (axisBefore + axisOld) / 2.0f;
					}

					pos = vs_[1].Pos;

					VERTEX vl = vs_[0];
					VERTEX vm = vs_[1];
					VERTEX vr = vs_[5];

					vm.Pos.X = 0.0f;
					vm.Pos.Y = 0.0f;
					vm.Pos.Z = 0.0f;

					::Effekseer::Vector3D F;
					::Effekseer::Vector3D R;
					::Effekseer::Vector3D U;

					U = axis;

					::Effekseer::Vector3D::Normal(F, -m_renderer->GetCameraFrontDirection());

					::Effekseer::Vector3D::Normal(R, ::Effekseer::Vector3D::Cross(R, U, F));
					::Effekseer::Vector3D::Normal(F, ::Effekseer::Vector3D::Cross(F, R, U));

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
					mat_rot.Value[3][0] = pos.X;
					mat_rot.Value[3][1] = pos.Y;
					mat_rot.Value[3][2] = pos.Z;

					::Effekseer::Vector3D::Transform(
						vl.Pos,
						vl.Pos,
						mat_rot);

					::Effekseer::Vector3D::Transform(
						vm.Pos,
						vm.Pos,
						mat_rot);

					::Effekseer::Vector3D::Transform(
						vr.Pos,
						vr.Pos,
						mat_rot);


					if (vertexType == VertexType::Distortion)
					{
						auto vl_ = (VERTEX_DISTORTION*)(&vl);
						auto vm_ = (VERTEX_DISTORTION*)(&vm);
						auto vr_ = (VERTEX_DISTORTION*)(&vr);

						vl_->Binormal = axis;
						vm_->Binormal = axis;
						vr_->Binormal = axis;

						::Effekseer::Vector3D tangent;
						::Effekseer::Vector3D::Normal(tangent, vr_->Pos - vl_->Pos);

						vl_->Tangent = tangent;
						vm_->Tangent = tangent;
						vr_->Tangent = tangent;
					}
					else if (vertexType == VertexType::Dynamic)
					{
						auto vl_ = (DynamicVertex*)(&vl);
						auto vm_ = (DynamicVertex*)(&vm);
						auto vr_ = (DynamicVertex*)(&vr);

						::Effekseer::Vector3D tangent;
						::Effekseer::Vector3D::Normal(tangent, vr_->Pos - vl_->Pos);

						Effekseer::Vector3D normal;
						Effekseer::Vector3D::Cross(normal, axis, tangent);
						Effekseer::Vector3D::Normal(normal, normal);

						Effekseer::Color normal_ = PackVector3DF(normal);
						Effekseer::Color tangent_ = PackVector3DF(tangent);

						vl_->Normal = normal_;
						vm_->Normal = normal_;
						vr_->Normal = normal_;

						vl_->Tangent = tangent_;
						vm_->Tangent = tangent_;
						vr_->Tangent = tangent_;
					}

					if (isFirst_)
					{
						vs_[0] = vl;
						vs_[1] = vm;
						vs_[4] = vm;
						vs_[5] = vr;
						vs_ += 2;

					}
					else if (isLast_)
					{
						vs_[0] = vl;
						vs_[1] = vm;
						vs_[4] = vm;
						vs_[5] = vr;
						vs_ += 6;
					}
					else
					{
						vs_[0] = vl;
						vs_[1] = vm;
						vs_[4] = vm;
						vs_[5] = vr;

						vs_[6] = vl;
						vs_[7] = vm;
						vs_[10] = vm;
						vs_[11] = vr;

						vs_ += 8;
					}
				}
			}

			// calculate UV
			if (parameter.TextureUVTypeParameterPtr->Type == ::Effekseer::TextureUVType::Strech)
			{
				verteies.Reset();
				for (size_t loop = 0; loop < instances.size() - 1; loop++)
				{
					const auto& param = instances[loop];

					for (int32_t sploop = 0; sploop < parameter.SplineDivision; sploop++)
					{
						float percent1 = (float)(param.InstanceIndex * parameter.SplineDivision + sploop) /
										 (float)((param.InstanceCount - 1) * parameter.SplineDivision);

						float percent2 = (float)(param.InstanceIndex * parameter.SplineDivision + sploop + 1) /
										 (float)((param.InstanceCount - 1) * parameter.SplineDivision);

						auto uvX1 = param.UV.X;
						auto uvX2 = param.UV.X + param.UV.X + param.UV.Width * 0.5f;
						auto uvX3 = param.UV.X + param.UV.X + param.UV.Width;
						auto uvY1 = param.UV.Y + percent1 * param.UV.Height;
						auto uvY2 = param.UV.Y + percent2 * param.UV.Height;

						AssignUV(verteies, uvX1, uvX2, uvX3, uvY1, uvY2);

						verteies += 8;
					}
				}
			}
			else if (parameter.TextureUVTypeParameterPtr->Type == ::Effekseer::TextureUVType::Tile)
			{
				const auto& uvParam = *parameter.TextureUVTypeParameterPtr;
				verteies.Reset();

				for (size_t loop = 0; loop < instances.size() - 1; loop++)
				{
					auto& param = instances[loop];

					if (loop < uvParam.TileEdgeTail)
					{
						float uvBegin = param.UV.Y;
						float uvEnd = param.UV.Y + param.UV.Height * uvParam.TileLoopAreaBegin;

						for (int32_t sploop = 0; sploop < parameter.SplineDivision; sploop++)
						{
							float percent1 = (float)(param.InstanceIndex * parameter.SplineDivision + sploop) /
											 (float)((uvParam.TileEdgeTail) * parameter.SplineDivision);

							float percent2 = (float)(param.InstanceIndex * parameter.SplineDivision + sploop + 1) /
											 (float)((uvParam.TileEdgeTail) * parameter.SplineDivision);

							auto uvX1 = param.UV.X;
							auto uvX2 = param.UV.X + param.UV.X + param.UV.Width * 0.5f;
							auto uvX3 = param.UV.X + param.UV.X + param.UV.Width;
							auto uvY1 = uvBegin + (uvEnd - uvBegin) * percent1;
							auto uvY2 = uvBegin + (uvEnd - uvBegin) * percent2;

							AssignUV(verteies, uvX1, uvX2, uvX3, uvY1, uvY2);

							verteies += 8;
						}
					}
					else if (loop >= param.InstanceCount - 1 - uvParam.TileEdgeHead)
					{
						float uvBegin = param.UV.Y + param.UV.Height * uvParam.TileLoopAreaEnd;
						float uvEnd = param.UV.Y + param.UV.Height * 1.0f;

						for (int32_t sploop = 0; sploop < parameter.SplineDivision; sploop++)
						{
							float percent1 = (float)((param.InstanceIndex - (param.InstanceCount - 1 - uvParam.TileEdgeHead)) *
														 parameter.SplineDivision +
													 sploop) /
											 (float)((uvParam.TileEdgeHead) * parameter.SplineDivision);

							float percent2 = (float)((param.InstanceIndex - (param.InstanceCount - 1 - uvParam.TileEdgeHead)) *
														 parameter.SplineDivision +
													 sploop + 1) /
											 (float)((uvParam.TileEdgeHead) * parameter.SplineDivision);

							auto uvX1 = param.UV.X;
							auto uvX2 = param.UV.X + param.UV.X + param.UV.Width * 0.5f;
							auto uvX3 = param.UV.X + param.UV.X + param.UV.Width;
							auto uvY1 = uvBegin + (uvEnd - uvBegin) * percent1;
							auto uvY2 = uvBegin + (uvEnd - uvBegin) * percent2;

							AssignUV(verteies, uvX1, uvX2, uvX3, uvY1, uvY2);

							verteies += 8;
						}
					}
					else
					{
						float uvBegin = param.UV.Y + param.UV.Height * uvParam.TileLoopAreaBegin;
						float uvEnd = param.UV.Y + param.UV.Height * uvParam.TileLoopAreaEnd;

						for (int32_t sploop = 0; sploop < parameter.SplineDivision; sploop++)
						{
							bool isFirst = param.InstanceIndex == 0 && sploop == 0;
							bool isLast = param.InstanceIndex == (param.InstanceCount - 1);

							float percent1 = (float)(sploop) / (float)(parameter.SplineDivision);

							float percent2 = (float)(sploop + 1) / (float)(parameter.SplineDivision);

							auto uvX1 = param.UV.X;
							auto uvX2 = param.UV.X + param.UV.X + param.UV.Width * 0.5f;
							auto uvX3 = param.UV.X + param.UV.X + param.UV.Width;
							auto uvY1 = uvBegin + (uvEnd - uvBegin) * percent1;
							auto uvY2 = uvBegin + (uvEnd - uvBegin) * percent2;

							AssignUV(verteies, uvX1, uvX2, uvX3, uvY1, uvY2);

							verteies += 8;
						}
					}
				}
			}
		}

	public:

		TrackRendererBase(RENDERER* renderer)
			: m_renderer(renderer)
			, m_ribbonCount(0)
			, m_ringBufferOffset(0)
			, m_ringBufferData(NULL)
		{
		}

		virtual ~TrackRendererBase()
		{
		}


	protected:

		void Rendering_(const efkTrackNodeParam& parameter, const efkTrackInstanceParam& instanceParameter, void* userData, const ::Effekseer::Matrix44& camera)
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

		template<typename VERTEX>
		void Rendering_Internal(const efkTrackNodeParam& parameter, const efkTrackInstanceParam& instanceParameter, void* userData, const ::Effekseer::Matrix44& camera)
		{
			if (m_ringBufferData == NULL) return;
			if (instanceParameter.InstanceCount < 2) return;

			const efkTrackInstanceParam& param = instanceParameter;

			bool isFirst = param.InstanceIndex == 0;
			bool isLast = param.InstanceIndex == (param.InstanceCount - 1);

			if (isFirst)
			{
				instances.reserve(param.InstanceCount);
				instances.resize(0);
				innstancesNodeParam = parameter;
			}

			instances.push_back(param);

			if (isLast)
			{
				RenderSplines<VERTEX>(camera);
			}
		}

	public:

		void Rendering(const efkTrackNodeParam& parameter, const efkTrackInstanceParam& instanceParameter, void* userData) override
		{
			Rendering_(parameter, instanceParameter, userData, m_renderer->GetCameraMatrix());
		}

		void BeginRenderingGroup(const efkTrackNodeParam& param, int32_t count, void* userData) override
		{
			m_ribbonCount = 0;
			int32_t vertexCount = ((count - 1) * param.SplineDivision) * 8;
			if (vertexCount <= 0) return;

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

			m_renderer->GetStandardRenderer()->UpdateStateAndRenderingIfRequired(state);

			m_renderer->GetStandardRenderer()->BeginRenderingAndRenderingIfRequired(vertexCount, m_ringBufferOffset, stride_, (void*&)m_ringBufferData);
			vertexCount_ = vertexCount;
		}
	};

	//----------------------------------------------------------------------------------
	//
	//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_RIBBON_RENDERER_H__