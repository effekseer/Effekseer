const char g_sprite_vs_src[] = R"(mtlcode
#include <metal_stdlib>
#pragma clang diagnostic ignored "-Wparentheses-equality"
using namespace metal;
struct ShaderInput1 {
  float4 atPosition [[attribute(0)]];
  float4 atColor [[attribute(1)]];
  float4 atTexCoord [[attribute(2)]];
};
struct ShaderOutput1 {
  float4 gl_Position [[position]];
  float4 vaColor;
  float4 vaTexCoord;
  float4 vaPos;
  float4 vaPosR;
  float4 vaPosU;
};
struct ShaderUniform1 {
  float4x4 uMatCamera;
  float4x4 uMatProjection;
  float4 mUVInversed;
};
vertex ShaderOutput1 main0 (ShaderInput1 _mtl_i [[stage_in]], constant ShaderUniform1& _mtl_u [[buffer(0)]])
{
  ShaderOutput1 _mtl_o;
  float4 cameraPos_1 = 0;
  float4 tmpvar_2 = 0;
  tmpvar_2 = (_mtl_u.uMatCamera * _mtl_i.atPosition);
  cameraPos_1 = (tmpvar_2 / tmpvar_2.w);
  _mtl_o.gl_Position = (_mtl_u.uMatProjection * cameraPos_1);
  _mtl_o.vaPosR = (_mtl_u.uMatProjection * (cameraPos_1 + float4(1.0, 0.0, 0.0, 0.0)));
  _mtl_o.vaPosU = (_mtl_u.uMatProjection * (cameraPos_1 + float4(0.0, 1.0, 0.0, 0.0)));
  _mtl_o.vaPos = (_mtl_o.gl_Position / _mtl_o.gl_Position.w);
  _mtl_o.vaPosR = (_mtl_o.vaPosR / _mtl_o.vaPosR.w);
  _mtl_o.vaPosU = (_mtl_o.vaPosU / _mtl_o.vaPosU.w);
  _mtl_o.vaColor = _mtl_i.atColor;
  _mtl_o.vaTexCoord.xzw = _mtl_i.atTexCoord.xzw;
  _mtl_o.vaTexCoord.y = (_mtl_u.mUVInversed.x + (_mtl_u.mUVInversed.y * _mtl_i.atTexCoord.y));
  return _mtl_o;
}
)";


const char g_sprite_fs_texture_src[] = R"(mtlcode
#include <metal_stdlib>
#pragma clang diagnostic ignored "-Wparentheses-equality"
using namespace metal;
struct ShaderInput2 {
  float4 vaColor;
  float4 vaTexCoord;
};
struct ShaderOutput2 {
  half4 gl_FragColor;
};
struct ShaderUniform2 {
};
fragment ShaderOutput2 main0 (ShaderInput2 _mtl_i [[stage_in]], constant ShaderUniform2& _mtl_u [[buffer(0)]]
  ,   texture2d<float> uTexture0 [[texture(0)]], sampler _mtlsmp_uTexture0 [[sampler(0)]])
{
  ShaderOutput2 _mtl_o;
  half4 tmpvar_1 = 0;
  tmpvar_1 = half4(uTexture0.sample(_mtlsmp_uTexture0, (float2)(_mtl_i.vaTexCoord.xy)));
  _mtl_o.gl_FragColor = ((half4)(_mtl_i.vaColor * (float4)(tmpvar_1)));
  return _mtl_o;
}
)";


