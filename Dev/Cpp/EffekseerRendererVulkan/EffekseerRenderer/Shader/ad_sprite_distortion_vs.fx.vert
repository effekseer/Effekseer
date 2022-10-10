#version 430

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
    vec4 PosVS;
    vec4 UV_Others;
    vec4 ProjBinormal;
    vec4 ProjTangent;
    vec4 PosP;
    vec4 Color;
    vec4 Alpha_Dist_UV;
    vec4 Blend_Alpha_Dist_UV;
    vec4 Blend_FBNextIndex_UV;
};

layout(set = 0, binding = 0, std140) uniform VS_ConstantBuffer
{
    layout(row_major) mat4 mCamera;
    layout(row_major) mat4 mCameraProj;
    vec4 mUVInversed;
    vec4 flipbookParameter1;
    vec4 flipbookParameter2;
} _262;

layout(location = 0) in vec3 Input_Pos;
layout(location = 1) in vec4 Input_Color;
layout(location = 2) in vec4 Input_Normal;
layout(location = 3) in vec4 Input_Tangent;
layout(location = 4) in vec2 Input_UV1;
layout(location = 5) in vec2 Input_UV2;
layout(location = 6) in vec4 Input_Alpha_Dist_UV;
layout(location = 7) in vec2 Input_BlendUV;
layout(location = 8) in vec4 Input_Blend_Alpha_Dist_UV;
layout(location = 9) in float Input_FlipbookIndex;
layout(location = 10) in float Input_AlphaThreshold;
layout(location = 0) centroid out vec4 _entryPointOutput_UV_Others;
layout(location = 1) out vec4 _entryPointOutput_ProjBinormal;
layout(location = 2) out vec4 _entryPointOutput_ProjTangent;
layout(location = 3) out vec4 _entryPointOutput_PosP;
layout(location = 4) centroid out vec4 _entryPointOutput_Color;
layout(location = 5) out vec4 _entryPointOutput_Alpha_Dist_UV;
layout(location = 6) out vec4 _entryPointOutput_Blend_Alpha_Dist_UV;
layout(location = 7) out vec4 _entryPointOutput_Blend_FBNextIndex_UV;

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
    vsoutput.Alpha_Dist_UV.y = _262.mUVInversed.x + (_262.mUVInversed.y * vsinput.Alpha_Dist_UV.y);
    vsoutput.Alpha_Dist_UV.w = _262.mUVInversed.x + (_262.mUVInversed.y * vsinput.Alpha_Dist_UV.w);
    vsoutput.Blend_FBNextIndex_UV = vec4(vsinput.BlendUV.x, vsinput.BlendUV.y, vsoutput.Blend_FBNextIndex_UV.z, vsoutput.Blend_FBNextIndex_UV.w);
    vsoutput.Blend_FBNextIndex_UV.y = _262.mUVInversed.x + (_262.mUVInversed.y * vsinput.BlendUV.y);
    vsoutput.Blend_Alpha_Dist_UV = vsinput.Blend_Alpha_Dist_UV;
    vsoutput.Blend_Alpha_Dist_UV.y = _262.mUVInversed.x + (_262.mUVInversed.y * vsinput.Blend_Alpha_Dist_UV.y);
    vsoutput.Blend_Alpha_Dist_UV.w = _262.mUVInversed.x + (_262.mUVInversed.y * vsinput.Blend_Alpha_Dist_UV.w);
    float flipbookRate = 0.0;
    vec2 flipbookNextIndexUV = vec2(0.0);
    float param = flipbookRate;
    vec2 param_1 = flipbookNextIndexUV;
    vec4 param_2 = _262.flipbookParameter1;
    vec4 param_3 = _262.flipbookParameter2;
    float param_4 = vsinput.FlipbookIndex;
    vec2 param_5 = vsoutput.UV_Others.xy;
    vec2 param_6 = vec2(_262.mUVInversed.xy);
    ApplyFlipbookVS(param, param_1, param_2, param_3, param_4, param_5, param_6);
    flipbookRate = param;
    flipbookNextIndexUV = param_1;
    vsoutput.Blend_FBNextIndex_UV = vec4(vsoutput.Blend_FBNextIndex_UV.x, vsoutput.Blend_FBNextIndex_UV.y, flipbookNextIndexUV.x, flipbookNextIndexUV.y);
    vsoutput.UV_Others.z = flipbookRate;
    vsoutput.UV_Others.w = vsinput.AlphaThreshold;
}

VS_Output _main(VS_Input Input)
{
    VS_Output Output = VS_Output(vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0));
    vec4 worldNormal = vec4((Input.Normal.xyz - vec3(0.5)) * 2.0, 0.0);
    vec4 worldTangent = vec4((Input.Tangent.xyz - vec3(0.5)) * 2.0, 0.0);
    vec4 worldBinormal = vec4(cross(worldNormal.xyz, worldTangent.xyz), 0.0);
    vec2 uv1 = Input.UV1;
    uv1.y = _262.mUVInversed.x + (_262.mUVInversed.y * uv1.y);
    Output.UV_Others = vec4(uv1.x, uv1.y, Output.UV_Others.z, Output.UV_Others.w);
    vec4 worldPos = vec4(Input.Pos.x, Input.Pos.y, Input.Pos.z, 1.0);
    Output.PosVS = worldPos * _262.mCameraProj;
    Output.ProjTangent = (worldPos + worldTangent) * _262.mCameraProj;
    Output.ProjBinormal = (worldPos + worldBinormal) * _262.mCameraProj;
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
    vec4 _position = flattenTemp.PosVS;
    _position.y = -_position.y;
    gl_Position = _position;
    _entryPointOutput_UV_Others = flattenTemp.UV_Others;
    _entryPointOutput_ProjBinormal = flattenTemp.ProjBinormal;
    _entryPointOutput_ProjTangent = flattenTemp.ProjTangent;
    _entryPointOutput_PosP = flattenTemp.PosP;
    _entryPointOutput_Color = flattenTemp.Color;
    _entryPointOutput_Alpha_Dist_UV = flattenTemp.Alpha_Dist_UV;
    _entryPointOutput_Blend_Alpha_Dist_UV = flattenTemp.Blend_Alpha_Dist_UV;
    _entryPointOutput_Blend_FBNextIndex_UV = flattenTemp.Blend_FBNextIndex_UV;
}

