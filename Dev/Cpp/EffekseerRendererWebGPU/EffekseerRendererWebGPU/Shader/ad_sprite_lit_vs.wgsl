diagnostic(off, derivative_uniformity);

struct VS_ConstantBuffer_1_1 {
  _262_mCamera : mat4x4<f32>,
  _262_mCameraProj : mat4x4<f32>,
  _262_mUVInversed : vec4<f32>,
  _262_flipbookParameter1 : vec4<f32>,
  _262_flipbookParameter2 : vec4<f32>,
}

@group(0) @binding(0) var<uniform> v : VS_ConstantBuffer_1_1;

var<private> Input_Pos : vec3<f32>;

var<private> Input_Color : vec4<f32>;

var<private> Input_Normal : vec4<f32>;

var<private> Input_Tangent : vec4<f32>;

var<private> Input_UV1 : vec2<f32>;

var<private> Input_UV2 : vec2<f32>;

var<private> Input_Alpha_Dist_UV : vec4<f32>;

var<private> Input_BlendUV : vec2<f32>;

var<private> Input_Blend_Alpha_Dist_UV : vec4<f32>;

var<private> Input_FlipbookIndex : f32;

var<private> Input_AlphaThreshold : f32;

var<private> gl_Position : vec4<f32>;

var<private> _entryPointOutput_Color : vec4<f32>;

var<private> _entryPointOutput_UV_Others : vec4<f32>;

var<private> _entryPointOutput_WorldN : vec3<f32>;

var<private> _entryPointOutput_WorldB : vec3<f32>;

var<private> _entryPointOutput_WorldT : vec3<f32>;

var<private> _entryPointOutput_Alpha_Dist_UV : vec4<f32>;

var<private> _entryPointOutput_Blend_Alpha_Dist_UV : vec4<f32>;

var<private> _entryPointOutput_Blend_FBNextIndex_UV : vec4<f32>;

var<private> _entryPointOutput_PosP : vec4<f32>;

var<private> v_1 : vec4<f32>;

var<private> v_2 : vec4<f32>;

var<private> v_3 : vec3<f32>;

var<private> v_4 : vec3<f32>;

var<private> v_5 : vec3<f32>;

var<private> v_6 : vec4<f32>;

var<private> v_7 : vec4<f32>;

var<private> v_8 : vec4<f32>;

var<private> v_9 : vec4<f32>;

var<private> v_10 : vec4<f32>;

struct SPIRV_Cross_Input {
  Input_Pos : vec3<f32>,
  Input_Color : vec4<f32>,
  Input_Normal : vec4<f32>,
  Input_Tangent : vec4<f32>,
  Input_UV1 : vec2<f32>,
  Input_UV2 : vec2<f32>,
  Input_Alpha_Dist_UV : vec4<f32>,
  Input_BlendUV : vec2<f32>,
  Input_Blend_Alpha_Dist_UV : vec4<f32>,
  Input_FlipbookIndex : f32,
  Input_AlphaThreshold : f32,
}

struct SPIRV_Cross_Output {
  _entryPointOutput_Color : vec4<f32>,
  _entryPointOutput_UV_Others : vec4<f32>,
  _entryPointOutput_WorldN : vec3<f32>,
  _entryPointOutput_WorldB : vec3<f32>,
  _entryPointOutput_WorldT : vec3<f32>,
  _entryPointOutput_Alpha_Dist_UV : vec4<f32>,
  _entryPointOutput_Blend_Alpha_Dist_UV : vec4<f32>,
  _entryPointOutput_Blend_FBNextIndex_UV : vec4<f32>,
  _entryPointOutput_PosP : vec4<f32>,
  gl_Position : vec4<f32>,
}

