﻿
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

template <typename RENDERER, bool FLIP_RGB_FLAG>
class TrackRendererBase : public ::Effekseer::TrackRenderer, public ::Effekseer::AlignedAllocationPolicy<16>
{
protected:
	RENDERER* m_renderer;
	int32_t m_ribbonCount;

	int32_t m_ringBufferOffset;
	uint8_t* m_ringBufferData;

	efkTrackNodeParam innstancesNodeParam;
	Effekseer::CustomAlignedVector<efkTrackInstanceParam> instances;
	Effekseer::SplineGenerator spline;

	int32_t vertexCount_ = 0;
	int32_t stride_ = 0;

	int32_t customData1Count_ = 0;
	int32_t customData2Count_ = 0;

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
			v[0].SetAlphaUV(uvX1, 0);
			v[0].SetAlphaUV(uvY1, 1);

			v[1].SetAlphaUV(uvX2, 0);
			v[1].SetAlphaUV(uvY1, 1);

			v[4].SetAlphaUV(uvX2, 0);
			v[4].SetAlphaUV(uvY1, 1);

			v[5].SetAlphaUV(uvX3, 0);
			v[5].SetAlphaUV(uvY1, 1);

			v[2].SetAlphaUV(uvX1, 0);
			v[2].SetAlphaUV(uvY2, 1);

			v[3].SetAlphaUV(uvX2, 0);
			v[3].SetAlphaUV(uvY2, 1);

			v[6].SetAlphaUV(uvX2, 0);
			v[6].SetAlphaUV(uvY2, 1);

