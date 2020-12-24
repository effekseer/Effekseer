
#include "FlipbookInterpolationUtils_PS.fx"
#include "TextureBlendingUtils_PS.fx"
#include "UVDistortionUtils_PS.fx"

struct AdvancedParameter
{
	float2 AlphaUV;
	float2 UVDistortionUV;
	float2 BlendUV;
	float2 BlendAlphaUV;
	float2 BlendUVDistortionUV;
	float2 FlipbookNextIndexUV;
	float FlipbookRate;
	float AlphaThreshold;
};

AdvancedParameter DisolveAdvancedParameter(in PS_Input psinput)
{
	AdvancedParameter ret;
	ret.AlphaUV = psinput.Alpha_Dist_UV.xy;
	ret.UVDistortionUV = psinput.Alpha_Dist_UV.zw;
	ret.BlendUV = psinput.Blend_FBNextIndex_UV.xy;
	ret.BlendAlphaUV = psinput.Blend_Alpha_Dist_UV.xy;
	ret.BlendUVDistortionUV = psinput.Blend_Alpha_Dist_UV.zw;
	ret.FlipbookNextIndexUV = psinput.Blend_FBNextIndex_UV.zw;
	ret.FlipbookRate = psinput.UV_Others.z;
	ret.AlphaThreshold = psinput.UV_Others.w;
	return ret;
}

