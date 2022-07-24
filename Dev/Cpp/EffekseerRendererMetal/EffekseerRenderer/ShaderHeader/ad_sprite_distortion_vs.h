static const char metal_ad_sprite_distortion_vs[] = R"(mtlcode
#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct VS_Input
{
    float3 Pos;
    float4 Color;
    float4 Normal;
    float4 Tangent;
    float2 UV1;
    float2 UV2;
    float4 Alpha_Dist_UV;
    float2 BlendUV;
    float4 Blend_Alpha_Dist_UV;
    float FlipbookIndex;
    float AlphaThreshold;
};

struct VS_Output
{
    float4 PosVS;
    float4 UV_Others;
    float4 ProjBinormal;
    float4 ProjTangent;
    float4 PosP;
    float4 Color;
    float4 Alpha_Dist_UV;
    float4 Blend_Alpha_Dist_UV;
    float4 Blend_FBNextIndex_UV;
};

struct VS_ConstantBuffer
{
    float4x4 mCamera;
    float4x4 mCameraProj;
    float4 mUVInversed;
    float4 flipbookParameter1;
    float4 flipbookParameter2;
};

struct main0_out
{
    float4 _entryPointOutput_UV_Others [[user(locn0)]];
    float4 _entryPointOutput_ProjBinormal [[user(locn1)]];
    float4 _entryPointOutput_ProjTangent [[user(locn2)]];
    float4 _entryPointOutput_PosP [[user(locn3)]];
    float4 _entryPointOutput_Color [[user(locn4)]];
    float4 _entryPointOutput_Alpha_Dist_UV [[user(locn5)]];
    float4 _entryPointOutput_Blend_Alpha_Dist_UV [[user(locn6)]];
    float4 _entryPointOutput_Blend_FBNextIndex_UV [[user(locn7)]];
    float4 gl_Position [[position]];
};

