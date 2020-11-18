#version 300 es
precision mediump float;
precision highp int;

struct PS_Input
{
    highp vec4 PosVS;
    highp vec2 UV;
    highp vec3 Normal;
    highp vec3 Binormal;
    highp vec3 Tangent;
    highp vec4 Color;
    highp vec4 PosP;
};

struct VS_ConstantBuffer
{
    highp vec4 fLightDirection;
    highp vec4 fLightColor;
    highp vec4 fLightAmbient;
    highp vec4 softParticleAndReconstructionParam1;
    highp vec4 reconstructionParam2;
};

uniform VS_ConstantBuffer CBPS0;

uniform highp sampler2D Sampler_g_colorSampler;
uniform highp sampler2D Sampler_g_normalSampler;
uniform highp sampler2D Sampler_g_depthSampler;

centroid in highp vec2 _VSPS_UV;
in highp vec3 _VSPS_Normal;
in highp vec3 _VSPS_Binormal;
in highp vec3 _VSPS_Tangent;
centroid in highp vec4 _VSPS_Color;
in highp vec4 _VSPS_PosP;
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
    highp vec4 Output = texture(Sampler_g_colorSampler, Input.UV) * Input.Color;
    highp vec3 texNormal = (texture(Sampler_g_normalSampler, Input.UV).xyz - vec3(0.5)) * 2.0;
    highp vec3 localNormal = normalize(mat3(vec3(Input.Tangent), vec3(Input.Binormal), vec3(Input.Normal)) * texNormal);
    highp float diffuse = max(dot(CBPS0.fLightDirection.xyz, localNormal), 0.0);
    highp vec3 _166 = Output.xyz * ((CBPS0.fLightColor.xyz * diffuse) + CBPS0.fLightAmbient.xyz);
    Output = vec4(_166.x, _166.y, _166.z, Output.w);
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
    Input.UV = _VSPS_UV;
    Input.Normal = _VSPS_Normal;
    Input.Binormal = _VSPS_Binormal;
    Input.Tangent = _VSPS_Tangent;
    Input.Color = _VSPS_Color;
    Input.PosP = _VSPS_PosP;
    highp vec4 _264 = _main(Input);
    _entryPointOutput = _264;
}

