#version 430

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
    vec2 OctNormal;
    vec2 OctTangent;
    vec2 UV1;
    vec2 UV2;
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
    vec4 flipbookParameter1;
    vec4 flipbookParameter2;
    vec4 fFlipbookIndexAndNextRate[40];
    vec4 fModelAlphaThreshold[40];
    vec4 fModelColor[40];
    vec4 fLightDirection;
    vec4 fLightColor;
    vec4 fLightAmbient;
    vec4 mUVInversed;
} _412;

layout(location = 0) in vec3 Input_Pos;
layout(location = 1) in vec2 Input_OctNormal;
layout(location = 2) in vec2 Input_OctTangent;
layout(location = 3) in vec2 Input_UV1;
layout(location = 4) in vec2 Input_UV2;
layout(location = 5) in vec4 Input_Color;
layout(location = 0) centroid out vec4 _entryPointOutput_Color;
layout(location = 1) centroid out vec4 _entryPointOutput_UV_Others;
layout(location = 2) out vec3 _entryPointOutput_WorldN;
layout(location = 3) out vec4 _entryPointOutput_Alpha_Dist_UV;
layout(location = 4) out vec4 _entryPointOutput_Blend_Alpha_Dist_UV;
layout(location = 5) out vec4 _entryPointOutput_Blend_FBNextIndex_UV;
layout(location = 6) out vec4 _entryPointOutput_PosP;

vec3 decodeOct(vec2 oct)
{
    vec3 v = vec3(oct, (1.0 - abs(oct.x)) - abs(oct.y));
    float t = max(-v.z, 0.0);
    vec3 _88 = v;
    vec3 _93 = v;
    vec2 _95 = _93.xy + ((-sign(_88.xy)) * t);
    v.x = _95.x;
    v.y = _95.y;
    return normalize(v);
}

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
    vec4 param_2 = _412.flipbookParameter1;
    vec4 param_3 = _412.flipbookParameter2;
    float param_4 = flipbookIndexAndNextRate;
    vec2 param_5 = uv1;
    vec2 param_6 = vec2(_412.mUVInversed.xy);
    ApplyFlipbookVS(param, param_1, param_2, param_3, param_4, param_5, param_6);
    flipbookRate = param;
    flipbookNextIndexUV = param_1;
    vsoutput.Blend_FBNextIndex_UV.z = flipbookNextIndexUV.x;
    vsoutput.Blend_FBNextIndex_UV.w = flipbookNextIndexUV.y;
    vsoutput.UV_Others.z = flipbookRate;
    vsoutput.UV_Others.w = modelAlphaThreshold;
    vsoutput.Alpha_Dist_UV.y = _412.mUVInversed.x + (_412.mUVInversed.y * vsoutput.Alpha_Dist_UV.y);
    vsoutput.Alpha_Dist_UV.w = _412.mUVInversed.x + (_412.mUVInversed.y * vsoutput.Alpha_Dist_UV.w);
    vsoutput.Blend_FBNextIndex_UV.y = _412.mUVInversed.x + (_412.mUVInversed.y * vsoutput.Blend_FBNextIndex_UV.y);
    vsoutput.Blend_Alpha_Dist_UV.y = _412.mUVInversed.x + (_412.mUVInversed.y * vsoutput.Blend_Alpha_Dist_UV.y);
    vsoutput.Blend_Alpha_Dist_UV.w = _412.mUVInversed.x + (_412.mUVInversed.y * vsoutput.Blend_Alpha_Dist_UV.w);
}

VS_Output _main(VS_Input Input)
{
    uint index = Input.Index;
    mat4 mModel = _412.mModel_Inst[index];
    vec4 uv = _412.fUV[index];
    vec4 alphaUV = _412.fAlphaUV[index];
    vec4 uvDistortionUV = _412.fUVDistortionUV[index];
    vec4 blendUV = _412.fBlendUV[index];
    vec4 blendAlphaUV = _412.fBlendAlphaUV[index];
    vec4 blendUVDistortionUV = _412.fBlendUVDistortionUV[index];
    vec4 modelColor = _412.fModelColor[index] * Input.Color;
    float flipbookIndexAndNextRate = _412.fFlipbookIndexAndNextRate[index].x;
    float modelAlphaThreshold = _412.fModelAlphaThreshold[index].x;
    VS_Output Output = VS_Output(vec4(0.0), vec4(0.0), vec4(0.0), vec3(0.0), vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0));
    vec4 localPosition = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    vec4 worldPos = localPosition * mModel;
    Output.PosVS = worldPos * _412.mCameraProj;
    vec2 outputUV = Input.UV1;
    outputUV.x = (outputUV.x * uv.z) + uv.x;
    outputUV.y = (outputUV.y * uv.w) + uv.y;
    outputUV.y = _412.mUVInversed.x + (_412.mUVInversed.y * outputUV.y);
    Output.UV_Others.x = outputUV.x;
    Output.UV_Others.y = outputUV.y;
    vec2 param = Input.OctNormal;
    vec3 localNormal = decodeOct(param);
    vec4 worldNormal = normalize(vec4(localNormal, 0.0) * mModel);
    Output.WorldN = localNormal;
    Output.Color = modelColor;
    vec2 param_1 = Input.UV1;
    vec2 param_2 = Output.UV_Others.xy;
    vec4 param_3 = alphaUV;
    vec4 param_4 = uvDistortionUV;
    vec4 param_5 = blendUV;
    vec4 param_6 = blendAlphaUV;
    vec4 param_7 = blendUVDistortionUV;
    float param_8 = flipbookIndexAndNextRate;
    float param_9 = modelAlphaThreshold;
    VS_Output param_10 = Output;
    CalculateAndStoreAdvancedParameter(param_1, param_2, param_3, param_4, param_5, param_6, param_7, param_8, param_9, param_10);
    Output = param_10;
    Output.PosP = Output.PosVS;
    return Output;
}

void main()
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

