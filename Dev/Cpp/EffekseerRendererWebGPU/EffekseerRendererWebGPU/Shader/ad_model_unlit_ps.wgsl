diagnostic(off, derivative_uniformity);

@group(1) @binding(2) var _uvDistortionTex : texture_2d<f32>;

@group(2) @binding(2) var sampler_uvDistortionTex : sampler;

@group(1) @binding(5) var _blendUVDistortionTex : texture_2d<f32>;

@group(2) @binding(5) var sampler_blendUVDistortionTex : sampler;

@group(1) @binding(0) var _colorTex : texture_2d<f32>;

@group(2) @binding(0) var sampler_colorTex : sampler;

struct PS_ConstantBuffer {
  _433_fLightDirection : vec4<f32>,
  _433_fLightColor : vec4<f32>,
  _433_fLightAmbient : vec4<f32>,
  _433_fFlipbookParameter : vec4<f32>,
  _433_fUVDistortionParameter : vec4<f32>,
  _433_fBlendTextureParameter : vec4<f32>,
  _433_fCameraFrontDirection : vec4<f32>,
  _433_fFalloffParameter : vec4<f32>,
  _433_fFalloffBeginColor : vec4<f32>,
  _433_fFalloffEndColor : vec4<f32>,
  _433_fEmissiveScaling : vec4<f32>,
  _433_fEdgeColor : vec4<f32>,
  _433_fEdgeParameter : vec4<f32>,
  _433_softParticleParam : vec4<f32>,
  _433_reconstructionParam1 : vec4<f32>,
  _433_reconstructionParam2 : vec4<f32>,
  _433_mUVInversedBack : vec4<f32>,
  _433_miscFlags : vec4<f32>,
}

@group(0) @binding(1) var<uniform> v : PS_ConstantBuffer;

@group(1) @binding(1) var _alphaTex : texture_2d<f32>;

@group(2) @binding(1) var sampler_alphaTex : sampler;

@group(1) @binding(3) var _blendTex : texture_2d<f32>;

@group(2) @binding(3) var sampler_blendTex : sampler;

@group(1) @binding(4) var _blendAlphaTex : texture_2d<f32>;

@group(2) @binding(4) var sampler_blendAlphaTex : sampler;

@group(1) @binding(6) var _depthTex : texture_2d<f32>;

@group(2) @binding(6) var sampler_depthTex : sampler;

var<private> gl_FragCoord : vec4<f32>;

var<private> Input_Color : vec4<f32>;

var<private> Input_UV_Others : vec4<f32>;

var<private> Input_WorldN : vec3<f32>;

var<private> Input_Alpha_Dist_UV : vec4<f32>;

var<private> Input_Blend_Alpha_Dist_UV : vec4<f32>;

var<private> Input_Blend_FBNextIndex_UV : vec4<f32>;

var<private> Input_PosP : vec4<f32>;

var<private> _entryPointOutput : vec4<f32>;

var<private> v_1 : vec4<f32>;

struct SPIRV_Cross_Input {
  Input_Color : vec4<f32>,
  Input_UV_Others : vec4<f32>,
  Input_WorldN : vec3<f32>,
  Input_Alpha_Dist_UV : vec4<f32>,
  Input_Blend_Alpha_Dist_UV : vec4<f32>,
  Input_Blend_FBNextIndex_UV : vec4<f32>,
  Input_PosP : vec4<f32>,
  gl_FragCoord : vec4<f32>,
}

fn main_inner(v_2 : vec4<f32>, v_3 : vec4<f32>, v_4 : vec3<f32>, v_5 : vec4<f32>, v_6 : vec4<f32>, v_7 : vec4<f32>, v_8 : vec4<f32>, v_9 : vec4<f32>) {
  var stage_input : SPIRV_Cross_Input;
  var param : SPIRV_Cross_Input;
  stage_input.Input_Color = v_2;
  stage_input.Input_UV_Others = v_3;
  stage_input.Input_WorldN = v_4;
  stage_input.Input_Alpha_Dist_UV = v_5;
  stage_input.Input_Blend_Alpha_Dist_UV = v_6;
  stage_input.Input_Blend_FBNextIndex_UV = v_7;
  stage_input.Input_PosP = v_8;
  stage_input.gl_FragCoord = v_9;
  param = stage_input;
  v_1 = v_10(&(param))._entryPointOutput;
}

