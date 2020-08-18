#version 420

struct VS_Input
{
    vec3 Pos;
    vec3 Normal;
    vec3 Binormal;
    vec3 Tangent;
    vec2 UV;
    vec4 Color;
    uvec4 Index;
};

struct VS_Output
{
    vec4 Position;
    vec2 UV;
    vec4 Normal;
    vec4 Binormal;
    vec4 Tangent;
    vec4 Pos;
    vec4 Color;
};

layout(set = 0, binding = 0, std140) uniform VS_ConstantBuffer
{
    layout(row_major) mat4 mCameraProj;
    layout(row_major) mat4 mModel[1];
    vec4 fUV[1];
    vec4 fModelColor[1];
    vec4 fLightDirection;
    vec4 fLightColor;
    vec4 fLightAmbient;
    vec4 mUVInversed;
} _27;

layout(location = 0) in vec3 Input_Pos;
layout(location = 1) in vec3 Input_Normal;
layout(location = 2) in vec3 Input_Binormal;
layout(location = 3) in vec3 Input_Tangent;
layout(location = 4) in vec2 Input_UV;
layout(location = 5) in vec4 Input_Color;
layout(location = 6) in uvec4 Input_Index;
layout(location = 0) out vec2 _entryPointOutput_UV;
layout(location = 1) out vec4 _entryPointOutput_Normal;
layout(location = 2) out vec4 _entryPointOutput_Binormal;
layout(location = 3) out vec4 _entryPointOutput_Tangent;
layout(location = 4) out vec4 _entryPointOutput_Pos;
layout(location = 5) out vec4 _entryPointOutput_Color;

VS_Output _main(VS_Input Input)
{
    mat4 matModel = _27.mModel[Input.Index.x];
    vec4 uv = _27.fUV[Input.Index.x];
    vec4 modelColor = _27.fModelColor[Input.Index.x] * Input.Color;
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
    Output.Position = localPosition * _27.mCameraProj;
    Output.UV.x = (Input.UV.x * uv.z) + uv.x;
    Output.UV.y = (Input.UV.y * uv.w) + uv.y;
    Output.Normal = localNormal * _27.mCameraProj;
    Output.Binormal = localBinormal * _27.mCameraProj;
    Output.Tangent = localTangent * _27.mCameraProj;
    Output.Pos = Output.Position;
    Output.Color = modelColor;
    Output.UV.y = _27.mUVInversed.x + (_27.mUVInversed.y * Output.UV.y);
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
    Input.Index = Input_Index;
    VS_Output flattenTemp = _main(Input);
    vec4 _position = flattenTemp.Position;
    _position.y = -_position.y;
    gl_Position = _position;
    _entryPointOutput_UV = flattenTemp.UV;
    _entryPointOutput_Normal = flattenTemp.Normal;
    _entryPointOutput_Binormal = flattenTemp.Binormal;
    _entryPointOutput_Tangent = flattenTemp.Tangent;
    _entryPointOutput_Pos = flattenTemp.Pos;
    _entryPointOutput_Color = flattenTemp.Color;
}