struct main0_in
{
    float3 Input_Pos [[attribute(0)]];
    float4 Input_Color [[attribute(1)]];
    float4 Input_Normal [[attribute(2)]];
    float4 Input_Tangent [[attribute(3)]];
    float2 Input_UV1 [[attribute(4)]];
    float2 Input_UV2 [[attribute(5)]];
    float4 Input_Alpha_Dist_UV [[attribute(6)]];
    float2 Input_BlendUV [[attribute(7)]];
    float4 Input_Blend_Alpha_Dist_UV [[attribute(8)]];
    float Input_FlipbookIndex [[attribute(9)]];
    float Input_AlphaThreshold [[attribute(10)]];
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
void CalculateAndStoreAdvancedParameter(thread const VS_Input& vsinput, thread VS_Output& vsoutput, constant VS_ConstantBuffer& v_262)
{
    vsoutput.Alpha_Dist_UV = vsinput.Alpha_Dist_UV;
    vsoutput.Alpha_Dist_UV.y = v_262.mUVInversed.x + (v_262.mUVInversed.y * vsinput.Alpha_Dist_UV.y);
    vsoutput.Alpha_Dist_UV.w = v_262.mUVInversed.x + (v_262.mUVInversed.y * vsinput.Alpha_Dist_UV.w);
    vsoutput.Blend_FBNextIndex_UV = float4(vsinput.BlendUV.x, vsinput.BlendUV.y, vsoutput.Blend_FBNextIndex_UV.z, vsoutput.Blend_FBNextIndex_UV.w);
    vsoutput.Blend_FBNextIndex_UV.y = v_262.mUVInversed.x + (v_262.mUVInversed.y * vsinput.BlendUV.y);
    vsoutput.Blend_Alpha_Dist_UV = vsinput.Blend_Alpha_Dist_UV;
    vsoutput.Blend_Alpha_Dist_UV.y = v_262.mUVInversed.x + (v_262.mUVInversed.y * vsinput.Blend_Alpha_Dist_UV.y);
    vsoutput.Blend_Alpha_Dist_UV.w = v_262.mUVInversed.x + (v_262.mUVInversed.y * vsinput.Blend_Alpha_Dist_UV.w);
    float flipbookRate = 0.0;
    float2 flipbookNextIndexUV = float2(0.0);
    float param = flipbookRate;
    float2 param_1 = flipbookNextIndexUV;
    float4 param_2 = v_262.flipbookParameter1;
    float4 param_3 = v_262.flipbookParameter2;
    float param_4 = vsinput.FlipbookIndex;
    float2 param_5 = vsoutput.UV_Others.xy;
    float2 param_6 = float2(v_262.mUVInversed.xy);
    ApplyFlipbookVS(param, param_1, param_2, param_3, param_4, param_5, param_6);
    flipbookRate = param;
    flipbookNextIndexUV = param_1;
    vsoutput.Blend_FBNextIndex_UV = float4(vsoutput.Blend_FBNextIndex_UV.x, vsoutput.Blend_FBNextIndex_UV.y, flipbookNextIndexUV.x, flipbookNextIndexUV.y);
    vsoutput.UV_Others.z = flipbookRate;
    vsoutput.UV_Others.w = vsinput.AlphaThreshold;
}

static inline __attribute__((always_inline))
VS_Output _main(VS_Input Input, constant VS_ConstantBuffer& v_262)
{
    VS_Output Output = VS_Output{ float4(0.0), float4(0.0), float4(0.0), float4(0.0), float4(0.0), float4(0.0), float4(0.0), float4(0.0), float4(0.0) };
    float4 worldNormal = float4((Input.Normal.xyz - float3(0.5)) * 2.0, 0.0);
    float4 worldTangent = float4((Input.Tangent.xyz - float3(0.5)) * 2.0, 0.0);
    float4 worldBinormal = float4(cross(worldNormal.xyz, worldTangent.xyz), 0.0);
    float2 uv1 = Input.UV1;
    uv1.y = v_262.mUVInversed.x + (v_262.mUVInversed.y * uv1.y);
    Output.UV_Others = float4(uv1.x, uv1.y, Output.UV_Others.z, Output.UV_Others.w);
    float4 worldPos = float4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    Output.PosVS = v_262.mCameraProj * worldPos;
    Output.ProjTangent = v_262.mCameraProj * (worldPos + worldTangent);
    Output.ProjBinormal = v_262.mCameraProj * (worldPos + worldBinormal);
    Output.Color = Input.Color;
    VS_Input param = Input;
    VS_Output param_1 = Output;
    CalculateAndStoreAdvancedParameter(param, param_1, v_262);
    Output = param_1;
    Output.PosP = Output.PosVS;
    return Output;
}

vertex main0_out main0(main0_in in [[stage_in]], constant VS_ConstantBuffer& v_262 [[buffer(0)]])
{
    main0_out out = {};
    VS_Input Input;
    Input.Pos = in.Input_Pos;
    Input.Color = in.Input_Color;
    Input.Normal = in.Input_Normal;
    Input.Tangent = in.Input_Tangent;
    Input.UV1 = in.Input_UV1;
    Input.UV2 = in.Input_UV2;
    Input.Alpha_Dist_UV = in.Input_Alpha_Dist_UV;
    Input.BlendUV = in.Input_BlendUV;
    Input.Blend_Alpha_Dist_UV = in.Input_Blend_Alpha_Dist_UV;
    Input.FlipbookIndex = in.Input_FlipbookIndex;
    Input.AlphaThreshold = in.Input_AlphaThreshold;
    VS_Output flattenTemp = _main(Input, v_262);
    out.gl_Position = flattenTemp.PosVS;
    out._entryPointOutput_UV_Others = flattenTemp.UV_Others;
    out._entryPointOutput_ProjBinormal = flattenTemp.ProjBinormal;
    out._entryPointOutput_ProjTangent = flattenTemp.ProjTangent;
    out._entryPointOutput_PosP = flattenTemp.PosP;
    out._entryPointOutput_Color = flattenTemp.Color;
    out._entryPointOutput_Alpha_Dist_UV = flattenTemp.Alpha_Dist_UV;
    out._entryPointOutput_Blend_Alpha_Dist_UV = flattenTemp.Blend_Alpha_Dist_UV;
    out._entryPointOutput_Blend_FBNextIndex_UV = flattenTemp.Blend_FBNextIndex_UV;
    return out;
}

)";

