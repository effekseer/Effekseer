static const char sprite_lit_ps_gl2[] = R"(
#version 120
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_Input
{
    vec4 Position;
    vec4 VColor;
    vec2 UV1;
    vec2 UV2;
    vec3 WorldP;
    vec3 WorldN;
    vec3 WorldT;
    vec3 WorldB;
    vec2 ScreenUV;
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
};

uniform VS_ConstantBuffer CBPS0;

uniform sampler2D Sampler_g_normalSampler;
uniform sampler2D Sampler_g_colorSampler;

centroid varying vec4 _VSPS_VColor;
centroid varying vec2 _VSPS_UV1;
centroid varying vec2 _VSPS_UV2;
varying vec3 _VSPS_WorldP;
varying vec3 _VSPS_WorldN;
varying vec3 _VSPS_WorldT;
varying vec3 _VSPS_WorldB;
varying vec2 _VSPS_ScreenUV;

vec4 _main(PS_Input Input)
{
    vec3 loN = texture2D(Sampler_g_normalSampler, Input.UV1).xyz;
    vec3 texNormal = (loN - vec3(0.5)) * 2.0;
    vec3 localNormal = normalize(mat3(vec3(Input.WorldT), vec3(Input.WorldB), vec3(Input.WorldN)) * texNormal);
    float diffuse = max(dot(CBPS0.fLightDirection.xyz, localNormal), 0.0);
    vec4 Output = texture2D(Sampler_g_colorSampler, Input.UV1) * Input.VColor;
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
    Input.Position = gl_FragCoord;
    Input.VColor = _VSPS_VColor;
    Input.UV1 = _VSPS_UV1;
    Input.UV2 = _VSPS_UV2;
    Input.WorldP = _VSPS_WorldP;
    Input.WorldN = _VSPS_WorldN;
    Input.WorldT = _VSPS_WorldT;
    Input.WorldB = _VSPS_WorldB;
    Input.ScreenUV = _VSPS_ScreenUV;
    vec4 _158 = _main(Input);
    gl_FragData[0] = _158;
}

)";

static const char sprite_lit_ps_gl3[] = R"(
#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_Input
{
    vec4 Position;
    vec4 VColor;
    vec2 UV1;
    vec2 UV2;
    vec3 WorldP;
    vec3 WorldN;
    vec3 WorldT;
    vec3 WorldB;
    vec2 ScreenUV;
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
};

uniform VS_ConstantBuffer CBPS0;

uniform sampler2D Sampler_g_normalSampler;
uniform sampler2D Sampler_g_colorSampler;

centroid in vec4 _VSPS_VColor;
centroid in vec2 _VSPS_UV1;
centroid in vec2 _VSPS_UV2;
in vec3 _VSPS_WorldP;
in vec3 _VSPS_WorldN;
in vec3 _VSPS_WorldT;
in vec3 _VSPS_WorldB;
in vec2 _VSPS_ScreenUV;
layout(location = 0) out vec4 _entryPointOutput;

vec4 _main(PS_Input Input)
{
    vec3 loN = texture(Sampler_g_normalSampler, Input.UV1).xyz;
    vec3 texNormal = (loN - vec3(0.5)) * 2.0;
    vec3 localNormal = normalize(mat3(vec3(Input.WorldT), vec3(Input.WorldB), vec3(Input.WorldN)) * texNormal);
    float diffuse = max(dot(CBPS0.fLightDirection.xyz, localNormal), 0.0);
    vec4 Output = texture(Sampler_g_colorSampler, Input.UV1) * Input.VColor;
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
    Input.Position = gl_FragCoord;
    Input.VColor = _VSPS_VColor;
    Input.UV1 = _VSPS_UV1;
    Input.UV2 = _VSPS_UV2;
    Input.WorldP = _VSPS_WorldP;
    Input.WorldN = _VSPS_WorldN;
    Input.WorldT = _VSPS_WorldT;
    Input.WorldB = _VSPS_WorldB;
    Input.ScreenUV = _VSPS_ScreenUV;
    vec4 _158 = _main(Input);
    _entryPointOutput = _158;
}

)";

static const char sprite_lit_ps_gles2[] = R"(

precision mediump float;
precision highp int;

struct PS_Input
{
    highp vec4 Position;
    highp vec4 VColor;
    highp vec2 UV1;
    highp vec2 UV2;
    highp vec3 WorldP;
    highp vec3 WorldN;
    highp vec3 WorldT;
    highp vec3 WorldB;
    highp vec2 ScreenUV;
};

struct VS_ConstantBuffer
{
    highp vec4 fLightDirection;
    highp vec4 fLightColor;
    highp vec4 fLightAmbient;
    highp vec4 fFlipbookParameter;
    highp vec4 fUVDistortionParameter;
    highp vec4 fBlendTextureParameter;
    highp vec4 fEmissiveScaling;
    highp vec4 fEdgeColor;
    highp vec4 fEdgeParameter;
};

uniform VS_ConstantBuffer CBPS0;

