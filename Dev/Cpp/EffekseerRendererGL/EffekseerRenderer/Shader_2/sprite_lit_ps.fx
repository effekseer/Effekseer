#version 120
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_Input
{
    vec4 PosVS;
    vec4 VColor;
    vec2 UV1;
    vec2 UV2;
    vec3 WorldP;
    vec3 WorldN;
    vec3 WorldT;
    vec3 WorldB;
    vec2 ScreenUV;
    vec4 PosP;
};

struct VS_ConstantBuffer
{
    vec4 fLightDirection;
    vec4 fLightColor;
    vec4 fLightAmbient;
    vec4 fFlipbookParameter;
    vec4 fUVDistortionParameter;
    vec4 fBlendTextureParameter;
    vec4 fEmissiveScaling;
    vec4 fEdgeColor;
    vec4 fEdgeParameter;
    vec4 softParticleAndReconstructionParam1;
    vec4 reconstructionParam2;
};

uniform VS_ConstantBuffer CBPS0;

uniform sampler2D Sampler_sampler_normalTex;
uniform sampler2D Sampler_sampler_colorTex;

centroid varying vec4 _VSPS_VColor;
centroid varying vec2 _VSPS_UV1;
centroid varying vec2 _VSPS_UV2;
varying vec3 _VSPS_WorldP;
varying vec3 _VSPS_WorldN;
varying vec3 _VSPS_WorldT;
varying vec3 _VSPS_WorldB;
varying vec2 _VSPS_ScreenUV;
varying vec4 _VSPS_PosP;

vec4 _main(PS_Input Input)
{
    vec3 loN = texture2D(Sampler_sampler_normalTex, Input.UV1).xyz;
    vec3 texNormal = (loN - vec3(0.5)) * 2.0;
    vec3 localNormal = normalize(mat3(vec3(Input.WorldT), vec3(Input.WorldB), vec3(Input.WorldN)) * texNormal);
    float diffuse = max(dot(CBPS0.fLightDirection.xyz, localNormal), 0.0);
    vec4 Output = texture2D(Sampler_sampler_colorTex, Input.UV1) * Input.VColor;
    vec3 _104 = Output.xyz * ((CBPS0.fLightColor.xyz * diffuse) + vec3(CBPS0.fLightAmbient.xyz));
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
    vec4 _162 = _main(Input);
    gl_FragData[0] = _162;
}

