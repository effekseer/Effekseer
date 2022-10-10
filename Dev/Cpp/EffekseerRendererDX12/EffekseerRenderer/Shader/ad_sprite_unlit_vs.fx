struct VS_Input
{
    float3 Pos;
    float4 Color;
    float2 UV;
    float4 Alpha_Dist_UV;
    float2 BlendUV;
    float4 Blend_Alpha_Dist_UV;
    float FlipbookIndex;
    float AlphaThreshold;
};

struct VS_Output
{
    float4 PosVS;
    float4 Color;
    float4 UV_Others;
    float3 WorldN;
    float4 Alpha_Dist_UV;
    float4 Blend_Alpha_Dist_UV;
    float4 Blend_FBNextIndex_UV;
    float4 PosP;
};

static const VS_Output _364 = { 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx };

cbuffer VS_ConstantBuffer : register(b0)
{
    column_major float4x4 _263_mCamera : packoffset(c0);
    column_major float4x4 _263_mCameraProj : packoffset(c4);
    float4 _263_mUVInversed : packoffset(c8);
    float4 _263_flipbookParameter1 : packoffset(c9);
    float4 _263_flipbookParameter2 : packoffset(c10);
};


static float4 gl_Position;
static float3 Input_Pos;
static float4 Input_Color;
static float2 Input_UV;
static float4 Input_Alpha_Dist_UV;
static float2 Input_BlendUV;
static float4 Input_Blend_Alpha_Dist_UV;
static float Input_FlipbookIndex;
static float Input_AlphaThreshold;
static float4 _entryPointOutput_Color;
static float4 _entryPointOutput_UV_Others;
static float3 _entryPointOutput_WorldN;
static float4 _entryPointOutput_Alpha_Dist_UV;
static float4 _entryPointOutput_Blend_Alpha_Dist_UV;
static float4 _entryPointOutput_Blend_FBNextIndex_UV;
static float4 _entryPointOutput_PosP;

struct SPIRV_Cross_Input
{
    float3 Input_Pos : TEXCOORD0;
    float4 Input_Color : TEXCOORD1;
    float2 Input_UV : TEXCOORD2;
    float4 Input_Alpha_Dist_UV : TEXCOORD3;
    float2 Input_BlendUV : TEXCOORD4;
    float4 Input_Blend_Alpha_Dist_UV : TEXCOORD5;
    float Input_FlipbookIndex : TEXCOORD6;
    float Input_AlphaThreshold : TEXCOORD7;
};

struct SPIRV_Cross_Output
{
    centroid float4 _entryPointOutput_Color : TEXCOORD0;
    centroid float4 _entryPointOutput_UV_Others : TEXCOORD1;
    float3 _entryPointOutput_WorldN : TEXCOORD2;
    float4 _entryPointOutput_Alpha_Dist_UV : TEXCOORD3;
    float4 _entryPointOutput_Blend_Alpha_Dist_UV : TEXCOORD4;
    float4 _entryPointOutput_Blend_FBNextIndex_UV : TEXCOORD5;
    float4 _entryPointOutput_PosP : TEXCOORD6;
    float4 gl_Position : SV_Position;
};

float mod(float x, float y)
{
    return x - y * floor(x / y);
}

float2 mod(float2 x, float2 y)
{
    return x - y * floor(x / y);
}

float3 mod(float3 x, float3 y)
{
    return x - y * floor(x / y);
}

float4 mod(float4 x, float4 y)
{
    return x - y * floor(x / y);
}

float2 GetFlipbookOriginUV(float2 FlipbookUV, float FlipbookIndex, float DivideX, float2 flipbookOneSize, float2 flipbookOffset)
{
    float2 DivideIndex;
    DivideIndex.x = float(int(FlipbookIndex) % int(DivideX));
    DivideIndex.y = float(int(FlipbookIndex) / int(DivideX));
    float2 UVOffset = (DivideIndex * flipbookOneSize) + flipbookOffset;
    return FlipbookUV - UVOffset;
}

float2 GetFlipbookUVForIndex(float2 OriginUV, float Index, float DivideX, float2 flipbookOneSize, float2 flipbookOffset)
{
    float2 DivideIndex;
    DivideIndex.x = float(int(Index) % int(DivideX));
    DivideIndex.y = float(int(Index) / int(DivideX));
    return (OriginUV + (DivideIndex * flipbookOneSize)) + flipbookOffset;
}

