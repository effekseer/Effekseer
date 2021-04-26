#if !defined(__EMSCRIPTEN__)
static const char model_lit_ps_gl2[] = R"(#version 120
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_Input
{
    vec4 PosVS;
    vec4 Color;
    vec2 UV;
    vec3 WorldN;
    vec3 WorldB;
    vec3 WorldT;
    vec4 PosP;
};

struct PS_ConstanBuffer
{
    vec4 fLightDirection;
    vec4 fLightColor;
    vec4 fLightAmbient;
    vec4 fFlipbookParameter;
    vec4 fUVDistortionParameter;
    vec4 fBlendTextureParameter;
    vec4 fCameraFrontDirection;
    vec4 fFalloffParameter;
    vec4 fFalloffBeginColor;
    vec4 fFalloffEndColor;
    vec4 fEmissiveScaling;
    vec4 fEdgeColor;
    vec4 fEdgeParameter;
    vec4 softParticleParam;
    vec4 reconstructionParam1;
    vec4 reconstructionParam2;
    vec4 mUVInversedBack;
};

uniform PS_ConstanBuffer CBPS0;

uniform sampler2D Sampler_sampler_colorTex;
uniform sampler2D Sampler_sampler_normalTex;

varying vec4 _VSPS_Color;
varying vec2 _VSPS_UV;
varying vec3 _VSPS_WorldN;
varying vec3 _VSPS_WorldB;
varying vec3 _VSPS_WorldT;
varying vec4 _VSPS_PosP;

vec4 _main(PS_Input Input)
{
    vec4 Output = texture2D(Sampler_sampler_colorTex, Input.UV) * Input.Color;
    vec3 texNormal = (texture2D(Sampler_sampler_normalTex, Input.UV).xyz - vec3(0.5)) * 2.0;
    vec3 localNormal = normalize(mat3(vec3(Input.WorldT), vec3(Input.WorldB), vec3(Input.WorldN)) * texNormal);
    float diffuse = max(dot(CBPS0.fLightDirection.xyz, localNormal), 0.0);
    vec3 _99 = Output.xyz * ((CBPS0.fLightColor.xyz * diffuse) + CBPS0.fLightAmbient.xyz);
    Output = vec4(_99.x, _99.y, _99.z, Output.w);
    vec3 _110 = Output.xyz * CBPS0.fEmissiveScaling.x;
    Output = vec4(_110.x, _110.y, _110.z, Output.w);
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
    Input.Color = _VSPS_Color;
    Input.UV = _VSPS_UV;
    Input.WorldN = _VSPS_WorldN;
    Input.WorldB = _VSPS_WorldB;
    Input.WorldT = _VSPS_WorldT;
    Input.PosP = _VSPS_PosP;
    vec4 _155 = _main(Input);
    gl_FragData[0] = _155;
}

)";

static const char model_lit_ps_gl3[] = R"(#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_Input
{
    vec4 PosVS;
    vec4 Color;
    vec2 UV;
    vec3 WorldN;
    vec3 WorldB;
    vec3 WorldT;
    vec4 PosP;
};

struct PS_ConstanBuffer
{
    vec4 fLightDirection;
    vec4 fLightColor;
    vec4 fLightAmbient;
    vec4 fFlipbookParameter;
    vec4 fUVDistortionParameter;
    vec4 fBlendTextureParameter;
    vec4 fCameraFrontDirection;
    vec4 fFalloffParameter;
    vec4 fFalloffBeginColor;
    vec4 fFalloffEndColor;
    vec4 fEmissiveScaling;
    vec4 fEdgeColor;
    vec4 fEdgeParameter;
    vec4 softParticleParam;
    vec4 reconstructionParam1;
    vec4 reconstructionParam2;
    vec4 mUVInversedBack;
};

uniform PS_ConstanBuffer CBPS0;

uniform sampler2D Sampler_sampler_colorTex;
uniform sampler2D Sampler_sampler_normalTex;
uniform sampler2D Sampler_sampler_depthTex;

