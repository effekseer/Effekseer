#if !defined(__EMSCRIPTEN__)
static const char model_distortion_ps_gl2[] = R"(#version 120
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_Input
{
    vec4 PosVS;
    vec2 UV;
    vec4 ProjBinormal;
    vec4 ProjTangent;
    vec4 PosP;
    vec4 Color;
};

struct PS_ConstanBuffer
{
    vec4 g_scale;
    vec4 mUVInversedBack;
    vec4 fFlipbookParameter;
    vec4 fUVDistortionParameter;
    vec4 fBlendTextureParameter;
    vec4 softParticleParam;
    vec4 reconstructionParam1;
    vec4 reconstructionParam2;
};

uniform PS_ConstanBuffer CBPS0;

uniform sampler2D Sampler_sampler_colorTex;
uniform sampler2D Sampler_sampler_backTex;

varying vec2 _VSPS_UV;
varying vec4 _VSPS_ProjBinormal;
varying vec4 _VSPS_ProjTangent;
varying vec4 _VSPS_PosP;
varying vec4 _VSPS_Color;

vec4 _main(PS_Input Input)
{
    vec4 Output = texture2D(Sampler_sampler_colorTex, Input.UV);
    Output.w *= Input.Color.w;
    vec2 pos = Input.PosP.xy / vec2(Input.PosP.w);
    vec2 posR = Input.ProjTangent.xy / vec2(Input.ProjTangent.w);
    vec2 posU = Input.ProjBinormal.xy / vec2(Input.ProjBinormal.w);
    float xscale = (((Output.x * 2.0) - 1.0) * Input.Color.x) * CBPS0.g_scale.x;
    float yscale = (((Output.y * 2.0) - 1.0) * Input.Color.y) * CBPS0.g_scale.x;
    vec2 uv = (pos + ((posR - pos) * xscale)) + ((posU - pos) * yscale);
    uv.x = (uv.x + 1.0) * 0.5;
    uv.y = 1.0 - ((uv.y + 1.0) * 0.5);
    uv.y = CBPS0.mUVInversedBack.x + (CBPS0.mUVInversedBack.y * uv.y);
    uv.y = 1.0 - uv.y;
    vec3 color = vec3(texture2D(Sampler_sampler_backTex, uv).xyz);
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
    Input.PosVS = gl_FragCoord;
    Input.UV = _VSPS_UV;
    Input.ProjBinormal = _VSPS_ProjBinormal;
    Input.ProjTangent = _VSPS_ProjTangent;
    Input.PosP = _VSPS_PosP;
    Input.Color = _VSPS_Color;
    vec4 _182 = _main(Input);
    gl_FragData[0] = _182;
}

)";

static const char model_distortion_ps_gl3[] = R"(#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_Input
{
    vec4 PosVS;
    vec2 UV;
    vec4 ProjBinormal;
    vec4 ProjTangent;
    vec4 PosP;
    vec4 Color;
};

struct PS_ConstanBuffer
{
    vec4 g_scale;
    vec4 mUVInversedBack;
    vec4 fFlipbookParameter;
    vec4 fUVDistortionParameter;
    vec4 fBlendTextureParameter;
    vec4 softParticleParam;
    vec4 reconstructionParam1;
    vec4 reconstructionParam2;
};

uniform PS_ConstanBuffer CBPS0;

uniform sampler2D Sampler_sampler_colorTex;
uniform sampler2D Sampler_sampler_backTex;
uniform sampler2D Sampler_sampler_depthTex;

centroid in vec2 _VSPS_UV;
in vec4 _VSPS_ProjBinormal;
in vec4 _VSPS_ProjTangent;
in vec4 _VSPS_PosP;
centroid in vec4 _VSPS_Color;
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
    float dir = sign(depth.x);
    depth *= dir;
    float alphaFar = (depth.x - depth.y) / distanceFar;
    float alphaNear = (depth.y - distanceNearOffset) / distanceNear;
    return min(max(min(alphaFar, alphaNear), 0.0), 1.0);
}

