#version 420

struct PS_Input
{
    vec4 PosVS;
    vec2 UV;
    vec4 Color;
    vec4 PosP;
};

layout(set = 1, binding = 0, std140) uniform VS_ConstantBuffer
{
    vec4 fLightDirection;
    vec4 fLightColor;
    vec4 fLightAmbient;
    vec4 softParticleAndReconstructionParam1;
    vec4 reconstructionParam2;
} _124;

layout(set = 1, binding = 1) uniform sampler2D Sampler_g_colorSampler;
layout(set = 1, binding = 2) uniform sampler2D Sampler_g_depthSampler;

layout(location = 0) centroid in vec2 Input_UV;
layout(location = 1) centroid in vec4 Input_Color;
layout(location = 2) in vec4 Input_PosP;
layout(location = 0) out vec4 _entryPointOutput;

float SoftParticle(float backgroundZ, float meshZ, float softparticleParam, vec2 reconstruct1, vec4 reconstruct2)
{
    float _distance = softparticleParam;
    vec2 rescale = reconstruct1;
    vec4 params = reconstruct2;
    vec2 zs = vec2((backgroundZ * rescale.x) + rescale.y, meshZ);
    vec2 depth = ((zs * params.w) - vec2(params.y)) / (vec2(params.x) - (zs * params.z));
    return min(max((depth.y - depth.x) / _distance, 0.0), 1.0);
}

vec4 _main(PS_Input Input)
{
    vec4 Output = texture(Sampler_g_colorSampler, Input.UV) * Input.Color;
    vec4 screenPos = Input.PosP / vec4(Input.PosP.w);
    vec2 screenUV = (screenPos.xy + vec2(1.0)) / vec2(2.0);
    screenUV.y = 1.0 - screenUV.y;
    float backgroundZ = texture(Sampler_g_depthSampler, screenUV).x;
    if (!(_124.softParticleAndReconstructionParam1.x == 0.0))
    {
        float param = backgroundZ;
        float param_1 = screenPos.z;
        float param_2 = _124.softParticleAndReconstructionParam1.x;
        vec2 param_3 = _124.softParticleAndReconstructionParam1.yz;
        vec4 param_4 = _124.reconstructionParam2;
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
    Input.Color = Input_Color;
    Input.PosP = Input_PosP;
    vec4 _183 = _main(Input);
    _entryPointOutput = _183;
}

