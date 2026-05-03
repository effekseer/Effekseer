diagnostic(off, derivative_uniformity);

struct PS_ConstantBuffer {
  _225_fLightDirection : vec4<f32>,
  _225_fLightColor : vec4<f32>,
  _225_fLightAmbient : vec4<f32>,
  _225_fFlipbookParameter : vec4<f32>,
  _225_fUVDistortionParameter : vec4<f32>,
  _225_fBlendTextureParameter : vec4<f32>,
  _225_fCameraFrontDirection : vec4<f32>,
  _225_fFalloffParameter : vec4<f32>,
  _225_fFalloffBeginColor : vec4<f32>,
  _225_fFalloffEndColor : vec4<f32>,
  _225_fEmissiveScaling : vec4<f32>,
  _225_fEdgeColor : vec4<f32>,
  _225_fEdgeParameter : vec4<f32>,
  _225_softParticleParam : vec4<f32>,
  _225_reconstructionParam1 : vec4<f32>,
  _225_reconstructionParam2 : vec4<f32>,
  _225_mUVInversedBack : vec4<f32>,
  _225_miscFlags : vec4<f32>,
}

@group(0) @binding(1) var<uniform> v : PS_ConstantBuffer;

@group(1) @binding(0) var _colorTex : texture_2d<f32>;

@group(2) @binding(0) var sampler_colorTex : sampler;

@group(1) @binding(1) var _depthTex : texture_2d<f32>;

@group(2) @binding(1) var sampler_depthTex : sampler;

var<private> gl_FragCoord : vec4<f32>;

var<private> Input_Color : vec4<f32>;

var<private> Input_UV : vec2<f32>;

var<private> Input_PosP : vec4<f32>;

var<private> _entryPointOutput : vec4<f32>;

var<private> v_1 : vec4<f32>;

struct SPIRV_Cross_Input {
  Input_Color : vec4<f32>,
  Input_UV : vec2<f32>,
  Input_PosP : vec4<f32>,
  gl_FragCoord : vec4<f32>,
}

fn main_inner(v_2 : vec4<f32>, v_3 : vec2<f32>, v_4 : vec4<f32>, v_5 : vec4<f32>) {
  var stage_input : SPIRV_Cross_Input;
  var param : SPIRV_Cross_Input;
  stage_input.Input_Color = v_2;
  stage_input.Input_UV = v_3;
  stage_input.Input_PosP = v_4;
  stage_input.gl_FragCoord = v_5;
  param = stage_input;
  v_1 = v_6(&(param))._entryPointOutput;
}

fn v_7(base : ptr<function, vec3<f32>>, power : ptr<function, vec3<f32>>) -> vec3<f32> {
  return pow(max(abs(*(base)), vec3<f32>(0.00000011920928955078f)), *(power));
}

fn v_8(c : ptr<function, vec3<f32>>) -> vec3<f32> {
  var param : vec3<f32>;
  var param_1 : vec3<f32>;
  var param_9 : vec3<f32>;
  var param_10 : vec3<f32>;
  param = *(c);
  param_1 = vec3<f32>(0.4166666567325592041f);
  param_9 = param;
  param_10 = param_1;
  return max(((v_7(&(param_9), &(param_10)) * 1.05499994754791259766f) - vec3<f32>(0.05499999970197677612f)), vec3<f32>());
}

fn v_9(c : ptr<function, vec4<f32>>) -> vec4<f32> {
  var param : vec3<f32>;
  var param_11 : vec3<f32>;
  param = (*(c)).xyz;
  param_11 = param;
  let v_10 = v_8(&(param_11));
  return vec4<f32>(v_10.x, v_10.y, v_10.z, (*(c)).w);
}

fn v_11(c : ptr<function, vec4<f32>>, isValid : ptr<function, bool>) -> vec4<f32> {
  var param : vec4<f32>;
  var param_12 : vec4<f32>;
  if (!(*(isValid))) {
    return *(c);
  }
  param = *(c);
  param_12 = param;
  return v_9(&(param_12));
}

fn v_12(backgroundZ : ptr<function, f32>, meshZ : ptr<function, f32>, softparticleParam : ptr<function, vec4<f32>>, reconstruct1 : ptr<function, vec4<f32>>, reconstruct2 : ptr<function, vec4<f32>>) -> f32 {
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
  let v_13 = (zs * params.w);
  let v_14 = params.y;
  let v_15 = params.x;
  depth = ((v_13 - vec2<f32>(v_14, v_14)) / (vec2<f32>(v_15, v_15) - (zs * params.z)));
  dir = sign(depth.x);
  let v_16 = dir;
  depth = (depth * v_16);
  alphaFar = ((depth.x - depth.y) / distanceFar);
  alphaNear = ((depth.y - distanceNearOffset) / distanceNear);
  return min(max(min(alphaFar, alphaNear), 0.0f), 1.0f);
}

fn v_17(c : ptr<function, vec3<f32>>) -> vec3<f32> {
  return min(*(c), (*(c) * ((*(c) * ((*(c) * 0.30530601739883422852f) + vec3<f32>(0.68217110633850097656f))) + vec3<f32>(0.01252287812530994415f))));
}

