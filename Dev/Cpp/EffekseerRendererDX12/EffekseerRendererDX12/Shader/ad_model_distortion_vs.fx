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
    float2 OctNormal;
    float2 OctTangent;
    float2 UV1;
    float2 UV2;
    float4 Color;
    uint Index;
};

static const VS_Output _590 = { 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx };

cbuffer VS_ConstantBuffer : register(b0)
{
    column_major float4x4 _451_mCameraProj : packoffset(c0);
    column_major float4x4 _451_mModel_Inst[40] : packoffset(c4);
    float4 _451_fUV[40] : packoffset(c164);
    float4 _451_fAlphaUV[40] : packoffset(c204);
    float4 _451_fUVDistortionUV[40] : packoffset(c244);
    float4 _451_fBlendUV[40] : packoffset(c284);
    float4 _451_fBlendAlphaUV[40] : packoffset(c324);
    float4 _451_fBlendUVDistortionUV[40] : packoffset(c364);
    float4 _451_flipbookParameter1 : packoffset(c404);
    float4 _451_flipbookParameter2 : packoffset(c405);
    float4 _451_fFlipbookIndexAndNextRate[40] : packoffset(c406);
    float4 _451_fModelAlphaThreshold[40] : packoffset(c446);
    float4 _451_fModelColor[40] : packoffset(c486);
    float4 _451_fLightDirection : packoffset(c526);
    float4 _451_fLightColor : packoffset(c527);
    float4 _451_fLightAmbient : packoffset(c528);
    float4 _451_mUVInversed : packoffset(c529);
};


static float4 gl_Position;
static int gl_InstanceIndex;
static float3 Input_Pos;
static float2 Input_OctNormal;
static float2 Input_OctTangent;
static float2 Input_UV1;
static float2 Input_UV2;
static float4 Input_Color;
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
    float2 Input_OctNormal : TEXCOORD1;
    float2 Input_OctTangent : TEXCOORD2;
    float2 Input_UV1 : TEXCOORD3;
    float2 Input_UV2 : TEXCOORD4;
    float4 Input_Color : TEXCOORD5;
    uint gl_InstanceIndex : SV_InstanceID;
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
    float3 _96 = v;
    float3 _101 = v;
    float2 _103 = _101.xy + ((-sign(_96.xy)) * t);
    v.x = _103.x;
    v.y = _103.y;
    return normalize(v);
}

void decodeOct(inout float2 octNormal, inout float2 octTangent, inout float3 normal, out float3 binormal, inout float3 tangent)
{
    octNormal = (octNormal * 2.0f) - 1.0f.xx;
    float2 param = octNormal;
    normal = decodeOct(param);
    octTangent = (octTangent * 2.0f) - 1.0f.xx;
    float s = sign(octTangent.y);
    octTangent = float2(octTangent.x, (abs(octTangent.y) * 2.0f) - 1.0f);
    float2 param_1 = octTangent;
    tangent = decodeOct(param_1);
    binormal = cross(normal, tangent) * s;
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
    float4 param_2 = _451_flipbookParameter1;
    float4 param_3 = _451_flipbookParameter2;
    float param_4 = flipbookIndexAndNextRate;
    float2 param_5 = uv1;
    float2 param_6 = float2(_451_mUVInversed.xy);
    ApplyFlipbookVS(param, param_1, param_2, param_3, param_4, param_5, param_6);
    flipbookRate = param;
    flipbookNextIndexUV = param_1;
    vsoutput.Blend_FBNextIndex_UV.z = flipbookNextIndexUV.x;
    vsoutput.Blend_FBNextIndex_UV.w = flipbookNextIndexUV.y;
    vsoutput.UV_Others.z = flipbookRate;
    vsoutput.UV_Others.w = modelAlphaThreshold;
    vsoutput.Alpha_Dist_UV.y = _451_mUVInversed.x + (_451_mUVInversed.y * vsoutput.Alpha_Dist_UV.y);
    vsoutput.Alpha_Dist_UV.w = _451_mUVInversed.x + (_451_mUVInversed.y * vsoutput.Alpha_Dist_UV.w);
    vsoutput.Blend_FBNextIndex_UV.y = _451_mUVInversed.x + (_451_mUVInversed.y * vsoutput.Blend_FBNextIndex_UV.y);
    vsoutput.Blend_Alpha_Dist_UV.y = _451_mUVInversed.x + (_451_mUVInversed.y * vsoutput.Blend_Alpha_Dist_UV.y);
    vsoutput.Blend_Alpha_Dist_UV.w = _451_mUVInversed.x + (_451_mUVInversed.y * vsoutput.Blend_Alpha_Dist_UV.w);
}