fn main_inner(v_11 : vec3<f32>, v_12 : vec4<f32>, v_13 : vec4<f32>, v_14 : vec4<f32>, v_15 : vec2<f32>, v_16 : vec2<f32>, v_17 : vec4<f32>, v_18 : vec2<f32>, v_19 : vec4<f32>, v_20 : f32, v_21 : f32) {
  var stage_input : SPIRV_Cross_Input;
  var flattenTemp : SPIRV_Cross_Output;
  var param : SPIRV_Cross_Input;
  stage_input.Input_Pos = v_11;
  stage_input.Input_Color = v_12;
  stage_input.Input_Normal = v_13;
  stage_input.Input_Tangent = v_14;
  stage_input.Input_UV1 = v_15;
  stage_input.Input_UV2 = v_16;
  stage_input.Input_Alpha_Dist_UV = v_17;
  stage_input.Input_BlendUV = v_18;
  stage_input.Input_Blend_Alpha_Dist_UV = v_19;
  stage_input.Input_FlipbookIndex = v_20;
  stage_input.Input_AlphaThreshold = v_21;
  param = stage_input;
  flattenTemp = v_22(&(param));
  v_1 = flattenTemp._entryPointOutput_Color;
  v_2 = flattenTemp._entryPointOutput_UV_Others;
  v_3 = flattenTemp._entryPointOutput_WorldN;
  v_4 = flattenTemp._entryPointOutput_WorldB;
  v_5 = flattenTemp._entryPointOutput_WorldT;
  v_6 = flattenTemp._entryPointOutput_Alpha_Dist_UV;
  v_7 = flattenTemp._entryPointOutput_Blend_Alpha_Dist_UV;
  v_8 = flattenTemp._entryPointOutput_Blend_FBNextIndex_UV;
  v_9 = flattenTemp._entryPointOutput_PosP;
  v_10 = flattenTemp.gl_Position;
}

fn v_23(x : ptr<function, f32>, y : ptr<function, f32>) -> f32 {
  return (*(x) - (*(y) * floor((*(x) / *(y)))));
}

fn v_24(FlipbookUV : ptr<function, vec2<f32>>, FlipbookIndex : ptr<function, f32>, DivideX : ptr<function, f32>, flipbookOneSize : ptr<function, vec2<f32>>, flipbookOffset : ptr<function, vec2<f32>>) -> vec2<f32> {
  var DivideIndex : vec2<f32>;
  var UVOffset : vec2<f32>;
  DivideIndex.x = f32((i32(*(FlipbookIndex)) % i32(*(DivideX))));
  DivideIndex.y = f32((i32(*(FlipbookIndex)) / i32(*(DivideX))));
  UVOffset = ((DivideIndex * *(flipbookOneSize)) + *(flipbookOffset));
  return (*(FlipbookUV) - UVOffset);
}

fn v_25(OriginUV : ptr<function, vec2<f32>>, Index : ptr<function, f32>, DivideX : ptr<function, f32>, flipbookOneSize : ptr<function, vec2<f32>>, flipbookOffset : ptr<function, vec2<f32>>) -> vec2<f32> {
  var DivideIndex : vec2<f32>;
  DivideIndex.x = f32((i32(*(Index)) % i32(*(DivideX))));
  DivideIndex.y = f32((i32(*(Index)) / i32(*(DivideX))));
  return ((*(OriginUV) + (DivideIndex * *(flipbookOneSize))) + *(flipbookOffset));
}