static const char g_sprite_vs_lighting_src[] = R"(mtlcode
#include <metal_stdlib>
#pragma clang diagnostic ignored "-Wparentheses-equality"
using namespace metal;
struct ShaderInput1 {
  float4 atPosition [[attribute(0)]];
  float4 atColor [[attribute(1)]];
  float3 atNormal [[attribute(2)]];
  float3 atTangent [[attribute(3)]];
  float2 atTexCoord [[attribute(4)]];
  float2 atTexCoord2 [[attribute(5)]];
};
struct ShaderOutput1 {
  float4 gl_Position [[position]];
  float4 v_VColor;
  float2 v_UV1;
  float2 v_UV2;
  float3 v_WorldP;
  float3 v_WorldN;
  float3 v_WorldT;
  float3 v_WorldB;
  float2 v_ScreenUV;
};
struct ShaderUniform1 {
  float4x4 uMatCamera;
  float4x4 uMatProjection;
  float4 mUVInversed;
};
vertex ShaderOutput1 main0 (ShaderInput1 _mtl_i [[stage_in]], constant ShaderUniform1& _mtl_u [[buffer(0)]])
{
  ShaderOutput1 _mtl_o;
  float2 uv2_1 = 0;
  float2 uv1_2 = 0;
  float3 tmpvar_3 = 0;
  tmpvar_3 = _mtl_i.atPosition.xyz;
  uv1_2.x = _mtl_i.atTexCoord.x;
  uv1_2.y = (_mtl_u.mUVInversed.x + (_mtl_u.mUVInversed.y * _mtl_i.atTexCoord.y));
  uv2_1.x = _mtl_i.atTexCoord2.x;
  uv2_1.y = (_mtl_u.mUVInversed.x + (_mtl_u.mUVInversed.y * _mtl_i.atTexCoord2.y));
  float3 tmpvar_4 = 0;
  tmpvar_4 = ((_mtl_i.atNormal - float3(0.5, 0.5, 0.5)) * 2.0);
  float3 tmpvar_5 = 0;
  tmpvar_5 = ((_mtl_i.atTangent - float3(0.5, 0.5, 0.5)) * 2.0);
  _mtl_o.v_WorldN = tmpvar_4;
  _mtl_o.v_WorldB = ((tmpvar_4.yzx * tmpvar_5.zxy) - (tmpvar_4.zxy * tmpvar_5.yzx));
  _mtl_o.v_WorldT = tmpvar_5;
  float4 tmpvar_6 = 0;
  tmpvar_6.w = 1.0;
  tmpvar_6.xyz = tmpvar_3;
  float4 tmpvar_7 = 0;
  tmpvar_7 = (_mtl_u.uMatCamera * tmpvar_6);
  _mtl_o.gl_Position = (_mtl_u.uMatProjection * (tmpvar_7 / tmpvar_7.w));
  _mtl_o.v_WorldP = tmpvar_3;
  _mtl_o.v_VColor = _mtl_i.atColor;
  _mtl_o.v_UV1 = uv1_2;
  _mtl_o.v_UV2 = uv2_1;
  _mtl_o.v_ScreenUV = (_mtl_o.gl_Position.xy / _mtl_o.gl_Position.w);
  _mtl_o.v_ScreenUV = ((_mtl_o.v_ScreenUV + float2(1.0, 1.0)) * 0.5);
  return _mtl_o;
}
)";

