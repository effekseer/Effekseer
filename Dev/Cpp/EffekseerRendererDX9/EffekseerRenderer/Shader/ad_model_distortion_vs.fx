struct VS_Output
{
    float4 PosVS;
    float4 UV_Others;
    float4 ProjBinormal;
    float4 ProjTangent;
    float4 PosP;
    float4 Color;
    float4 Alpha_Dist_UV;
    float4 Blend_Alpha_Dist_UV;
    float4 Blend_FBNextIndex_UV;
};

struct VS_Input
{
    float3 Pos;
    float3 Normal;
    float3 Binormal;
    float3 Tangent;
    float2 UV;
    float4 Color;
    float Index;
};

static const VS_Output _494 = { 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx };

cbuffer VS_ConstantBuffer : register(b0)
{
    column_major float4x4 _365_mCameraProj : register(c0);
    column_major float4x4 _365_mModel_Inst[10] : register(c4);
    float4 _365_fUV[10] : register(c44);
    float4 _365_fAlphaUV[10] : register(c54);
    float4 _365_fUVDistortionUV[10] : register(c64);
    float4 _365_fBlendUV[10] : register(c74);
    float4 _365_fBlendAlphaUV[10] : register(c84);
    float4 _365_fBlendUVDistortionUV[10] : register(c94);
    float4 _365_fFlipbookParameter : register(c104);
    float4 _365_fFlipbookIndexAndNextRate[10] : register(c105);
    float4 _365_fModelAlphaThreshold[10] : register(c115);
    float4 _365_fModelColor[10] : register(c125);
    float4 _365_fLightDirection : register(c135);
    float4 _365_fLightColor : register(c136);
    float4 _365_fLightAmbient : register(c137);
    float4 _365_mUVInversed : register(c138);
};

static const float4 gl_HalfPixel = 0.0f.xxxx;

static float4 gl_Position;
static float3 Input_Pos;
static float3 Input_Normal;
static float3 Input_Binormal;
static float3 Input_Tangent;
static float2 Input_UV;
static float4 Input_Color;
static float Input_Index;
static float4 _entryPointOutput_UV_Others;
static float4 _entryPointOutput_ProjBinormal;
static float4 _entryPointOutput_ProjTangent;
static float4 _entryPointOutput_PosP;
static float4 _entryPointOutput_Color;
static float4 _entryPointOutput_Alpha_Dist_UV;
static float4 _entryPointOutput_Blend_Alpha_Dist_UV;
static float4 _entryPointOutput_Blend_FBNextIndex_UV;

struct SPIRV_Cross_Input
{
    float3 Input_Pos : TEXCOORD0;
    float3 Input_Normal : TEXCOORD1;
    float3 Input_Binormal : TEXCOORD2;
    float3 Input_Tangent : TEXCOORD3;
    float2 Input_UV : TEXCOORD4;
    float4 Input_Color : TEXCOORD5;
    float Input_Index : TEXCOORD6;
};

