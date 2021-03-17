struct VS_Input
{
    float3 Pos;
    float4 Color;
    float4 Normal;
    float4 Tangent;
    float2 UV1;
    float2 UV2;
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
    float3 WorldB;
    float3 WorldT;
    float4 Alpha_Dist_UV;
    float4 Blend_Alpha_Dist_UV;
    float4 Blend_FBNextIndex_UV;
};

static const VS_Output _349 = { 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxx, 0.0f.xxx, 0.0f.xxx, 0.0f.xxxx, 0.0f.xxxx, 0.0f.xxxx };

cbuffer VS_ConstantBuffer : register(b0)
{
    column_major float4x4 _255_mCamera : register(c0);
    column_major float4x4 _255_mCameraProj : register(c4);
    float4 _255_mUVInversed : register(c8);
    float4 _255_fFlipbookParameter : register(c9);
};

static const float4 gl_HalfPixel = 0.0f.xxxx;

static float4 gl_Position;
static float3 Input_Pos;
static float4 Input_Color;
static float4 Input_Normal;
static float4 Input_Tangent;
static float2 Input_UV1;
static float2 Input_UV2;
static float4 Input_Alpha_Dist_UV;
static float2 Input_BlendUV;
static float4 Input_Blend_Alpha_Dist_UV;
static float Input_FlipbookIndex;
static float Input_AlphaThreshold;
static float4 _entryPointOutput_Color;
static float4 _entryPointOutput_UV_Others;
static float3 _entryPointOutput_WorldN;
static float3 _entryPointOutput_WorldB;
static float3 _entryPointOutput_WorldT;
static float4 _entryPointOutput_Alpha_Dist_UV;
static float4 _entryPointOutput_Blend_Alpha_Dist_UV;
static float4 _entryPointOutput_Blend_FBNextIndex_UV;

struct SPIRV_Cross_Input
{
    float3 Input_Pos : TEXCOORD0;
    float4 Input_Color : TEXCOORD1;
    float4 Input_Normal : TEXCOORD2;
    float4 Input_Tangent : TEXCOORD3;
    float2 Input_UV1 : TEXCOORD4;
    float2 Input_UV2 : TEXCOORD5;
    float4 Input_Alpha_Dist_UV : TEXCOORD6;
    float2 Input_BlendUV : TEXCOORD7;
    float4 Input_Blend_Alpha_Dist_UV : TEXCOORD8;
    float Input_FlipbookIndex : TEXCOORD9;
    float Input_AlphaThreshold : TEXCOORD10;
};

