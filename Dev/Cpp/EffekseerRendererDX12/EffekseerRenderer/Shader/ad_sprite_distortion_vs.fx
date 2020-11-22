struct VS_Input
{
    float3 Pos;
    float4 Color;
    float2 UV;
    float3 Binormal;
    float3 Tangent;
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
    float2 UV;
    float4 PosP;
    float4 PosU;
    float4 PosR;
    float4 Alpha_Dist_UV;
    float4 Blend_Alpha_Dist_UV;
    float4 Blend_FBNextIndex_UV;
    float2 Others;
};

static const VS_Output _347 = { 0.0f.xxxx, 0.0f.xxxx, 0.0f.xx, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xx };

cbuffer VS_ConstantBuffer : register(b0)
{
    column_major float4x4 _256_mCamera : packoffset(c0);
    column_major float4x4 _256_mProj : packoffset(c4);
    float4 _256_mUVInversed : packoffset(c8);
    float4 _256_mflipbookParameter : packoffset(c9);
};


static float4 gl_Position;
static float3 Input_Pos;
static float4 Input_Color;
static float2 Input_UV;
static float3 Input_Binormal;
static float3 Input_Tangent;
static float4 Input_Alpha_Dist_UV;
static float2 Input_BlendUV;
static float4 Input_Blend_Alpha_Dist_UV;
static float Input_FlipbookIndex;
static float Input_AlphaThreshold;
static float4 _entryPointOutput_Color;
static float2 _entryPointOutput_UV;
static float4 _entryPointOutput_PosP;
static float4 _entryPointOutput_PosU;
static float4 _entryPointOutput_PosR;
static float4 _entryPointOutput_Alpha_Dist_UV;
static float4 _entryPointOutput_Blend_Alpha_Dist_UV;
static float4 _entryPointOutput_Blend_FBNextIndex_UV;
static float2 _entryPointOutput_Others;

struct SPIRV_Cross_Input
{
    float3 Input_Pos : TEXCOORD0;
    float4 Input_Color : TEXCOORD1;
    float2 Input_UV : TEXCOORD2;
    float3 Input_Binormal : TEXCOORD3;
    float3 Input_Tangent : TEXCOORD4;
    float4 Input_Alpha_Dist_UV : TEXCOORD5;
    float2 Input_BlendUV : TEXCOORD6;
    float4 Input_Blend_Alpha_Dist_UV : TEXCOORD7;
    float Input_FlipbookIndex : TEXCOORD8;
    float Input_AlphaThreshold : TEXCOORD9;
};

