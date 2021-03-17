#version 120
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_Output
{
    vec4 PosVS;
    vec4 Color;
    vec4 UV_Others;
    vec3 WorldN;
    vec4 Alpha_Dist_UV;
    vec4 Blend_Alpha_Dist_UV;
    vec4 Blend_FBNextIndex_UV;
};

struct VS_Input
{
    vec3 Pos;
    vec3 Normal;
    vec3 Binormal;
    vec3 Tangent;
    vec2 UV;
    vec4 Color;
};

struct VS_ConstantBuffer
{
    mat4 mCameraProj;
    mat4 mModel;
    vec4 fUV;
    vec4 fAlphaUV;
    vec4 fUVDistortionUV;
    vec4 fBlendUV;
    vec4 fBlendAlphaUV;
    vec4 fBlendUVDistortionUV;
    vec4 fFlipbookParameter;
    vec4 fFlipbookIndexAndNextRate;
    vec4 fModelAlphaThreshold;
    vec4 fModelColor;
    vec4 fLightDirection;
    vec4 fLightColor;
    vec4 fLightAmbient;
    vec4 mUVInversed;
};

uniform VS_ConstantBuffer CBVS0;

attribute vec3 Input_Pos;
attribute vec3 Input_Normal;
attribute vec3 Input_Binormal;
attribute vec3 Input_Tangent;
attribute vec2 Input_UV;
attribute vec4 Input_Color;
centroid varying vec4 _VSPS_Color;
centroid varying vec4 _VSPS_UV_Others;
varying vec3 _VSPS_WorldN;
varying vec4 _VSPS_Alpha_Dist_UV;
varying vec4 _VSPS_Blend_Alpha_Dist_UV;
varying vec4 _VSPS_Blend_FBNextIndex_UV;

float IntMod(float x, float y)
{
    return floor(mod(x, y));
}

vec2 GetFlipbookOneSizeUV(float DivideX, float DivideY)
{
    return vec2(1.0) / vec2(DivideX, DivideY);
}

vec2 GetFlipbookOriginUV(vec2 FlipbookUV, float FlipbookIndex, float DivideX, float DivideY)
{
    float param = FlipbookIndex;
    float param_1 = DivideX;
    vec2 DivideIndex;
    DivideIndex.x = IntMod(param, param_1);
    DivideIndex.y = float(int(FlipbookIndex) / int(DivideX));
    float param_2 = DivideX;
    float param_3 = DivideY;
    vec2 FlipbookOneSize = GetFlipbookOneSizeUV(param_2, param_3);
    vec2 UVOffset = DivideIndex * FlipbookOneSize;
    vec2 OriginUV = FlipbookUV - UVOffset;
    OriginUV *= vec2(DivideX, DivideY);
    return OriginUV;
}

vec2 GetFlipbookUVForIndex(vec2 OriginUV, float Index, float DivideX, float DivideY)
{
    float param = Index;
    float param_1 = DivideX;
    vec2 DivideIndex;
    DivideIndex.x = IntMod(param, param_1);
    DivideIndex.y = float(int(Index) / int(DivideX));
    float param_2 = DivideX;
    float param_3 = DivideY;
    vec2 FlipbookOneSize = GetFlipbookOneSizeUV(param_2, param_3);
    return (OriginUV * FlipbookOneSize) + (DivideIndex * FlipbookOneSize);
}

