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
    float4 Position;
    float4 VColor;
    float2 UV1;
    float2 UV2;
    float3 WorldP;
    float3 WorldN;
    float3 WorldT;
    float3 WorldB;
    float2 ScreenUV;
};

static const VS_Output _22 = { 0.0f.xxxx, 0.0f.xxxx, 0.0f.xx, 0.0f.xx, 0.0f.xxx, 0.0f.xxx, 0.0f.xxx, 0.0f.xxx, 0.0f.xx };

cbuffer VS_ConstantBuffer : register(b0)
{
    column_major float4x4 _62_mCamera : register(c0);
    column_major float4x4 _62_mProj : register(c4);
    float4 _62_mUVInversed : register(c8);
    float4 _62_mflipbookParameter : register(c9);
};

static const float4 gl_HalfPixel = 0.0f.xxxx;

static float4 gl_Position;
static float3 Input_Pos;
static float4 Input_Color;
static float4 Input_Normal;
static float4 Input_Tangent;
static float2 Input_UV1;
static float2 Input_UV2;
static float4 _entryPointOutput_VColor;
static float2 _entryPointOutput_UV1;
static float2 _entryPointOutput_UV2;
static float3 _entryPointOutput_WorldP;
static float3 _entryPointOutput_WorldN;
static float3 _entryPointOutput_WorldT;
static float3 _entryPointOutput_WorldB;
static float2 _entryPointOutput_ScreenUV;

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
    centroid float4 _entryPointOutput_VColor : TEXCOORD0;
    centroid float2 _entryPointOutput_UV1 : TEXCOORD1;
    centroid float2 _entryPointOutput_UV2 : TEXCOORD2;
    float3 _entryPointOutput_WorldP : TEXCOORD3;
    float3 _entryPointOutput_WorldN : TEXCOORD4;
    float3 _entryPointOutput_WorldT : TEXCOORD5;
    float3 _entryPointOutput_WorldB : TEXCOORD6;
    float2 _entryPointOutput_ScreenUV : TEXCOORD7;
    float4 gl_Position : POSITION;
};

VS_Output _main(VS_Input Input)
{
    VS_Output Output = _22;
    float3 worldPos = Input.Pos;
    float3 worldNormal = (float3(Input.Normal.xyz) - 0.5f.xxx) * 2.0f;
    float3 worldTangent = (float3(Input.Tangent.xyz) - 0.5f.xxx) * 2.0f;
    float3 worldBinormal = cross(worldNormal, worldTangent);
    float2 uv1 = Input.UV1;
    float2 uv2 = Input.UV1;
    uv1.y = _62_mUVInversed.x + (_62_mUVInversed.y * uv1.y);
    uv2.y = _62_mUVInversed.x + (_62_mUVInversed.y * uv2.y);
    Output.WorldN = worldNormal;
    Output.WorldB = worldBinormal;
    Output.WorldT = worldTangent;
    float3 pixelNormalDir = float3(0.5f, 0.5f, 1.0f);
    float4 cameraPos = mul(_62_mCamera, float4(worldPos, 1.0f));
    cameraPos /= cameraPos.w.xxxx;
    Output.Position = mul(_62_mProj, cameraPos);
    Output.WorldP = worldPos;
    Output.VColor = Input.Color;
    Output.UV1 = uv1;
    Output.UV2 = uv2;
    Output.ScreenUV = Output.Position.xy / Output.Position.w.xx;
    Output.ScreenUV = float2(Output.ScreenUV.x + 1.0f, 1.0f - Output.ScreenUV.y) * 0.5f;
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
    gl_Position = flattenTemp.Position;
    _entryPointOutput_VColor = flattenTemp.VColor;
    _entryPointOutput_UV1 = flattenTemp.UV1;
    _entryPointOutput_UV2 = flattenTemp.UV2;
    _entryPointOutput_WorldP = flattenTemp.WorldP;
    _entryPointOutput_WorldN = flattenTemp.WorldN;
    _entryPointOutput_WorldT = flattenTemp.WorldT;
    _entryPointOutput_WorldB = flattenTemp.WorldB;
    _entryPointOutput_ScreenUV = flattenTemp.ScreenUV;
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
    stage_output._entryPointOutput_VColor = _entryPointOutput_VColor;
    stage_output._entryPointOutput_UV1 = _entryPointOutput_UV1;
    stage_output._entryPointOutput_UV2 = _entryPointOutput_UV2;
    stage_output._entryPointOutput_WorldP = _entryPointOutput_WorldP;
    stage_output._entryPointOutput_WorldN = _entryPointOutput_WorldN;
    stage_output._entryPointOutput_WorldT = _entryPointOutput_WorldT;
    stage_output._entryPointOutput_WorldB = _entryPointOutput_WorldB;
    stage_output._entryPointOutput_ScreenUV = _entryPointOutput_ScreenUV;
    return stage_output;
}
