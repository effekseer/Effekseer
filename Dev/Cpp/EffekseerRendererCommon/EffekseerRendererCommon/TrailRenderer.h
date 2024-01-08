#pragma once

#include <Effekseer.h>
#include <assert.h>
#include <string.h>

#include "EffekseerRenderer.CommonUtils.h"

namespace EffekseerRenderer
{
class TrailRendererUtils
{
public:
	template <typename VERTEX, int TARGET>
	static void AssignUV(StrideView<VERTEX>& v, float uvX1, float uvX2, float uvX3, float uvY1, float uvY2)
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
			SetVertexAlphaUV(v[0], uvX1, 0);
			SetVertexAlphaUV(v[0], uvY1, 1);

			SetVertexAlphaUV(v[1], uvX2, 0);
			SetVertexAlphaUV(v[1], uvY1, 1);

			SetVertexAlphaUV(v[4], uvX2, 0);
			SetVertexAlphaUV(v[4], uvY1, 1);

			SetVertexAlphaUV(v[5], uvX3, 0);
			SetVertexAlphaUV(v[5], uvY1, 1);

			SetVertexAlphaUV(v[2], uvX1, 0);
			SetVertexAlphaUV(v[2], uvY2, 1);

			SetVertexAlphaUV(v[3], uvX2, 0);
			SetVertexAlphaUV(v[3], uvY2, 1);

			SetVertexAlphaUV(v[6], uvX2, 0);
			SetVertexAlphaUV(v[6], uvY2, 1);

