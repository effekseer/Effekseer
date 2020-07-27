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

struct VS_Output
{
    float4 Pos;
    float2 UV;
    float4 Color;
};

static const VS_Output _56 = { 0.0f.xxxx, 0.0f.xx, 0.0f.xxxx };

cbuffer VS_ConstantBuffer : register(b0)
{
    column_major float4x4 _27_mCameraProj : packoffset(c0);
    column_major float4x4 _27_mModel[1] : packoffset(c4);
    float4 _27_fUV[1] : packoffset(c8);
    float4 _27_fModelColor[1] : packoffset(c9);
    float4 _27_fLightDirection : packoffset(c10);
    float4 _27_fLightColor : packoffset(c11);
    float4 _27_fLightAmbient : packoffset(c12);
    float4 _27_mUVInversed : packoffset(c13);
};


static float4 gl_Position;
static float3 Input_Pos;
static float3 Input_Normal;
static float3 Input_Binormal;
static float3 Input_Tangent;
static float2 Input_UV;
static float4 Input_Color;
static uint4 Input_Index;
static float2 _entryPointOutput_UV;
static float4 _entryPointOutput_Color;

struct SPIRV_Cross_Input
{
    float3 Input_Pos : TEXCOORD0;
    float3 Input_Normal : TEXCOORD1;
    float3 Input_Binormal : TEXCOORD2;
    float3 Input_Tangent : TEXCOORD3;
    float2 Input_UV : TEXCOORD4;
    float4 Input_Color : TEXCOORD5;
    uint4 Input_Index : TEXCOORD6;
};

struct SPIRV_Cross_Output
{
    float2 _entryPointOutput_UV : TEXCOORD0;
    float4 _entryPointOutput_Color : TEXCOORD1;
    float4 gl_Position : SV_Position;
};

VS_Output _main(VS_Input Input)
{
    float4x4 matModel = _27_mModel[Input.Index.x];
    float4 uv = _27_fUV[Input.Index.x];
    float4 modelColor = _27_fModelColor[Input.Index.x] * Input.Color;
    VS_Output Output = _56;
    float4 localPosition = float4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0f);
    localPosition = mul(matModel, localPosition);
    Output.Pos = mul(_27_mCameraProj, localPosition);
    Output.Color = modelColor;
    Output.UV.x = (Input.UV.x * uv.z) + uv.x;
    Output.UV.y = (Input.UV.y * uv.w) + uv.y;
    Output.UV.y = _27_mUVInversed.x + (_27_mUVInversed.y * Output.UV.y);
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
    gl_Position = flattenTemp.Pos;
    _entryPointOutput_UV = flattenTemp.UV;
    _entryPointOutput_Color = flattenTemp.Color;
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
    stage_output._entryPointOutput_UV = _entryPointOutput_UV;
    stage_output._entryPointOutput_Color = _entryPointOutput_Color;
    return stage_output;
}