VS_Output _main(VS_Input Input)
{
    uint index = Input.Index;
    float4x4 mModel = _451_mModel_Inst[index];
    float4 uv = _451_fUV[index];
    float4 alphaUV = _451_fAlphaUV[index];
    float4 uvDistortionUV = _451_fUVDistortionUV[index];
    float4 blendUV = _451_fBlendUV[index];
    float4 blendAlphaUV = _451_fBlendAlphaUV[index];
    float4 blendUVDistortionUV = _451_fBlendUVDistortionUV[index];
    float4 modelColor = _451_fModelColor[index] * Input.Color;
    float flipbookIndexAndNextRate = _451_fFlipbookIndexAndNextRate[index].x;
    float modelAlphaThreshold = _451_fModelAlphaThreshold[index].x;
    VS_Output Output = _590;
    float4 localPosition = float4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0f);
    float4 worldPos = mul(mModel, localPosition);
    Output.PosVS = mul(_451_mCameraProj, worldPos);
    float2 outputUV = Input.UV1;
    outputUV.x = (outputUV.x * uv.z) + uv.x;
    outputUV.y = (outputUV.y * uv.w) + uv.y;
    outputUV.y = _451_mUVInversed.x + (_451_mUVInversed.y * outputUV.y);
    Output.UV_Others.x = outputUV.x;
    Output.UV_Others.y = outputUV.y;
    float2 param = Input.OctNormal;
    float2 param_1 = Input.OctTangent;
    float3 param_2;
    float3 param_3;
    float3 param_4;
    decodeOct(param, param_1, param_2, param_3, param_4);
    float3 localNormal = param_2;
    float3 localBinormal = param_3;
    float3 localTangent = param_4;
    float4 worldNormal = mul(mModel, float4(localNormal, 0.0f));
    float4 worldBinormal = mul(mModel, float4(localBinormal, 0.0f));
    float4 worldTangent = mul(mModel, float4(localTangent, 0.0f));
    worldNormal = normalize(worldNormal);
    worldBinormal = normalize(worldBinormal);
    worldTangent = normalize(worldTangent);
    Output.ProjTangent = mul(_451_mCameraProj, worldPos + worldTangent);
    Output.ProjBinormal = mul(_451_mCameraProj, worldPos + worldBinormal);
    Output.Color = modelColor;
    float2 param_5 = Input.UV1;
    float2 param_6 = Output.UV_Others.xy;
    float4 param_7 = alphaUV;
    float4 param_8 = uvDistortionUV;
    float4 param_9 = blendUV;
    float4 param_10 = blendAlphaUV;
    float4 param_11 = blendUVDistortionUV;
    float param_12 = flipbookIndexAndNextRate;
    float param_13 = modelAlphaThreshold;
    VS_Output param_14 = Output;
    CalculateAndStoreAdvancedParameter(param_5, param_6, param_7, param_8, param_9, param_10, param_11, param_12, param_13, param_14);
    Output = param_14;
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
    _entryPointOutput_UV_Others = flattenTemp.UV_Others;
    _entryPointOutput_ProjBinormal = flattenTemp.ProjBinormal;
    _entryPointOutput_ProjTangent = flattenTemp.ProjTangent;
    _entryPointOutput_PosP = flattenTemp.PosP;
    _entryPointOutput_Color = flattenTemp.Color;
    _entryPointOutput_Alpha_Dist_UV = flattenTemp.Alpha_Dist_UV;
    _entryPointOutput_Blend_Alpha_Dist_UV = flattenTemp.Blend_Alpha_Dist_UV;
    _entryPointOutput_Blend_FBNextIndex_UV = flattenTemp.Blend_FBNextIndex_UV;
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
