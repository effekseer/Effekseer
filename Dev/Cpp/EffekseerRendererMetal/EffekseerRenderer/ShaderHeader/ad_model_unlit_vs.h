static const char metal_ad_model_unlit_vs[] = R"(mtlcode
#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct VS_Output
{
    float4 PosVS;
    float4 Color;
    float4 UV_Others;
    float3 WorldN;
    float4 Alpha_Dist_UV;
    float4 Blend_Alpha_Dist_UV;
    float4 Blend_FBNextIndex_UV;
    float4 PosP;
};

struct VS_Input
{
    float3 Pos;
    float3 Normal;
    float3 Binormal;
    float3 Tangent;
    float2 UV;
    float4 Color;
    uint Index;
};

struct VS_ConstantBuffer
{
    float4x4 mCameraProj;
    float4x4 mModel_Inst[40];
    float4 fUV[40];
    float4 fAlphaUV[40];
    float4 fUVDistortionUV[40];
    float4 fBlendUV[40];
    float4 fBlendAlphaUV[40];
    float4 fBlendUVDistortionUV[40];
    float4 flipbookParameter1;
    float4 flipbookParameter2;
    float4 fFlipbookIndexAndNextRate[40];
    float4 fModelAlphaThreshold[40];
    float4 fModelColor[40];
    float4 fLightDirection;
    float4 fLightColor;
    float4 fLightAmbient;
    float4 mUVInversed;
};

struct main0_out
{
    float4 _entryPointOutput_Color [[user(locn0)]];
    float4 _entryPointOutput_UV_Others [[user(locn1)]];
    float3 _entryPointOutput_WorldN [[user(locn2)]];
    float4 _entryPointOutput_Alpha_Dist_UV [[user(locn3)]];
    float4 _entryPointOutput_Blend_Alpha_Dist_UV [[user(locn4)]];
    float4 _entryPointOutput_Blend_FBNextIndex_UV [[user(locn5)]];
    float4 _entryPointOutput_PosP [[user(locn6)]];
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
};

// Implementation of the GLSL mod() function, which is slightly different than Metal fmod()
template<typename Tx, typename Ty>
inline Tx mod(Tx x, Ty y)
{
    return x - y * floor(x / y);
}

static inline __attribute__((always_inline))
float2 GetFlipbookOriginUV(thread const float2& FlipbookUV, thread const float& FlipbookIndex, thread const float& DivideX, thread const float2& flipbookOneSize, thread const float2& flipbookOffset)
{
    float2 DivideIndex;
    DivideIndex.x = float(int(FlipbookIndex) % int(DivideX));
    DivideIndex.y = float(int(FlipbookIndex) / int(DivideX));
    float2 UVOffset = (DivideIndex * flipbookOneSize) + flipbookOffset;
    return FlipbookUV - UVOffset;
}

static inline __attribute__((always_inline))
float2 GetFlipbookUVForIndex(thread const float2& OriginUV, thread const float& Index, thread const float& DivideX, thread const float2& flipbookOneSize, thread const float2& flipbookOffset)
{
    float2 DivideIndex;
    DivideIndex.x = float(int(Index) % int(DivideX));
    DivideIndex.y = float(int(Index) / int(DivideX));
    return (OriginUV + (DivideIndex * flipbookOneSize)) + flipbookOffset;
}

