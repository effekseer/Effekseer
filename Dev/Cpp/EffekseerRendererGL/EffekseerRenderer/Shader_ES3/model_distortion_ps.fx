#version 300 es
precision mediump float;
precision highp int;

struct PS_Input
{
    highp vec4 Position;
    highp vec2 UV;
    highp vec4 Normal;
    highp vec4 Binormal;
    highp vec4 Tangent;
    highp vec4 Pos;
    highp vec4 Color;
};

struct PS_ConstanBuffer
{
    highp vec4 g_scale;
    highp vec4 mUVInversedBack;
    highp vec4 fFlipbookParameter;
    highp vec4 fUVDistortionParameter;
    highp vec4 fBlendTextureParameter;
};

uniform PS_ConstanBuffer CBPS0;

uniform highp sampler2D Sampler_g_sampler;
uniform highp sampler2D Sampler_g_backSampler;

centroid in highp vec2 _VSPS_UV;
in highp vec4 _VSPS_Normal;
in highp vec4 _VSPS_Binormal;
in highp vec4 _VSPS_Tangent;
in highp vec4 _VSPS_Pos;
centroid in highp vec4 _VSPS_Color;
layout(location = 0) out highp vec4 _entryPointOutput;

highp vec4 _main(PS_Input Input)
{
    highp vec4 Output = texture(Sampler_g_sampler, Input.UV);
    Output.w *= Input.Color.w;
    highp vec2 pos = Input.Pos.xy / vec2(Input.Pos.w);
    highp vec2 posU = Input.Tangent.xy / vec2(Input.Tangent.w);
    highp vec2 posR = Input.Binormal.xy / vec2(Input.Binormal.w);
    highp float xscale = (((Output.x * 2.0) - 1.0) * Input.Color.x) * CBPS0.g_scale.x;
    highp float yscale = (((Output.y * 2.0) - 1.0) * Input.Color.y) * CBPS0.g_scale.x;
    highp vec2 uv = (pos + ((posR - pos) * xscale)) + ((posU - pos) * yscale);
    uv.x = (uv.x + 1.0) * 0.5;
    uv.y = 1.0 - ((uv.y + 1.0) * 0.5);
    uv.y = CBPS0.mUVInversedBack.x + (CBPS0.mUVInversedBack.y * uv.y);
    uv.y = 1.0 - uv.y;
    highp vec3 color = vec3(texture(Sampler_g_backSampler, uv).xyz);
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
    highp vec4 _186 = _main(Input);
    _entryPointOutput = _186;
}