centroid in vec4 _VSPS_Color;
centroid in vec2 _VSPS_UV;
in vec3 _VSPS_WorldN;
in vec3 _VSPS_WorldB;
in vec3 _VSPS_WorldT;
in vec4 _VSPS_PosP;
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
    vec4 Output = texture(Sampler_sampler_colorTex, Input.UV) * Input.Color;
    vec3 texNormal = (texture(Sampler_sampler_normalTex, Input.UV).xyz - vec3(0.5)) * 2.0;
    vec3 localNormal = normalize(mat3(vec3(Input.WorldT), vec3(Input.WorldB), vec3(Input.WorldN)) * texNormal);
    float diffuse = max(dot(CBPS0.fLightDirection.xyz, localNormal), 0.0);
    vec3 _178 = Output.xyz * ((CBPS0.fLightColor.xyz * diffuse) + CBPS0.fLightAmbient.xyz);
    Output = vec4(_178.x, _178.y, _178.z, Output.w);
    vec3 _187 = Output.xyz * CBPS0.fEmissiveScaling.x;
    Output = vec4(_187.x, _187.y, _187.z, Output.w);
    vec4 screenPos = Input.PosP / vec4(Input.PosP.w);
    vec2 screenUV = (screenPos.xy + vec2(1.0)) / vec2(2.0);
    screenUV.y = 1.0 - screenUV.y;
    screenUV.y = 1.0 - screenUV.y;
    screenUV.y = CBPS0.mUVInversedBack.x + (CBPS0.mUVInversedBack.y * screenUV.y);
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
    Input.Color = _VSPS_Color;
    Input.UV = _VSPS_UV;
    Input.WorldN = _VSPS_WorldN;
    Input.WorldB = _VSPS_WorldB;
    Input.WorldT = _VSPS_WorldT;
    Input.PosP = _VSPS_PosP;
    vec4 _296 = _main(Input);
    _entryPointOutput = _296;
}

)";

#endif

static const char model_lit_ps_gles2[] = R"(
precision mediump float;
precision highp int;

struct PS_Input
{
    highp vec4 PosVS;
    highp vec4 Color;
    highp vec2 UV;
    highp vec3 WorldN;
    highp vec3 WorldB;
    highp vec3 WorldT;
    highp vec4 PosP;
};

struct PS_ConstanBuffer
{
    highp vec4 fLightDirection;
    highp vec4 fLightColor;
    highp vec4 fLightAmbient;
    highp vec4 fFlipbookParameter;
    highp vec4 fUVDistortionParameter;
    highp vec4 fBlendTextureParameter;
    highp vec4 fCameraFrontDirection;
    highp vec4 fFalloffParameter;
    highp vec4 fFalloffBeginColor;
    highp vec4 fFalloffEndColor;
    highp vec4 fEmissiveScaling;
    highp vec4 fEdgeColor;
    highp vec4 fEdgeParameter;
    highp vec4 softParticleParam;
    highp vec4 reconstructionParam1;
    highp vec4 reconstructionParam2;
    highp vec4 mUVInversedBack;
};

uniform PS_ConstanBuffer CBPS0;

uniform  sampler2D Sampler_sampler_colorTex;
uniform  sampler2D Sampler_sampler_normalTex;

varying  vec4 _VSPS_Color;
varying  vec2 _VSPS_UV;
varying  vec3 _VSPS_WorldN;
varying  vec3 _VSPS_WorldB;
varying  vec3 _VSPS_WorldT;
varying  vec4 _VSPS_PosP;

highp vec4 _main(PS_Input Input)
{
    highp vec4 Output = texture2D(Sampler_sampler_colorTex, Input.UV) * Input.Color;
    highp vec3 texNormal = (texture2D(Sampler_sampler_normalTex, Input.UV).xyz - vec3(0.5)) * 2.0;
    highp vec3 localNormal = normalize(mat3(vec3(Input.WorldT), vec3(Input.WorldB), vec3(Input.WorldN)) * texNormal);
    highp float diffuse = max(dot(CBPS0.fLightDirection.xyz, localNormal), 0.0);
    highp vec3 _99 = Output.xyz * ((CBPS0.fLightColor.xyz * diffuse) + CBPS0.fLightAmbient.xyz);
    Output = vec4(_99.x, _99.y, _99.z, Output.w);
    highp vec3 _110 = Output.xyz * CBPS0.fEmissiveScaling.x;
    Output = vec4(_110.x, _110.y, _110.z, Output.w);
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
    Input.Color = _VSPS_Color;
    Input.UV = _VSPS_UV;
    Input.WorldN = _VSPS_WorldN;
    Input.WorldB = _VSPS_WorldB;
    Input.WorldT = _VSPS_WorldT;
    Input.PosP = _VSPS_PosP;
    highp vec4 _155 = _main(Input);
    gl_FragData[0] = _155;
}

)";

