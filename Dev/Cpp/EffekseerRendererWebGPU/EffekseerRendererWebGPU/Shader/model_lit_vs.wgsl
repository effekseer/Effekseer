diagnostic(off, derivative_uniformity);

struct VS_ConstantBuffer_1_1 {
  _31_mCameraProj : mat4x4<f32>,
  _31_mModel_Inst : array<mat4x4<f32>, 40u>,
  _31_fUV : array<vec4<f32>, 40u>,
  _31_fModelColor : array<vec4<f32>, 40u>,
  _31_fLightDirection : vec4<f32>,
  _31_fLightColor : vec4<f32>,
  _31_fLightAmbient : vec4<f32>,
  _31_mUVInversed : vec4<f32>,
}

@group(0) @binding(0) var<uniform> v : VS_ConstantBuffer_1_1;

var<private> Input_Pos : vec3<f32>;

var<private> Input_Normal : vec3<f32>;

var<private> Input_Binormal : vec3<f32>;

var<private> Input_Tangent : vec3<f32>;

var<private> Input_UV1 : vec2<f32>;

var<private> Input_UV2 : vec2<f32>;

var<private> Input_Color : vec4<f32>;

var<private> gl_InstanceIndex : i32;

var<private> gl_Position : vec4<f32>;

var<private> _entryPointOutput_Color : vec4<f32>;

var<private> _entryPointOutput_UV : vec2<f32>;

var<private> _entryPointOutput_WorldN : vec3<f32>;

var<private> _entryPointOutput_WorldB : vec3<f32>;

var<private> _entryPointOutput_WorldT : vec3<f32>;

var<private> _entryPointOutput_PosP : vec4<f32>;

var<private> v_1 : vec4<f32>;

var<private> v_2 : vec2<f32>;

var<private> v_3 : vec3<f32>;

var<private> v_4 : vec3<f32>;

var<private> v_5 : vec3<f32>;

var<private> v_6 : vec4<f32>;

var<private> v_7 : vec4<f32>;

struct SPIRV_Cross_Input {
  Input_Pos : vec3<f32>,
  Input_Normal : vec3<f32>,
  Input_Binormal : vec3<f32>,
  Input_Tangent : vec3<f32>,
  Input_UV1 : vec2<f32>,
  Input_UV2 : vec2<f32>,
  Input_Color : vec4<f32>,
  gl_InstanceIndex : u32,
}

struct SPIRV_Cross_Output {
  _entryPointOutput_Color : vec4<f32>,
  _entryPointOutput_UV : vec2<f32>,
  _entryPointOutput_WorldN : vec3<f32>,
  _entryPointOutput_WorldB : vec3<f32>,
  _entryPointOutput_WorldT : vec3<f32>,
  _entryPointOutput_PosP : vec4<f32>,
  gl_Position : vec4<f32>,
}

fn main_inner(v_8 : vec3<f32>, v_9 : vec3<f32>, v_10 : vec3<f32>, v_11 : vec3<f32>, v_12 : vec2<f32>, v_13 : vec2<f32>, v_14 : vec4<f32>, v_15 : u32) {
  var stage_input : SPIRV_Cross_Input;
  var flattenTemp : SPIRV_Cross_Output;
  var param : SPIRV_Cross_Input;
  stage_input.Input_Pos = v_8;
  stage_input.Input_Normal = v_9;
  stage_input.Input_Binormal = v_10;
  stage_input.Input_Tangent = v_11;
  stage_input.Input_UV1 = v_12;
  stage_input.Input_UV2 = v_13;
  stage_input.Input_Color = v_14;
  stage_input.gl_InstanceIndex = v_15;
  param = stage_input;
  flattenTemp = v_16(&(param));
  v_1 = flattenTemp._entryPointOutput_Color;
  v_2 = flattenTemp._entryPointOutput_UV;
  v_3 = flattenTemp._entryPointOutput_WorldN;
  v_4 = flattenTemp._entryPointOutput_WorldB;
  v_5 = flattenTemp._entryPointOutput_WorldT;
  v_6 = flattenTemp._entryPointOutput_PosP;
  v_7 = flattenTemp.gl_Position;
}

struct VS_Input {
  Pos : vec3<f32>,
  Normal : vec3<f32>,
  Binormal : vec3<f32>,
  Tangent : vec3<f32>,
  UV1 : vec2<f32>,
  UV2 : vec2<f32>,
  Color : vec4<f32>,
  Index : u32,
}

struct VS_Output {
  PosVS : vec4<f32>,
  Color : vec4<f32>,
  UV : vec2<f32>,
  WorldN : vec3<f32>,
  WorldB : vec3<f32>,
  WorldT : vec3<f32>,
  PosP : vec4<f32>,
}