fn v_26(flipbookRate : ptr<function, f32>, flipbookUV : ptr<function, vec2<f32>>, flipbookParameter1 : ptr<function, vec4<f32>>, flipbookParameter2 : ptr<function, vec4<f32>>, flipbookIndex : ptr<function, f32>, uv : ptr<function, vec2<f32>>, uvInversed : ptr<function, vec2<f32>>) {
  var flipbookEnabled : f32;
  var flipbookLoopType : f32;
  var divideX : f32;
  var divideY : f32;
  var flipbookOneSize : vec2<f32>;
  var flipbookOffset : vec2<f32>;
  var Index : f32;
  var IndexOffset : f32;
  var NextIndex : f32;
  var FlipbookMaxCount : f32;
  var param : f32;
  var param_10 : f32;
  var param_11 : f32;
  var param_12 : f32;
  var Reverse : bool;
  var param_13 : f32;
  var param_14 : f32;
  var param_15 : f32;
  var param_16 : f32;
  var param_17 : f32;
  var param_18 : f32;
  var param_19 : f32;
  var param_20 : f32;
  var notInversedUV : vec2<f32>;
  var param_21 : vec2<f32>;
  var param_1 : f32;
  var param_2 : f32;
  var param_3 : vec2<f32>;
  var param_4 : vec2<f32>;
  var OriginUV : vec2<f32>;
  var param_22 : vec2<f32>;
  var param_23 : f32;
  var param_24 : f32;
  var param_25 : vec2<f32>;
  var param_26 : vec2<f32>;
  var param_5 : vec2<f32>;
  var param_6 : f32;
  var param_7 : f32;
  var param_8 : vec2<f32>;
  var param_9 : vec2<f32>;
  var param_27 : vec2<f32>;
  var param_28 : f32;
  var param_29 : f32;
  var param_30 : vec2<f32>;
  var param_31 : vec2<f32>;
  flipbookEnabled = (*(flipbookParameter1)).x;
  flipbookLoopType = (*(flipbookParameter1)).y;
  divideX = (*(flipbookParameter1)).z;
  divideY = (*(flipbookParameter1)).w;
  flipbookOneSize = (*(flipbookParameter2)).xy;
  flipbookOffset = (*(flipbookParameter2)).zw;
  if ((flipbookEnabled > 0.0f)) {
    *(flipbookRate) = fract(*(flipbookIndex));
    Index = floor(*(flipbookIndex));
    IndexOffset = 1.0f;
    NextIndex = (Index + IndexOffset);
    FlipbookMaxCount = (divideX * divideY);
    if ((flipbookLoopType == 0.0f)) {
      if ((NextIndex >= FlipbookMaxCount)) {
        NextIndex = (FlipbookMaxCount - 1.0f);
        Index = (FlipbookMaxCount - 1.0f);
      }
    } else if ((flipbookLoopType == 1.0f)) {
      param = Index;
      param_10 = FlipbookMaxCount;
      Index = v_23(&(param), &(param_10));
      param_11 = NextIndex;
      param_12 = FlipbookMaxCount;
      NextIndex = v_23(&(param_11), &(param_12));
    } else if ((flipbookLoopType == 2.0f)) {
      param_13 = floor((Index / FlipbookMaxCount));
      param_14 = 2.0f;
      Reverse = (v_23(&(param_13), &(param_14)) == 1.0f);
      param_15 = Index;
      param_16 = FlipbookMaxCount;
      Index = v_23(&(param_15), &(param_16));
      if (Reverse) {
        Index = ((FlipbookMaxCount - 1.0f) - floor(Index));
      }
      param_17 = floor((NextIndex / FlipbookMaxCount));
      param_18 = 2.0f;
      Reverse = (v_23(&(param_17), &(param_18)) == 1.0f);
      param_19 = NextIndex;
      param_20 = FlipbookMaxCount;
      NextIndex = v_23(&(param_19), &(param_20));
      if (Reverse) {
        NextIndex = ((FlipbookMaxCount - 1.0f) - floor(NextIndex));
      }
    }
    notInversedUV = *(uv);
    notInversedUV.y = ((*(uvInversed)).x + ((*(uvInversed)).y * notInversedUV.y));
    param_21 = notInversedUV;
    param_1 = Index;
    param_2 = divideX;
    param_3 = flipbookOneSize;
    param_4 = flipbookOffset;
    param_22 = param_21;
    param_23 = param_1;
    param_24 = param_2;
    param_25 = param_3;
    param_26 = param_4;
    OriginUV = v_24(&(param_22), &(param_23), &(param_24), &(param_25), &(param_26));
    param_5 = OriginUV;
    param_6 = NextIndex;
    param_7 = divideX;
    param_8 = flipbookOneSize;
    param_9 = flipbookOffset;
    param_27 = param_5;
    param_28 = param_6;
    param_29 = param_7;
    param_30 = param_8;
    param_31 = param_9;
    *(flipbookUV) = v_25(&(param_27), &(param_28), &(param_29), &(param_30), &(param_31));
    (*(flipbookUV)).y = ((*(uvInversed)).x + ((*(uvInversed)).y * (*(flipbookUV)).y));
  }
}

