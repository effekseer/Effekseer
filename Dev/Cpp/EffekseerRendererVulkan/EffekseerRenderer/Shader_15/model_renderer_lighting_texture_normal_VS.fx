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
    vec4 Pos;
    vec2 UV;
    vec3 Normal;
    vec3 Binormal;
    vec3 Tangent;
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
layout(location = 1) out vec3 _entryPointOutput_Normal;
layout(location = 2) out vec3 _entryPointOutput_Binormal;
layout(location = 3) out vec3 _entryPointOutput_Tangent;
layout(location = 4) out vec4 _entryPointOutput_Color;

VS_Output _main(VS_Input Input)
{
    mat4 matModel = _27.mModel[Input.Index.x];
    vec4 uv = _27.fUV[Input.Index.x];
    vec4 modelColor = _27.fModelColor[Input.Index.x] * Input.Color;
    VS_Output Output = VS_Output(vec4(0.0), vec2(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec4(0.0));
    vec4 localPosition = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    vec4 cameraPosition = localPosition * matModel;
    Output.Pos = cameraPosition * _27.mCameraProj;
    Output.Color = modelColor;
    Output.UV.x = (Input.UV.x * uv.z) + uv.x;
    Output.UV.y = (Input.UV.y * uv.w) + uv.y;
    mat3 lightMat = mat3(matModel[0].xyz, matModel[1].xyz, matModel[2].xyz);
    vec4 localNormal = vec4(0.0, 0.0, 0.0, 1.0);
    vec3 _112 = normalize(Input.Normal * lightMat);
    localNormal = vec4(_112.x, _112.y, _112.z, localNormal.w);
    vec4 localBinormal = vec4(0.0, 0.0, 0.0, 1.0);
    vec3 _119 = normalize(Input.Binormal * lightMat);
    localBinormal = vec4(_119.x, _119.y, _119.z, localBinormal.w);
    vec4 localTangent = vec4(0.0, 0.0, 0.0, 1.0);
    vec3 _126 = normalize(Input.Tangent * lightMat);
    localTangent = vec4(_126.x, _126.y, _126.z, localTangent.w);
    Output.Normal = localNormal.xyz;
    Output.Binormal = localBinormal.xyz;
    Output.Tangent = localTangent.xyz;
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
    vec4 _position = flattenTemp.Pos;
    _position.y = -_position.y;
    gl_Position = _position;
    _entryPointOutput_UV = flattenTemp.UV;
    _entryPointOutput_Normal = flattenTemp.Normal;
    _entryPointOutput_Binormal = flattenTemp.Binormal;
    _entryPointOutput_Tangent = flattenTemp.Tangent;
    _entryPointOutput_Color = flattenTemp.Color;
}