vec4 _main(PS_Input Input)
{
    vec4 Output = texture(Sampler_sampler_colorTex, Input.UV);
    Output.w *= Input.Color.w;
    vec2 pos = Input.PosP.xy / vec2(Input.PosP.w);
    vec2 posR = Input.ProjTangent.xy / vec2(Input.ProjTangent.w);
    vec2 posU = Input.ProjBinormal.xy / vec2(Input.ProjBinormal.w);
    float xscale = (((Output.x * 2.0) - 1.0) * Input.Color.x) * CBPS0.g_scale.x;
    float yscale = (((Output.y * 2.0) - 1.0) * Input.Color.y) * CBPS0.g_scale.x;
    vec2 uv = (pos + ((posR - pos) * xscale)) + ((posU - pos) * yscale);
    uv.x = (uv.x + 1.0) * 0.5;
    uv.y = 1.0 - ((uv.y + 1.0) * 0.5);
    uv.y = CBPS0.mUVInversedBack.x + (CBPS0.mUVInversedBack.y * uv.y);
    uv.y = 1.0 - uv.y;
    vec3 color = vec3(texture(Sampler_sampler_backTex, uv).xyz);
    Output = vec4(color.x, color.y, color.z, Output.w);
    vec4 screenPos = Input.PosP / vec4(Input.PosP.w);
    vec2 screenUV = (screenPos.xy + vec2(1.0)) / vec2(2.0);
    screenUV.y = 1.0 - screenUV.y;
    screenUV.y = 1.0 - screenUV.y;
    if (!(CBPS0.softParticleParam.w == 0.0))
    {
        float backgroundZ = texture(Sampler_sampler_depthTex, screenUV).x;
        float param = backgroundZ;
        float param_1 = screenPos.z;
        vec4 param_2 = CBPS0.softParticleParam;
        vec4 param_3 = CBPS0.reconstructionParam1;
        vec4 param_4 = CBPS0.reconstructionParam2;
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
    Input.UV = _VSPS_UV;
    Input.ProjBinormal = _VSPS_ProjBinormal;
    Input.ProjTangent = _VSPS_ProjTangent;
    Input.PosP = _VSPS_PosP;
    Input.Color = _VSPS_Color;
    vec4 _318 = _main(Input);
    _entryPointOutput = _318;
}

)";

#endif

static const char model_distortion_ps_gles2[] = R"(
precision mediump float;
precision highp int;

struct PS_Input
{
    highp vec4 PosVS;
    highp vec2 UV;
    highp vec4 ProjBinormal;
    highp vec4 ProjTangent;
    highp vec4 PosP;
    highp vec4 Color;
};

struct PS_ConstanBuffer
{
    highp vec4 g_scale;
    highp vec4 mUVInversedBack;
    highp vec4 fFlipbookParameter;
    highp vec4 fUVDistortionParameter;
    highp vec4 fBlendTextureParameter;
    highp vec4 softParticleParam;
    highp vec4 reconstructionParam1;
    highp vec4 reconstructionParam2;
};

uniform PS_ConstanBuffer CBPS0;

uniform  sampler2D Sampler_sampler_colorTex;
uniform  sampler2D Sampler_sampler_backTex;

varying  vec2 _VSPS_UV;
varying  vec4 _VSPS_ProjBinormal;
varying  vec4 _VSPS_ProjTangent;
varying  vec4 _VSPS_PosP;
varying  vec4 _VSPS_Color;

highp vec4 _main(PS_Input Input)
{
    highp vec4 Output = texture2D(Sampler_sampler_colorTex, Input.UV);
    Output.w *= Input.Color.w;
    highp vec2 pos = Input.PosP.xy / vec2(Input.PosP.w);
    highp vec2 posR = Input.ProjTangent.xy / vec2(Input.ProjTangent.w);
    highp vec2 posU = Input.ProjBinormal.xy / vec2(Input.ProjBinormal.w);
    highp float xscale = (((Output.x * 2.0) - 1.0) * Input.Color.x) * CBPS0.g_scale.x;
    highp float yscale = (((Output.y * 2.0) - 1.0) * Input.Color.y) * CBPS0.g_scale.x;
    highp vec2 uv = (pos + ((posR - pos) * xscale)) + ((posU - pos) * yscale);
    uv.x = (uv.x + 1.0) * 0.5;
    uv.y = 1.0 - ((uv.y + 1.0) * 0.5);
    uv.y = CBPS0.mUVInversedBack.x + (CBPS0.mUVInversedBack.y * uv.y);
    uv.y = 1.0 - uv.y;
    highp vec3 color = vec3(texture2D(Sampler_sampler_backTex, uv).xyz);
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
    Input.PosVS = gl_FragCoord;
    Input.UV = _VSPS_UV;
    Input.ProjBinormal = _VSPS_ProjBinormal;
    Input.ProjTangent = _VSPS_ProjTangent;
    Input.PosP = _VSPS_PosP;
    Input.Color = _VSPS_Color;
    highp vec4 _182 = _main(Input);
    gl_FragData[0] = _182;
}

)";

static const char model_distortion_ps_gles3[] = R"(#version 300 es
precision mediump float;
precision highp int;

struct PS_Input
{
    highp vec4 PosVS;
    highp vec2 UV;
    highp vec4 ProjBinormal;
    highp vec4 ProjTangent;
    highp vec4 PosP;
    highp vec4 Color;
};

