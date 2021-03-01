struct PS_Input
{
    float4 PosVS;
    float4 Color;
    float4 UV_Others;
    float3 WorldN;
    float4 Alpha_Dist_UV;
    float4 Blend_Alpha_Dist_UV;
    float4 Blend_FBNextIndex_UV;
};

struct AdvancedParameter
{
    float2 AlphaUV;
    float2 UVDistortionUV;
    float2 BlendUV;
    float2 BlendAlphaUV;
    float2 BlendUVDistortionUV;
    float2 FlipbookNextIndexUV;
    float FlipbookRate;
    float AlphaThreshold;
};

cbuffer PS_ConstanBuffer : register(b0)
{
    float4 _208_fLightDirection : register(c0);
    float4 _208_fLightColor : register(c1);
    float4 _208_fLightAmbient : register(c2);
    float4 _208_fFlipbookParameter : register(c3);
    float4 _208_fUVDistortionParameter : register(c4);
    float4 _208_fBlendTextureParameter : register(c5);
    float4 _208_fCameraFrontDirection : register(c6);
    float4 _208_fFalloffParameter : register(c7);
    float4 _208_fFalloffBeginColor : register(c8);
    float4 _208_fFalloffEndColor : register(c9);
    float4 _208_fEmissiveScaling : register(c10);
    float4 _208_fEdgeColor : register(c11);
    float4 _208_fEdgeParameter : register(c12);
    float4 _208_softParticleParam : register(c13);
    float4 _208_reconstructionParam1 : register(c14);
    float4 _208_reconstructionParam2 : register(c15);
    float4 _208_mUVInversedBack : register(c16);
};

uniform sampler2D Sampler_sampler_uvDistortionTex : register(s2);
uniform sampler2D Sampler_sampler_colorTex : register(s0);
uniform sampler2D Sampler_sampler_alphaTex : register(s1);
uniform sampler2D Sampler_sampler_blendUVDistortionTex : register(s5);
uniform sampler2D Sampler_sampler_blendTex : register(s3);
uniform sampler2D Sampler_sampler_blendAlphaTex : register(s4);

static float4 gl_FragCoord;
static float4 Input_Color;
static float4 Input_UV_Others;
static float3 Input_WorldN;
static float4 Input_Alpha_Dist_UV;
static float4 Input_Blend_Alpha_Dist_UV;
static float4 Input_Blend_FBNextIndex_UV;
static float4 _entryPointOutput;

struct SPIRV_Cross_Input
{
    centroid float4 Input_Color : TEXCOORD0;
    centroid float4 Input_UV_Others : TEXCOORD1;
    float3 Input_WorldN : TEXCOORD2;
    float4 Input_Alpha_Dist_UV : TEXCOORD3;
    float4 Input_Blend_Alpha_Dist_UV : TEXCOORD4;
    float4 Input_Blend_FBNextIndex_UV : TEXCOORD5;
    float4 gl_FragCoord : VPOS;
};

struct SPIRV_Cross_Output
{
    float4 _entryPointOutput : COLOR0;
};

AdvancedParameter DisolveAdvancedParameter(PS_Input psinput)
{
    AdvancedParameter ret;
    ret.AlphaUV = psinput.Alpha_Dist_UV.xy;
    ret.UVDistortionUV = psinput.Alpha_Dist_UV.zw;
    ret.BlendUV = psinput.Blend_FBNextIndex_UV.xy;
    ret.BlendAlphaUV = psinput.Blend_Alpha_Dist_UV.xy;
    ret.BlendUVDistortionUV = psinput.Blend_Alpha_Dist_UV.zw;
    ret.FlipbookNextIndexUV = psinput.Blend_FBNextIndex_UV.zw;
    ret.FlipbookRate = psinput.UV_Others.z;
    ret.AlphaThreshold = psinput.UV_Others.w;
    return ret;
}

float2 UVDistortionOffset(float2 uv, float2 uvInversed, sampler2D SPIRV_Cross_Combinedts)
{
    float2 UVOffset = (tex2D(SPIRV_Cross_Combinedts, uv).xy * 2.0f) - 1.0f.xx;
    UVOffset.y *= (-1.0f);
    UVOffset.y = uvInversed.x + (uvInversed.y * UVOffset.y);
    return UVOffset;
}

void ApplyFlipbook(inout float4 dst, float4 flipbookParameter, float4 vcolor, float2 nextUV, float flipbookRate, sampler2D SPIRV_Cross_Combinedts)
{
    if (flipbookParameter.x > 0.0f)
    {
        float4 NextPixelColor = tex2D(SPIRV_Cross_Combinedts, nextUV) * vcolor;
        if (flipbookParameter.y == 1.0f)
        {
            dst = lerp(dst, NextPixelColor, flipbookRate.xxxx);
        }
    }
}

