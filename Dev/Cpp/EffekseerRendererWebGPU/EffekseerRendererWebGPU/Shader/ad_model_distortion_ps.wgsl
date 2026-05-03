diagnostic(off, derivative_uniformity);

@group(1) @binding(3) var _uvDistortionTex : texture_2d<f32>;

@group(2) @binding(3) var sampler_uvDistortionTex : sampler;

@group(1) @binding(6) var _blendUVDistortionTex : texture_2d<f32>;

@group(2) @binding(6) var sampler_blendUVDistortionTex : sampler;

@group(1) @binding(0) var _colorTex : texture_2d<f32>;

@group(2) @binding(0) var sampler_colorTex : sampler;

struct PS_ConstantBuffer {
  _393_g_scale : vec4<f32>,
  _393_mUVInversedBack : vec4<f32>,
  _393_fFlipbookParameter : vec4<f32>,
  _393_fUVDistortionParameter : vec4<f32>,
  _393_fBlendTextureParameter : vec4<f32>,
  _393_softParticleParam : vec4<f32>,
  _393_reconstructionParam1 : vec4<f32>,
  _393_reconstructionParam2 : vec4<f32>,
}

@group(0) @binding(1) var<uniform> v : PS_ConstantBuffer;

@group(1) @binding(2) var _alphaTex : texture_2d<f32>;

@group(2) @binding(2) var sampler_alphaTex : sampler;

@group(1) @binding(4) var _blendTex : texture_2d<f32>;

@group(2) @binding(4) var sampler_blendTex : sampler;

@group(1) @binding(5) var _blendAlphaTex : texture_2d<f32>;

@group(2) @binding(5) var sampler_blendAlphaTex : sampler;

@group(1) @binding(1) var _backTex : texture_2d<f32>;

@group(2) @binding(1) var sampler_backTex : sampler;

@group(1) @binding(7) var _depthTex : texture_2d<f32>;

@group(2) @binding(7) var sampler_depthTex : sampler;

var<private> gl_FragCoord : vec4<f32>;

var<private> Input_UV_Others : vec4<f32>;

var<private> Input_ProjBinormal : vec4<f32>;

var<private> Input_ProjTangent : vec4<f32>;

var<private> Input_PosP : vec4<f32>;

var<private> Input_Color : vec4<f32>;

var<private> Input_Alpha_Dist_UV : vec4<f32>;

var<private> Input_Blend_Alpha_Dist_UV : vec4<f32>;

var<private> Input_Blend_FBNextIndex_UV : vec4<f32>;

var<private> _entryPointOutput : vec4<f32>;

var<private> v_1 : vec4<f32>;

struct SPIRV_Cross_Input {
  Input_UV_Others : vec4<f32>,
  Input_ProjBinormal : vec4<f32>,
  Input_ProjTangent : vec4<f32>,
  Input_PosP : vec4<f32>,
  Input_Color : vec4<f32>,
  Input_Alpha_Dist_UV : vec4<f32>,
  Input_Blend_Alpha_Dist_UV : vec4<f32>,
  Input_Blend_FBNextIndex_UV : vec4<f32>,
  gl_FragCoord : vec4<f32>,
}

fn main_inner(v_2 : vec4<f32>, v_3 : vec4<f32>, v_4 : vec4<f32>, v_5 : vec4<f32>, v_6 : vec4<f32>, v_7 : vec4<f32>, v_8 : vec4<f32>, v_9 : vec4<f32>, v_10 : vec4<f32>) {
  var stage_input : SPIRV_Cross_Input;
  var param : SPIRV_Cross_Input;
  stage_input.Input_UV_Others = v_2;
  stage_input.Input_ProjBinormal = v_3;
  stage_input.Input_ProjTangent = v_4;
  stage_input.Input_PosP = v_5;
  stage_input.Input_Color = v_6;
  stage_input.Input_Alpha_Dist_UV = v_7;
  stage_input.Input_Blend_Alpha_Dist_UV = v_8;
  stage_input.Input_Blend_FBNextIndex_UV = v_9;
  stage_input.gl_FragCoord = v_10;
  param = stage_input;
  v_1 = v_11(&(param))._entryPointOutput;
}

