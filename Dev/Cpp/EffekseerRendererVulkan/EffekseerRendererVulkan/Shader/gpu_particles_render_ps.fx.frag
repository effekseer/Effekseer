#version 430

struct PS_Input
{
    vec4 Pos;
    vec2 UV;
    vec4 Color;
    vec3 WorldN;
    vec3 WorldB;
    vec3 WorldT;
};

struct ParameterData
{
    int EmitCount;
    int EmitPerFrame;
    float EmitOffset;
    uint Padding0;
    vec2 LifeTime;
    uint EmitShapeType;
    uint EmitRotationApplied;
    vec4 EmitShapeData[2];
    vec3 Direction;
    float Spread;
    vec2 InitialSpeed;
    vec2 Damping;
    vec4 AngularOffset[2];
    vec4 AngularVelocity[2];
    vec4 ScaleData1[2];
    vec4 ScaleData2[2];
    vec3 ScaleEasing;
    uint ScaleFlags;
    vec3 Gravity;
    uint Padding2;
    vec3 VortexCenter;
    float VortexRotation;
    vec3 VortexAxis;
    float VortexAttraction;
    float TurbulencePower;
    uint TurbulenceSeed;
    float TurbulenceScale;
    float TurbulenceOctave;
    uint RenderState;
    uint ShapeType;
    uint ShapeData;
    float ShapeSize;
    float Emissive;
    float FadeIn;
    float FadeOut;
    uint MaterialType;
    uvec4 ColorData;
    vec3 ColorEasing;
    uint ColorFlags;
};

struct RenderConstants
{
    vec3 CameraPos;
    uint CoordinateReversed;
    vec3 CameraFront;
    float Reserved1;
    vec3 LightDir;
    float Reserved2;
    vec4 LightColor;
    vec4 LightAmbient;
    mat4 ProjMat;
    mat4 CameraMat;
    mat4x3 BillboardMat;
    mat4x3 YAxisFixedMat;
};

layout(set = 0, binding = 1, std140) uniform cb1
{
    ParameterData paramData;
} _45;

layout(set = 0, binding = 0, std140) uniform cb0
{
    layout(row_major) RenderConstants constants;
} _103;

layout(set = 1, binding = 2) uniform sampler2D Sampler_ColorSamp;
layout(set = 1, binding = 3) uniform sampler2D Sampler_NormalSamp;

layout(location = 0) in vec2 input_UV;
layout(location = 1) in vec4 input_Color;
layout(location = 2) in vec3 input_WorldN;
layout(location = 3) in vec3 input_WorldB;
layout(location = 4) in vec3 input_WorldT;
layout(location = 0) out vec4 _entryPointOutput;

vec4 _main(PS_Input _input)
{
    vec4 color = _input.Color * texture(Sampler_ColorSamp, _input.UV);
    if (_45.paramData.MaterialType == 1u)
    {
        vec3 texNormal = (texture(Sampler_NormalSamp, _input.UV).xyz * 2.0) - vec3(1.0);
        vec3 normal = normalize(mat3(vec3(_input.WorldT), vec3(_input.WorldB), vec3(_input.WorldN)) * texNormal);
        float diffuse = max(dot(_103.constants.LightDir, normal), 0.0);
        vec4 _122 = color;
        vec3 _124 = _122.xyz * ((_103.constants.LightColor.xyz * diffuse) + _103.constants.LightAmbient.xyz);
        color.x = _124.x;
        color.y = _124.y;
        color.z = _124.z;
    }
    return color;
}

void main()
{
    PS_Input _input;
    _input.Pos = gl_FragCoord;
    _input.UV = input_UV;
    _input.Color = input_Color;
    _input.WorldN = input_WorldN;
    _input.WorldB = input_WorldB;
    _input.WorldT = input_WorldT;
    _entryPointOutput = _main(_input);
}