void ApplyFlipbookVS(inout float flipbookRate, inout vec2 flipbookUV, vec4 flipbookParameter, float flipbookIndex, vec2 uv)
{
    if (flipbookParameter.x > 0.0)
    {
        flipbookRate = fract(flipbookIndex);
        float Index = floor(flipbookIndex);
        float IndexOffset = 1.0;
        float NextIndex = Index + IndexOffset;
        float FlipbookMaxCount = flipbookParameter.z * flipbookParameter.w;
        if (flipbookParameter.y == 0.0)
        {
            if (NextIndex >= FlipbookMaxCount)
            {
                NextIndex = FlipbookMaxCount - 1.0;
                Index = FlipbookMaxCount - 1.0;
            }
        }
        else
        {
            if (flipbookParameter.y == 1.0)
            {
                Index = mod(Index, FlipbookMaxCount);
                NextIndex = mod(NextIndex, FlipbookMaxCount);
            }
            else
            {
                if (flipbookParameter.y == 2.0)
                {
                    bool Reverse = mod(floor(Index / FlipbookMaxCount), 2.0) == 1.0;
                    Index = mod(Index, FlipbookMaxCount);
                    if (Reverse)
                    {
                        Index = (FlipbookMaxCount - 1.0) - floor(Index);
                    }
                    Reverse = mod(floor(NextIndex / FlipbookMaxCount), 2.0) == 1.0;
                    NextIndex = mod(NextIndex, FlipbookMaxCount);
                    if (Reverse)
                    {
                        NextIndex = (FlipbookMaxCount - 1.0) - floor(NextIndex);
                    }
                }
            }
        }
        vec2 param = uv;
        float param_1 = Index;
        float param_2 = flipbookParameter.z;
        float param_3 = flipbookParameter.w;
        vec2 OriginUV = GetFlipbookOriginUV(param, param_1, param_2, param_3);
        vec2 param_4 = OriginUV;
        float param_5 = NextIndex;
        float param_6 = flipbookParameter.z;
        float param_7 = flipbookParameter.w;
        flipbookUV = GetFlipbookUVForIndex(param_4, param_5, param_6, param_7);
    }
}

void CalculateAndStoreAdvancedParameter(vec2 uv, vec2 uv1, vec4 alphaUV, vec4 uvDistortionUV, vec4 blendUV, vec4 blendAlphaUV, vec4 blendUVDistortionUV, float flipbookIndexAndNextRate, float modelAlphaThreshold, inout VS_Output vsoutput)
{
    vsoutput.Alpha_Dist_UV.x = (uv.x * alphaUV.z) + alphaUV.x;
    vsoutput.Alpha_Dist_UV.y = (uv.y * alphaUV.w) + alphaUV.y;
    vsoutput.Alpha_Dist_UV.z = (uv.x * uvDistortionUV.z) + uvDistortionUV.x;
    vsoutput.Alpha_Dist_UV.w = (uv.y * uvDistortionUV.w) + uvDistortionUV.y;
    vsoutput.Blend_FBNextIndex_UV.x = (uv.x * blendUV.z) + blendUV.x;
    vsoutput.Blend_FBNextIndex_UV.y = (uv.y * blendUV.w) + blendUV.y;
    vsoutput.Blend_Alpha_Dist_UV.x = (uv.x * blendAlphaUV.z) + blendAlphaUV.x;
    vsoutput.Blend_Alpha_Dist_UV.y = (uv.y * blendAlphaUV.w) + blendAlphaUV.y;
    vsoutput.Blend_Alpha_Dist_UV.z = (uv.x * blendUVDistortionUV.z) + blendUVDistortionUV.x;
    vsoutput.Blend_Alpha_Dist_UV.w = (uv.y * blendUVDistortionUV.w) + blendUVDistortionUV.y;
    float flipbookRate = 0.0;
    vec2 flipbookNextIndexUV = vec2(0.0);
    float param = flipbookRate;
    vec2 param_1 = flipbookNextIndexUV;
    vec4 param_2 = CBVS0.fFlipbookParameter;
    float param_3 = flipbookIndexAndNextRate;
    vec2 param_4 = uv1;
    ApplyFlipbookVS(param, param_1, param_2, param_3, param_4);
    flipbookRate = param;
    flipbookNextIndexUV = param_1;
    vsoutput.Blend_FBNextIndex_UV = vec4(vsoutput.Blend_FBNextIndex_UV.x, vsoutput.Blend_FBNextIndex_UV.y, flipbookNextIndexUV.x, flipbookNextIndexUV.y);
    vsoutput.UV_Others.z = flipbookRate;
    vsoutput.UV_Others.w = modelAlphaThreshold;
    vsoutput.Alpha_Dist_UV.y = CBVS0.mUVInversed.x + (CBVS0.mUVInversed.y * vsoutput.Alpha_Dist_UV.y);
    vsoutput.Alpha_Dist_UV.w = CBVS0.mUVInversed.x + (CBVS0.mUVInversed.y * vsoutput.Alpha_Dist_UV.w);
    vsoutput.Blend_FBNextIndex_UV.y = CBVS0.mUVInversed.x + (CBVS0.mUVInversed.y * vsoutput.Blend_FBNextIndex_UV.y);
    vsoutput.Blend_Alpha_Dist_UV.y = CBVS0.mUVInversed.x + (CBVS0.mUVInversed.y * vsoutput.Blend_Alpha_Dist_UV.y);
    vsoutput.Blend_Alpha_Dist_UV.w = CBVS0.mUVInversed.x + (CBVS0.mUVInversed.y * vsoutput.Blend_Alpha_Dist_UV.w);
}

