#version 420

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

layout(set = 0, binding = 0, std140) uniform VS_ConstantBuffer
{
    layout(row_major) mat4 mCamera;
    layout(row_major) mat4 mProj;
    vec4 mUVInversed;
    vec4 mflipbookParameter;
} _63;

layout(location = 0) in vec3 Input_Pos;
layout(location = 1) in vec4 Input_Color;
layout(location = 2) in vec2 Input_UV;
layout(location = 3) in vec3 Input_Binormal;
layout(location = 4) in vec3 Input_Tangent;
layout(location = 0) centroid out vec4 _entryPointOutput_Color;
layout(location = 1) centroid out vec2 _entryPointOutput_UV;
layout(location = 2) out vec4 _entryPointOutput_Pos;
layout(location = 3) out vec4 _entryPointOutput_PosU;
layout(location = 4) out vec4 _entryPointOutput_PosR;

VS_Output _main(VS_Input Input)
{
    VS_Output Output = VS_Output(vec4(0.0), vec4(0.0), vec2(0.0), vec4(0.0), vec4(0.0), vec4(0.0));
    vec4 pos4 = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    vec4 localBinormal = vec4(Input.Pos.x + Input.Binormal.x, Input.Pos.y + Input.Binormal.y, Input.Pos.z + Input.Binormal.z, 1.0);
    vec4 localTangent = vec4(Input.Pos.x + Input.Tangent.x, Input.Pos.y + Input.Tangent.y, Input.Pos.z + Input.Tangent.z, 1.0);
    localBinormal *= _63.mCamera;
    localTangent *= _63.mCamera;
    vec4 cameraPos = pos4 * _63.mCamera;
    cameraPos /= vec4(cameraPos.w);
    localBinormal /= vec4(localBinormal.w);
    localTangent /= vec4(localTangent.w);
    localBinormal = cameraPos + normalize(localBinormal - cameraPos);
    localTangent = cameraPos + normalize(localTangent - cameraPos);
    Output.Position = cameraPos * _63.mProj;
    Output.Pos = Output.Position;
    Output.PosU = localBinormal * _63.mProj;
    Output.PosR = localTangent * _63.mProj;
    Output.PosU /= vec4(Output.PosU.w);
    Output.PosR /= vec4(Output.PosR.w);
    Output.Pos /= vec4(Output.Pos.w);
    Output.Color = Input.Color;
    Output.UV = Input.UV;
    Output.UV.y = _63.mUVInversed.x + (_63.mUVInversed.y * Input.UV.y);
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
    vec4 _position = flattenTemp.Position;
    _position.y = -_position.y;
    gl_Position = _position;
    _entryPointOutput_Color = flattenTemp.Color;
    _entryPointOutput_UV = flattenTemp.UV;
    _entryPointOutput_Pos = flattenTemp.Pos;
    _entryPointOutput_PosU = flattenTemp.PosU;
    _entryPointOutput_PosR = flattenTemp.PosR;
}

