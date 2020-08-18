#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_Input
{
    vec4 Position;
    vec2 UV;
    vec4 Normal;
    vec4 Binormal;
    vec4 Tangent;
    vec4 Pos;
    vec4 Color;
};

struct PS_ConstanBuffer
{
    vec4 g_scale;
    vec4 mUVInversedBack;
    vec4 fFlipbookParameter;
    vec4 fUVDistortionParameter;
    vec4 fBlendTextureParameter;
};

uniform PS_ConstanBuffer CBPS0;

layout(binding = 0) uniform sampler2D Sampler_g_sampler;
layout(binding = 1) uniform sampler2D Sampler_g_backSampler;

in vec2 _VSPS_UV;
in vec4 _VSPS_Normal;
in vec4 _VSPS_Binormal;
in vec4 _VSPS_Tangent;
in vec4 _VSPS_Pos;
in vec4 _VSPS_Color;
layout(location = 0) out vec4 _entryPointOutput;

vec4 _main(PS_Input Input)
{
    vec4 Output = texture(Sampler_g_sampler, Input.UV);
    Output.w *= Input.Color.w;
    vec2 pos = Input.Pos.xy / vec2(Input.Pos.w);
    vec2 posU = Input.Tangent.xy / vec2(Input.Tangent.w);
    vec2 posR = Input.Binormal.xy / vec2(Input.Binormal.w);
    float xscale = (((Output.x * 2.0) - 1.0) * Input.Color.x) * CBPS0.g_scale.x;
    float yscale = (((Output.y * 2.0) - 1.0) * Input.Color.y) * CBPS0.g_scale.x;
    vec2 uv = (pos + ((posR - pos) * xscale)) + ((posU - pos) * yscale);
    uv.x = (uv.x + 1.0) * 0.5;
    uv.y = 1.0 - ((uv.y + 1.0) * 0.5);
    uv.y = CBPS0.mUVInversedBack.x + (CBPS0.mUVInversedBack.y * uv.y);
    uv.y = 1.0 - uv.y;
    vec3 color = vec3(texture(Sampler_g_backSampler, uv).xyz);
    Output = vec4(color.x, color.y, color.z, Output.w);
    if (Output.w == 0.0)
    {
        discard;
    }
    return Output;
}

void main()
{
    PS_Input Input;
    Input.Position = gl_FragCoord;
    Input.UV = _VSPS_UV;
    Input.Normal = _VSPS_Normal;
    Input.Binormal = _VSPS_Binormal;
    Input.Tangent = _VSPS_Tangent;
    Input.Pos = _VSPS_Pos;
    Input.Color = _VSPS_Color;
    vec4 _186 = _main(Input);
    _entryPointOutput = _186;
}

