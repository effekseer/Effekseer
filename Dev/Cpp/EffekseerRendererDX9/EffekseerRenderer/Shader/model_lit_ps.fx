struct PS_Input
{
    float4 PosVS;
    float2 UV;
    float3 Normal;
    float3 Binormal;
    float3 Tangent;
    float4 Color;
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
    float4 _80_fLightDirection : register(c0);
    float4 _80_fLightColor : register(c1);
    float4 _80_fLightAmbient : register(c2);
    float4 _80_fFlipbookParameter : register(c3);
    float4 _80_fUVDistortionParameter : register(c4);
    float4 _80_fBlendTextureParameter : register(c5);
    float4 _80_fCameraFrontDirection : register(c6);
    FalloffParameter _80_fFalloffParam : register(c7);
    float4 _80_fEmissiveScaling : register(c10);
    float4 _80_fEdgeColor : register(c11);
    float4 _80_fEdgeParameter : register(c12);
    float4 _80_softParticleAndReconstructionParam1 : register(c13);
    float4 _80_reconstructionParam2 : register(c14);
};

uniform sampler2D Sampler_sampler_colorTex : register(s0);
uniform sampler2D Sampler_sampler_normalTex : register(s1);

static float4 gl_FragCoord;
static float2 Input_UV;
static float3 Input_Normal;
static float3 Input_Binormal;
static float3 Input_Tangent;
static float4 Input_Color;
static float4 Input_PosP;
static float4 _entryPointOutput;

struct SPIRV_Cross_Input
{
    centroid float2 Input_UV : TEXCOORD0;
    float3 Input_Normal : TEXCOORD1;
    float3 Input_Binormal : TEXCOORD2;
    float3 Input_Tangent : TEXCOORD3;
    centroid float4 Input_Color : TEXCOORD4;
    float4 Input_PosP : TEXCOORD5;
    float4 gl_FragCoord : VPOS;
};

struct SPIRV_Cross_Output
{
    float4 _entryPointOutput : COLOR0;
};

float4 _main(PS_Input Input)
{
    float4 Output = tex2D(Sampler_sampler_colorTex, Input.UV) * Input.Color;
    float3 texNormal = (tex2D(Sampler_sampler_normalTex, Input.UV).xyz - 0.5f.xxx) * 2.0f;
    float3 localNormal = normalize(mul(texNormal, float3x3(float3(Input.Tangent), float3(Input.Binormal), float3(Input.Normal))));
    float diffuse = max(dot(_80_fLightDirection.xyz, localNormal), 0.0f);
    float3 _100 = Output.xyz * ((_80_fLightColor.xyz * diffuse) + _80_fLightAmbient.xyz);
    Output = float4(_100.x, _100.y, _100.z, Output.w);
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
    Input.UV = Input_UV;
    Input.Normal = Input_Normal;
    Input.Binormal = Input_Binormal;
    Input.Tangent = Input_Tangent;
    Input.Color = Input_Color;
    Input.PosP = Input_PosP;
    float4 _146 = _main(Input);
    _entryPointOutput = _146;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord + float4(0.5f, 0.5f, 0.0f, 0.0f);
    Input_UV = stage_input.Input_UV;
    Input_Normal = stage_input.Input_Normal;
    Input_Binormal = stage_input.Input_Binormal;
    Input_Tangent = stage_input.Input_Tangent;
    Input_Color = stage_input.Input_Color;
    Input_PosP = stage_input.Input_PosP;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output._entryPointOutput = float4(_entryPointOutput);
    return stage_output;
}