static inline __attribute__((always_inline))
void ApplyFlipbookVS(thread float& flipbookRate, thread float2& flipbookUV, thread const float4& flipbookParameter1, thread const float4& flipbookParameter2, thread const float& flipbookIndex, thread const float2& uv, thread const float2& uvInversed)
{
    float flipbookEnabled = flipbookParameter1.x;
    float flipbookLoopType = flipbookParameter1.y;
    float divideX = flipbookParameter1.z;
    float divideY = flipbookParameter1.w;
    float2 flipbookOneSize = flipbookParameter2.xy;
    float2 flipbookOffset = flipbookParameter2.zw;
    if (flipbookEnabled > 0.0)
    {
        flipbookRate = fract(flipbookIndex);
        float Index = floor(flipbookIndex);
        float IndexOffset = 1.0;
        float NextIndex = Index + IndexOffset;
        float FlipbookMaxCount = divideX * divideY;
        if (flipbookLoopType == 0.0)
        {
            if (NextIndex >= FlipbookMaxCount)
            {
                NextIndex = FlipbookMaxCount - 1.0;
                Index = FlipbookMaxCount - 1.0;
            }
        }
        else
        {
            if (flipbookLoopType == 1.0)
            {
                Index = mod(Index, FlipbookMaxCount);
                NextIndex = mod(NextIndex, FlipbookMaxCount);
            }
            else
            {
                if (flipbookLoopType == 2.0)
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
        float2 notInversedUV = uv;
        notInversedUV.y = uvInversed.x + (uvInversed.y * notInversedUV.y);
        float2 param = notInversedUV;
        float param_1 = Index;
        float param_2 = divideX;
        float2 param_3 = flipbookOneSize;
        float2 param_4 = flipbookOffset;
        float2 OriginUV = GetFlipbookOriginUV(param, param_1, param_2, param_3, param_4);
        float2 param_5 = OriginUV;
        float param_6 = NextIndex;
        float param_7 = divideX;
        float2 param_8 = flipbookOneSize;
        float2 param_9 = flipbookOffset;
        flipbookUV = GetFlipbookUVForIndex(param_5, param_6, param_7, param_8, param_9);
        flipbookUV.y = uvInversed.x + (uvInversed.y * flipbookUV.y);
    }
}

static inline __attribute__((always_inline))
void CalculateAndStoreAdvancedParameter(thread const float2& uv, thread const float2& uv1, thread const float4& alphaUV, thread const float4& uvDistortionUV, thread const float4& blendUV, thread const float4& blendAlphaUV, thread const float4& blendUVDistortionUV, thread const float& flipbookIndexAndNextRate, thread const float& modelAlphaThreshold, thread VS_Output& vsoutput, constant VS_ConstantBuffer& v_372)
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
    float4 param_2 = v_372.flipbookParameter1;
    float4 param_3 = v_372.flipbookParameter2;
    float param_4 = flipbookIndexAndNextRate;
    float2 param_5 = uv1;
    float2 param_6 = float2(v_372.mUVInversed.xy);
    ApplyFlipbookVS(param, param_1, param_2, param_3, param_4, param_5, param_6);
    flipbookRate = param;
    flipbookNextIndexUV = param_1;
    vsoutput.Blend_FBNextIndex_UV = float4(vsoutput.Blend_FBNextIndex_UV.x, vsoutput.Blend_FBNextIndex_UV.y, flipbookNextIndexUV.x, flipbookNextIndexUV.y);
    vsoutput.UV_Others.z = flipbookRate;
    vsoutput.UV_Others.w = modelAlphaThreshold;
    vsoutput.Alpha_Dist_UV.y = v_372.mUVInversed.x + (v_372.mUVInversed.y * vsoutput.Alpha_Dist_UV.y);
    vsoutput.Alpha_Dist_UV.w = v_372.mUVInversed.x + (v_372.mUVInversed.y * vsoutput.Alpha_Dist_UV.w);
    vsoutput.Blend_FBNextIndex_UV.y = v_372.mUVInversed.x + (v_372.mUVInversed.y * vsoutput.Blend_FBNextIndex_UV.y);
    vsoutput.Blend_Alpha_Dist_UV.y = v_372.mUVInversed.x + (v_372.mUVInversed.y * vsoutput.Blend_Alpha_Dist_UV.y);
    vsoutput.Blend_Alpha_Dist_UV.w = v_372.mUVInversed.x + (v_372.mUVInversed.y * vsoutput.Blend_Alpha_Dist_UV.w);
}

static inline __attribute__((always_inline))
VS_Output _main(VS_Input Input, constant VS_ConstantBuffer& v_372)
{
    uint index = Input.Index;
    float4x4 mModel = transpose(v_372.mModel_Inst[index]);
    float4 uv = v_372.fUV[index];
    float4 alphaUV = v_372.fAlphaUV[index];
    float4 uvDistortionUV = v_372.fUVDistortionUV[index];
    float4 blendUV = v_372.fBlendUV[index];
    float4 blendAlphaUV = v_372.fBlendAlphaUV[index];
    float4 blendUVDistortionUV = v_372.fBlendUVDistortionUV[index];
    float4 modelColor = v_372.fModelColor[index] * Input.Color;
    float flipbookIndexAndNextRate = v_372.fFlipbookIndexAndNextRate[index].x;
    float modelAlphaThreshold = v_372.fModelAlphaThreshold[index].x;
    VS_Output Output = VS_Output{ float4(0.0), float4(0.0), float4(0.0), float3(0.0), float4(0.0), float4(0.0), float4(0.0), float4(0.0) };
    float4 localPosition = float4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    float4 worldPos = localPosition * mModel;
    Output.PosVS = v_372.mCameraProj * worldPos;
    float2 outputUV = Input.UV;
    outputUV.x = (outputUV.x * uv.z) + uv.x;
    outputUV.y = (outputUV.y * uv.w) + uv.y;
    outputUV.y = v_372.mUVInversed.x + (v_372.mUVInversed.y * outputUV.y);
    Output.UV_Others = float4(outputUV.x, outputUV.y, Output.UV_Others.z, Output.UV_Others.w);
    float4 localNormal = float4(Input.Normal.x, Input.Normal.y, Input.Normal.z, 0.0);
    localNormal = normalize(localNormal * mModel);
    Output.WorldN = localNormal.xyz;
    Output.Color = modelColor;
    float2 param = Input.UV;
    float2 param_1 = Output.UV_Others.xy;
    float4 param_2 = alphaUV;
    float4 param_3 = uvDistortionUV;
    float4 param_4 = blendUV;
    float4 param_5 = blendAlphaUV;
    float4 param_6 = blendUVDistortionUV;
    float param_7 = flipbookIndexAndNextRate;
    float param_8 = modelAlphaThreshold;
    VS_Output param_9 = Output;
    CalculateAndStoreAdvancedParameter(param, param_1, param_2, param_3, param_4, param_5, param_6, param_7, param_8, param_9, v_372);
    Output = param_9;
    Output.PosP = Output.PosVS;
    return Output;
}

vertex main0_out main0(main0_in in [[stage_in]], constant VS_ConstantBuffer& v_372 [[buffer(0)]], uint gl_InstanceIndex [[instance_id]])
{
    main0_out out = {};
    VS_Input Input;
    Input.Pos = in.Input_Pos;
    Input.Normal = in.Input_Normal;
    Input.Binormal = in.Input_Binormal;
    Input.Tangent = in.Input_Tangent;
    Input.UV = in.Input_UV;
    Input.Color = in.Input_Color;
    Input.Index = gl_InstanceIndex;
    VS_Output flattenTemp = _main(Input, v_372);
    out.gl_Position = flattenTemp.PosVS;
    out._entryPointOutput_Color = flattenTemp.Color;
    out._entryPointOutput_UV_Others = flattenTemp.UV_Others;
    out._entryPointOutput_WorldN = flattenTemp.WorldN;
    out._entryPointOutput_Alpha_Dist_UV = flattenTemp.Alpha_Dist_UV;
    out._entryPointOutput_Blend_Alpha_Dist_UV = flattenTemp.Blend_Alpha_Dist_UV;
    out._entryPointOutput_Blend_FBNextIndex_UV = flattenTemp.Blend_FBNextIndex_UV;
    out._entryPointOutput_PosP = flattenTemp.PosP;
    return out;
}

)";

