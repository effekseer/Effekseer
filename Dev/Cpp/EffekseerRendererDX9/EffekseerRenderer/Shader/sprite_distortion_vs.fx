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
    float4 Color;
    float2 UV;
    float4 PosP;
    float4 PosU;
    float4 PosR;
};

static const VS_Output _21 = { 0.0f.xxxx, 0.0f.xxxx, 0.0f.xx, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx };

cbuffer VS_ConstantBuffer : register(b0)
{
    column_major float4x4 _88_mCamera : register(c0);
    column_major float4x4 _88_mProj : register(c4);
    float4 _88_mUVInversed : register(c8);
    float4 _88_mflipbookParameter : register(c9);
};

static const float4 gl_HalfPixel = 0.0f.xxxx;

static float4 gl_Position;
static float3 Input_Pos;
static float4 Input_Color;
static float4 Input_Normal;
static float4 Input_Tangent;
static float2 Input_UV1;
static float2 Input_UV2;
static float4 _entryPointOutput_Color;
static float2 _entryPointOutput_UV;
static float4 _entryPointOutput_PosP;
static float4 _entryPointOutput_PosU;
static float4 _entryPointOutput_PosR;

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
    centroid float4 _entryPointOutput_Color : TEXCOORD0;
    centroid float2 _entryPointOutput_UV : TEXCOORD1;
    float4 _entryPointOutput_PosP : TEXCOORD2;
    float4 _entryPointOutput_PosU : TEXCOORD3;
    float4 _entryPointOutput_PosR : TEXCOORD4;
    float4 gl_Position : POSITION;
};

VS_Output _main(VS_Input Input)
{
    VS_Output Output = _21;
    float4 pos4 = float4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0f);
    float3 worldNormal = (Input.Normal.xyz - 0.5f.xxx) * 2.0f;
    float3 worldTangent = (Input.Tangent.xyz - 0.5f.xxx) * 2.0f;
    float3 worldBinormal = cross(worldNormal, worldTangent);
    float4 localBinormal = float4(Input.Pos.x + worldBinormal.x, Input.Pos.y + worldBinormal.y, Input.Pos.z + worldBinormal.z, 1.0f);
    float4 localTangent = float4(Input.Pos.x + worldTangent.x, Input.Pos.y + worldTangent.y, Input.Pos.z + worldTangent.z, 1.0f);
    localBinormal = mul(_88_mCamera, localBinormal);
    localTangent = mul(_88_mCamera, localTangent);
    float4 cameraPos = mul(_88_mCamera, pos4);
    cameraPos /= cameraPos.w.xxxx;
    localBinormal /= localBinormal.w.xxxx;
    localTangent /= localTangent.w.xxxx;
    localBinormal = cameraPos + normalize(localBinormal - cameraPos);
    localTangent = cameraPos + normalize(localTangent - cameraPos);
    Output.PosVS = mul(_88_mProj, cameraPos);
    Output.PosP = Output.PosVS;
    Output.PosU = mul(_88_mProj, localBinormal);
    Output.PosR = mul(_88_mProj, localTangent);
    Output.PosU /= Output.PosU.w.xxxx;
    Output.PosR /= Output.PosR.w.xxxx;
    Output.PosP /= Output.PosP.w.xxxx;
    Output.Color = Input.Color;
    Output.UV = Input.UV1;
    Output.UV.y = _88_mUVInversed.x + (_88_mUVInversed.y * Input.UV1.y);
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
    _entryPointOutput_Color = flattenTemp.Color;
    _entryPointOutput_UV = flattenTemp.UV;
    _entryPointOutput_PosP = flattenTemp.PosP;
    _entryPointOutput_PosU = flattenTemp.PosU;
    _entryPointOutput_PosR = flattenTemp.PosR;
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
    stage_output._entryPointOutput_Color = _entryPointOutput_Color;
    stage_output._entryPointOutput_UV = _entryPointOutput_UV;
    stage_output._entryPointOutput_PosP = _entryPointOutput_PosP;
    stage_output._entryPointOutput_PosU = _entryPointOutput_PosU;
    stage_output._entryPointOutput_PosR = _entryPointOutput_PosR;
    return stage_output;
}
