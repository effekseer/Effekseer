static const char metal_model_renderer_lighting_texture_normal_VS[] = R"(mtlcode
#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct VS_Output
{
    float4 Pos;
    float2 UV;
    float3 Normal;
    float3 Binormal;
    float3 Tangent;
    float4 Color;
    float4 Alpha_Dist_UV;
    float4 Blend_Alpha_Dist_UV;
    float4 Blend_FBNextIndex_UV;
    float2 Others;
};

struct VS_Input
{
    float3 Pos;
    float3 Normal;
    float3 Binormal;
    float3 Tangent;
    float2 UV;
    float4 Color;
    uint4 Index;
};

struct VS_ConstantBuffer
{
    float4x4 mCameraProj;
    float4x4 mModel[1];
    float4 fUV[1];
    float4 fAlphaUV[1];
    float4 fUVDistortionUV[1];
    float4 fBlendUV[1];
    float4 fBlendAlphaUV[1];
    float4 fBlendUVDistortionUV[1];
    float4 fFlipbookParameter;
    float4 fFlipbookIndexAndNextRate[1];
    float4 fModelAlphaThreshold[1];
    float4 fModelColor[1];
    float4 fLightDirection;
    float4 fLightColor;
    float4 fLightAmbient;
    float4 mUVInversed;
};

struct main0_out
{
    float2 _entryPointOutput_UV [[user(locn0)]];
    float3 _entryPointOutput_Normal [[user(locn1)]];
    float3 _entryPointOutput_Binormal [[user(locn2)]];
    float3 _entryPointOutput_Tangent [[user(locn3)]];
    float4 _entryPointOutput_Color [[user(locn4)]];
    float4 _entryPointOutput_Alpha_Dist_UV [[user(locn5)]];
    float4 _entryPointOutput_Blend_Alpha_Dist_UV [[user(locn6)]];
    float4 _entryPointOutput_Blend_FBNextIndex_UV [[user(locn7)]];
    float2 _entryPointOutput_Others [[user(locn8)]];
    float4 gl_Position [[position]];
};

struct main0_in
{
    float3 Input_Pos [[attribute(0)]];
    float3 Input_Normal [[attribute(1)]];
    float3 Input_Binormal [[attribute(2)]];
    float3 Input_Tangent [[attribute(3)]];
    float2 Input_UV [[attribute(4)]];
    float4 Input_Color [[attribute(5)]];
    uint4 Input_Index [[attribute(6)]];
};

// Implementation of the GLSL mod() function, which is slightly different than Metal fmod()
template<typename Tx, typename Ty>
inline Tx mod(Tx x, Ty y)
{
    return x - y * floor(x / y);
}

static inline __attribute__((always_inline))
float2 GetFlipbookOneSizeUV(thread const float& DivideX, thread const float& DivideY)
{
    return float2(1.0) / float2(DivideX, DivideY);
}

static inline __attribute__((always_inline))
float2 GetFlipbookOriginUV(thread const float2& FlipbookUV, thread const float& FlipbookIndex, thread const float& DivideX, thread const float& DivideY)
{
    float2 DivideIndex;
    DivideIndex.x = float(int(FlipbookIndex) % int(DivideX));
    DivideIndex.y = float(int(FlipbookIndex) / int(DivideX));
    float param = DivideX;
    float param_1 = DivideY;
    float2 FlipbookOneSize = GetFlipbookOneSizeUV(param, param_1);
    float2 UVOffset = DivideIndex * FlipbookOneSize;
    float2 OriginUV = FlipbookUV - UVOffset;
    OriginUV *= float2(DivideX, DivideY);
    return OriginUV;
}

static inline __attribute__((always_inline))
float2 GetFlipbookUVForIndex(thread const float2& OriginUV, thread const float& Index, thread const float& DivideX, thread const float& DivideY)
{
    float2 DivideIndex;
    DivideIndex.x = float(int(Index) % int(DivideX));
    DivideIndex.y = float(int(Index) / int(DivideX));
    float param = DivideX;
    float param_1 = DivideY;
    float2 FlipbookOneSize = GetFlipbookOneSizeUV(param, param_1);
    return (OriginUV * FlipbookOneSize) + (DivideIndex * FlipbookOneSize);
}

static inline __attribute__((always_inline))
void ApplyFlipbookVS(thread float& flipbookRate, thread float2& flipbookUV, thread const float4& flipbookParameter, thread const float& flipbookIndex, thread const float2& uv)
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
        float2 param = uv;
        float param_1 = Index;
        float param_2 = flipbookParameter.z;
        float param_3 = flipbookParameter.w;
        float2 OriginUV = GetFlipbookOriginUV(param, param_1, param_2, param_3);
        float2 param_4 = OriginUV;
        float param_5 = NextIndex;
        float param_6 = flipbookParameter.z;
        float param_7 = flipbookParameter.w;
        flipbookUV = GetFlipbookUVForIndex(param_4, param_5, param_6, param_7);
    }
}

