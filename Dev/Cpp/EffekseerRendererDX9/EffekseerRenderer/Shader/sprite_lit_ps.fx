struct PS_Input
{
    float4 PosVS;
    float4 VColor;
    float2 UV1;
    float2 UV2;
    float3 WorldP;
    float3 WorldN;
    float3 WorldT;
    float3 WorldB;
    float2 ScreenUV;
    float4 PosP;
};

struct FalloffParameter
{
    float4 Param;
    float4 BeginColor;
    float4 EndColor;
};

cbuffer PS_ConstanBuffer : register(b0)
{
    float4 _70_fLightDirection : register(c0);
    float4 _70_fLightColor : register(c1);
    float4 _70_fLightAmbient : register(c2);
    float4 _70_fFlipbookParameter : register(c3);
    float4 _70_fUVDistortionParameter : register(c4);
    float4 _70_fBlendTextureParameter : register(c5);
    float4 _70_fCameraFrontDirection : register(c6);
    FalloffParameter _70_fFalloffParam : register(c7);
    float4 _70_fEmissiveScaling : register(c10);
    float4 _70_fEdgeColor : register(c11);
    float4 _70_fEdgeParameter : register(c12);
    float4 _70_softParticleAndReconstructionParam1 : register(c13);
    float4 _70_reconstructionParam2 : register(c14);
};

uniform sampler2D Sampler_sampler_normalTex : register(s1);
uniform sampler2D Sampler_sampler_colorTex : register(s0);

static float4 gl_FragCoord;
static float4 Input_VColor;
static float2 Input_UV1;
static float2 Input_UV2;
static float3 Input_WorldP;
static float3 Input_WorldN;
static float3 Input_WorldT;
static float3 Input_WorldB;
static float2 Input_ScreenUV;
static float4 Input_PosP;
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
    float4 Input_PosP : TEXCOORD8;
    float4 gl_FragCoord : VPOS;
};

struct SPIRV_Cross_Output
{
    float4 _entryPointOutput : COLOR0;
};

float4 _main(PS_Input Input)
{
    float3 loN = tex2D(Sampler_sampler_normalTex, Input.UV1).xyz;
    float3 texNormal = (loN - 0.5f.xxx) * 2.0f;
    float3 localNormal = normalize(mul(texNormal, float3x3(float3(Input.WorldT), float3(Input.WorldB), float3(Input.WorldN))));
    float diffuse = max(dot(_70_fLightDirection.xyz, localNormal), 0.0f);
    float4 Output = tex2D(Sampler_sampler_colorTex, Input.UV1) * Input.VColor;
    float3 _105 = Output.xyz * ((_70_fLightColor.xyz * diffuse) + float3(_70_fLightAmbient.xyz));
    Output = float4(_105.x, _105.y, _105.z, Output.w);
    if (Output.w == 0.0f)
    {
        discard;
    }
    return Output;
}

void frag_main()
{
    PS_Input Input;
    Input.PosVS = gl_FragCoord;
    Input.VColor = Input_VColor;
    Input.UV1 = Input_UV1;
    Input.UV2 = Input_UV2;
    Input.WorldP = Input_WorldP;
    Input.WorldN = Input_WorldN;
    Input.WorldT = Input_WorldT;
    Input.WorldB = Input_WorldB;
    Input.ScreenUV = Input_ScreenUV;
    Input.PosP = Input_PosP;
    float4 _163 = _main(Input);
    _entryPointOutput = _163;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord + float4(0.5f, 0.5f, 0.0f, 0.0f);
    Input_VColor = stage_input.Input_VColor;
    Input_UV1 = stage_input.Input_UV1;
    Input_UV2 = stage_input.Input_UV2;
    Input_WorldP = stage_input.Input_WorldP;
    Input_WorldN = stage_input.Input_WorldN;
    Input_WorldT = stage_input.Input_WorldT;
    Input_WorldB = stage_input.Input_WorldB;
    Input_ScreenUV = stage_input.Input_ScreenUV;
    Input_PosP = stage_input.Input_PosP;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output._entryPointOutput = float4(_entryPointOutput);
    return stage_output;
}
