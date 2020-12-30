
#include "FlipbookInterpolationUtils.fx"

void CalculateAndStoreAdvancedParameter(in VS_Input vsinput, inout VS_Output vsoutput)
{
	// alpha uv distortion texture
	vsoutput.Alpha_Dist_UV = vsinput.Alpha_Dist_UV;
	vsoutput.Alpha_Dist_UV.y = mUVInversed.x + mUVInversed.y * vsinput.Alpha_Dist_UV.y;
	vsoutput.Alpha_Dist_UV.w = mUVInversed.x + mUVInversed.y * vsinput.Alpha_Dist_UV.w;

	// blend texture
	vsoutput.Blend_FBNextIndex_UV.xy = vsinput.BlendUV;
	vsoutput.Blend_FBNextIndex_UV.y = mUVInversed.x + mUVInversed.y * vsinput.BlendUV.y;

	// blend alpha uv distortion texture
	vsoutput.Blend_Alpha_Dist_UV = vsinput.Blend_Alpha_Dist_UV;
	vsoutput.Blend_Alpha_Dist_UV.y = mUVInversed.x + mUVInversed.y * vsinput.Blend_Alpha_Dist_UV.y;
	vsoutput.Blend_Alpha_Dist_UV.w = mUVInversed.x + mUVInversed.y * vsinput.Blend_Alpha_Dist_UV.w;

	// flipbook interpolation
	float flipbookRate = 0.0f;
	float2 flipbookNextIndexUV = 0.0f;
	ApplyFlipbookVS(flipbookRate, flipbookNextIndexUV, fFlipbookParameter, vsinput.FlipbookIndex, vsoutput.UV_Others.xy);

	vsoutput.Blend_FBNextIndex_UV.zw = flipbookNextIndexUV;
	vsoutput.UV_Others.z = flipbookRate;
	vsoutput.UV_Others.w = vsinput.AlphaThreshold;
}