static const char g_sprite_fs_lighting_src[] = R"(mtlcode
#include <metal_stdlib>
#pragma clang diagnostic ignored "-Wparentheses-equality"
using namespace metal;
struct ShaderInput2 {
  float4 v_VColor;
  float2 v_UV1;
  float3 v_WorldN;
  float3 v_WorldT;
  float3 v_WorldB;
};
struct ShaderOutput2 {
  half4 gl_FragColor;
};
struct ShaderUniform2 {
  float4 LightDirection;
  float4 LightColor;
  float4 LightAmbient;
};
fragment ShaderOutput2 main0 (ShaderInput2 _mtl_i [[stage_in]], constant ShaderUniform2& _mtl_u [[buffer(0)]]
  ,   texture2d<float> ColorTexture [[texture(0)]], sampler _mtlsmp_ColorTexture [[sampler(0)]]
  ,   texture2d<float> NormalTexture [[texture(1)]], sampler _mtlsmp_NormalTexture [[sampler(1)]])
{
  ShaderOutput2 _mtl_o;
  half4 tmpvar_1 = 0;
  tmpvar_1 = half4(NormalTexture.sample(_mtlsmp_NormalTexture, (float2)(_mtl_i.v_UV1)));
  float3x3 tmpvar_2;
  tmpvar_2[0] = _mtl_i.v_WorldT;
  tmpvar_2[1] = _mtl_i.v_WorldB;
  tmpvar_2[2] = _mtl_i.v_WorldN;
  half4 tmpvar_3 = 0;
  tmpvar_3 = half4(ColorTexture.sample(_mtlsmp_ColorTexture, (float2)(_mtl_i.v_UV1)));
  _mtl_o.gl_FragColor = ((half4)(_mtl_i.v_VColor * (float4)(tmpvar_3)));
  _mtl_o.gl_FragColor.xyz = (_mtl_o.gl_FragColor.xyz * ((half3)(_mtl_u.LightColor.xyz * (float3)(half3(max ((half)0.0,
    ((half)dot ((float3)normalize(((half3)(tmpvar_2 * (float3)((
      (tmpvar_1.xyz - (half)(0.5))
     * (half)(2.0)))))), _mtl_u.LightDirection.xyz))
  ))) + _mtl_u.LightAmbient.xyz)));
  return _mtl_o;
}
)";


const char g_sprite_distortion_vs_src[] = R"(mtlcode
#include <metal_stdlib>
#pragma clang diagnostic ignored "-Wparentheses-equality"
using namespace metal;
struct ShaderInput1 {
  float4 atPosition [[attribute(0)]];
  float4 atColor [[attribute(1)]];
  float4 atTexCoord [[attribute(2)]];
  float4 atBinormal [[attribute(3)]];
  float4 atTangent [[attribute(4)]];
};
struct ShaderOutput1 {
  float4 gl_Position [[position]];
  float4 vaColor;
  float4 vaTexCoord;
  float4 vaPos;
  float4 vaPosR;
  float4 vaPosU;
};
struct ShaderUniform1 {
  float4x4 uMatCamera;
  float4x4 uMatProjection;
  float4 mUVInversed;
};
vertex ShaderOutput1 main0 (ShaderInput1 _mtl_i [[stage_in]], constant ShaderUniform1& _mtl_u [[buffer(0)]])
{
  ShaderOutput1 _mtl_o;
  float4 cameraPos_1 = 0;
  float4 localTangent_2 = 0;
  float4 localBinormal_3 = 0;
  float4 tmpvar_4 = 0;
  tmpvar_4.w = 1.0;
  tmpvar_4.xyz = (_mtl_i.atPosition.xyz + _mtl_i.atBinormal.xyz);
  float4 tmpvar_5 = 0;
  tmpvar_5.w = 1.0;
  tmpvar_5.xyz = (_mtl_i.atPosition.xyz + _mtl_i.atTangent.xyz);
  localBinormal_3 = (_mtl_u.uMatCamera * tmpvar_4);
  localTangent_2 = (_mtl_u.uMatCamera * tmpvar_5);
  float4 tmpvar_6 = 0;
  tmpvar_6 = (_mtl_u.uMatCamera * _mtl_i.atPosition);
  cameraPos_1 = (tmpvar_6 / tmpvar_6.w);
  localBinormal_3 = (localBinormal_3 / localBinormal_3.w);
  localTangent_2 = (localTangent_2 / localTangent_2.w);
  localBinormal_3 = (cameraPos_1 + normalize((localBinormal_3 - cameraPos_1)));
  localTangent_2 = (cameraPos_1 + normalize((localTangent_2 - cameraPos_1)));
  _mtl_o.gl_Position = (_mtl_u.uMatProjection * cameraPos_1);
  _mtl_o.vaPosR = (_mtl_u.uMatProjection * localTangent_2);
  _mtl_o.vaPosU = (_mtl_u.uMatProjection * localBinormal_3);
  _mtl_o.vaPos = (_mtl_o.gl_Position / _mtl_o.gl_Position.w);
  _mtl_o.vaPosR = (_mtl_o.vaPosR / _mtl_o.vaPosR.w);
  _mtl_o.vaPosU = (_mtl_o.vaPosU / _mtl_o.vaPosU.w);
  _mtl_o.vaColor = _mtl_i.atColor;
  _mtl_o.vaTexCoord.xzw = _mtl_i.atTexCoord.xzw;
  _mtl_o.vaTexCoord.y = (_mtl_u.mUVInversed.x + (_mtl_u.mUVInversed.y * _mtl_i.atTexCoord.y));
  return _mtl_o;
}
)";


