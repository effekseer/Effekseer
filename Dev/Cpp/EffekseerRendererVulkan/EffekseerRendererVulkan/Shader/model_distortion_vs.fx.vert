#version 430

struct VS_Input
{
    vec3 Pos;
    vec2 OctNormal;
    vec2 OctTangent;
    vec2 UV1;
    vec2 UV2;
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
    layout(row_major) mat4 mModel_Inst[40];
    vec4 fUV[40];
    vec4 fModelColor[40];
    vec4 fLightDirection;
    vec4 fLightColor;
    vec4 fLightAmbient;
    vec4 mUVInversed;
} _118;

layout(location = 0) in vec3 Input_Pos;
layout(location = 1) in vec2 Input_OctNormal;
layout(location = 2) in vec2 Input_OctTangent;
layout(location = 3) in vec2 Input_UV1;
layout(location = 4) in vec2 Input_UV2;
layout(location = 5) in vec4 Input_Color;
layout(location = 0) centroid out vec2 _entryPointOutput_UV;
layout(location = 1) out vec4 _entryPointOutput_ProjBinormal;
layout(location = 2) out vec4 _entryPointOutput_ProjTangent;
layout(location = 3) out vec4 _entryPointOutput_PosP;
layout(location = 4) centroid out vec4 _entryPointOutput_Color;

vec3 decodeOct(vec2 oct)
{
    vec3 v = vec3(oct, (1.0 - abs(oct.x)) - abs(oct.y));
    float t = max(-v.z, 0.0);
    vec3 _56 = v;
    vec3 _61 = v;
    vec2 _63 = _61.xy + ((-sign(_56.xy)) * t);
    v.x = _63.x;
    v.y = _63.y;
    return normalize(v);
}

void decodeOct(inout vec2 octNormal, inout vec2 octTangent, inout vec3 normal, out vec3 binormal, inout vec3 tangent)
{
    octNormal = (octNormal * 2.0) - vec2(1.0);
    vec2 param = octNormal;
    normal = decodeOct(param);
    octTangent = (octTangent * 2.0) - vec2(1.0);
    float s = sign(octTangent.y);
    octTangent = vec2(octTangent.x, (abs(octTangent.y) * 2.0) - 1.0);
    vec2 param_1 = octTangent;
    tangent = decodeOct(param_1);
    binormal = cross(normal, tangent) * s;
}

VS_Output _main(VS_Input Input)
{
    uint index = Input.Index;
    mat4 mModel = _118.mModel_Inst[index];
    vec4 uv = _118.fUV[index];
    vec4 modelColor = _118.fModelColor[index] * Input.Color;
    VS_Output Output = VS_Output(vec4(0.0), vec2(0.0), vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0));
    vec4 localPos = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    vec4 worldPos = localPos * mModel;
    Output.PosVS = worldPos * _118.mCameraProj;
    Output.Color = modelColor;
    vec2 outputUV = Input.UV1;
    outputUV.x = (outputUV.x * uv.z) + uv.x;
    outputUV.y = (outputUV.y * uv.w) + uv.y;
    outputUV.y = _118.mUVInversed.x + (_118.mUVInversed.y * outputUV.y);
    Output.UV = outputUV;
    vec2 param = Input.OctNormal;
    vec2 param_1 = Input.OctTangent;
    vec3 param_2;
    vec3 param_3;
    vec3 param_4;
    decodeOct(param, param_1, param_2, param_3, param_4);
    vec3 localNormal = param_2;
    vec3 localBinormal = param_3;
    vec3 localTangent = param_4;
    vec4 worldNormal = vec4(localNormal, 0.0) * mModel;
    vec4 worldBinormal = vec4(localTangent, 0.0) * mModel;
    vec4 worldTangent = vec4(localBinormal, 0.0) * mModel;
    worldNormal = normalize(worldNormal);
    worldBinormal = normalize(worldBinormal);
    worldTangent = normalize(worldTangent);
    Output.ProjBinormal = (worldPos + worldBinormal) * _118.mCameraProj;
    Output.ProjTangent = (worldPos + worldTangent) * _118.mCameraProj;
    Output.PosP = Output.PosVS;
    return Output;
}

void main()
{
    VS_Input Input;
    Input.Pos = Input_Pos;
    Input.OctNormal = Input_OctNormal;
    Input.OctTangent = Input_OctTangent;
    Input.UV1 = Input_UV1;
    Input.UV2 = Input_UV2;
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

