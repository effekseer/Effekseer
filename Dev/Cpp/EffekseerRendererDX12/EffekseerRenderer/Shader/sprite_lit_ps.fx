struct PS_Input
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

cbuffer VS_ConstantBuffer : register(b1)
{
    float4 _69_fLightDirection : packoffset(c0);
    float4 _69_fLightColor : packoffset(c1);
    float4 _69_fLightAmbient : packoffset(c2);
    float4 _69_fFlipbookParameter : packoffset(c3);
    float4 _69_fUVDistortionParameter : packoffset(c4);
    float4 _69_fBlendTextureParameter : packoffset(c5);
    float4 _69_fEmissiveScaling : packoffset(c6);
    float4 _69_fEdgeColor : packoffset(c7);
    float4 _69_fEdgeParameter : packoffset(c8);
};

Texture2D<float4> g_normalTexture : register(t1);
SamplerState g_normalSampler : register(s1);
Texture2D<float4> g_colorTexture : register(t0);
SamplerState g_colorSampler : register(s0);

static float4 gl_FragCoord;
static float4 Input_VColor;
static float2 Input_UV1;
static float2 Input_UV2;
static float3 Input_WorldP;
static float3 Input_WorldN;
static float3 Input_WorldT;
static float3 Input_WorldB;
static float2 Input_ScreenUV;
static float4 _entryPointOutput;

struct SPIRV_Cross_Input
{
    centroid float4 Input_VColor : TEXCOORD0;
    centroid float2 Input_UV1 : TEXCOORD1;
    centroid float2 Input_UV2 : TEXCOORD2;
    float3 Input_WorldP : TEXCOORD3;
    float3 Input_WorldN : TEXCOORD4;
    float3 Input_WorldT : TEXCOORD5;
    float3 Input_WorldB : TEXCOORD6;
    float2 Input_ScreenUV : TEXCOORD7;
    float4 gl_FragCoord : SV_Position;
};

struct SPIRV_Cross_Output
{
    float4 _entryPointOutput : SV_Target0;
};

float4 _main(PS_Input Input)
{
    float3 loN = g_normalTexture.Sample(g_normalSampler, Input.UV1).xyz;
    float3 texNormal = (loN - 0.5f.xxx) * 2.0f;
    float3 localNormal = normalize(mul(texNormal, float3x3(float3(Input.WorldT), float3(Input.WorldB), float3(Input.WorldN))));
    float diffuse = max(dot(_69_fLightDirection.xyz, localNormal), 0.0f);
    float4 Output = g_colorTexture.Sample(g_colorSampler, Input.UV1) * Input.VColor;
    float3 _104 = Output.xyz * ((_69_fLightColor.xyz * diffuse) + float3(_69_fLightAmbient.xyz));
    Output = float4(_104.x, _104.y, _104.z, Output.w);
    if (Output.w == 0.0f)
    {
        discard;
    }
    return Output;
}

void frag_main()
{
    PS_Input Input;
    Input.Position = gl_FragCoord;
    Input.VColor = Input_VColor;
    Input.UV1 = Input_UV1;
    Input.UV2 = Input_UV2;
    Input.WorldP = Input_WorldP;
    Input.WorldN = Input_WorldN;
    Input.WorldT = Input_WorldT;
    Input.WorldB = Input_WorldB;
    Input.ScreenUV = Input_ScreenUV;
    float4 _158 = _main(Input);
    _entryPointOutput = _158;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord;
    Input_VColor = stage_input.Input_VColor;
    Input_UV1 = stage_input.Input_UV1;
    Input_UV2 = stage_input.Input_UV2;
    Input_WorldP = stage_input.Input_WorldP;
    Input_WorldN = stage_input.Input_WorldN;
    Input_WorldT = stage_input.Input_WorldT;
    Input_WorldB = stage_input.Input_WorldB;
    Input_ScreenUV = stage_input.Input_ScreenUV;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output._entryPointOutput = _entryPointOutput;
    return stage_output;
}
