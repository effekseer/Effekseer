#version 300 es

struct VS_Input
{
    vec3 Pos;
    vec4 Color;
    vec2 UV;
    vec4 Alpha_Dist_UV;
    vec2 BlendUV;
    vec4 Blend_Alpha_Dist_UV;
    float FlipbookIndex;
    float AlphaThreshold;
};

struct VS_Output
{
    vec4 PosVS;
    vec4 Color;
    vec4 UV_Others;
    vec3 WorldN;
    vec4 Alpha_Dist_UV;
    vec4 Blend_Alpha_Dist_UV;
    vec4 Blend_FBNextIndex_UV;
    vec4 PosP;
};

struct VS_ConstantBuffer
{
    mat4 mCamera;
    mat4 mCameraProj;
    vec4 mUVInversed;
    vec4 flipbookParameter1;
    vec4 flipbookParameter2;
};

uniform VS_ConstantBuffer CBVS0;

layout(location = 0) in vec3 Input_Pos;
layout(location = 1) in vec4 Input_Color;
layout(location = 2) in vec2 Input_UV;
layout(location = 3) in vec4 Input_Alpha_Dist_UV;
layout(location = 4) in vec2 Input_BlendUV;
layout(location = 5) in vec4 Input_Blend_Alpha_Dist_UV;
layout(location = 6) in float Input_FlipbookIndex;
layout(location = 7) in float Input_AlphaThreshold;
centroid out vec4 _VSPS_Color;
centroid out vec4 _VSPS_UV_Others;
out vec3 _VSPS_WorldN;
out vec4 _VSPS_Alpha_Dist_UV;
out vec4 _VSPS_Blend_Alpha_Dist_UV;
out vec4 _VSPS_Blend_FBNextIndex_UV;
out vec4 _VSPS_PosP;

vec2 GetFlipbookOriginUV(vec2 FlipbookUV, float FlipbookIndex, float DivideX, vec2 flipbookOneSize, vec2 flipbookOffset)
{
    vec2 DivideIndex;
    DivideIndex.x = float(int(FlipbookIndex) % int(DivideX));
    DivideIndex.y = float(int(FlipbookIndex) / int(DivideX));
    vec2 UVOffset = (DivideIndex * flipbookOneSize) + flipbookOffset;
    return FlipbookUV - UVOffset;
}

vec2 GetFlipbookUVForIndex(vec2 OriginUV, float Index, float DivideX, vec2 flipbookOneSize, vec2 flipbookOffset)
{
    vec2 DivideIndex;
    DivideIndex.x = float(int(Index) % int(DivideX));
    DivideIndex.y = float(int(Index) / int(DivideX));
    return (OriginUV + (DivideIndex * flipbookOneSize)) + flipbookOffset;
}

void ApplyFlipbookVS(inout float flipbookRate, inout vec2 flipbookUV, vec4 flipbookParameter1, vec4 flipbookParameter2, float flipbookIndex, vec2 uv, vec2 uvInversed)
{
    float flipbookEnabled = flipbookParameter1.x;
    float flipbookLoopType = flipbookParameter1.y;
    float divideX = flipbookParameter1.z;
    float divideY = flipbookParameter1.w;
    vec2 flipbookOneSize = flipbookParameter2.xy;
    vec2 flipbookOffset = flipbookParameter2.zw;
    if (flipbookEnabled > 0.0)
    {
        flipbookRate = fract(flipbookIndex);
        float Index = floor(flipbookIndex);
        float IndexOffset = 1.0;
        float NextIndex = Index + IndexOffset;
        float FlipbookMaxCount = divideX * divideY;
        if (flipbookLoopType == 0.0)
        {
            if (NextIndex >= FlipbookMaxCount)
            {
                NextIndex = FlipbookMaxCount - 1.0;
                Index = FlipbookMaxCount - 1.0;
            }
        }
        else
        {
            if (flipbookLoopType == 1.0)
            {
                Index = mod(Index, FlipbookMaxCount);
                NextIndex = mod(NextIndex, FlipbookMaxCount);
            }
            else
            {
                if (flipbookLoopType == 2.0)
                {
                    bool Reverse = mod(floor(Index / FlipbookMaxCount), 2.0) == 1.0;
                    Index = mod(Index, FlipbookMaxCount);
                    if (Reverse)
                    {
                        Index = (FlipbookMaxCount - 1.0) - floor(Index);
                    }
                    Reverse = mod(floor(NextIndex / FlipbookMaxCount), 2.0) == 1.0;
                    NextIndex = mod(NextIndex, FlipbookMaxCount);
                    if (Reverse)
                    {
                        NextIndex = (FlipbookMaxCount - 1.0) - floor(NextIndex);
                    }
                }
            }
        }
        vec2 notInversedUV = uv;
        notInversedUV.y = uvInversed.x + (uvInversed.y * notInversedUV.y);
        vec2 param = notInversedUV;
        float param_1 = Index;
        float param_2 = divideX;
        vec2 param_3 = flipbookOneSize;
        vec2 param_4 = flipbookOffset;
        vec2 OriginUV = GetFlipbookOriginUV(param, param_1, param_2, param_3, param_4);
        vec2 param_5 = OriginUV;
        float param_6 = NextIndex;
        float param_7 = divideX;
        vec2 param_8 = flipbookOneSize;
        vec2 param_9 = flipbookOffset;
        flipbookUV = GetFlipbookUVForIndex(param_5, param_6, param_7, param_8, param_9);
        flipbookUV.y = uvInversed.x + (uvInversed.y * flipbookUV.y);
    }
}

