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

struct VS_Input
{
    float3 Pos;
    float2 OctNormal;
    float2 OctTangent;
    float2 UV1;
    float2 UV2;
    float4 Color;
    uint Index;
};

static const VS_Output _552 = { 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx };

cbuffer VS_ConstantBuffer : register(b0)
{
    column_major float4x4 _412_mCameraProj : packoffset(c0);
    column_major float4x4 _412_mModel_Inst[40] : packoffset(c4);
    float4 _412_fUV[40] : packoffset(c164);
    float4 _412_fAlphaUV[40] : packoffset(c204);
    float4 _412_fUVDistortionUV[40] : packoffset(c244);
    float4 _412_fBlendUV[40] : packoffset(c284);
    float4 _412_fBlendAlphaUV[40] : packoffset(c324);
    float4 _412_fBlendUVDistortionUV[40] : packoffset(c364);
    float4 _412_flipbookParameter1 : packoffset(c404);
    float4 _412_flipbookParameter2 : packoffset(c405);
    float4 _412_fFlipbookIndexAndNextRate[40] : packoffset(c406);
    float4 _412_fModelAlphaThreshold[40] : packoffset(c446);
    float4 _412_fModelColor[40] : packoffset(c486);
    float4 _412_fLightDirection : packoffset(c526);
    float4 _412_fLightColor : packoffset(c527);
    float4 _412_fLightAmbient : packoffset(c528);
    float4 _412_mUVInversed : packoffset(c529);
};


static float4 gl_Position;
static int gl_InstanceIndex;
static float3 Input_Pos;
static float2 Input_OctNormal;
static float2 Input_OctTangent;
static float2 Input_UV1;
static float2 Input_UV2;
static float4 Input_Color;
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
    float2 Input_OctNormal : TEXCOORD1;
    float2 Input_OctTangent : TEXCOORD2;
    float2 Input_UV1 : TEXCOORD3;
    float2 Input_UV2 : TEXCOORD4;
    float4 Input_Color : TEXCOORD5;
    uint gl_InstanceIndex : SV_InstanceID;
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

float3 decodeOct(float2 oct)
{
    float3 v = float3(oct, (1.0f - abs(oct.x)) - abs(oct.y));
    float t = max(-v.z, 0.0f);
    float3 _88 = v;
    float3 _93 = v;
    float2 _95 = _93.xy + ((-sign(_88.xy)) * t);
    v.x = _95.x;
    v.y = _95.y;
    return normalize(v);
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
    float4 param_2 = _412_flipbookParameter1;
    float4 param_3 = _412_flipbookParameter2;
    float param_4 = flipbookIndexAndNextRate;
    float2 param_5 = uv1;
    float2 param_6 = float2(_412_mUVInversed.xy);
    ApplyFlipbookVS(param, param_1, param_2, param_3, param_4, param_5, param_6);
    flipbookRate = param;
    flipbookNextIndexUV = param_1;
    vsoutput.Blend_FBNextIndex_UV.z = flipbookNextIndexUV.x;
    vsoutput.Blend_FBNextIndex_UV.w = flipbookNextIndexUV.y;
    vsoutput.UV_Others.z = flipbookRate;
    vsoutput.UV_Others.w = modelAlphaThreshold;
    vsoutput.Alpha_Dist_UV.y = _412_mUVInversed.x + (_412_mUVInversed.y * vsoutput.Alpha_Dist_UV.y);
    vsoutput.Alpha_Dist_UV.w = _412_mUVInversed.x + (_412_mUVInversed.y * vsoutput.Alpha_Dist_UV.w);
    vsoutput.Blend_FBNextIndex_UV.y = _412_mUVInversed.x + (_412_mUVInversed.y * vsoutput.Blend_FBNextIndex_UV.y);
    vsoutput.Blend_Alpha_Dist_UV.y = _412_mUVInversed.x + (_412_mUVInversed.y * vsoutput.Blend_Alpha_Dist_UV.y);
    vsoutput.Blend_Alpha_Dist_UV.w = _412_mUVInversed.x + (_412_mUVInversed.y * vsoutput.Blend_Alpha_Dist_UV.w);
}

VS_Output _main(VS_Input Input)
{
    uint index = Input.Index;
    float4x4 mModel = _412_mModel_Inst[index];
    float4 uv = _412_fUV[index];
    float4 alphaUV = _412_fAlphaUV[index];
    float4 uvDistortionUV = _412_fUVDistortionUV[index];
    float4 blendUV = _412_fBlendUV[index];
    float4 blendAlphaUV = _412_fBlendAlphaUV[index];
    float4 blendUVDistortionUV = _412_fBlendUVDistortionUV[index];
    float4 modelColor = _412_fModelColor[index] * Input.Color;
    float flipbookIndexAndNextRate = _412_fFlipbookIndexAndNextRate[index].x;
    float modelAlphaThreshold = _412_fModelAlphaThreshold[index].x;
    VS_Output Output = _552;
    float4 localPosition = float4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0f);
    float4 worldPos = mul(mModel, localPosition);
    Output.PosVS = mul(_412_mCameraProj, worldPos);
    float2 outputUV = Input.UV1;
    outputUV.x = (outputUV.x * uv.z) + uv.x;
    outputUV.y = (outputUV.y * uv.w) + uv.y;
    outputUV.y = _412_mUVInversed.x + (_412_mUVInversed.y * outputUV.y);
    Output.UV_Others.x = outputUV.x;
    Output.UV_Others.y = outputUV.y;
    float2 param = Input.OctNormal;
    float3 localNormal = decodeOct(param);
    float4 worldNormal = normalize(mul(mModel, float4(localNormal, 0.0f)));
    Output.WorldN = localNormal;
    Output.Color = modelColor;
    float2 param_1 = Input.UV1;
    float2 param_2 = Output.UV_Others.xy;
    float4 param_3 = alphaUV;
    float4 param_4 = uvDistortionUV;
    float4 param_5 = blendUV;
    float4 param_6 = blendAlphaUV;
    float4 param_7 = blendUVDistortionUV;
    float param_8 = flipbookIndexAndNextRate;
    float param_9 = modelAlphaThreshold;
    VS_Output param_10 = Output;
    CalculateAndStoreAdvancedParameter(param_1, param_2, param_3, param_4, param_5, param_6, param_7, param_8, param_9, param_10);
    Output = param_10;
    Output.PosP = Output.PosVS;
    return Output;
}

void vert_main()
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
    gl_InstanceIndex = int(stage_input.gl_InstanceIndex);
    Input_Pos = stage_input.Input_Pos;
    Input_OctNormal = stage_input.Input_OctNormal;
    Input_OctTangent = stage_input.Input_OctTangent;
    Input_UV1 = stage_input.Input_UV1;
    Input_UV2 = stage_input.Input_UV2;
    Input_Color = stage_input.Input_Color;
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
