static const char model_distortion_vs_gl2[] = R"(
#version 120
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_Input
{
    vec3 Pos;
    vec3 Normal;
    vec3 Binormal;
    vec3 Tangent;
    vec2 UV;
    vec4 Color;
};

struct VS_Output
{
    vec4 PosVS;
    vec2 UV;
    vec4 Normal;
    vec4 Binormal;
    vec4 Tangent;
    vec4 PosP;
    vec4 Color;
};

struct VS_ConstantBuffer
{
    mat4 mCameraProj;
    mat4 mModel;
    vec4 fUV;
    vec4 fModelColor;
    vec4 fLightDirection;
    vec4 fLightColor;
    vec4 fLightAmbient;
    vec4 mUVInversed;
};

uniform VS_ConstantBuffer CBVS0;

attribute vec3 Input_Pos;
attribute vec3 Input_Normal;
attribute vec3 Input_Binormal;
attribute vec3 Input_Tangent;
attribute vec2 Input_UV;
attribute vec4 Input_Color;
centroid varying vec2 _VSPS_UV;
varying vec4 _VSPS_Normal;
varying vec4 _VSPS_Binormal;
varying vec4 _VSPS_Tangent;
varying vec4 _VSPS_PosP;
centroid varying vec4 _VSPS_Color;

VS_Output _main(VS_Input Input)
{
    vec4 uv = CBVS0.fUV;
    vec4 modelColor = CBVS0.fModelColor * Input.Color;
    VS_Output Output = VS_Output(vec4(0.0), vec2(0.0), vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0));
    vec4 localPosition = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    vec4 localNormal = vec4(Input.Pos.x + Input.Normal.x, Input.Pos.y + Input.Normal.y, Input.Pos.z + Input.Normal.z, 1.0);
    vec4 localBinormal = vec4(Input.Pos.x + Input.Binormal.x, Input.Pos.y + Input.Binormal.y, Input.Pos.z + Input.Binormal.z, 1.0);
    vec4 localTangent = vec4(Input.Pos.x + Input.Tangent.x, Input.Pos.y + Input.Tangent.y, Input.Pos.z + Input.Tangent.z, 1.0);
    localPosition = CBVS0.mModel * localPosition;
    localNormal = CBVS0.mModel * localNormal;
    localBinormal = CBVS0.mModel * localBinormal;
    localTangent = CBVS0.mModel * localTangent;
    localNormal = localPosition + normalize(localNormal - localPosition);
    localBinormal = localPosition + normalize(localBinormal - localPosition);
    localTangent = localPosition + normalize(localTangent - localPosition);
    Output.PosVS = CBVS0.mCameraProj * localPosition;
    Output.UV.x = (Input.UV.x * uv.z) + uv.x;
    Output.UV.y = (Input.UV.y * uv.w) + uv.y;
    Output.Normal = CBVS0.mCameraProj * localNormal;
    Output.Binormal = CBVS0.mCameraProj * localBinormal;
    Output.Tangent = CBVS0.mCameraProj * localTangent;
    Output.PosP = Output.PosVS;
    Output.Color = modelColor;
    Output.UV.y = CBVS0.mUVInversed.x + (CBVS0.mUVInversed.y * Output.UV.y);
    return Output;
}

void main()
{
    VS_Input Input;
    Input.Pos = Input_Pos;
    Input.Normal = Input_Normal;
    Input.Binormal = Input_Binormal;
    Input.Tangent = Input_Tangent;
    Input.UV = Input_UV;
    Input.Color = Input_Color;
    VS_Output flattenTemp = _main(Input);
    gl_Position = flattenTemp.PosVS;
    _VSPS_UV = flattenTemp.UV;
    _VSPS_Normal = flattenTemp.Normal;
    _VSPS_Binormal = flattenTemp.Binormal;
    _VSPS_Tangent = flattenTemp.Tangent;
    _VSPS_PosP = flattenTemp.PosP;
    _VSPS_Color = flattenTemp.Color;
}

)";

static const char model_distortion_vs_gl3[] = R"(
#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif
#ifdef GL_ARB_shader_draw_parameters
#extension GL_ARB_shader_draw_parameters : enable
#endif

struct VS_Input
{
    vec3 Pos;
    vec3 Normal;
    vec3 Binormal;
    vec3 Tangent;
    vec2 UV;
    vec4 Color;
    uint Index;
};