const char g_sprite_fs_texture_distortion_src[] = R"(mtlcode
#include <metal_stdlib>
#pragma clang diagnostic ignored "-Wparentheses-equality"
using namespace metal;
struct ShaderInput2 {
  float4 vaColor;
  float4 vaTexCoord;
  float4 vaPos;
  float4 vaPosR;
  float4 vaPosU;
};
struct ShaderOutput2 {
  half4 gl_FragColor;
};
struct ShaderUniform2 {
  float4 g_scale;
  float4 mUVInversedBack;
};
fragment ShaderOutput2 main0 (ShaderInput2 _mtl_i [[stage_in]], constant ShaderUniform2& _mtl_u [[buffer(0)]]
  ,   texture2d<float> uTexture0 [[texture(0)]], sampler _mtlsmp_uTexture0 [[sampler(0)]]
  ,   texture2d<float> uBackTexture0 [[texture(1)]], sampler _mtlsmp_uBackTexture0 [[sampler(1)]])
{
  ShaderOutput2 _mtl_o;
  half2 uv_1 = 0;
  half4 color_2 = 0;
  half4 tmpvar_3 = 0;
  tmpvar_3 = half4(uTexture0.sample(_mtlsmp_uTexture0, (float2)(_mtl_i.vaTexCoord.xy)));
  color_2.w = ((half)((float)(tmpvar_3.w) * _mtl_i.vaColor.w));
  float2 tmpvar_4 = 0;
  tmpvar_4 = (_mtl_i.vaPos.xy / _mtl_i.vaPos.w);
  float2 tmpvar_5 = 0;
  tmpvar_5 = (_mtl_i.vaPosU.xy / _mtl_i.vaPosU.w);
  float2 tmpvar_6 = 0;
  tmpvar_6 = (_mtl_i.vaPosR.xy / _mtl_i.vaPosR.w);
  uv_1 = (((
    ((half2)(tmpvar_4 + (float2)(((half2)((float2)(((half2)((float2)(((half2)(
      (tmpvar_6 - tmpvar_4)
     * (float)(
      ((tmpvar_3.x * (half)(2.0)) - (half)(1.0))
    )))) * _mtl_i.vaColor.x))) * _mtl_u.g_scale.x)))))
   +
    ((half2)((float2)(((half2)((float2)(((half2)((tmpvar_5 - tmpvar_4) * (float)((
      (tmpvar_3.y * (half)(2.0))
     - (half)(1.0)))))) * _mtl_i.vaColor.y))) * _mtl_u.g_scale.x))
  ) + (half2)(float2(1.0, 1.0))) * (half2)(float2(0.5, 0.5)));
  uv_1.y = ((half)(_mtl_u.mUVInversedBack.x + (float)(((half)(_mtl_u.mUVInversedBack.y * (float)(uv_1.y))))));
  half4 tmpvar_7 = 0;
  tmpvar_7 = half4(uBackTexture0.sample(_mtlsmp_uBackTexture0, (float2)(uv_1)));
  color_2.xyz = tmpvar_7.xyz;
  _mtl_o.gl_FragColor = color_2;
  return _mtl_o;
}
)";