struct SPIRV_Cross_Output
{
    centroid float4 _entryPointOutput_Color : TEXCOORD0;
    centroid float2 _entryPointOutput_UV : TEXCOORD1;
    float4 _entryPointOutput_PosP : TEXCOORD2;
    float4 _entryPointOutput_PosU : TEXCOORD3;
    float4 _entryPointOutput_PosR : TEXCOORD4;
    float4 _entryPointOutput_Alpha_Dist_UV : TEXCOORD5;
    float4 _entryPointOutput_Blend_Alpha_Dist_UV : TEXCOORD6;
    float4 _entryPointOutput_Blend_FBNextIndex_UV : TEXCOORD7;
    float2 _entryPointOutput_Others : TEXCOORD8;
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

float2 GetFlipbookOneSizeUV(float DivideX, float DivideY)
{
    return 1.0f.xx / float2(DivideX, DivideY);
}

float2 GetFlipbookOriginUV(float2 FlipbookUV, float FlipbookIndex, float DivideX, float DivideY)
{
    float2 DivideIndex;
    DivideIndex.x = float(int(FlipbookIndex) % int(DivideX));
    DivideIndex.y = float(int(FlipbookIndex) / int(DivideX));
    float param = DivideX;
    float param_1 = DivideY;
    float2 FlipbookOneSize = GetFlipbookOneSizeUV(param, param_1);
    float2 UVOffset = DivideIndex * FlipbookOneSize;
    float2 OriginUV = FlipbookUV - UVOffset;
    OriginUV *= float2(DivideX, DivideY);
    return OriginUV;
}

float2 GetFlipbookUVForIndex(float2 OriginUV, float Index, float DivideX, float DivideY)
{
    float2 DivideIndex;
    DivideIndex.x = float(int(Index) % int(DivideX));
    DivideIndex.y = float(int(Index) / int(DivideX));
    float param = DivideX;
    float param_1 = DivideY;
    float2 FlipbookOneSize = GetFlipbookOneSizeUV(param, param_1);
    return (OriginUV * FlipbookOneSize) + (DivideIndex * FlipbookOneSize);
}

void ApplyFlipbookVS(inout float flipbookRate, inout float2 flipbookUV, float4 flipbookParameter, float flipbookIndex, float2 uv)
{
    if (flipbookParameter.x > 0.0f)
    {
        flipbookRate = frac(flipbookIndex);
        float Index = floor(flipbookIndex);
        float IndexOffset = 1.0f;
        float NextIndex = Index + IndexOffset;
        float FlipbookMaxCount = flipbookParameter.z * flipbookParameter.w;
        if (flipbookParameter.y == 0.0f)
        {
            if (NextIndex >= FlipbookMaxCount)
            {
                NextIndex = FlipbookMaxCount - 1.0f;
                Index = FlipbookMaxCount - 1.0f;
            }
        }
        else
        {
            if (flipbookParameter.y == 1.0f)
            {
                Index = mod(Index, FlipbookMaxCount);
                NextIndex = mod(NextIndex, FlipbookMaxCount);
            }
            else
            {
                if (flipbookParameter.y == 2.0f)
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
        float2 param = uv;
        float param_1 = Index;
        float param_2 = flipbookParameter.z;
        float param_3 = flipbookParameter.w;
        float2 OriginUV = GetFlipbookOriginUV(param, param_1, param_2, param_3);
        float2 param_4 = OriginUV;
        float param_5 = NextIndex;
        float param_6 = flipbookParameter.z;
        float param_7 = flipbookParameter.w;
        flipbookUV = GetFlipbookUVForIndex(param_4, param_5, param_6, param_7);
    }
}

void CalculateAndStoreAdvancedParameter(VS_Input vsinput, inout VS_Output vsoutput)
{
    vsoutput.Alpha_Dist_UV = vsinput.Alpha_Dist_UV;
    vsoutput.Alpha_Dist_UV.y = _256_mUVInversed.x + (_256_mUVInversed.y * vsinput.Alpha_Dist_UV.y);
    vsoutput.Alpha_Dist_UV.w = _256_mUVInversed.x + (_256_mUVInversed.y * vsinput.Alpha_Dist_UV.w);
    vsoutput.Blend_FBNextIndex_UV = float4(vsinput.BlendUV.x, vsinput.BlendUV.y, vsoutput.Blend_FBNextIndex_UV.z, vsoutput.Blend_FBNextIndex_UV.w);
    vsoutput.Blend_FBNextIndex_UV.y = _256_mUVInversed.x + (_256_mUVInversed.y * vsinput.BlendUV.y);
    vsoutput.Blend_Alpha_Dist_UV = vsinput.Blend_Alpha_Dist_UV;
    vsoutput.Blend_Alpha_Dist_UV.y = _256_mUVInversed.x + (_256_mUVInversed.y * vsinput.Blend_Alpha_Dist_UV.y);
    vsoutput.Blend_Alpha_Dist_UV.w = _256_mUVInversed.x + (_256_mUVInversed.y * vsinput.Blend_Alpha_Dist_UV.w);
    float flipbookRate = 0.0f;
    float2 flipbookNextIndexUV = 0.0f.xx;
    float param = flipbookRate;
    float2 param_1 = flipbookNextIndexUV;
    float4 param_2 = _256_mflipbookParameter;
    float param_3 = vsinput.FlipbookIndex;
    float2 param_4 = vsoutput.UV;
    ApplyFlipbookVS(param, param_1, param_2, param_3, param_4);
    flipbookRate = param;
    flipbookNextIndexUV = param_1;
    vsoutput.Blend_FBNextIndex_UV = float4(vsoutput.Blend_FBNextIndex_UV.x, vsoutput.Blend_FBNextIndex_UV.y, flipbookNextIndexUV.x, flipbookNextIndexUV.y);
    vsoutput.Others.x = flipbookRate;
    vsoutput.Others.y = vsinput.AlphaThreshold;
}

VS_Output _main(VS_Input Input)
{
    VS_Output Output = _347;
    float4 pos4 = float4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0f);
    float4 localBinormal = float4(Input.Pos.x + Input.Binormal.x, Input.Pos.y + Input.Binormal.y, Input.Pos.z + Input.Binormal.z, 1.0f);
    float4 localTangent = float4(Input.Pos.x + Input.Tangent.x, Input.Pos.y + Input.Tangent.y, Input.Pos.z + Input.Tangent.z, 1.0f);
    localBinormal = mul(_256_mCamera, localBinormal);
    localTangent = mul(_256_mCamera, localTangent);
    float4 cameraPos = mul(_256_mCamera, pos4);
    cameraPos /= cameraPos.w.xxxx;
    localBinormal /= localBinormal.w.xxxx;
    localTangent /= localTangent.w.xxxx;
    localBinormal = cameraPos + normalize(localBinormal - cameraPos);
    localTangent = cameraPos + normalize(localTangent - cameraPos);
    Output.PosVS = mul(_256_mProj, cameraPos);
    Output.PosP = Output.PosVS;
    Output.PosU = mul(_256_mProj, localBinormal);
    Output.PosR = mul(_256_mProj, localTangent);
    Output.PosU /= Output.PosU.w.xxxx;
    Output.PosR /= Output.PosR.w.xxxx;
    Output.PosP /= Output.PosP.w.xxxx;
    Output.Color = Input.Color;
    Output.UV = Input.UV;
    Output.UV.y = _256_mUVInversed.x + (_256_mUVInversed.y * Input.UV.y);
    VS_Input param = Input;
    VS_Output param_1 = Output;
    CalculateAndStoreAdvancedParameter(param, param_1);
    Output = param_1;
    return Output;
}

void vert_main()
{
    VS_Input Input;
    Input.Pos = Input_Pos;
    Input.Color = Input_Color;
    Input.UV = Input_UV;
    Input.Binormal = Input_Binormal;
    Input.Tangent = Input_Tangent;
    Input.Alpha_Dist_UV = Input_Alpha_Dist_UV;
    Input.BlendUV = Input_BlendUV;
    Input.Blend_Alpha_Dist_UV = Input_Blend_Alpha_Dist_UV;
    Input.FlipbookIndex = Input_FlipbookIndex;
    Input.AlphaThreshold = Input_AlphaThreshold;
    VS_Output flattenTemp = _main(Input);
    gl_Position = flattenTemp.PosVS;
    _entryPointOutput_Color = flattenTemp.Color;
    _entryPointOutput_UV = flattenTemp.UV;
    _entryPointOutput_PosP = flattenTemp.PosP;
    _entryPointOutput_PosU = flattenTemp.PosU;
    _entryPointOutput_PosR = flattenTemp.PosR;
    _entryPointOutput_Alpha_Dist_UV = flattenTemp.Alpha_Dist_UV;
    _entryPointOutput_Blend_Alpha_Dist_UV = flattenTemp.Blend_Alpha_Dist_UV;
    _entryPointOutput_Blend_FBNextIndex_UV = flattenTemp.Blend_FBNextIndex_UV;
    _entryPointOutput_Others = flattenTemp.Others;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    Input_Pos = stage_input.Input_Pos;
    Input_Color = stage_input.Input_Color;
    Input_UV = stage_input.Input_UV;
    Input_Binormal = stage_input.Input_Binormal;
    Input_Tangent = stage_input.Input_Tangent;
    Input_Alpha_Dist_UV = stage_input.Input_Alpha_Dist_UV;
    Input_BlendUV = stage_input.Input_BlendUV;
    Input_Blend_Alpha_Dist_UV = stage_input.Input_Blend_Alpha_Dist_UV;
    Input_FlipbookIndex = stage_input.Input_FlipbookIndex;
    Input_AlphaThreshold = stage_input.Input_AlphaThreshold;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    stage_output._entryPointOutput_Color = _entryPointOutput_Color;
    stage_output._entryPointOutput_UV = _entryPointOutput_UV;
    stage_output._entryPointOutput_PosP = _entryPointOutput_PosP;
    stage_output._entryPointOutput_PosU = _entryPointOutput_PosU;
    stage_output._entryPointOutput_PosR = _entryPointOutput_PosR;
    stage_output._entryPointOutput_Alpha_Dist_UV = _entryPointOutput_Alpha_Dist_UV;
    stage_output._entryPointOutput_Blend_Alpha_Dist_UV = _entryPointOutput_Blend_Alpha_Dist_UV;
    stage_output._entryPointOutput_Blend_FBNextIndex_UV = _entryPointOutput_Blend_FBNextIndex_UV;
    stage_output._entryPointOutput_Others = _entryPointOutput_Others;
    return stage_output;
}