static const char model_lit_ps_gles3[] = R"(#version 300 es
precision mediump float;
precision highp int;

struct PS_Input
{
    highp vec4 PosVS;
    highp vec4 Color;
    highp vec2 UV;
    highp vec3 WorldN;
    highp vec3 WorldB;
    highp vec3 WorldT;
    highp vec4 PosP;
};

struct PS_ConstanBuffer
{
    highp vec4 fLightDirection;
    highp vec4 fLightColor;
    highp vec4 fLightAmbient;
    highp vec4 fFlipbookParameter;
    highp vec4 fUVDistortionParameter;
    highp vec4 fBlendTextureParameter;
    highp vec4 fCameraFrontDirection;
    highp vec4 fFalloffParameter;
    highp vec4 fFalloffBeginColor;
    highp vec4 fFalloffEndColor;
    highp vec4 fEmissiveScaling;
    highp vec4 fEdgeColor;
    highp vec4 fEdgeParameter;
    highp vec4 softParticleParam;
    highp vec4 reconstructionParam1;
    highp vec4 reconstructionParam2;
    highp vec4 mUVInversedBack;
};

uniform PS_ConstanBuffer CBPS0;

uniform highp sampler2D Sampler_sampler_colorTex;
uniform highp sampler2D Sampler_sampler_normalTex;
uniform highp sampler2D Sampler_sampler_depthTex;

centroid in highp vec4 _VSPS_Color;
centroid in highp vec2 _VSPS_UV;
in highp vec3 _VSPS_WorldN;
in highp vec3 _VSPS_WorldB;
in highp vec3 _VSPS_WorldT;
in highp vec4 _VSPS_PosP;
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
    highp float alphaFar = (depth.y - depth.x) / distanceFar;
    highp float alphaNear = ((-distanceNearOffset) - depth.y) / distanceNear;
    return min(max(min(alphaFar, alphaNear), 0.0), 1.0);
}

highp vec4 _main(PS_Input Input)
{
    highp vec4 Output = texture(Sampler_sampler_colorTex, Input.UV) * Input.Color;
    highp vec3 texNormal = (texture(Sampler_sampler_normalTex, Input.UV).xyz - vec3(0.5)) * 2.0;
    highp vec3 localNormal = normalize(mat3(vec3(Input.WorldT), vec3(Input.WorldB), vec3(Input.WorldN)) * texNormal);
    highp float diffuse = max(dot(CBPS0.fLightDirection.xyz, localNormal), 0.0);
    highp vec3 _178 = Output.xyz * ((CBPS0.fLightColor.xyz * diffuse) + CBPS0.fLightAmbient.xyz);
    Output = vec4(_178.x, _178.y, _178.z, Output.w);
    highp vec3 _187 = Output.xyz * CBPS0.fEmissiveScaling.x;
    Output = vec4(_187.x, _187.y, _187.z, Output.w);
    highp vec4 screenPos = Input.PosP / vec4(Input.PosP.w);
    highp vec2 screenUV = (screenPos.xy + vec2(1.0)) / vec2(2.0);
    screenUV.y = 1.0 - screenUV.y;
    screenUV.y = 1.0 - screenUV.y;
    screenUV.y = CBPS0.mUVInversedBack.x + (CBPS0.mUVInversedBack.y * screenUV.y);
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
    Input.Color = _VSPS_Color;
    Input.UV = _VSPS_UV;
    Input.WorldN = _VSPS_WorldN;
    Input.WorldB = _VSPS_WorldB;
    Input.WorldT = _VSPS_WorldT;
    Input.PosP = _VSPS_PosP;
    highp vec4 _296 = _main(Input);
    _entryPointOutput = _296;
}

)";


    static const char* get_model_lit_ps (EffekseerRendererGL::OpenGLDeviceType deviceType)
    {
    #if !defined(__EMSCRIPTEN__)
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGL3)
            return model_lit_ps_gl3;
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGL2)
            return model_lit_ps_gl2;
    #endif
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGLES3)
            return model_lit_ps_gles3;
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGLES2)
            return model_lit_ps_gles2;
        return nullptr;
    }
    