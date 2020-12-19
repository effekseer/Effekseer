static const char sprite_distortion_vs_gl2[] = R"(
#version 120
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_Input
{
    vec3 Pos;
    vec4 Color;
    vec4 Normal;
    vec4 Tangent;
    vec2 UV1;
    vec2 UV2;
};

struct VS_Output
{
    vec4 PosVS;
    vec2 UV;
    vec4 ProjBinormal;
    vec4 ProjTangent;
    vec4 PosP;
    vec4 Color;
};

struct VS_ConstantBuffer
{
    mat4 mCamera;
    mat4 mProj;
    vec4 mUVInversed;
    vec4 mflipbookParameter;
};

uniform VS_ConstantBuffer CBVS0;

attribute vec3 Input_Pos;
attribute vec4 Input_Color;
attribute vec4 Input_Normal;
attribute vec4 Input_Tangent;
attribute vec2 Input_UV1;
attribute vec2 Input_UV2;
centroid varying vec2 _VSPS_UV;
varying vec4 _VSPS_ProjBinormal;
varying vec4 _VSPS_ProjTangent;
varying vec4 _VSPS_PosP;
centroid varying vec4 _VSPS_Color;

VS_Output _main(VS_Input Input)
{
    VS_Output Output = VS_Output(vec4(0.0), vec2(0.0), vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0));
    vec3 worldPos = Input.Pos;
    vec3 worldNormal = (vec3(Input.Normal.xyz) - vec3(0.5)) * 2.0;
    vec3 worldTangent = (vec3(Input.Tangent.xyz) - vec3(0.5)) * 2.0;
    vec3 worldBinormal = cross(worldNormal, worldTangent);
    vec4 pos4 = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    vec4 cameraPos = CBVS0.mCamera * pos4;
    Output.PosVS = CBVS0.mProj * cameraPos;
    Output.PosP = Output.PosVS;
    vec2 uv1 = Input.UV1;
    uv1.y = CBVS0.mUVInversed.x + (CBVS0.mUVInversed.y * uv1.y);
    Output.UV = uv1;
    vec4 localTangent = pos4;
    vec4 localBinormal = pos4;
    vec3 _106 = localTangent.xyz + worldTangent;
    localTangent = vec4(_106.x, _106.y, _106.z, localTangent.w);
    vec3 _112 = localBinormal.xyz + worldBinormal;
    localBinormal = vec4(_112.x, _112.y, _112.z, localBinormal.w);
    Output.ProjTangent = CBVS0.mProj * (CBVS0.mCamera * localTangent);
    Output.ProjBinormal = CBVS0.mProj * (CBVS0.mCamera * localBinormal);
    Output.Color = Input.Color;
    return Output;
}

void main()
{
    VS_Input Input;
    Input.Pos = Input_Pos;
    Input.Color = Input_Color;
    Input.Normal = Input_Normal;
    Input.Tangent = Input_Tangent;
    Input.UV1 = Input_UV1;
    Input.UV2 = Input_UV2;
    VS_Output flattenTemp = _main(Input);
    gl_Position = flattenTemp.PosVS;
    _VSPS_UV = flattenTemp.UV;
    _VSPS_ProjBinormal = flattenTemp.ProjBinormal;
    _VSPS_ProjTangent = flattenTemp.ProjTangent;
    _VSPS_PosP = flattenTemp.PosP;
    _VSPS_Color = flattenTemp.Color;
}

)";

static const char sprite_distortion_vs_gl3[] = R"(
#version 330
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_Input
{
    vec3 Pos;
    vec4 Color;
    vec4 Normal;
    vec4 Tangent;
    vec2 UV1;
    vec2 UV2;
};

struct VS_Output
{
    vec4 PosVS;
    vec2 UV;
    vec4 ProjBinormal;
    vec4 ProjTangent;
    vec4 PosP;
    vec4 Color;
};

struct VS_ConstantBuffer
{
    mat4 mCamera;
    mat4 mProj;
    vec4 mUVInversed;
    vec4 mflipbookParameter;
};

uniform VS_ConstantBuffer CBVS0;

layout(location = 0) in vec3 Input_Pos;
layout(location = 1) in vec4 Input_Color;
layout(location = 2) in vec4 Input_Normal;
layout(location = 3) in vec4 Input_Tangent;
layout(location = 4) in vec2 Input_UV1;
layout(location = 5) in vec2 Input_UV2;
centroid out vec2 _VSPS_UV;
out vec4 _VSPS_ProjBinormal;
out vec4 _VSPS_ProjTangent;
out vec4 _VSPS_PosP;
centroid out vec4 _VSPS_Color;

