diagnostic(off, derivative_uniformity);

struct EmitterData_1_1 {
  FlagBits : u32,
  Seed : u32,
  ParticleHead : u32,
  ParticleSize : u32,
  TrailHead : u32,
  TrailSize : u32,
  TrailPhase : u32,
  NextEmitCount : u32,
  TotalEmitCount : u32,
  EmitPointCount : u32,
  TimeCount : f32,
  TimeStopped : f32,
  Reserved0 : u32,
  Reserved1 : u32,
  DeltaTime : f32,
  Color : u32,
  Transform : mat3x4<f32>,
}

struct cb2_1_1 {
  _21_emitter : EmitterData_1_1,
}

@group(0) @binding(2) var<uniform> v : cb2_1_1;

struct Particles {
  m : array<u32>,
}

@group(2) @binding(0) var<storage, read_write> Particles_1 : Particles;

var<private> gl_GlobalInvocationID : vec3<u32>;

struct ComputeConstants {
  CoordinateReversed : u32,
  Reserved0 : f32,
  Reserved1 : f32,
  Reserved2 : f32,
}

struct cb0 {
  _95_constants : ComputeConstants,
}

@group(0) @binding(0) var<uniform> v_1 : cb0;

struct ParameterData {
  EmitCount : i32,
  EmitPerFrame : i32,
  EmitOffset : f32,
  Padding0 : u32,
  LifeTime : vec2<f32>,
  EmitShapeType : u32,
  EmitRotationApplied : u32,
  EmitShapeData : array<vec4<f32>, 2u>,
  Direction : vec3<f32>,
  Spread : f32,
  InitialSpeed : vec2<f32>,
  Damping : vec2<f32>,
  AngularOffset : array<vec4<f32>, 2u>,
  AngularVelocity : array<vec4<f32>, 2u>,
  ScaleData1 : array<vec4<f32>, 2u>,
  ScaleData2 : array<vec4<f32>, 2u>,
  ScaleEasing : vec3<f32>,
  ScaleFlags : u32,
  Gravity : vec3<f32>,
  Padding2 : u32,
  VortexCenter : vec3<f32>,
  VortexRotation : f32,
  VortexAxis : vec3<f32>,
  VortexAttraction : f32,
  TurbulencePower : f32,
  TurbulenceSeed : u32,
  TurbulenceScale : f32,
  TurbulenceOctave : f32,
  RenderState : u32,
  ShapeType : u32,
  ShapeData : u32,
  ShapeSize : f32,
  Emissive : f32,
  FadeIn : f32,
  FadeOut : f32,
  MaterialType : u32,
  ColorData : vec4<u32>,
  ColorEasing : vec3<f32>,
  ColorFlags : u32,
}

struct cb1 {
  _108_paramData : ParameterData,
}

@group(0) @binding(1) var<uniform> v_2 : cb1;

struct SPIRV_Cross_Input {
  gl_GlobalInvocationID : vec3<u32>,
}

@compute @workgroup_size(256u, 1u, 1u)
fn main(@builtin(global_invocation_id) v_3 : vec3<u32>) {
  var stage_input : SPIRV_Cross_Input;
  var param : SPIRV_Cross_Input;
  stage_input.gl_GlobalInvocationID = v_3;
  param = stage_input;
  v_4(&(param));
}

struct ParticleData {
  FlagBits : u32,
  Seed : u32,
  LifeAge : f32,
  InheritColor : u32,
  Color : u32,
  Direction : u32,
  Velocity : vec2<u32>,
  Transform : mat4x3<f32>,
}