struct VS_Input {
  Pos : vec3<f32>,
  Color : vec4<f32>,
  Normal : vec4<f32>,
  Tangent : vec4<f32>,
  UV1 : vec2<f32>,
  UV2 : vec2<f32>,
  Alpha_Dist_UV : vec4<f32>,
  BlendUV : vec2<f32>,
  Blend_Alpha_Dist_UV : vec4<f32>,
  FlipbookIndex : f32,
  AlphaThreshold : f32,
}

struct VS_Output {
  PosVS : vec4<f32>,
  Color : vec4<f32>,
  UV_Others : vec4<f32>,
  WorldN : vec3<f32>,
  WorldB : vec3<f32>,
  WorldT : vec3<f32>,
  Alpha_Dist_UV : vec4<f32>,
  Blend_Alpha_Dist_UV : vec4<f32>,
  Blend_FBNextIndex_UV : vec4<f32>,
  PosP : vec4<f32>,
}

fn v_27(vsinput : ptr<function, VS_Input>, vsoutput : ptr<function, VS_Output>) {
  var flipbookRate : f32;
  var flipbookNextIndexUV : vec2<f32>;
  var param : f32;
  var param_1 : vec2<f32>;
  var param_2 : vec4<f32>;
  var param_3 : vec4<f32>;
  var param_4 : f32;
  var param_5 : vec2<f32>;
  var param_6 : vec2<f32>;
  var param_32 : f32;
  var param_33 : vec2<f32>;
  var param_34 : vec4<f32>;
  var param_35 : vec4<f32>;
  var param_36 : f32;
  var param_37 : vec2<f32>;
  var param_38 : vec2<f32>;
  (*(vsoutput)).Alpha_Dist_UV = (*(vsinput)).Alpha_Dist_UV;
  (*(vsoutput)).Alpha_Dist_UV.y = (v._262_mUVInversed.x + (v._262_mUVInversed.y * (*(vsinput)).Alpha_Dist_UV.y));
  (*(vsoutput)).Alpha_Dist_UV.w = (v._262_mUVInversed.x + (v._262_mUVInversed.y * (*(vsinput)).Alpha_Dist_UV.w));
  (*(vsoutput)).Blend_FBNextIndex_UV.x = (*(vsinput)).BlendUV.x;
  (*(vsoutput)).Blend_FBNextIndex_UV.y = (*(vsinput)).BlendUV.y;
  (*(vsoutput)).Blend_FBNextIndex_UV.y = (v._262_mUVInversed.x + (v._262_mUVInversed.y * (*(vsinput)).BlendUV.y));
  (*(vsoutput)).Blend_Alpha_Dist_UV = (*(vsinput)).Blend_Alpha_Dist_UV;
  (*(vsoutput)).Blend_Alpha_Dist_UV.y = (v._262_mUVInversed.x + (v._262_mUVInversed.y * (*(vsinput)).Blend_Alpha_Dist_UV.y));
  (*(vsoutput)).Blend_Alpha_Dist_UV.w = (v._262_mUVInversed.x + (v._262_mUVInversed.y * (*(vsinput)).Blend_Alpha_Dist_UV.w));
  flipbookRate = 0.0f;
  flipbookNextIndexUV = vec2<f32>();
  param = flipbookRate;
  param_1 = flipbookNextIndexUV;
  param_2 = v._262_flipbookParameter1;
  param_3 = v._262_flipbookParameter2;
  param_4 = (*(vsinput)).FlipbookIndex;
  param_5 = (*(vsoutput)).UV_Others.xy;
  param_6 = v._262_mUVInversed.xy;
  param_32 = param;
  param_33 = param_1;
  param_34 = param_2;
  param_35 = param_3;
  param_36 = param_4;
  param_37 = param_5;
  param_38 = param_6;
  v_26(&(param_32), &(param_33), &(param_34), &(param_35), &(param_36), &(param_37), &(param_38));
  param = param_32;
  param_1 = param_33;
  flipbookRate = param;
  flipbookNextIndexUV = param_1;
  (*(vsoutput)).Blend_FBNextIndex_UV.z = flipbookNextIndexUV.x;
  (*(vsoutput)).Blend_FBNextIndex_UV.w = flipbookNextIndexUV.y;
  (*(vsoutput)).UV_Others.z = flipbookRate;
  (*(vsoutput)).UV_Others.w = (*(vsinput)).AlphaThreshold;
}

