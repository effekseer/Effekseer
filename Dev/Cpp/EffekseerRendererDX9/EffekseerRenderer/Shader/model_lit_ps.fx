struct PS_Input
{
    float4 PosVS;
    float4 Color;
    float2 UV;
    float3 WorldN;
    float3 WorldB;
    float3 WorldT;
    float4 PosP;
};

cbuffer PS_ConstanBuffer : register(b0)
{
    float4 _141_fLightDirection : register(c0);
    float4 _141_fLightColor : register(c1);
    float4 _141_fLightAmbient : register(c2);
    float4 _141_fFlipbookParameter : register(c3);
    float4 _141_fUVDistortionParameter : register(c4);
    float4 _141_fBlendTextureParameter : register(c5);
    float4 _141_fCameraFrontDirection : register(c6);
    float4 _141_fFalloffParameter : register(c7);
    float4 _141_fFalloffBeginColor : register(c8);
    float4 _141_fFalloffEndColor : register(c9);
    float4 _141_fEmissiveScaling : register(c10);
    float4 _141_fEdgeColor : register(c11);
    float4 _141_fEdgeParameter : register(c12);
    float4 _141_softParticleParam : register(c13);
    float4 _141_reconstructionParam1 : register(c14);
    float4 _141_reconstructionParam2 : register(c15);
    float4 _141_mUVInversedBack : register(c16);
    float4 _141_miscFlags : register(c17);
};

uniform sampler2D Sampler_sampler_colorTex : register(s0);
uniform sampler2D Sampler_sampler_normalTex : register(s1);

static float4 gl_FragCoord;
static float4 Input_Color;
static float2 Input_UV;
static float3 Input_WorldN;
static float3 Input_WorldB;
static float3 Input_WorldT;
static float4 Input_PosP;
static float4 _entryPointOutput;

struct SPIRV_Cross_Input
{
    centroid float4 Input_Color : TEXCOORD0;
    centroid float2 Input_UV : TEXCOORD1;
    float3 Input_WorldN : TEXCOORD2;
    float3 Input_WorldB : TEXCOORD3;
    float3 Input_WorldT : TEXCOORD4;
    float4 Input_PosP : TEXCOORD5;
    float4 gl_FragCoord : VPOS;
};

struct SPIRV_Cross_Output
{
    float4 _entryPointOutput : COLOR0;
};

float3 PositivePow(float3 base, float3 power)
{
    return pow(max(abs(base), 1.1920928955078125e-07f.xxx), power);
}

float3 LinearToSRGB(float3 c)
{
    float3 param = c;
    float3 param_1 = 0.4166666567325592041015625f.xxx;
    return max((PositivePow(param, param_1) * 1.05499994754791259765625f) - 0.054999999701976776123046875f.xxx, 0.0f.xxx);
}

float4 LinearToSRGB(float4 c)
{
    float3 param = c.xyz;
    return float4(LinearToSRGB(param), c.w);
}

float4 ConvertFromSRGBTexture(float4 c, bool isValid)
{
    if (!isValid)
    {
        return c;
    }
    float4 param = c;
    return LinearToSRGB(param);
}

float3 SRGBToLinear(float3 c)
{
    return min(c, c * ((c * ((c * 0.305306017398834228515625f) + 0.6821711063385009765625f.xxx)) + 0.01252287812530994415283203125f.xxx));
}

float4 SRGBToLinear(float4 c)
{
    float3 param = c.xyz;
    return float4(SRGBToLinear(param), c.w);
}

float4 ConvertToScreen(float4 c, bool isValid)
{
    if (!isValid)
    {
        return c;
    }
    float4 param = c;
    return SRGBToLinear(param);
}

float4 _main(PS_Input Input)
{
    bool convertColorSpace = _141_miscFlags.x != 0.0f;
    float4 param = tex2D(Sampler_sampler_colorTex, Input.UV);
    bool param_1 = convertColorSpace;
    float4 Output = ConvertFromSRGBTexture(param, param_1) * Input.Color;
    float3 texNormal = (tex2D(Sampler_sampler_normalTex, Input.UV).xyz - 0.5f.xxx) * 2.0f;
    float3 localNormal = normalize(mul(texNormal, float3x3(float3(Input.WorldT), float3(Input.WorldB), float3(Input.WorldN))));
    float diffuse = max(dot(_141_fLightDirection.xyz, localNormal), 0.0f);
    float3 _229 = Output.xyz * ((_141_fLightColor.xyz * diffuse) + _141_fLightAmbient.xyz);
    Output = float4(_229.x, _229.y, _229.z, Output.w);
    float3 _237 = Output.xyz * _141_fEmissiveScaling.x;
    Output = float4(_237.x, _237.y, _237.z, Output.w);
    if (Output.w == 0.0f)
    {
        discard;
    }
    float4 param_2 = Output;
    bool param_3 = convertColorSpace;
    return ConvertToScreen(param_2, param_3);
}

void frag_main()
{
    PS_Input Input;
    Input.PosVS = gl_FragCoord;
    Input.Color = Input_Color;
    Input.UV = Input_UV;
    Input.WorldN = Input_WorldN;
    Input.WorldB = Input_WorldB;
    Input.WorldT = Input_WorldT;
    Input.PosP = Input_PosP;
    float4 _284 = _main(Input);
    _entryPointOutput = _284;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord + float4(0.5f, 0.5f, 0.0f, 0.0f);
    Input_Color = stage_input.Input_Color;
    Input_UV = stage_input.Input_UV;
    Input_WorldN = stage_input.Input_WorldN;
    Input_WorldB = stage_input.Input_WorldB;
    Input_WorldT = stage_input.Input_WorldT;
    Input_PosP = stage_input.Input_PosP;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output._entryPointOutput = float4(_entryPointOutput);
    return stage_output;
}
