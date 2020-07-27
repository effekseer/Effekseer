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
    vec4 Position;
    vec4 VColor;
    vec2 UV1;
    vec2 UV2;
    vec3 WorldP;
    vec3 WorldN;
    vec3 WorldT;
    vec3 WorldB;
    vec2 ScreenUV;
};

layout(set = 0, binding = 0, std140) uniform VS_ConstantBuffer
{
    layout(row_major) mat4 mCamera;
    layout(row_major) mat4 mProj;
    vec4 mUVInversed;
} _62;

layout(location = 0) in vec3 Input_Pos;
layout(location = 1) in vec4 Input_Color;
layout(location = 2) in vec4 Input_Normal;
layout(location = 3) in vec4 Input_Tangent;
layout(location = 4) in vec2 Input_UV1;
layout(location = 5) in vec2 Input_UV2;
layout(location = 0) out vec4 _entryPointOutput_VColor;
layout(location = 1) out vec2 _entryPointOutput_UV1;
layout(location = 2) out vec2 _entryPointOutput_UV2;
layout(location = 3) out vec3 _entryPointOutput_WorldP;
layout(location = 4) out vec3 _entryPointOutput_WorldN;
layout(location = 5) out vec3 _entryPointOutput_WorldT;
layout(location = 6) out vec3 _entryPointOutput_WorldB;
layout(location = 7) out vec2 _entryPointOutput_ScreenUV;

VS_Output _main(VS_Input Input)
{
    VS_Output Output = VS_Output(vec4(0.0), vec4(0.0), vec2(0.0), vec2(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec2(0.0));
    vec3 worldPos = Input.Pos;
    vec3 worldNormal = (vec3(Input.Normal.xyz) - vec3(0.5)) * 2.0;
    vec3 worldTangent = (vec3(Input.Tangent.xyz) - vec3(0.5)) * 2.0;
    vec3 worldBinormal = cross(worldNormal, worldTangent);
    vec2 uv1 = Input.UV1;
    vec2 uv2 = Input.UV1;
    uv1.y = _62.mUVInversed.x + (_62.mUVInversed.y * uv1.y);
    uv2.y = _62.mUVInversed.x + (_62.mUVInversed.y * uv2.y);
    Output.WorldN = worldNormal;
    Output.WorldB = worldBinormal;
    Output.WorldT = worldTangent;
    vec3 pixelNormalDir = vec3(0.5, 0.5, 1.0);
    vec4 cameraPos = vec4(worldPos, 1.0) * _62.mCamera;
    cameraPos /= vec4(cameraPos.w);
    Output.Position = cameraPos * _62.mProj;
    Output.WorldP = worldPos;
    Output.VColor = Input.Color;
    Output.UV1 = uv1;
    Output.UV2 = uv2;
    Output.ScreenUV = Output.Position.xy / vec2(Output.Position.w);
    Output.ScreenUV = vec2(Output.ScreenUV.x + 1.0, 1.0 - Output.ScreenUV.y) * 0.5;
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
    vec4 _position = flattenTemp.Position;
    _position.y = -_position.y;
    gl_Position = _position;
    _entryPointOutput_VColor = flattenTemp.VColor;
    _entryPointOutput_UV1 = flattenTemp.UV1;
    _entryPointOutput_UV2 = flattenTemp.UV2;
    _entryPointOutput_WorldP = flattenTemp.WorldP;
    _entryPointOutput_WorldN = flattenTemp.WorldN;
    _entryPointOutput_WorldT = flattenTemp.WorldT;
    _entryPointOutput_WorldB = flattenTemp.WorldB;
    _entryPointOutput_ScreenUV = flattenTemp.ScreenUV;
}

