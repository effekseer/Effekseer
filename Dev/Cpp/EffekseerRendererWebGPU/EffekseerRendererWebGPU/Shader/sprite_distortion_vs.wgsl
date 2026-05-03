diagnostic(off, derivative_uniformity);

struct VS_ConstantBuffer_1_1 {
  _72_mCamera : mat4x4<f32>,
  _72_mCameraProj : mat4x4<f32>,
  _72_mUVInversed : vec4<f32>,
  _72_mflipbookParameter : vec4<f32>,
}

@group(0) @binding(0) var<uniform> v : VS_ConstantBuffer_1_1;

var<private> Input_Pos : vec3<f32>;

var<private> Input_Color : vec4<f32>;

var<private> Input_Normal : vec4<f32>;

var<private> Input_Tangent : vec4<f32>;

var<private> Input_UV1 : vec2<f32>;

var<private> Input_UV2 : vec2<f32>;

var<private> gl_Position : vec4<f32>;

var<private> _entryPointOutput_UV : vec2<f32>;

var<private> _entryPointOutput_ProjBinormal : vec4<f32>;

var<private> _entryPointOutput_ProjTangent : vec4<f32>;

var<private> _entryPointOutput_PosP : vec4<f32>;

var<private> _entryPointOutput_Color : vec4<f32>;

var<private> v_1 : vec2<f32>;

var<private> v_2 : vec4<f32>;

var<private> v_3 : vec4<f32>;

var<private> v_4 : vec4<f32>;

var<private> v_5 : vec4<f32>;

var<private> v_6 : vec4<f32>;

struct SPIRV_Cross_Input {
  Input_Pos : vec3<f32>,
  Input_Color : vec4<f32>,
  Input_Normal : vec4<f32>,
  Input_Tangent : vec4<f32>,
  Input_UV1 : vec2<f32>,
  Input_UV2 : vec2<f32>,
}

struct SPIRV_Cross_Output {
  _entryPointOutput_UV : vec2<f32>,
  _entryPointOutput_ProjBinormal : vec4<f32>,
  _entryPointOutput_ProjTangent : vec4<f32>,
  _entryPointOutput_PosP : vec4<f32>,
  _entryPointOutput_Color : vec4<f32>,
  gl_Position : vec4<f32>,
}

fn main_inner(v_7 : vec3<f32>, v_8 : vec4<f32>, v_9 : vec4<f32>, v_10 : vec4<f32>, v_11 : vec2<f32>, v_12 : vec2<f32>) {
  var stage_input : SPIRV_Cross_Input;
  var flattenTemp : SPIRV_Cross_Output;
  var param : SPIRV_Cross_Input;
  stage_input.Input_Pos = v_7;
  stage_input.Input_Color = v_8;
  stage_input.Input_Normal = v_9;
  stage_input.Input_Tangent = v_10;
  stage_input.Input_UV1 = v_11;
  stage_input.Input_UV2 = v_12;
  param = stage_input;
  flattenTemp = v_13(&(param));
  v_1 = flattenTemp._entryPointOutput_UV;
  v_2 = flattenTemp._entryPointOutput_ProjBinormal;
  v_3 = flattenTemp._entryPointOutput_ProjTangent;
  v_4 = flattenTemp._entryPointOutput_PosP;
  v_5 = flattenTemp._entryPointOutput_Color;
  v_6 = flattenTemp.gl_Position;
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
  UV : vec2<f32>,
  ProjBinormal : vec4<f32>,
  ProjTangent : vec4<f32>,
  PosP : vec4<f32>,
  Color : vec4<f32>,
}

