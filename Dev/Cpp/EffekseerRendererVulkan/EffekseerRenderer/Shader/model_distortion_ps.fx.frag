#version 420

struct PS_Input
{
    vec4 PosVS;
    vec2 UV;
    vec4 ProjBinormal;
    vec4 ProjTangent;
    vec4 PosP;
    vec4 Color;
};

layout(set = 1, binding = 0, std140) uniform PS_ConstanBuffer
{
    vec4 g_scale;
    vec4 mUVInversedBack;
    vec4 fFlipbookParameter;
    vec4 fUVDistortionParameter;
    vec4 fBlendTextureParameter;
    vec4 softParticleParam;
    vec4 reconstructionParam1;
    vec4 reconstructionParam2;
} _149;

layout(set = 1, binding = 1) uniform sampler2D Sampler_sampler_colorTex;
layout(set = 1, binding = 2) uniform sampler2D Sampler_sampler_backTex;
layout(set = 1, binding = 3) uniform sampler2D Sampler_sampler_depthTex;

layout(location = 0) centroid in vec2 Input_UV;
layout(location = 1) in vec4 Input_ProjBinormal;
layout(location = 2) in vec4 Input_ProjTangent;
layout(location = 3) in vec4 Input_PosP;
layout(location = 4) centroid in vec4 Input_Color;
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
    vec4 Output = texture(Sampler_sampler_colorTex, Input.UV);
    Output.w *= Input.Color.w;
    vec2 pos = Input.PosP.xy / vec2(Input.PosP.w);
    vec2 posR = Input.ProjTangent.xy / vec2(Input.ProjTangent.w);
    vec2 posU = Input.ProjBinormal.xy / vec2(Input.ProjBinormal.w);
    float xscale = (((Output.x * 2.0) - 1.0) * Input.Color.x) * _149.g_scale.x;
    float yscale = (((Output.y * 2.0) - 1.0) * Input.Color.y) * _149.g_scale.x;
    vec2 uv = (pos + ((posR - pos) * xscale)) + ((posU - pos) * yscale);
    uv.x = (uv.x + 1.0) * 0.5;
    uv.y = 1.0 - ((uv.y + 1.0) * 0.5);
    uv.y = _149.mUVInversedBack.x + (_149.mUVInversedBack.y * uv.y);
    vec3 color = vec3(texture(Sampler_sampler_backTex, uv).xyz);
    Output = vec4(color.x, color.y, color.z, Output.w);
    vec4 screenPos = Input.PosP / vec4(Input.PosP.w);
    vec2 screenUV = (screenPos.xy + vec2(1.0)) / vec2(2.0);
    screenUV.y = 1.0 - screenUV.y;
    if (!(_149.softParticleParam.w == 0.0))
    {
        float backgroundZ = texture(Sampler_sampler_depthTex, screenUV).x;
        float param = backgroundZ;
        float param_1 = screenPos.z;
        vec4 param_2 = _149.softParticleParam;
        vec4 param_3 = _149.reconstructionParam1;
        vec4 param_4 = _149.reconstructionParam2;
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
    Input.UV = Input_UV;
    Input.ProjBinormal = Input_ProjBinormal;
    Input.ProjTangent = Input_ProjTangent;
    Input.PosP = Input_PosP;
    Input.Color = Input_Color;
    vec4 _304 = _main(Input);
    _entryPointOutput = _304;
}