struct PS_Input {
  PosVS : vec4<f32>,
  UV_Others : vec4<f32>,
  ProjBinormal : vec4<f32>,
  ProjTangent : vec4<f32>,
  PosP : vec4<f32>,
  Color : vec4<f32>,
  Alpha_Dist_UV : vec4<f32>,
  Blend_Alpha_Dist_UV : vec4<f32>,
  Blend_FBNextIndex_UV : vec4<f32>,
}

struct AdvancedParameter {
  AlphaUV : vec2<f32>,
  UVDistortionUV : vec2<f32>,
  BlendUV : vec2<f32>,
  BlendAlphaUV : vec2<f32>,
  BlendUVDistortionUV : vec2<f32>,
  FlipbookNextIndexUV : vec2<f32>,
  FlipbookRate : f32,
  AlphaThreshold : f32,
}

fn v_12(psinput : ptr<function, PS_Input>) -> AdvancedParameter {
  var ret : AdvancedParameter;
  ret.AlphaUV = (*(psinput)).Alpha_Dist_UV.xy;
  ret.UVDistortionUV = (*(psinput)).Alpha_Dist_UV.zw;
  ret.BlendUV = (*(psinput)).Blend_FBNextIndex_UV.xy;
  ret.BlendAlphaUV = (*(psinput)).Blend_Alpha_Dist_UV.xy;
  ret.BlendUVDistortionUV = (*(psinput)).Blend_Alpha_Dist_UV.zw;
  ret.FlipbookNextIndexUV = (*(psinput)).Blend_FBNextIndex_UV.zw;
  ret.FlipbookRate = (*(psinput)).UV_Others.z;
  ret.AlphaThreshold = (*(psinput)).UV_Others.w;
  return ret;
}

fn v_13(base : ptr<function, vec3<f32>>, power : ptr<function, vec3<f32>>) -> vec3<f32> {
  return pow(max(abs(*(base)), vec3<f32>(0.00000011920928955078f)), *(power));
}

fn v_14(c : ptr<function, vec3<f32>>) -> vec3<f32> {
  var param : vec3<f32>;
  var param_1 : vec3<f32>;
  var param_16 : vec3<f32>;
  var param_17 : vec3<f32>;
  param = *(c);
  param_1 = vec3<f32>(0.4166666567325592041f);
  param_16 = param;
  param_17 = param_1;
  return max(((v_13(&(param_16), &(param_17)) * 1.05499994754791259766f) - vec3<f32>(0.05499999970197677612f)), vec3<f32>());
}

fn v_15(c : ptr<function, vec4<f32>>) -> vec4<f32> {
  var param : vec3<f32>;
  var param_18 : vec3<f32>;
  param = (*(c)).xyz;
  param_18 = param;
  let v_16 = v_14(&(param_18));
  return vec4<f32>(v_16.x, v_16.y, v_16.z, (*(c)).w);
}

fn v_17(c : ptr<function, vec4<f32>>, isValid : ptr<function, bool>) -> vec4<f32> {
  var param : vec4<f32>;
  var param_19 : vec4<f32>;
  if (!(*(isValid))) {
    return *(c);
  }
  param = *(c);
  param_19 = param;
  return v_15(&(param_19));
}

fn v_18(uv : ptr<function, vec2<f32>>, uvInversed : ptr<function, vec2<f32>>, convertFromSRGB : ptr<function, bool>) -> vec2<f32> {
  var sampledColor : vec4<f32>;
  var param : vec4<f32>;
  var param_1 : bool;
  var param_20 : vec4<f32>;
  var param_21 : bool;
  var UVOffset : vec2<f32>;
  sampledColor = textureSample(_uvDistortionTex, sampler_uvDistortionTex, *(uv));
  if (*(convertFromSRGB)) {
    param = sampledColor;
    param_1 = *(convertFromSRGB);
    param_20 = param;
    param_21 = param_1;
    sampledColor = v_17(&(param_20), &(param_21));
  }
  UVOffset = ((sampledColor.xy * 2.0f) - vec2<f32>(1.0f));
  UVOffset.y = (UVOffset.y * -1.0f);
  UVOffset.y = ((*(uvInversed)).x + ((*(uvInversed)).y * UVOffset.y));
  return UVOffset;
}

