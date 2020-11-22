#version 300 es
precision mediump float;
precision highp int;

struct PS_Input
{
    highp vec4 PosVS;
    highp vec4 VColor;
    highp vec2 UV1;
    highp vec2 UV2;
    highp vec3 WorldP;
    highp vec3 WorldN;
    highp vec3 WorldT;
    highp vec3 WorldB;
    highp vec2 ScreenUV;
    highp vec4 PosP;
};

struct VS_ConstantBuffer
{
    highp vec4 fLightDirection;
    highp vec4 fLightColor;
    highp vec4 fLightAmbient;
    highp vec4 fFlipbookParameter;
    highp vec4 fUVDistortionParameter;
    highp vec4 fBlendTextureParameter;
    highp vec4 fEmissiveScaling;
    highp vec4 fEdgeColor;
    highp vec4 fEdgeParameter;
    highp vec4 softParticleAndReconstructionParam1;
    highp vec4 reconstructionParam2;
};

uniform VS_ConstantBuffer CBPS0;

uniform highp sampler2D Sampler_sampler_normalTex;
uniform highp sampler2D Sampler_sampler_colorTex;
uniform highp sampler2D Sampler_sampler_depthTex;

centroid in highp vec4 _VSPS_VColor;
centroid in highp vec2 _VSPS_UV1;
centroid in highp vec2 _VSPS_UV2;
in highp vec3 _VSPS_WorldP;
in highp vec3 _VSPS_WorldN;
in highp vec3 _VSPS_WorldT;
in highp vec3 _VSPS_WorldB;
in highp vec2 _VSPS_ScreenUV;
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
    highp vec3 loN = texture(Sampler_sampler_normalTex, Input.UV1).xyz;
    highp vec3 texNormal = (loN - vec3(0.5)) * 2.0;
    highp vec3 localNormal = normalize(mat3(vec3(Input.WorldT), vec3(Input.WorldB), vec3(Input.WorldN)) * texNormal);
    highp float diffuse = max(dot(CBPS0.fLightDirection.xyz, localNormal), 0.0);
    highp vec4 Output = texture(Sampler_sampler_colorTex, Input.UV1) * Input.VColor;
    highp vec3 _171 = Output.xyz * ((CBPS0.fLightColor.xyz * diffuse) + vec3(CBPS0.fLightAmbient.xyz));
    Output = vec4(_171.x, _171.y, _171.z, Output.w);
    highp vec4 screenPos = Input.PosP / vec4(Input.PosP.w);
    highp vec2 screenUV = (screenPos.xy + vec2(1.0)) / vec2(2.0);
    screenUV.y = 1.0 - screenUV.y;
    screenUV.y = 1.0 - screenUV.y;
    highp float backgroundZ = texture(Sampler_sampler_depthTex, screenUV).x;
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
    Input.VColor = _VSPS_VColor;
    Input.UV1 = _VSPS_UV1;
    Input.UV2 = _VSPS_UV2;
    Input.WorldP = _VSPS_WorldP;
    Input.WorldN = _VSPS_WorldN;
    Input.WorldT = _VSPS_WorldT;
    Input.WorldB = _VSPS_WorldB;
    Input.ScreenUV = _VSPS_ScreenUV;
    Input.PosP = _VSPS_PosP;
    highp vec4 _282 = _main(Input);
    _entryPointOutput = _282;
}

