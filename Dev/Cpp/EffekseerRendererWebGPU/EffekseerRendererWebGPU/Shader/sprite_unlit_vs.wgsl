diagnostic(off, derivative_uniformity);

struct VS_ConstantBuffer_1_1 {
  _39_mCamera : mat4x4<f32>,
  _39_mCameraProj : mat4x4<f32>,
  _39_mUVInversed : vec4<f32>,
  _39_mflipbookParameter : vec4<f32>,
}

@group(0) @binding(0) var<uniform> v : VS_ConstantBuffer_1_1;

var<private> Input_Pos : vec3<f32>;

var<private> Input_Color : vec4<f32>;

var<private> Input_UV : vec2<f32>;

var<private> gl_Position : vec4<f32>;

var<private> _entryPointOutput_Color : vec4<f32>;

var<private> _entryPointOutput_UV : vec2<f32>;

var<private> _entryPointOutput_PosP : vec4<f32>;

var<private> v_1 : vec4<f32>;

var<private> v_2 : vec2<f32>;

var<private> v_3 : vec4<f32>;

var<private> v_4 : vec4<f32>;

struct SPIRV_Cross_Input {
  Input_Pos : vec3<f32>,
  Input_Color : vec4<f32>,
  Input_UV : vec2<f32>,
}

struct SPIRV_Cross_Output {
  _entryPointOutput_Color : vec4<f32>,
  _entryPointOutput_UV : vec2<f32>,
  _entryPointOutput_PosP : vec4<f32>,
  gl_Position : vec4<f32>,
}

fn main_inner(v_5 : vec3<f32>, v_6 : vec4<f32>, v_7 : vec2<f32>) {
  var stage_input : SPIRV_Cross_Input;
  var flattenTemp : SPIRV_Cross_Output;
  var param : SPIRV_Cross_Input;
  stage_input.Input_Pos = v_5;
  stage_input.Input_Color = v_6;
  stage_input.Input_UV = v_7;
  param = stage_input;
  flattenTemp = v_8(&(param));
  v_1 = flattenTemp._entryPointOutput_Color;
  v_2 = flattenTemp._entryPointOutput_UV;
  v_3 = flattenTemp._entryPointOutput_PosP;
  v_4 = flattenTemp.gl_Position;
}

struct VS_Input {
  Pos : vec3<f32>,
  Color : vec4<f32>,
  UV : vec2<f32>,
}

struct VS_Output {
  PosVS : vec4<f32>,
  Color : vec4<f32>,
  UV : vec2<f32>,
  PosP : vec4<f32>,
}

fn v_9(Input : ptr<function, VS_Input>) -> VS_Output {
  var Output : VS_Output;
  var worldPos : vec4<f32>;
  var uv1 : vec2<f32>;
  Output.PosVS = vec4<f32>();
  Output.Color = vec4<f32>();
  Output.UV = vec2<f32>();
  Output.PosP = vec4<f32>();
  worldPos = vec4<f32>((*(Input)).Pos.x, (*(Input)).Pos.y, (*(Input)).Pos.z, 1.0f);
  Output.PosVS = (v._39_mCameraProj * worldPos);
  Output.Color = (*(Input)).Color;
  uv1 = (*(Input)).UV;
  uv1.y = (v._39_mUVInversed.x + (v._39_mUVInversed.y * uv1.y));
  Output.UV = uv1;
  Output.PosP = Output.PosVS;
  return Output;
}

fn v_10() {
  var Input : VS_Input;
  var flattenTemp : VS_Output;
  var param : VS_Input;
  Input.Pos = Input_Pos;
  Input.Color = Input_Color;
  Input.UV = Input_UV;
  param = Input;
  flattenTemp = v_9(&(param));
  gl_Position = flattenTemp.PosVS;
  _entryPointOutput_Color = flattenTemp.Color;
  _entryPointOutput_UV = flattenTemp.UV;
  _entryPointOutput_PosP = flattenTemp.PosP;
}

fn v_8(stage_input : ptr<function, SPIRV_Cross_Input>) -> SPIRV_Cross_Output {
  var stage_output : SPIRV_Cross_Output;
  Input_Pos = (*(stage_input)).Input_Pos;
  Input_Color = (*(stage_input)).Input_Color;
  Input_UV = (*(stage_input)).Input_UV;
  v_10();
  stage_output.gl_Position = gl_Position;
  stage_output._entryPointOutput_Color = _entryPointOutput_Color;
  stage_output._entryPointOutput_UV = _entryPointOutput_UV;
  stage_output._entryPointOutput_PosP = _entryPointOutput_PosP;
  return stage_output;
}

struct tint_symbol {
  @location(0u) @interpolate(perspective, centroid)
  m : vec4<f32>,
  @location(1u) @interpolate(perspective, centroid)
  m_1 : vec2<f32>,
  @location(2u)
  m_2 : vec4<f32>,
  @builtin(position)
  m_3 : vec4<f32>,
}

@vertex
fn main(@location(0u) v_11 : vec3<f32>, @location(1u) v_12 : vec4<f32>, @location(2u) v_13 : vec2<f32>) -> tint_symbol {
  main_inner(v_11, v_12, v_13);
  return tint_symbol(v_1, v_2, v_3, v_4);
}
