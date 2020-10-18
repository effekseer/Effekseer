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
    vec4 Color;
    vec4 PosP;
};

layout(set = 0, binding = 0, std140) uniform VS_ConstantBuffer
{
    layout(row_major) mat4 mCameraProj;
    layout(row_major) mat4 mModel[10];
    vec4 fUV[10];
    vec4 fModelColor[10];
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
layout(location = 1) centroid out vec4 _entryPointOutput_Color;
layout(location = 2) out vec4 _entryPointOutput_PosP;

VS_Output _main(VS_Input Input)
{
    uint index = Input.Index;
    mat4 matModel = _31.mModel[index];
    vec4 uv = _31.fUV[index];
    vec4 modelColor = _31.fModelColor[index] * Input.Color;
    VS_Output Output = VS_Output(vec4(0.0), vec2(0.0), vec4(0.0), vec4(0.0));
    vec4 localPosition = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    vec4 cameraPosition = localPosition * matModel;
    Output.PosVS = cameraPosition * _31.mCameraProj;
    Output.Color = modelColor;
    Output.UV.x = (Input.UV.x * uv.z) + uv.x;
    Output.UV.y = (Input.UV.y * uv.w) + uv.y;
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
    _entryPointOutput_Color = flattenTemp.Color;
    _entryPointOutput_PosP = flattenTemp.PosP;
}