const char g_model_lighting_vs_src[] = R"(mtlcode
#include <metal_stdlib>
#pragma clang diagnostic ignored "-Wparentheses-equality"
using namespace metal;
struct ShaderInput1 {
  float4 a_Position [[attribute(0)]];
  float4 a_Normal [[attribute(1)]];
  float4 a_Binormal [[attribute(2)]];
  float4 a_Tangent [[attribute(3)]];
  float4 a_TexCoord [[attribute(4)]];
  float4 a_Color [[attribute(5)]];
};
struct ShaderOutput1 {
  float4 gl_Position [[position]];
  float4 v_Normal;
  float4 v_Binormal;
  float4 v_Tangent;
  float4 v_TexCoord;
  float4 v_Color;
};
struct ShaderUniform1 {
  float4x4 ProjectionMatrix;
  float4x4 ModelMatrix;
  float4 UVOffset;
  float4 ModelColor;
  float4 LightDirection;
  float4 LightColor;
  float4 LightAmbient;
  float4 mUVInversed;
};
vertex ShaderOutput1 main0 (ShaderInput1 _mtl_i [[stage_in]], constant ShaderUniform1& _mtl_u [[buffer(0)]])
{
  ShaderOutput1 _mtl_o;
  _mtl_o.gl_Position = (_mtl_u.ProjectionMatrix * (_mtl_u.ModelMatrix * _mtl_i.a_Position));
  _mtl_o.v_TexCoord.xy = ((_mtl_i.a_TexCoord.xy * _mtl_u.UVOffset.zw) + _mtl_u.UVOffset.xy);
  float3x3 tmpvar_1;
  tmpvar_1[0] = _mtl_u.ModelMatrix[0].xyz;
  tmpvar_1[1] = _mtl_u.ModelMatrix[1].xyz;
  tmpvar_1[2] = _mtl_u.ModelMatrix[2].xyz;
  float4 tmpvar_2 = 0;
  tmpvar_2.w = 1.0;
  tmpvar_2.xyz = normalize((tmpvar_1 * _mtl_i.a_Normal.xyz));
  _mtl_o.v_Normal = tmpvar_2;
  float4 tmpvar_3 = 0;
  tmpvar_3.w = 1.0;
  tmpvar_3.xyz = normalize((tmpvar_1 * _mtl_i.a_Binormal.xyz));
  _mtl_o.v_Binormal = tmpvar_3;
  float4 tmpvar_4 = 0;
  tmpvar_4.w = 1.0;
  tmpvar_4.xyz = normalize((tmpvar_1 * _mtl_i.a_Tangent.xyz));
  _mtl_o.v_Tangent = tmpvar_4;
  _mtl_o.v_Color = _mtl_u.ModelColor * _mtl_i.a_Color;
  _mtl_o.v_TexCoord.y = (_mtl_u.mUVInversed.x + (_mtl_u.mUVInversed.y * _mtl_o.v_TexCoord.y));
  return _mtl_o;
}
)";


