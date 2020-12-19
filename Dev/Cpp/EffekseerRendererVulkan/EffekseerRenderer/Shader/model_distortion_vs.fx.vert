#version 420

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
    vec4 ProjBinormal;
    vec4 ProjTangent;
    vec4 PosP;
    vec4 Color;
};

layout(set = 0, binding = 0, std140) uniform VS_ConstantBuffer
{
    layout(row_major) mat4 mCameraProj;
    layout(row_major) mat4 mModel[40];
    vec4 fUV[40];
    vec4 fModelColor[40];
    vec4 fLightDirection;
    vec4 fLightColor;
    vec4 fLightAmbient;
    vec4 mUVInversed;
} _31;

layout(location = 0) in vec3 Input_Pos;
layout(location = 1) in vec3 Input_Normal;
layout(location = 2) in vec3 Input_Binormal;
layout(location = 3) in vec3 Input_Tangent;
layout(location = 4) in vec2 Input_UV;
layout(location = 5) in vec4 Input_Color;
layout(location = 0) centroid out vec2 _entryPointOutput_UV;
layout(location = 1) out vec4 _entryPointOutput_ProjBinormal;
layout(location = 2) out vec4 _entryPointOutput_ProjTangent;
layout(location = 3) out vec4 _entryPointOutput_PosP;
layout(location = 4) centroid out vec4 _entryPointOutput_Color;

VS_Output _main(VS_Input Input)
{
    uint index = Input.Index;
    mat4 matModel = _31.mModel[index];
    vec4 uv = _31.fUV[index];
    vec4 modelColor = _31.fModelColor[index] * Input.Color;
    VS_Output Output = VS_Output(vec4(0.0), vec2(0.0), vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0));
    vec4 localPosition = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    localPosition *= matModel;
    Output.PosVS = localPosition * _31.mCameraProj;
    Output.Color = modelColor;
    Output.UV.x = (Input.UV.x * uv.z) + uv.x;
    Output.UV.y = (Input.UV.y * uv.w) + uv.y;
    vec4 localNormal = vec4(Input.Normal.x, Input.Normal.y, Input.Normal.z, 0.0);
    vec4 localBinormal = vec4(Input.Binormal.x, Input.Binormal.y, Input.Binormal.z, 0.0);
    vec4 localTangent = vec4(Input.Tangent.x, Input.Tangent.y, Input.Tangent.z, 0.0);
    localNormal *= matModel;
    localBinormal *= matModel;
    localTangent *= matModel;
    localNormal = normalize(localNormal);
    localBinormal = normalize(localBinormal);
    localTangent = normalize(localTangent);
    localBinormal = localPosition + localBinormal;
    localTangent = localPosition + localTangent;
    Output.ProjBinormal = localBinormal * _31.mCameraProj;
    Output.ProjTangent = localTangent * _31.mCameraProj;
    Output.UV.y = _31.mUVInversed.x + (_31.mUVInversed.y * Output.UV.y);
    Output.PosP = Output.PosVS;
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
    Input.Index = uint(gl_InstanceIndex);
    VS_Output flattenTemp = _main(Input);
    vec4 _position = flattenTemp.PosVS;
    _position.y = -_position.y;
    gl_Position = _position;
    _entryPointOutput_UV = flattenTemp.UV;
    _entryPointOutput_ProjBinormal = flattenTemp.ProjBinormal;
    _entryPointOutput_ProjTangent = flattenTemp.ProjTangent;
    _entryPointOutput_PosP = flattenTemp.PosP;
    _entryPointOutput_Color = flattenTemp.Color;
}

