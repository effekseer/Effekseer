#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//
//   fxc /Zpc /Tps_4_0 /Emain /Fh ShaderHeader/ad_model_distortion_ps.h
//    Shader/ad_model_distortion_ps.fx
//
//
// Buffer Definitions: 
//
// cbuffer PS_ConstanBuffer
// {
//
//   float4 _263_g_scale;               // Offset:    0 Size:    16
//   float4 _263_mUVInversedBack;       // Offset:   16 Size:    16
//   float4 _263_fFlipbookParameter;    // Offset:   32 Size:    16
//   float4 _263_fUVDistortionParameter;// Offset:   48 Size:    16
//   float4 _263_fBlendTextureParameter;// Offset:   64 Size:    16
//   float4 _263_softParticleAndReconstructionParam1;// Offset:   80 Size:    16
//   float4 _263_reconstructionParam2;  // Offset:   96 Size:    16
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim Slot Elements
// ------------------------------ ---------- ------- ----------- ---- --------
// sampler_colorTex                  sampler      NA          NA    0        1
// sampler_backTex                   sampler      NA          NA    1        1
// sampler_alphaTex                  sampler      NA          NA    2        1
// sampler_uvDistortionTex           sampler      NA          NA    3        1
// sampler_blendTex                  sampler      NA          NA    4        1
// sampler_blendAlphaTex             sampler      NA          NA    5        1
// sampler_blendUVDistortionTex      sampler      NA          NA    6        1
// sampler_depthTex                  sampler      NA          NA    7        1
// _colorTex                         texture  float4          2d    0        1
// _backTex                          texture  float4          2d    1        1
// _alphaTex                         texture  float4          2d    2        1
// _uvDistortionTex                  texture  float4          2d    3        1
// _blendTex                         texture  float4          2d    4        1
// _blendAlphaTex                    texture  float4          2d    5        1
// _blendUVDistortionTex             texture  float4          2d    6        1
// _depthTex                         texture  float4          2d    7        1
// PS_ConstanBuffer                  cbuffer      NA          NA    1        1
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue Format   Used
// -------------------- ----- ------ -------- -------- ------ ------
// TEXCOORD                 0   xy          0     NONE  float   xy  
// TEXCOORD                 1   xyzw        1     NONE  float   xy w
// TEXCOORD                 2   xyzw        2     NONE  float   xy w
// TEXCOORD                 3   xyzw        3     NONE  float   xyzw
// TEXCOORD                 4   xyzw        4     NONE  float   xy w
// TEXCOORD                 5   xyzw        5     NONE  float   xyzw
// TEXCOORD                 6   xyzw        6     NONE  float   xyzw
// TEXCOORD                 7   xyzw        7     NONE  float   xyzw
// TEXCOORD                 8   xy          8     NONE  float   xy  
// SV_Position              0   xyzw        9      POS  float       
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue Format   Used
// -------------------- ----- ------ -------- -------- ------ ------
// SV_Target                0   xyzw        0   TARGET  float   xyzw
//
ps_4_0
dcl_constantbuffer cb1[7], immediateIndexed
dcl_sampler s0, mode_default
dcl_sampler s1, mode_default
dcl_sampler s2, mode_default
dcl_sampler s3, mode_default
dcl_sampler s4, mode_default
dcl_sampler s5, mode_default
dcl_sampler s6, mode_default
dcl_sampler s7, mode_default
dcl_resource_texture2d (float,float,float,float) t0
dcl_resource_texture2d (float,float,float,float) t1
dcl_resource_texture2d (float,float,float,float) t2
dcl_resource_texture2d (float,float,float,float) t3
dcl_resource_texture2d (float,float,float,float) t4
dcl_resource_texture2d (float,float,float,float) t5
dcl_resource_texture2d (float,float,float,float) t6
dcl_resource_texture2d (float,float,float,float) t7
dcl_input_ps linear centroid v0.xy
dcl_input_ps linear v1.xyw
dcl_input_ps linear v2.xyw
dcl_input_ps linear v3.xyzw
dcl_input_ps linear centroid v4.xyw
dcl_input_ps linear v5.xyzw
dcl_input_ps linear v6.xyzw
dcl_input_ps linear v7.xyzw
dcl_input_ps linear v8.xy
dcl_output o0.xyzw
dcl_temps 4
sample r0.xyzw, v5.zwzz, t3.xyzw, s3
mad r0.xy, r0.xyxx, l(2.000000, 2.000000, 0.000000, 0.000000), l(-1.000000, -1.000000, 0.000000, 0.000000)
mad r0.z, -r0.y, cb1[3].w, cb1[3].z
mad r0.yw, r0.xxxz, cb1[3].xxxx, v0.xxxy
sample r1.xyzw, r0.ywyy, t0.xyzw, s0
mul r1.z, r1.w, v4.w
lt r0.y, l(0.000000), cb1[2].x
if_nz r0.y
  mad r0.yw, r0.xxxz, cb1[3].xxxx, v7.zzzw
  sample r2.xyzw, r0.ywyy, t0.xyzw, s0
  eq r0.y, cb1[2].y, l(1.000000)
  mad r2.xyz, r2.xywx, v4.xywx, -r1.xyzx
  mad r2.xyz, v8.xxxx, r2.xyzx, r1.xyzx
  movc r1.xyz, r0.yyyy, r2.xyzx, r1.xyzx
