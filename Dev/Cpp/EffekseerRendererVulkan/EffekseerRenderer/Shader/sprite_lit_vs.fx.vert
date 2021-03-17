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
    vec3 WorldN;
    vec3 WorldB;
    vec3 WorldT;
    vec4 PosP;
};

layout(set = 0, binding = 0, std140) uniform VS_ConstantBuffer
{
    layout(row_major) mat4 mCamera;
    layout(row_major) mat4 mCameraProj;
    vec4 mUVInversed;
    vec4 mflipbookParameter;
} _73;

layout(location = 0) in vec3 Input_Pos;
layout(location = 1) in vec4 Input_Color;
layout(location = 2) in vec4 Input_Normal;
layout(location = 3) in vec4 Input_Tangent;
layout(location = 4) in vec2 Input_UV1;
layout(location = 5) in vec2 Input_UV2;
layout(location = 0) centroid out vec4 _entryPointOutput_Color;
layout(location = 1) centroid out vec2 _entryPointOutput_UV;
layout(location = 2) out vec3 _entryPointOutput_WorldN;
layout(location = 3) out vec3 _entryPointOutput_WorldB;
layout(location = 4) out vec3 _entryPointOutput_WorldT;
layout(location = 5) out vec4 _entryPointOutput_PosP;

VS_Output _main(VS_Input Input)
{
    VS_Output Output = VS_Output(vec4(0.0), vec4(0.0), vec2(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec4(0.0));
    vec4 worldNormal = vec4((Input.Normal.xyz - vec3(0.5)) * 2.0, 0.0);
    vec4 worldTangent = vec4((Input.Tangent.xyz - vec3(0.5)) * 2.0, 0.0);
    vec4 worldBinormal = vec4(cross(worldNormal.xyz, worldTangent.xyz), 0.0);
    vec4 worldPos = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    Output.PosVS = worldPos * _73.mCameraProj;
    Output.Color = Input.Color;
    vec2 uv1 = Input.UV1;
    uv1.y = _73.mUVInversed.x + (_73.mUVInversed.y * uv1.y);
    Output.UV = uv1;
    Output.WorldN = worldNormal.xyz;
    Output.WorldB = worldBinormal.xyz;
    Output.WorldT = worldTangent.xyz;
    Output.PosP = Output.PosVS;
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
    _entryPointOutput_WorldN = flattenTemp.WorldN;
    _entryPointOutput_WorldB = flattenTemp.WorldB;
    _entryPointOutput_WorldT = flattenTemp.WorldT;
    _entryPointOutput_PosP = flattenTemp.PosP;
}