			v[7].SetAlphaUV(uvX3, 0);
			v[7].SetAlphaUV(uvY2, 1);
		}
		else if (TARGET == 3)
		{
			v[0].SetUVDistortionUV(uvX1, 0);
			v[0].SetUVDistortionUV(uvY1, 1);

			v[1].SetUVDistortionUV(uvX2, 0);
			v[1].SetUVDistortionUV(uvY1, 1);

			v[4].SetUVDistortionUV(uvX2, 0);
			v[4].SetUVDistortionUV(uvY1, 1);

			v[5].SetUVDistortionUV(uvX3, 0);
			v[5].SetUVDistortionUV(uvY1, 1);

			v[2].SetUVDistortionUV(uvX1, 0);
			v[2].SetUVDistortionUV(uvY2, 1);

			v[3].SetUVDistortionUV(uvX2, 0);
			v[3].SetUVDistortionUV(uvY2, 1);

			v[6].SetUVDistortionUV(uvX2, 0);
			v[6].SetUVDistortionUV(uvY2, 1);

			v[7].SetUVDistortionUV(uvX3, 0);
			v[7].SetUVDistortionUV(uvY2, 1);
		}
		else if (TARGET == 4)
		{
			v[0].SetBlendUV(uvX1, 0);
			v[0].SetBlendUV(uvY1, 1);

			v[1].SetBlendUV(uvX2, 0);
			v[1].SetBlendUV(uvY1, 1);

			v[4].SetBlendUV(uvX2, 0);
			v[4].SetBlendUV(uvY1, 1);

			v[5].SetBlendUV(uvX3, 0);
			v[5].SetBlendUV(uvY1, 1);

			v[2].SetBlendUV(uvX1, 0);
			v[2].SetBlendUV(uvY2, 1);

			v[3].SetBlendUV(uvX2, 0);
			v[3].SetBlendUV(uvY2, 1);

			v[6].SetBlendUV(uvX2, 0);
			v[6].SetBlendUV(uvY2, 1);

			v[7].SetBlendUV(uvX3, 0);
			v[7].SetBlendUV(uvY2, 1);
		}
		else if (TARGET == 5)
		{
			v[0].SetBlendAlphaUV(uvX1, 0);
			v[0].SetBlendAlphaUV(uvY1, 1);

			v[1].SetBlendAlphaUV(uvX2, 0);
			v[1].SetBlendAlphaUV(uvY1, 1);

			v[4].SetBlendAlphaUV(uvX2, 0);
			v[4].SetBlendAlphaUV(uvY1, 1);

			v[5].SetBlendAlphaUV(uvX3, 0);
			v[5].SetBlendAlphaUV(uvY1, 1);

			v[2].SetBlendAlphaUV(uvX1, 0);
			v[2].SetBlendAlphaUV(uvY2, 1);

			v[3].SetBlendAlphaUV(uvX2, 0);
			v[3].SetBlendAlphaUV(uvY2, 1);

			v[6].SetBlendAlphaUV(uvX2, 0);
			v[6].SetBlendAlphaUV(uvY2, 1);

			v[7].SetBlendAlphaUV(uvX3, 0);
			v[7].SetBlendAlphaUV(uvY2, 1);
		}
		else if (TARGET == 6)
		{
			v[0].SetBlendUVDistortionUV(uvX1, 0);
			v[0].SetBlendUVDistortionUV(uvY1, 1);

			v[1].SetBlendUVDistortionUV(uvX2, 0);
			v[1].SetBlendUVDistortionUV(uvY1, 1);

			v[4].SetBlendUVDistortionUV(uvX2, 0);
			v[4].SetBlendUVDistortionUV(uvY1, 1);

			v[5].SetBlendUVDistortionUV(uvX3, 0);
			v[5].SetBlendUVDistortionUV(uvY1, 1);

			v[2].SetBlendUVDistortionUV(uvX1, 0);
			v[2].SetBlendUVDistortionUV(uvY2, 1);

			v[3].SetBlendUVDistortionUV(uvX2, 0);
			v[3].SetBlendUVDistortionUV(uvY2, 1);

			v[6].SetBlendUVDistortionUV(uvX2, 0);
			v[6].SetBlendUVDistortionUV(uvY2, 1);

			v[7].SetBlendUVDistortionUV(uvX3, 0);
			v[7].SetBlendUVDistortionUV(uvY2, 1);
		}
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
				else if (TARGET == 2)
				{
					uvx = param.AlphaUV.X;
					uvw = param.AlphaUV.Width;
					uvy = param.AlphaUV.Y;
					uvh = param.AlphaUV.Height;
				}
				else if (TARGET == 3)
				{
					uvx = param.UVDistortionUV.X;
					uvw = param.UVDistortionUV.Width;
					uvy = param.UVDistortionUV.Y;
					uvh = param.UVDistortionUV.Height;
				}
				else if (TARGET == 4)
				{
					uvx = param.BlendUV.X;
					uvw = param.BlendUV.Width;
					uvy = param.BlendUV.Y;
					uvh = param.BlendUV.Height;
				}
				else if (TARGET == 5)
				{
					uvx = param.BlendAlphaUV.X;
					uvw = param.BlendAlphaUV.Width;
					uvy = param.BlendAlphaUV.Y;
					uvh = param.BlendAlphaUV.Height;
				}
				else if (TARGET == 6)
				{
					uvx = param.BlendUVDistortionUV.X;
					uvw = param.BlendUVDistortionUV.Width;
					uvy = param.BlendUVDistortionUV.Y;
					uvh = param.BlendUVDistortionUV.Height;
				}

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
				else if (TARGET == 2)
				{
					uvx = param.AlphaUV.X;
					uvw = param.AlphaUV.Width;
					uvy = param.AlphaUV.Y;
					uvh = param.AlphaUV.Height;
				}
				else if (TARGET == 3)
				{
					uvx = param.UVDistortionUV.X;
					uvw = param.UVDistortionUV.Width;
					uvy = param.UVDistortionUV.Y;
					uvh = param.UVDistortionUV.Height;
				}
				else if (TARGET == 4)
				{
					uvx = param.BlendUV.X;
					uvw = param.BlendUV.Width;
					uvy = param.BlendUV.Y;
					uvh = param.BlendUV.Height;
				}
				else if (TARGET == 5)
				{
					uvx = param.BlendAlphaUV.X;
					uvw = param.BlendAlphaUV.Width;
					uvy = param.BlendAlphaUV.Y;
					uvh = param.BlendAlphaUV.Height;
				}
				else if (TARGET == 6)
				{
					uvx = param.BlendUVDistortionUV.X;
					uvw = param.BlendUVDistortionUV.Width;
					uvy = param.BlendUVDistortionUV.Y;
					uvh = param.BlendUVDistortionUV.Height;
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
						float percent1 =
							(float)((param.InstanceIndex - (param.InstanceCount - 1 - uvParam.TileEdgeHead)) * parameter.SplineDivision +
									sploop) /
							(float)((uvParam.TileEdgeHead) * parameter.SplineDivision);

						float percent2 =
							(float)((param.InstanceIndex - (param.InstanceCount - 1 - uvParam.TileEdgeHead)) * parameter.SplineDivision +
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

	template <typename VERTEX, bool FLIP_RGB>
	void RenderSplines(const ::Effekseer::Mat44f& camera)
	{
		if (instances.size() == 0)
		{
			return;
		}

		auto& parameter = innstancesNodeParam;

		// Calculate spline
		if (parameter.SplineDivision > 1)
		{
			spline.Reset();

			for (size_t loop = 0; loop < instances.size(); loop++)
			{
				auto p = efkVector3D();
				auto& param = instances[loop];

				auto mat = param.SRTMatrix43;

				if (parameter.EnableViewOffset == true)
				{
					ApplyViewOffset(mat, camera, param.ViewOffsetDistance);
				}

				ApplyDepthParameters(mat,
									 m_renderer->GetCameraFrontDirection(),
									 m_renderer->GetCameraPosition(),
									 // s,
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

				if (parameter.EnableViewOffset == true)
				{
					ApplyViewOffset(mat, camera, param.ViewOffsetDistance);
				}

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

				float percent = (float)(param.InstanceIndex * parameter.SplineDivision + sploop) /
								(float)((param.InstanceCount - 1) * parameter.SplineDivision);

				if (param.InstanceIndex < param.InstanceCount / 2)
				{
					float l = percent;
					l = l * 2.0f;
					size = param.SizeFor + (param.SizeMiddle - param.SizeFor) * l;

					leftColor.R = (uint8_t)Effekseer::Clamp(param.ColorLeft.R + (param.ColorLeftMiddle.R - param.ColorLeft.R) * l, 255, 0);
					leftColor.G = (uint8_t)Effekseer::Clamp(param.ColorLeft.G + (param.ColorLeftMiddle.G - param.ColorLeft.G) * l, 255, 0);
					leftColor.B = (uint8_t)Effekseer::Clamp(param.ColorLeft.B + (param.ColorLeftMiddle.B - param.ColorLeft.B) * l, 255, 0);
					leftColor.A = (uint8_t)Effekseer::Clamp(param.ColorLeft.A + (param.ColorLeftMiddle.A - param.ColorLeft.A) * l, 255, 0);

					centerColor.R =
						(uint8_t)Effekseer::Clamp(param.ColorCenter.R + (param.ColorCenterMiddle.R - param.ColorCenter.R) * l, 255, 0);
					centerColor.G =
						(uint8_t)Effekseer::Clamp(param.ColorCenter.G + (param.ColorCenterMiddle.G - param.ColorCenter.G) * l, 255, 0);
					centerColor.B =
						(uint8_t)Effekseer::Clamp(param.ColorCenter.B + (param.ColorCenterMiddle.B - param.ColorCenter.B) * l, 255, 0);
					centerColor.A =
						(uint8_t)Effekseer::Clamp(param.ColorCenter.A + (param.ColorCenterMiddle.A - param.ColorCenter.A) * l, 255, 0);

					rightColor.R =
						(uint8_t)Effekseer::Clamp(param.ColorRight.R + (param.ColorRightMiddle.R - param.ColorRight.R) * l, 255, 0);
					rightColor.G =
						(uint8_t)Effekseer::Clamp(param.ColorRight.G + (param.ColorRightMiddle.G - param.ColorRight.G) * l, 255, 0);
					rightColor.B =
						(uint8_t)Effekseer::Clamp(param.ColorRight.B + (param.ColorRightMiddle.B - param.ColorRight.B) * l, 255, 0);
					rightColor.A =
						(uint8_t)Effekseer::Clamp(param.ColorRight.A + (param.ColorRightMiddle.A - param.ColorRight.A) * l, 255, 0);
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

					centerColor.R =
						(uint8_t)Effekseer::Clamp(param.ColorCenter.R + (param.ColorCenterMiddle.R - param.ColorCenter.R) * l, 255, 0);
					centerColor.G =
						(uint8_t)Effekseer::Clamp(param.ColorCenter.G + (param.ColorCenterMiddle.G - param.ColorCenter.G) * l, 255, 0);
					centerColor.B =
						(uint8_t)Effekseer::Clamp(param.ColorCenter.B + (param.ColorCenterMiddle.B - param.ColorCenter.B) * l, 255, 0);
					centerColor.A =
						(uint8_t)Effekseer::Clamp(param.ColorCenter.A + (param.ColorCenterMiddle.A - param.ColorCenter.A) * l, 255, 0);

					rightColor.R =
						(uint8_t)Effekseer::Clamp(param.ColorRight.R + (param.ColorRightMiddle.R - param.ColorRight.R) * l, 255, 0);
					rightColor.G =
						(uint8_t)Effekseer::Clamp(param.ColorRight.G + (param.ColorRightMiddle.G - param.ColorRight.G) * l, 255, 0);
					rightColor.B =
						(uint8_t)Effekseer::Clamp(param.ColorRight.B + (param.ColorRightMiddle.B - param.ColorRight.B) * l, 255, 0);
					rightColor.A =
						(uint8_t)Effekseer::Clamp(param.ColorRight.A + (param.ColorRightMiddle.A - param.ColorRight.A) * l, 255, 0);
				}

				VERTEX v[3];

				v[0].Pos.X = (-size / 2.0f) * s.GetX();
				v[0].Pos.Y = 0.0f;
				v[0].Pos.Z = 0.0f;
				v[0].SetColor(leftColor, FLIP_RGB);

				v[1].Pos.X = 0.0f;
				v[1].Pos.Y = 0.0f;
				v[1].Pos.Z = 0.0f;
				v[1].SetColor(centerColor, FLIP_RGB);

				v[2].Pos.X = (size / 2.0f) * s.GetX();
				v[2].Pos.Y = 0.0f;
				v[2].Pos.Z = 0.0f;
				v[2].SetColor(rightColor, FLIP_RGB);

				v[0].SetFlipbookIndexAndNextRate(param.FlipbookIndexAndNextRate);
				v[1].SetFlipbookIndexAndNextRate(param.FlipbookIndexAndNextRate);
				v[2].SetFlipbookIndexAndNextRate(param.FlipbookIndexAndNextRate);

				v[0].SetAlphaThreshold(param.AlphaThreshold);
				v[1].SetAlphaThreshold(param.AlphaThreshold);
				v[2].SetAlphaThreshold(param.AlphaThreshold);

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

				if (IsDistortionVertex<VERTEX>())
				{
					const auto binormalVector = ToStruct(axis);
					vl.SetBinormal(binormalVector);
					vm.SetBinormal(binormalVector);
					vr.SetBinormal(binormalVector);

					::Effekseer::Vec3f tangent = vl.Pos - vr.Pos;
					tangent.Normalize();

					const auto tangentVector = ToStruct(tangent);

					vl.SetTangent(tangentVector);
					vm.SetTangent(tangentVector);
					vr.SetTangent(tangentVector);
				}
				else if (IsDynamicVertex<VERTEX>() || IsLightingVertex<VERTEX>())
				{
					::Effekseer::Vec3f tangent = SafeNormalize(Effekseer::Vec3f(vl.Pos - vr.Pos));
					Effekseer::Vec3f normal = SafeNormalize(Effekseer::Vec3f::Cross(tangent, axis));

					if (!parameter.IsRightHand)
					{
						normal = -normal;
					}

					Effekseer::Color normal_ = PackVector3DF(normal);
					Effekseer::Color tangent_ = PackVector3DF(tangent);

					vl.SetPackedNormal(normal_);
					vm.SetPackedNormal(normal_);
					vr.SetPackedNormal(normal_);

					vl.SetPackedTangent(tangent_);
					vm.SetPackedTangent(tangent_);
					vr.SetPackedTangent(tangent_);
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

		if (IsDynamicVertex<VERTEX>() || IsLightingVertex<VERTEX>())
		{
			AssignUVs<VERTEX, 1>(parameter, verteies);
		}

		AssignUVs<VERTEX, 2>(parameter, verteies);
		AssignUVs<VERTEX, 3>(parameter, verteies);
		AssignUVs<VERTEX, 4>(parameter, verteies);
		AssignUVs<VERTEX, 5>(parameter, verteies);
		AssignUVs<VERTEX, 6>(parameter, verteies);

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
			StrideView<float> custom(m_ringBufferData + sizeof(DynamicVertex) + sizeof(float) * customData1Count_, stride_, vertexCount_);
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
		, m_ringBufferData(nullptr)
	{
	}

	virtual ~TrackRendererBase()
	{
	}

protected:
	void Rendering_(const efkTrackNodeParam& parameter,
					const efkTrackInstanceParam& instanceParameter,
					void* userData,
					const ::Effekseer::Mat44f& camera)
	{
		const auto& state = m_renderer->GetStandardRenderer()->GetState();
		const ShaderParameterCollector& collector = state.Collector;

		if (collector.ShaderType == RendererShaderType::Material)
		{
			Rendering_Internal<DynamicVertex, FLIP_RGB_FLAG>(parameter, instanceParameter, userData, camera);
		}
		else if (collector.ShaderType == RendererShaderType::AdvancedLit)
		{
			Rendering_Internal<AdvancedLightingVertex, FLIP_RGB_FLAG>(parameter, instanceParameter, userData, camera);
		}
		else if (collector.ShaderType == RendererShaderType::AdvancedBackDistortion)
		{
			Rendering_Internal<AdvancedVertexDistortion, FLIP_RGB_FLAG>(parameter, instanceParameter, userData, camera);
		}
		else if (collector.ShaderType == RendererShaderType::AdvancedUnlit)
		{
			Rendering_Internal<AdvancedSimpleVertex, FLIP_RGB_FLAG>(parameter, instanceParameter, userData, camera);
		}
		else if (collector.ShaderType == RendererShaderType::Lit)
		{
			Rendering_Internal<LightingVertex, FLIP_RGB_FLAG>(parameter, instanceParameter, userData, camera);
		}
		else if (collector.ShaderType == RendererShaderType::BackDistortion)
		{
			Rendering_Internal<VertexDistortion, FLIP_RGB_FLAG>(parameter, instanceParameter, userData, camera);
		}
		else
		{
			Rendering_Internal<SimpleVertex, FLIP_RGB_FLAG>(parameter, instanceParameter, userData, camera);
		}
	}

	template <typename VERTEX, bool FLIP_RGB>
	void Rendering_Internal(const efkTrackNodeParam& parameter,
							const efkTrackInstanceParam& instanceParameter,
							void* userData,
							const ::Effekseer::Mat44f& camera)
	{
		if (m_ringBufferData == nullptr)
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
			RenderSplines<VERTEX, FLIP_RGB>(camera);
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
		
		state.EnableInterpolation = param.BasicParameterPtr->EnableInterpolation;
		state.UVLoopType = param.BasicParameterPtr->UVLoopType;
		state.InterpolationType = param.BasicParameterPtr->InterpolationType;
		state.FlipbookDivideX = param.BasicParameterPtr->FlipbookDivideX;
		state.FlipbookDivideY = param.BasicParameterPtr->FlipbookDivideY;

		state.UVDistortionIntensity = param.BasicParameterPtr->UVDistortionIntensity;

		state.TextureBlendType = param.BasicParameterPtr->TextureBlendType;

		state.BlendUVDistortionIntensity = param.BasicParameterPtr->BlendUVDistortionIntensity;

		state.EmissiveScaling = param.BasicParameterPtr->EmissiveScaling;

		state.EdgeThreshold = param.BasicParameterPtr->EdgeThreshold;
		state.EdgeColor[0] = param.BasicParameterPtr->EdgeColor[0];
		state.EdgeColor[1] = param.BasicParameterPtr->EdgeColor[1];
		state.EdgeColor[2] = param.BasicParameterPtr->EdgeColor[2];
		state.EdgeColor[3] = param.BasicParameterPtr->EdgeColor[3];
		state.EdgeColorScaling = param.BasicParameterPtr->EdgeColorScaling;
		state.IsAlphaCuttoffEnabled = param.BasicParameterPtr->IsAlphaCutoffEnabled;
		state.SoftParticleDistance = param.BasicParameterPtr->SoftParticleDistance;

		state.Distortion = param.BasicParameterPtr->MaterialType == Effekseer::RendererMaterialType::BackDistortion;
		state.DistortionIntensity = param.BasicParameterPtr->DistortionIntensity;
		state.MaterialType = param.BasicParameterPtr->MaterialType;

		state.CopyMaterialFromParameterToState(
			m_renderer,
			param.EffectPointer,
			param.BasicParameterPtr);

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