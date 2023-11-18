struct VS_Input
{
    float3 Pos;
    float3 Normal;
    float3 Binormal;
    float3 Tangent;
    float2 UV;
    float4 Color;
    float Index;
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

static const VS_Output _59 = { 0.0f.xxxx, 0.0f.xxxx, 0.0f.xx, 0.0f.xxx, 0.0f.xxx, 0.0f.xxx, 0.0f.xxxx };

cbuffer VS_ConstantBuffer : register(b0)
{
    column_major float4x4 _32_mCameraProj : register(c0);
    column_major float4x4 _32_mModel_Inst[10] : register(c4);
    float4 _32_fUV[10] : register(c44);
    float4 _32_fModelColor[10] : register(c54);
    float4 _32_fLightDirection : register(c64);
    float4 _32_fLightColor : register(c65);
    float4 _32_fLightAmbient : register(c66);
    float4 _32_mUVInversed : register(c67);
};

static const float4 gl_HalfPixel = 0.0f.xxxx;

static float4 gl_Position;
static float3 Input_Pos;
static float3 Input_Normal;
static float3 Input_Binormal;
static float3 Input_Tangent;
static float2 Input_UV;
static float4 Input_Color;
static float Input_Index;
static float4 _entryPointOutput_Color;
static float2 _entryPointOutput_UV;
static float3 _entryPointOutput_WorldN;
static float3 _entryPointOutput_WorldB;
static float3 _entryPointOutput_WorldT;
static float4 _entryPointOutput_PosP;

struct SPIRV_Cross_Input
{
    float3 Input_Pos : TEXCOORD0;
    float3 Input_Normal : TEXCOORD1;
    float3 Input_Binormal : TEXCOORD2;
    float3 Input_Tangent : TEXCOORD3;
    float2 Input_UV : TEXCOORD4;
    float4 Input_Color : TEXCOORD5;
    float Input_Index : TEXCOORD6;
};

struct SPIRV_Cross_Output
{
    centroid float4 _entryPointOutput_Color : TEXCOORD0;
    centroid float2 _entryPointOutput_UV : TEXCOORD1;
    float3 _entryPointOutput_WorldN : TEXCOORD2;
    float3 _entryPointOutput_WorldB : TEXCOORD3;
    float3 _entryPointOutput_WorldT : TEXCOORD4;
    float4 _entryPointOutput_PosP : TEXCOORD5;
    float4 gl_Position : POSITION;
};

VS_Output _main(VS_Input Input)
{
    int index = int(Input.Index);
    float4x4 mModel = _32_mModel_Inst[index];
    float4 uv = _32_fUV[index];
    float4 modelColor = _32_fModelColor[index] * Input.Color;
    VS_Output Output = _59;
    float4 localPos = float4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0f);
    float4 worldPos = mul(mModel, localPos);
    Output.PosVS = mul(_32_mCameraProj, worldPos);
    Output.Color = modelColor;
    float2 outputUV = Input.UV;
    outputUV.x = (outputUV.x * uv.z) + uv.x;
    outputUV.y = (outputUV.y * uv.w) + uv.y;
    outputUV.y = _32_mUVInversed.x + (_32_mUVInversed.y * outputUV.y);
    Output.UV = outputUV;
    float4 localNormal = float4(Input.Normal.x, Input.Normal.y, Input.Normal.z, 0.0f);
    float4 localBinormal = float4(Input.Binormal.x, Input.Binormal.y, Input.Binormal.z, 0.0f);
    float4 localTangent = float4(Input.Tangent.x, Input.Tangent.y, Input.Tangent.z, 0.0f);
    float4 worldNormal = mul(mModel, localNormal);
    float4 worldBinormal = mul(mModel, localBinormal);
    float4 worldTangent = mul(mModel, localTangent);
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
    Input.Normal = Input_Normal;
    Input.Binormal = Input_Binormal;
    Input.Tangent = Input_Tangent;
    Input.UV = Input_UV;
    Input.Color = Input_Color;
    Input.Index = Input_Index;
    VS_Output flattenTemp = _main(Input);
    gl_Position = flattenTemp.PosVS;
    _entryPointOutput_Color = flattenTemp.Color;
    _entryPointOutput_UV = flattenTemp.UV;
    _entryPointOutput_WorldN = flattenTemp.WorldN;
    _entryPointOutput_WorldB = flattenTemp.WorldB;
    _entryPointOutput_WorldT = flattenTemp.WorldT;
    _entryPointOutput_PosP = flattenTemp.PosP;
    gl_Position.x = gl_Position.x - gl_HalfPixel.x * gl_Position.w;
    gl_Position.y = gl_Position.y + gl_HalfPixel.y * gl_Position.w;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    Input_Pos = stage_input.Input_Pos;
    Input_Normal = stage_input.Input_Normal;
    Input_Binormal = stage_input.Input_Binormal;
    Input_Tangent = stage_input.Input_Tangent;
    Input_UV = stage_input.Input_UV;
    Input_Color = stage_input.Input_Color;
    Input_Index = stage_input.Input_Index;
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
