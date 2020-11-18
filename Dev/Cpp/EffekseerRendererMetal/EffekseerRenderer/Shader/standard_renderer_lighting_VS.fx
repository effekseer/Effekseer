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
    float4 VColor;
    float2 UV;
    float3 WorldN;
    float3 WorldT;
    float3 WorldB;
    float4 Alpha_Dist_UV;
    float4 Blend_Alpha_Dist_UV;
    float4 Blend_FBNextIndex_UV;
    float2 Others;
    float4 PosP;
};

struct VS_ConstantBuffer
{
    float4x4 mCamera;
    float4x4 mProj;
    float4 mUVInversed;
    float4 mflipbookParameter;
};

struct main0_out
{
    float4 _entryPointOutput_VColor [[user(locn0)]];
    float2 _entryPointOutput_UV [[user(locn1)]];
    float3 _entryPointOutput_WorldN [[user(locn2)]];
    float3 _entryPointOutput_WorldT [[user(locn3)]];
    float3 _entryPointOutput_WorldB [[user(locn4)]];
    float4 _entryPointOutput_Alpha_Dist_UV [[user(locn5)]];
    float4 _entryPointOutput_Blend_Alpha_Dist_UV [[user(locn6)]];
    float4 _entryPointOutput_Blend_FBNextIndex_UV [[user(locn7)]];
    float2 _entryPointOutput_Others [[user(locn8)]];
    float4 _entryPointOutput_PosP [[user(locn9)]];
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
void CalculateAndStoreAdvancedParameter(thread const VS_Input& vsinput, thread VS_Output& vsoutput, constant VS_ConstantBuffer& v_255)
{
    vsoutput.Alpha_Dist_UV = vsinput.Alpha_Dist_UV;
    vsoutput.Alpha_Dist_UV.y = v_255.mUVInversed.x + (v_255.mUVInversed.y * vsinput.Alpha_Dist_UV.y);
    vsoutput.Alpha_Dist_UV.w = v_255.mUVInversed.x + (v_255.mUVInversed.y * vsinput.Alpha_Dist_UV.w);
    vsoutput.Blend_FBNextIndex_UV = float4(vsinput.BlendUV.x, vsinput.BlendUV.y, vsoutput.Blend_FBNextIndex_UV.z, vsoutput.Blend_FBNextIndex_UV.w);
    vsoutput.Blend_FBNextIndex_UV.y = v_255.mUVInversed.x + (v_255.mUVInversed.y * vsinput.BlendUV.y);
    vsoutput.Blend_Alpha_Dist_UV = vsinput.Blend_Alpha_Dist_UV;
    vsoutput.Blend_Alpha_Dist_UV.y = v_255.mUVInversed.x + (v_255.mUVInversed.y * vsinput.Blend_Alpha_Dist_UV.y);
    vsoutput.Blend_Alpha_Dist_UV.w = v_255.mUVInversed.x + (v_255.mUVInversed.y * vsinput.Blend_Alpha_Dist_UV.w);
    float flipbookRate = 0.0;
    float2 flipbookNextIndexUV = float2(0.0);
    float param = flipbookRate;
    float2 param_1 = flipbookNextIndexUV;
    float4 param_2 = v_255.mflipbookParameter;
    float param_3 = vsinput.FlipbookIndex;
    float2 param_4 = vsoutput.UV;
    ApplyFlipbookVS(param, param_1, param_2, param_3, param_4);
    flipbookRate = param;
    flipbookNextIndexUV = param_1;
    vsoutput.Blend_FBNextIndex_UV = float4(vsoutput.Blend_FBNextIndex_UV.x, vsoutput.Blend_FBNextIndex_UV.y, flipbookNextIndexUV.x, flipbookNextIndexUV.y);
    vsoutput.Others.x = flipbookRate;
    vsoutput.Others.y = vsinput.AlphaThreshold;
}

static inline __attribute__((always_inline))
VS_Output _main(VS_Input Input, constant VS_ConstantBuffer& v_255)
{
    VS_Output Output = VS_Output{ float4(0.0), float4(0.0), float2(0.0), float3(0.0), float3(0.0), float3(0.0), float4(0.0), float4(0.0), float4(0.0), float2(0.0), float4(0.0) };
    float3 worldPos = Input.Pos;
    float3 worldNormal = (Input.Normal.xyz - float3(0.5)) * 2.0;
    float3 worldTangent = (Input.Tangent.xyz - float3(0.5)) * 2.0;
    float3 worldBinormal = cross(worldNormal, worldTangent);
    float2 uv1 = Input.UV1;
    float2 uv2 = Input.UV1;
    uv1.y = v_255.mUVInversed.x + (v_255.mUVInversed.y * uv1.y);
    uv2.y = v_255.mUVInversed.x + (v_255.mUVInversed.y * uv2.y);
    Output.WorldN = worldNormal;
    Output.WorldB = worldBinormal;
    Output.WorldT = worldTangent;
    float3 pixelNormalDir = float3(0.5, 0.5, 1.0);
    float4 cameraPos = v_255.mCamera * float4(worldPos, 1.0);
    cameraPos /= float4(cameraPos.w);
    Output.PosVS = v_255.mProj * cameraPos;
    Output.VColor = Input.Color;
    Output.UV = uv1;
    VS_Input param = Input;
    VS_Output param_1 = Output;
    CalculateAndStoreAdvancedParameter(param, param_1, v_255);
    Output = param_1;
    Output.PosP = Output.PosVS;
    return Output;
}

vertex main0_out main0(main0_in in [[stage_in]], constant VS_ConstantBuffer& v_255 [[buffer(0)]])
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
    VS_Output flattenTemp = _main(Input, v_255);
    out.gl_Position = flattenTemp.PosVS;
    out._entryPointOutput_VColor = flattenTemp.VColor;
    out._entryPointOutput_UV = flattenTemp.UV;
    out._entryPointOutput_WorldN = flattenTemp.WorldN;
    out._entryPointOutput_WorldT = flattenTemp.WorldT;
    out._entryPointOutput_WorldB = flattenTemp.WorldB;
    out._entryPointOutput_Alpha_Dist_UV = flattenTemp.Alpha_Dist_UV;
    out._entryPointOutput_Blend_Alpha_Dist_UV = flattenTemp.Blend_Alpha_Dist_UV;
    out._entryPointOutput_Blend_FBNextIndex_UV = flattenTemp.Blend_FBNextIndex_UV;
    out._entryPointOutput_Others = flattenTemp.Others;
    out._entryPointOutput_PosP = flattenTemp.PosP;
    return out;
}

