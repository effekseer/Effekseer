
#include "FlipbookInterpolationUtils.fx"

void CalculateAndStoreAdvancedParameter(in float2 uv, in float2 uv1, in float4 alphaUV, in float4 uvDistortionUV, in float4 blendUV, in float4 blendAlphaUV, in float4 blendUVDistortionUV, in float flipbookIndexAndNextRate, in float modelAlphaThreshold, inout VS_Output vsoutput)
{
	// alpha texture
	vsoutput.Alpha_Dist_UV.x = uv.x * alphaUV.z + alphaUV.x;
	vsoutput.Alpha_Dist_UV.y = uv.y * alphaUV.w + alphaUV.y;

	// uv distortion texture
	vsoutput.Alpha_Dist_UV.z = uv.x * uvDistortionUV.z + uvDistortionUV.x;
	vsoutput.Alpha_Dist_UV.w = uv.y * uvDistortionUV.w + uvDistortionUV.y;

    // blend texture
	vsoutput.Blend_FBNextIndex_UV.x = uv.x * blendUV.z + blendUV.x;
	vsoutput.Blend_FBNextIndex_UV.y = uv.y * blendUV.w + blendUV.y;

	// blend alpha texture
	vsoutput.Blend_Alpha_Dist_UV.x = uv.x * blendAlphaUV.z + blendAlphaUV.x;
	vsoutput.Blend_Alpha_Dist_UV.y = uv.y * blendAlphaUV.w + blendAlphaUV.y;

	// blend uv distortion texture
	vsoutput.Blend_Alpha_Dist_UV.z = uv.x * blendUVDistortionUV.z + blendUVDistortionUV.x;
	vsoutput.Blend_Alpha_Dist_UV.w = uv.y * blendUVDistortionUV.w + blendUVDistortionUV.y;

	// flipbook interpolation
	float flipbookRate = 0.0f;
	float2 flipbookNextIndexUV = 0.0f;
	ApplyFlipbookVS(flipbookRate, flipbookNextIndexUV, fFlipbookParameter, flipbookIndexAndNextRate, uv1);

	vsoutput.Blend_FBNextIndex_UV.zw = flipbookNextIndexUV;
	vsoutput.UV_Others.z = flipbookRate;
	vsoutput.UV_Others.w = modelAlphaThreshold;

	// flip
	vsoutput.Alpha_Dist_UV.y = mUVInversed.x + mUVInversed.y * vsoutput.Alpha_Dist_UV.y;
	vsoutput.Alpha_Dist_UV.w = mUVInversed.x + mUVInversed.y * vsoutput.Alpha_Dist_UV.w;
	vsoutput.Blend_FBNextIndex_UV.y = mUVInversed.x + mUVInversed.y * vsoutput.Blend_FBNextIndex_UV.y;
	vsoutput.Blend_Alpha_Dist_UV.y = mUVInversed.x + mUVInversed.y * vsoutput.Blend_Alpha_Dist_UV.y;
	vsoutput.Blend_Alpha_Dist_UV.w = mUVInversed.x + mUVInversed.y * vsoutput.Blend_Alpha_Dist_UV.w;
}
