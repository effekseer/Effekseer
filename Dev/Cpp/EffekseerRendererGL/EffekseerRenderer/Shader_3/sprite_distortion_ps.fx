#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_Input
{
    vec4 PosVS;
    vec4 Color;
    vec2 UV;
    vec4 PosP;
    vec4 PosU;
    vec4 PosR;
};

struct VS_ConstantBuffer
{
    vec4 g_scale;
    vec4 mUVInversedBack;
    vec4 flipbookParameter;
    vec4 uvDistortionParameter;
    vec4 blendTextureParameter;
    vec4 softParticleAndReconstructionParam1;
    vec4 reconstructionParam2;
};

uniform VS_ConstantBuffer CBPS0;

layout(binding = 0) uniform sampler2D Sampler_sampler_colorTex;
layout(binding = 1) uniform sampler2D Sampler_sampler_backTex;
layout(binding = 2) uniform sampler2D Sampler_sampler_depthTex;

centroid in vec4 _VSPS_Color;
centroid in vec2 _VSPS_UV;
in vec4 _VSPS_PosP;
in vec4 _VSPS_PosU;
in vec4 _VSPS_PosR;
layout(location = 0) out vec4 _entryPointOutput;

float SoftParticle(float backgroundZ, float meshZ, float softparticleParam, vec2 reconstruct1, vec4 reconstruct2)
{
    float _distance = softparticleParam;
    vec2 rescale = reconstruct1;
    vec4 params = reconstruct2;
    float tempY = params.y;
    params.y = params.z;
    params.z = tempY;
    vec2 zs = vec2((backgroundZ * rescale.x) + rescale.y, meshZ);
    vec2 depth = ((zs * params.w) - vec2(params.y)) / (vec2(params.x) - (zs * params.z));
    return min(max((depth.y - depth.x) / _distance, 0.0), 1.0);
}

vec4 _main(PS_Input Input)
{
    vec4 Output = texture(Sampler_sampler_colorTex, Input.UV);
    Output.w *= Input.Color.w;
    vec2 pos = Input.PosP.xy / vec2(Input.PosP.w);
    vec2 posU = Input.PosU.xy / vec2(Input.PosU.w);
    vec2 posR = Input.PosR.xy / vec2(Input.PosR.w);
    float xscale = (((Output.x * 2.0) - 1.0) * Input.Color.x) * CBPS0.g_scale.x;
    float yscale = (((Output.y * 2.0) - 1.0) * Input.Color.y) * CBPS0.g_scale.x;
    vec2 uv = (pos + ((posR - pos) * xscale)) + ((posU - pos) * yscale);
    uv.x = (uv.x + 1.0) * 0.5;
    uv.y = 1.0 - ((uv.y + 1.0) * 0.5);
    uv.y = CBPS0.mUVInversedBack.x + (CBPS0.mUVInversedBack.y * uv.y);
    uv.y = 1.0 - uv.y;
    vec3 color = vec3(texture(Sampler_sampler_backTex, uv).xyz);
    Output = vec4(color.x, color.y, color.z, Output.w);
    vec4 screenPos = Input.PosP / vec4(Input.PosP.w);
    vec2 screenUV = (screenPos.xy + vec2(1.0)) / vec2(2.0);
    screenUV.y = 1.0 - screenUV.y;
    screenUV.y = 1.0 - screenUV.y;
    float backgroundZ = texture(Sampler_sampler_depthTex, screenUV).x;
    if (!(CBPS0.softParticleAndReconstructionParam1.x == 0.0))
    {
        float param = backgroundZ;
        float param_1 = screenPos.z;
        float param_2 = CBPS0.softParticleAndReconstructionParam1.x;
        vec2 param_3 = CBPS0.softParticleAndReconstructionParam1.yz;
        vec4 param_4 = CBPS0.reconstructionParam2;
        Output.w *= SoftParticle(param, param_1, param_2, param_3, param_4);
    }
    if (Output.w == 0.0)
    {
        discard;
    }
    return Output;
}

void main()
{
    PS_Input Input;
    Input.PosVS = gl_FragCoord;
    Input.Color = _VSPS_Color;
    Input.UV = _VSPS_UV;
    Input.PosP = _VSPS_PosP;
    Input.PosU = _VSPS_PosU;
    Input.PosR = _VSPS_PosR;
    vec4 _300 = _main(Input);
    _entryPointOutput = _300;
}

