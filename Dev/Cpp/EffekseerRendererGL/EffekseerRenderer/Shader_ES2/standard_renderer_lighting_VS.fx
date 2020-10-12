#version 200 es

struct VS_Input
{
    vec3 Pos;
    vec4 Color;
    vec4 Normal;
    vec4 Tangent;
    vec2 UV1;
    vec2 UV2;
    vec4 Alpha_Dist_UV;
    vec2 BlendUV;
    vec4 Blend_Alpha_Dist_UV;
    float FlipbookIndex;
    float AlphaThreshold;
};

struct VS_Output
{
    vec4 Position;
    vec4 VColor;
    vec2 UV;
    vec3 WorldN;
    vec3 WorldT;
    vec3 WorldB;
    vec4 Alpha_Dist_UV;
    vec4 Blend_Alpha_Dist_UV;
    vec4 Blend_FBNextIndex_UV;
    vec2 Others;
};

struct VS_ConstantBuffer
{
    mat4 mCamera;
    mat4 mProj;
    vec4 mUVInversed;
    vec4 mflipbookParameter;
};

uniform VS_ConstantBuffer CBVS0;

attribute vec3 Input_Pos;
attribute vec4 Input_Color;
attribute vec4 Input_Normal;
attribute vec4 Input_Tangent;
attribute vec2 Input_UV1;
attribute vec2 Input_UV2;
attribute vec4 Input_Alpha_Dist_UV;
attribute vec2 Input_BlendUV;
attribute vec4 Input_Blend_Alpha_Dist_UV;
attribute float Input_FlipbookIndex;
attribute float Input_AlphaThreshold;
centroid varying vec4 _VSPS_VColor;
centroid varying vec2 _VSPS_UV;
varying vec3 _VSPS_WorldN;
varying vec3 _VSPS_WorldT;
varying vec3 _VSPS_WorldB;
varying vec4 _VSPS_Alpha_Dist_UV;
varying vec4 _VSPS_Blend_Alpha_Dist_UV;
varying vec4 _VSPS_Blend_FBNextIndex_UV;
varying vec2 _VSPS_Others;

float IntMod(float x, float y)
{
    return floor(mod(x, y));
}

vec2 GetFlipbookOneSizeUV(float DivideX, float DivideY)
{
    return vec2(1.0) / vec2(DivideX, DivideY);
}

vec2 GetFlipbookOriginUV(vec2 FlipbookUV, float FlipbookIndex, float DivideX, float DivideY)
{
    float param = FlipbookIndex;
    float param_1 = DivideX;
    vec2 DivideIndex;
    DivideIndex.x = IntMod(param, param_1);
    DivideIndex.y = float(int(FlipbookIndex) / int(DivideX));
    float param_2 = DivideX;
    float param_3 = DivideY;
    vec2 FlipbookOneSize = GetFlipbookOneSizeUV(param_2, param_3);
    vec2 UVOffset = DivideIndex * FlipbookOneSize;
    vec2 OriginUV = FlipbookUV - UVOffset;
    OriginUV *= vec2(DivideX, DivideY);
    return OriginUV;
}

vec2 GetFlipbookUVForIndex(vec2 OriginUV, float Index, float DivideX, float DivideY)
{
    float param = Index;
    float param_1 = DivideX;
    vec2 DivideIndex;
    DivideIndex.x = IntMod(param, param_1);
    DivideIndex.y = float(int(Index) / int(DivideX));
    float param_2 = DivideX;
    float param_3 = DivideY;
    vec2 FlipbookOneSize = GetFlipbookOneSizeUV(param_2, param_3);
    return (OriginUV * FlipbookOneSize) + (DivideIndex * FlipbookOneSize);
}

