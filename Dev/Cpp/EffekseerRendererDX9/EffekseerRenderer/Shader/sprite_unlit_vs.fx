struct VS_Input
{
    float3 Pos;
    float4 Color;
    float2 UV;
};

struct VS_Output
{
    float4 PosVS;
    float4 Color;
    float2 UV;
    float4 PosP;
};

static const VS_Output _21 = { 0.0f.xxxx, 0.0f.xxxx, 0.0f.xx, 0.0f.xxxx };

cbuffer VS_ConstantBuffer : register(b0)
{
    column_major float4x4 _40_mCamera : register(c0);
    column_major float4x4 _40_mProj : register(c4);
    float4 _40_mUVInversed : register(c8);
    float4 _40_mflipbookParameter : register(c9);
};

static const float4 gl_HalfPixel = 0.0f.xxxx;

static float4 gl_Position;
static float3 Input_Pos;
static float4 Input_Color;
static float2 Input_UV;
static float4 _entryPointOutput_Color;
static float2 _entryPointOutput_UV;
static float4 _entryPointOutput_PosP;

struct SPIRV_Cross_Input
{
    float3 Input_Pos : TEXCOORD0;
    float4 Input_Color : TEXCOORD1;
    float2 Input_UV : TEXCOORD2;
};

struct SPIRV_Cross_Output
{
    centroid float4 _entryPointOutput_Color : TEXCOORD0;
    centroid float2 _entryPointOutput_UV : TEXCOORD1;
    float4 _entryPointOutput_PosP : TEXCOORD2;
    float4 gl_Position : POSITION;
};

VS_Output _main(VS_Input Input)
{
    VS_Output Output = _21;
    float4 pos4 = float4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0f);
    float4 cameraPos = mul(_40_mCamera, pos4);
    Output.PosVS = mul(_40_mProj, cameraPos);
    Output.Color = Input.Color;
    Output.UV = Input.UV;
    Output.UV.y = _40_mUVInversed.x + (_40_mUVInversed.y * Input.UV.y);
    Output.PosP = Output.PosVS;
    return Output;
}

void vert_main()
{
    VS_Input Input;
    Input.Pos = Input_Pos;
    Input.Color = Input_Color;
    Input.UV = Input_UV;
    VS_Output flattenTemp = _main(Input);
    gl_Position = flattenTemp.PosVS;
    _entryPointOutput_Color = flattenTemp.Color;
    _entryPointOutput_UV = flattenTemp.UV;
    _entryPointOutput_PosP = flattenTemp.PosP;
    gl_Position.x = gl_Position.x - gl_HalfPixel.x * gl_Position.w;
    gl_Position.y = gl_Position.y + gl_HalfPixel.y * gl_Position.w;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    Input_Pos = stage_input.Input_Pos;
    Input_Color = stage_input.Input_Color;
    Input_UV = stage_input.Input_UV;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    stage_output._entryPointOutput_Color = _entryPointOutput_Color;
    stage_output._entryPointOutput_UV = _entryPointOutput_UV;
    stage_output._entryPointOutput_PosP = _entryPointOutput_PosP;
    return stage_output;
}
