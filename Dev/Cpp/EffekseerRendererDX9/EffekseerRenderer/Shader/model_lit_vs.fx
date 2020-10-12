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
    float4 Pos;
    float2 UV;
    float3 Normal;
    float3 Binormal;
    float3 Tangent;
    float4 Color;
};

static const VS_Output _59 = { 0.0f.xxxx, 0.0f.xx, 0.0f.xxx, 0.0f.xxx, 0.0f.xxx, 0.0f.xxxx };

cbuffer VS_ConstantBuffer : register(b0)
{
    column_major float4x4 _32_mCameraProj : register(c0);
    column_major float4x4 _32_mModel[10] : register(c4);
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
static float2 _entryPointOutput_UV;
static float3 _entryPointOutput_Normal;
static float3 _entryPointOutput_Binormal;
static float3 _entryPointOutput_Tangent;
static float4 _entryPointOutput_Color;

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
    centroid float2 _entryPointOutput_UV : TEXCOORD0;
    float3 _entryPointOutput_Normal : TEXCOORD1;
    float3 _entryPointOutput_Binormal : TEXCOORD2;
    float3 _entryPointOutput_Tangent : TEXCOORD3;
    centroid float4 _entryPointOutput_Color : TEXCOORD4;
    float4 gl_Position : POSITION;
};

VS_Output _main(VS_Input Input)
{
    int index = int(Input.Index);
    float4x4 matModel = _32_mModel[index];
    float4 uv = _32_fUV[index];
    float4 modelColor = _32_fModelColor[index] * Input.Color;
    VS_Output Output = _59;
    float4 localPosition = float4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0f);
    float4 cameraPosition = mul(matModel, localPosition);
    Output.Pos = mul(_32_mCameraProj, cameraPosition);
    Output.Color = modelColor;
    Output.UV.x = (Input.UV.x * uv.z) + uv.x;
    Output.UV.y = (Input.UV.y * uv.w) + uv.y;
    float4 localNormal = float4(Input.Normal.x, Input.Normal.y, Input.Normal.z, 0.0f);
    localNormal = normalize(mul(matModel, localNormal));
    float4 localBinormal = float4(Input.Binormal.x, Input.Binormal.y, Input.Binormal.z, 0.0f);
    localBinormal = normalize(mul(matModel, localBinormal));
    float4 localTangent = float4(Input.Tangent.x, Input.Tangent.y, Input.Tangent.z, 0.0f);
    localTangent = normalize(mul(matModel, localTangent));
    Output.Normal = localNormal.xyz;
    Output.Binormal = localBinormal.xyz;
    Output.Tangent = localTangent.xyz;
    Output.UV.y = _32_mUVInversed.x + (_32_mUVInversed.y * Output.UV.y);
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
    _entryPointOutput_Normal = flattenTemp.Normal;
    _entryPointOutput_Binormal = flattenTemp.Binormal;
    _entryPointOutput_Tangent = flattenTemp.Tangent;
    _entryPointOutput_Color = flattenTemp.Color;
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
    stage_output._entryPointOutput_UV = _entryPointOutput_UV;
    stage_output._entryPointOutput_Normal = _entryPointOutput_Normal;
    stage_output._entryPointOutput_Binormal = _entryPointOutput_Binormal;
    stage_output._entryPointOutput_Tangent = _entryPointOutput_Tangent;
    stage_output._entryPointOutput_Color = _entryPointOutput_Color;
    return stage_output;
}
