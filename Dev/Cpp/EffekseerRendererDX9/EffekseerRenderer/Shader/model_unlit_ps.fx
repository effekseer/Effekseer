struct PS_Input
{
    float4 PosVS;
    float4 Color;
    float2 UV;
    float4 PosP;
};

cbuffer PS_ConstanBuffer : register(b0)
{
    float4 _35_fLightDirection : register(c0);
    float4 _35_fLightColor : register(c1);
    float4 _35_fLightAmbient : register(c2);
    float4 _35_fFlipbookParameter : register(c3);
    float4 _35_fUVDistortionParameter : register(c4);
    float4 _35_fBlendTextureParameter : register(c5);
    float4 _35_fCameraFrontDirection : register(c6);
    float4 _35_fFalloffParameter : register(c7);
    float4 _35_fFalloffBeginColor : register(c8);
    float4 _35_fFalloffEndColor : register(c9);
    float4 _35_fEmissiveScaling : register(c10);
    float4 _35_fEdgeColor : register(c11);
    float4 _35_fEdgeParameter : register(c12);
    float4 _35_softParticleParam : register(c13);
    float4 _35_reconstructionParam1 : register(c14);
    float4 _35_reconstructionParam2 : register(c15);
    float4 _35_mUVInversedBack : register(c16);
};

uniform sampler2D Sampler_sampler_colorTex : register(s0);

static float4 gl_FragCoord;
static float4 Input_Color;
static float2 Input_UV;
static float4 Input_PosP;
static float4 _entryPointOutput;

struct SPIRV_Cross_Input
{
    centroid float4 Input_Color : TEXCOORD0;
    centroid float2 Input_UV : TEXCOORD1;
    float4 Input_PosP : TEXCOORD2;
    float4 gl_FragCoord : VPOS;
};

struct SPIRV_Cross_Output
{
    float4 _entryPointOutput : COLOR0;
};

float4 _main(PS_Input Input)
{
    float4 Output = tex2D(Sampler_sampler_colorTex, Input.UV) * Input.Color;
    float3 _45 = Output.xyz * _35_fEmissiveScaling.x;
    Output = float4(_45.x, _45.y, _45.z, Output.w);
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
    Input.Color = Input_Color;
    Input.UV = Input_UV;
    Input.PosP = Input_PosP;
    float4 _83 = _main(Input);
    _entryPointOutput = _83;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord + float4(0.5f, 0.5f, 0.0f, 0.0f);
    Input_Color = stage_input.Input_Color;
    Input_UV = stage_input.Input_UV;
    Input_PosP = stage_input.Input_PosP;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output._entryPointOutput = float4(_entryPointOutput);
    return stage_output;
}
