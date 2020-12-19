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
    vec2 UV;
    vec4 ProjBinormal;
    vec4 ProjTangent;
    vec4 PosP;
    vec4 Color;
};

layout(set = 0, binding = 0, std140) uniform VS_ConstantBuffer
{
    layout(row_major) mat4 mCamera;
    layout(row_major) mat4 mProj;
    vec4 mUVInversed;
    vec4 mflipbookParameter;
} _68;

layout(location = 0) in vec3 Input_Pos;
layout(location = 1) in vec4 Input_Color;
layout(location = 2) in vec4 Input_Normal;
layout(location = 3) in vec4 Input_Tangent;
layout(location = 4) in vec2 Input_UV1;
layout(location = 5) in vec2 Input_UV2;
layout(location = 0) centroid out vec2 _entryPointOutput_UV;
layout(location = 1) out vec4 _entryPointOutput_ProjBinormal;
layout(location = 2) out vec4 _entryPointOutput_ProjTangent;
layout(location = 3) out vec4 _entryPointOutput_PosP;
layout(location = 4) centroid out vec4 _entryPointOutput_Color;

VS_Output _main(VS_Input Input)
{
    VS_Output Output = VS_Output(vec4(0.0), vec2(0.0), vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0));
    vec3 worldPos = Input.Pos;
    vec3 worldNormal = (vec3(Input.Normal.xyz) - vec3(0.5)) * 2.0;
    vec3 worldTangent = (vec3(Input.Tangent.xyz) - vec3(0.5)) * 2.0;
    vec3 worldBinormal = cross(worldNormal, worldTangent);
    vec4 pos4 = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    vec4 cameraPos = pos4 * _68.mCamera;
    Output.PosVS = cameraPos * _68.mProj;
    Output.PosP = Output.PosVS;
    vec2 uv1 = Input.UV1;
    uv1.y = _68.mUVInversed.x + (_68.mUVInversed.y * uv1.y);
    Output.UV = uv1;
    vec4 localTangent = pos4;
    vec4 localBinormal = pos4;
    vec3 _106 = localTangent.xyz + worldTangent;
    localTangent = vec4(_106.x, _106.y, _106.z, localTangent.w);
    vec3 _112 = localBinormal.xyz + worldBinormal;
    localBinormal = vec4(_112.x, _112.y, _112.z, localBinormal.w);
    Output.ProjTangent = (localTangent * _68.mCamera) * _68.mProj;
    Output.ProjBinormal = (localBinormal * _68.mCamera) * _68.mProj;
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
    vec4 _position = flattenTemp.PosVS;
    _position.y = -_position.y;
    gl_Position = _position;
    _entryPointOutput_UV = flattenTemp.UV;
    _entryPointOutput_ProjBinormal = flattenTemp.ProjBinormal;
    _entryPointOutput_ProjTangent = flattenTemp.ProjTangent;
    _entryPointOutput_PosP = flattenTemp.PosP;
    _entryPointOutput_Color = flattenTemp.Color;
}