struct SPIRV_Cross_Output
{
    centroid float4 _entryPointOutput_UV_Others : TEXCOORD0;
    float4 _entryPointOutput_ProjBinormal : TEXCOORD1;
    float4 _entryPointOutput_ProjTangent : TEXCOORD2;
    float4 _entryPointOutput_PosP : TEXCOORD3;
    centroid float4 _entryPointOutput_Color : TEXCOORD4;
    float4 _entryPointOutput_Alpha_Dist_UV : TEXCOORD5;
    float4 _entryPointOutput_Blend_Alpha_Dist_UV : TEXCOORD6;
    float4 _entryPointOutput_Blend_FBNextIndex_UV : TEXCOORD7;
    float4 gl_Position : POSITION;
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

void CalculateAndStoreAdvancedParameter(float2 uv, float2 uv1, float4 alphaUV, float4 uvDistortionUV, float4 blendUV, float4 blendAlphaUV, float4 blendUVDistortionUV, float flipbookIndexAndNextRate, float modelAlphaThreshold, inout VS_Output vsoutput)
{
    vsoutput.Alpha_Dist_UV.x = (uv.x * alphaUV.z) + alphaUV.x;
    vsoutput.Alpha_Dist_UV.y = (uv.y * alphaUV.w) + alphaUV.y;
    vsoutput.Alpha_Dist_UV.z = (uv.x * uvDistortionUV.z) + uvDistortionUV.x;
    vsoutput.Alpha_Dist_UV.w = (uv.y * uvDistortionUV.w) + uvDistortionUV.y;
    vsoutput.Blend_FBNextIndex_UV.x = (uv.x * blendUV.z) + blendUV.x;
    vsoutput.Blend_FBNextIndex_UV.y = (uv.y * blendUV.w) + blendUV.y;
    vsoutput.Blend_Alpha_Dist_UV.x = (uv.x * blendAlphaUV.z) + blendAlphaUV.x;
    vsoutput.Blend_Alpha_Dist_UV.y = (uv.y * blendAlphaUV.w) + blendAlphaUV.y;
    vsoutput.Blend_Alpha_Dist_UV.z = (uv.x * blendUVDistortionUV.z) + blendUVDistortionUV.x;
    vsoutput.Blend_Alpha_Dist_UV.w = (uv.y * blendUVDistortionUV.w) + blendUVDistortionUV.y;
    float flipbookRate = 0.0f;
    float2 flipbookNextIndexUV = 0.0f.xx;
    float param = flipbookRate;
    float2 param_1 = flipbookNextIndexUV;
    float4 param_2 = _365_fFlipbookParameter;
    float param_3 = flipbookIndexAndNextRate;
    float2 param_4 = uv1;
    ApplyFlipbookVS(param, param_1, param_2, param_3, param_4);
    flipbookRate = param;
    flipbookNextIndexUV = param_1;
    vsoutput.Blend_FBNextIndex_UV = float4(vsoutput.Blend_FBNextIndex_UV.x, vsoutput.Blend_FBNextIndex_UV.y, flipbookNextIndexUV.x, flipbookNextIndexUV.y);
    vsoutput.UV_Others.z = flipbookRate;
    vsoutput.UV_Others.w = modelAlphaThreshold;
    vsoutput.Alpha_Dist_UV.y = _365_mUVInversed.x + (_365_mUVInversed.y * vsoutput.Alpha_Dist_UV.y);
    vsoutput.Alpha_Dist_UV.w = _365_mUVInversed.x + (_365_mUVInversed.y * vsoutput.Alpha_Dist_UV.w);
    vsoutput.Blend_FBNextIndex_UV.y = _365_mUVInversed.x + (_365_mUVInversed.y * vsoutput.Blend_FBNextIndex_UV.y);
    vsoutput.Blend_Alpha_Dist_UV.y = _365_mUVInversed.x + (_365_mUVInversed.y * vsoutput.Blend_Alpha_Dist_UV.y);
    vsoutput.Blend_Alpha_Dist_UV.w = _365_mUVInversed.x + (_365_mUVInversed.y * vsoutput.Blend_Alpha_Dist_UV.w);
}

VS_Output _main(VS_Input Input)
{
    int index = int(Input.Index);
    float4x4 mModel = _365_mModel_Inst[index];
    float4 uv = _365_fUV[index];
    float4 alphaUV = _365_fAlphaUV[index];
    float4 uvDistortionUV = _365_fUVDistortionUV[index];
    float4 blendUV = _365_fBlendUV[index];
    float4 blendAlphaUV = _365_fBlendAlphaUV[index];
    float4 blendUVDistortionUV = _365_fBlendUVDistortionUV[index];
    float4 modelColor = _365_fModelColor[index] * Input.Color;
    float flipbookIndexAndNextRate = _365_fFlipbookIndexAndNextRate[index].x;
    float modelAlphaThreshold = _365_fModelAlphaThreshold[index].x;
    VS_Output Output = _494;
    float4 localPosition = float4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0f);
    float4 worldPos = mul(mModel, localPosition);
    Output.PosVS = mul(_365_mCameraProj, worldPos);
    float2 outputUV = Input.UV;
    outputUV.x = (outputUV.x * uv.z) + uv.x;
    outputUV.y = (outputUV.y * uv.w) + uv.y;
    outputUV.y = _365_mUVInversed.x + (_365_mUVInversed.y * outputUV.y);
    Output.UV_Others = float4(outputUV.x, outputUV.y, Output.UV_Others.z, Output.UV_Others.w);
    float4 localNormal = float4(Input.Normal.x, Input.Normal.y, Input.Normal.z, 0.0f);
    float4 localBinormal = float4(Input.Binormal.x, Input.Binormal.y, Input.Binormal.z, 0.0f);
    float4 localTangent = float4(Input.Tangent.x, Input.Tangent.y, Input.Tangent.z, 0.0f);
    float4 worldNormal = mul(mModel, localNormal);
    float4 worldBinormal = mul(mModel, localBinormal);
    float4 worldTangent = mul(mModel, localTangent);
    worldNormal = normalize(worldNormal);
    worldBinormal = normalize(worldBinormal);
    worldTangent = normalize(worldTangent);
    Output.ProjTangent = mul(_365_mCameraProj, worldPos + worldTangent);
    Output.ProjBinormal = mul(_365_mCameraProj, worldPos + worldBinormal);
    Output.Color = modelColor;
    float2 param = Input.UV;
    float2 param_1 = Output.UV_Others.xy;
    float4 param_2 = alphaUV;
    float4 param_3 = uvDistortionUV;
    float4 param_4 = blendUV;
    float4 param_5 = blendAlphaUV;
    float4 param_6 = blendUVDistortionUV;
    float param_7 = flipbookIndexAndNextRate;
    float param_8 = modelAlphaThreshold;
    VS_Output param_9 = Output;
    CalculateAndStoreAdvancedParameter(param, param_1, param_2, param_3, param_4, param_5, param_6, param_7, param_8, param_9);
    Output = param_9;
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
    Input.Index = Input_Index;
    VS_Output flattenTemp = _main(Input);
    gl_Position = flattenTemp.PosVS;
    _entryPointOutput_UV_Others = flattenTemp.UV_Others;
    _entryPointOutput_ProjBinormal = flattenTemp.ProjBinormal;
    _entryPointOutput_ProjTangent = flattenTemp.ProjTangent;
    _entryPointOutput_PosP = flattenTemp.PosP;
    _entryPointOutput_Color = flattenTemp.Color;
    _entryPointOutput_Alpha_Dist_UV = flattenTemp.Alpha_Dist_UV;
    _entryPointOutput_Blend_Alpha_Dist_UV = flattenTemp.Blend_Alpha_Dist_UV;
    _entryPointOutput_Blend_FBNextIndex_UV = flattenTemp.Blend_FBNextIndex_UV;
    gl_Position.x = gl_Position.x - gl_HalfPixel.x * gl_Position.w;
    gl_Position.y = gl_Position.y + gl_HalfPixel.y * gl_Position.w;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    Input_Pos = stage_input.Input_Pos;
    Input_Normal = stage_input.Input_Normal;
    Input_Binormal = stage_input.Input_Binormal;
    Input_Tangent = stage_input.Input_Tangent;
    Input_UV = stage_input.Input_UV;
    Input_Color = stage_input.Input_Color;
    Input_Index = stage_input.Input_Index;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    stage_output._entryPointOutput_UV_Others = _entryPointOutput_UV_Others;
    stage_output._entryPointOutput_ProjBinormal = _entryPointOutput_ProjBinormal;
    stage_output._entryPointOutput_ProjTangent = _entryPointOutput_ProjTangent;
    stage_output._entryPointOutput_PosP = _entryPointOutput_PosP;
    stage_output._entryPointOutput_Color = _entryPointOutput_Color;
    stage_output._entryPointOutput_Alpha_Dist_UV = _entryPointOutput_Alpha_Dist_UV;
    stage_output._entryPointOutput_Blend_Alpha_Dist_UV = _entryPointOutput_Blend_Alpha_Dist_UV;
    stage_output._entryPointOutput_Blend_FBNextIndex_UV = _entryPointOutput_Blend_FBNextIndex_UV;
    return stage_output;
}