fn v_28(Input : ptr<function, VS_Input>) -> VS_Output {
  var Output : VS_Output;
  var worldNormal : vec4<f32>;
  var worldTangent : vec4<f32>;
  var worldBinormal : vec4<f32>;
  var uv1 : vec2<f32>;
  var worldPos : vec4<f32>;
  var param : VS_Input;
  var param_1 : VS_Output;
  var param_39 : VS_Input;
  var param_40 : VS_Output;
  Output.PosVS = vec4<f32>();
  Output.Color = vec4<f32>();
  Output.UV_Others = vec4<f32>();
  Output.WorldN = vec3<f32>();
  Output.WorldB = vec3<f32>();
  Output.WorldT = vec3<f32>();
  Output.Alpha_Dist_UV = vec4<f32>();
  Output.Blend_Alpha_Dist_UV = vec4<f32>();
  Output.Blend_FBNextIndex_UV = vec4<f32>();
  Output.PosP = vec4<f32>();
  let v_29 = (((*(Input)).Normal.xyz - vec3<f32>(0.5f)) * 2.0f);
  worldNormal = vec4<f32>(v_29.x, v_29.y, v_29.z, 0.0f);
  let v_30 = (((*(Input)).Tangent.xyz - vec3<f32>(0.5f)) * 2.0f);
  worldTangent = vec4<f32>(v_30.x, v_30.y, v_30.z, 0.0f);
  let v_31 = cross(worldNormal.xyz, worldTangent.xyz);
  worldBinormal = vec4<f32>(v_31.x, v_31.y, v_31.z, 0.0f);
  uv1 = (*(Input)).UV1;
  uv1.y = (v._262_mUVInversed.x + (v._262_mUVInversed.y * uv1.y));
  Output.UV_Others.x = uv1.x;
  Output.UV_Others.y = uv1.y;
  worldPos = vec4<f32>((*(Input)).Pos.x, (*(Input)).Pos.y, (*(Input)).Pos.z, 1.0f);
  Output.PosVS = (v._262_mCameraProj * worldPos);
  Output.WorldN = worldNormal.xyz;
  Output.WorldB = worldBinormal.xyz;
  Output.WorldT = worldTangent.xyz;
  Output.Color = (*(Input)).Color;
  param = *(Input);
  param_1 = Output;
  param_39 = param;
  param_40 = param_1;
  v_27(&(param_39), &(param_40));
  param_1 = param_40;
  Output = param_1;
  Output.PosP = Output.PosVS;
  return Output;
}

fn v_32() {
  var Input : VS_Input;
  var flattenTemp : VS_Output;
  var param : VS_Input;
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
  param = Input;
  flattenTemp = v_28(&(param));
  gl_Position = flattenTemp.PosVS;
  _entryPointOutput_Color = flattenTemp.Color;
  _entryPointOutput_UV_Others = flattenTemp.UV_Others;
  _entryPointOutput_WorldN = flattenTemp.WorldN;
  _entryPointOutput_WorldB = flattenTemp.WorldB;
  _entryPointOutput_WorldT = flattenTemp.WorldT;
  _entryPointOutput_Alpha_Dist_UV = flattenTemp.Alpha_Dist_UV;
  _entryPointOutput_Blend_Alpha_Dist_UV = flattenTemp.Blend_Alpha_Dist_UV;
  _entryPointOutput_Blend_FBNextIndex_UV = flattenTemp.Blend_FBNextIndex_UV;
  _entryPointOutput_PosP = flattenTemp.PosP;
}

