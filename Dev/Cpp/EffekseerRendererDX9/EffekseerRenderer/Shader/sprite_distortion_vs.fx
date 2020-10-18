struct VS_Input
{
    float3 Pos;
    float4 Color;
    float2 UV;
    float3 Binormal;
    float3 Tangent;
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
    column_major float4x4 _63_mCamera : register(c0);
    column_major float4x4 _63_mProj : register(c4);
    float4 _63_mUVInversed : register(c8);
    float4 _63_mflipbookParameter : register(c9);
};

static const float4 gl_HalfPixel = 0.0f.xxxx;

static float4 gl_Position;
static float3 Input_Pos;
static float4 Input_Color;
static float2 Input_UV;
static float3 Input_Binormal;
static float3 Input_Tangent;
static float4 _entryPointOutput_Color;
static float2 _entryPointOutput_UV;
static float4 _entryPointOutput_PosP;
static float4 _entryPointOutput_PosU;
static float4 _entryPointOutput_PosR;

struct SPIRV_Cross_Input
{
    float3 Input_Pos : TEXCOORD0;
    float4 Input_Color : TEXCOORD1;
    float2 Input_UV : TEXCOORD2;
    float3 Input_Binormal : TEXCOORD3;
    float3 Input_Tangent : TEXCOORD4;
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
    float4 localBinormal = float4(Input.Pos.x + Input.Binormal.x, Input.Pos.y + Input.Binormal.y, Input.Pos.z + Input.Binormal.z, 1.0f);
    float4 localTangent = float4(Input.Pos.x + Input.Tangent.x, Input.Pos.y + Input.Tangent.y, Input.Pos.z + Input.Tangent.z, 1.0f);
    localBinormal = mul(_63_mCamera, localBinormal);
    localTangent = mul(_63_mCamera, localTangent);
    float4 cameraPos = mul(_63_mCamera, pos4);
    cameraPos /= cameraPos.w.xxxx;
    localBinormal /= localBinormal.w.xxxx;
    localTangent /= localTangent.w.xxxx;
    localBinormal = cameraPos + normalize(localBinormal - cameraPos);
    localTangent = cameraPos + normalize(localTangent - cameraPos);
    Output.PosVS = mul(_63_mProj, cameraPos);
    Output.PosP = Output.PosVS;
    Output.PosU = mul(_63_mProj, localBinormal);
    Output.PosR = mul(_63_mProj, localTangent);
    Output.PosU /= Output.PosU.w.xxxx;
    Output.PosR /= Output.PosR.w.xxxx;
    Output.PosP /= Output.PosP.w.xxxx;
    Output.Color = Input.Color;
    Output.UV = Input.UV;
    Output.UV.y = _63_mUVInversed.x + (_63_mUVInversed.y * Input.UV.y);
    return Output;
}

void vert_main()
{
    VS_Input Input;
    Input.Pos = Input_Pos;
    Input.Color = Input_Color;
    Input.UV = Input_UV;
    Input.Binormal = Input_Binormal;
    Input.Tangent = Input_Tangent;
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
    Input_UV = stage_input.Input_UV;
    Input_Binormal = stage_input.Input_Binormal;
    Input_Tangent = stage_input.Input_Tangent;
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
