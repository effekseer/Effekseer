diagnostic(off, derivative_uniformity);

struct VS_ConstantBuffer_1_1 {
  _73_mCamera : mat4x4<f32>,
  _73_mCameraProj : mat4x4<f32>,
  _73_mUVInversed : vec4<f32>,
  _73_mflipbookParameter : vec4<f32>,
}

@group(0) @binding(0) var<uniform> v : VS_ConstantBuffer_1_1;

var<private> Input_Pos : vec3<f32>;

var<private> Input_Color : vec4<f32>;

var<private> Input_Normal : vec4<f32>;

var<private> Input_Tangent : vec4<f32>;

var<private> Input_UV1 : vec2<f32>;

var<private> Input_UV2 : vec2<f32>;

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
  Input_Color : vec4<f32>,
  Input_Normal : vec4<f32>,
  Input_Tangent : vec4<f32>,
  Input_UV1 : vec2<f32>,
  Input_UV2 : vec2<f32>,
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

fn main_inner(v_8 : vec3<f32>, v_9 : vec4<f32>, v_10 : vec4<f32>, v_11 : vec4<f32>, v_12 : vec2<f32>, v_13 : vec2<f32>) {
  var stage_input : SPIRV_Cross_Input;
  var flattenTemp : SPIRV_Cross_Output;
  var param : SPIRV_Cross_Input;
  stage_input.Input_Pos = v_8;
  stage_input.Input_Color = v_9;
  stage_input.Input_Normal = v_10;
  stage_input.Input_Tangent = v_11;
  stage_input.Input_UV1 = v_12;
  stage_input.Input_UV2 = v_13;
  param = stage_input;
  flattenTemp = v_14(&(param));
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
  Color : vec4<f32>,
  Normal : vec4<f32>,
  Tangent : vec4<f32>,
  UV1 : vec2<f32>,
  UV2 : vec2<f32>,
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

fn v_15(Input : ptr<function, VS_Input>) -> VS_Output {
  var Output : VS_Output;
  var worldNormal : vec4<f32>;
  var worldTangent : vec4<f32>;
  var worldBinormal : vec4<f32>;
  var worldPos : vec4<f32>;
  var uv1 : vec2<f32>;
  Output.PosVS = vec4<f32>();
  Output.Color = vec4<f32>();
  Output.UV = vec2<f32>();
  Output.WorldN = vec3<f32>();
  Output.WorldB = vec3<f32>();
  Output.WorldT = vec3<f32>();
  Output.PosP = vec4<f32>();
  let v_16 = (((*(Input)).Normal.xyz - vec3<f32>(0.5f)) * 2.0f);
  worldNormal = vec4<f32>(v_16.x, v_16.y, v_16.z, 0.0f);
  let v_17 = (((*(Input)).Tangent.xyz - vec3<f32>(0.5f)) * 2.0f);
  worldTangent = vec4<f32>(v_17.x, v_17.y, v_17.z, 0.0f);
  let v_18 = cross(worldNormal.xyz, worldTangent.xyz);
  worldBinormal = vec4<f32>(v_18.x, v_18.y, v_18.z, 0.0f);
  worldPos = vec4<f32>((*(Input)).Pos.x, (*(Input)).Pos.y, (*(Input)).Pos.z, 1.0f);
  Output.PosVS = (v._73_mCameraProj * worldPos);
  Output.Color = (*(Input)).Color;
  uv1 = (*(Input)).UV1;
  uv1.y = (v._73_mUVInversed.x + (v._73_mUVInversed.y * uv1.y));
  Output.UV = uv1;
  Output.WorldN = worldNormal.xyz;
  Output.WorldB = worldBinormal.xyz;
  Output.WorldT = worldTangent.xyz;
  Output.PosP = Output.PosVS;
  return Output;
}

fn v_19() {
  var Input : VS_Input;
  var flattenTemp : VS_Output;
  var param : VS_Input;
  Input.Pos = Input_Pos;
  Input.Color = Input_Color;
  Input.Normal = Input_Normal;
  Input.Tangent = Input_Tangent;
  Input.UV1 = Input_UV1;
  Input.UV2 = Input_UV2;
  param = Input;
  flattenTemp = v_15(&(param));
  gl_Position = flattenTemp.PosVS;
  _entryPointOutput_Color = flattenTemp.Color;
  _entryPointOutput_UV = flattenTemp.UV;
  _entryPointOutput_WorldN = flattenTemp.WorldN;
  _entryPointOutput_WorldB = flattenTemp.WorldB;
  _entryPointOutput_WorldT = flattenTemp.WorldT;
  _entryPointOutput_PosP = flattenTemp.PosP;
}

fn v_14(stage_input : ptr<function, SPIRV_Cross_Input>) -> SPIRV_Cross_Output {
  var stage_output : SPIRV_Cross_Output;
  Input_Pos = (*(stage_input)).Input_Pos;
  Input_Color = (*(stage_input)).Input_Color;
  Input_Normal = (*(stage_input)).Input_Normal;
  Input_Tangent = (*(stage_input)).Input_Tangent;
  Input_UV1 = (*(stage_input)).Input_UV1;
  Input_UV2 = (*(stage_input)).Input_UV2;
  v_19();
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
fn main(@location(0u) v_20 : vec3<f32>, @location(1u) v_21 : vec4<f32>, @location(2u) v_22 : vec4<f32>, @location(3u) v_23 : vec4<f32>, @location(4u) v_24 : vec2<f32>, @location(5u) v_25 : vec2<f32>) -> tint_symbol {
  main_inner(v_20, v_21, v_22, v_23, v_24, v_25);
  return tint_symbol(v_1, v_2, v_3, v_4, v_5, v_6, v_7);
}
