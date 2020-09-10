struct VS_Input
{
    float3 Pos;
    float3 Normal;
    float3 Binormal;
    float3 Tangent;
    float2 UV;
    float4 Color;
    uint Index;
};

struct VS_Output
{
    float4 Pos;
    float2 UV;
    float4 Color;
};

static const VS_Output _54 = { 0.0f.xxxx, 0.0f.xx, 0.0f.xxxx };

cbuffer VS_ConstantBuffer : register(b0)
{
    column_major float4x4 _26_mCameraProj : packoffset(c0);
    column_major float4x4 _26_mModel[10] : packoffset(c4);
    float4 _26_fUV[10] : packoffset(c44);
    float4 _26_fModelColor[10] : packoffset(c54);
    float4 _26_fLightDirection : packoffset(c64);
    float4 _26_fLightColor : packoffset(c65);
    float4 _26_fLightAmbient : packoffset(c66);
    float4 _26_mUVInversed : packoffset(c67);
};


static float4 gl_Position;
static int gl_InstanceIndex;
static float3 Input_Pos;
static float3 Input_Normal;
static float3 Input_Binormal;
static float3 Input_Tangent;
static float2 Input_UV;
static float4 Input_Color;
static float2 _entryPointOutput_UV;
static float4 _entryPointOutput_Color;

struct SPIRV_Cross_Input
{
    float3 Input_Pos : TEXCOORD0;
    float3 Input_Normal : TEXCOORD1;
    float3 Input_Binormal : TEXCOORD2;
    float3 Input_Tangent : TEXCOORD3;
    float2 Input_UV : TEXCOORD4;
    float4 Input_Color : TEXCOORD5;
    uint gl_InstanceIndex : SV_InstanceID;
};

struct SPIRV_Cross_Output
{
    float2 _entryPointOutput_UV : TEXCOORD0;
    float4 _entryPointOutput_Color : TEXCOORD1;
    float4 gl_Position : SV_Position;
};

VS_Output _main(VS_Input Input)
{
    float4x4 matModel = _26_mModel[Input.Index];
    float4 uv = _26_fUV[Input.Index];
    float4 modelColor = _26_fModelColor[Input.Index] * Input.Color;
    VS_Output Output = _54;
    float4 localPosition = float4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0f);
    float4 cameraPosition = mul(matModel, localPosition);
    Output.Pos = mul(_26_mCameraProj, cameraPosition);
    Output.Color = modelColor;
    Output.UV.x = (Input.UV.x * uv.z) + uv.x;
    Output.UV.y = (Input.UV.y * uv.w) + uv.y;
    Output.UV.y = _26_mUVInversed.x + (_26_mUVInversed.y * Output.UV.y);
    return Output;
}

void vert_main()
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
    gl_Position = flattenTemp.Pos;
    _entryPointOutput_UV = flattenTemp.UV;
    _entryPointOutput_Color = flattenTemp.Color;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_InstanceIndex = int(stage_input.gl_InstanceIndex);
    Input_Pos = stage_input.Input_Pos;
    Input_Normal = stage_input.Input_Normal;
    Input_Binormal = stage_input.Input_Binormal;
    Input_Tangent = stage_input.Input_Tangent;
    Input_UV = stage_input.Input_UV;
    Input_Color = stage_input.Input_Color;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    stage_output._entryPointOutput_UV = _entryPointOutput_UV;
    stage_output._entryPointOutput_Color = _entryPointOutput_Color;
    return stage_output;
}