endif 
mad r0.xy, r0.xzxx, cb1[3].xxxx, v5.xyxx
sample r0.xyzw, r0.xyxx, t2.xyzw, s2
mul r0.x, r0.w, r0.x
mul r0.x, r0.x, r1.z
max r0.y, v8.y, l(0.000000)
ge r0.y, r0.y, r0.x
discard r0.y
sample r2.xyzw, v6.zwzz, t6.xyzw, s6
mad r2.xy, r2.xyxx, l(2.000000, 2.000000, 0.000000, 0.000000), l(-1.000000, -1.000000, 0.000000, 0.000000)
mad r2.z, -r2.y, cb1[3].w, cb1[3].z
mad r0.yz, r2.xxzx, cb1[3].yyyy, v7.xxyx
sample r3.xyzw, r0.yzyy, t4.xyzw, s4
mad r0.yz, r2.xxzx, cb1[3].yyyy, v6.xxyx
sample r2.xyzw, r0.yzyy, t5.xyzw, s5
mul r0.y, r2.w, r2.x
mul r0.z, r0.y, r3.w
mul r1.zw, r0.zzzz, r3.xxxy
mad r0.y, -r3.w, r0.y, l(1.000000)
mad r0.yw, r1.xxxy, r0.yyyy, r1.zzzw
mad r2.xy, r3.xyxx, r0.zzzz, r1.xyxx
mad r2.zw, -r3.xxxy, r0.zzzz, r1.xxxy
eq r3.xyzw, cb1[4].xxxx, l(0.000000, 1.000000, 2.000000, 3.000000)
mul r1.zw, r1.zzzw, r1.xxxy
movc r1.xy, r3.wwww, r1.zwzz, r1.xyxx
movc r1.xy, r3.zzzz, r2.zwzz, r1.xyxx
movc r1.xy, r3.yyyy, r2.xyxx, r1.xyxx
movc r0.yz, r3.xxxx, r0.yywy, r1.xxyx
div r1.xy, v3.xyxx, v3.wwww
div r1.zw, v2.xxxy, v2.wwww
div r2.xy, v1.xyxx, v1.wwww
mad r0.yz, r0.yyzy, l(0.000000, 2.000000, 2.000000, 0.000000), l(0.000000, -1.000000, -1.000000, 0.000000)
mul r0.yz, r0.yyzy, v4.xxyx
mul r0.yz, r0.yyzy, cb1[0].xxxx
add r1.zw, -r1.xxxy, r1.zzzw
mad r0.yw, r1.zzzw, r0.yyyy, r1.xxxy
add r1.xy, -r1.xyxx, r2.xyxx
mad r0.yz, r1.xxyx, r0.zzzz, r0.yywy
add r0.yz, r0.yyzy, l(0.000000, 1.000000, 1.000000, 0.000000)
mul r1.x, r0.y, l(0.500000)
mad r0.y, -r0.z, l(0.500000), l(1.000000)
mad r1.z, cb1[1].y, r0.y, cb1[1].x
sample r1.xyzw, r1.xzxx, t1.xyzw, s1
div r2.yzw, v3.xxyz, v3.wwww
add r0.yz, r2.yyzy, l(0.000000, 1.000000, 1.000000, 0.000000)
mul r3.x, r0.y, l(0.500000)
mad r3.z, -r0.z, l(0.500000), l(1.000000)
sample r3.xyzw, r3.xzxx, t7.xyzw, s7
ne r0.y, cb1[5].x, l(0.000000)
mad r2.x, r3.x, cb1[5].y, cb1[5].z
mad r0.zw, r2.xxxw, cb1[6].wwww, -cb1[6].yyyy
mad r2.xy, -r2.xwxx, cb1[6].zzzz, cb1[6].xxxx
div r0.zw, r0.zzzw, r2.xxxy
add r0.z, -r0.z, r0.w
div_sat r0.z, r0.z, cb1[5].x
mul r0.z, r0.z, r0.x
movc o0.w, r0.y, r0.z, r0.x
mov o0.xyz, r1.xyzx
ret 
// Approximately 73 instruction slots used
#endif

