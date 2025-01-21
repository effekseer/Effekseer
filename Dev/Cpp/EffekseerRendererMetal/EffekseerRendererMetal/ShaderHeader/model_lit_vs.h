static const char metal_model_lit_vs[] = R"(mtlcode
#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct VS_Input
{
    float3 Pos;
    float2 OctNormal;
    float2 OctTangent;
    float2 UV1;
    float2 UV2;
    float4 Color;
    uint Index;
};

struct VS_Output
{
    float4 PosVS;
    float4 Color;
    float2 UV;
    float3 WorldN;
    float3 WorldB;
    float3 WorldT;
    float4 PosP;
};

struct VS_ConstantBuffer
{
    float4x4 mCameraProj;
    float4x4 mModel_Inst[40];
    float4 fUV[40];
    float4 fModelColor[40];
    float4 fLightDirection;
    float4 fLightColor;
    float4 fLightAmbient;
    float4 mUVInversed;
};

struct main0_out
{
    float4 _entryPointOutput_Color [[user(locn0)]];
    float2 _entryPointOutput_UV [[user(locn1)]];
    float3 _entryPointOutput_WorldN [[user(locn2)]];
    float3 _entryPointOutput_WorldB [[user(locn3)]];
    float3 _entryPointOutput_WorldT [[user(locn4)]];
    float4 _entryPointOutput_PosP [[user(locn5)]];
    float4 gl_Position [[position]];
};

struct main0_in
{
    float3 Input_Pos [[attribute(0)]];
    float2 Input_OctNormal [[attribute(1)]];
    float2 Input_OctTangent [[attribute(2)]];
    float2 Input_UV1 [[attribute(3)]];
    float2 Input_UV2 [[attribute(4)]];
    float4 Input_Color [[attribute(5)]];
};

static inline __attribute__((always_inline))
float3 decodeOct(thread const float2& oct)
{
    float3 v = float3(oct, (1.0 - abs(oct.x)) - abs(oct.y));
    float t = fast::max(-v.z, 0.0);
    float3 _56 = v;
    float3 _61 = v;
    float2 _63 = _61.xy + ((-sign(_56.xy)) * t);
    v.x = _63.x;
    v.y = _63.y;
    return fast::normalize(v);
}

static inline __attribute__((always_inline))
void decodeOct(thread float2& octNormal, thread float2& octTangent, thread float3& normal, thread float3& binormal, thread float3& tangent)
{
    octNormal = (octNormal * 2.0) - float2(1.0);
    float2 param = octNormal;
    normal = decodeOct(param);
    octTangent = (octTangent * 2.0) - float2(1.0);
    float s = sign(octTangent.y);
    octTangent = float2(octTangent.x, (abs(octTangent.y) * 2.0) - 1.0);
    float2 param_1 = octTangent;
    tangent = decodeOct(param_1);
    binormal = cross(normal, tangent) * s;
}

static inline __attribute__((always_inline))
VS_Output _main(VS_Input Input, constant VS_ConstantBuffer& _118)
{
    uint index = Input.Index;
    float4x4 mModel = transpose(_118.mModel_Inst[index]);
    float4 uv = _118.fUV[index];
    float4 modelColor = _118.fModelColor[index] * Input.Color;
    VS_Output Output = VS_Output{ float4(0.0), float4(0.0), float2(0.0), float3(0.0), float3(0.0), float3(0.0), float4(0.0) };
    float4 localPos = float4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    float4 worldPos = localPos * mModel;
    Output.PosVS = _118.mCameraProj * worldPos;
    Output.Color = modelColor;
    float2 outputUV = Input.UV1;
    outputUV.x = (outputUV.x * uv.z) + uv.x;
    outputUV.y = (outputUV.y * uv.w) + uv.y;
    outputUV.y = _118.mUVInversed.x + (_118.mUVInversed.y * outputUV.y);
    Output.UV = outputUV;
    float2 param = Input.OctNormal;
    float2 param_1 = Input.OctTangent;
    float3 param_2;
    float3 param_3;
    float3 param_4;
    decodeOct(param, param_1, param_2, param_3, param_4);
    float3 localNormal = param_2;
    float3 localBinormal = param_3;
    float3 localTangent = param_4;
    float4 worldNormal = float4(localNormal, 0.0) * mModel;
    float4 worldBinormal = float4(localTangent, 0.0) * mModel;
    float4 worldTangent = float4(localBinormal, 0.0) * mModel;
    worldNormal = fast::normalize(worldNormal);
    worldBinormal = fast::normalize(worldBinormal);
    worldTangent = fast::normalize(worldTangent);
    Output.WorldN = worldNormal.xyz;
    Output.WorldB = worldBinormal.xyz;
    Output.WorldT = worldTangent.xyz;
    Output.PosP = Output.PosVS;
    return Output;
}

vertex main0_out main0(main0_in in [[stage_in]], constant VS_ConstantBuffer& _118 [[buffer(0)]], uint gl_InstanceIndex [[instance_id]])
{
    main0_out out = {};
    VS_Input Input;
    Input.Pos = in.Input_Pos;
    Input.OctNormal = in.Input_OctNormal;
    Input.OctTangent = in.Input_OctTangent;
    Input.UV1 = in.Input_UV1;
    Input.UV2 = in.Input_UV2;
    Input.Color = in.Input_Color;
    Input.Index = gl_InstanceIndex;
    VS_Output flattenTemp = _main(Input, _118);
    out.gl_Position = flattenTemp.PosVS;
    out._entryPointOutput_Color = flattenTemp.Color;
    out._entryPointOutput_UV = flattenTemp.UV;
    out._entryPointOutput_WorldN = flattenTemp.WorldN;
    out._entryPointOutput_WorldB = flattenTemp.WorldB;
    out._entryPointOutput_WorldT = flattenTemp.WorldT;
    out._entryPointOutput_PosP = flattenTemp.PosP;
    return out;
}

)";