struct VS_Output
{
    vec4 PosVS;
    vec2 UV;
    vec4 Normal;
    vec4 Binormal;
    vec4 Tangent;
    vec4 PosP;
    vec4 Color;
};

struct VS_ConstantBuffer
{
    mat4 mCameraProj;
    mat4 mModel[10];
    vec4 fUV[10];
    vec4 fModelColor[10];
    vec4 fLightDirection;
    vec4 fLightColor;
    vec4 fLightAmbient;
    vec4 mUVInversed;
};

uniform VS_ConstantBuffer CBVS0;

layout(location = 0) in vec3 Input_Pos;
layout(location = 1) in vec3 Input_Normal;
layout(location = 2) in vec3 Input_Binormal;
layout(location = 3) in vec3 Input_Tangent;
layout(location = 4) in vec2 Input_UV;
layout(location = 5) in vec4 Input_Color;
#ifdef GL_ARB_shader_draw_parameters
#define SPIRV_Cross_BaseInstance gl_BaseInstanceARB
#else
uniform int SPIRV_Cross_BaseInstance;
#endif
centroid out vec2 _VSPS_UV;
out vec4 _VSPS_Normal;
out vec4 _VSPS_Binormal;
out vec4 _VSPS_Tangent;
out vec4 _VSPS_PosP;
centroid out vec4 _VSPS_Color;

VS_Output _main(VS_Input Input)
{
    uint index = Input.Index;
    mat4 matModel = CBVS0.mModel[index];
    vec4 uv = CBVS0.fUV[index];
    vec4 modelColor = CBVS0.fModelColor[index] * Input.Color;
    VS_Output Output = VS_Output(vec4(0.0), vec2(0.0), vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0));
    vec4 localPosition = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    vec4 localNormal = vec4(Input.Pos.x + Input.Normal.x, Input.Pos.y + Input.Normal.y, Input.Pos.z + Input.Normal.z, 1.0);
    vec4 localBinormal = vec4(Input.Pos.x + Input.Binormal.x, Input.Pos.y + Input.Binormal.y, Input.Pos.z + Input.Binormal.z, 1.0);
    vec4 localTangent = vec4(Input.Pos.x + Input.Tangent.x, Input.Pos.y + Input.Tangent.y, Input.Pos.z + Input.Tangent.z, 1.0);
    localPosition *= matModel;
    localNormal *= matModel;
    localBinormal *= matModel;
    localTangent *= matModel;
    localNormal = localPosition + normalize(localNormal - localPosition);
    localBinormal = localPosition + normalize(localBinormal - localPosition);
    localTangent = localPosition + normalize(localTangent - localPosition);
    Output.PosVS = localPosition * CBVS0.mCameraProj;
    Output.UV.x = (Input.UV.x * uv.z) + uv.x;
    Output.UV.y = (Input.UV.y * uv.w) + uv.y;
    Output.Normal = localNormal * CBVS0.mCameraProj;
    Output.Binormal = localBinormal * CBVS0.mCameraProj;
    Output.Tangent = localTangent * CBVS0.mCameraProj;
    Output.PosP = Output.PosVS;
    Output.Color = modelColor;
    Output.UV.y = CBVS0.mUVInversed.x + (CBVS0.mUVInversed.y * Output.UV.y);
    return Output;
}

void main()
{
    VS_Input Input;
    Input.Pos = Input_Pos;
    Input.Normal = Input_Normal;
    Input.Binormal = Input_Binormal;
    Input.Tangent = Input_Tangent;
    Input.UV = Input_UV;
    Input.Color = Input_Color;
    Input.Index = uint((gl_InstanceID + SPIRV_Cross_BaseInstance));
    VS_Output flattenTemp = _main(Input);
    gl_Position = flattenTemp.PosVS;
    _VSPS_UV = flattenTemp.UV;
    _VSPS_Normal = flattenTemp.Normal;
    _VSPS_Binormal = flattenTemp.Binormal;
    _VSPS_Tangent = flattenTemp.Tangent;
    _VSPS_PosP = flattenTemp.PosP;
    _VSPS_Color = flattenTemp.Color;
}

)";

static const char model_distortion_vs_gles2[] = R"(


struct VS_Input
{
    vec3 Pos;
    vec3 Normal;
    vec3 Binormal;
    vec3 Tangent;
    vec2 UV;
    vec4 Color;
};

