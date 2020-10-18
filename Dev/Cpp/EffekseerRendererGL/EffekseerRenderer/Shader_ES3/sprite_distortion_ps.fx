#version 300 es
precision mediump float;
precision highp int;

struct PS_Input
{
    highp vec4 PosVS;
    highp vec4 Color;
    highp vec2 UV;
    highp vec4 PosP;
    highp vec4 PosU;
    highp vec4 PosR;
};

struct VS_ConstantBuffer
{
    highp vec4 g_scale;
    highp vec4 mUVInversedBack;
    highp vec4 flipbookParameter;
    highp vec4 uvDistortionParameter;
    highp vec4 blendTextureParameter;
    highp vec4 softParticleAndReconstructionParam1;
    highp vec4 reconstructionParam2;
};

uniform VS_ConstantBuffer CBPS0;

uniform highp sampler2D Sampler_g_sampler;
uniform highp sampler2D Sampler_g_backSampler;
uniform highp sampler2D Sampler_g_depthSampler;

centroid in highp vec4 _VSPS_Color;
centroid in highp vec2 _VSPS_UV;
in highp vec4 _VSPS_PosP;
in highp vec4 _VSPS_PosU;
in highp vec4 _VSPS_PosR;
layout(location = 0) out highp vec4 _entryPointOutput;

highp float SoftParticle(highp float backgroundZ, highp float meshZ, highp float softparticleParam, highp vec2 reconstruct1, highp vec4 reconstruct2)
{
    highp float _distance = softparticleParam;
    highp vec2 rescale = reconstruct1;
    highp vec4 params = reconstruct2;
    highp float tempY = params.y;
    params.y = params.z;
    params.z = tempY;
    highp vec2 zs = vec2((backgroundZ * rescale.x) + rescale.y, meshZ);
    highp vec2 depth = ((zs * params.w) - vec2(params.y)) / (vec2(params.x) - (zs * params.z));
    return min(max((depth.y - depth.x) / _distance, 0.0), 1.0);
}

highp vec4 _main(PS_Input Input)
{
    highp vec4 Output = texture(Sampler_g_sampler, Input.UV);
    Output.w *= Input.Color.w;
    highp vec2 pos = Input.PosP.xy / vec2(Input.PosP.w);
    highp vec2 posU = Input.PosU.xy / vec2(Input.PosU.w);
    highp vec2 posR = Input.PosR.xy / vec2(Input.PosR.w);
    highp float xscale = (((Output.x * 2.0) - 1.0) * Input.Color.x) * CBPS0.g_scale.x;
    highp float yscale = (((Output.y * 2.0) - 1.0) * Input.Color.y) * CBPS0.g_scale.x;
    highp vec2 uv = (pos + ((posR - pos) * xscale)) + ((posU - pos) * yscale);
    uv.x = (uv.x + 1.0) * 0.5;
    uv.y = 1.0 - ((uv.y + 1.0) * 0.5);
    uv.y = CBPS0.mUVInversedBack.x + (CBPS0.mUVInversedBack.y * uv.y);
    uv.y = 1.0 - uv.y;
    highp vec3 color = vec3(texture(Sampler_g_backSampler, uv).xyz);
    Output = vec4(color.x, color.y, color.z, Output.w);
    highp vec4 screenPos = Input.PosP / vec4(Input.PosP.w);
    highp vec2 screenUV = (screenPos.xy + vec2(1.0)) / vec2(2.0);
    screenUV.y = 1.0 - screenUV.y;
    screenUV.y = 1.0 - screenUV.y;
    highp float backgroundZ = texture(Sampler_g_depthSampler, screenUV).x;
    if (!(CBPS0.softParticleAndReconstructionParam1.x == 0.0))
    {
        highp float param = backgroundZ;
        highp float param_1 = screenPos.z;
        highp float param_2 = CBPS0.softParticleAndReconstructionParam1.x;
        highp vec2 param_3 = CBPS0.softParticleAndReconstructionParam1.yz;
        highp vec4 param_4 = CBPS0.reconstructionParam2;
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
    highp vec4 _300 = _main(Input);
    _entryPointOutput = _300;
}

