
#ifndef __EFFEKSEERRENDERER_TRACK_RENDERER_BASE_H__
#define __EFFEKSEERRENDERER_TRACK_RENDERER_BASE_H__

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
typedef ::Effekseer::TrackRenderer::NodeParameter efkTrackNodeParam;
typedef ::Effekseer::TrackRenderer::InstanceParameter efkTrackInstanceParam;
typedef ::Effekseer::Vec3f efkVector3D;

template <typename RENDERER, typename VERTEX_NORMAL, typename VERTEX_DISTORTION>
class TrackRendererBase
	: public ::Effekseer::TrackRenderer,
	  public ::Effekseer::AlignedAllocationPolicy<16>
{
protected:
	RENDERER* m_renderer;
	int32_t m_ribbonCount;

	int32_t m_ringBufferOffset;
	uint8_t* m_ringBufferData;

	efkTrackNodeParam innstancesNodeParam;
	Effekseer::CustomAlignedVector<efkTrackInstanceParam> instances;
	SplineGenerator spline;

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
	void AssignUV(StrideView<VERTEX>& v, float uvX1, float uvX2, float uvX3, float uvY1, float uvY2)
	{
		if (TARGET == 0)
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
#ifdef __EFFEKSEER_BUILD_VERSION16__
		else if (TARGET == 1)
		{
			v[0].UV2[0] = uvX1;
			v[0].UV2[1] = uvY1;

			v[1].UV2[0] = uvX2;
			v[1].UV2[1] = uvY1;

			v[4].UV2[0] = uvX2;
			v[4].UV2[1] = uvY1;

			v[5].UV2[0] = uvX3;
			v[5].UV2[1] = uvY1;

			v[2].UV2[0] = uvX1;
			v[2].UV2[1] = uvY2;

			v[3].UV2[0] = uvX2;
			v[3].UV2[1] = uvY2;

			v[6].UV2[0] = uvX2;
			v[6].UV2[1] = uvY2;

			v[7].UV2[0] = uvX3;
			v[7].UV2[1] = uvY2;
		}
		else if (TARGET == 2)
		{
			v[0].AlphaUV[0] = uvX1;
			v[0].AlphaUV[1] = uvY1;

			v[1].AlphaUV[0] = uvX2;
			v[1].AlphaUV[1] = uvY1;

			v[4].AlphaUV[0] = uvX2;
			v[4].AlphaUV[1] = uvY1;

			v[5].AlphaUV[0] = uvX3;
			v[5].AlphaUV[1] = uvY1;

			v[2].AlphaUV[0] = uvX1;
			v[2].AlphaUV[1] = uvY2;

			v[3].AlphaUV[0] = uvX2;
			v[3].AlphaUV[1] = uvY2;

			v[6].AlphaUV[0] = uvX2;
			v[6].AlphaUV[1] = uvY2;

			v[7].AlphaUV[0] = uvX3;
			v[7].AlphaUV[1] = uvY2;
		}
#else
		else
		{
			v[0].UV2[0] = uvX1;
			v[0].UV2[1] = uvY1;

			v[1].UV2[0] = uvX2;
			v[1].UV2[1] = uvY1;

			v[4].UV2[0] = uvX2;
			v[4].UV2[1] = uvY1;

			v[5].UV2[0] = uvX3;
			v[5].UV2[1] = uvY1;

			v[2].UV2[0] = uvX1;
			v[2].UV2[1] = uvY2;

			v[3].UV2[0] = uvX2;
			v[3].UV2[1] = uvY2;

			v[6].UV2[0] = uvX2;
			v[6].UV2[1] = uvY2;

			v[7].UV2[0] = uvX3;
			v[7].UV2[1] = uvY2;
		}
#endif
	}

	template <typename VERTEX, int TARGET>
	void AssignUVs(efkTrackNodeParam& parameter, StrideView<VERTEX> verteies)
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
					auto uvX2 = uvx + uvw * 0.5f;
					auto uvX3 = uvx + uvw;
					auto uvY1 = uvy + percent1 * uvh;
					auto uvY2 = uvy + percent2 * uvh;

					AssignUV<VERTEX, TARGET>(verteies, uvX1, uvX2, uvX3, uvY1, uvY2);

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
						auto uvX2 = uvx + uvw * 0.5f;
						auto uvX3 = uvx + uvw;
						auto uvY1 = uvBegin + (uvEnd - uvBegin) * percent1;
						auto uvY2 = uvBegin + (uvEnd - uvBegin) * percent2;

						AssignUV<VERTEX, TARGET>(verteies, uvX1, uvX2, uvX3, uvY1, uvY2);

						verteies += 8;
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
						auto uvX2 = uvx + uvw * 0.5f;
						auto uvX3 = uvx + uvw;
						auto uvY1 = uvBegin + (uvEnd - uvBegin) * percent1;
						auto uvY2 = uvBegin + (uvEnd - uvBegin) * percent2;

						AssignUV<VERTEX, TARGET>(verteies, uvX1, uvX2, uvX3, uvY1, uvY2);

						verteies += 8;
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
						auto uvX2 = uvx + uvx + uvw * 0.5f;
						auto uvX3 = uvx + uvw;
						auto uvY1 = uvBegin + (uvEnd - uvBegin) * percent1;
						auto uvY2 = uvBegin + (uvEnd - uvBegin) * percent2;

						AssignUV<VERTEX, TARGET>(verteies, uvX1, uvX2, uvX3, uvY1, uvY2);

						verteies += 8;
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
			spline.Reset();

			for (size_t loop = 0; loop < instances.size(); loop++)
			{
				auto p = efkVector3D();
				auto& param = instances[loop];

				auto mat = param.SRTMatrix43;

				ApplyDepthParameters(mat,
									 m_renderer->GetCameraFrontDirection(),
									 m_renderer->GetCameraPosition(),
									 //s,
									 parameter.DepthParameterPtr,
									 parameter.IsRightHand);

				p = mat.GetTranslation();
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
				auto mat = param.SRTMatrix43;

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

				bool isFirst = param.InstanceIndex == 0 && sploop == 0;
				bool isLast = param.InstanceIndex == (param.InstanceCount - 1);

				float size = 0.0f;
				::Effekseer::Color leftColor;
				::Effekseer::Color centerColor;
				::Effekseer::Color rightColor;

				float percent = (float)(param.InstanceIndex * parameter.SplineDivision + sploop) / (float)((param.InstanceCount - 1) * parameter.SplineDivision);

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

				VERTEX v[3];

				v[0].Pos.X = (-size / 2.0f) * s.GetX();
				v[0].Pos.Y = 0.0f;
				v[0].Pos.Z = 0.0f;
				v[0].SetColor(leftColor);

				v[1].Pos.X = 0.0f;
				v[1].Pos.Y = 0.0f;
				v[1].Pos.Z = 0.0f;
				v[1].SetColor(centerColor);

				v[2].Pos.X = (size / 2.0f) * s.GetX();
				v[2].Pos.Y = 0.0f;
				v[2].Pos.Z = 0.0f;
				v[2].SetColor(rightColor);

#ifdef __EFFEKSEER_BUILD_VERSION16__
				v[0].FlipbookIndexAndNextRate = param.FlipbookIndexAndNextRate;
				v[1].FlipbookIndexAndNextRate = param.FlipbookIndexAndNextRate;
				v[2].FlipbookIndexAndNextRate = param.FlipbookIndexAndNextRate;

				v[0].AlphaThreshold = param.AlphaThreshold;
				v[1].AlphaThreshold = param.AlphaThreshold;
				v[2].AlphaThreshold = param.AlphaThreshold;
#endif

				if (parameter.SplineDivision > 1)
				{
					v[1].Pos = ToStruct(spline.GetValue(param.InstanceIndex + sploop / (float)parameter.SplineDivision));
				}
				else
				{
					v[1].Pos = ToStruct(t);
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
			Effekseer::Vec3f axisBefore;

			for (size_t i = 0; i < (instances.size() - 1) * parameter.SplineDivision + 1; i++)
			{
				bool isFirst_ = (i == 0);
				bool isLast_ = (i == ((instances.size() - 1) * parameter.SplineDivision));
				Effekseer::Vec3f axis;
				Effekseer::Vec3f pos;

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
					axis = vs_[9].Pos - vs_[7].Pos;
					axis = SafeNormalize(axis);
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

				::Effekseer::Vec3f F;
				::Effekseer::Vec3f R;
				::Effekseer::Vec3f U;

				// It can be optimized because X is only not zero.
				/*
					U = axis;

					F = ::Effekseer::Vec3f(m_renderer->GetCameraFrontDirection()).Normalize();
					R = ::Effekseer::Vec3f::Cross(U, F).Normalize();
					F = ::Effekseer::Vec3f::Cross(R, U).Normalize();
					
					::Effekseer::Mat43f mat_rot(
						-R.GetX(), -R.GetY(), -R.GetZ(),
						 U.GetX(),  U.GetY(),  U.GetZ(),
						 F.GetX(),  F.GetY(),  F.GetZ(),
						pos.GetX(), pos.GetY(), pos.GetZ());

					vl.Pos = ToStruct(::Effekseer::Vec3f::Transform(vl.Pos, mat_rot));
					vm.Pos = ToStruct(::Effekseer::Vec3f::Transform(vm.Pos, mat_rot));
					vr.Pos = ToStruct(::Effekseer::Vec3f::Transform(vr.Pos,mat_rot));
					*/

				U = axis;
				F = m_renderer->GetCameraFrontDirection();
				R = SafeNormalize(::Effekseer::Vec3f::Cross(U, F));

				assert(vl.Pos.Y == 0.0f);
				assert(vr.Pos.Y == 0.0f);
				assert(vl.Pos.Z == 0.0f);
				assert(vr.Pos.Z == 0.0f);
				assert(vm.Pos.X == 0.0f);
				assert(vm.Pos.Y == 0.0f);
				assert(vm.Pos.Z == 0.0f);

				vl.Pos = ToStruct(-R * vl.Pos.X + pos);
				vm.Pos = ToStruct(pos);
				vr.Pos = ToStruct(-R * vr.Pos.X + pos);

				if (vertexType == VertexType::Distortion)
				{
					auto vl_ = (VERTEX_DISTORTION*)(&vl);
					auto vm_ = (VERTEX_DISTORTION*)(&vm);
					auto vr_ = (VERTEX_DISTORTION*)(&vr);

					vl_->Binormal = vm_->Binormal = vr_->Binormal = ToStruct(axis);

					::Effekseer::Vec3f tangent = vl_->Pos - vr_->Pos;
					tangent.Normalize();

					vl_->Tangent = vm_->Tangent = vr_->Tangent = ToStruct(tangent);
				}
				else if (vertexType == VertexType::Dynamic)
				{
					auto vl_ = (DynamicVertex*)(&vl);
					auto vm_ = (DynamicVertex*)(&vm);
					auto vr_ = (DynamicVertex*)(&vr);

					::Effekseer::Vec3f tangent = SafeNormalize(Effekseer::Vec3f(vl_->Pos - vr_->Pos));
					Effekseer::Vec3f normal = SafeNormalize(Effekseer::Vec3f::Cross(tangent, axis));

					if (!parameter.IsRightHand)
					{
						normal = -normal;
					}

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
		AssignUVs<VERTEX, 0>(parameter, verteies);

		if (vertexType == VertexType::Dynamic)
		{
			AssignUVs<VERTEX, 1>(parameter, verteies);
		}

#ifdef __EFFEKSEER_BUILD_VERSION16__
		AssignUVs<VERTEX, 2>(parameter, verteies);
#endif

		// custom parameter
		if (customData1Count_ > 0)
		{
			StrideView<float> custom(m_ringBufferData + sizeof(DynamicVertex), stride_, vertexCount_);
			for (size_t loop = 0; loop < instances.size() - 1; loop++)
			{
				auto& param = instances[loop];

				for (int32_t sploop = 0; sploop < parameter.SplineDivision; sploop++)
				{
					for (size_t i = 0; i < 8; i++)
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
					for (size_t i = 0; i < 8; i++)
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
	void Rendering_(const efkTrackNodeParam& parameter, const efkTrackInstanceParam& instanceParameter, void* userData, const ::Effekseer::Mat44f& camera)
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
	void Rendering_Internal(const efkTrackNodeParam& parameter, const efkTrackInstanceParam& instanceParameter, void* userData, const ::Effekseer::Mat44f& camera)
	{
		if (m_ringBufferData == NULL)
			return;
		if (instanceParameter.InstanceCount < 2)
			return;

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
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRenderer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_RIBBON_RENDERER_H__