fn v_17(Input : ptr<function, VS_Input>) -> VS_Output {
  var index : u32;
  var mModel : mat4x4<f32>;
  var uv : vec4<f32>;
  var modelColor : vec4<f32>;
  var Output : VS_Output;
  var localPos : vec4<f32>;
  var worldPos : vec4<f32>;
  var outputUV : vec2<f32>;
  var localNormal : vec4<f32>;
  var localBinormal : vec4<f32>;
  var localTangent : vec4<f32>;
  var worldNormal : vec4<f32>;
  var worldBinormal : vec4<f32>;
  var worldTangent : vec4<f32>;
  index = (*(Input)).Index;
  mModel = v._31_mModel_Inst[index];
  uv = v._31_fUV[index];
  modelColor = (v._31_fModelColor[index] * (*(Input)).Color);
  Output.PosVS = vec4<f32>();
  Output.Color = vec4<f32>();
  Output.UV = vec2<f32>();
  Output.WorldN = vec3<f32>();
  Output.WorldB = vec3<f32>();
  Output.WorldT = vec3<f32>();
  Output.PosP = vec4<f32>();
  localPos = vec4<f32>((*(Input)).Pos.x, (*(Input)).Pos.y, (*(Input)).Pos.z, 1.0f);
  worldPos = (mModel * localPos);
  Output.PosVS = (v._31_mCameraProj * worldPos);
  Output.Color = modelColor;
  outputUV = (*(Input)).UV1;
  outputUV.x = ((outputUV.x * uv.z) + uv.x);
  outputUV.y = ((outputUV.y * uv.w) + uv.y);
  outputUV.y = (v._31_mUVInversed.x + (v._31_mUVInversed.y * outputUV.y));
  Output.UV = outputUV;
  localNormal = vec4<f32>((*(Input)).Normal.x, (*(Input)).Normal.y, (*(Input)).Normal.z, 0.0f);
  localBinormal = vec4<f32>((*(Input)).Binormal.x, (*(Input)).Binormal.y, (*(Input)).Binormal.z, 0.0f);
  localTangent = vec4<f32>((*(Input)).Tangent.x, (*(Input)).Tangent.y, (*(Input)).Tangent.z, 0.0f);
  worldNormal = (mModel * localNormal);
  worldBinormal = (mModel * localBinormal);
  worldTangent = (mModel * localTangent);
  worldNormal = vec4<f32>(normalize(worldNormal.xyz), 0.0f);
  worldBinormal = vec4<f32>(normalize(worldBinormal.xyz), 0.0f);
  worldTangent = vec4<f32>(normalize(worldTangent.xyz), 0.0f);
  Output.WorldN = worldNormal.xyz;
  Output.WorldB = worldBinormal.xyz;
  Output.WorldT = worldTangent.xyz;
  Output.PosP = Output.PosVS;
  return Output;
}

fn v_18() {
  var Input : VS_Input;
  var flattenTemp : VS_Output;
  var param : VS_Input;
  Input.Pos = Input_Pos;
  Input.Normal = Input_Normal;
  Input.Binormal = Input_Binormal;
  Input.Tangent = Input_Tangent;
  Input.UV1 = Input_UV1;
  Input.UV2 = Input_UV2;
  Input.Color = Input_Color;
  Input.Index = bitcast<u32>(gl_InstanceIndex);
  param = Input;
  flattenTemp = v_17(&(param));
  gl_Position = flattenTemp.PosVS;
  _entryPointOutput_Color = flattenTemp.Color;
  _entryPointOutput_UV = flattenTemp.UV;
  _entryPointOutput_WorldN = flattenTemp.WorldN;
  _entryPointOutput_WorldB = flattenTemp.WorldB;
  _entryPointOutput_WorldT = flattenTemp.WorldT;
  _entryPointOutput_PosP = flattenTemp.PosP;
}

fn v_16(stage_input : ptr<function, SPIRV_Cross_Input>) -> SPIRV_Cross_Output {
  var stage_output : SPIRV_Cross_Output;
  gl_InstanceIndex = bitcast<i32>((*(stage_input)).gl_InstanceIndex);
  Input_Pos = (*(stage_input)).Input_Pos;
  Input_Normal = (*(stage_input)).Input_Normal;
  Input_Binormal = (*(stage_input)).Input_Binormal;
  Input_Tangent = (*(stage_input)).Input_Tangent;
  Input_UV1 = (*(stage_input)).Input_UV1;
  Input_UV2 = (*(stage_input)).Input_UV2;
  Input_Color = (*(stage_input)).Input_Color;
  v_18();
  stage_output.gl_Position = gl_Position;
  stage_output._entryPointOutput_Color = _entryPointOutput_Color;
  stage_output._entryPointOutput_UV = _entryPointOutput_UV;
  stage_output._entryPointOutput_WorldN = _entryPointOutput_WorldN;
  stage_output._entryPointOutput_WorldB = _entryPointOutput_WorldB;
  stage_output._entryPointOutput_WorldT = _entryPointOutput_WorldT;
  stage_output._entryPointOutput_PosP = _entryPointOutput_PosP;
  return stage_output;
}

struct tint_symbol {
  @location(0u) @interpolate(perspective, centroid)
  m : vec4<f32>,
  @location(1u) @interpolate(perspective, centroid)
  m_1 : vec2<f32>,
  @location(2u)
  m_2 : vec3<f32>,
  @location(3u)
  m_3 : vec3<f32>,
  @location(4u)
  m_4 : vec3<f32>,
  @location(5u)
  m_5 : vec4<f32>,
  @builtin(position)
  m_6 : vec4<f32>,
}

@vertex
fn main(@location(0u) v_19 : vec3<f32>, @location(1u) v_20 : vec3<f32>, @location(2u) v_21 : vec3<f32>, @location(3u) v_22 : vec3<f32>, @location(4u) v_23 : vec2<f32>, @location(5u) v_24 : vec2<f32>, @location(6u) v_25 : vec4<f32>, @builtin(instance_index) v_26 : u32) -> tint_symbol {
  main_inner(v_19, v_20, v_21, v_22, v_23, v_24, v_25, v_26);
  return tint_symbol(v_1, v_2, v_3, v_4, v_5, v_6, v_7);
}