void ApplyFlipbookVS(inout float flipbookRate, inout float2 flipbookUV, float4 flipbookParameter1, float4 flipbookParameter2, float flipbookIndex, float2 uv, float2 uvInversed)
{
    float flipbookEnabled = flipbookParameter1.x;
    float flipbookLoopType = flipbookParameter1.y;
    float divideX = flipbookParameter1.z;
    float divideY = flipbookParameter1.w;
    float2 flipbookOneSize = flipbookParameter2.xy;
    float2 flipbookOffset = flipbookParameter2.zw;
    if (flipbookEnabled > 0.0f)
    {
        flipbookRate = frac(flipbookIndex);
        float Index = floor(flipbookIndex);
        float IndexOffset = 1.0f;
        float NextIndex = Index + IndexOffset;
        float FlipbookMaxCount = divideX * divideY;
        if (flipbookLoopType == 0.0f)
        {
            if (NextIndex >= FlipbookMaxCount)
            {
                NextIndex = FlipbookMaxCount - 1.0f;
                Index = FlipbookMaxCount - 1.0f;
            }
        }
        else
        {
            if (flipbookLoopType == 1.0f)
            {
                Index = mod(Index, FlipbookMaxCount);
                NextIndex = mod(NextIndex, FlipbookMaxCount);
            }
            else
            {
                if (flipbookLoopType == 2.0f)
                {
                    bool Reverse = mod(floor(Index / FlipbookMaxCount), 2.0f) == 1.0f;
                    Index = mod(Index, FlipbookMaxCount);
                    if (Reverse)
                    {
                        Index = (FlipbookMaxCount - 1.0f) - floor(Index);
                    }
                    Reverse = mod(floor(NextIndex / FlipbookMaxCount), 2.0f) == 1.0f;
                    NextIndex = mod(NextIndex, FlipbookMaxCount);
                    if (Reverse)
                    {
                        NextIndex = (FlipbookMaxCount - 1.0f) - floor(NextIndex);
                    }
                }
            }
        }
        float2 notInversedUV = uv;
        notInversedUV.y = uvInversed.x + (uvInversed.y * notInversedUV.y);
        float2 param = notInversedUV;
        float param_1 = Index;
        float param_2 = divideX;
        float2 param_3 = flipbookOneSize;
        float2 param_4 = flipbookOffset;
        float2 OriginUV = GetFlipbookOriginUV(param, param_1, param_2, param_3, param_4);
        float2 param_5 = OriginUV;
        float param_6 = NextIndex;
        float param_7 = divideX;
        float2 param_8 = flipbookOneSize;
        float2 param_9 = flipbookOffset;
        flipbookUV = GetFlipbookUVForIndex(param_5, param_6, param_7, param_8, param_9);
        flipbookUV.y = uvInversed.x + (uvInversed.y * flipbookUV.y);
    }
}

void CalculateAndStoreAdvancedParameter(VS_Input vsinput, inout VS_Output vsoutput)
{
    vsoutput.Alpha_Dist_UV = vsinput.Alpha_Dist_UV;
    vsoutput.Alpha_Dist_UV.y = _263_mUVInversed.x + (_263_mUVInversed.y * vsinput.Alpha_Dist_UV.y);
    vsoutput.Alpha_Dist_UV.w = _263_mUVInversed.x + (_263_mUVInversed.y * vsinput.Alpha_Dist_UV.w);
    vsoutput.Blend_FBNextIndex_UV = float4(vsinput.BlendUV.x, vsinput.BlendUV.y, vsoutput.Blend_FBNextIndex_UV.z, vsoutput.Blend_FBNextIndex_UV.w);
    vsoutput.Blend_FBNextIndex_UV.y = _263_mUVInversed.x + (_263_mUVInversed.y * vsinput.BlendUV.y);
    vsoutput.Blend_Alpha_Dist_UV = vsinput.Blend_Alpha_Dist_UV;
    vsoutput.Blend_Alpha_Dist_UV.y = _263_mUVInversed.x + (_263_mUVInversed.y * vsinput.Blend_Alpha_Dist_UV.y);
    vsoutput.Blend_Alpha_Dist_UV.w = _263_mUVInversed.x + (_263_mUVInversed.y * vsinput.Blend_Alpha_Dist_UV.w);
    float flipbookRate = 0.0f;
    float2 flipbookNextIndexUV = 0.0f.xx;
    float param = flipbookRate;
    float2 param_1 = flipbookNextIndexUV;
    float4 param_2 = _263_flipbookParameter1;
    float4 param_3 = _263_flipbookParameter2;
    float param_4 = vsinput.FlipbookIndex;
    float2 param_5 = vsoutput.UV_Others.xy;
    float2 param_6 = float2(_263_mUVInversed.xy);
    ApplyFlipbookVS(param, param_1, param_2, param_3, param_4, param_5, param_6);
    flipbookRate = param;
    flipbookNextIndexUV = param_1;
    vsoutput.Blend_FBNextIndex_UV = float4(vsoutput.Blend_FBNextIndex_UV.x, vsoutput.Blend_FBNextIndex_UV.y, flipbookNextIndexUV.x, flipbookNextIndexUV.y);
    vsoutput.UV_Others.z = flipbookRate;
    vsoutput.UV_Others.w = vsinput.AlphaThreshold;
}