VS_Output _main(VS_Input Input)
{
    vec4 uv = CBVS0.fUV;
    vec4 alphaUV = CBVS0.fAlphaUV;
    vec4 uvDistortionUV = CBVS0.fUVDistortionUV;
    vec4 blendUV = CBVS0.fBlendUV;
    vec4 blendAlphaUV = CBVS0.fBlendAlphaUV;
    vec4 blendUVDistortionUV = CBVS0.fBlendUVDistortionUV;
    vec4 modelColor = CBVS0.fModelColor * Input.Color;
    float flipbookIndexAndNextRate = CBVS0.fFlipbookIndexAndNextRate.x;
    float modelAlphaThreshold = CBVS0.fModelAlphaThreshold.x;
    VS_Output Output = VS_Output(vec4(0.0), vec4(0.0), vec4(0.0), vec3(0.0), vec4(0.0), vec4(0.0), vec4(0.0));
    vec4 localPosition = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    vec4 worldPos = CBVS0.mModel * localPosition;
    Output.PosVS = CBVS0.mCameraProj * worldPos;
    vec2 outputUV = Input.UV;
    outputUV.x = (outputUV.x * uv.z) + uv.x;
    outputUV.y = (outputUV.y * uv.w) + uv.y;
    outputUV.y = CBVS0.mUVInversed.x + (CBVS0.mUVInversed.y * outputUV.y);
    Output.UV_Others = vec4(outputUV.x, outputUV.y, Output.UV_Others.z, Output.UV_Others.w);
    vec4 localNormal = vec4(Input.Normal.x, Input.Normal.y, Input.Normal.z, 0.0);
    localNormal = normalize(CBVS0.mModel * localNormal);
    Output.WorldN = localNormal.xyz;
    Output.Color = modelColor;
    vec2 param = Input.UV;
    vec2 param_1 = Output.UV_Others.xy;
    vec4 param_2 = alphaUV;
    vec4 param_3 = uvDistortionUV;
    vec4 param_4 = blendUV;
    vec4 param_5 = blendAlphaUV;
    vec4 param_6 = blendUVDistortionUV;
    float param_7 = flipbookIndexAndNextRate;
    float param_8 = modelAlphaThreshold;
    VS_Output param_9 = Output;
    CalculateAndStoreAdvancedParameter(param, param_1, param_2, param_3, param_4, param_5, param_6, param_7, param_8, param_9);
    Output = param_9;
    return Output;
}

void main()
{
    VS_Input Input;
    Input.Pos = Input_Pos;
    Input.Normal = Input_Normal;
    Input.Binormal = Input_Binormal;
    Input.Tangent = Input_Tangent;
    Input.UV = Input_UV;
    Input.Color = Input_Color;
    VS_Output flattenTemp = _main(Input);
    gl_Position = flattenTemp.PosVS;
    _VSPS_Color = flattenTemp.Color;
    _VSPS_UV_Others = flattenTemp.UV_Others;
    _VSPS_WorldN = flattenTemp.WorldN;
    _VSPS_Alpha_Dist_UV = flattenTemp.Alpha_Dist_UV;
    _VSPS_Blend_Alpha_Dist_UV = flattenTemp.Blend_Alpha_Dist_UV;
    _VSPS_Blend_FBNextIndex_UV = flattenTemp.Blend_FBNextIndex_UV;
}

