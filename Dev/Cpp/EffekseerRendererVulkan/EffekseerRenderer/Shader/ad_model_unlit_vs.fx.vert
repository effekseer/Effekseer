#version 420

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

struct VS_Input
{
    vec3 Pos;
    vec3 Normal;
    vec3 Binormal;
    vec3 Tangent;
    vec2 UV;
    vec4 Color;
    uint Index;
};

layout(set = 0, binding = 0, std140) uniform VS_ConstantBuffer
{
    layout(row_major) mat4 mCameraProj;
    layout(row_major) mat4 mModel_Inst[40];
    vec4 fUV[40];
    vec4 fAlphaUV[40];
    vec4 fUVDistortionUV[40];
    vec4 fBlendUV[40];
    vec4 fBlendAlphaUV[40];
    vec4 fBlendUVDistortionUV[40];
    vec4 fFlipbookParameter;
    vec4 fFlipbookIndexAndNextRate[40];
    vec4 fModelAlphaThreshold[40];
    vec4 fModelColor[40];
    vec4 fLightDirection;
    vec4 fLightColor;
    vec4 fLightAmbient;
    vec4 mUVInversed;
} _365;

layout(location = 0) in vec3 Input_Pos;
layout(location = 1) in vec3 Input_Normal;
layout(location = 2) in vec3 Input_Binormal;
layout(location = 3) in vec3 Input_Tangent;
layout(location = 4) in vec2 Input_UV;
layout(location = 5) in vec4 Input_Color;
layout(location = 0) centroid out vec4 _entryPointOutput_Color;
layout(location = 1) centroid out vec4 _entryPointOutput_UV_Others;
layout(location = 2) out vec3 _entryPointOutput_WorldN;
layout(location = 3) out vec4 _entryPointOutput_Alpha_Dist_UV;
layout(location = 4) out vec4 _entryPointOutput_Blend_Alpha_Dist_UV;
layout(location = 5) out vec4 _entryPointOutput_Blend_FBNextIndex_UV;
layout(location = 6) out vec4 _entryPointOutput_PosP;

vec2 GetFlipbookOneSizeUV(float DivideX, float DivideY)
{
    return vec2(1.0) / vec2(DivideX, DivideY);
}

vec2 GetFlipbookOriginUV(vec2 FlipbookUV, float FlipbookIndex, float DivideX, float DivideY)
{
    vec2 DivideIndex;
    DivideIndex.x = float(int(FlipbookIndex) % int(DivideX));
    DivideIndex.y = float(int(FlipbookIndex) / int(DivideX));
    float param = DivideX;
    float param_1 = DivideY;
    vec2 FlipbookOneSize = GetFlipbookOneSizeUV(param, param_1);
    vec2 UVOffset = DivideIndex * FlipbookOneSize;
    vec2 OriginUV = FlipbookUV - UVOffset;
    OriginUV *= vec2(DivideX, DivideY);
    return OriginUV;
}

vec2 GetFlipbookUVForIndex(vec2 OriginUV, float Index, float DivideX, float DivideY)
{
    vec2 DivideIndex;
    DivideIndex.x = float(int(Index) % int(DivideX));
    DivideIndex.y = float(int(Index) / int(DivideX));
    float param = DivideX;
    float param_1 = DivideY;
    vec2 FlipbookOneSize = GetFlipbookOneSizeUV(param, param_1);
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

void CalculateAndStoreAdvancedParameter(vec2 uv, vec2 uv1, vec4 alphaUV, vec4 uvDistortionUV, vec4 blendUV, vec4 blendAlphaUV, vec4 blendUVDistortionUV, float flipbookIndexAndNextRate, float modelAlphaThreshold, inout VS_Output vsoutput)
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
    float flipbookRate = 0.0;
    vec2 flipbookNextIndexUV = vec2(0.0);
    float param = flipbookRate;
    vec2 param_1 = flipbookNextIndexUV;
    vec4 param_2 = _365.fFlipbookParameter;
    float param_3 = flipbookIndexAndNextRate;
    vec2 param_4 = uv1;
    ApplyFlipbookVS(param, param_1, param_2, param_3, param_4);
    flipbookRate = param;
    flipbookNextIndexUV = param_1;
    vsoutput.Blend_FBNextIndex_UV = vec4(vsoutput.Blend_FBNextIndex_UV.x, vsoutput.Blend_FBNextIndex_UV.y, flipbookNextIndexUV.x, flipbookNextIndexUV.y);
    vsoutput.UV_Others.z = flipbookRate;
    vsoutput.UV_Others.w = modelAlphaThreshold;
    vsoutput.Alpha_Dist_UV.y = _365.mUVInversed.x + (_365.mUVInversed.y * vsoutput.Alpha_Dist_UV.y);
    vsoutput.Alpha_Dist_UV.w = _365.mUVInversed.x + (_365.mUVInversed.y * vsoutput.Alpha_Dist_UV.w);
    vsoutput.Blend_FBNextIndex_UV.y = _365.mUVInversed.x + (_365.mUVInversed.y * vsoutput.Blend_FBNextIndex_UV.y);
    vsoutput.Blend_Alpha_Dist_UV.y = _365.mUVInversed.x + (_365.mUVInversed.y * vsoutput.Blend_Alpha_Dist_UV.y);
    vsoutput.Blend_Alpha_Dist_UV.w = _365.mUVInversed.x + (_365.mUVInversed.y * vsoutput.Blend_Alpha_Dist_UV.w);
}