VS_Output _main(VS_Input Input)
{
    VS_Output Output = _364;
    float2 uv1 = Input.UV;
    uv1.y = _263_mUVInversed.x + (_263_mUVInversed.y * uv1.y);
    Output.UV_Others = float4(uv1.x, uv1.y, Output.UV_Others.z, Output.UV_Others.w);
    float4 worldPos = float4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0f);
    Output.PosVS = mul(_263_mCameraProj, worldPos);
    Output.Color = Input.Color;
    VS_Input param = Input;
    VS_Output param_1 = Output;
    CalculateAndStoreAdvancedParameter(param, param_1);
    Output = param_1;
    Output.PosP = Output.PosVS;
    return Output;
}

void vert_main()
{
    VS_Input Input;
    Input.Pos = Input_Pos;
    Input.Color = Input_Color;
    Input.UV = Input_UV;
    Input.Alpha_Dist_UV = Input_Alpha_Dist_UV;
    Input.BlendUV = Input_BlendUV;
    Input.Blend_Alpha_Dist_UV = Input_Blend_Alpha_Dist_UV;
    Input.FlipbookIndex = Input_FlipbookIndex;
    Input.AlphaThreshold = Input_AlphaThreshold;
    VS_Output flattenTemp = _main(Input);
    gl_Position = flattenTemp.PosVS;
    _entryPointOutput_Color = flattenTemp.Color;
    _entryPointOutput_UV_Others = flattenTemp.UV_Others;
    _entryPointOutput_WorldN = flattenTemp.WorldN;
    _entryPointOutput_Alpha_Dist_UV = flattenTemp.Alpha_Dist_UV;
    _entryPointOutput_Blend_Alpha_Dist_UV = flattenTemp.Blend_Alpha_Dist_UV;
    _entryPointOutput_Blend_FBNextIndex_UV = flattenTemp.Blend_FBNextIndex_UV;
    _entryPointOutput_PosP = flattenTemp.PosP;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    Input_Pos = stage_input.Input_Pos;
    Input_Color = stage_input.Input_Color;
    Input_UV = stage_input.Input_UV;
    Input_Alpha_Dist_UV = stage_input.Input_Alpha_Dist_UV;
    Input_BlendUV = stage_input.Input_BlendUV;
    Input_Blend_Alpha_Dist_UV = stage_input.Input_Blend_Alpha_Dist_UV;
    Input_FlipbookIndex = stage_input.Input_FlipbookIndex;
    Input_AlphaThreshold = stage_input.Input_AlphaThreshold;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    stage_output._entryPointOutput_Color = _entryPointOutput_Color;
    stage_output._entryPointOutput_UV_Others = _entryPointOutput_UV_Others;
    stage_output._entryPointOutput_WorldN = _entryPointOutput_WorldN;
    stage_output._entryPointOutput_Alpha_Dist_UV = _entryPointOutput_Alpha_Dist_UV;
    stage_output._entryPointOutput_Blend_Alpha_Dist_UV = _entryPointOutput_Blend_Alpha_Dist_UV;
    stage_output._entryPointOutput_Blend_FBNextIndex_UV = _entryPointOutput_Blend_FBNextIndex_UV;
    stage_output._entryPointOutput_PosP = _entryPointOutput_PosP;
    return stage_output;
}