const char g_model_lighting_fs_src[] = R"(mtlcode
#include <metal_stdlib>
#pragma clang diagnostic ignored "-Wparentheses-equality"
using namespace metal;
struct ShaderInput2 {
  float4 v_Normal;
  float4 v_Binormal;
  float4 v_Tangent;
  float4 v_TexCoord;
  float4 v_Color;
};
struct ShaderOutput2 {
  half4 gl_FragColor;
};
struct ShaderUniform2 {
  float4 LightDirection;
  float4 LightColor;
  float4 LightAmbient;
};
fragment ShaderOutput2 main0 (ShaderInput2 _mtl_i [[stage_in]], constant ShaderUniform2& _mtl_u [[buffer(0)]]
  ,   texture2d<float> ColorTexture [[texture(0)]], sampler _mtlsmp_ColorTexture [[sampler(0)]]
  ,   texture2d<float> NormalTexture [[texture(1)]], sampler _mtlsmp_NormalTexture [[sampler(1)]])
{
  ShaderOutput2 _mtl_o;
  half4 tmpvar_1 = 0;
  tmpvar_1 = half4(NormalTexture.sample(_mtlsmp_NormalTexture, (float2)(_mtl_i.v_TexCoord.xy)));
  float3x3 tmpvar_2;
  tmpvar_2[0] = _mtl_i.v_Tangent.xyz;
  tmpvar_2[1] = _mtl_i.v_Binormal.xyz;
  tmpvar_2[2] = _mtl_i.v_Normal.xyz;
  half4 tmpvar_3 = 0;
  tmpvar_3 = half4(ColorTexture.sample(_mtlsmp_ColorTexture, (float2)(_mtl_i.v_TexCoord.xy)));
  _mtl_o.gl_FragColor = ((half4)(_mtl_i.v_Color * (float4)(tmpvar_3)));
  _mtl_o.gl_FragColor.xyz = (_mtl_o.gl_FragColor.xyz * half3(_mtl_u.LightColor.xyz * max ((half)0.0, ((half)dot ((float3)
    normalize(((half3)(tmpvar_2 * (float3)(((tmpvar_1.xyz - (half)(0.5)) * (half)(2.0))))))
  , _mtl_u.LightDirection.xyz)))));
  _mtl_o.gl_FragColor.xyz = ((half3)((float3)(_mtl_o.gl_FragColor.xyz) + _mtl_u.LightAmbient.xyz));
  return _mtl_o;
}
)";


const char g_model_texture_vs_src[] = R"(mtlcode
#include <metal_stdlib>
#pragma clang diagnostic ignored "-Wparentheses-equality"
using namespace metal;
struct ShaderInput1 {
  float4 a_Position [[attribute(0)]];
  float4 a_Normal [[attribute(1)]];
  float4 a_Binormal [[attribute(2)]];
  float4 a_Tangent [[attribute(3)]];
  float4 a_TexCoord [[attribute(4)]];
  float4 a_Color [[attribute(5)]];
};
struct ShaderOutput1 {
  float4 gl_Position [[position]];
  float4 v_Normal;
  float4 v_Binormal;
  float4 v_Tangent;
  float4 v_TexCoord;
  float4 v_Color;
};
struct ShaderUniform1 {
  float4x4 ProjectionMatrix;
  float4x4 ModelMatrix;
  float4 UVOffset;
  float4 ModelColor;
  float4 LightDirection;
  float4 LightColor;
  float4 LightAmbient;
  float4 mUVInversed;
};
vertex ShaderOutput1 main0 (ShaderInput1 _mtl_i [[stage_in]], constant ShaderUniform1& _mtl_u [[buffer(0)]])
{
  ShaderOutput1 _mtl_o;
  _mtl_o.gl_Position = (_mtl_u.ProjectionMatrix * (_mtl_u.ModelMatrix * _mtl_i.a_Position));
  _mtl_o.v_TexCoord.xy = ((_mtl_i.a_TexCoord.xy * _mtl_u.UVOffset.zw) + _mtl_u.UVOffset.xy);
  _mtl_o.v_Color = (_mtl_u.ModelColor * _mtl_i.a_Color);
  _mtl_o.v_TexCoord.y = (_mtl_u.mUVInversed.x + (_mtl_u.mUVInversed.y * _mtl_o.v_TexCoord.y));
  return _mtl_o;
}
)";


