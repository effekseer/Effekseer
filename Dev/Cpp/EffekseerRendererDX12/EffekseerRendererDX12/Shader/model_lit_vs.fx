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

static const VS_Output _144 = { 0.0f.xxxx, 0.0f.xxxx, 0.0f.xx, 0.0f.xxx, 0.0f.xxx, 0.0f.xxx, 0.0f.xxxx };

cbuffer VS_ConstantBuffer : register(b0)
{
    column_major float4x4 _118_mCameraProj : packoffset(c0);
    column_major float4x4 _118_mModel_Inst[40] : packoffset(c4);
    float4 _118_fUV[40] : packoffset(c164);
    float4 _118_fModelColor[40] : packoffset(c204);
    float4 _118_fLightDirection : packoffset(c244);
    float4 _118_fLightColor : packoffset(c245);
    float4 _118_fLightAmbient : packoffset(c246);
    float4 _118_mUVInversed : packoffset(c247);
};


static float4 gl_Position;
static int gl_InstanceIndex;
static float3 Input_Pos;
static float2 Input_OctNormal;
static float2 Input_OctTangent;
static float2 Input_UV1;
static float2 Input_UV2;
static float4 Input_Color;
static float4 _entryPointOutput_Color;
static float2 _entryPointOutput_UV;
static float3 _entryPointOutput_WorldN;
static float3 _entryPointOutput_WorldB;
static float3 _entryPointOutput_WorldT;
static float4 _entryPointOutput_PosP;

struct SPIRV_Cross_Input
{
    float3 Input_Pos : TEXCOORD0;
    float2 Input_OctNormal : TEXCOORD1;
    float2 Input_OctTangent : TEXCOORD2;
    float2 Input_UV1 : TEXCOORD3;
    float2 Input_UV2 : TEXCOORD4;
    float4 Input_Color : TEXCOORD5;
    uint gl_InstanceIndex : SV_InstanceID;
};

struct SPIRV_Cross_Output
{
    centroid float4 _entryPointOutput_Color : TEXCOORD0;
    centroid float2 _entryPointOutput_UV : TEXCOORD1;
    float3 _entryPointOutput_WorldN : TEXCOORD2;
    float3 _entryPointOutput_WorldB : TEXCOORD3;
    float3 _entryPointOutput_WorldT : TEXCOORD4;
    float4 _entryPointOutput_PosP : TEXCOORD5;
    float4 gl_Position : SV_Position;
};

float3 decodeOct(float2 oct)
{
    float3 v = float3(oct, (1.0f - abs(oct.x)) - abs(oct.y));
    float t = max(-v.z, 0.0f);
    float3 _56 = v;
    float3 _61 = v;
    float2 _63 = _61.xy + ((-sign(_56.xy)) * t);
    v.x = _63.x;
    v.y = _63.y;
    return normalize(v);
}

void decodeOct(inout float2 octNormal, inout float2 octTangent, inout float3 normal, out float3 binormal, inout float3 tangent)
{
    octNormal = (octNormal * 2.0f) - 1.0f.xx;
    float2 param = octNormal;
    normal = decodeOct(param);
    octTangent = (octTangent * 2.0f) - 1.0f.xx;
    float s = sign(octTangent.y);
    octTangent = float2(octTangent.x, (abs(octTangent.y) * 2.0f) - 1.0f);
    float2 param_1 = octTangent;
    tangent = decodeOct(param_1);
    binormal = cross(normal, tangent) * s;
}

VS_Output _main(VS_Input Input)
{
    uint index = Input.Index;
    float4x4 mModel = _118_mModel_Inst[index];
    float4 uv = _118_fUV[index];
    float4 modelColor = _118_fModelColor[index] * Input.Color;
    VS_Output Output = _144;
    float4 localPos = float4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0f);
    float4 worldPos = mul(mModel, localPos);
    Output.PosVS = mul(_118_mCameraProj, worldPos);
    Output.Color = modelColor;
    float2 outputUV = Input.UV1;
    outputUV.x = (outputUV.x * uv.z) + uv.x;
    outputUV.y = (outputUV.y * uv.w) + uv.y;
    outputUV.y = _118_mUVInversed.x + (_118_mUVInversed.y * outputUV.y);
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
    float4 worldNormal = mul(mModel, float4(localNormal, 0.0f));
    float4 worldBinormal = mul(mModel, float4(localTangent, 0.0f));
    float4 worldTangent = mul(mModel, float4(localBinormal, 0.0f));
    worldNormal = normalize(worldNormal);
    worldBinormal = normalize(worldBinormal);
    worldTangent = normalize(worldTangent);
    Output.WorldN = worldNormal.xyz;
    Output.WorldB = worldBinormal.xyz;
    Output.WorldT = worldTangent.xyz;
    Output.PosP = Output.PosVS;
    return Output;
}

void vert_main()
{
    VS_Input Input;
    Input.Pos = Input_Pos;
    Input.OctNormal = Input_OctNormal;
    Input.OctTangent = Input_OctTangent;
    Input.UV1 = Input_UV1;
    Input.UV2 = Input_UV2;
    Input.Color = Input_Color;
    Input.Index = uint(gl_InstanceIndex);
    VS_Output flattenTemp = _main(Input);
    gl_Position = flattenTemp.PosVS;
    _entryPointOutput_Color = flattenTemp.Color;
    _entryPointOutput_UV = flattenTemp.UV;
    _entryPointOutput_WorldN = flattenTemp.WorldN;
    _entryPointOutput_WorldB = flattenTemp.WorldB;
    _entryPointOutput_WorldT = flattenTemp.WorldT;
    _entryPointOutput_PosP = flattenTemp.PosP;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_InstanceIndex = int(stage_input.gl_InstanceIndex);
    Input_Pos = stage_input.Input_Pos;
    Input_OctNormal = stage_input.Input_OctNormal;
    Input_OctTangent = stage_input.Input_OctTangent;
    Input_UV1 = stage_input.Input_UV1;
    Input_UV2 = stage_input.Input_UV2;
    Input_Color = stage_input.Input_Color;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    stage_output._entryPointOutput_Color = _entryPointOutput_Color;
    stage_output._entryPointOutput_UV = _entryPointOutput_UV;
    stage_output._entryPointOutput_WorldN = _entryPointOutput_WorldN;
    stage_output._entryPointOutput_WorldB = _entryPointOutput_WorldB;
    stage_output._entryPointOutput_WorldT = _entryPointOutput_WorldT;
    stage_output._entryPointOutput_PosP = _entryPointOutput_PosP;
    return stage_output;
}