uniform  sampler2D Sampler_g_normalSampler;
uniform  sampler2D Sampler_g_colorSampler;

centroid varying  vec4 _VSPS_VColor;
centroid varying  vec2 _VSPS_UV1;
centroid varying  vec2 _VSPS_UV2;
varying  vec3 _VSPS_WorldP;
varying  vec3 _VSPS_WorldN;
varying  vec3 _VSPS_WorldT;
varying  vec3 _VSPS_WorldB;
varying  vec2 _VSPS_ScreenUV;

highp vec4 _main(PS_Input Input)
{
    highp vec3 loN = texture2D(Sampler_g_normalSampler, Input.UV1).xyz;
    highp vec3 texNormal = (loN - vec3(0.5)) * 2.0;
    highp vec3 localNormal = normalize(mat3(vec3(Input.WorldT), vec3(Input.WorldB), vec3(Input.WorldN)) * texNormal);
    highp float diffuse = max(dot(CBPS0.fLightDirection.xyz, localNormal), 0.0);
    highp vec4 Output = texture2D(Sampler_g_colorSampler, Input.UV1) * Input.VColor;
    highp vec3 _104 = Output.xyz * ((CBPS0.fLightColor.xyz * diffuse) + vec3(CBPS0.fLightAmbient.xyz));
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
    Input.Position = gl_FragCoord;
    Input.VColor = _VSPS_VColor;
    Input.UV1 = _VSPS_UV1;
    Input.UV2 = _VSPS_UV2;
    Input.WorldP = _VSPS_WorldP;
    Input.WorldN = _VSPS_WorldN;
    Input.WorldT = _VSPS_WorldT;
    Input.WorldB = _VSPS_WorldB;
    Input.ScreenUV = _VSPS_ScreenUV;
    highp vec4 _158 = _main(Input);
    gl_FragData[0] = _158;
}

)";

static const char sprite_lit_ps_gles3[] = R"(
#version 300 es
precision mediump float;
precision highp int;

struct PS_Input
{
    highp vec4 Position;
    highp vec4 VColor;
    highp vec2 UV1;
    highp vec2 UV2;
    highp vec3 WorldP;
    highp vec3 WorldN;
    highp vec3 WorldT;
    highp vec3 WorldB;
    highp vec2 ScreenUV;
};

struct VS_ConstantBuffer
{
    highp vec4 fLightDirection;
    highp vec4 fLightColor;
    highp vec4 fLightAmbient;
    highp vec4 fFlipbookParameter;
    highp vec4 fUVDistortionParameter;
    highp vec4 fBlendTextureParameter;
    highp vec4 fEmissiveScaling;
    highp vec4 fEdgeColor;
    highp vec4 fEdgeParameter;
};

uniform VS_ConstantBuffer CBPS0;

uniform highp sampler2D Sampler_g_normalSampler;
uniform highp sampler2D Sampler_g_colorSampler;

centroid in highp vec4 _VSPS_VColor;
centroid in highp vec2 _VSPS_UV1;
centroid in highp vec2 _VSPS_UV2;
in highp vec3 _VSPS_WorldP;
in highp vec3 _VSPS_WorldN;
in highp vec3 _VSPS_WorldT;
in highp vec3 _VSPS_WorldB;
in highp vec2 _VSPS_ScreenUV;
layout(location = 0) out highp vec4 _entryPointOutput;

highp vec4 _main(PS_Input Input)
{
    highp vec3 loN = texture(Sampler_g_normalSampler, Input.UV1).xyz;
    highp vec3 texNormal = (loN - vec3(0.5)) * 2.0;
    highp vec3 localNormal = normalize(mat3(vec3(Input.WorldT), vec3(Input.WorldB), vec3(Input.WorldN)) * texNormal);
    highp float diffuse = max(dot(CBPS0.fLightDirection.xyz, localNormal), 0.0);
    highp vec4 Output = texture(Sampler_g_colorSampler, Input.UV1) * Input.VColor;
    highp vec3 _104 = Output.xyz * ((CBPS0.fLightColor.xyz * diffuse) + vec3(CBPS0.fLightAmbient.xyz));
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
    Input.Position = gl_FragCoord;
    Input.VColor = _VSPS_VColor;
    Input.UV1 = _VSPS_UV1;
    Input.UV2 = _VSPS_UV2;
    Input.WorldP = _VSPS_WorldP;
    Input.WorldN = _VSPS_WorldN;
    Input.WorldT = _VSPS_WorldT;
    Input.WorldB = _VSPS_WorldB;
    Input.ScreenUV = _VSPS_ScreenUV;
    highp vec4 _158 = _main(Input);
    _entryPointOutput = _158;
}

)";


    static const char* get_sprite_lit_ps (EffekseerRendererGL::OpenGLDeviceType deviceType)
    {
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGL3)
            return sprite_lit_ps_gl3;
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGL2)
            return sprite_lit_ps_gl2;
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGLES3)
            return sprite_lit_ps_gles3;
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGLES2)
            return sprite_lit_ps_gles2;
        return nullptr;
    }
    