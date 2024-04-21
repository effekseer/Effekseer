struct PS_Input
{
    float4 Pos;
    float2 UV;
    float4 Color;
    float3 WorldN;
    float3 WorldB;
    float3 WorldT;
};

struct ParameterData
{
    int EmitCount;
    int EmitPerFrame;
    float EmitOffset;
    uint Padding0;
    float2 LifeTime;
    uint EmitShapeType;
    uint EmitRotationApplied;
    float4 EmitShapeData[2];
    float3 Direction;
    float Spread;
    float2 InitialSpeed;
    float2 Damping;
    float4 AngularOffset[2];
    float4 AngularVelocity[2];
    float4 ScaleData1[2];
    float4 ScaleData2[2];
    float3 ScaleEasing;
    uint ScaleFlags;
    float3 Gravity;
    uint Padding2;
    float3 VortexCenter;
    float VortexRotation;
    float3 VortexAxis;
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
    uint4 ColorData;
    float3 ColorEasing;
    uint ColorFlags;
};

struct RenderConstants
{
    float3 CameraPos;
    uint CoordinateReversed;
    float3 CameraFront;
    float Reserved1;
    float3 LightDir;
    float Reserved2;
    float4 LightColor;
    float4 LightAmbient;
    column_major float4x4 ProjMat;
    column_major float4x4 CameraMat;
    column_major float4x3 BillboardMat;
    column_major float4x3 YAxisFixedMat;
};

cbuffer cb1 : register(b1)
{
    ParameterData _45_paramData : packoffset(c0);
};

cbuffer cb0 : register(b0)
{
    RenderConstants _103_constants : packoffset(c0);
};

Texture2D<float4> ColorTex : register(t2);
SamplerState ColorSamp : register(s2);
Texture2D<float4> NormalTex : register(t3);
SamplerState NormalSamp : register(s3);

static float4 gl_FragCoord;
static float2 input_UV;
static float4 input_Color;
static float3 input_WorldN;
static float3 input_WorldB;
static float3 input_WorldT;
static float4 _entryPointOutput;

struct SPIRV_Cross_Input
{
    float2 input_UV : TEXCOORD0;
    float4 input_Color : TEXCOORD1;
    float3 input_WorldN : TEXCOORD2;
    float3 input_WorldB : TEXCOORD3;
    float3 input_WorldT : TEXCOORD4;
    float4 gl_FragCoord : SV_Position;
};

struct SPIRV_Cross_Output
{
    float4 _entryPointOutput : SV_Target0;
};

float4 _main(PS_Input _input)
{
    float4 color = _input.Color * ColorTex.Sample(ColorSamp, _input.UV);
    if (_45_paramData.MaterialType == 1u)
    {
        float3 texNormal = (NormalTex.Sample(NormalSamp, _input.UV).xyz * 2.0f) - 1.0f.xxx;
        float3 normal = normalize(mul(texNormal, float3x3(float3(_input.WorldT), float3(_input.WorldB), float3(_input.WorldN))));
        float diffuse = max(dot(_103_constants.LightDir, normal), 0.0f);
        float4 _122 = color;
        float3 _124 = _122.xyz * ((_103_constants.LightColor.xyz * diffuse) + _103_constants.LightAmbient.xyz);
        color.x = _124.x;
        color.y = _124.y;
        color.z = _124.z;
    }
    return color;
}

void frag_main()
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

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord;
    gl_FragCoord.w = 1.0 / gl_FragCoord.w;
    input_UV = stage_input.input_UV;
    input_Color = stage_input.input_Color;
    input_WorldN = stage_input.input_WorldN;
    input_WorldB = stage_input.input_WorldB;
    input_WorldT = stage_input.input_WorldT;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output._entryPointOutput = _entryPointOutput;
    return stage_output;
}