struct SPIRV_Cross_Output
{
    centroid float4 _entryPointOutput_Color : TEXCOORD0;
    centroid float4 _entryPointOutput_UV_Others : TEXCOORD1;
    float3 _entryPointOutput_WorldN : TEXCOORD2;
    float3 _entryPointOutput_WorldB : TEXCOORD3;
    float3 _entryPointOutput_WorldT : TEXCOORD4;
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

void CalculateAndStoreAdvancedParameter(VS_Input vsinput, inout VS_Output vsoutput)
{
    vsoutput.Alpha_Dist_UV = vsinput.Alpha_Dist_UV;
    vsoutput.Alpha_Dist_UV.y = _255_mUVInversed.x + (_255_mUVInversed.y * vsinput.Alpha_Dist_UV.y);
    vsoutput.Alpha_Dist_UV.w = _255_mUVInversed.x + (_255_mUVInversed.y * vsinput.Alpha_Dist_UV.w);
    vsoutput.Blend_FBNextIndex_UV = float4(vsinput.BlendUV.x, vsinput.BlendUV.y, vsoutput.Blend_FBNextIndex_UV.z, vsoutput.Blend_FBNextIndex_UV.w);
    vsoutput.Blend_FBNextIndex_UV.y = _255_mUVInversed.x + (_255_mUVInversed.y * vsinput.BlendUV.y);
    vsoutput.Blend_Alpha_Dist_UV = vsinput.Blend_Alpha_Dist_UV;
    vsoutput.Blend_Alpha_Dist_UV.y = _255_mUVInversed.x + (_255_mUVInversed.y * vsinput.Blend_Alpha_Dist_UV.y);
    vsoutput.Blend_Alpha_Dist_UV.w = _255_mUVInversed.x + (_255_mUVInversed.y * vsinput.Blend_Alpha_Dist_UV.w);
    float flipbookRate = 0.0f;
    float2 flipbookNextIndexUV = 0.0f.xx;
    float param = flipbookRate;
    float2 param_1 = flipbookNextIndexUV;
    float4 param_2 = _255_fFlipbookParameter;
    float param_3 = vsinput.FlipbookIndex;
    float2 param_4 = vsoutput.UV_Others.xy;
    ApplyFlipbookVS(param, param_1, param_2, param_3, param_4);
    flipbookRate = param;
    flipbookNextIndexUV = param_1;
    vsoutput.Blend_FBNextIndex_UV = float4(vsoutput.Blend_FBNextIndex_UV.x, vsoutput.Blend_FBNextIndex_UV.y, flipbookNextIndexUV.x, flipbookNextIndexUV.y);
    vsoutput.UV_Others.z = flipbookRate;
    vsoutput.UV_Others.w = vsinput.AlphaThreshold;
}

VS_Output _main(VS_Input Input)
{
    VS_Output Output = _349;
    float4 worldNormal = float4((Input.Normal.xyz - 0.5f.xxx) * 2.0f, 0.0f);
    float4 worldTangent = float4((Input.Tangent.xyz - 0.5f.xxx) * 2.0f, 0.0f);
    float4 worldBinormal = float4(cross(worldNormal.xyz, worldTangent.xyz), 0.0f);
    float2 uv1 = Input.UV1;
    uv1.y = _255_mUVInversed.x + (_255_mUVInversed.y * uv1.y);
    Output.UV_Others = float4(uv1.x, uv1.y, Output.UV_Others.z, Output.UV_Others.w);
    float4 worldPos = float4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0f);
    Output.PosVS = mul(_255_mCameraProj, worldPos);
    Output.WorldN = worldNormal.xyz;
    Output.WorldB = worldBinormal.xyz;
    Output.WorldT = worldTangent.xyz;
    Output.Color = Input.Color;
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
    Input.Normal = Input_Normal;
    Input.Tangent = Input_Tangent;
    Input.UV1 = Input_UV1;
    Input.UV2 = Input_UV2;
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
    _entryPointOutput_WorldB = flattenTemp.WorldB;
    _entryPointOutput_WorldT = flattenTemp.WorldT;
    _entryPointOutput_Alpha_Dist_UV = flattenTemp.Alpha_Dist_UV;
    _entryPointOutput_Blend_Alpha_Dist_UV = flattenTemp.Blend_Alpha_Dist_UV;
    _entryPointOutput_Blend_FBNextIndex_UV = flattenTemp.Blend_FBNextIndex_UV;
    gl_Position.x = gl_Position.x - gl_HalfPixel.x * gl_Position.w;
    gl_Position.y = gl_Position.y + gl_HalfPixel.y * gl_Position.w;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    Input_Pos = stage_input.Input_Pos;
    Input_Color = stage_input.Input_Color;
    Input_Normal = stage_input.Input_Normal;
    Input_Tangent = stage_input.Input_Tangent;
    Input_UV1 = stage_input.Input_UV1;
    Input_UV2 = stage_input.Input_UV2;
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
    stage_output._entryPointOutput_WorldB = _entryPointOutput_WorldB;
    stage_output._entryPointOutput_WorldT = _entryPointOutput_WorldT;
    stage_output._entryPointOutput_Alpha_Dist_UV = _entryPointOutput_Alpha_Dist_UV;
    stage_output._entryPointOutput_Blend_Alpha_Dist_UV = _entryPointOutput_Blend_Alpha_Dist_UV;
    stage_output._entryPointOutput_Blend_FBNextIndex_UV = _entryPointOutput_Blend_FBNextIndex_UV;
    return stage_output;
}
