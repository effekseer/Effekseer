struct VS_Input
{
    float3 Pos;
    float4 Color;
    float4 Normal;
    float4 Tangent;
    float2 UV1;
    float2 UV2;
};

struct VS_Output
{
    float4 PosVS;
    float2 UV;
    float4 ProjBinormal;
    float4 ProjTangent;
    float4 PosP;
    float4 Color;
};

static const VS_Output _21 = { 0.0f.xxxx, 0.0f.xx, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx };

cbuffer VS_ConstantBuffer : register(b0)
{
    column_major float4x4 _68_mCamera : register(c0);
    column_major float4x4 _68_mProj : register(c4);
    float4 _68_mUVInversed : register(c8);
    float4 _68_mflipbookParameter : register(c9);
};

static const float4 gl_HalfPixel = 0.0f.xxxx;

static float4 gl_Position;
static float3 Input_Pos;
static float4 Input_Color;
static float4 Input_Normal;
static float4 Input_Tangent;
static float2 Input_UV1;
static float2 Input_UV2;
static float2 _entryPointOutput_UV;
static float4 _entryPointOutput_ProjBinormal;
static float4 _entryPointOutput_ProjTangent;
static float4 _entryPointOutput_PosP;
static float4 _entryPointOutput_Color;

struct SPIRV_Cross_Input
{
    float3 Input_Pos : TEXCOORD0;
    float4 Input_Color : TEXCOORD1;
    float4 Input_Normal : TEXCOORD2;
    float4 Input_Tangent : TEXCOORD3;
    float2 Input_UV1 : TEXCOORD4;
    float2 Input_UV2 : TEXCOORD5;
};

struct SPIRV_Cross_Output
{
    centroid float2 _entryPointOutput_UV : TEXCOORD0;
    float4 _entryPointOutput_ProjBinormal : TEXCOORD1;
    float4 _entryPointOutput_ProjTangent : TEXCOORD2;
    float4 _entryPointOutput_PosP : TEXCOORD3;
    centroid float4 _entryPointOutput_Color : TEXCOORD4;
    float4 gl_Position : POSITION;
};

VS_Output _main(VS_Input Input)
{
    VS_Output Output = _21;
    float3 worldPos = Input.Pos;
    float3 worldNormal = (float3(Input.Normal.xyz) - 0.5f.xxx) * 2.0f;
    float3 worldTangent = (float3(Input.Tangent.xyz) - 0.5f.xxx) * 2.0f;
    float3 worldBinormal = cross(worldNormal, worldTangent);
    float4 pos4 = float4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0f);
    float4 cameraPos = mul(_68_mCamera, pos4);
    Output.PosVS = mul(_68_mProj, cameraPos);
    Output.PosP = Output.PosVS;
    float2 uv1 = Input.UV1;
    uv1.y = _68_mUVInversed.x + (_68_mUVInversed.y * uv1.y);
    Output.UV = uv1;
    float4 localTangent = pos4;
    float4 localBinormal = pos4;
    float3 _106 = localTangent.xyz + worldTangent;
    localTangent = float4(_106.x, _106.y, _106.z, localTangent.w);
    float3 _112 = localBinormal.xyz + worldBinormal;
    localBinormal = float4(_112.x, _112.y, _112.z, localBinormal.w);
    Output.ProjTangent = mul(_68_mProj, mul(_68_mCamera, localTangent));
    Output.ProjBinormal = mul(_68_mProj, mul(_68_mCamera, localBinormal));
    Output.Color = Input.Color;
    return Output;
}

void vert_main()
{
    VS_Input Input;
    Input.Pos = Input_Pos;
    Input.Color = Input_Color;
    Input.Normal = Input_Normal;
    Input.Tangent = Input_Tangent;
    Input.UV1 = Input_UV1;
    Input.UV2 = Input_UV2;
    VS_Output flattenTemp = _main(Input);
    gl_Position = flattenTemp.PosVS;
    _entryPointOutput_UV = flattenTemp.UV;
    _entryPointOutput_ProjBinormal = flattenTemp.ProjBinormal;
    _entryPointOutput_ProjTangent = flattenTemp.ProjTangent;
    _entryPointOutput_PosP = flattenTemp.PosP;
    _entryPointOutput_Color = flattenTemp.Color;
    gl_Position.x = gl_Position.x - gl_HalfPixel.x * gl_Position.w;
    gl_Position.y = gl_Position.y + gl_HalfPixel.y * gl_Position.w;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    Input_Pos = stage_input.Input_Pos;
    Input_Color = stage_input.Input_Color;
    Input_Normal = stage_input.Input_Normal;
    Input_Tangent = stage_input.Input_Tangent;
    Input_UV1 = stage_input.Input_UV1;
    Input_UV2 = stage_input.Input_UV2;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    stage_output._entryPointOutput_UV = _entryPointOutput_UV;
    stage_output._entryPointOutput_ProjBinormal = _entryPointOutput_ProjBinormal;
    stage_output._entryPointOutput_ProjTangent = _entryPointOutput_ProjTangent;
    stage_output._entryPointOutput_PosP = _entryPointOutput_PosP;
    stage_output._entryPointOutput_Color = _entryPointOutput_Color;
    return stage_output;
}