const BYTE g_main[] =
{
     68,  88,  66,  67, 197, 146, 
     96,  61, 249, 185, 235,  85, 
    212,  84,  87, 187,  74, 202, 
     14, 151,   1,   0,   0,   0, 
    104,  18,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
     76,   5,   0,   0, 100,   6, 
      0,   0, 152,   6,   0,   0, 
    236,  17,   0,   0,  82,  68, 
     69,  70,  16,   5,   0,   0, 
      1,   0,   0,   0,  84,   3, 
      0,   0,  17,   0,   0,   0, 
     28,   0,   0,   0,   0,   4, 
    255, 255,  16,   1,   0,   0, 
    220,   4,   0,   0,  60,   2, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
     77,   2,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,  93,   2,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0, 110,   2, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
    134,   2,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   4,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0, 151,   2,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   5,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0, 173,   2, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      6,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
    202,   2,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   7,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0, 219,   2,   0,   0, 
      2,   0,   0,   0,   5,   0, 
      0,   0,   4,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,   1,   0,   0,   0, 
     13,   0,   0,   0, 229,   2, 
      0,   0,   2,   0,   0,   0, 
      5,   0,   0,   0,   4,   0, 
      0,   0, 255, 255, 255, 255, 
      1,   0,   0,   0,   1,   0, 
      0,   0,  13,   0,   0,   0, 
    238,   2,   0,   0,   2,   0, 
      0,   0,   5,   0,   0,   0, 
      4,   0,   0,   0, 255, 255, 
    255, 255,   2,   0,   0,   0, 
      1,   0,   0,   0,  13,   0, 
      0,   0, 248,   2,   0,   0, 
      2,   0,   0,   0,   5,   0, 
      0,   0,   4,   0,   0,   0, 
    255, 255, 255, 255,   3,   0, 
      0,   0,   1,   0,   0,   0, 
     13,   0,   0,   0,   9,   3, 
      0,   0,   2,   0,   0,   0, 
      5,   0,   0,   0,   4,   0, 
      0,   0, 255, 255, 255, 255, 
      4,   0,   0,   0,   1,   0, 
      0,   0,  13,   0,   0,   0, 
     19,   3,   0,   0,   2,   0, 
      0,   0,   5,   0,   0,   0, 
      4,   0,   0,   0, 255, 255, 
    255, 255,   5,   0,   0,   0, 
      1,   0,   0,   0,  13,   0, 
      0,   0,  34,   3,   0,   0, 
      2,   0,   0,   0,   5,   0, 
      0,   0,   4,   0,   0,   0, 
    255, 255, 255, 255,   6,   0, 
      0,   0,   1,   0,   0,   0, 
     13,   0,   0,   0,  56,   3, 
      0,   0,   2,   0,   0,   0, 
      5,   0,   0,   0,   4,   0, 
      0,   0, 255, 255, 255, 255, 
      7,   0,   0,   0,   1,   0, 
      0,   0,  13,   0,   0,   0, 
     66,   3,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0, 115,  97, 109, 112, 
    108, 101, 114,  95,  99, 111, 
    108, 111, 114,  84, 101, 120, 
      0, 115,  97, 109, 112, 108, 
    101, 114,  95,  98,  97,  99, 
    107,  84, 101, 120,   0, 115, 
     97, 109, 112, 108, 101, 114, 
     95,  97, 108, 112, 104,  97, 
     84, 101, 120,   0, 115,  97, 
    109, 112, 108, 101, 114,  95, 
    117, 118,  68, 105, 115, 116, 
    111, 114, 116, 105, 111, 110, 
     84, 101, 120,   0, 115,  97, 
    109, 112, 108, 101, 114,  95, 
     98, 108, 101, 110, 100,  84, 
    101, 120,   0, 115,  97, 109, 
    112, 108, 101, 114,  95,  98, 
    108, 101, 110, 100,  65, 108, 
    112, 104,  97,  84, 101, 120, 
      0, 115,  97, 109, 112, 108, 
    101, 114,  95,  98, 108, 101, 
    110, 100,  85,  86,  68, 105, 
    115, 116, 111, 114, 116, 105, 
    111, 110,  84, 101, 120,   0, 
    115,  97, 109, 112, 108, 101, 
    114,  95, 100, 101, 112, 116, 
    104,  84, 101, 120,   0,  95, 
     99, 111, 108, 111, 114,  84, 
    101, 120,   0,  95,  98,  97, 
     99, 107,  84, 101, 120,   0, 
     95,  97, 108, 112, 104,  97, 
     84, 101, 120,   0,  95, 117, 
    118,  68, 105, 115, 116, 111, 
    114, 116, 105, 111, 110,  84, 
    101, 120,   0,  95,  98, 108, 
    101, 110, 100,  84, 101, 120, 
      0,  95,  98, 108, 101, 110, 
    100,  65, 108, 112, 104,  97, 
     84, 101, 120,   0,  95,  98, 
    108, 101, 110, 100,  85,  86, 
     68, 105, 115, 116, 111, 114, 
    116, 105, 111, 110,  84, 101, 
    120,   0,  95, 100, 101, 112, 
    116, 104,  84, 101, 120,   0, 
     80,  83,  95,  67, 111, 110, 
    115, 116,  97, 110,  66, 117, 
    102, 102, 101, 114,   0, 171, 
     66,   3,   0,   0,   7,   0, 
      0,   0, 108,   3,   0,   0, 
    112,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     20,   4,   0,   0,   0,   0, 
      0,   0,  16,   0,   0,   0, 
      2,   0,   0,   0,  36,   4, 
      0,   0,   0,   0,   0,   0, 
     52,   4,   0,   0,  16,   0, 
      0,   0,  16,   0,   0,   0, 
      2,   0,   0,   0,  36,   4, 
      0,   0,   0,   0,   0,   0, 
     73,   4,   0,   0,  32,   0, 
      0,   0,  16,   0,   0,   0, 
      2,   0,   0,   0,  36,   4, 
      0,   0,   0,   0,   0,   0, 
     97,   4,   0,   0,  48,   0, 
      0,   0,  16,   0,   0,   0, 
      2,   0,   0,   0,  36,   4, 
      0,   0,   0,   0,   0,   0, 
    125,   4,   0,   0,  64,   0, 
      0,   0,  16,   0,   0,   0, 
      2,   0,   0,   0,  36,   4, 
      0,   0,   0,   0,   0,   0, 
    153,   4,   0,   0,  80,   0, 
      0,   0,  16,   0,   0,   0, 
      2,   0,   0,   0,  36,   4, 
      0,   0,   0,   0,   0,   0, 
    194,   4,   0,   0,  96,   0, 
      0,   0,  16,   0,   0,   0, 
      2,   0,   0,   0,  36,   4, 
      0,   0,   0,   0,   0,   0, 
     95,  50,  54,  51,  95, 103, 
     95, 115,  99,  97, 108, 101, 
      0, 171, 171, 171,   1,   0, 
      3,   0,   1,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  95,  50,  54,  51, 
     95, 109,  85,  86,  73, 110, 
    118, 101, 114, 115, 101, 100, 
     66,  97,  99, 107,   0,  95, 
     50,  54,  51,  95, 102,  70, 
    108, 105, 112,  98, 111, 111, 
    107,  80,  97, 114,  97, 109, 
    101, 116, 101, 114,   0,  95, 
     50,  54,  51,  95, 102,  85, 
     86,  68, 105, 115, 116, 111, 
    114, 116, 105, 111, 110,  80, 
     97, 114,  97, 109, 101, 116, 
    101, 114,   0,  95,  50,  54, 
     51,  95, 102,  66, 108, 101, 
    110, 100,  84, 101, 120, 116, 
    117, 114, 101,  80,  97, 114, 
     97, 109, 101, 116, 101, 114, 
      0,  95,  50,  54,  51,  95, 
    115, 111, 102, 116,  80,  97, 
    114, 116, 105,  99, 108, 101, 
     65, 110, 100,  82, 101,  99, 
    111, 110, 115, 116, 114, 117, 
     99, 116, 105, 111, 110,  80, 
     97, 114,  97, 109,  49,   0, 
     95,  50,  54,  51,  95, 114, 
    101,  99, 111, 110, 115, 116, 
    114, 117,  99, 116, 105, 111, 
    110,  80,  97, 114,  97, 109, 
     50,   0,  77, 105,  99, 114, 
    111, 115, 111, 102, 116,  32, 
     40,  82,  41,  32,  72,  76, 
     83,  76,  32,  83, 104,  97, 
    100, 101, 114,  32,  67, 111, 
    109, 112, 105, 108, 101, 114, 
     32,  57,  46,  50,  57,  46, 
     57,  53,  50,  46,  51,  49, 
     49,  49,   0, 171, 171, 171, 
     73,  83,  71,  78,  16,   1, 
      0,   0,  10,   0,   0,   0, 
      8,   0,   0,   0, 248,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   3,   0,   0, 248,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
     15,  11,   0,   0, 248,   0, 
      0,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   2,   0,   0,   0, 
     15,  11,   0,   0, 248,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   3,   0,   0,   0, 
     15,  15,   0,   0, 248,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   4,   0,   0,   0, 
     15,  11,   0,   0, 248,   0, 
      0,   0,   5,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   5,   0,   0,   0, 
     15,  15,   0,   0, 248,   0, 
      0,   0,   6,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   6,   0,   0,   0, 
     15,  15,   0,   0, 248,   0, 
      0,   0,   7,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   7,   0,   0,   0, 
     15,  15,   0,   0, 248,   0, 
      0,   0,   8,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   8,   0,   0,   0, 
      3,   3,   0,   0,   1,   1, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   3,   0, 
      0,   0,   9,   0,   0,   0, 
     15,   0,   0,   0,  84,  69, 
     88,  67,  79,  79,  82,  68, 
      0,  83,  86,  95,  80, 111, 
    115, 105, 116, 105, 111, 110, 
      0, 171, 171, 171,  79,  83, 
     71,  78,  44,   0,   0,   0, 
      1,   0,   0,   0,   8,   0, 
      0,   0,  32,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,  15,   0, 
      0,   0,  83,  86,  95,  84, 
     97, 114, 103, 101, 116,   0, 
    171, 171,  83,  72,  68,  82, 
     76,  11,   0,   0,  64,   0, 
      0,   0, 211,   2,   0,   0, 
     89,   0,   0,   4,  70, 142, 
     32,   0,   1,   0,   0,   0, 
      7,   0,   0,   0,  90,   0, 
      0,   3,   0,  96,  16,   0, 
      0,   0,   0,   0,  90,   0, 
      0,   3,   0,  96,  16,   0, 
      1,   0,   0,   0,  90,   0, 
      0,   3,   0,  96,  16,   0, 
      2,   0,   0,   0,  90,   0, 
      0,   3,   0,  96,  16,   0, 
      3,   0,   0,   0,  90,   0, 
      0,   3,   0,  96,  16,   0, 
      4,   0,   0,   0,  90,   0, 
      0,   3,   0,  96,  16,   0, 
      5,   0,   0,   0,  90,   0, 
      0,   3,   0,  96,  16,   0, 
      6,   0,   0,   0,  90,   0, 
      0,   3,   0,  96,  16,   0, 
      7,   0,   0,   0,  88,  24, 
      0,   4,   0, 112,  16,   0, 
      0,   0,   0,   0,  85,  85, 
      0,   0,  88,  24,   0,   4, 
      0, 112,  16,   0,   1,   0, 
      0,   0,  85,  85,   0,   0, 
     88,  24,   0,   4,   0, 112, 
     16,   0,   2,   0,   0,   0, 
     85,  85,   0,   0,  88,  24, 
      0,   4,   0, 112,  16,   0, 
      3,   0,   0,   0,  85,  85, 
      0,   0,  88,  24,   0,   4, 
      0, 112,  16,   0,   4,   0, 
      0,   0,  85,  85,   0,   0, 
     88,  24,   0,   4,   0, 112, 
     16,   0,   5,   0,   0,   0, 
     85,  85,   0,   0,  88,  24, 
      0,   4,   0, 112,  16,   0, 
      6,   0,   0,   0,  85,  85, 
      0,   0,  88,  24,   0,   4, 
      0, 112,  16,   0,   7,   0, 
      0,   0,  85,  85,   0,   0, 
     98,  24,   0,   3,  50,  16, 
     16,   0,   0,   0,   0,   0, 
     98,  16,   0,   3, 178,  16, 
     16,   0,   1,   0,   0,   0, 
     98,  16,   0,   3, 178,  16, 
     16,   0,   2,   0,   0,   0, 
     98,  16,   0,   3, 242,  16, 
     16,   0,   3,   0,   0,   0, 
     98,  24,   0,   3, 178,  16, 
     16,   0,   4,   0,   0,   0, 
     98,  16,   0,   3, 242,  16, 
     16,   0,   5,   0,   0,   0, 
     98,  16,   0,   3, 242,  16, 
     16,   0,   6,   0,   0,   0, 
     98,  16,   0,   3, 242,  16, 
     16,   0,   7,   0,   0,   0, 
     98,  16,   0,   3,  50,  16, 
     16,   0,   8,   0,   0,   0, 
    101,   0,   0,   3, 242,  32, 
     16,   0,   0,   0,   0,   0, 
    104,   0,   0,   2,   4,   0, 
      0,   0,  69,   0,   0,   9, 
    242,   0,  16,   0,   0,   0, 
      0,   0, 230,  26,  16,   0, 
      5,   0,   0,   0,  70, 126, 
     16,   0,   3,   0,   0,   0, 
      0,  96,  16,   0,   3,   0, 
      0,   0,  50,   0,   0,  15, 
     50,   0,  16,   0,   0,   0, 
      0,   0,  70,   0,  16,   0, 
      0,   0,   0,   0,   2,  64, 
      0,   0,   0,   0,   0,  64, 
      0,   0,   0,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
    128, 191,   0,   0, 128, 191, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  50,   0,   0,  12, 
     66,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16, 128, 
     65,   0,   0,   0,   0,   0, 
      0,   0,  58, 128,  32,   0, 
      1,   0,   0,   0,   3,   0, 
      0,   0,  42, 128,  32,   0, 
      1,   0,   0,   0,   3,   0, 
      0,   0,  50,   0,   0,  10, 
    162,   0,  16,   0,   0,   0, 
      0,   0,   6,   8,  16,   0, 
      0,   0,   0,   0,   6, 128, 
     32,   0,   1,   0,   0,   0, 
      3,   0,   0,   0,   6,  20, 
     16,   0,   0,   0,   0,   0, 
     69,   0,   0,   9, 242,   0, 
     16,   0,   1,   0,   0,   0, 
    214,   5,  16,   0,   0,   0, 
      0,   0,  70, 126,  16,   0, 
      0,   0,   0,   0,   0,  96, 
     16,   0,   0,   0,   0,   0, 
     56,   0,   0,   7,  66,   0, 
     16,   0,   1,   0,   0,   0, 
     58,   0,  16,   0,   1,   0, 
      0,   0,  58,  16,  16,   0, 
      4,   0,   0,   0,  49,   0, 
      0,   8,  34,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
     10, 128,  32,   0,   1,   0, 
      0,   0,   2,   0,   0,   0, 
     31,   0,   4,   3,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     50,   0,   0,  10, 162,   0, 
     16,   0,   0,   0,   0,   0, 
      6,   8,  16,   0,   0,   0, 
      0,   0,   6, 128,  32,   0, 
      1,   0,   0,   0,   3,   0, 
      0,   0, 166,  30,  16,   0, 
      7,   0,   0,   0,  69,   0, 
      0,   9, 242,   0,  16,   0, 
      2,   0,   0,   0, 214,   5, 
     16,   0,   0,   0,   0,   0, 
     70, 126,  16,   0,   0,   0, 
      0,   0,   0,  96,  16,   0, 
      0,   0,   0,   0,  24,   0, 
      0,   8,  34,   0,  16,   0, 
      0,   0,   0,   0,  26, 128, 
     32,   0,   1,   0,   0,   0, 
      2,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128,  63, 
     50,   0,   0,  10, 114,   0, 
     16,   0,   2,   0,   0,   0, 
     70,   3,  16,   0,   2,   0, 
      0,   0,  70,  19,  16,   0, 
      4,   0,   0,   0,  70,   2, 
     16, 128,  65,   0,   0,   0, 
      1,   0,   0,   0,  50,   0, 
      0,   9, 114,   0,  16,   0, 
      2,   0,   0,   0,   6,  16, 
     16,   0,   8,   0,   0,   0, 
     70,   2,  16,   0,   2,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0,  55,   0, 
      0,   9, 114,   0,  16,   0, 
      1,   0,   0,   0,  86,   5, 
     16,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   2,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0,  21,   0, 
      0,   1,  50,   0,   0,  10, 
     50,   0,  16,   0,   0,   0, 
      0,   0, 134,   0,  16,   0, 
      0,   0,   0,   0,   6, 128, 
     32,   0,   1,   0,   0,   0, 
      3,   0,   0,   0,  70,  16, 
     16,   0,   5,   0,   0,   0, 
     69,   0,   0,   9, 242,   0, 
     16,   0,   0,   0,   0,   0, 
     70,   0,  16,   0,   0,   0, 
      0,   0,  70, 126,  16,   0, 
      2,   0,   0,   0,   0,  96, 
     16,   0,   2,   0,   0,   0, 
     56,   0,   0,   7,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     58,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  56,   0, 
      0,   7,  18,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
     42,   0,  16,   0,   1,   0, 
      0,   0,  52,   0,   0,   7, 
     34,   0,  16,   0,   0,   0, 
      0,   0,  26,  16,  16,   0, 
      8,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
     29,   0,   0,   7,  34,   0, 
     16,   0,   0,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  13,   0, 
      4,   3,  26,   0,  16,   0, 
      0,   0,   0,   0,  69,   0, 
      0,   9, 242,   0,  16,   0, 
      2,   0,   0,   0, 230,  26, 
     16,   0,   6,   0,   0,   0, 
     70, 126,  16,   0,   6,   0, 
      0,   0,   0,  96,  16,   0, 
      6,   0,   0,   0,  50,   0, 
      0,  15,  50,   0,  16,   0, 
      2,   0,   0,   0,  70,   0, 
     16,   0,   2,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,  64,   0,   0,   0,  64, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0, 128, 191,   0,   0, 
    128, 191,   0,   0,   0,   0, 
      0,   0,   0,   0,  50,   0, 
      0,  12,  66,   0,  16,   0, 
      2,   0,   0,   0,  26,   0, 
     16, 128,  65,   0,   0,   0, 
      2,   0,   0,   0,  58, 128, 
     32,   0,   1,   0,   0,   0, 
      3,   0,   0,   0,  42, 128, 
     32,   0,   1,   0,   0,   0, 
      3,   0,   0,   0,  50,   0, 
      0,  10,  98,   0,  16,   0, 
      0,   0,   0,   0,   6,   2, 
     16,   0,   2,   0,   0,   0, 
     86, 133,  32,   0,   1,   0, 
      0,   0,   3,   0,   0,   0, 
      6,  17,  16,   0,   7,   0, 
      0,   0,  69,   0,   0,   9, 
    242,   0,  16,   0,   3,   0, 
      0,   0, 150,   5,  16,   0, 
      0,   0,   0,   0,  70, 126, 
     16,   0,   4,   0,   0,   0, 
      0,  96,  16,   0,   4,   0, 
      0,   0,  50,   0,   0,  10, 
     98,   0,  16,   0,   0,   0, 
      0,   0,   6,   2,  16,   0, 
      2,   0,   0,   0,  86, 133, 
     32,   0,   1,   0,   0,   0, 
      3,   0,   0,   0,   6,  17, 
     16,   0,   6,   0,   0,   0, 
     69,   0,   0,   9, 242,   0, 
     16,   0,   2,   0,   0,   0, 
    150,   5,  16,   0,   0,   0, 
      0,   0,  70, 126,  16,   0, 
      5,   0,   0,   0,   0,  96, 
     16,   0,   5,   0,   0,   0, 
     56,   0,   0,   7,  34,   0, 
     16,   0,   0,   0,   0,   0, 
     58,   0,  16,   0,   2,   0, 
      0,   0,  10,   0,  16,   0, 
      2,   0,   0,   0,  56,   0, 
      0,   7,  66,   0,  16,   0, 
      0,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     58,   0,  16,   0,   3,   0, 
      0,   0,  56,   0,   0,   7, 
    194,   0,  16,   0,   1,   0, 
      0,   0, 166,  10,  16,   0, 
      0,   0,   0,   0,   6,   4, 
     16,   0,   3,   0,   0,   0, 
     50,   0,   0,  10,  34,   0, 
     16,   0,   0,   0,   0,   0, 
     58,   0,  16, 128,  65,   0, 
      0,   0,   3,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0, 128,  63,  50,   0, 
      0,   9, 162,   0,  16,   0, 
      0,   0,   0,   0,   6,   4, 
     16,   0,   1,   0,   0,   0, 
     86,   5,  16,   0,   0,   0, 
      0,   0, 166,  14,  16,   0, 
      1,   0,   0,   0,  50,   0, 
      0,   9,  50,   0,  16,   0, 
      2,   0,   0,   0,  70,   0, 
     16,   0,   3,   0,   0,   0, 
    166,  10,  16,   0,   0,   0, 
      0,   0,  70,   0,  16,   0, 
      1,   0,   0,   0,  50,   0, 
      0,  10, 194,   0,  16,   0, 
      2,   0,   0,   0,   6,   4, 
     16, 128,  65,   0,   0,   0, 
      3,   0,   0,   0, 166,  10, 
     16,   0,   0,   0,   0,   0, 
      6,   4,  16,   0,   1,   0, 
      0,   0,  24,   0,   0,  11, 
    242,   0,  16,   0,   3,   0, 
      0,   0,   6, 128,  32,   0, 
      1,   0,   0,   0,   4,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    128,  63,   0,   0,   0,  64, 
      0,   0,  64,  64,  56,   0, 
      0,   7, 194,   0,  16,   0, 
      1,   0,   0,   0, 166,  14, 
     16,   0,   1,   0,   0,   0, 
      6,   4,  16,   0,   1,   0, 
      0,   0,  55,   0,   0,   9, 
     50,   0,  16,   0,   1,   0, 
      0,   0, 246,  15,  16,   0, 
      3,   0,   0,   0, 230,  10, 
     16,   0,   1,   0,   0,   0, 
     70,   0,  16,   0,   1,   0, 
      0,   0,  55,   0,   0,   9, 
     50,   0,  16,   0,   1,   0, 
      0,   0, 166,  10,  16,   0, 
      3,   0,   0,   0, 230,  10, 
     16,   0,   2,   0,   0,   0, 
     70,   0,  16,   0,   1,   0, 
      0,   0,  55,   0,   0,   9, 
     50,   0,  16,   0,   1,   0, 
      0,   0,  86,   5,  16,   0, 
      3,   0,   0,   0,  70,   0, 
     16,   0,   2,   0,   0,   0, 
     70,   0,  16,   0,   1,   0, 
      0,   0,  55,   0,   0,   9, 
     98,   0,  16,   0,   0,   0, 
      0,   0,   6,   0,  16,   0, 
      3,   0,   0,   0,  86,   7, 
     16,   0,   0,   0,   0,   0, 
      6,   1,  16,   0,   1,   0, 
      0,   0,  14,   0,   0,   7, 
     50,   0,  16,   0,   1,   0, 
      0,   0,  70,  16,  16,   0, 
      3,   0,   0,   0, 246,  31, 
     16,   0,   3,   0,   0,   0, 
     14,   0,   0,   7, 194,   0, 
     16,   0,   1,   0,   0,   0, 
      6,  20,  16,   0,   2,   0, 
      0,   0, 246,  31,  16,   0, 
      2,   0,   0,   0,  14,   0, 
      0,   7,  50,   0,  16,   0, 
      2,   0,   0,   0,  70,  16, 
     16,   0,   1,   0,   0,   0, 
    246,  31,  16,   0,   1,   0, 
      0,   0,  50,   0,   0,  15, 
     98,   0,  16,   0,   0,   0, 
      0,   0,  86,   6,  16,   0, 
      0,   0,   0,   0,   2,  64, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,  64,   0,   0, 
      0,  64,   0,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0, 128, 191, 
      0,   0, 128, 191,   0,   0, 
      0,   0,  56,   0,   0,   7, 
     98,   0,  16,   0,   0,   0, 
      0,   0,  86,   6,  16,   0, 
      0,   0,   0,   0,   6,  17, 
     16,   0,   4,   0,   0,   0, 
     56,   0,   0,   8,  98,   0, 
     16,   0,   0,   0,   0,   0, 
     86,   6,  16,   0,   0,   0, 
      0,   0,   6, 128,  32,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   8, 
    194,   0,  16,   0,   1,   0, 
      0,   0,   6,   4,  16, 128, 
     65,   0,   0,   0,   1,   0, 
      0,   0, 166,  14,  16,   0, 
      1,   0,   0,   0,  50,   0, 
      0,   9, 162,   0,  16,   0, 
      0,   0,   0,   0, 166,  14, 
     16,   0,   1,   0,   0,   0, 
     86,   5,  16,   0,   0,   0, 
      0,   0,   6,   4,  16,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   8,  50,   0,  16,   0, 
      1,   0,   0,   0,  70,   0, 
     16, 128,  65,   0,   0,   0, 
      1,   0,   0,   0,  70,   0, 
     16,   0,   2,   0,   0,   0, 
     50,   0,   0,   9,  98,   0, 
     16,   0,   0,   0,   0,   0, 
      6,   1,  16,   0,   1,   0, 
      0,   0, 166,  10,  16,   0, 
      0,   0,   0,   0,  86,   7, 
     16,   0,   0,   0,   0,   0, 
      0,   0,   0,  10,  98,   0, 
     16,   0,   0,   0,   0,   0, 
     86,   6,  16,   0,   0,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    128,  63,   0,   0, 128,  63, 
      0,   0,   0,   0,  56,   0, 
      0,   7,  18,   0,  16,   0, 
      1,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,  63,  50,   0,   0,  10, 
     34,   0,  16,   0,   0,   0, 
      0,   0,  42,   0,  16, 128, 
     65,   0,   0,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,  63,   1,  64, 
      0,   0,   0,   0, 128,  63, 
     50,   0,   0,  11,  66,   0, 
     16,   0,   1,   0,   0,   0, 
     26, 128,  32,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0,  10, 128,  32,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,  69,   0,   0,   9, 
    242,   0,  16,   0,   1,   0, 
      0,   0, 134,   0,  16,   0, 
      1,   0,   0,   0,  70, 126, 
     16,   0,   1,   0,   0,   0, 
      0,  96,  16,   0,   1,   0, 
      0,   0,  14,   0,   0,   7, 
    226,   0,  16,   0,   2,   0, 
      0,   0,   6,  25,  16,   0, 
      3,   0,   0,   0, 246,  31, 
     16,   0,   3,   0,   0,   0, 
      0,   0,   0,  10,  98,   0, 
     16,   0,   0,   0,   0,   0, 
     86,   6,  16,   0,   2,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    128,  63,   0,   0, 128,  63, 
      0,   0,   0,   0,  56,   0, 
      0,   7,  18,   0,  16,   0, 
      3,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,  63,  50,   0,   0,  10, 
     66,   0,  16,   0,   3,   0, 
      0,   0,  42,   0,  16, 128, 
     65,   0,   0,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,  63,   1,  64, 
      0,   0,   0,   0, 128,  63, 
     69,   0,   0,   9, 242,   0, 
     16,   0,   3,   0,   0,   0, 
    134,   0,  16,   0,   3,   0, 
      0,   0,  70, 126,  16,   0, 
      7,   0,   0,   0,   0,  96, 
     16,   0,   7,   0,   0,   0, 
     57,   0,   0,   8,  34,   0, 
     16,   0,   0,   0,   0,   0, 
     10, 128,  32,   0,   1,   0, 
      0,   0,   5,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  50,   0,   0,  11, 
     18,   0,  16,   0,   2,   0, 
      0,   0,  10,   0,  16,   0, 
      3,   0,   0,   0,  26, 128, 
     32,   0,   1,   0,   0,   0, 
      5,   0,   0,   0,  42, 128, 
     32,   0,   1,   0,   0,   0, 
      5,   0,   0,   0,  50,   0, 
      0,  12, 194,   0,  16,   0, 
      0,   0,   0,   0,   6,  12, 
     16,   0,   2,   0,   0,   0, 
    246, 143,  32,   0,   1,   0, 
      0,   0,   6,   0,   0,   0, 
     86, 133,  32, 128,  65,   0, 
      0,   0,   1,   0,   0,   0, 
      6,   0,   0,   0,  50,   0, 
      0,  12,  50,   0,  16,   0, 
      2,   0,   0,   0, 198,   0, 
     16, 128,  65,   0,   0,   0, 
      2,   0,   0,   0, 166, 138, 
     32,   0,   1,   0,   0,   0, 
      6,   0,   0,   0,   6, 128, 
     32,   0,   1,   0,   0,   0, 
      6,   0,   0,   0,  14,   0, 
      0,   7, 194,   0,  16,   0, 
      0,   0,   0,   0, 166,  14, 
     16,   0,   0,   0,   0,   0, 
      6,   4,  16,   0,   2,   0, 
      0,   0,   0,   0,   0,   8, 
     66,   0,  16,   0,   0,   0, 
      0,   0,  42,   0,  16, 128, 
     65,   0,   0,   0,   0,   0, 
      0,   0,  58,   0,  16,   0, 
      0,   0,   0,   0,  14,  32, 
      0,   8,  66,   0,  16,   0, 
      0,   0,   0,   0,  42,   0, 
     16,   0,   0,   0,   0,   0, 
     10, 128,  32,   0,   1,   0, 
      0,   0,   5,   0,   0,   0, 
     56,   0,   0,   7,  66,   0, 
     16,   0,   0,   0,   0,   0, 
     42,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  55,   0, 
      0,   9, 130,  32,  16,   0, 
      0,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     42,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   5, 114,  32,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
     62,   0,   0,   1,  83,  84, 
     65,  84, 116,   0,   0,   0, 
     73,   0,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
     10,   0,   0,   0,  29,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   9,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   5,   0, 
      0,   0,   6,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0
};