VS_Output _main(VS_Input Input)
{
    VS_Output Output = VS_Output(vec4(0.0), vec2(0.0), vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0));
    vec3 worldPos = Input.Pos;
    vec3 worldNormal = (vec3(Input.Normal.xyz) - vec3(0.5)) * 2.0;
    vec3 worldTangent = (vec3(Input.Tangent.xyz) - vec3(0.5)) * 2.0;
    vec3 worldBinormal = cross(worldNormal, worldTangent);
    vec4 pos4 = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    vec4 cameraPos = pos4 * CBVS0.mCamera;
    Output.PosVS = cameraPos * CBVS0.mProj;
    Output.PosP = Output.PosVS;
    vec2 uv1 = Input.UV1;
    uv1.y = CBVS0.mUVInversed.x + (CBVS0.mUVInversed.y * uv1.y);
    Output.UV = uv1;
    vec4 localTangent = pos4;
    vec4 localBinormal = pos4;
    vec3 _106 = localTangent.xyz + worldTangent;
    localTangent = vec4(_106.x, _106.y, _106.z, localTangent.w);
    vec3 _112 = localBinormal.xyz + worldBinormal;
    localBinormal = vec4(_112.x, _112.y, _112.z, localBinormal.w);
    Output.ProjTangent = (localTangent * CBVS0.mCamera) * CBVS0.mProj;
    Output.ProjBinormal = (localBinormal * CBVS0.mCamera) * CBVS0.mProj;
    Output.Color = Input.Color;
    return Output;
}

void main()
{
    VS_Input Input;
    Input.Pos = Input_Pos;
    Input.Color = Input_Color;
    Input.Normal = Input_Normal;
    Input.Tangent = Input_Tangent;
    Input.UV1 = Input_UV1;
    Input.UV2 = Input_UV2;
    VS_Output flattenTemp = _main(Input);
    gl_Position = flattenTemp.PosVS;
    _VSPS_UV = flattenTemp.UV;
    _VSPS_ProjBinormal = flattenTemp.ProjBinormal;
    _VSPS_ProjTangent = flattenTemp.ProjTangent;
    _VSPS_PosP = flattenTemp.PosP;
    _VSPS_Color = flattenTemp.Color;
}

)";

static const char sprite_distortion_vs_gles2[] = R"(


struct VS_Input
{
    vec3 Pos;
    vec4 Color;
    vec4 Normal;
    vec4 Tangent;
    vec2 UV1;
    vec2 UV2;
};

struct VS_Output
{
    vec4 PosVS;
    vec2 UV;
    vec4 ProjBinormal;
    vec4 ProjTangent;
    vec4 PosP;
    vec4 Color;
};

struct VS_ConstantBuffer
{
    mat4 mCamera;
    mat4 mProj;
    vec4 mUVInversed;
    vec4 mflipbookParameter;
};

uniform VS_ConstantBuffer CBVS0;

attribute vec3 Input_Pos;
attribute vec4 Input_Color;
attribute vec4 Input_Normal;
attribute vec4 Input_Tangent;
attribute vec2 Input_UV1;
attribute vec2 Input_UV2;
varying vec2 _VSPS_UV;
varying vec4 _VSPS_ProjBinormal;
varying vec4 _VSPS_ProjTangent;
varying vec4 _VSPS_PosP;
varying vec4 _VSPS_Color;

VS_Output _main(VS_Input Input)
{
    VS_Output Output = VS_Output(vec4(0.0), vec2(0.0), vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0));
    vec3 worldPos = Input.Pos;
    vec3 worldNormal = (vec3(Input.Normal.xyz) - vec3(0.5)) * 2.0;
    vec3 worldTangent = (vec3(Input.Tangent.xyz) - vec3(0.5)) * 2.0;
    vec3 worldBinormal = cross(worldNormal, worldTangent);
    vec4 pos4 = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    vec4 cameraPos = CBVS0.mCamera * pos4;
    Output.PosVS = CBVS0.mProj * cameraPos;
    Output.PosP = Output.PosVS;
    vec2 uv1 = Input.UV1;
    uv1.y = CBVS0.mUVInversed.x + (CBVS0.mUVInversed.y * uv1.y);
    Output.UV = uv1;
    vec4 localTangent = pos4;
    vec4 localBinormal = pos4;
    vec3 _106 = localTangent.xyz + worldTangent;
    localTangent = vec4(_106.x, _106.y, _106.z, localTangent.w);
    vec3 _112 = localBinormal.xyz + worldBinormal;
    localBinormal = vec4(_112.x, _112.y, _112.z, localBinormal.w);
    Output.ProjTangent = CBVS0.mProj * (CBVS0.mCamera * localTangent);
    Output.ProjBinormal = CBVS0.mProj * (CBVS0.mCamera * localBinormal);
    Output.Color = Input.Color;
    return Output;
}