fn v_22(stage_input : ptr<function, SPIRV_Cross_Input>) -> SPIRV_Cross_Output {
  var stage_output : SPIRV_Cross_Output;
  Input_Pos = (*(stage_input)).Input_Pos;
  Input_Color = (*(stage_input)).Input_Color;
  Input_Normal = (*(stage_input)).Input_Normal;
  Input_Tangent = (*(stage_input)).Input_Tangent;
  Input_UV1 = (*(stage_input)).Input_UV1;
  Input_UV2 = (*(stage_input)).Input_UV2;
  Input_Alpha_Dist_UV = (*(stage_input)).Input_Alpha_Dist_UV;
  Input_BlendUV = (*(stage_input)).Input_BlendUV;
  Input_Blend_Alpha_Dist_UV = (*(stage_input)).Input_Blend_Alpha_Dist_UV;
  Input_FlipbookIndex = (*(stage_input)).Input_FlipbookIndex;
  Input_AlphaThreshold = (*(stage_input)).Input_AlphaThreshold;
  v_32();
  stage_output.gl_Position = gl_Position;
  stage_output._entryPointOutput_Color = _entryPointOutput_Color;
  stage_output._entryPointOutput_UV_Others = _entryPointOutput_UV_Others;
  stage_output._entryPointOutput_WorldN = _entryPointOutput_WorldN;
  stage_output._entryPointOutput_WorldB = _entryPointOutput_WorldB;
  stage_output._entryPointOutput_WorldT = _entryPointOutput_WorldT;
  stage_output._entryPointOutput_Alpha_Dist_UV = _entryPointOutput_Alpha_Dist_UV;
  stage_output._entryPointOutput_Blend_Alpha_Dist_UV = _entryPointOutput_Blend_Alpha_Dist_UV;
  stage_output._entryPointOutput_Blend_FBNextIndex_UV = _entryPointOutput_Blend_FBNextIndex_UV;
  stage_output._entryPointOutput_PosP = _entryPointOutput_PosP;
  return stage_output;
}

struct tint_symbol {
  @location(0u) @interpolate(perspective, centroid)
  m : vec4<f32>,
  @location(1u) @interpolate(perspective, centroid)
  m_1 : vec4<f32>,
  @location(2u)
  m_2 : vec3<f32>,
  @location(3u)
  m_3 : vec3<f32>,
  @location(4u)
  m_4 : vec3<f32>,
  @location(5u)
  m_5 : vec4<f32>,
  @location(6u)
  m_6 : vec4<f32>,
  @location(7u)
  m_7 : vec4<f32>,
  @location(8u)
  m_8 : vec4<f32>,
  @builtin(position)
  m_9 : vec4<f32>,
}

@vertex
fn main(@location(0u) v_33 : vec3<f32>, @location(1u) v_34 : vec4<f32>, @location(2u) v_35 : vec4<f32>, @location(3u) v_36 : vec4<f32>, @location(4u) v_37 : vec2<f32>, @location(5u) v_38 : vec2<f32>, @location(6u) v_39 : vec4<f32>, @location(7u) v_40 : vec2<f32>, @location(8u) v_41 : vec4<f32>, @location(9u) v_42 : f32, @location(10u) v_43 : f32) -> tint_symbol {
  main_inner(v_33, v_34, v_35, v_36, v_37, v_38, v_39, v_40, v_41, v_42, v_43);
  return tint_symbol(v_1, v_2, v_3, v_4, v_5, v_6, v_7, v_8, v_9, v_10);
}