void ApplyFlipbookVS(inout float flipbookRate, inout vec2 flipbookUV, vec4 flipbookParameter, float flipbookIndex, vec2 uv)
{
    if (flipbookParameter.x > 0.0)
    {
        flipbookRate = fract(flipbookIndex);
        float Index = floor(flipbookIndex);
        float IndexOffset = 1.0;
        float NextIndex = Index + IndexOffset;
        float FlipbookMaxCount = flipbookParameter.z * flipbookParameter.w;
        if (flipbookParameter.y == 0.0)
        {
            if (NextIndex >= FlipbookMaxCount)
            {
                NextIndex = FlipbookMaxCount - 1.0;
                Index = FlipbookMaxCount - 1.0;
            }
        }
        else
        {
            if (flipbookParameter.y == 1.0)
            {
                Index = mod(Index, FlipbookMaxCount);
                NextIndex = mod(NextIndex, FlipbookMaxCount);
            }
            else
            {
                if (flipbookParameter.y == 2.0)
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
        vec2 param = uv;
        float param_1 = Index;
        float param_2 = flipbookParameter.z;
        float param_3 = flipbookParameter.w;
        vec2 OriginUV = GetFlipbookOriginUV(param, param_1, param_2, param_3);
        vec2 param_4 = OriginUV;
        float param_5 = NextIndex;
        float param_6 = flipbookParameter.z;
        float param_7 = flipbookParameter.w;
        flipbookUV = GetFlipbookUVForIndex(param_4, param_5, param_6, param_7);
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
    vec4 param_2 = CBVS0.mflipbookParameter;
    float param_3 = vsinput.FlipbookIndex;
    vec2 param_4 = vsoutput.UV;
    ApplyFlipbookVS(param, param_1, param_2, param_3, param_4);
    flipbookRate = param;
    flipbookNextIndexUV = param_1;
    vsoutput.Blend_FBNextIndex_UV = vec4(vsoutput.Blend_FBNextIndex_UV.x, vsoutput.Blend_FBNextIndex_UV.y, flipbookNextIndexUV.x, flipbookNextIndexUV.y);
    vsoutput.Others.x = flipbookRate;
    vsoutput.Others.y = vsinput.AlphaThreshold;
}

VS_Output _main(VS_Input Input)
{
    VS_Output Output = VS_Output(vec4(0.0), vec4(0.0), vec2(0.0), vec3(0.0), vec3(0.0), vec3(0.0), vec4(0.0), vec4(0.0), vec4(0.0), vec2(0.0));
    vec3 worldPos = Input.Pos;
    vec3 worldNormal = (Input.Normal.xyz - vec3(0.5)) * 2.0;
    vec3 worldTangent = (Input.Tangent.xyz - vec3(0.5)) * 2.0;
    vec3 worldBinormal = cross(worldNormal, worldTangent);
    vec2 uv1 = Input.UV1;
    vec2 uv2 = Input.UV1;
    uv1.y = CBVS0.mUVInversed.x + (CBVS0.mUVInversed.y * uv1.y);
    uv2.y = CBVS0.mUVInversed.x + (CBVS0.mUVInversed.y * uv2.y);
    Output.WorldN = worldNormal;
    Output.WorldB = worldBinormal;
    Output.WorldT = worldTangent;
    vec3 pixelNormalDir = vec3(0.5, 0.5, 1.0);
    vec4 cameraPos = CBVS0.mCamera * vec4(worldPos, 1.0);
    cameraPos /= vec4(cameraPos.w);
    Output.Position = CBVS0.mProj * cameraPos;
    Output.VColor = Input.Color;
    Output.UV = uv1;
    VS_Input param = Input;
    VS_Output param_1 = Output;
    CalculateAndStoreAdvancedParameter(param, param_1);
    Output = param_1;
    return Output;
}

void main()
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
    gl_Position = flattenTemp.Position;
    _VSPS_VColor = flattenTemp.VColor;
    _VSPS_UV = flattenTemp.UV;
    _VSPS_WorldN = flattenTemp.WorldN;
    _VSPS_WorldT = flattenTemp.WorldT;
    _VSPS_WorldB = flattenTemp.WorldB;
    _VSPS_Alpha_Dist_UV = flattenTemp.Alpha_Dist_UV;
    _VSPS_Blend_Alpha_Dist_UV = flattenTemp.Blend_Alpha_Dist_UV;
    _VSPS_Blend_FBNextIndex_UV = flattenTemp.Blend_FBNextIndex_UV;
    _VSPS_Others = flattenTemp.Others;
}

