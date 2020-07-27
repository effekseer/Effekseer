#version 420

struct VS_Input
{
    vec3 Pos;
    vec4 Color;
    vec2 UV;
};

struct VS_Output
{
    vec4 Position;
    vec4 Color;
    vec2 UV;
};

layout(set = 0, binding = 0, std140) uniform VS_ConstantBuffer
{
    layout(row_major) mat4 mCamera;
    layout(row_major) mat4 mProj;
    vec4 mUVInversed;
} _40;

layout(location = 0) in vec3 Input_Pos;
layout(location = 1) in vec4 Input_Color;
layout(location = 2) in vec2 Input_UV;
layout(location = 0) out vec4 _entryPointOutput_Color;
layout(location = 1) out vec2 _entryPointOutput_UV;

VS_Output _main(VS_Input Input)
{
    VS_Output Output = VS_Output(vec4(0.0), vec4(0.0), vec2(0.0));
    vec4 pos4 = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    vec4 cameraPos = pos4 * _40.mCamera;
    cameraPos /= vec4(cameraPos.w);
    Output.Position = cameraPos * _40.mProj;
    vec4 cameraPosU = cameraPos + vec4(0.0, 1.0, 0.0, 0.0);
    vec4 cameraPosR = cameraPos + vec4(1.0, 0.0, 0.0, 0.0);
    Output.Color = Input.Color;
    Output.UV = Input.UV;
    Output.UV.y = _40.mUVInversed.x + (_40.mUVInversed.y * Input.UV.y);
    return Output;
}

void main()
{
    VS_Input Input;
    Input.Pos = Input_Pos;
    Input.Color = Input_Color;
    Input.UV = Input_UV;
    VS_Output flattenTemp = _main(Input);
    vec4 _position = flattenTemp.Position;
    _position.y = -_position.y;
    gl_Position = _position;
    _entryPointOutput_Color = flattenTemp.Color;
    _entryPointOutput_UV = flattenTemp.UV;
}