struct VS_Output
{
    vec4 PosVS;
    vec2 UV;
    vec4 Normal;
    vec4 Binormal;
    vec4 Tangent;
    vec4 PosP;
    vec4 Color;
};

struct VS_ConstantBuffer
{
    mat4 mCameraProj;
    mat4 mModel;
    vec4 fUV;
    vec4 fModelColor;
    vec4 fLightDirection;
    vec4 fLightColor;
    vec4 fLightAmbient;
    vec4 mUVInversed;
};

uniform VS_ConstantBuffer CBVS0;

attribute vec3 Input_Pos;
attribute vec3 Input_Normal;
attribute vec3 Input_Binormal;
attribute vec3 Input_Tangent;
attribute vec2 Input_UV;
attribute vec4 Input_Color;
varying vec2 _VSPS_UV;
varying vec4 _VSPS_Normal;
varying vec4 _VSPS_Binormal;
varying vec4 _VSPS_Tangent;
varying vec4 _VSPS_PosP;
varying vec4 _VSPS_Color;

VS_Output _main(VS_Input Input)
{
    vec4 uv = CBVS0.fUV;
    vec4 modelColor = CBVS0.fModelColor * Input.Color;
    VS_Output Output = VS_Output(vec4(0.0), vec2(0.0), vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0));
    vec4 localPosition = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    vec4 localNormal = vec4(Input.Pos.x + Input.Normal.x, Input.Pos.y + Input.Normal.y, Input.Pos.z + Input.Normal.z, 1.0);
    vec4 localBinormal = vec4(Input.Pos.x + Input.Binormal.x, Input.Pos.y + Input.Binormal.y, Input.Pos.z + Input.Binormal.z, 1.0);
    vec4 localTangent = vec4(Input.Pos.x + Input.Tangent.x, Input.Pos.y + Input.Tangent.y, Input.Pos.z + Input.Tangent.z, 1.0);
    localPosition = CBVS0.mModel * localPosition;
    localNormal = CBVS0.mModel * localNormal;
    localBinormal = CBVS0.mModel * localBinormal;
    localTangent = CBVS0.mModel * localTangent;
    localNormal = localPosition + normalize(localNormal - localPosition);
    localBinormal = localPosition + normalize(localBinormal - localPosition);
    localTangent = localPosition + normalize(localTangent - localPosition);
    Output.PosVS = CBVS0.mCameraProj * localPosition;
    Output.UV.x = (Input.UV.x * uv.z) + uv.x;
    Output.UV.y = (Input.UV.y * uv.w) + uv.y;
    Output.Normal = CBVS0.mCameraProj * localNormal;
    Output.Binormal = CBVS0.mCameraProj * localBinormal;
    Output.Tangent = CBVS0.mCameraProj * localTangent;
    Output.PosP = Output.PosVS;
    Output.Color = modelColor;
    Output.UV.y = CBVS0.mUVInversed.x + (CBVS0.mUVInversed.y * Output.UV.y);
    return Output;
}

void main()
{
    VS_Input Input;
    Input.Pos = Input_Pos;
    Input.Normal = Input_Normal;
    Input.Binormal = Input_Binormal;
    Input.Tangent = Input_Tangent;
    Input.UV = Input_UV;
    Input.Color = Input_Color;
    VS_Output flattenTemp = _main(Input);
    gl_Position = flattenTemp.PosVS;
    _VSPS_UV = flattenTemp.UV;
    _VSPS_Normal = flattenTemp.Normal;
    _VSPS_Binormal = flattenTemp.Binormal;
    _VSPS_Tangent = flattenTemp.Tangent;
    _VSPS_PosP = flattenTemp.PosP;
    _VSPS_Color = flattenTemp.Color;
}

)";

static const char model_distortion_vs_gles3[] = R"(
#version 300 es
#ifdef GL_ARB_shader_draw_parameters
#extension GL_ARB_shader_draw_parameters : enable
#endif

struct VS_Input
{
    vec3 Pos;
    vec3 Normal;
    vec3 Binormal;
    vec3 Tangent;
    vec2 UV;
    vec4 Color;
    uint Index;
};

struct VS_Output
{
    vec4 PosVS;
    vec2 UV;
    vec4 Normal;
    vec4 Binormal;
    vec4 Tangent;
    vec4 PosP;
    vec4 Color;
};