fn v_19(uv : ptr<function, vec2<f32>>, uvInversed : ptr<function, vec2<f32>>, convertFromSRGB : ptr<function, bool>) -> vec2<f32> {
  var sampledColor : vec4<f32>;
  var param : vec4<f32>;
  var param_1 : bool;
  var param_22 : vec4<f32>;
  var param_23 : bool;
  var UVOffset : vec2<f32>;
  sampledColor = textureSample(_blendUVDistortionTex, sampler_blendUVDistortionTex, *(uv));
  if (*(convertFromSRGB)) {
    param = sampledColor;
    param_1 = *(convertFromSRGB);
    param_22 = param;
    param_23 = param_1;
    sampledColor = v_17(&(param_22), &(param_23));
  }
  UVOffset = ((sampledColor.xy * 2.0f) - vec2<f32>(1.0f));
  UVOffset.y = (UVOffset.y * -1.0f);
  UVOffset.y = ((*(uvInversed)).x + ((*(uvInversed)).y * UVOffset.y));
  return UVOffset;
}

fn v_20(dst : ptr<function, vec4<f32>>, flipbookParameter : ptr<function, vec4<f32>>, vcolor : ptr<function, vec4<f32>>, nextUV : ptr<function, vec2<f32>>, flipbookRate : ptr<function, f32>, convertFromSRGB : ptr<function, bool>) {
  var sampledColor : vec4<f32>;
  var param : vec4<f32>;
  var param_1 : bool;
  var param_24 : vec4<f32>;
  var param_25 : bool;
  var NextPixelColor : vec4<f32>;
  if (((*(flipbookParameter)).x > 0.0f)) {
    sampledColor = textureSample(_colorTex, sampler_colorTex, *(nextUV));
    if (*(convertFromSRGB)) {
      param = sampledColor;
      param_1 = *(convertFromSRGB);
      param_24 = param;
      param_25 = param_1;
      sampledColor = v_17(&(param_24), &(param_25));
    }
    NextPixelColor = (sampledColor * *(vcolor));
    if (((*(flipbookParameter)).y == 1.0f)) {
      let v_21 = *(dst);
      let v_22 = NextPixelColor;
      let v_23 = *(flipbookRate);
      *(dst) = mix(v_21, v_22, vec4<f32>(v_23, v_23, v_23, v_23));
    }
  }
}

fn v_24(dstColor : ptr<function, vec4<f32>>, blendColor : ptr<function, vec4<f32>>, blendType : ptr<function, f32>) {
  var _169 : vec4<f32>;
  var _172 : vec3<f32>;
  var _187 : vec4<f32>;
  var _189 : vec3<f32>;
  var _204 : vec4<f32>;
  var _206 : vec3<f32>;
  var _221 : vec4<f32>;
  var _223 : vec3<f32>;
  if ((*(blendType) == 0.0f)) {
    _169 = *(dstColor);
    _172 = (((*(blendColor)).xyz * (*(blendColor)).w) + (_169.xyz * (1.0f - (*(blendColor)).w)));
    (*(dstColor)).x = _172.x;
    (*(dstColor)).y = _172.y;
    (*(dstColor)).z = _172.z;
  } else if ((*(blendType) == 1.0f)) {
    _187 = *(dstColor);
    _189 = (_187.xyz + ((*(blendColor)).xyz * (*(blendColor)).w));
    (*(dstColor)).x = _189.x;
    (*(dstColor)).y = _189.y;
    (*(dstColor)).z = _189.z;
  } else if ((*(blendType) == 2.0f)) {
    _204 = *(dstColor);
    _206 = (_204.xyz - ((*(blendColor)).xyz * (*(blendColor)).w));
    (*(dstColor)).x = _206.x;
    (*(dstColor)).y = _206.y;
    (*(dstColor)).z = _206.z;
  } else if ((*(blendType) == 3.0f)) {
    _221 = *(dstColor);
    _223 = (_221.xyz * ((*(blendColor)).xyz * (*(blendColor)).w));
    (*(dstColor)).x = _223.x;
    (*(dstColor)).y = _223.y;
    (*(dstColor)).z = _223.z;
  }
}