VS_Output _main(VS_Input Input)
{
    uint index = Input.Index;
    mat4 mModel = _365.mModel_Inst[index];
    vec4 uv = _365.fUV[index];
    vec4 alphaUV = _365.fAlphaUV[index];
    vec4 uvDistortionUV = _365.fUVDistortionUV[index];
    vec4 blendUV = _365.fBlendUV[index];
    vec4 blendAlphaUV = _365.fBlendAlphaUV[index];
    vec4 blendUVDistortionUV = _365.fBlendUVDistortionUV[index];
    vec4 modelColor = _365.fModelColor[index] * Input.Color;
    float flipbookIndexAndNextRate = _365.fFlipbookIndexAndNextRate[index].x;
    float modelAlphaThreshold = _365.fModelAlphaThreshold[index].x;
    VS_Output Output = VS_Output(vec4(0.0), vec4(0.0), vec4(0.0), vec3(0.0), vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0));
    vec4 localPosition = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    vec4 worldPos = localPosition * mModel;
    Output.PosVS = worldPos * _365.mCameraProj;
    vec2 outputUV = Input.UV;
    outputUV.x = (outputUV.x * uv.z) + uv.x;
    outputUV.y = (outputUV.y * uv.w) + uv.y;
    outputUV.y = _365.mUVInversed.x + (_365.mUVInversed.y * outputUV.y);
    Output.UV_Others = vec4(outputUV.x, outputUV.y, Output.UV_Others.z, Output.UV_Others.w);
    vec4 localNormal = vec4(Input.Normal.x, Input.Normal.y, Input.Normal.z, 0.0);
    localNormal = normalize(localNormal * mModel);
    Output.WorldN = localNormal.xyz;
    Output.Color = modelColor;
    vec2 param = Input.UV;
    vec2 param_1 = Output.UV_Others.xy;
    vec4 param_2 = alphaUV;
    vec4 param_3 = uvDistortionUV;
    vec4 param_4 = blendUV;
    vec4 param_5 = blendAlphaUV;
    vec4 param_6 = blendUVDistortionUV;
    float param_7 = flipbookIndexAndNextRate;
    float param_8 = modelAlphaThreshold;
    VS_Output param_9 = Output;
    CalculateAndStoreAdvancedParameter(param, param_1, param_2, param_3, param_4, param_5, param_6, param_7, param_8, param_9);
    Output = param_9;
    Output.PosP = Output.PosVS;
    return Output;
}

void main()
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
    vec4 _position = flattenTemp.PosVS;
    _position.y = -_position.y;
    gl_Position = _position;
    _entryPointOutput_Color = flattenTemp.Color;
    _entryPointOutput_UV_Others = flattenTemp.UV_Others;
    _entryPointOutput_WorldN = flattenTemp.WorldN;
    _entryPointOutput_Alpha_Dist_UV = flattenTemp.Alpha_Dist_UV;
    _entryPointOutput_Blend_Alpha_Dist_UV = flattenTemp.Blend_Alpha_Dist_UV;
    _entryPointOutput_Blend_FBNextIndex_UV = flattenTemp.Blend_FBNextIndex_UV;
    _entryPointOutput_PosP = flattenTemp.PosP;
}