struct PS_ConstanBuffer
{
    highp vec4 g_scale;
    highp vec4 mUVInversedBack;
    highp vec4 fFlipbookParameter;
    highp vec4 fUVDistortionParameter;
    highp vec4 fBlendTextureParameter;
    highp vec4 softParticleParam;
    highp vec4 reconstructionParam1;
    highp vec4 reconstructionParam2;
};

uniform PS_ConstanBuffer CBPS0;

uniform highp sampler2D Sampler_sampler_colorTex;
uniform highp sampler2D Sampler_sampler_backTex;
uniform highp sampler2D Sampler_sampler_depthTex;

centroid in highp vec2 _VSPS_UV;
in highp vec4 _VSPS_ProjBinormal;
in highp vec4 _VSPS_ProjTangent;
in highp vec4 _VSPS_PosP;
centroid in highp vec4 _VSPS_Color;
layout(location = 0) out highp vec4 _entryPointOutput;

highp float SoftParticle(highp float backgroundZ, highp float meshZ, highp vec4 softparticleParam, highp vec4 reconstruct1, highp vec4 reconstruct2)
{
    highp float distanceFar = softparticleParam.x;
    highp float distanceNear = softparticleParam.y;
    highp float distanceNearOffset = softparticleParam.z;
    highp vec2 rescale = reconstruct1.xy;
    highp vec4 params = reconstruct2;
    highp vec2 zs = vec2((backgroundZ * rescale.x) + rescale.y, meshZ);
    highp vec2 depth = ((zs * params.w) - vec2(params.y)) / (vec2(params.x) - (zs * params.z));
    highp float dir = sign(depth.x);
    depth *= dir;
    highp float alphaFar = (depth.x - depth.y) / distanceFar;
    highp float alphaNear = (depth.y - distanceNearOffset) / distanceNear;
    return min(max(min(alphaFar, alphaNear), 0.0), 1.0);
}

highp vec4 _main(PS_Input Input)
{
    highp vec4 Output = texture(Sampler_sampler_colorTex, Input.UV);
    Output.w *= Input.Color.w;
    highp vec2 pos = Input.PosP.xy / vec2(Input.PosP.w);
    highp vec2 posR = Input.ProjTangent.xy / vec2(Input.ProjTangent.w);
    highp vec2 posU = Input.ProjBinormal.xy / vec2(Input.ProjBinormal.w);
    highp float xscale = (((Output.x * 2.0) - 1.0) * Input.Color.x) * CBPS0.g_scale.x;
    highp float yscale = (((Output.y * 2.0) - 1.0) * Input.Color.y) * CBPS0.g_scale.x;
    highp vec2 uv = (pos + ((posR - pos) * xscale)) + ((posU - pos) * yscale);
    uv.x = (uv.x + 1.0) * 0.5;
    uv.y = 1.0 - ((uv.y + 1.0) * 0.5);
    uv.y = CBPS0.mUVInversedBack.x + (CBPS0.mUVInversedBack.y * uv.y);
    uv.y = 1.0 - uv.y;
    highp vec3 color = vec3(texture(Sampler_sampler_backTex, uv).xyz);
    Output = vec4(color.x, color.y, color.z, Output.w);
    highp vec4 screenPos = Input.PosP / vec4(Input.PosP.w);
    highp vec2 screenUV = (screenPos.xy + vec2(1.0)) / vec2(2.0);
    screenUV.y = 1.0 - screenUV.y;
    screenUV.y = 1.0 - screenUV.y;
    if (!(CBPS0.softParticleParam.w == 0.0))
    {
        highp float backgroundZ = texture(Sampler_sampler_depthTex, screenUV).x;
        highp float param = backgroundZ;
        highp float param_1 = screenPos.z;
        highp vec4 param_2 = CBPS0.softParticleParam;
        highp vec4 param_3 = CBPS0.reconstructionParam1;
        highp vec4 param_4 = CBPS0.reconstructionParam2;
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
    Input.UV = _VSPS_UV;
    Input.ProjBinormal = _VSPS_ProjBinormal;
    Input.ProjTangent = _VSPS_ProjTangent;
    Input.PosP = _VSPS_PosP;
    Input.Color = _VSPS_Color;
    highp vec4 _318 = _main(Input);
    _entryPointOutput = _318;
}

)";


    static const char* get_model_distortion_ps (EffekseerRendererGL::OpenGLDeviceType deviceType)
    {
    #if !defined(__EMSCRIPTEN__)
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGL3)
            return model_distortion_ps_gl3;
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGL2)
            return model_distortion_ps_gl2;
    #endif
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGLES3)
            return model_distortion_ps_gles3;
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGLES2)
            return model_distortion_ps_gles2;
        return nullptr;
    }
    