			SetVertexAlphaUV(v[7], uvX3, 0);
			SetVertexAlphaUV(v[7], uvY2, 1);
		}
		else if (TARGET == 3)
		{
			SetVertexUVDistortionUV(v[0], uvX1, 0);
			SetVertexUVDistortionUV(v[0], uvY1, 1);

			SetVertexUVDistortionUV(v[1], uvX2, 0);
			SetVertexUVDistortionUV(v[1], uvY1, 1);

			SetVertexUVDistortionUV(v[4], uvX2, 0);
			SetVertexUVDistortionUV(v[4], uvY1, 1);

			SetVertexUVDistortionUV(v[5], uvX3, 0);
			SetVertexUVDistortionUV(v[5], uvY1, 1);

			SetVertexUVDistortionUV(v[2], uvX1, 0);
			SetVertexUVDistortionUV(v[2], uvY2, 1);

			SetVertexUVDistortionUV(v[3], uvX2, 0);
			SetVertexUVDistortionUV(v[3], uvY2, 1);

			SetVertexUVDistortionUV(v[6], uvX2, 0);
			SetVertexUVDistortionUV(v[6], uvY2, 1);

			SetVertexUVDistortionUV(v[7], uvX3, 0);
			SetVertexUVDistortionUV(v[7], uvY2, 1);
		}
		else if (TARGET == 4)
		{
			SetVertexBlendUV(v[0], uvX1, 0);
			SetVertexBlendUV(v[0], uvY1, 1);

			SetVertexBlendUV(v[1], uvX2, 0);
			SetVertexBlendUV(v[1], uvY1, 1);

			SetVertexBlendUV(v[4], uvX2, 0);
			SetVertexBlendUV(v[4], uvY1, 1);

			SetVertexBlendUV(v[5], uvX3, 0);
			SetVertexBlendUV(v[5], uvY1, 1);

			SetVertexBlendUV(v[2], uvX1, 0);
			SetVertexBlendUV(v[2], uvY2, 1);

			SetVertexBlendUV(v[3], uvX2, 0);
			SetVertexBlendUV(v[3], uvY2, 1);

			SetVertexBlendUV(v[6], uvX2, 0);
			SetVertexBlendUV(v[6], uvY2, 1);

			SetVertexBlendUV(v[7], uvX3, 0);
			SetVertexBlendUV(v[7], uvY2, 1);
		}
		else if (TARGET == 5)
		{
			SetVertexBlendAlphaUV(v[0], uvX1, 0);
			SetVertexBlendAlphaUV(v[0], uvY1, 1);

			SetVertexBlendAlphaUV(v[1], uvX2, 0);
			SetVertexBlendAlphaUV(v[1], uvY1, 1);

			SetVertexBlendAlphaUV(v[4], uvX2, 0);
			SetVertexBlendAlphaUV(v[4], uvY1, 1);

			SetVertexBlendAlphaUV(v[5], uvX3, 0);
			SetVertexBlendAlphaUV(v[5], uvY1, 1);

			SetVertexBlendAlphaUV(v[2], uvX1, 0);
			SetVertexBlendAlphaUV(v[2], uvY2, 1);

			SetVertexBlendAlphaUV(v[3], uvX2, 0);
			SetVertexBlendAlphaUV(v[3], uvY2, 1);

			SetVertexBlendAlphaUV(v[6], uvX2, 0);
			SetVertexBlendAlphaUV(v[6], uvY2, 1);

			SetVertexBlendAlphaUV(v[7], uvX3, 0);
			SetVertexBlendAlphaUV(v[7], uvY2, 1);
		}
		else if (TARGET == 6)
		{
			SetVertexBlendUVDistortionUV(v[0], uvX1, 0);
			SetVertexBlendUVDistortionUV(v[0], uvY1, 1);

			SetVertexBlendUVDistortionUV(v[1], uvX2, 0);
			SetVertexBlendUVDistortionUV(v[1], uvY1, 1);

			SetVertexBlendUVDistortionUV(v[4], uvX2, 0);
			SetVertexBlendUVDistortionUV(v[4], uvY1, 1);

			SetVertexBlendUVDistortionUV(v[5], uvX3, 0);
			SetVertexBlendUVDistortionUV(v[5], uvY1, 1);

			SetVertexBlendUVDistortionUV(v[2], uvX1, 0);
			SetVertexBlendUVDistortionUV(v[2], uvY2, 1);

			SetVertexBlendUVDistortionUV(v[3], uvX2, 0);
			SetVertexBlendUVDistortionUV(v[3], uvY2, 1);

			SetVertexBlendUVDistortionUV(v[6], uvX2, 0);
			SetVertexBlendUVDistortionUV(v[6], uvY2, 1);

			SetVertexBlendUVDistortionUV(v[7], uvX3, 0);
			SetVertexBlendUVDistortionUV(v[7], uvY2, 1);
		}
	}

	template <typename VERTEX, typename INSTANCE, int TARGET>
	static void AssignUVs(const Effekseer::NodeRendererTextureUVTypeParameter& uvParam, Effekseer::CustomAlignedVector<INSTANCE>& instances, StrideView<VERTEX> verteies, int spline_division, float globalScale)
	{
		float uvx = 0.0f;
		float uvw = 1.0f;
		float uvy = 0.0f;
		float uvh = 1.0f;

		if (uvParam.Type == ::Effekseer::TextureUVType::Strech)
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

				for (int32_t sploop = 0; sploop < spline_division; sploop++)
				{
					float percent1 = (float)(param.InstanceIndex * spline_division + sploop) /
									 (float)((param.InstanceCount - 1) * spline_division);

					float percent2 = (float)(param.InstanceIndex * spline_division + sploop + 1) /
									 (float)((param.InstanceCount - 1) * spline_division);

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
		else if (uvParam.Type == ::Effekseer::TextureUVType::TilePerParticle)
		{
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

					for (int32_t sploop = 0; sploop < spline_division; sploop++)
					{
						float percent1 = (float)(param.InstanceIndex * spline_division + sploop) /
										 (float)((uvParam.TileEdgeTail) * spline_division);

						float percent2 = (float)(param.InstanceIndex * spline_division + sploop + 1) /
										 (float)((uvParam.TileEdgeTail) * spline_division);

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

					for (int32_t sploop = 0; sploop < spline_division; sploop++)
					{
						const int headOffset = param.InstanceCount - 1 - uvParam.TileEdgeHead;
						float percent1 =
							(float)((param.InstanceIndex - headOffset) * spline_division + sploop) /
							(float)((uvParam.TileEdgeHead) * spline_division);

						float percent2 =
							(float)((param.InstanceIndex - headOffset) * spline_division + sploop + 1) /
							(float)((uvParam.TileEdgeHead) * spline_division);

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

					for (int32_t sploop = 0; sploop < spline_division; sploop++)
					{
						float percent1 = (float)(sploop) / (float)(spline_division);

						float percent2 = (float)(sploop + 1) / (float)(spline_division);

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
		else if (uvParam.Type == ::Effekseer::TextureUVType::Tile)
		{
			float current = 0.0f;
			float lengthAll = 0.0f;
			const float lengthPerUV = uvParam.TileLength * globalScale;
			for (size_t loop = 0; loop < instances.size() - 1; loop++)
			{
				const float length = (instances[loop + 1].SRTMatrix43.GetTranslation() - instances[loop].SRTMatrix43.GetTranslation()).GetLength();
				lengthAll += length;
			}

			current = lengthPerUV - fmod(lengthAll, lengthPerUV);

			verteies.Reset();
			for (size_t loop = 0; loop < instances.size() - 1; loop++)
			{
				const float length = (instances[loop + 1].SRTMatrix43.GetTranslation() - instances[loop].SRTMatrix43.GetTranslation()).GetLength();
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

				for (int32_t sploop = 0; sploop < spline_division; sploop++)
				{
					float percent1 = (float)(sploop) / (float)(spline_division);
					float percent2 = (float)(sploop + 1) / (float)(spline_division);
					float current1 = current + length * percent1;
					float current2 = current + length * percent2;

					auto uvX1 = uvx;
					auto uvX2 = uvx + uvw * 0.5f;
					auto uvX3 = uvx + uvw;

					auto uvY1 = uvy + (current1 / lengthPerUV) * uvh;
					auto uvY2 = uvy + (current2 / lengthPerUV) * uvh;

					AssignUV<VERTEX, TARGET>(verteies, uvX1, uvX2, uvX3, uvY1, uvY2);

					verteies += 8;
				}

				current += length;
			}
		}
	}
};
} // namespace EffekseerRenderer