fn v_14(Input : ptr<function, VS_Input>) -> VS_Output {
  var Output : VS_Output;
  var worldNormal : vec4<f32>;
  var worldTangent : vec4<f32>;
  var worldBinormal : vec4<f32>;
  var worldPos : vec4<f32>;
  var uv1 : vec2<f32>;
  Output.PosVS = vec4<f32>();
  Output.UV = vec2<f32>();
  Output.ProjBinormal = vec4<f32>();
  Output.ProjTangent = vec4<f32>();
  Output.PosP = vec4<f32>();
  Output.Color = vec4<f32>();
  let v_15 = (((*(Input)).Normal.xyz - vec3<f32>(0.5f)) * 2.0f);
  worldNormal = vec4<f32>(v_15.x, v_15.y, v_15.z, 0.0f);
  let v_16 = (((*(Input)).Tangent.xyz - vec3<f32>(0.5f)) * 2.0f);
  worldTangent = vec4<f32>(v_16.x, v_16.y, v_16.z, 0.0f);
  let v_17 = cross(worldNormal.xyz, worldTangent.xyz);
  worldBinormal = vec4<f32>(v_17.x, v_17.y, v_17.z, 0.0f);
  worldPos = vec4<f32>((*(Input)).Pos.x, (*(Input)).Pos.y, (*(Input)).Pos.z, 1.0f);
  Output.PosVS = (v._72_mCameraProj * worldPos);
  Output.Color = (*(Input)).Color;
  uv1 = (*(Input)).UV1;
  uv1.y = (v._72_mUVInversed.x + (v._72_mUVInversed.y * uv1.y));
  Output.UV = uv1;
  Output.ProjTangent = (v._72_mCameraProj * (worldPos + worldTangent));
  Output.ProjBinormal = (v._72_mCameraProj * (worldPos + worldBinormal));
  Output.PosP = Output.PosVS;
  return Output;
}

fn v_18() {
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
  flattenTemp = v_14(&(param));
  gl_Position = flattenTemp.PosVS;
  _entryPointOutput_UV = flattenTemp.UV;
  _entryPointOutput_ProjBinormal = flattenTemp.ProjBinormal;
  _entryPointOutput_ProjTangent = flattenTemp.ProjTangent;
  _entryPointOutput_PosP = flattenTemp.PosP;
  _entryPointOutput_Color = flattenTemp.Color;
}

fn v_13(stage_input : ptr<function, SPIRV_Cross_Input>) -> SPIRV_Cross_Output {
  var stage_output : SPIRV_Cross_Output;
  Input_Pos = (*(stage_input)).Input_Pos;
  Input_Color = (*(stage_input)).Input_Color;
  Input_Normal = (*(stage_input)).Input_Normal;
  Input_Tangent = (*(stage_input)).Input_Tangent;
  Input_UV1 = (*(stage_input)).Input_UV1;
  Input_UV2 = (*(stage_input)).Input_UV2;
  v_18();
  stage_output.gl_Position = gl_Position;
  stage_output._entryPointOutput_UV = _entryPointOutput_UV;
  stage_output._entryPointOutput_ProjBinormal = _entryPointOutput_ProjBinormal;
  stage_output._entryPointOutput_ProjTangent = _entryPointOutput_ProjTangent;
  stage_output._entryPointOutput_PosP = _entryPointOutput_PosP;
  stage_output._entryPointOutput_Color = _entryPointOutput_Color;
  return stage_output;
}

struct tint_symbol {
  @location(0u) @interpolate(perspective, centroid)
  m : vec2<f32>,
  @location(1u)
  m_1 : vec4<f32>,
  @location(2u)
  m_2 : vec4<f32>,
  @location(3u)
  m_3 : vec4<f32>,
  @location(4u) @interpolate(perspective, centroid)
  m_4 : vec4<f32>,
  @builtin(position)
  m_5 : vec4<f32>,
}

@vertex
fn main(@location(0u) v_19 : vec3<f32>, @location(1u) v_20 : vec4<f32>, @location(2u) v_21 : vec4<f32>, @location(3u) v_22 : vec4<f32>, @location(4u) v_23 : vec2<f32>, @location(5u) v_24 : vec2<f32>) -> tint_symbol {
  main_inner(v_19, v_20, v_21, v_22, v_23, v_24);
  return tint_symbol(v_1, v_2, v_3, v_4, v_5, v_6);
}