void CalculateAndStoreAdvancedParameter(VS_Input vsinput, inout VS_Output vsoutput)
{
    vsoutput.Alpha_Dist_UV = vsinput.Alpha_Dist_UV;
    vsoutput.Alpha_Dist_UV.y = CBVS0.mUVInversed.x + (CBVS0.mUVInversed.y * vsinput.Alpha_Dist_UV.y);
    vsoutput.Alpha_Dist_UV.w = CBVS0.mUVInversed.x + (CBVS0.mUVInversed.y * vsinput.Alpha_Dist_UV.w);
    vsoutput.Blend_FBNextIndex_UV = vec4(vsinput.BlendUV.x, vsinput.BlendUV.y, vsoutput.Blend_FBNextIndex_UV.z, vsoutput.Blend_FBNextIndex_UV.w);
    vsoutput.Blend_FBNextIndex_UV.y = CBVS0.mUVInversed.x + (CBVS0.mUVInversed.y * vsinput.BlendUV.y);
    vsoutput.Blend_Alpha_Dist_UV = vsinput.Blend_Alpha_Dist_UV;
    vsoutput.Blend_Alpha_Dist_UV.y = CBVS0.mUVInversed.x + (CBVS0.mUVInversed.y * vsinput.Blend_Alpha_Dist_UV.y);
    vsoutput.Blend_Alpha_Dist_UV.w = CBVS0.mUVInversed.x + (CBVS0.mUVInversed.y * vsinput.Blend_Alpha_Dist_UV.w);
    float flipbookRate = 0.0;
    vec2 flipbookNextIndexUV = vec2(0.0);
    float param = flipbookRate;
    vec2 param_1 = flipbookNextIndexUV;
    vec4 param_2 = CBVS0.flipbookParameter1;
    vec4 param_3 = CBVS0.flipbookParameter2;
    float param_4 = vsinput.FlipbookIndex;
    vec2 param_5 = vsoutput.UV_Others.xy;
    vec2 param_6 = vec2(CBVS0.mUVInversed.xy);
    ApplyFlipbookVS(param, param_1, param_2, param_3, param_4, param_5, param_6);
    flipbookRate = param;
    flipbookNextIndexUV = param_1;
    vsoutput.Blend_FBNextIndex_UV = vec4(vsoutput.Blend_FBNextIndex_UV.x, vsoutput.Blend_FBNextIndex_UV.y, flipbookNextIndexUV.x, flipbookNextIndexUV.y);
    vsoutput.UV_Others.z = flipbookRate;
    vsoutput.UV_Others.w = vsinput.AlphaThreshold;
}

VS_Output _main(VS_Input Input)
{
    VS_Output Output = VS_Output(vec4(0.0), vec4(0.0), vec4(0.0), vec3(0.0), vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0));
    vec2 uv1 = Input.UV;
    uv1.y = CBVS0.mUVInversed.x + (CBVS0.mUVInversed.y * uv1.y);
    Output.UV_Others = vec4(uv1.x, uv1.y, Output.UV_Others.z, Output.UV_Others.w);
    vec4 worldPos = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    Output.PosVS = worldPos * CBVS0.mCameraProj;
    Output.Color = Input.Color;
    VS_Input param = Input;
    VS_Output param_1 = Output;
    CalculateAndStoreAdvancedParameter(param, param_1);
    Output = param_1;
    Output.PosP = Output.PosVS;
    return Output;
}

void main()
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
    _VSPS_Color = flattenTemp.Color;
    _VSPS_UV_Others = flattenTemp.UV_Others;
    _VSPS_WorldN = flattenTemp.WorldN;
    _VSPS_Alpha_Dist_UV = flattenTemp.Alpha_Dist_UV;
    _VSPS_Blend_Alpha_Dist_UV = flattenTemp.Blend_Alpha_Dist_UV;
    _VSPS_Blend_FBNextIndex_UV = flattenTemp.Blend_FBNextIndex_UV;
    _VSPS_PosP = flattenTemp.PosP;
}

