static const char model_renderer_texture_PS_gl2[] = R"(
#version 120
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_Input
{
    vec4 Pos;
    vec2 UV;
    vec4 Color;
};

struct VS_ConstantBuffer
{
    vec4 fLightDirection;
    vec4 fLightColor;
    vec4 fLightAmbient;
};

uniform VS_ConstantBuffer CBPS0;

uniform sampler2D Sampler_g_colorSampler;

varying vec2 _VSPS_UV;
varying vec4 _VSPS_Color;

vec4 _main(PS_Input Input)
{
    vec4 Output = texture2D(Sampler_g_colorSampler, Input.UV) * Input.Color;
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
    Input.UV = _VSPS_UV;
    Input.Color = _VSPS_Color;
    vec4 _65 = _main(Input);
    gl_FragData[0] = _65;
}

)";

static const char model_renderer_texture_PS_gl3[] = R"(
#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_Input
{
    vec4 Pos;
    vec2 UV;
    vec4 Color;
};

struct VS_ConstantBuffer
{
    vec4 fLightDirection;
    vec4 fLightColor;
    vec4 fLightAmbient;
};

uniform VS_ConstantBuffer CBPS0;

layout(binding = 0) uniform sampler2D Sampler_g_colorSampler;

in vec2 _VSPS_UV;
in vec4 _VSPS_Color;
layout(location = 0) out vec4 _entryPointOutput;

vec4 _main(PS_Input Input)
{
    vec4 Output = texture(Sampler_g_colorSampler, Input.UV) * Input.Color;
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
    Input.UV = _VSPS_UV;
    Input.Color = _VSPS_Color;
    vec4 _65 = _main(Input);
    _entryPointOutput = _65;
}

)";

static const char model_renderer_texture_PS_gles2[] = R"(
#version 200 es
precision mediump float;
precision highp int;

struct PS_Input
{
    highp vec4 Pos;
    highp vec2 UV;
    highp vec4 Color;
};

struct VS_ConstantBuffer
{
    highp vec4 fLightDirection;
    highp vec4 fLightColor;
    highp vec4 fLightAmbient;
};

uniform VS_ConstantBuffer CBPS0;

uniform highp sampler2D Sampler_g_colorSampler;

varying highp vec2 _VSPS_UV;
varying highp vec4 _VSPS_Color;

highp vec4 _main(PS_Input Input)
{
    highp vec4 Output = texture2D(Sampler_g_colorSampler, Input.UV) * Input.Color;
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
    Input.UV = _VSPS_UV;
    Input.Color = _VSPS_Color;
    highp vec4 _65 = _main(Input);
    gl_FragData[0] = _65;
}

)";

static const char model_renderer_texture_PS_gles3[] = R"(
#version 300 es
precision mediump float;
precision highp int;

struct PS_Input
{
    highp vec4 Pos;
    highp vec2 UV;
    highp vec4 Color;
};

struct VS_ConstantBuffer
{
    highp vec4 fLightDirection;
    highp vec4 fLightColor;
    highp vec4 fLightAmbient;
};

uniform VS_ConstantBuffer CBPS0;

uniform highp sampler2D Sampler_g_colorSampler;

in highp vec2 _VSPS_UV;
in highp vec4 _VSPS_Color;
layout(location = 0) out highp vec4 _entryPointOutput;

highp vec4 _main(PS_Input Input)
{
    highp vec4 Output = texture(Sampler_g_colorSampler, Input.UV) * Input.Color;
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
    Input.UV = _VSPS_UV;
    Input.Color = _VSPS_Color;
    highp vec4 _65 = _main(Input);
    _entryPointOutput = _65;
}

)";


    static const char* get_model_renderer_texture_PS (EffekseerRendererGL::OpenGLDeviceType deviceType)
    {
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGL3)
            return model_renderer_texture_PS_gl2;
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGL2)
            return model_renderer_texture_PS_gl3;
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGLES3)
            return model_renderer_texture_PS_gles2;
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGLES2 || deviceType == EffekseerRendererGL::OpenGLDeviceType::Emscripten)
            return model_renderer_texture_PS_gles3;
        return nullptr;
    }
    