fn v_5(dtid : ptr<function, vec3<u32>>) {
  var particleID : u32;
  var particle : ParticleData;
  var byteAddrTemp : i32;
  var byteAddrTemp_1 : i32;
  var byteAddrTemp_2 : i32;
  var byteAddrTemp_3 : i32;
  var byteAddrTemp_4 : i32;
  var byteAddrTemp_5 : i32;
  var byteAddrTemp_6 : i32;
  var byteAddrTemp_7 : i32;
  var byteAddrTemp_8 : i32;
  var byteAddrTemp_9 : i32;
  var byteAddrTemp_10 : i32;
  var byteAddrTemp_11 : i32;
  var byteAddrTemp_12 : i32;
  var byteAddrTemp_13 : i32;
  var byteAddrTemp_14 : i32;
  var byteAddrTemp_15 : i32;
  var byteAddrTemp_16 : i32;
  var byteAddrTemp_17 : i32;
  var byteAddrTemp_18 : i32;
  particleID = (v._21_emitter.ParticleHead + (*(dtid)).x);
  particle.FlagBits = 0u;
  particle.Seed = 0u;
  particle.LifeAge = 0.0f;
  particle.InheritColor = 0u;
  particle.Color = 0u;
  particle.Direction = 0u;
  particle.Velocity = vec2<u32>();
  particle.Transform = mat4x3<f32>();
  byteAddrTemp = bitcast<i32>((((particleID * 80u) + 0u) >> bitcast<u32>(2i)));
  let v_6 = byteAddrTemp;
  Particles_1.m[v_6] = particle.FlagBits;
  byteAddrTemp_1 = bitcast<i32>((((particleID * 80u) + 4u) >> bitcast<u32>(2i)));
  let v_7 = byteAddrTemp_1;
  Particles_1.m[v_7] = particle.Seed;
  byteAddrTemp_2 = bitcast<i32>((((particleID * 80u) + 8u) >> bitcast<u32>(2i)));
  let v_8 = byteAddrTemp_2;
  Particles_1.m[v_8] = bitcast<u32>(particle.LifeAge);
  byteAddrTemp_3 = bitcast<i32>((((particleID * 80u) + 12u) >> bitcast<u32>(2i)));
  let v_9 = byteAddrTemp_3;
  Particles_1.m[v_9] = particle.InheritColor;
  byteAddrTemp_4 = bitcast<i32>((((particleID * 80u) + 16u) >> bitcast<u32>(2i)));
  let v_10 = byteAddrTemp_4;
  Particles_1.m[v_10] = particle.Color;
  byteAddrTemp_5 = bitcast<i32>((((particleID * 80u) + 20u) >> bitcast<u32>(2i)));
  let v_11 = byteAddrTemp_5;
  Particles_1.m[v_11] = particle.Direction;
  byteAddrTemp_6 = bitcast<i32>((((particleID * 80u) + 24u) >> bitcast<u32>(2i)));
  let v_12 = byteAddrTemp_6;
  Particles_1.m[v_12] = particle.Velocity.x;
  let v_13 = (byteAddrTemp_6 + 1i);
  Particles_1.m[v_13] = particle.Velocity.y;
  byteAddrTemp_7 = bitcast<i32>((((particleID * 80u) + 32u) >> bitcast<u32>(2i)));
  let v_14 = byteAddrTemp_7;
  Particles_1.m[v_14] = bitcast<u32>(particle.Transform[0i].x);
  byteAddrTemp_8 = bitcast<i32>((((particleID * 80u) + 36u) >> bitcast<u32>(2i)));
  let v_15 = byteAddrTemp_8;
  Particles_1.m[v_15] = bitcast<u32>(particle.Transform[1i].x);
  byteAddrTemp_9 = bitcast<i32>((((particleID * 80u) + 40u) >> bitcast<u32>(2i)));
  let v_16 = byteAddrTemp_9;
  Particles_1.m[v_16] = bitcast<u32>(particle.Transform[2i].x);
  byteAddrTemp_10 = bitcast<i32>((((particleID * 80u) + 44u) >> bitcast<u32>(2i)));
  let v_17 = byteAddrTemp_10;
  Particles_1.m[v_17] = bitcast<u32>(particle.Transform[3i].x);
  byteAddrTemp_11 = bitcast<i32>((((particleID * 80u) + 48u) >> bitcast<u32>(2i)));
  let v_18 = byteAddrTemp_11;
  Particles_1.m[v_18] = bitcast<u32>(particle.Transform[0i].y);
  byteAddrTemp_12 = bitcast<i32>((((particleID * 80u) + 52u) >> bitcast<u32>(2i)));
  let v_19 = byteAddrTemp_12;
  Particles_1.m[v_19] = bitcast<u32>(particle.Transform[1i].y);
  byteAddrTemp_13 = bitcast<i32>((((particleID * 80u) + 56u) >> bitcast<u32>(2i)));
  let v_20 = byteAddrTemp_13;
  Particles_1.m[v_20] = bitcast<u32>(particle.Transform[2i].y);
  byteAddrTemp_14 = bitcast<i32>((((particleID * 80u) + 60u) >> bitcast<u32>(2i)));
  let v_21 = byteAddrTemp_14;
  Particles_1.m[v_21] = bitcast<u32>(particle.Transform[3i].y);
  byteAddrTemp_15 = bitcast<i32>((((particleID * 80u) + 64u) >> bitcast<u32>(2i)));
  let v_22 = byteAddrTemp_15;
  Particles_1.m[v_22] = bitcast<u32>(particle.Transform[0i].z);
  byteAddrTemp_16 = bitcast<i32>((((particleID * 80u) + 68u) >> bitcast<u32>(2i)));
  let v_23 = byteAddrTemp_16;
  Particles_1.m[v_23] = bitcast<u32>(particle.Transform[1i].z);
  byteAddrTemp_17 = bitcast<i32>((((particleID * 80u) + 72u) >> bitcast<u32>(2i)));
  let v_24 = byteAddrTemp_17;
  Particles_1.m[v_24] = bitcast<u32>(particle.Transform[2i].z);
  byteAddrTemp_18 = bitcast<i32>((((particleID * 80u) + 76u) >> bitcast<u32>(2i)));
  let v_25 = byteAddrTemp_18;
  Particles_1.m[v_25] = bitcast<u32>(particle.Transform[3i].z);
}

fn v_26() {
  var dtid : vec3<u32>;
  var param : vec3<u32>;
  var param_1 : vec3<u32>;
  dtid = gl_GlobalInvocationID;
  param = dtid;
  param_1 = param;
  v_5(&(param_1));
}

fn v_4(stage_input : ptr<function, SPIRV_Cross_Input>) {
  gl_GlobalInvocationID = (*(stage_input)).gl_GlobalInvocationID;
  v_26();
}