fn v_25(backgroundZ : ptr<function, f32>, meshZ : ptr<function, f32>, softparticleParam : ptr<function, vec4<f32>>, reconstruct1 : ptr<function, vec4<f32>>, reconstruct2 : ptr<function, vec4<f32>>) -> f32 {
  var distanceFar : f32;
  var distanceNear : f32;
  var distanceNearOffset : f32;
  var rescale : vec2<f32>;
  var params : vec4<f32>;
  var zs : vec2<f32>;
  var depth : vec2<f32>;
  var dir : f32;
  var alphaFar : f32;
  var alphaNear : f32;
  distanceFar = (*(softparticleParam)).x;
  distanceNear = (*(softparticleParam)).y;
  distanceNearOffset = (*(softparticleParam)).z;
  rescale = (*(reconstruct1)).xy;
  params = *(reconstruct2);
  zs = vec2<f32>(((*(backgroundZ) * rescale.x) + rescale.y), *(meshZ));
  let v_26 = (zs * params.w);
  let v_27 = params.y;
  let v_28 = params.x;
  depth = ((v_26 - vec2<f32>(v_27, v_27)) / (vec2<f32>(v_28, v_28) - (zs * params.z)));
  dir = sign(depth.x);
  let v_29 = dir;
  depth = (depth * v_29);
  alphaFar = ((depth.x - depth.y) / distanceFar);
  alphaNear = ((depth.y - distanceNearOffset) / distanceNear);
  return min(max(min(alphaFar, alphaNear), 0.0f), 1.0f);
}

