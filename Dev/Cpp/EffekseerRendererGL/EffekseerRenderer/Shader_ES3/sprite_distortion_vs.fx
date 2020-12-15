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
    vec4 pos4 = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    vec3 worldNormal = (Input.Normal.xyz - vec3(0.5)) * 2.0;
    vec3 worldTangent = (Input.Tangent.xyz - vec3(0.5)) * 2.0;
    vec3 worldBinormal = cross(worldNormal, worldTangent);
    vec4 cameraPos = pos4 * CBVS0.mCamera;
    Output.PosVS = cameraPos * CBVS0.mProj;
    Output.PosP = Output.PosVS;
    vec4 localTangent = pos4;
    vec4 localBinormal = pos4;
    vec3 _82 = localTangent.xyz + worldTangent;
    localTangent = vec4(_82.x, _82.y, _82.z, localTangent.w);
    vec3 _88 = localBinormal.xyz + worldBinormal;
    localBinormal = vec4(_88.x, _88.y, _88.z, localBinormal.w);
    Output.ProjTangent = (localTangent * CBVS0.mCamera) * CBVS0.mProj;
    Output.ProjBinormal = (localBinormal * CBVS0.mCamera) * CBVS0.mProj;
    Output.Color = Input.Color;
    Output.UV = Input.UV1;
    Output.UV.y = CBVS0.mUVInversed.x + (CBVS0.mUVInversed.y * Input.UV1.y);
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