const char g_model_texture_fs_src[] = R"(mtlcode
#include <metal_stdlib>
#pragma clang diagnostic ignored "-Wparentheses-equality"
using namespace metal;
struct ShaderInput2 {
  float4 v_Normal;
  float4 v_Binormal;
  float4 v_Tangent;
  float4 v_TexCoord;
  float4 v_Color;
};
struct ShaderOutput2 {
  half4 gl_FragColor;
};
struct ShaderUniform2 {
  float4 LightDirection;
  float4 LightColor;
  float4 LightAmbient;
};
fragment ShaderOutput2 main0 (ShaderInput2 _mtl_i [[stage_in]], constant ShaderUniform2& _mtl_u [[buffer(0)]]
  ,   texture2d<float> ColorTexture [[texture(0)]], sampler _mtlsmp_ColorTexture [[sampler(0)]])
{
  ShaderOutput2 _mtl_o;
  half4 tmpvar_1 = 0;
  tmpvar_1 = half4(ColorTexture.sample(_mtlsmp_ColorTexture, (float2)(_mtl_i.v_TexCoord.xy)));
  _mtl_o.gl_FragColor = ((half4)(_mtl_i.v_Color * (float4)(tmpvar_1)));
  return _mtl_o;
}
)";


const char g_model_distortion_vs_src[] = R"(mtlcode
#include <metal_stdlib>
#pragma clang diagnostic ignored "-Wparentheses-equality"
using namespace metal;
struct ShaderInput1 {
  float4 a_Position [[attribute(0)]];
  float4 a_Normal [[attribute(1)]];
  float4 a_Binormal [[attribute(2)]];
  float4 a_Tangent [[attribute(3)]];
  float4 a_TexCoord [[attribute(4)]];
  float4 a_Color [[attribute(5)]];
};
struct ShaderOutput1 {
  float4 gl_Position [[position]];
  float4 v_Normal;
  float4 v_Binormal;
  float4 v_Tangent;
  float4 v_TexCoord;
  float4 v_Pos;
  float4 v_Color;
};
struct ShaderUniform1 {
  float4x4 ProjectionMatrix;
  float4x4 ModelMatrix;
  float4 UVOffset;
  float4 ModelColor;
  float4 LightDirection;
  float4 LightColor;
  float4 LightAmbient;
  float4 mUVInversed;
};
vertex ShaderOutput1 main0 (ShaderInput1 _mtl_i [[stage_in]], constant ShaderUniform1& _mtl_u [[buffer(0)]])
{
  ShaderOutput1 _mtl_o;
  float4 localTangent_1 = 0;
  float4 localBinormal_2 = 0;
  float4 localNormal_3 = 0;
  float4 localPosition_4 = 0;
  float4 tmpvar_5 = 0;
  tmpvar_5.w = 1.0;
  tmpvar_5.xyz = _mtl_i.a_Position.xyz;
  float4 tmpvar_6 = 0;
  tmpvar_6.w = 1.0;
  tmpvar_6.xyz = (_mtl_i.a_Position.xyz + _mtl_i.a_Normal.xyz);
  float4 tmpvar_7 = 0;
  tmpvar_7.w = 1.0;
  tmpvar_7.xyz = (_mtl_i.a_Position.xyz + _mtl_i.a_Binormal.xyz);
  float4 tmpvar_8 = 0;
  tmpvar_8.w = 1.0;
  tmpvar_8.xyz = (_mtl_i.a_Position.xyz + _mtl_i.a_Tangent.xyz);
  localPosition_4 = (_mtl_u.ModelMatrix * tmpvar_5);
  localNormal_3 = (_mtl_u.ModelMatrix * tmpvar_6);
  localBinormal_2 = (_mtl_u.ModelMatrix * tmpvar_7);
  localTangent_1 = (_mtl_u.ModelMatrix * tmpvar_8);
  localNormal_3 = (localPosition_4 + normalize((localNormal_3 - localPosition_4)));
  localBinormal_2 = (localPosition_4 + normalize((localBinormal_2 - localPosition_4)));
  localTangent_1 = (localPosition_4 + normalize((localTangent_1 - localPosition_4)));
  _mtl_o.gl_Position = (_mtl_u.ProjectionMatrix * localPosition_4);
  _mtl_o.v_TexCoord.xy = ((_mtl_i.a_TexCoord.xy * _mtl_u.UVOffset.zw) + _mtl_u.UVOffset.xy);
  _mtl_o.v_Normal = (_mtl_u.ProjectionMatrix * localNormal_3);
  _mtl_o.v_Binormal = (_mtl_u.ProjectionMatrix * localBinormal_2);
  _mtl_o.v_Tangent = (_mtl_u.ProjectionMatrix * localTangent_1);
  _mtl_o.v_Pos = _mtl_o.gl_Position;
  _mtl_o.v_Color = (_mtl_u.ModelColor * _mtl_i.a_Color);
  _mtl_o.v_TexCoord.y = (_mtl_u.mUVInversed.x + (_mtl_u.mUVInversed.y * _mtl_o.v_TexCoord.y));
  return _mtl_o;
}
)";


