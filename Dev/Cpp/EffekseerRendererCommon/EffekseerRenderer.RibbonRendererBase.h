
#ifndef	__EFFEKSEERRENDERER_RIBBON_RENDERER_BASE_H__
#define	__EFFEKSEERRENDERER_RIBBON_RENDERER_BASE_H__

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
	typedef ::Effekseer::RibbonRenderer::NodeParameter efkRibbonNodeParam;
	typedef ::Effekseer::RibbonRenderer::InstanceParameter efkRibbonInstanceParam;
	typedef ::Effekseer::Vector3D efkVector3D;

	template<typename RENDERER, typename VERTEX_NORMAL, typename VERTEX_DISTORTION>
	class RibbonRendererBase
		: public ::Effekseer::RibbonRenderer
	{
	private:

	protected:
		RENDERER*						m_renderer;
		int32_t							m_ribbonCount;

		int32_t							m_ringBufferOffset;
		uint8_t*						m_ringBufferData;

		efkRibbonNodeParam					innstancesNodeParam;
		std::vector<efkRibbonInstanceParam>	instances;
		SplineGenerator spline_left;
		SplineGenerator spline_right;

		int32_t vertexCount_ = 0;
		int32_t stride_ = 0;

		int32_t customData1Count_ = 0;
		int32_t customData2Count_ = 0;

		enum class VertexType
		{
			Normal,
			Distortion,
			Dynamic,
		};

		VertexType GetVertexType(const VERTEX_NORMAL* v) { return VertexType::Normal; }

		VertexType GetVertexType(const VERTEX_DISTORTION* v) { return VertexType::Distortion; }

		VertexType GetVertexType(const DynamicVertex* v) { return VertexType::Dynamic; }

		template <typename VERTEX, int TARGET> void AssignUV(StrideView<VERTEX> v, float uvX1, float uvX2, float uvY1, float uvY2)
		{
			if (TARGET == 0)
			{
				v[0].UV[0] = uvX1;
				v[0].UV[1] = uvY1;

				v[1].UV[0] = uvX2;
				v[1].UV[1] = uvY1;

				v[2].UV[0] = uvX1;
				v[2].UV[1] = uvY2;

				v[3].UV[0] = uvX2;
				v[3].UV[1] = uvY2;
			}
			else
			{
				v[0].UV2[0] = uvX1;
				v[0].UV2[1] = uvY1;
					   
				v[1].UV2[0] = uvX2;
				v[1].UV2[1] = uvY1;
					   
				v[2].UV2[0] = uvX1;
				v[2].UV2[1] = uvY2;
					   
				v[3].UV2[0] = uvX2;
				v[3].UV2[1] = uvY2;
			}
		}

		template <typename VERTEX, int TARGET> 
		void AssignUVs(efkRibbonNodeParam& parameter, StrideView<VERTEX> verteies)
		{
			float uvx = 0.0f;
			float uvw = 1.0f;
			float uvy = 0.0f;
			float uvh = 1.0f;

			if (parameter.TextureUVTypeParameterPtr->Type == ::Effekseer::TextureUVType::Strech)
			{
				verteies.Reset();

				for (size_t loop = 0; loop < instances.size() - 1; loop++)
				{
					const auto& param = instances[loop];
					if (TARGET == 0)
					{
						uvx = param.UV.X;
						uvw = param.UV.Width;
						uvy = param.UV.Y;
						uvh = param.UV.Height;
					}

					for (int32_t sploop = 0; sploop < parameter.SplineDivision; sploop++)
					{
						float percent1 = (float)(param.InstanceIndex * parameter.SplineDivision + sploop) /
										 (float)((param.InstanceCount - 1) * parameter.SplineDivision);

						float percent2 = (float)(param.InstanceIndex * parameter.SplineDivision + sploop + 1) /
										 (float)((param.InstanceCount - 1) * parameter.SplineDivision);

						auto uvX1 = uvx;
						auto uvX2 = uvx + uvw;
						auto uvY1 = uvy + percent1 * uvh;
						auto uvY2 = uvy + percent2 * uvh;

						AssignUV<VERTEX, TARGET>(verteies, uvX1, uvX2, uvY1, uvY2);

						verteies += 4;
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
					if (TARGET == 0)
					{
						uvx = param.UV.X;
						uvw = param.UV.Width;
						uvy = param.UV.Y;
						uvh = param.UV.Height;
					}

					if (loop < uvParam.TileEdgeTail)
					{
						float uvBegin = uvy;
						float uvEnd = uvy + uvh * uvParam.TileLoopAreaBegin;

						for (int32_t sploop = 0; sploop < parameter.SplineDivision; sploop++)
						{
							float percent1 = (float)(param.InstanceIndex * parameter.SplineDivision + sploop) /
											 (float)((uvParam.TileEdgeTail) * parameter.SplineDivision);

							float percent2 = (float)(param.InstanceIndex * parameter.SplineDivision + sploop + 1) /
											 (float)((uvParam.TileEdgeTail) * parameter.SplineDivision);

							auto uvX1 = uvx;
							auto uvX2 = uvx + uvw;
							auto uvY1 = uvBegin + (uvEnd - uvBegin) * percent1;
							auto uvY2 = uvBegin + (uvEnd - uvBegin) * percent2;

							AssignUV<VERTEX, TARGET>(verteies, uvX1, uvX2, uvY1, uvY2);

							verteies += 4;
						}
					}
					else if (loop >= param.InstanceCount - 1 - uvParam.TileEdgeHead)
					{
						float uvBegin = uvy + uvh * uvParam.TileLoopAreaEnd;
						float uvEnd = uvy + uvh * 1.0f;

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

							auto uvX1 = uvx;
							auto uvX2 = uvx + uvw;
							auto uvY1 = uvBegin + (uvEnd - uvBegin) * percent1;
							auto uvY2 = uvBegin + (uvEnd - uvBegin) * percent2;

							AssignUV<VERTEX, TARGET>(verteies, uvX1, uvX2, uvY1, uvY2);

							verteies += 4;
						}
					}
					else
					{
						float uvBegin = uvy + uvh * uvParam.TileLoopAreaBegin;
						float uvEnd = uvy + uvh * uvParam.TileLoopAreaEnd;

						for (int32_t sploop = 0; sploop < parameter.SplineDivision; sploop++)
						{
							bool isFirst = param.InstanceIndex == 0 && sploop == 0;
							bool isLast = param.InstanceIndex == (param.InstanceCount - 1);

							float percent1 = (float)(sploop) / (float)(parameter.SplineDivision);

							float percent2 = (float)(sploop + 1) / (float)(parameter.SplineDivision);

							auto uvX1 = uvx;
							auto uvX2 = uvx + uvw;
							auto uvY1 = uvBegin + (uvEnd - uvBegin) * percent1;
							auto uvY2 = uvBegin + (uvEnd - uvBegin) * percent2;

							AssignUV<VERTEX, TARGET>(verteies, uvX1, uvX2, uvY1, uvY2);

							verteies += 4;
						}
					}
				}
			}

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
				spline_left.Reset();
				spline_right.Reset();

				for (size_t loop = 0; loop < instances.size(); loop++)
				{
					auto pl = efkVector3D();
					auto pr = efkVector3D();

					auto& param = instances[loop];

					pl.X = param.Positions[0];
					pl.Y = 0.0f;
					pl.Z = 0.0f;

					pr.X = param.Positions[1];
					pr.Y = 0.0f;
					pr.Z = 0.0f;

					if (parameter.ViewpointDependent)
					{
						::Effekseer::Matrix43 mat = param.SRTMatrix43;
						::Effekseer::Vector3D s;
						::Effekseer::Matrix43 r;
						::Effekseer::Vector3D t;
						mat.GetSRT(s, r, t);

						ApplyDepthParameters(r,
											 t,
											 s,
											 m_renderer->GetCameraFrontDirection(),
											 m_renderer->GetCameraPosition(),
											 parameter.DepthParameterPtr,
											 parameter.IsRightHand);

						// extend
						pl.X = pl.X * s.X;
						pr.X = pr.X * s.X;

						::Effekseer::Vector3D F;
						::Effekseer::Vector3D R;
						::Effekseer::Vector3D U;

						U = ::Effekseer::Vector3D(r.Value[1][0], r.Value[1][1], r.Value[1][2]);

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
						mat_rot.Value[3][0] = t.X;
						mat_rot.Value[3][1] = t.Y;
						mat_rot.Value[3][2] = t.Z;


						::Effekseer::Vector3D::Transform(
							pl,
							pl,
							mat_rot);

						::Effekseer::Vector3D::Transform(
							pr,
							pr,
							mat_rot);

						spline_left.AddVertex(pl);
						spline_right.AddVertex(pr);
					}
					else
					{
						::Effekseer::Matrix43 mat = param.SRTMatrix43;

						::Effekseer::Vector3D s;

						ApplyDepthParameters(mat,
											 m_renderer->GetCameraFrontDirection(),
											 m_renderer->GetCameraPosition(),
											 //s,
											 parameter.DepthParameterPtr,
											 parameter.IsRightHand);

						::Effekseer::Vector3D::Transform(
							pl,
							pl,
							mat);

						::Effekseer::Vector3D::Transform(
							pr,
							pr, 
							mat);

						spline_left.AddVertex(pl);
						spline_right.AddVertex(pr);
					}
				}

				spline_left.Calculate();
				spline_right.Calculate();
			}

			StrideView<VERTEX> verteies(m_ringBufferData, stride_, vertexCount_);
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
						verteies[0].Pos = spline_left.GetValue(param.InstanceIndex + sploop / (float)parameter.SplineDivision);
						verteies[1].Pos = spline_right.GetValue(param.InstanceIndex + sploop / (float)parameter.SplineDivision);

						verteies[0].SetColor(Effekseer::Color::Lerp(param.Colors[0], param.Colors[2], percent_instance));
						verteies[1].SetColor(Effekseer::Color::Lerp(param.Colors[1], param.Colors[3], percent_instance));
					}
					else
					{
						for (int i = 0; i < 2; i++)
						{
							verteies[i].Pos.X = param.Positions[i];
							verteies[i].Pos.Y = 0.0f;
							verteies[i].Pos.Z = 0.0f;
							verteies[i].SetColor(param.Colors[i]);
						}
					}


					float percent = (float)(param.InstanceIndex  * parameter.SplineDivision + sploop) / (float)((param.InstanceCount - 1) * parameter.SplineDivision);

					if (parameter.ViewpointDependent)
					{
						::Effekseer::Matrix43 mat = param.SRTMatrix43;
						::Effekseer::Vector3D s;
						::Effekseer::Matrix43 r;
						::Effekseer::Vector3D t;
						mat.GetSRT(s, r, t);

						ApplyDepthParameters(r,
											 t,
											 s,
											 m_renderer->GetCameraFrontDirection(),
											 m_renderer->GetCameraPosition(),
											 parameter.DepthParameterPtr,
											 parameter.IsRightHand);

						if (parameter.SplineDivision > 1)
						{
						}
						else
						{
							for (int i = 0; i < 2; i++)
							{
								verteies[i].Pos.X = verteies[i].Pos.X * s.X;
							}

							::Effekseer::Vector3D F;
							::Effekseer::Vector3D R;
							::Effekseer::Vector3D U;

							U = ::Effekseer::Vector3D(r.Value[1][0], r.Value[1][1], r.Value[1][2]);

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
							mat_rot.Value[3][0] = t.X;
							mat_rot.Value[3][1] = t.Y;
							mat_rot.Value[3][2] = t.Z;

							for (int i = 0; i < 2; i++)
							{
								::Effekseer::Vector3D::Transform(
									verteies[i].Pos,
									verteies[i].Pos,
									mat_rot);
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
							::Effekseer::Matrix43 mat = param.SRTMatrix43;

							::Effekseer::Vector3D s;
	
							ApplyDepthParameters(mat,
												 m_renderer->GetCameraFrontDirection(),
												 m_renderer->GetCameraPosition(),
												 //s,
												 parameter.DepthParameterPtr,
												 parameter.IsRightHand);


							for (int i = 0; i < 2; i++)
							{
								::Effekseer::Vector3D::Transform(
									verteies[i].Pos,
									verteies[i].Pos,
									mat);
							}
						}
					}

					if (isFirst || isLast)
					{
						verteies += 2;
					}
					else
					{
						verteies[2] = verteies[0];
						verteies[3] = verteies[1];
						verteies += 4;
					}

					if (!isFirst)
					{
						m_ribbonCount++;
					}

					if (isLast)
					{
						break;
					}
				}
			}

			// calculate UV
			AssignUVs<VERTEX, 0>(parameter, verteies);

			if (vertexType == VertexType::Dynamic)
			{
				AssignUVs<VERTEX, 1>(parameter, verteies);
			}

			// Apply distortion
			if (vertexType == VertexType::Distortion)
			{
				StrideView<VERTEX_DISTORTION> vs_(m_ringBufferData, stride_, vertexCount_);
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
						axis = (vs_[5].Pos - vs_[3].Pos);
						Effekseer::Vector3D::Normal(axis, axis);
						axisBefore = axis;

						axis = (axisBefore + axisOld) / 2.0f;
						Effekseer::Vector3D::Normal(axis, axis);
					}

					auto tangent = vs_[1].Pos - vs_[0].Pos;
					Effekseer::Vector3D::Normal(tangent, tangent);

					if (isFirst_)
					{
						vs_[0].Binormal = axis;
						vs_[1].Binormal = axis;

						vs_[0].Tangent = tangent;
						vs_[1].Tangent = tangent;

						vs_ += 2;

					}
					else if (isLast_)
					{
						vs_[0].Binormal = axis;
						vs_[1].Binormal = axis;

						vs_[0].Tangent = tangent;
						vs_[1].Tangent = tangent;

						vs_ += 2;
					}
					else
					{
						vs_[0].Binormal = axis;
						vs_[1].Binormal = axis;
						vs_[2].Binormal = axis;
						vs_[3].Binormal = axis;

						vs_[0].Tangent = tangent;
						vs_[1].Tangent = tangent;
						vs_[2].Tangent = tangent;
						vs_[3].Tangent = tangent;

						vs_ += 4;
					}
				}
			}
			else if (vertexType == VertexType::Dynamic)
			{
				StrideView<DynamicVertex> vs_(m_ringBufferData, stride_, vertexCount_);
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
						axis = (vs_[5].Pos - vs_[3].Pos);
						Effekseer::Vector3D::Normal(axis, axis);
						axisBefore = axis;

						axis = (axisBefore + axisOld) / 2.0f;
						Effekseer::Vector3D::Normal(axis, axis);
					}

					auto tangent = vs_[1].Pos - vs_[0].Pos;
					Effekseer::Vector3D::Normal(tangent, tangent);

					Effekseer::Vector3D normal;
					Effekseer::Vector3D::Cross(normal, axis, tangent);
					Effekseer::Vector3D::Normal(normal, normal);

					if (isFirst_)
					{
						vs_[0].Normal = PackVector3DF(normal);					
						vs_[0].Tangent = PackVector3DF(tangent);
						vs_[1].Tangent = vs_[0].Tangent;
						vs_[1].Normal = vs_[0].Normal;

						vs_ += 2;
					}
					else if (isLast_)
					{
						vs_[0].Normal = PackVector3DF(normal);
						vs_[0].Tangent = PackVector3DF(tangent);
						vs_[1].Tangent = vs_[0].Tangent;
						vs_[1].Normal = vs_[0].Normal;

						vs_ += 2;
					}
					else
					{
						vs_[0].Normal = PackVector3DF(normal);
						vs_[0].Tangent = PackVector3DF(tangent);
						vs_[1].Tangent = vs_[0].Tangent;
						vs_[1].Normal = vs_[0].Normal;
						vs_[2].Tangent = vs_[0].Tangent;
						vs_[2].Normal = vs_[0].Normal;
						vs_[3].Tangent = vs_[0].Tangent;
						vs_[3].Normal = vs_[0].Normal;

						vs_ += 4;
					}
				}
			}

			// custom parameter
			if (customData1Count_ > 0)
			{
				StrideView<float> custom(m_ringBufferData + sizeof(DynamicVertex), stride_, vertexCount_);
				for (size_t loop = 0; loop < instances.size() - 1; loop++)
				{
					auto& param = instances[loop];

					for (int32_t sploop = 0; sploop < parameter.SplineDivision; sploop++)
					{
						for (size_t i = 0; i < 4; i++)
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
				StrideView<float> custom(
					m_ringBufferData + sizeof(DynamicVertex) + sizeof(float) * customData1Count_, stride_, vertexCount_);
				for (size_t loop = 0; loop < instances.size() - 1; loop++)
				{
					auto& param = instances[loop];

					for (int32_t sploop = 0; sploop < parameter.SplineDivision; sploop++)
					{
						for (size_t i = 0; i < 4; i++)
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
			: m_renderer(renderer)
			, m_ribbonCount(0)
			, m_ringBufferOffset(0)
			, m_ringBufferData(NULL)
		{
		}

		virtual ~RibbonRendererBase()
		{
		}


	protected:

		void Rendering_(const efkRibbonNodeParam& parameter, const efkRibbonInstanceParam& instanceParameter, void* userData, const ::Effekseer::Matrix44& camera)
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
		void Rendering_Internal(const efkRibbonNodeParam& parameter, const efkRibbonInstanceParam& instanceParameter, void* userData, const ::Effekseer::Matrix44& camera)
		{
			if (m_ringBufferData == NULL) return;
			if (instanceParameter.InstanceCount < 2) return;

			bool isFirst = instanceParameter.InstanceIndex == 0;
			bool isLast = instanceParameter.InstanceIndex == (instanceParameter.InstanceCount - 1);

			auto& param = instanceParameter;

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

		void BeginRenderingGroup(const efkRibbonNodeParam& param, int32_t count, void* userData) override
		{
			m_ribbonCount = 0;
			int32_t vertexCount = ((count - 1) * param.SplineDivision) * 4;
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
			customData1Count_ = state.CustomData1Count;
			customData2Count_ = state.CustomData2Count;

			m_renderer->GetStandardRenderer()->UpdateStateAndRenderingIfRequired(state);

			m_renderer->GetStandardRenderer()->BeginRenderingAndRenderingIfRequired(vertexCount, stride_, (void*&)m_ringBufferData);
			vertexCount_ = vertexCount;
		}

		void Rendering(const efkRibbonNodeParam& parameter, const efkRibbonInstanceParam& instanceParameter, void* userData) override
		{
			Rendering_(parameter, instanceParameter, userData, m_renderer->GetCameraMatrix());
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