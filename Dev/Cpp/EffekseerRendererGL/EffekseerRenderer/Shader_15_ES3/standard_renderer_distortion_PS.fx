#version 300 es
precision mediump float;
precision highp int;

struct PS_Input
{
    highp vec4 Position;
    highp vec4 Color;
    highp vec2 UV;
    highp vec4 Pos;
    highp vec4 PosU;
    highp vec4 PosR;
};

struct VS_ConstantBuffer
{
    highp vec4 g_scale;
    highp vec4 mUVInversedBack;
};

uniform VS_ConstantBuffer CBPS0;

uniform highp sampler2D Sampler_g_sampler;
uniform highp sampler2D Sampler_g_backSampler;

in highp vec4 _VSPS_Color;
in highp vec2 _VSPS_UV;
in highp vec4 _VSPS_Pos;
in highp vec4 _VSPS_PosU;
in highp vec4 _VSPS_PosR;
layout(location = 0) out highp vec4 _entryPointOutput;

highp vec4 _main(PS_Input Input)
{
    highp vec4 Output = texture(Sampler_g_sampler, Input.UV);
    Output.w *= Input.Color.w;
    highp vec2 pos = Input.Pos.xy / vec2(Input.Pos.w);
    highp vec2 posU = Input.PosU.xy / vec2(Input.PosU.w);
    highp vec2 posR = Input.PosR.xy / vec2(Input.PosR.w);
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
    Input.Color = _VSPS_Color;
    Input.UV = _VSPS_UV;
    Input.Pos = _VSPS_Pos;
    Input.PosU = _VSPS_PosU;
    Input.PosR = _VSPS_PosR;
    highp vec4 _182 = _main(Input);
    _entryPointOutput = _182;
}