const char g_model_distortion_fs_src[] = R"(mtlcode
#include <metal_stdlib>
#pragma clang diagnostic ignored "-Wparentheses-equality"
using namespace metal;
struct ShaderInput2 {
  float4 v_Binormal;
  float4 v_Tangent;
  float4 v_TexCoord;
  float4 v_Pos;
  float4 v_Color;
};
struct ShaderOutput2 {
  half4 gl_FragColor;
};
struct ShaderUniform2 {
  float4 g_scale;
  float4 mUVInversedBack;
};
fragment ShaderOutput2 main0 (ShaderInput2 _mtl_i [[stage_in]], constant ShaderUniform2& _mtl_u [[buffer(0)]]
  ,   texture2d<float> uTexture0 [[texture(0)]], sampler _mtlsmp_uTexture0 [[sampler(0)]]
  ,   texture2d<float> uBackTexture0 [[texture(1)]], sampler _mtlsmp_uBackTexture0 [[sampler(1)]])
{
  ShaderOutput2 _mtl_o;
  half2 uv_1 = 0;
  half4 tmpvar_2 = 0;
  tmpvar_2 = half4(uTexture0.sample(_mtlsmp_uTexture0, (float2)(_mtl_i.v_TexCoord.xy)));
  _mtl_o.gl_FragColor.w = ((half)((float)(tmpvar_2.w) * _mtl_i.v_Color.w));
  float2 tmpvar_3 = 0;
  tmpvar_3 = (_mtl_i.v_Pos.xy / _mtl_i.v_Pos.w);
  float2 tmpvar_4 = 0;
  tmpvar_4 = (_mtl_i.v_Tangent.xy / _mtl_i.v_Tangent.w);
  float2 tmpvar_5 = 0;
  tmpvar_5 = (_mtl_i.v_Binormal.xy / _mtl_i.v_Binormal.w);
  uv_1 = (((
    ((half2)(tmpvar_3 + (float2)(((half2)((tmpvar_5 - tmpvar_3) * (float)(((half)((float)(((half)((float)(
      ((tmpvar_2.x * (half)(2.0)) - (half)(1.0))
    ) * _mtl_i.v_Color.x))) * _mtl_u.g_scale.x))))))))
   +
    ((half2)((tmpvar_4 - tmpvar_3) * (float)(((half)((float)(((half)((float)((
      (tmpvar_2.y * (half)(2.0))
     - (half)(1.0))) * _mtl_i.v_Color.y))) * _mtl_u.g_scale.x)))))
  ) + (half2)(float2(1.0, 1.0))) * (half2)(float2(0.5, 0.5)));
  uv_1.y = ((half)(_mtl_u.mUVInversedBack.x + (float)(((half)(_mtl_u.mUVInversedBack.y * (float)(uv_1.y))))));
  half4 tmpvar_6 = 0;
  tmpvar_6 = half4(uBackTexture0.sample(_mtlsmp_uBackTexture0, (float2)(uv_1)));
  _mtl_o.gl_FragColor.xyz = tmpvar_6.xyz;
  return _mtl_o;
}
)";
