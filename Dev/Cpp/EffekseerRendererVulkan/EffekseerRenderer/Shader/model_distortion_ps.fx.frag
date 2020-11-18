#version 420

struct PS_Input
{
    vec4 PosVS;
    vec2 UV;
    vec4 Normal;
    vec4 Binormal;
    vec4 Tangent;
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
    vec4 softParticleAndReconstructionParam1;
    vec4 reconstructionParam2;
} _129;

layout(set = 1, binding = 1) uniform sampler2D Sampler_g_sampler;
layout(set = 1, binding = 2) uniform sampler2D Sampler_g_backSampler;
layout(set = 1, binding = 3) uniform sampler2D Sampler_g_depthSampler;

layout(location = 0) centroid in vec2 Input_UV;
layout(location = 1) in vec4 Input_Normal;
layout(location = 2) in vec4 Input_Binormal;
layout(location = 3) in vec4 Input_Tangent;
layout(location = 4) in vec4 Input_PosP;
layout(location = 5) centroid in vec4 Input_Color;
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
    vec4 Output = texture(Sampler_g_sampler, Input.UV);
    Output.w *= Input.Color.w;
    vec2 pos = Input.PosP.xy / vec2(Input.PosP.w);
    vec2 posU = Input.Tangent.xy / vec2(Input.Tangent.w);
    vec2 posR = Input.Binormal.xy / vec2(Input.Binormal.w);
    float xscale = (((Output.x * 2.0) - 1.0) * Input.Color.x) * _129.g_scale.x;
    float yscale = (((Output.y * 2.0) - 1.0) * Input.Color.y) * _129.g_scale.x;
    vec2 uv = (pos + ((posR - pos) * xscale)) + ((posU - pos) * yscale);
    uv.x = (uv.x + 1.0) * 0.5;
    uv.y = 1.0 - ((uv.y + 1.0) * 0.5);
    uv.y = _129.mUVInversedBack.x + (_129.mUVInversedBack.y * uv.y);
    vec3 color = vec3(texture(Sampler_g_backSampler, uv).xyz);
    Output = vec4(color.x, color.y, color.z, Output.w);
    vec4 screenPos = Input.PosP / vec4(Input.PosP.w);
    vec2 screenUV = (screenPos.xy + vec2(1.0)) / vec2(2.0);
    screenUV.y = 1.0 - screenUV.y;
    float backgroundZ = texture(Sampler_g_depthSampler, screenUV).x;
    if (!(_129.softParticleAndReconstructionParam1.x == 0.0))
    {
        float param = backgroundZ;
        float param_1 = screenPos.z;
        float param_2 = _129.softParticleAndReconstructionParam1.x;
        vec2 param_3 = _129.softParticleAndReconstructionParam1.yz;
        vec4 param_4 = _129.reconstructionParam2;
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
    Input.Normal = Input_Normal;
    Input.Binormal = Input_Binormal;
    Input.Tangent = Input_Tangent;
    Input.PosP = Input_PosP;
    Input.Color = Input_Color;
    vec4 _287 = _main(Input);
    _entryPointOutput = _287;
}