fn v_18(c : ptr<function, vec4<f32>>) -> vec4<f32> {
  var param : vec3<f32>;
  var param_13 : vec3<f32>;
  param = (*(c)).xyz;
  param_13 = param;
  let v_19 = v_17(&(param_13));
  return vec4<f32>(v_19.x, v_19.y, v_19.z, (*(c)).w);
}

fn v_20(c : ptr<function, vec4<f32>>, isValid : ptr<function, bool>) -> vec4<f32> {
  var param : vec4<f32>;
  var param_14 : vec4<f32>;
  if (!(*(isValid))) {
    return *(c);
  }
  param = *(c);
  param_14 = param;
  return v_18(&(param_14));
}

struct PS_Input {
  PosVS : vec4<f32>,
  Color : vec4<f32>,
  UV : vec2<f32>,
  PosP : vec4<f32>,
}

fn v_21(Input : ptr<function, PS_Input>) -> vec4<f32> {
  var convertColorSpace : bool;
  var param : vec4<f32>;
  var param_1 : bool;
  var Output : vec4<f32>;
  var param_15 : vec4<f32>;
  var param_16 : bool;
  var _256 : vec4<f32>;
  var _258 : vec3<f32>;
  var screenPos : vec4<f32>;
  var screenUV : vec2<f32>;
  var backgroundZ : f32;
  var param_2 : f32;
  var param_3 : f32;
  var param_4 : vec4<f32>;
  var param_5 : vec4<f32>;
  var param_6 : vec4<f32>;
  var param_17 : f32;
  var param_18 : f32;
  var param_19 : vec4<f32>;
  var param_20 : vec4<f32>;
  var param_21 : vec4<f32>;
  var param_7 : vec4<f32>;
  var param_8 : bool;
  var param_22 : vec4<f32>;
  var param_23 : bool;
  convertColorSpace = !((v._225_miscFlags.x == 0.0f));
  param = textureSample(_colorTex, sampler_colorTex, (*(Input)).UV);
  param_1 = convertColorSpace;
  param_15 = param;
  param_16 = param_1;
  Output = (v_11(&(param_15), &(param_16)) * (*(Input)).Color);
  _256 = Output;
  _258 = (_256.xyz * v._225_fEmissiveScaling.x);
  Output.x = _258.x;
  Output.y = _258.y;
  Output.z = _258.z;
  let v_22 = (*(Input)).PosP;
  let v_23 = (*(Input)).PosP.w;
  screenPos = (v_22 / vec4<f32>(v_23, v_23, v_23, v_23));
  screenUV = ((screenPos.xy + vec2<f32>(1.0f)) / vec2<f32>(2.0f));
  screenUV.y = (1.0f - screenUV.y);
  screenUV.y = (v._225_mUVInversedBack.x + (v._225_mUVInversedBack.y * screenUV.y));
  if (!((v._225_softParticleParam.w == 0.0f))) {
    backgroundZ = textureSample(_depthTex, sampler_depthTex, screenUV).x;
    param_2 = backgroundZ;
    param_3 = screenPos.z;
    param_4 = v._225_softParticleParam;
    param_5 = v._225_reconstructionParam1;
    param_6 = v._225_reconstructionParam2;
    param_17 = param_2;
    param_18 = param_3;
    param_19 = param_4;
    param_20 = param_5;
    param_21 = param_6;
    let v_24 = v_12(&(param_17), &(param_18), &(param_19), &(param_20), &(param_21));
    Output.w = (Output.w * v_24);
  }
  if ((Output.w == 0.0f)) {
    discard;
    return vec4<f32>();
  }
  param_7 = Output;
  param_8 = convertColorSpace;
  param_22 = param_7;
  param_23 = param_8;
  return v_20(&(param_22), &(param_23));
}

fn v_25() {
  var Input : PS_Input;
  var _363 : vec4<f32>;
  var param : PS_Input;
  Input.PosVS = gl_FragCoord;
  Input.Color = Input_Color;
  Input.UV = Input_UV;
  Input.PosP = Input_PosP;
  param = Input;
  _363 = v_21(&(param));
  _entryPointOutput = _363;
}

struct SPIRV_Cross_Output {
  _entryPointOutput : vec4<f32>,
}

fn v_6(stage_input : ptr<function, SPIRV_Cross_Input>) -> SPIRV_Cross_Output {
  var stage_output : SPIRV_Cross_Output;
  gl_FragCoord = (*(stage_input)).gl_FragCoord;
  gl_FragCoord.w = (1.0f / gl_FragCoord.w);
  Input_Color = (*(stage_input)).Input_Color;
  Input_UV = (*(stage_input)).Input_UV;
  Input_PosP = (*(stage_input)).Input_PosP;
  v_25();
  stage_output._entryPointOutput = _entryPointOutput;
  return stage_output;
}

@fragment
fn main(@location(0u) @interpolate(perspective, centroid) v_26 : vec4<f32>, @location(1u) @interpolate(perspective, centroid) v_27 : vec2<f32>, @location(2u) v_28 : vec4<f32>, @builtin(position) v_29 : vec4<f32>) -> @location(0u) vec4<f32> {
  main_inner(v_26, v_27, v_28, v_29);
  return v_1;
}
