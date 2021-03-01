#version 420

struct PS_Input
{
    vec4 PosVS;
    vec4 Color;
    vec2 UV;
    vec3 WorldN;
    vec3 WorldB;
    vec3 WorldT;
    vec4 PosP;
};

layout(set = 1, binding = 0, std140) uniform PS_ConstanBuffer
{
    vec4 fLightDirection;
    vec4 fLightColor;
    vec4 fLightAmbient;
    vec4 fFlipbookParameter;
    vec4 fUVDistortionParameter;
    vec4 fBlendTextureParameter;
    vec4 fCameraFrontDirection;
    vec4 fFalloffParameter;
    vec4 fFalloffBeginColor;
    vec4 fFalloffEndColor;
    vec4 fEmissiveScaling;
    vec4 fEdgeColor;
    vec4 fEdgeParameter;
    vec4 softParticleParam;
    vec4 reconstructionParam1;
    vec4 reconstructionParam2;
    vec4 mUVInversedBack;
} _158;

layout(set = 1, binding = 1) uniform sampler2D Sampler_sampler_colorTex;
layout(set = 1, binding = 2) uniform sampler2D Sampler_sampler_normalTex;
layout(set = 1, binding = 3) uniform sampler2D Sampler_sampler_depthTex;

layout(location = 0) centroid in vec4 Input_Color;
layout(location = 1) centroid in vec2 Input_UV;
layout(location = 2) in vec3 Input_WorldN;
layout(location = 3) in vec3 Input_WorldB;
layout(location = 4) in vec3 Input_WorldT;
layout(location = 5) in vec4 Input_PosP;
layout(location = 0) out vec4 _entryPointOutput;

float SoftParticle(float backgroundZ, float meshZ, vec4 softparticleParam, vec4 reconstruct1, vec4 reconstruct2)
{
    float distanceFar = softparticleParam.x;
    float distanceNear = softparticleParam.y;
    float distanceNearOffset = softparticleParam.z;
    vec2 rescale = reconstruct1.xy;
    vec4 params = reconstruct2;
    vec2 zs = vec2((backgroundZ * rescale.x) + rescale.y, meshZ);
    vec2 depth = ((zs * params.w) - vec2(params.y)) / (vec2(params.x) - (zs * params.z));
    float alphaFar = (depth.y - depth.x) / distanceFar;
    float alphaNear = ((-distanceNearOffset) - depth.y) / distanceNear;
    return min(max(min(alphaFar, alphaNear), 0.0), 1.0);
}

vec4 _main(PS_Input Input)
{
    vec4 Output = texture(Sampler_sampler_colorTex, Input.UV) * Input.Color;
    vec3 texNormal = (texture(Sampler_sampler_normalTex, Input.UV).xyz - vec3(0.5)) * 2.0;
    vec3 localNormal = normalize(mat3(vec3(Input.WorldT), vec3(Input.WorldB), vec3(Input.WorldN)) * texNormal);
    float diffuse = max(dot(_158.fLightDirection.xyz, localNormal), 0.0);
    vec3 _178 = Output.xyz * ((_158.fLightColor.xyz * diffuse) + _158.fLightAmbient.xyz);
    Output = vec4(_178.x, _178.y, _178.z, Output.w);
    vec3 _187 = Output.xyz * _158.fEmissiveScaling.x;
    Output = vec4(_187.x, _187.y, _187.z, Output.w);
    vec4 screenPos = Input.PosP / vec4(Input.PosP.w);
    vec2 screenUV = (screenPos.xy + vec2(1.0)) / vec2(2.0);
    screenUV.y = 1.0 - screenUV.y;
    screenUV.y = _158.mUVInversedBack.x + (_158.mUVInversedBack.y * screenUV.y);
    if (!(_158.softParticleParam.w == 0.0))
    {
        float backgroundZ = texture(Sampler_sampler_depthTex, screenUV).x;
        float param = backgroundZ;
        float param_1 = screenPos.z;
        vec4 param_2 = _158.softParticleParam;
        vec4 param_3 = _158.reconstructionParam1;
        vec4 param_4 = _158.reconstructionParam2;
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
    Input.Color = Input_Color;
    Input.UV = Input_UV;
    Input.WorldN = Input_WorldN;
    Input.WorldB = Input_WorldB;
    Input.WorldT = Input_WorldT;
    Input.PosP = Input_PosP;
    vec4 _292 = _main(Input);
    _entryPointOutput = _292;
}

