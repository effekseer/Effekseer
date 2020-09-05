static const char sprite_unlit_ps_gl2[] = R"(
#version 120
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_Input
{
    vec4 Pos;
    vec4 Color;
    vec2 UV;
};

struct PS_ConstanBuffer
{
    vec4 flipbookParameter;
    vec4 uvDistortionParameter;
    vec4 blendTextureParameter;
    vec4 emissiveScaling;
    vec4 edgeColor;
    vec4 edgeParameter;
};

uniform PS_ConstanBuffer CBPS0;

uniform sampler2D Sampler_g_sampler;

varying vec4 _VSPS_Color;
varying vec2 _VSPS_UV;

vec4 _main(PS_Input Input)
{
    vec4 Output = Input.Color * texture2D(Sampler_g_sampler, Input.UV);
    if (Output.w == 0.0)
    {
        discard;
    }
    return Output;
}

void main()
{
    PS_Input Input;
    Input.Pos = gl_FragCoord;
    Input.Color = _VSPS_Color;
    Input.UV = _VSPS_UV;
    vec4 _65 = _main(Input);
    gl_FragData[0] = _65;
}

)";

static const char sprite_unlit_ps_gl3[] = R"(
#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_Input
{
    vec4 Pos;
    vec4 Color;
    vec2 UV;
};

struct PS_ConstanBuffer
{
    vec4 flipbookParameter;
    vec4 uvDistortionParameter;
    vec4 blendTextureParameter;
    vec4 emissiveScaling;
    vec4 edgeColor;
    vec4 edgeParameter;
};

uniform PS_ConstanBuffer CBPS0;

uniform sampler2D Sampler_g_sampler;

in vec4 _VSPS_Color;
in vec2 _VSPS_UV;
layout(location = 0) out vec4 _entryPointOutput;

vec4 _main(PS_Input Input)
{
    vec4 Output = Input.Color * texture(Sampler_g_sampler, Input.UV);
    if (Output.w == 0.0)
    {
        discard;
    }
    return Output;
}

void main()
{
    PS_Input Input;
    Input.Pos = gl_FragCoord;
    Input.Color = _VSPS_Color;
    Input.UV = _VSPS_UV;
    vec4 _65 = _main(Input);
    _entryPointOutput = _65;
}

)";

static const char sprite_unlit_ps_gles2[] = R"(

precision mediump float;
precision highp int;

struct PS_Input
{
    highp vec4 Pos;
    highp vec4 Color;
    highp vec2 UV;
};

struct PS_ConstanBuffer
{
    highp vec4 flipbookParameter;
    highp vec4 uvDistortionParameter;
    highp vec4 blendTextureParameter;
    highp vec4 emissiveScaling;
    highp vec4 edgeColor;
    highp vec4 edgeParameter;
};

uniform PS_ConstanBuffer CBPS0;

uniform  sampler2D Sampler_g_sampler;

varying  vec4 _VSPS_Color;
varying  vec2 _VSPS_UV;

highp vec4 _main(PS_Input Input)
{
    highp vec4 Output = Input.Color * texture2D(Sampler_g_sampler, Input.UV);
    if (Output.w == 0.0)
    {
        discard;
    }
    return Output;
}

void main()
{
    PS_Input Input;
    Input.Pos = gl_FragCoord;
    Input.Color = _VSPS_Color;
    Input.UV = _VSPS_UV;
    highp vec4 _65 = _main(Input);
    gl_FragData[0] = _65;
}

)";

static const char sprite_unlit_ps_gles3[] = R"(
#version 300 es
precision mediump float;
precision highp int;

struct PS_Input
{
    highp vec4 Pos;
    highp vec4 Color;
    highp vec2 UV;
};

struct PS_ConstanBuffer
{
    highp vec4 flipbookParameter;
    highp vec4 uvDistortionParameter;
    highp vec4 blendTextureParameter;
    highp vec4 emissiveScaling;
    highp vec4 edgeColor;
    highp vec4 edgeParameter;
};

uniform PS_ConstanBuffer CBPS0;

uniform highp sampler2D Sampler_g_sampler;

in highp vec4 _VSPS_Color;
in highp vec2 _VSPS_UV;
layout(location = 0) out highp vec4 _entryPointOutput;

highp vec4 _main(PS_Input Input)
{
    highp vec4 Output = Input.Color * texture(Sampler_g_sampler, Input.UV);
    if (Output.w == 0.0)
    {
        discard;
    }
    return Output;
}

void main()
{
    PS_Input Input;
    Input.Pos = gl_FragCoord;
    Input.Color = _VSPS_Color;
    Input.UV = _VSPS_UV;
    highp vec4 _65 = _main(Input);
    _entryPointOutput = _65;
}

)";


    static const char* get_sprite_unlit_ps (EffekseerRendererGL::OpenGLDeviceType deviceType)
    {
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGL3)
            return sprite_unlit_ps_gl3;
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGL2)
            return sprite_unlit_ps_gl2;
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGLES3)
            return sprite_unlit_ps_gles3;
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGLES2)
            return sprite_unlit_ps_gles2;
        return nullptr;
    }
    