static inline __attribute__((always_inline))
void CalculateAndStoreAdvancedParameter(thread const float2& uv, thread const float4& alphaUV, thread const float4& uvDistortionUV, thread const float4& blendUV, thread const float4& blendAlphaUV, thread const float4& blendUVDistortionUV, thread const float& flipbookIndexAndNextRate, thread const float& modelAlphaThreshold, thread VS_Output& vsoutput, constant VS_ConstantBuffer& v_364)
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
    float2 flipbookNextIndexUV = float2(0.0);
    float param = flipbookRate;
    float2 param_1 = flipbookNextIndexUV;
    float4 param_2 = v_364.fFlipbookParameter;
    float param_3 = flipbookIndexAndNextRate;
    float2 param_4 = uv;
    ApplyFlipbookVS(param, param_1, param_2, param_3, param_4);
    flipbookRate = param;
    flipbookNextIndexUV = param_1;
    vsoutput.Blend_FBNextIndex_UV = float4(vsoutput.Blend_FBNextIndex_UV.x, vsoutput.Blend_FBNextIndex_UV.y, flipbookNextIndexUV.x, flipbookNextIndexUV.y);
    vsoutput.Others.x = flipbookRate;
    vsoutput.Others.y = modelAlphaThreshold;
    vsoutput.Alpha_Dist_UV.y = v_364.mUVInversed.x + (v_364.mUVInversed.y * vsoutput.Alpha_Dist_UV.y);
    vsoutput.Alpha_Dist_UV.w = v_364.mUVInversed.x + (v_364.mUVInversed.y * vsoutput.Alpha_Dist_UV.w);
    vsoutput.Blend_FBNextIndex_UV.y = v_364.mUVInversed.x + (v_364.mUVInversed.y * vsoutput.Blend_FBNextIndex_UV.y);
    vsoutput.Blend_Alpha_Dist_UV.y = v_364.mUVInversed.x + (v_364.mUVInversed.y * vsoutput.Blend_Alpha_Dist_UV.y);
    vsoutput.Blend_Alpha_Dist_UV.w = v_364.mUVInversed.x + (v_364.mUVInversed.y * vsoutput.Blend_Alpha_Dist_UV.w);
}

static inline __attribute__((always_inline))
VS_Output _main(VS_Input Input, constant VS_ConstantBuffer& v_364)
{
    float4x4 matModel = transpose(v_364.mModel[Input.Index.x]);
    float4 uv = v_364.fUV[Input.Index.x];
    float4 alphaUV = v_364.fAlphaUV[Input.Index.x];
    float4 uvDistortionUV = v_364.fUVDistortionUV[Input.Index.x];
    float4 blendUV = v_364.fBlendUV[Input.Index.x];
    float4 blendAlphaUV = v_364.fBlendAlphaUV[Input.Index.x];
    float4 blendUVDistortionUV = v_364.fBlendUVDistortionUV[Input.Index.x];
    float4 modelColor = v_364.fModelColor[Input.Index.x] * Input.Color;
    float flipbookIndexAndNextRate = v_364.fFlipbookIndexAndNextRate[Input.Index.x].x;
    float modelAlphaThreshold = v_364.fModelAlphaThreshold[Input.Index.x].x;
    VS_Output Output = VS_Output{ float4(0.0), float2(0.0), float3(0.0), float3(0.0), float3(0.0), float4(0.0), float4(0.0), float4(0.0), float4(0.0), float2(0.0) };
    float4 localPosition = float4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    float4 cameraPosition = localPosition * matModel;
    Output.Pos = v_364.mCameraProj * cameraPosition;
    Output.UV.x = (Input.UV.x * uv.z) + uv.x;
    Output.UV.y = (Input.UV.y * uv.w) + uv.y;
    float4 localNormal = float4(Input.Normal.x, Input.Normal.y, Input.Normal.z, 0.0);
    localNormal = normalize(localNormal * matModel);
    float4 localBinormal = float4(Input.Binormal.x, Input.Binormal.y, Input.Binormal.z, 0.0);
    localBinormal = normalize(localBinormal * matModel);
    float4 localTangent = float4(Input.Tangent.x, Input.Tangent.y, Input.Tangent.z, 0.0);
    localTangent = normalize(localTangent * matModel);
    Output.Normal = localNormal.xyz;
    Output.Binormal = localBinormal.xyz;
    Output.Tangent = localTangent.xyz;
    Output.Color = modelColor;
    Output.UV.y = v_364.mUVInversed.x + (v_364.mUVInversed.y * Output.UV.y);
    float2 param = Output.UV;
    float4 param_1 = alphaUV;
    float4 param_2 = uvDistortionUV;
    float4 param_3 = blendUV;
    float4 param_4 = blendAlphaUV;
    float4 param_5 = blendUVDistortionUV;
    float param_6 = flipbookIndexAndNextRate;
    float param_7 = modelAlphaThreshold;
    VS_Output param_8 = Output;
    CalculateAndStoreAdvancedParameter(param, param_1, param_2, param_3, param_4, param_5, param_6, param_7, param_8, v_364);
    Output = param_8;
    return Output;
}

vertex main0_out main0(main0_in in [[stage_in]], constant VS_ConstantBuffer& v_364 [[buffer(0)]])
{
    main0_out out = {};
    VS_Input Input;
    Input.Pos = in.Input_Pos;
    Input.Normal = in.Input_Normal;
    Input.Binormal = in.Input_Binormal;
    Input.Tangent = in.Input_Tangent;
    Input.UV = in.Input_UV;
    Input.Color = in.Input_Color;
    Input.Index = in.Input_Index;
    VS_Output flattenTemp = _main(Input, v_364);
    out.gl_Position = flattenTemp.Pos;
    out._entryPointOutput_UV = flattenTemp.UV;
    out._entryPointOutput_Normal = flattenTemp.Normal;
    out._entryPointOutput_Binormal = flattenTemp.Binormal;
    out._entryPointOutput_Tangent = flattenTemp.Tangent;
    out._entryPointOutput_Color = flattenTemp.Color;
    out._entryPointOutput_Alpha_Dist_UV = flattenTemp.Alpha_Dist_UV;
    out._entryPointOutput_Blend_Alpha_Dist_UV = flattenTemp.Blend_Alpha_Dist_UV;
    out._entryPointOutput_Blend_FBNextIndex_UV = flattenTemp.Blend_FBNextIndex_UV;
    out._entryPointOutput_Others = flattenTemp.Others;
    return out;
}

)";