fn v_30(Input : ptr<function, PS_Input>) -> vec4<f32> {
  var param : PS_Input;
  var advancedParam : AdvancedParameter;
  var param_26 : PS_Input;
  var param_1 : vec2<f32>;
  var param_2 : vec2<f32>;
  var param_3 : bool;
  var UVOffset : vec2<f32>;
  var param_27 : vec2<f32>;
  var param_28 : vec2<f32>;
  var param_29 : bool;
  var Output : vec4<f32>;
  var param_4 : vec4<f32>;
  var param_5 : f32;
  var param_6 : bool;
  var param_30 : vec4<f32>;
  var param_31 : vec4<f32>;
  var param_32 : vec4<f32>;
  var param_33 : vec2<f32>;
  var param_34 : f32;
  var param_35 : bool;
  var AlphaTexColor : vec4<f32>;
  var param_7 : vec2<f32>;
  var param_8 : vec2<f32>;
  var param_9 : bool;
  var BlendUVOffset : vec2<f32>;
  var param_36 : vec2<f32>;
  var param_37 : vec2<f32>;
  var param_38 : bool;
  var BlendTextureColor : vec4<f32>;
  var BlendAlphaTextureColor : vec4<f32>;
  var param_10 : vec4<f32>;
  var param_39 : vec4<f32>;
  var param_40 : vec4<f32>;
  var param_41 : f32;
  var pos : vec2<f32>;
  var posR : vec2<f32>;
  var posU : vec2<f32>;
  var xscale : f32;
  var yscale : f32;
  var uv : vec2<f32>;
  var color : vec3<f32>;
  var screenPos : vec4<f32>;
  var screenUV : vec2<f32>;
  var backgroundZ : f32;
  var param_11 : f32;
  var param_12 : f32;
  var param_13 : vec4<f32>;
  var param_14 : vec4<f32>;
  var param_15 : vec4<f32>;
  var param_42 : f32;
  var param_43 : f32;
  var param_44 : vec4<f32>;
  var param_45 : vec4<f32>;
  var param_46 : vec4<f32>;
  param = *(Input);
  param_26 = param;
  advancedParam = v_12(&(param_26));
  param_1 = advancedParam.UVDistortionUV;
  param_2 = v._393_fUVDistortionParameter.zw;
  param_3 = false;
  param_27 = param_1;
  param_28 = param_2;
  param_29 = param_3;
  UVOffset = v_18(&(param_27), &(param_28), &(param_29));
  let v_31 = v._393_fUVDistortionParameter.x;
  UVOffset = (UVOffset * v_31);
  Output = textureSample(_colorTex, sampler_colorTex, ((*(Input)).UV_Others.xy + UVOffset));
  let v_32 = (*(Input)).Color.w;
  Output.w = (Output.w * v_32);
  param_4 = Output;
  param_5 = advancedParam.FlipbookRate;
  param_6 = false;
  let v_33 = (advancedParam.FlipbookNextIndexUV + UVOffset);
  param_30 = param_4;
  param_31 = v._393_fFlipbookParameter;
  param_32 = (*(Input)).Color;
  param_33 = v_33;
  param_34 = param_5;
  param_35 = param_6;
  v_20(&(param_30), &(param_31), &(param_32), &(param_33), &(param_34), &(param_35));
  param_4 = param_30;
  Output = param_4;
  AlphaTexColor = textureSample(_alphaTex, sampler_alphaTex, (advancedParam.AlphaUV + UVOffset));
  let v_34 = (AlphaTexColor.x * AlphaTexColor.w);
  Output.w = (Output.w * v_34);
  param_7 = advancedParam.BlendUVDistortionUV;
  param_8 = v._393_fUVDistortionParameter.zw;
  param_9 = false;
  param_36 = param_7;
  param_37 = param_8;
  param_38 = param_9;
  BlendUVOffset = v_19(&(param_36), &(param_37), &(param_38));
  let v_35 = v._393_fUVDistortionParameter.y;
  BlendUVOffset = (BlendUVOffset * v_35);
  BlendTextureColor = textureSample(_blendTex, sampler_blendTex, (advancedParam.BlendUV + BlendUVOffset));
  BlendAlphaTextureColor = textureSample(_blendAlphaTex, sampler_blendAlphaTex, (advancedParam.BlendAlphaUV + BlendUVOffset));
  let v_36 = (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
  BlendTextureColor.w = (BlendTextureColor.w * v_36);
  param_10 = Output;
  param_39 = param_10;
  param_40 = BlendTextureColor;
  param_41 = v._393_fBlendTextureParameter.x;
  v_24(&(param_39), &(param_40), &(param_41));
  param_10 = param_39;
  Output = param_10;
  if ((Output.w <= max(0.0f, advancedParam.AlphaThreshold))) {
    discard;
    return vec4<f32>();
  }
  let v_37 = (*(Input)).PosP.xy;
  let v_38 = (*(Input)).PosP.w;
  pos = (v_37 / vec2<f32>(v_38, v_38));
  let v_39 = (*(Input)).ProjTangent.xy;
  let v_40 = (*(Input)).ProjTangent.w;
  posR = (v_39 / vec2<f32>(v_40, v_40));
  let v_41 = (*(Input)).ProjBinormal.xy;
  let v_42 = (*(Input)).ProjBinormal.w;
  posU = (v_41 / vec2<f32>(v_42, v_42));
  xscale = ((((Output.x * 2.0f) - 1.0f) * (*(Input)).Color.x) * v._393_g_scale.x);
  yscale = ((((Output.y * 2.0f) - 1.0f) * (*(Input)).Color.y) * v._393_g_scale.x);
  uv = ((pos + ((posR - pos) * xscale)) + ((posU - pos) * yscale));
  uv.x = ((uv.x + 1.0f) * 0.5f);
  uv.y = (1.0f - ((uv.y + 1.0f) * 0.5f));
  uv.y = (v._393_mUVInversedBack.x + (v._393_mUVInversedBack.y * uv.y));
  color = textureSample(_backTex, sampler_backTex, uv).xyz;
  Output.x = color.x;
  Output.y = color.y;
  Output.z = color.z;
  let v_43 = (*(Input)).PosP;
  let v_44 = (*(Input)).PosP.w;
  screenPos = (v_43 / vec4<f32>(v_44, v_44, v_44, v_44));
  screenUV = ((screenPos.xy + vec2<f32>(1.0f)) / vec2<f32>(2.0f));
  screenUV.y = (1.0f - screenUV.y);
  if (!((v._393_softParticleParam.w == 0.0f))) {
    backgroundZ = textureSample(_depthTex, sampler_depthTex, screenUV).x;
    param_11 = backgroundZ;
    param_12 = screenPos.z;
    param_13 = v._393_softParticleParam;
    param_14 = v._393_reconstructionParam1;
    param_15 = v._393_reconstructionParam2;
    param_42 = param_11;
    param_43 = param_12;
    param_44 = param_13;
    param_45 = param_14;
    param_46 = param_15;
    let v_45 = v_25(&(param_42), &(param_43), &(param_44), &(param_45), &(param_46));
    Output.w = (Output.w * v_45);
  }
  return Output;
}

fn v_46() {
  var Input : PS_Input;
  var _706 : vec4<f32>;
  var param : PS_Input;
  Input.PosVS = gl_FragCoord;
  Input.UV_Others = Input_UV_Others;
  Input.ProjBinormal = Input_ProjBinormal;
  Input.ProjTangent = Input_ProjTangent;
  Input.PosP = Input_PosP;
  Input.Color = Input_Color;
  Input.Alpha_Dist_UV = Input_Alpha_Dist_UV;
  Input.Blend_Alpha_Dist_UV = Input_Blend_Alpha_Dist_UV;
  Input.Blend_FBNextIndex_UV = Input_Blend_FBNextIndex_UV;
  param = Input;
  _706 = v_30(&(param));
  _entryPointOutput = _706;
}

struct SPIRV_Cross_Output {
  _entryPointOutput : vec4<f32>,
}

fn v_11(stage_input : ptr<function, SPIRV_Cross_Input>) -> SPIRV_Cross_Output {
  var stage_output : SPIRV_Cross_Output;
  gl_FragCoord = (*(stage_input)).gl_FragCoord;
  gl_FragCoord.w = (1.0f / gl_FragCoord.w);
  Input_UV_Others = (*(stage_input)).Input_UV_Others;
  Input_ProjBinormal = (*(stage_input)).Input_ProjBinormal;
  Input_ProjTangent = (*(stage_input)).Input_ProjTangent;
  Input_PosP = (*(stage_input)).Input_PosP;
  Input_Color = (*(stage_input)).Input_Color;
  Input_Alpha_Dist_UV = (*(stage_input)).Input_Alpha_Dist_UV;
  Input_Blend_Alpha_Dist_UV = (*(stage_input)).Input_Blend_Alpha_Dist_UV;
  Input_Blend_FBNextIndex_UV = (*(stage_input)).Input_Blend_FBNextIndex_UV;
  v_46();
  stage_output._entryPointOutput = _entryPointOutput;
  return stage_output;
}

@fragment
fn main(@location(0u) @interpolate(perspective, centroid) v_47 : vec4<f32>, @location(1u) v_48 : vec4<f32>, @location(2u) v_49 : vec4<f32>, @location(3u) v_50 : vec4<f32>, @location(4u) @interpolate(perspective, centroid) v_51 : vec4<f32>, @location(5u) v_52 : vec4<f32>, @location(6u) v_53 : vec4<f32>, @location(7u) v_54 : vec4<f32>, @builtin(position) v_55 : vec4<f32>) -> @location(0u) vec4<f32> {
  main_inner(v_47, v_48, v_49, v_50, v_51, v_52, v_53, v_54, v_55);
  return v_1;
}