struct VS_ConstantBuffer
{
    mat4 mCameraProj;
    mat4 mModel[10];
    vec4 fUV[10];
    vec4 fModelColor[10];
    vec4 fLightDirection;
    vec4 fLightColor;
    vec4 fLightAmbient;
    vec4 mUVInversed;
};

uniform VS_ConstantBuffer CBVS0;

layout(location = 0) in vec3 Input_Pos;
layout(location = 1) in vec3 Input_Normal;
layout(location = 2) in vec3 Input_Binormal;
layout(location = 3) in vec3 Input_Tangent;
layout(location = 4) in vec2 Input_UV;
layout(location = 5) in vec4 Input_Color;
#ifdef GL_ARB_shader_draw_parameters
#define SPIRV_Cross_BaseInstance gl_BaseInstanceARB
#else
uniform int SPIRV_Cross_BaseInstance;
#endif
centroid out vec2 _VSPS_UV;
out vec4 _VSPS_Normal;
out vec4 _VSPS_Binormal;
out vec4 _VSPS_Tangent;
out vec4 _VSPS_PosP;
centroid out vec4 _VSPS_Color;

VS_Output _main(VS_Input Input)
{
    uint index = Input.Index;
    mat4 matModel = CBVS0.mModel[index];
    vec4 uv = CBVS0.fUV[index];
    vec4 modelColor = CBVS0.fModelColor[index] * Input.Color;
    VS_Output Output = VS_Output(vec4(0.0), vec2(0.0), vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0));
    vec4 localPosition = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    vec4 localNormal = vec4(Input.Pos.x + Input.Normal.x, Input.Pos.y + Input.Normal.y, Input.Pos.z + Input.Normal.z, 1.0);
    vec4 localBinormal = vec4(Input.Pos.x + Input.Binormal.x, Input.Pos.y + Input.Binormal.y, Input.Pos.z + Input.Binormal.z, 1.0);
    vec4 localTangent = vec4(Input.Pos.x + Input.Tangent.x, Input.Pos.y + Input.Tangent.y, Input.Pos.z + Input.Tangent.z, 1.0);
    localPosition *= matModel;
    localNormal *= matModel;
    localBinormal *= matModel;
    localTangent *= matModel;
    localNormal = localPosition + normalize(localNormal - localPosition);
    localBinormal = localPosition + normalize(localBinormal - localPosition);
    localTangent = localPosition + normalize(localTangent - localPosition);
    Output.PosVS = localPosition * CBVS0.mCameraProj;
    Output.UV.x = (Input.UV.x * uv.z) + uv.x;
    Output.UV.y = (Input.UV.y * uv.w) + uv.y;
    Output.Normal = localNormal * CBVS0.mCameraProj;
    Output.Binormal = localBinormal * CBVS0.mCameraProj;
    Output.Tangent = localTangent * CBVS0.mCameraProj;
    Output.PosP = Output.PosVS;
    Output.Color = modelColor;
    Output.UV.y = CBVS0.mUVInversed.x + (CBVS0.mUVInversed.y * Output.UV.y);
    return Output;
}

void main()
{
    VS_Input Input;
    Input.Pos = Input_Pos;
    Input.Normal = Input_Normal;
    Input.Binormal = Input_Binormal;
    Input.Tangent = Input_Tangent;
    Input.UV = Input_UV;
    Input.Color = Input_Color;
    Input.Index = uint((gl_InstanceID + SPIRV_Cross_BaseInstance));
    VS_Output flattenTemp = _main(Input);
    gl_Position = flattenTemp.PosVS;
    _VSPS_UV = flattenTemp.UV;
    _VSPS_Normal = flattenTemp.Normal;
    _VSPS_Binormal = flattenTemp.Binormal;
    _VSPS_Tangent = flattenTemp.Tangent;
    _VSPS_PosP = flattenTemp.PosP;
    _VSPS_Color = flattenTemp.Color;
}

)";


    static const char* get_model_distortion_vs (EffekseerRendererGL::OpenGLDeviceType deviceType)
    {
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGL3)
            return model_distortion_vs_gl3;
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGL2)
            return model_distortion_vs_gl2;
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGLES3)
            return model_distortion_vs_gles3;
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGLES2)
            return model_distortion_vs_gles2;
        return nullptr;
    }
    