void main()
{
    VS_Input Input;
    Input.Pos = Input_Pos;
    Input.Color = Input_Color;
    Input.Normal = Input_Normal;
    Input.Tangent = Input_Tangent;
    Input.UV1 = Input_UV1;
    Input.UV2 = Input_UV2;
    VS_Output flattenTemp = _main(Input);
    gl_Position = flattenTemp.PosVS;
    _VSPS_UV = flattenTemp.UV;
    _VSPS_ProjBinormal = flattenTemp.ProjBinormal;
    _VSPS_ProjTangent = flattenTemp.ProjTangent;
    _VSPS_PosP = flattenTemp.PosP;
    _VSPS_Color = flattenTemp.Color;
}

)";

static const char sprite_distortion_vs_gles3[] = R"(
#version 300 es

struct VS_Input
{
    vec3 Pos;
    vec4 Color;
    vec4 Normal;
    vec4 Tangent;
    vec2 UV1;
    vec2 UV2;
};

struct VS_Output
{
    vec4 PosVS;
    vec2 UV;
    vec4 ProjBinormal;
    vec4 ProjTangent;
    vec4 PosP;
    vec4 Color;
};

struct VS_ConstantBuffer
{
    mat4 mCamera;
    mat4 mProj;
    vec4 mUVInversed;
    vec4 mflipbookParameter;
};

uniform VS_ConstantBuffer CBVS0;

layout(location = 0) in vec3 Input_Pos;
layout(location = 1) in vec4 Input_Color;
layout(location = 2) in vec4 Input_Normal;
layout(location = 3) in vec4 Input_Tangent;
layout(location = 4) in vec2 Input_UV1;
layout(location = 5) in vec2 Input_UV2;
centroid out vec2 _VSPS_UV;
out vec4 _VSPS_ProjBinormal;
out vec4 _VSPS_ProjTangent;
out vec4 _VSPS_PosP;
centroid out vec4 _VSPS_Color;

VS_Output _main(VS_Input Input)
{
    VS_Output Output = VS_Output(vec4(0.0), vec2(0.0), vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0));
    vec3 worldPos = Input.Pos;
    vec3 worldNormal = (vec3(Input.Normal.xyz) - vec3(0.5)) * 2.0;
    vec3 worldTangent = (vec3(Input.Tangent.xyz) - vec3(0.5)) * 2.0;
    vec3 worldBinormal = cross(worldNormal, worldTangent);
    vec4 pos4 = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    vec4 cameraPos = pos4 * CBVS0.mCamera;
    Output.PosVS = cameraPos * CBVS0.mProj;
    Output.PosP = Output.PosVS;
    vec2 uv1 = Input.UV1;
    uv1.y = CBVS0.mUVInversed.x + (CBVS0.mUVInversed.y * uv1.y);
    Output.UV = uv1;
    vec4 localTangent = pos4;
    vec4 localBinormal = pos4;
    vec3 _106 = localTangent.xyz + worldTangent;
    localTangent = vec4(_106.x, _106.y, _106.z, localTangent.w);
    vec3 _112 = localBinormal.xyz + worldBinormal;
    localBinormal = vec4(_112.x, _112.y, _112.z, localBinormal.w);
    Output.ProjTangent = (localTangent * CBVS0.mCamera) * CBVS0.mProj;
    Output.ProjBinormal = (localBinormal * CBVS0.mCamera) * CBVS0.mProj;
    Output.Color = Input.Color;
    return Output;
}

void main()
{
    VS_Input Input;
    Input.Pos = Input_Pos;
    Input.Color = Input_Color;
    Input.Normal = Input_Normal;
    Input.Tangent = Input_Tangent;
    Input.UV1 = Input_UV1;
    Input.UV2 = Input_UV2;
    VS_Output flattenTemp = _main(Input);
    gl_Position = flattenTemp.PosVS;
    _VSPS_UV = flattenTemp.UV;
    _VSPS_ProjBinormal = flattenTemp.ProjBinormal;
    _VSPS_ProjTangent = flattenTemp.ProjTangent;
    _VSPS_PosP = flattenTemp.PosP;
    _VSPS_Color = flattenTemp.Color;
}

)";


    static const char* get_sprite_distortion_vs (EffekseerRendererGL::OpenGLDeviceType deviceType)
    {
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGL3)
            return sprite_distortion_vs_gl3;
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGL2)
            return sprite_distortion_vs_gl2;
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGLES3)
            return sprite_distortion_vs_gles3;
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGLES2)
            return sprite_distortion_vs_gles2;
        return nullptr;
    }
    