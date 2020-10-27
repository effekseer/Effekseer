
#ifndef __EFFEKSEERRENDERER_RIBBON_RENDERER_BASE_H__
#define __EFFEKSEERRENDERER_RIBBON_RENDERER_BASE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <Effekseer.Internal.h>
#include <Effekseer.h>
#include <assert.h>
#include <string.h>

#include "EffekseerRenderer.CommonUtils.h"
#include "EffekseerRenderer.IndexBufferBase.h"
#include "EffekseerRenderer.RenderStateBase.h"
#include "EffekseerRenderer.StandardRenderer.h"
#include "EffekseerRenderer.VertexBufferBase.h"

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
typedef ::Effekseer::Vec3f efkVector3D;

template <typename RENDERER, typename VERTEX_NORMAL, typename VERTEX_DISTORTION>
class RibbonRendererBase
	: public ::Effekseer::RibbonRenderer,
	  public ::Effekseer::AlignedAllocationPolicy<16>
{
private:
protected:
	RENDERER* m_renderer;
	int32_t m_ribbonCount;

	int32_t m_ringBufferOffset;
	uint8_t* m_ringBufferData;

	efkRibbonNodeParam innstancesNodeParam;
	Effekseer::CustomAlignedVector<efkRibbonInstanceParam> instances;
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

	VertexType GetVertexType(const VERTEX_NORMAL* v)
	{
		return VertexType::Normal;
	}

	VertexType GetVertexType(const VERTEX_DISTORTION* v)
	{
		return VertexType::Distortion;
	}

	VertexType GetVertexType(const DynamicVertex* v)
	{
		return VertexType::Dynamic;
	}

	template <typename VERTEX, int TARGET>
	void AssignUV(StrideView<VERTEX> v, float uvX1, float uvX2, float uvY1, float uvY2)
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
#ifdef __EFFEKSEER_BUILD_VERSION16__
		else if (TARGET == 1)
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
		else if (TARGET == 2)
		{
			v[0].AlphaUV[0] = uvX1;
			v[0].AlphaUV[1] = uvY1;

			v[1].AlphaUV[0] = uvX2;
			v[1].AlphaUV[1] = uvY1;

			v[2].AlphaUV[0] = uvX1;
			v[2].AlphaUV[1] = uvY2;

			v[3].AlphaUV[0] = uvX2;
			v[3].AlphaUV[1] = uvY2;
		}
#else
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
#endif
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
#ifdef __EFFEKSEER_BUILD_VERSION16__
				else if (TARGET == 2)
				{
					uvx = param.AlphaUV.X;
					uvw = param.AlphaUV.Width;
					uvy = param.AlphaUV.Y;
					uvh = param.AlphaUV.Height;
				}
#endif

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
#ifdef __EFFEKSEER_BUILD_VERSION16__
				else if (TARGET == 2)
				{
					uvx = param.AlphaUV.X;
					uvw = param.AlphaUV.Width;
					uvy = param.AlphaUV.Y;
					uvh = param.AlphaUV.Height;
				}
#endif

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

	template <typename VERTEX>
	void RenderSplines(const ::Effekseer::Mat44f& camera)
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
				auto& param = instances[loop];

				efkVector3D pl(param.Positions[0], 0.0f, 0.0f);
				efkVector3D pr(param.Positions[1], 0.0f, 0.0f);

				if (parameter.ViewpointDependent)
				{
					::Effekseer::Mat43f mat = param.SRTMatrix43;
					::Effekseer::Vec3f s;
					::Effekseer::Mat43f r;
					::Effekseer::Vec3f t;
					mat.GetSRT(s, r, t);

					ApplyDepthParameters(r,
										 t,
										 s,
										 m_renderer->GetCameraFrontDirection(),
										 m_renderer->GetCameraPosition(),
										 parameter.DepthParameterPtr,
										 parameter.IsRightHand);

					// extend
					pl.SetX(pl.GetX() * s.GetX());
					pr.SetX(pr.GetX() * s.GetX());

					::Effekseer::Vec3f F;
					::Effekseer::Vec3f R;
					::Effekseer::Vec3f U;

					U = ::Effekseer::Vec3f(r.X.GetY(), r.Y.GetY(), r.X.GetY());
					F = ::Effekseer::Vec3f(-m_renderer->GetCameraFrontDirection()).Normalize();
					R = ::Effekseer::Vec3f::Cross(U, F).Normalize();
					F = ::Effekseer::Vec3f::Cross(R, U).Normalize();

					::Effekseer::Mat43f mat_rot(
						-R.GetX(), -R.GetY(), -R.GetZ(), U.GetX(), U.GetY(), U.GetZ(), F.GetX(), F.GetY(), F.GetZ(), t.GetX(), t.GetY(), t.GetZ());

					pl = ::Effekseer::Vec3f::Transform(pl, mat_rot);
					pr = ::Effekseer::Vec3f::Transform(pr, mat_rot);

					spline_left.AddVertex(pl);
					spline_right.AddVertex(pr);
				}
				else
				{
					::Effekseer::Mat43f mat = param.SRTMatrix43;

					ApplyDepthParameters(mat,
										 m_renderer->GetCameraFrontDirection(),
										 m_renderer->GetCameraPosition(),
										 //s,
										 parameter.DepthParameterPtr,
										 parameter.IsRightHand);

					pl = ::Effekseer::Vec3f::Transform(pl, mat);
					pr = ::Effekseer::Vec3f::Transform(pr, mat);

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
					verteies[0].Pos = ToStruct(spline_left.GetValue(param.InstanceIndex + sploop / (float)parameter.SplineDivision));
					verteies[1].Pos = ToStruct(spline_right.GetValue(param.InstanceIndex + sploop / (float)parameter.SplineDivision));

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

#ifdef __EFFEKSEER_BUILD_VERSION16__
						verteies[i].FlipbookIndexAndNextRate = param.FlipbookIndexAndNextRate;
						verteies[i].AlphaThreshold = param.AlphaThreshold;
#endif
					}
				}

				float percent = (float)(param.InstanceIndex * parameter.SplineDivision + sploop) / (float)((param.InstanceCount - 1) * parameter.SplineDivision);

				if (parameter.ViewpointDependent)
				{
					::Effekseer::Mat43f mat = param.SRTMatrix43;
					::Effekseer::Vec3f s;
					::Effekseer::Mat43f r;
					::Effekseer::Vec3f t;
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
							verteies[i].Pos.X = verteies[i].Pos.X * s.GetX();
						}

						::Effekseer::Vec3f F;
						::Effekseer::Vec3f R;
						::Effekseer::Vec3f U;

						U = ::Effekseer::Vec3f(r.X.GetY(), r.Y.GetY(), r.Z.GetY());

						F = ::Effekseer::Vec3f(-m_renderer->GetCameraFrontDirection()).Normalize();
						R = ::Effekseer::Vec3f::Cross(U, F).Normalize();
						F = ::Effekseer::Vec3f::Cross(R, U).Normalize();

						::Effekseer::Mat43f mat_rot(
							-R.GetX(), -R.GetY(), -R.GetZ(), U.GetX(), U.GetY(), U.GetZ(), F.GetX(), F.GetY(), F.GetZ(), t.GetX(), t.GetY(), t.GetZ());

						for (int i = 0; i < 2; i++)
						{
							verteies[i].Pos = ToStruct(::Effekseer::Vec3f::Transform(verteies[i].Pos, mat_rot));
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
						::Effekseer::Mat43f mat = param.SRTMatrix43;

						ApplyDepthParameters(mat,
											 m_renderer->GetCameraFrontDirection(),
											 m_renderer->GetCameraPosition(),
											 //s,
											 parameter.DepthParameterPtr,
											 parameter.IsRightHand);

						for (int i = 0; i < 2; i++)
						{
							verteies[i].Pos = ToStruct(::Effekseer::Vec3f::Transform(verteies[i].Pos, mat));
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

#ifdef __EFFEKSEER_BUILD_VERSION16__
		AssignUVs<VERTEX, 2>(parameter, verteies);
#endif

		// Apply distortion
		if (vertexType == VertexType::Distortion)
		{
			StrideView<VERTEX_DISTORTION> vs_(m_ringBufferData, stride_, vertexCount_);
			Effekseer::Vec3f axisBefore;

			for (size_t i = 0; i < (instances.size() - 1) * parameter.SplineDivision + 1; i++)
			{
				bool isFirst_ = (i == 0);
				bool isLast_ = (i == ((instances.size() - 1) * parameter.SplineDivision));

				Effekseer::Vec3f axis;

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
					Effekseer::Vec3f axisOld = axisBefore;
					axis = (vs_[5].Pos - vs_[3].Pos);
					axis = SafeNormalize(axis);
					axisBefore = axis;

					axis = (axisBefore + axisOld) / 2.0f;
					axis = SafeNormalize(axis);
				}

				Effekseer::Vec3f tangent = vs_[1].Pos - vs_[0].Pos;
				tangent = tangent.Normalize();

				if (isFirst_)
				{
					vs_[0].Binormal = vs_[1].Binormal = ToStruct(axis);
					vs_[0].Tangent = vs_[1].Tangent = ToStruct(tangent);
					vs_ += 2;
				}
				else if (isLast_)
				{
					vs_[0].Binormal = vs_[1].Binormal = ToStruct(axis);
					vs_[0].Tangent = vs_[1].Tangent = ToStruct(tangent);
					vs_ += 2;
				}
				else
				{
					vs_[0].Binormal = vs_[1].Binormal = vs_[2].Binormal = vs_[3].Binormal = ToStruct(axis);
					vs_[0].Tangent = vs_[1].Tangent = vs_[2].Tangent = vs_[3].Tangent = ToStruct(tangent);
					vs_ += 4;
				}
			}
		}
		else if (vertexType == VertexType::Dynamic)
		{
			StrideView<DynamicVertex> vs_(m_ringBufferData, stride_, vertexCount_);
			Effekseer::Vec3f axisBefore;

			for (size_t i = 0; i < (instances.size() - 1) * parameter.SplineDivision + 1; i++)
			{
				bool isFirst_ = (i == 0);
				bool isLast_ = (i == ((instances.size() - 1) * parameter.SplineDivision));

				Effekseer::Vec3f axis;

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
					Effekseer::Vec3f axisOld = axisBefore;
					axis = (vs_[5].Pos - vs_[3].Pos);
					axis = SafeNormalize(axis);
					axisBefore = axis;

					axis = (axisBefore + axisOld) / 2.0f;
					axis = SafeNormalize(axis);
				}

				Effekseer::Vec3f tangent = vs_[1].Pos - vs_[0].Pos;
				tangent = SafeNormalize(tangent);

				Effekseer::Vec3f normal = Effekseer::Vec3f::Cross(axis, tangent);
				normal = SafeNormalize(normal);

				if (!parameter.IsRightHand)
				{
					normal = -normal;
				}

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
	void Rendering_(const efkRibbonNodeParam& parameter, const efkRibbonInstanceParam& instanceParameter, void* userData, const ::Effekseer::Mat44f& camera)
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

	template <typename VERTEX>
	void Rendering_Internal(const efkRibbonNodeParam& parameter, const efkRibbonInstanceParam& instanceParameter, void* userData, const ::Effekseer::Mat44f& camera)
	{
		if (m_ringBufferData == NULL)
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
		if (vertexCount <= 0)
			return;

		EffekseerRenderer::StandardRendererState state;
		state.AlphaBlend = param.BasicParameterPtr->AlphaBlend;
		state.CullingType = ::Effekseer::CullingType::Double;
		state.DepthTest = param.ZTest;
		state.DepthWrite = param.ZWrite;
		state.TextureFilter1 = param.BasicParameterPtr->TextureFilter1;
		state.TextureWrap1 = param.BasicParameterPtr->TextureWrap1;
		state.TextureFilter2 = param.BasicParameterPtr->TextureFilter2;
		state.TextureWrap2 = param.BasicParameterPtr->TextureWrap2;
#ifdef __EFFEKSEER_BUILD_VERSION16__
		state.TextureFilter3 = param.BasicParameterPtr->TextureFilter3;
		state.TextureWrap3 = param.BasicParameterPtr->TextureWrap3;

		state.EnableInterpolation = param.BasicParameterPtr->EnableInterpolation;
		state.UVLoopType = param.BasicParameterPtr->UVLoopType;
		state.InterpolationType = param.BasicParameterPtr->InterpolationType;
		state.FlipbookDivideX = param.BasicParameterPtr->FlipbookDivideX;
		state.FlipbookDivideY = param.BasicParameterPtr->FlipbookDivideY;
#endif

		state.Distortion = param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::BackDistortion;
		state.DistortionIntensity = param.BasicParameterPtr->DistortionIntensity;
		state.MaterialType = param.BasicParameterPtr->MaterialType;

		state.CopyMaterialFromParameterToState(param.EffectPointer,
											   param.BasicParameterPtr->MaterialParameterPtr,
											   param.BasicParameterPtr->Texture1Index,
											   param.BasicParameterPtr->Texture2Index
#ifdef __EFFEKSEER_BUILD_VERSION16__
											   ,
											   param.BasicParameterPtr->Texture3Index
#endif
		);
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
} // namespace EffekseerRenderer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_RIBBON_RENDERER_H__