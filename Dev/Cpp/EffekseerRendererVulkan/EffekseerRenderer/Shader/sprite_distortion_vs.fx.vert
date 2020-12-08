#version 420

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
    vec4 Color;
    vec2 UV;
    vec4 PosP;
    vec4 PosU;
    vec4 PosR;
};

layout(set = 0, binding = 0, std140) uniform VS_ConstantBuffer
{
    layout(row_major) mat4 mCamera;
    layout(row_major) mat4 mProj;
    vec4 mUVInversed;
    vec4 mflipbookParameter;
} _88;

layout(location = 0) in vec3 Input_Pos;
layout(location = 1) in vec4 Input_Color;
layout(location = 2) in vec4 Input_Normal;
layout(location = 3) in vec4 Input_Tangent;
layout(location = 4) in vec2 Input_UV1;
layout(location = 5) in vec2 Input_UV2;
layout(location = 0) centroid out vec4 _entryPointOutput_Color;
layout(location = 1) centroid out vec2 _entryPointOutput_UV;
layout(location = 2) out vec4 _entryPointOutput_PosP;
layout(location = 3) out vec4 _entryPointOutput_PosU;
layout(location = 4) out vec4 _entryPointOutput_PosR;

VS_Output _main(VS_Input Input)
{
    VS_Output Output = VS_Output(vec4(0.0), vec4(0.0), vec2(0.0), vec4(0.0), vec4(0.0), vec4(0.0));
    vec4 pos4 = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    vec3 worldNormal = (Input.Normal.xyz - vec3(0.5)) * 2.0;
    vec3 worldTangent = (Input.Tangent.xyz - vec3(0.5)) * 2.0;
    vec3 worldBinormal = cross(worldNormal, worldTangent);
    vec4 localBinormal = vec4(Input.Pos.x + worldBinormal.x, Input.Pos.y + worldBinormal.y, Input.Pos.z + worldBinormal.z, 1.0);
    vec4 localTangent = vec4(Input.Pos.x + worldTangent.x, Input.Pos.y + worldTangent.y, Input.Pos.z + worldTangent.z, 1.0);
    localBinormal *= _88.mCamera;
    localTangent *= _88.mCamera;
    vec4 cameraPos = pos4 * _88.mCamera;
    cameraPos /= vec4(cameraPos.w);
    localBinormal /= vec4(localBinormal.w);
    localTangent /= vec4(localTangent.w);
    localBinormal = cameraPos + normalize(localBinormal - cameraPos);
    localTangent = cameraPos + normalize(localTangent - cameraPos);
    Output.PosVS = cameraPos * _88.mProj;
    Output.PosP = Output.PosVS;
    Output.PosU = localBinormal * _88.mProj;
    Output.PosR = localTangent * _88.mProj;
    Output.PosU /= vec4(Output.PosU.w);
    Output.PosR /= vec4(Output.PosR.w);
    Output.PosP /= vec4(Output.PosP.w);
    Output.Color = Input.Color;
    Output.UV = Input.UV1;
    Output.UV.y = _88.mUVInversed.x + (_88.mUVInversed.y * Input.UV1.y);
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
    vec4 _position = flattenTemp.PosVS;
    _position.y = -_position.y;
    gl_Position = _position;
    _entryPointOutput_Color = flattenTemp.Color;
    _entryPointOutput_UV = flattenTemp.UV;
    _entryPointOutput_PosP = flattenTemp.PosP;
    _entryPointOutput_PosU = flattenTemp.PosU;
    _entryPointOutput_PosR = flattenTemp.PosR;
}