struct PS_Input {
  PosVS : vec4<f32>,
  Color : vec4<f32>,
  UV_Others : vec4<f32>,
  WorldN : vec3<f32>,
  Alpha_Dist_UV : vec4<f32>,
  Blend_Alpha_Dist_UV : vec4<f32>,
  Blend_FBNextIndex_UV : vec4<f32>,
  PosP : vec4<f32>,
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

fn v_11(psinput : ptr<function, PS_Input>) -> AdvancedParameter {
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

fn v_12(base : ptr<function, vec3<f32>>, power : ptr<function, vec3<f32>>) -> vec3<f32> {
  return pow(max(abs(*(base)), vec3<f32>(0.00000011920928955078f)), *(power));
}

fn v_13(c : ptr<function, vec3<f32>>) -> vec3<f32> {
  var param : vec3<f32>;
  var param_1 : vec3<f32>;
  var param_26 : vec3<f32>;
  var param_27 : vec3<f32>;
  param = *(c);
  param_1 = vec3<f32>(0.4166666567325592041f);
  param_26 = param;
  param_27 = param_1;
  return max(((v_12(&(param_26), &(param_27)) * 1.05499994754791259766f) - vec3<f32>(0.05499999970197677612f)), vec3<f32>());
}

fn v_14(c : ptr<function, vec4<f32>>) -> vec4<f32> {
  var param : vec3<f32>;
  var param_28 : vec3<f32>;
  param = (*(c)).xyz;
  param_28 = param;
  let v_15 = v_13(&(param_28));
  return vec4<f32>(v_15.x, v_15.y, v_15.z, (*(c)).w);
}

fn v_16(c : ptr<function, vec4<f32>>, isValid : ptr<function, bool>) -> vec4<f32> {
  var param : vec4<f32>;
  var param_29 : vec4<f32>;
  if (!(*(isValid))) {
    return *(c);
  }
  param = *(c);
  param_29 = param;
  return v_14(&(param_29));
}

fn v_17(uv : ptr<function, vec2<f32>>, uvInversed : ptr<function, vec2<f32>>, convertFromSRGB : ptr<function, bool>) -> vec2<f32> {
  var sampledColor : vec4<f32>;
  var param : vec4<f32>;
  var param_1 : bool;
  var param_30 : vec4<f32>;
  var param_31 : bool;
  var UVOffset : vec2<f32>;
  sampledColor = textureSample(_uvDistortionTex, sampler_uvDistortionTex, *(uv));
  if (*(convertFromSRGB)) {
    param = sampledColor;
    param_1 = *(convertFromSRGB);
    param_30 = param;
    param_31 = param_1;
    sampledColor = v_16(&(param_30), &(param_31));
  }
  UVOffset = ((sampledColor.xy * 2.0f) - vec2<f32>(1.0f));
  UVOffset.y = (UVOffset.y * -1.0f);
  UVOffset.y = ((*(uvInversed)).x + ((*(uvInversed)).y * UVOffset.y));
  return UVOffset;
}

fn v_18(uv : ptr<function, vec2<f32>>, uvInversed : ptr<function, vec2<f32>>, convertFromSRGB : ptr<function, bool>) -> vec2<f32> {
  var sampledColor : vec4<f32>;
  var param : vec4<f32>;
  var param_1 : bool;
  var param_32 : vec4<f32>;
  var param_33 : bool;
  var UVOffset : vec2<f32>;
  sampledColor = textureSample(_blendUVDistortionTex, sampler_blendUVDistortionTex, *(uv));
  if (*(convertFromSRGB)) {
    param = sampledColor;
    param_1 = *(convertFromSRGB);
    param_32 = param;
    param_33 = param_1;
    sampledColor = v_16(&(param_32), &(param_33));
  }
  UVOffset = ((sampledColor.xy * 2.0f) - vec2<f32>(1.0f));
  UVOffset.y = (UVOffset.y * -1.0f);
  UVOffset.y = ((*(uvInversed)).x + ((*(uvInversed)).y * UVOffset.y));
  return UVOffset;
}

fn v_19(dst : ptr<function, vec4<f32>>, flipbookParameter : ptr<function, vec4<f32>>, vcolor : ptr<function, vec4<f32>>, nextUV : ptr<function, vec2<f32>>, flipbookRate : ptr<function, f32>, convertFromSRGB : ptr<function, bool>) {
  var sampledColor : vec4<f32>;
  var param : vec4<f32>;
  var param_1 : bool;
  var param_34 : vec4<f32>;
  var param_35 : bool;
  var NextPixelColor : vec4<f32>;
  if (((*(flipbookParameter)).x > 0.0f)) {
    sampledColor = textureSample(_colorTex, sampler_colorTex, *(nextUV));
    if (*(convertFromSRGB)) {
      param = sampledColor;
      param_1 = *(convertFromSRGB);
      param_34 = param;
      param_35 = param_1;
      sampledColor = v_16(&(param_34), &(param_35));
    }
    NextPixelColor = (sampledColor * *(vcolor));
    if (((*(flipbookParameter)).y == 1.0f)) {
      let v_20 = *(dst);
      let v_21 = NextPixelColor;
      let v_22 = *(flipbookRate);
      *(dst) = mix(v_20, v_21, vec4<f32>(v_22, v_22, v_22, v_22));
    }
  }
}

fn v_23(dstColor : ptr<function, vec4<f32>>, blendColor : ptr<function, vec4<f32>>, blendType : ptr<function, f32>) {
  var _219 : vec4<f32>;
  var _222 : vec3<f32>;
  var _237 : vec4<f32>;
  var _239 : vec3<f32>;
  var _254 : vec4<f32>;
  var _256 : vec3<f32>;
  var _271 : vec4<f32>;
  var _273 : vec3<f32>;
  if ((*(blendType) == 0.0f)) {
    _219 = *(dstColor);
    _222 = (((*(blendColor)).xyz * (*(blendColor)).w) + (_219.xyz * (1.0f - (*(blendColor)).w)));
    (*(dstColor)).x = _222.x;
    (*(dstColor)).y = _222.y;
    (*(dstColor)).z = _222.z;
  } else if ((*(blendType) == 1.0f)) {
    _237 = *(dstColor);
    _239 = (_237.xyz + ((*(blendColor)).xyz * (*(blendColor)).w));
    (*(dstColor)).x = _239.x;
    (*(dstColor)).y = _239.y;
    (*(dstColor)).z = _239.z;
  } else if ((*(blendType) == 2.0f)) {
    _254 = *(dstColor);
    _256 = (_254.xyz - ((*(blendColor)).xyz * (*(blendColor)).w));
    (*(dstColor)).x = _256.x;
    (*(dstColor)).y = _256.y;
    (*(dstColor)).z = _256.z;
  } else if ((*(blendType) == 3.0f)) {
    _271 = *(dstColor);
    _273 = (_271.xyz * ((*(blendColor)).xyz * (*(blendColor)).w));
    (*(dstColor)).x = _273.x;
    (*(dstColor)).y = _273.y;
    (*(dstColor)).z = _273.z;
  }
}

fn v_24(backgroundZ : ptr<function, f32>, meshZ : ptr<function, f32>, softparticleParam : ptr<function, vec4<f32>>, reconstruct1 : ptr<function, vec4<f32>>, reconstruct2 : ptr<function, vec4<f32>>) -> f32 {
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
  let v_25 = (zs * params.w);
  let v_26 = params.y;
  let v_27 = params.x;
  depth = ((v_25 - vec2<f32>(v_26, v_26)) / (vec2<f32>(v_27, v_27) - (zs * params.z)));
  dir = sign(depth.x);
  let v_28 = dir;
  depth = (depth * v_28);
  alphaFar = ((depth.x - depth.y) / distanceFar);
  alphaNear = ((depth.y - distanceNearOffset) / distanceNear);
  return min(max(min(alphaFar, alphaNear), 0.0f), 1.0f);
}

fn v_29(c : ptr<function, vec3<f32>>) -> vec3<f32> {
  return min(*(c), (*(c) * ((*(c) * ((*(c) * 0.30530601739883422852f) + vec3<f32>(0.68217110633850097656f))) + vec3<f32>(0.01252287812530994415f))));
}

fn v_30(c : ptr<function, vec4<f32>>) -> vec4<f32> {
  var param : vec3<f32>;
  var param_36 : vec3<f32>;
  param = (*(c)).xyz;
  param_36 = param;
  let v_31 = v_29(&(param_36));
  return vec4<f32>(v_31.x, v_31.y, v_31.z, (*(c)).w);
}

fn v_32(c : ptr<function, vec4<f32>>, isValid : ptr<function, bool>) -> vec4<f32> {
  var param : vec4<f32>;
  var param_37 : vec4<f32>;
  if (!(*(isValid))) {
    return *(c);
  }
  param = *(c);
  param_37 = param;
  return v_30(&(param_37));
}

fn v_33(Input : ptr<function, PS_Input>) -> vec4<f32> {
  var convertColorSpace : bool;
  var param : PS_Input;
  var advancedParam : AdvancedParameter;
  var param_38 : PS_Input;
  var param_1 : vec2<f32>;
  var param_2 : vec2<f32>;
  var param_3 : bool;
  var UVOffset : vec2<f32>;
  var param_39 : vec2<f32>;
  var param_40 : vec2<f32>;
  var param_41 : bool;
  var param_4 : vec4<f32>;
  var param_5 : bool;
  var Output : vec4<f32>;
  var param_42 : vec4<f32>;
  var param_43 : bool;
  var param_6 : vec4<f32>;
  var param_7 : f32;
  var param_8 : bool;
  var param_44 : vec4<f32>;
  var param_45 : vec4<f32>;
  var param_46 : vec4<f32>;
  var param_47 : vec2<f32>;
  var param_48 : f32;
  var param_49 : bool;
  var param_9 : vec4<f32>;
  var param_10 : bool;
  var AlphaTexColor : vec4<f32>;
  var param_50 : vec4<f32>;
  var param_51 : bool;
  var param_11 : vec2<f32>;
  var param_12 : vec2<f32>;
  var param_13 : bool;
  var BlendUVOffset : vec2<f32>;
  var param_52 : vec2<f32>;
  var param_53 : vec2<f32>;
  var param_54 : bool;
  var param_14 : vec4<f32>;
  var param_15 : bool;
  var BlendTextureColor : vec4<f32>;
  var param_55 : vec4<f32>;
  var param_56 : bool;
  var param_16 : vec4<f32>;
  var param_17 : bool;
  var BlendAlphaTextureColor : vec4<f32>;
  var param_57 : vec4<f32>;
  var param_58 : bool;
  var param_18 : vec4<f32>;
  var param_59 : vec4<f32>;
  var param_60 : vec4<f32>;
  var param_61 : f32;
  var cameraVec : vec3<f32>;
  var CdotN : f32;
  var FalloffBlendColor : vec4<f32>;
  var _625 : vec4<f32>;
  var _627 : vec3<f32>;
  var _642 : vec4<f32>;
  var _644 : vec3<f32>;
  var _659 : vec4<f32>;
  var _661 : vec3<f32>;
  var _677 : vec4<f32>;
  var _679 : vec3<f32>;
  var screenPos : vec4<f32>;
  var screenUV : vec2<f32>;
  var backgroundZ : f32;
  var param_19 : f32;
  var param_20 : f32;
  var param_21 : vec4<f32>;
  var param_22 : vec4<f32>;
  var param_23 : vec4<f32>;
  var param_62 : f32;
  var param_63 : f32;
  var param_64 : vec4<f32>;
  var param_65 : vec4<f32>;
  var param_66 : vec4<f32>;
  var _765 : vec4<f32>;
  var _768 : f32;
  var _777 : vec3<f32>;
  var param_24 : vec4<f32>;
  var param_25 : bool;
  var param_67 : vec4<f32>;
  var param_68 : bool;
  convertColorSpace = !((v._433_miscFlags.x == 0.0f));
  param = *(Input);
  param_38 = param;
  advancedParam = v_11(&(param_38));
  param_1 = advancedParam.UVDistortionUV;
  param_2 = v._433_fUVDistortionParameter.zw;
  param_3 = convertColorSpace;
  param_39 = param_1;
  param_40 = param_2;
  param_41 = param_3;
  UVOffset = v_17(&(param_39), &(param_40), &(param_41));
  let v_34 = v._433_fUVDistortionParameter.x;
  UVOffset = (UVOffset * v_34);
  param_4 = textureSample(_colorTex, sampler_colorTex, ((*(Input)).UV_Others.xy + UVOffset));
  param_5 = convertColorSpace;
  param_42 = param_4;
  param_43 = param_5;
  Output = (v_16(&(param_42), &(param_43)) * (*(Input)).Color);
  param_6 = Output;
  param_7 = advancedParam.FlipbookRate;
  param_8 = convertColorSpace;
  let v_35 = (advancedParam.FlipbookNextIndexUV + UVOffset);
  param_44 = param_6;
  param_45 = v._433_fFlipbookParameter;
  param_46 = (*(Input)).Color;
  param_47 = v_35;
  param_48 = param_7;
  param_49 = param_8;
  v_19(&(param_44), &(param_45), &(param_46), &(param_47), &(param_48), &(param_49));
  param_6 = param_44;
  Output = param_6;
  param_9 = textureSample(_alphaTex, sampler_alphaTex, (advancedParam.AlphaUV + UVOffset));
  param_10 = convertColorSpace;
  param_50 = param_9;
  param_51 = param_10;
  AlphaTexColor = v_16(&(param_50), &(param_51));
  let v_36 = (AlphaTexColor.x * AlphaTexColor.w);
  Output.w = (Output.w * v_36);
  param_11 = advancedParam.BlendUVDistortionUV;
  param_12 = v._433_fUVDistortionParameter.zw;
  param_13 = convertColorSpace;
  param_52 = param_11;
  param_53 = param_12;
  param_54 = param_13;
  BlendUVOffset = v_18(&(param_52), &(param_53), &(param_54));
  let v_37 = v._433_fUVDistortionParameter.y;
  BlendUVOffset = (BlendUVOffset * v_37);
  param_14 = textureSample(_blendTex, sampler_blendTex, (advancedParam.BlendUV + BlendUVOffset));
  param_15 = convertColorSpace;
  param_55 = param_14;
  param_56 = param_15;
  BlendTextureColor = v_16(&(param_55), &(param_56));
  param_16 = textureSample(_blendAlphaTex, sampler_blendAlphaTex, (advancedParam.BlendAlphaUV + BlendUVOffset));
  param_17 = convertColorSpace;
  param_57 = param_16;
  param_58 = param_17;
  BlendAlphaTextureColor = v_16(&(param_57), &(param_58));
  let v_38 = (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
  BlendTextureColor.w = (BlendTextureColor.w * v_38);
  param_18 = Output;
  param_59 = param_18;
  param_60 = BlendTextureColor;
  param_61 = v._433_fBlendTextureParameter.x;
  v_23(&(param_59), &(param_60), &(param_61));
  param_18 = param_59;
  Output = param_18;
  if ((v._433_fFalloffParameter.x == 1.0f)) {
    cameraVec = normalize(-(v._433_fCameraFrontDirection.xyz));
    CdotN = clamp(dot(cameraVec, normalize((*(Input)).WorldN)), 0.0f, 1.0f);
    let v_39 = v._433_fFalloffEndColor;
    let v_40 = v._433_fFalloffBeginColor;
    let v_41 = pow(CdotN, v._433_fFalloffParameter.z);
    FalloffBlendColor = mix(v_39, v_40, vec4<f32>(v_41, v_41, v_41, v_41));
    if ((v._433_fFalloffParameter.y == 0.0f)) {
      _625 = Output;
      _627 = (_625.xyz + FalloffBlendColor.xyz);
      Output.x = _627.x;
      Output.y = _627.y;
      Output.z = _627.z;
    } else if ((v._433_fFalloffParameter.y == 1.0f)) {
      _642 = Output;
      _644 = (_642.xyz - FalloffBlendColor.xyz);
      Output.x = _644.x;
      Output.y = _644.y;
      Output.z = _644.z;
    } else if ((v._433_fFalloffParameter.y == 2.0f)) {
      _659 = Output;
      _661 = (_659.xyz * FalloffBlendColor.xyz);
      Output.x = _661.x;
      Output.y = _661.y;
      Output.z = _661.z;
    }
    let v_42 = FalloffBlendColor.w;
    Output.w = (Output.w * v_42);
  }
  _677 = Output;
  _679 = (_677.xyz * v._433_fEmissiveScaling.x);
  Output.x = _679.x;
  Output.y = _679.y;
  Output.z = _679.z;
  let v_43 = (*(Input)).PosP;
  let v_44 = (*(Input)).PosP.w;
  screenPos = (v_43 / vec4<f32>(v_44, v_44, v_44, v_44));
  screenUV = ((screenPos.xy + vec2<f32>(1.0f)) / vec2<f32>(2.0f));
  screenUV.y = (1.0f - screenUV.y);
  screenUV.y = (v._433_mUVInversedBack.x + (v._433_mUVInversedBack.y * screenUV.y));
  if (!((v._433_softParticleParam.w == 0.0f))) {
    backgroundZ = textureSample(_depthTex, sampler_depthTex, screenUV).x;
    param_19 = backgroundZ;
    param_20 = screenPos.z;
    param_21 = v._433_softParticleParam;
    param_22 = v._433_reconstructionParam1;
    param_23 = v._433_reconstructionParam2;
    param_62 = param_19;
    param_63 = param_20;
    param_64 = param_21;
    param_65 = param_22;
    param_66 = param_23;
    let v_45 = v_24(&(param_62), &(param_63), &(param_64), &(param_65), &(param_66));
    Output.w = (Output.w * v_45);
  }
  if ((Output.w <= max(0.0f, advancedParam.AlphaThreshold))) {
    discard;
    return vec4<f32>();
  }
  _765 = Output;
  _768 = Output.w;
  let v_46 = (v._433_fEdgeColor.xyz * v._433_fEdgeParameter.y);
  let v_47 = _765.xyz;
  let v_48 = ceil(((_768 - advancedParam.AlphaThreshold) - v._433_fEdgeParameter.x));
  _777 = mix(v_46, v_47, vec3<f32>(v_48, v_48, v_48));
  Output.x = _777.x;
  Output.y = _777.y;
  Output.z = _777.z;
  param_24 = Output;
  param_25 = convertColorSpace;
  param_67 = param_24;
  param_68 = param_25;
  return v_32(&(param_67), &(param_68));
}

fn v_49() {
  var Input : PS_Input;
  var _821 : vec4<f32>;
  var param : PS_Input;
  Input.PosVS = gl_FragCoord;
  Input.Color = Input_Color;
  Input.UV_Others = Input_UV_Others;
  Input.WorldN = Input_WorldN;
  Input.Alpha_Dist_UV = Input_Alpha_Dist_UV;
  Input.Blend_Alpha_Dist_UV = Input_Blend_Alpha_Dist_UV;
  Input.Blend_FBNextIndex_UV = Input_Blend_FBNextIndex_UV;
  Input.PosP = Input_PosP;
  param = Input;
  _821 = v_33(&(param));
  _entryPointOutput = _821;
}

struct SPIRV_Cross_Output {
  _entryPointOutput : vec4<f32>,
}

fn v_10(stage_input : ptr<function, SPIRV_Cross_Input>) -> SPIRV_Cross_Output {
  var stage_output : SPIRV_Cross_Output;
  gl_FragCoord = (*(stage_input)).gl_FragCoord;
  gl_FragCoord.w = (1.0f / gl_FragCoord.w);
  Input_Color = (*(stage_input)).Input_Color;
  Input_UV_Others = (*(stage_input)).Input_UV_Others;
  Input_WorldN = (*(stage_input)).Input_WorldN;
  Input_Alpha_Dist_UV = (*(stage_input)).Input_Alpha_Dist_UV;
  Input_Blend_Alpha_Dist_UV = (*(stage_input)).Input_Blend_Alpha_Dist_UV;
  Input_Blend_FBNextIndex_UV = (*(stage_input)).Input_Blend_FBNextIndex_UV;
  Input_PosP = (*(stage_input)).Input_PosP;
  v_49();
  stage_output._entryPointOutput = _entryPointOutput;
  return stage_output;
}

@fragment
fn main(@location(0u) @interpolate(perspective, centroid) v_50 : vec4<f32>, @location(1u) @interpolate(perspective, centroid) v_51 : vec4<f32>, @location(2u) v_52 : vec3<f32>, @location(3u) v_53 : vec4<f32>, @location(4u) v_54 : vec4<f32>, @location(5u) v_55 : vec4<f32>, @location(6u) v_56 : vec4<f32>, @builtin(position) v_57 : vec4<f32>) -> @location(0u) vec4<f32> {
  main_inner(v_50, v_51, v_52, v_53, v_54, v_55, v_56, v_57);
  return v_1;
}
