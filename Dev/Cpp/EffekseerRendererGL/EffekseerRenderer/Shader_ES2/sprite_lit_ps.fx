#version 200 es
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

uniform highp sampler2D Sampler_g_normalSampler;
uniform highp sampler2D Sampler_g_colorSampler;

centroid varying highp vec4 _VSPS_VColor;
centroid varying highp vec2 _VSPS_UV1;
centroid varying highp vec2 _VSPS_UV2;
varying highp vec3 _VSPS_WorldP;
varying highp vec3 _VSPS_WorldN;
varying highp vec3 _VSPS_WorldT;
varying highp vec3 _VSPS_WorldB;
varying highp vec2 _VSPS_ScreenUV;
varying highp vec4 _VSPS_PosP;

highp vec4 _main(PS_Input Input)
{
    highp vec3 loN = texture2D(Sampler_g_normalSampler, Input.UV1).xyz;
    highp vec3 texNormal = (loN - vec3(0.5)) * 2.0;
    highp vec3 localNormal = normalize(mat3(vec3(Input.WorldT), vec3(Input.WorldB), vec3(Input.WorldN)) * texNormal);
    highp float diffuse = max(dot(CBPS0.fLightDirection.xyz, localNormal), 0.0);
    highp vec4 Output = texture2D(Sampler_g_colorSampler, Input.UV1) * Input.VColor;
    highp vec3 _104 = Output.xyz * ((CBPS0.fLightColor.xyz * diffuse) + vec3(CBPS0.fLightAmbient.xyz));
    Output = vec4(_104.x, _104.y, _104.z, Output.w);
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
    highp vec4 _162 = _main(Input);
    gl_FragData[0] = _162;
}