void ApplyTextureBlending(inout float4 dstColor, float4 blendColor, float blendType)
{
    if (blendType == 0.0f)
    {
        float3 _85 = (blendColor.xyz * blendColor.w) + (dstColor.xyz * (1.0f - blendColor.w));
        dstColor = float4(_85.x, _85.y, _85.z, dstColor.w);
    }
    else
    {
        if (blendType == 1.0f)
        {
            float3 _97 = dstColor.xyz + (blendColor.xyz * blendColor.w);
            dstColor = float4(_97.x, _97.y, _97.z, dstColor.w);
        }
        else
        {
            if (blendType == 2.0f)
            {
                float3 _110 = dstColor.xyz - (blendColor.xyz * blendColor.w);
                dstColor = float4(_110.x, _110.y, _110.z, dstColor.w);
            }
            else
            {
                if (blendType == 3.0f)
                {
                    float3 _123 = dstColor.xyz * (blendColor.xyz * blendColor.w);
                    dstColor = float4(_123.x, _123.y, _123.z, dstColor.w);
                }
            }
        }
    }
}

float4 _main(PS_Input Input)
{
    PS_Input param = Input;
    AdvancedParameter advancedParam = DisolveAdvancedParameter(param);
    float2 param_1 = advancedParam.UVDistortionUV;
    float2 param_2 = _208_fUVDistortionParameter.zw;
    float2 UVOffset = UVDistortionOffset(param_1, param_2, Sampler_sampler_uvDistortionTex);
    UVOffset *= _208_fUVDistortionParameter.x;
    float4 Output = tex2D(Sampler_sampler_colorTex, Input.UV_Others.xy + UVOffset) * Input.Color;
    float4 param_3 = Output;
    float param_4 = advancedParam.FlipbookRate;
    ApplyFlipbook(param_3, _208_fFlipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, param_4, Sampler_sampler_colorTex);
    Output = param_3;
    float4 AlphaTexColor = tex2D(Sampler_sampler_alphaTex, advancedParam.AlphaUV + UVOffset);
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    float2 param_5 = advancedParam.BlendUVDistortionUV;
    float2 param_6 = _208_fUVDistortionParameter.zw;
    float2 BlendUVOffset = UVDistortionOffset(param_5, param_6, Sampler_sampler_blendUVDistortionTex);
    BlendUVOffset *= _208_fUVDistortionParameter.y;
    float4 BlendTextureColor = tex2D(Sampler_sampler_blendTex, advancedParam.BlendUV + BlendUVOffset);
    float4 BlendAlphaTextureColor = tex2D(Sampler_sampler_blendAlphaTex, advancedParam.BlendAlphaUV + BlendUVOffset);
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    float4 param_7 = Output;
    ApplyTextureBlending(param_7, BlendTextureColor, _208_fBlendTextureParameter.x);
    Output = param_7;
    if (_208_fFalloffParameter.x == 1.0f)
    {
        float3 cameraVec = normalize(-_208_fCameraFrontDirection.xyz);
        float CdotN = clamp(dot(cameraVec, normalize(Input.WorldN)), 0.0f, 1.0f);
        float4 FalloffBlendColor = lerp(_208_fFalloffEndColor, _208_fFalloffBeginColor, pow(CdotN, _208_fFalloffParameter.z).xxxx);
        if (_208_fFalloffParameter.y == 0.0f)
        {
            float3 _367 = Output.xyz + FalloffBlendColor.xyz;
            Output = float4(_367.x, _367.y, _367.z, Output.w);
        }
        else
        {
            if (_208_fFalloffParameter.y == 1.0f)
            {
                float3 _380 = Output.xyz - FalloffBlendColor.xyz;
                Output = float4(_380.x, _380.y, _380.z, Output.w);
            }
            else
            {
                if (_208_fFalloffParameter.y == 2.0f)
                {
                    float3 _393 = Output.xyz * FalloffBlendColor.xyz;
                    Output = float4(_393.x, _393.y, _393.z, Output.w);
                }
            }
        }
        Output.w *= FalloffBlendColor.w;
    }
    float3 _407 = Output.xyz * _208_fEmissiveScaling.x;
    Output = float4(_407.x, _407.y, _407.z, Output.w);
    if (Output.w <= max(0.0f, advancedParam.AlphaThreshold))
    {
        discard;
    }
    float3 _439 = lerp(_208_fEdgeColor.xyz * _208_fEdgeParameter.y, Output.xyz, ceil((Output.w - advancedParam.AlphaThreshold) - _208_fEdgeParameter.x).xxx);
    Output = float4(_439.x, _439.y, _439.z, Output.w);
    return Output;
}

void frag_main()
{
    PS_Input Input;
    Input.PosVS = gl_FragCoord;
    Input.Color = Input_Color;
    Input.UV_Others = Input_UV_Others;
    Input.WorldN = Input_WorldN;
    Input.Alpha_Dist_UV = Input_Alpha_Dist_UV;
    Input.Blend_Alpha_Dist_UV = Input_Blend_Alpha_Dist_UV;
    Input.Blend_FBNextIndex_UV = Input_Blend_FBNextIndex_UV;
    float4 _472 = _main(Input);
    _entryPointOutput = _472;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord + float4(0.5f, 0.5f, 0.0f, 0.0f);
    Input_Color = stage_input.Input_Color;
    Input_UV_Others = stage_input.Input_UV_Others;
    Input_WorldN = stage_input.Input_WorldN;
    Input_Alpha_Dist_UV = stage_input.Input_Alpha_Dist_UV;
    Input_Blend_Alpha_Dist_UV = stage_input.Input_Blend_Alpha_Dist_UV;
    Input_Blend_FBNextIndex_UV = stage_input.Input_Blend_FBNextIndex_UV;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output._entryPointOutput = float4(_entryPointOutput);
    return stage_output;
}
