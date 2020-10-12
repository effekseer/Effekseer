#version 300 es

struct VS_Input
{
    vec3 Pos;
    vec4 Color;
    vec2 UV;
    vec3 Binormal;
    vec3 Tangent;
};

struct VS_Output
{
    vec4 Position;
    vec4 Color;
    vec2 UV;
    vec4 Pos;
    vec4 PosU;
    vec4 PosR;
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
layout(location = 2) in vec2 Input_UV;
layout(location = 3) in vec3 Input_Binormal;
layout(location = 4) in vec3 Input_Tangent;
centroid out vec4 _VSPS_Color;
centroid out vec2 _VSPS_UV;
out vec4 _VSPS_Pos;
out vec4 _VSPS_PosU;
out vec4 _VSPS_PosR;

VS_Output _main(VS_Input Input)
{
    VS_Output Output = VS_Output(vec4(0.0), vec4(0.0), vec2(0.0), vec4(0.0), vec4(0.0), vec4(0.0));
    vec4 pos4 = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    vec4 localBinormal = vec4(Input.Pos.x + Input.Binormal.x, Input.Pos.y + Input.Binormal.y, Input.Pos.z + Input.Binormal.z, 1.0);
    vec4 localTangent = vec4(Input.Pos.x + Input.Tangent.x, Input.Pos.y + Input.Tangent.y, Input.Pos.z + Input.Tangent.z, 1.0);
    localBinormal *= CBVS0.mCamera;
    localTangent *= CBVS0.mCamera;
    vec4 cameraPos = pos4 * CBVS0.mCamera;
    cameraPos /= vec4(cameraPos.w);
    localBinormal /= vec4(localBinormal.w);
    localTangent /= vec4(localTangent.w);
    localBinormal = cameraPos + normalize(localBinormal - cameraPos);
    localTangent = cameraPos + normalize(localTangent - cameraPos);
    Output.Position = cameraPos * CBVS0.mProj;
    Output.Pos = Output.Position;
    Output.PosU = localBinormal * CBVS0.mProj;
    Output.PosR = localTangent * CBVS0.mProj;
    Output.PosU /= vec4(Output.PosU.w);
    Output.PosR /= vec4(Output.PosR.w);
    Output.Pos /= vec4(Output.Pos.w);
    Output.Color = Input.Color;
    Output.UV = Input.UV;
    Output.UV.y = CBVS0.mUVInversed.x + (CBVS0.mUVInversed.y * Input.UV.y);
    return Output;
}

void main()
{
    VS_Input Input;
    Input.Pos = Input_Pos;
    Input.Color = Input_Color;
    Input.UV = Input_UV;
    Input.Binormal = Input_Binormal;
    Input.Tangent = Input_Tangent;
    VS_Output flattenTemp = _main(Input);
    gl_Position = flattenTemp.Position;
    _VSPS_Color = flattenTemp.Color;
    _VSPS_UV = flattenTemp.UV;
    _VSPS_Pos = flattenTemp.Pos;
    _VSPS_PosU = flattenTemp.PosU;
    _VSPS_PosR = flattenTemp.PosR;
}

