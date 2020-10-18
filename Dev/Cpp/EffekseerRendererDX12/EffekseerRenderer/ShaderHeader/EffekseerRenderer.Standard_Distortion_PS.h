#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//
//   fxc /Zpc /Tps_4_0 /D /Emain /Fh
//    ShaderHeader/EffekseerRenderer.Standard_Distortion_PS.h
//    Shader/standard_renderer_distortion_PS.fx
//
//
// Buffer Definitions: 
//
// cbuffer PS_ConstanBuffer
// {
//
//   float4 _263_g_scale;               // Offset:    0 Size:    16
//   float4 _263_mUVInversedBack;       // Offset:   16 Size:    16
//   float4 _263_flipbookParameter;     // Offset:   32 Size:    16
//   float4 _263_uvDistortionParameter; // Offset:   48 Size:    16
//   float4 _263_blendTextureParameter; // Offset:   64 Size:    16
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
// g_sampler                         sampler      NA          NA    0        1
// g_backSampler                     sampler      NA          NA    1        1
// g_alphaSampler                    sampler      NA          NA    2        1
// g_uvDistortionSampler             sampler      NA          NA    3        1
// g_blendSampler                    sampler      NA          NA    4        1
// g_blendAlphaSampler               sampler      NA          NA    5        1
// g_blendUVDistortionSampler        sampler      NA          NA    6        1
// g_depthSampler                    sampler      NA          NA    7        1
// g_texture                         texture  float4          2d    0        1
// g_backTexture                     texture  float4          2d    1        1
// g_alphaTexture                    texture  float4          2d    2        1
// g_uvDistortionTexture             texture  float4          2d    3        1
// g_blendTexture                    texture  float4          2d    4        1
// g_blendAlphaTexture               texture  float4          2d    5        1
// g_blendUVDistortionTexture        texture  float4          2d    6        1
// g_depthTexture                    texture  float4          2d    7        1
// PS_ConstanBuffer                  cbuffer      NA          NA    1        1
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue Format   Used
// -------------------- ----- ------ -------- -------- ------ ------
// TEXCOORD                 0   xyzw        0     NONE  float   xy w
// TEXCOORD                 1   xy          1     NONE  float   xy  
// TEXCOORD                 2   xyzw        2     NONE  float   xyzw
// TEXCOORD                 3   xyzw        3     NONE  float   xy w
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
dcl_input_ps linear centroid v0.xyw
dcl_input_ps linear centroid v1.xy
dcl_input_ps linear v2.xyzw
dcl_input_ps linear v3.xyw
dcl_input_ps linear v4.xyw
dcl_input_ps linear v5.xyzw
dcl_input_ps linear v6.xyzw
dcl_input_ps linear v7.xyzw
dcl_input_ps linear v8.xy
dcl_output o0.xyzw
dcl_temps 4
sample r0.xyzw, v5.zwzz, t3.xyzw, s3
mad r0.xy, r0.xyxx, l(2.000000, 2.000000, 0.000000, 0.000000), l(-1.000000, -1.000000, 0.000000, 0.000000)
mad r0.z, -r0.y, cb1[3].w, cb1[3].z
mad r0.yw, r0.xxxz, cb1[3].xxxx, v1.xxxy
sample r1.xyzw, r0.ywyy, t0.xyzw, s0
mul r1.z, r1.w, v0.w
lt r0.y, l(0.000000), cb1[2].x
if_nz r0.y
  mad r0.yw, r0.xxxz, cb1[3].xxxx, v7.zzzw
  sample r2.xyzw, r0.ywyy, t0.xyzw, s0
  eq r0.y, cb1[2].y, l(1.000000)
  mad r2.xyz, r2.xywx, v0.xywx, -r1.xyzx
  mad r2.xyz, v8.xxxx, r2.xyzx, r1.xyzx
  movc r1.xyz, r0.yyyy, r2.xyzx, r1.xyzx
endif 
div r2.yzw, v2.xxyz, v2.wwww
add r0.yw, r2.yyyz, l(0.000000, 1.000000, 0.000000, 1.000000)
mul r3.x, r0.y, l(0.500000)
mad r3.z, -r0.w, l(0.500000), l(1.000000)
sample r3.xyzw, r3.xzxx, t7.xyzw, s7
ne r0.y, cb1[5].x, l(0.000000)
mad r2.x, r3.x, cb1[5].y, cb1[5].z
mad r2.yz, r2.xxwx, cb1[6].wwww, -cb1[6].yyyy
mad r2.xw, -r2.xxxw, cb1[6].zzzz, cb1[6].xxxx
div r2.xy, r2.yzyy, r2.xwxx
add r0.w, -r2.x, r2.y
div_sat r0.w, r0.w, cb1[5].x
mul r0.w, r0.w, r1.z
movc r0.y, r0.y, r0.w, r1.z
mad r0.xz, r0.xxzx, cb1[3].xxxx, v5.xxyx
sample r2.xyzw, r0.xzxx, t2.xyzw, s2
mul r0.x, r2.w, r2.x
mul r0.x, r0.x, r0.y
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
div r1.xy, v2.xyxx, v2.wwww
div r1.zw, v3.xxxy, v3.wwww
div r2.xy, v4.xyxx, v4.wwww
mad r0.yz, r0.yyzy, l(0.000000, 2.000000, 2.000000, 0.000000), l(0.000000, -1.000000, -1.000000, 0.000000)
mul r0.yz, r0.yyzy, v0.xxyx
mul r0.yz, r0.yyzy, cb1[0].xxxx
add r2.xy, -r1.xyxx, r2.xyxx
mad r0.yw, r2.xxxy, r0.yyyy, r1.xxxy
add r1.xy, -r1.xyxx, r1.zwzz
mad r0.yz, r1.xxyx, r0.zzzz, r0.yywy
add r0.yz, r0.yyzy, l(0.000000, 1.000000, 1.000000, 0.000000)
mul r1.x, r0.y, l(0.500000)
mad r0.y, -r0.z, l(0.500000), l(1.000000)
mad r1.z, cb1[1].y, r0.y, cb1[1].x
sample r1.xyzw, r1.xzxx, t1.xyzw, s1
mov o0.xyz, r1.xyzx
mov o0.w, r0.x
ret 
// Approximately 74 instruction slots used
#endif

const BYTE g_main[] =
{
     68,  88,  66,  67,  42,  45, 
     97, 151, 102,   7, 198, 116, 
    129, 178, 151, 243, 184,   6, 
     47, 207,   1,   0,   0,   0, 
    136,  18,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
     88,   5,   0,   0, 112,   6, 
      0,   0, 164,   6,   0,   0, 
     12,  18,   0,   0,  82,  68, 
     69,  70,  28,   5,   0,   0, 
      1,   0,   0,   0, 100,   3, 
      0,   0,  17,   0,   0,   0, 
     28,   0,   0,   0,   0,   4, 
    255, 255,  16,   1,   0,   0, 
    233,   4,   0,   0,  60,   2, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
     70,   2,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,  84,   2,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,  99,   2, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
    121,   2,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   4,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0, 136,   2,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   5,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0, 156,   2, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      6,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
    183,   2,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   7,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0, 198,   2,   0,   0, 
      2,   0,   0,   0,   5,   0, 
      0,   0,   4,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,   1,   0,   0,   0, 
     13,   0,   0,   0, 208,   2, 
      0,   0,   2,   0,   0,   0, 
      5,   0,   0,   0,   4,   0, 
      0,   0, 255, 255, 255, 255, 
      1,   0,   0,   0,   1,   0, 
      0,   0,  13,   0,   0,   0, 
    222,   2,   0,   0,   2,   0, 
      0,   0,   5,   0,   0,   0, 
      4,   0,   0,   0, 255, 255, 
    255, 255,   2,   0,   0,   0, 
      1,   0,   0,   0,  13,   0, 
      0,   0, 237,   2,   0,   0, 
      2,   0,   0,   0,   5,   0, 
      0,   0,   4,   0,   0,   0, 
    255, 255, 255, 255,   3,   0, 
      0,   0,   1,   0,   0,   0, 
     13,   0,   0,   0,   3,   3, 
      0,   0,   2,   0,   0,   0, 
      5,   0,   0,   0,   4,   0, 
      0,   0, 255, 255, 255, 255, 
      4,   0,   0,   0,   1,   0, 
      0,   0,  13,   0,   0,   0, 
     18,   3,   0,   0,   2,   0, 
      0,   0,   5,   0,   0,   0, 
      4,   0,   0,   0, 255, 255, 
    255, 255,   5,   0,   0,   0, 
      1,   0,   0,   0,  13,   0, 
      0,   0,  38,   3,   0,   0, 
      2,   0,   0,   0,   5,   0, 
      0,   0,   4,   0,   0,   0, 
    255, 255, 255, 255,   6,   0, 
      0,   0,   1,   0,   0,   0, 
     13,   0,   0,   0,  65,   3, 
      0,   0,   2,   0,   0,   0, 
      5,   0,   0,   0,   4,   0, 
      0,   0, 255, 255, 255, 255, 
      7,   0,   0,   0,   1,   0, 
      0,   0,  13,   0,   0,   0, 
     80,   3,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0, 103,  95, 115,  97, 
    109, 112, 108, 101, 114,   0, 
    103,  95,  98,  97,  99, 107, 
     83,  97, 109, 112, 108, 101, 
    114,   0, 103,  95,  97, 108, 
    112, 104,  97,  83,  97, 109, 
    112, 108, 101, 114,   0, 103, 
     95, 117, 118,  68, 105, 115, 
    116, 111, 114, 116, 105, 111, 
    110,  83,  97, 109, 112, 108, 
    101, 114,   0, 103,  95,  98, 
    108, 101, 110, 100,  83,  97, 
    109, 112, 108, 101, 114,   0, 
    103,  95,  98, 108, 101, 110, 
    100,  65, 108, 112, 104,  97, 
     83,  97, 109, 112, 108, 101, 
    114,   0, 103,  95,  98, 108, 
    101, 110, 100,  85,  86,  68, 
    105, 115, 116, 111, 114, 116, 
    105, 111, 110,  83,  97, 109, 
    112, 108, 101, 114,   0, 103, 
     95, 100, 101, 112, 116, 104, 
     83,  97, 109, 112, 108, 101, 
    114,   0, 103,  95, 116, 101, 
    120, 116, 117, 114, 101,   0, 
    103,  95,  98,  97,  99, 107, 
     84, 101, 120, 116, 117, 114, 
    101,   0, 103,  95,  97, 108, 
    112, 104,  97,  84, 101, 120, 
    116, 117, 114, 101,   0, 103, 
     95, 117, 118,  68, 105, 115, 
    116, 111, 114, 116, 105, 111, 
    110,  84, 101, 120, 116, 117, 
    114, 101,   0, 103,  95,  98, 
    108, 101, 110, 100,  84, 101, 
    120, 116, 117, 114, 101,   0, 
    103,  95,  98, 108, 101, 110, 
    100,  65, 108, 112, 104,  97, 
     84, 101, 120, 116, 117, 114, 
    101,   0, 103,  95,  98, 108, 
    101, 110, 100,  85,  86,  68, 
    105, 115, 116, 111, 114, 116, 
    105, 111, 110,  84, 101, 120, 
    116, 117, 114, 101,   0, 103, 
     95, 100, 101, 112, 116, 104, 
     84, 101, 120, 116, 117, 114, 
    101,   0,  80,  83,  95,  67, 
    111, 110, 115, 116,  97, 110, 
     66, 117, 102, 102, 101, 114, 
      0, 171, 171, 171,  80,   3, 
      0,   0,   7,   0,   0,   0, 
    124,   3,   0,   0, 112,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  36,   4, 
      0,   0,   0,   0,   0,   0, 
     16,   0,   0,   0,   2,   0, 
      0,   0,  52,   4,   0,   0, 
      0,   0,   0,   0,  68,   4, 
      0,   0,  16,   0,   0,   0, 
     16,   0,   0,   0,   2,   0, 
      0,   0,  52,   4,   0,   0, 
      0,   0,   0,   0,  89,   4, 
      0,   0,  32,   0,   0,   0, 
     16,   0,   0,   0,   2,   0, 
      0,   0,  52,   4,   0,   0, 
      0,   0,   0,   0, 112,   4, 
      0,   0,  48,   0,   0,   0, 
     16,   0,   0,   0,   2,   0, 
      0,   0,  52,   4,   0,   0, 
      0,   0,   0,   0, 139,   4, 
      0,   0,  64,   0,   0,   0, 
     16,   0,   0,   0,   2,   0, 
      0,   0,  52,   4,   0,   0, 
      0,   0,   0,   0, 166,   4, 
      0,   0,  80,   0,   0,   0, 
     16,   0,   0,   0,   2,   0, 
      0,   0,  52,   4,   0,   0, 
      0,   0,   0,   0, 207,   4, 
      0,   0,  96,   0,   0,   0, 
     16,   0,   0,   0,   2,   0, 
      0,   0,  52,   4,   0,   0, 
      0,   0,   0,   0,  95,  50, 
     54,  51,  95, 103,  95, 115, 
     99,  97, 108, 101,   0, 171, 
    171, 171,   1,   0,   3,   0, 
      1,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     95,  50,  54,  51,  95, 109, 
     85,  86,  73, 110, 118, 101, 
    114, 115, 101, 100,  66,  97, 
     99, 107,   0,  95,  50,  54, 
     51,  95, 102, 108, 105, 112, 
     98, 111, 111, 107,  80,  97, 
    114,  97, 109, 101, 116, 101, 
    114,   0,  95,  50,  54,  51, 
     95, 117, 118,  68, 105, 115, 
    116, 111, 114, 116, 105, 111, 
    110,  80,  97, 114,  97, 109, 
    101, 116, 101, 114,   0,  95, 
     50,  54,  51,  95,  98, 108, 
    101, 110, 100,  84, 101, 120, 
    116, 117, 114, 101,  80,  97, 
    114,  97, 109, 101, 116, 101, 
    114,   0,  95,  50,  54,  51, 
     95, 115, 111, 102, 116,  80, 
     97, 114, 116, 105,  99, 108, 
    101,  65, 110, 100,  82, 101, 
     99, 111, 110, 115, 116, 114, 
    117,  99, 116, 105, 111, 110, 
     80,  97, 114,  97, 109,  49, 
      0,  95,  50,  54,  51,  95, 
    114, 101,  99, 111, 110, 115, 
    116, 114, 117,  99, 116, 105, 
    111, 110,  80,  97, 114,  97, 
    109,  50,   0,  77, 105,  99, 
    114, 111, 115, 111, 102, 116, 
     32,  40,  82,  41,  32,  72, 
     76,  83,  76,  32,  83, 104, 
     97, 100, 101, 114,  32,  67, 
    111, 109, 112, 105, 108, 101, 
    114,  32,  57,  46,  50,  57, 
     46,  57,  53,  50,  46,  51, 
     49,  49,  49,   0, 171, 171, 
     73,  83,  71,  78,  16,   1, 
      0,   0,  10,   0,   0,   0, 
      8,   0,   0,   0, 248,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     15,  11,   0,   0, 248,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
      3,   3,   0,   0, 248,   0, 
      0,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   2,   0,   0,   0, 
     15,  15,   0,   0, 248,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   3,   0,   0,   0, 
     15,  11,   0,   0, 248,   0, 
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
     96,  11,   0,   0,  64,   0, 
      0,   0, 216,   2,   0,   0, 
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
     98,  24,   0,   3, 178,  16, 
     16,   0,   0,   0,   0,   0, 
     98,  24,   0,   3,  50,  16, 
     16,   0,   1,   0,   0,   0, 
     98,  16,   0,   3, 242,  16, 
     16,   0,   2,   0,   0,   0, 
     98,  16,   0,   3, 178,  16, 
     16,   0,   3,   0,   0,   0, 
     98,  16,   0,   3, 178,  16, 
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
     16,   0,   1,   0,   0,   0, 
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
      0,   0,   0,   0,  49,   0, 
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
      0,   0,   0,   0,  70,   2, 
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
      0,   1,  14,   0,   0,   7, 
    226,   0,  16,   0,   2,   0, 
      0,   0,   6,  25,  16,   0, 
      2,   0,   0,   0, 246,  31, 
     16,   0,   2,   0,   0,   0, 
      0,   0,   0,  10, 162,   0, 
     16,   0,   0,   0,   0,   0, 
     86,   9,  16,   0,   2,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    128,  63,   0,   0,   0,   0, 
      0,   0, 128,  63,  56,   0, 
      0,   7,  18,   0,  16,   0, 
      3,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,  63,  50,   0,   0,  10, 
     66,   0,  16,   0,   3,   0, 
      0,   0,  58,   0,  16, 128, 
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
      0,  12,  98,   0,  16,   0, 
      2,   0,   0,   0,   6,   3, 
     16,   0,   2,   0,   0,   0, 
    246, 143,  32,   0,   1,   0, 
      0,   0,   6,   0,   0,   0, 
     86, 133,  32, 128,  65,   0, 
      0,   0,   1,   0,   0,   0, 
      6,   0,   0,   0,  50,   0, 
      0,  12, 146,   0,  16,   0, 
      2,   0,   0,   0,   6,  12, 
     16, 128,  65,   0,   0,   0, 
      2,   0,   0,   0, 166, 138, 
     32,   0,   1,   0,   0,   0, 
      6,   0,   0,   0,   6, 128, 
     32,   0,   1,   0,   0,   0, 
      6,   0,   0,   0,  14,   0, 
      0,   7,  50,   0,  16,   0, 
      2,   0,   0,   0, 150,   5, 
     16,   0,   2,   0,   0,   0, 
    198,   0,  16,   0,   2,   0, 
      0,   0,   0,   0,   0,   8, 
    130,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16, 128, 
     65,   0,   0,   0,   2,   0, 
      0,   0,  26,   0,  16,   0, 
      2,   0,   0,   0,  14,  32, 
      0,   8, 130,   0,  16,   0, 
      0,   0,   0,   0,  58,   0, 
     16,   0,   0,   0,   0,   0, 
     10, 128,  32,   0,   1,   0, 
      0,   0,   5,   0,   0,   0, 
     56,   0,   0,   7, 130,   0, 
     16,   0,   0,   0,   0,   0, 
     58,   0,  16,   0,   0,   0, 
      0,   0,  42,   0,  16,   0, 
      1,   0,   0,   0,  55,   0, 
      0,   9,  34,   0,  16,   0, 
      0,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     58,   0,  16,   0,   0,   0, 
      0,   0,  42,   0,  16,   0, 
      1,   0,   0,   0,  50,   0, 
      0,  10,  82,   0,  16,   0, 
      0,   0,   0,   0,   6,   2, 
     16,   0,   0,   0,   0,   0, 
      6, 128,  32,   0,   1,   0, 
      0,   0,   3,   0,   0,   0, 
      6,  17,  16,   0,   5,   0, 
      0,   0,  69,   0,   0,   9, 
    242,   0,  16,   0,   2,   0, 
      0,   0, 134,   0,  16,   0, 
      0,   0,   0,   0,  70, 126, 
     16,   0,   2,   0,   0,   0, 
      0,  96,  16,   0,   2,   0, 
      0,   0,  56,   0,   0,   7, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  58,   0,  16,   0, 
      2,   0,   0,   0,  10,   0, 
     16,   0,   2,   0,   0,   0, 
     56,   0,   0,   7,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,  52,   0, 
      0,   7,  34,   0,  16,   0, 
      0,   0,   0,   0,  26,  16, 
     16,   0,   8,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  29,   0,   0,   7, 
     34,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
     13,   0,   4,   3,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     69,   0,   0,   9, 242,   0, 
     16,   0,   2,   0,   0,   0, 
    230,  26,  16,   0,   6,   0, 
      0,   0,  70, 126,  16,   0, 
      6,   0,   0,   0,   0,  96, 
     16,   0,   6,   0,   0,   0, 
     50,   0,   0,  15,  50,   0, 
     16,   0,   2,   0,   0,   0, 
     70,   0,  16,   0,   2,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,  64,   0,   0, 
      0,  64,   0,   0,   0,   0, 
      0,   0,   0,   0,   2,  64, 
      0,   0,   0,   0, 128, 191, 
      0,   0, 128, 191,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     50,   0,   0,  12,  66,   0, 
     16,   0,   2,   0,   0,   0, 
     26,   0,  16, 128,  65,   0, 
      0,   0,   2,   0,   0,   0, 
     58, 128,  32,   0,   1,   0, 
      0,   0,   3,   0,   0,   0, 
     42, 128,  32,   0,   1,   0, 
      0,   0,   3,   0,   0,   0, 
     50,   0,   0,  10,  98,   0, 
     16,   0,   0,   0,   0,   0, 
      6,   2,  16,   0,   2,   0, 
      0,   0,  86, 133,  32,   0, 
      1,   0,   0,   0,   3,   0, 
      0,   0,   6,  17,  16,   0, 
      7,   0,   0,   0,  69,   0, 
      0,   9, 242,   0,  16,   0, 
      3,   0,   0,   0, 150,   5, 
     16,   0,   0,   0,   0,   0, 
     70, 126,  16,   0,   4,   0, 
      0,   0,   0,  96,  16,   0, 
      4,   0,   0,   0,  50,   0, 
      0,  10,  98,   0,  16,   0, 
      0,   0,   0,   0,   6,   2, 
     16,   0,   2,   0,   0,   0, 
     86, 133,  32,   0,   1,   0, 
      0,   0,   3,   0,   0,   0, 
      6,  17,  16,   0,   6,   0, 
      0,   0,  69,   0,   0,   9, 
    242,   0,  16,   0,   2,   0, 
      0,   0, 150,   5,  16,   0, 
      0,   0,   0,   0,  70, 126, 
     16,   0,   5,   0,   0,   0, 
      0,  96,  16,   0,   5,   0, 
      0,   0,  56,   0,   0,   7, 
     34,   0,  16,   0,   0,   0, 
      0,   0,  58,   0,  16,   0, 
      2,   0,   0,   0,  10,   0, 
     16,   0,   2,   0,   0,   0, 
     56,   0,   0,   7,  66,   0, 
     16,   0,   0,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0,  58,   0,  16,   0, 
      3,   0,   0,   0,  56,   0, 
      0,   7, 194,   0,  16,   0, 
      1,   0,   0,   0, 166,  10, 
     16,   0,   0,   0,   0,   0, 
      6,   4,  16,   0,   3,   0, 
      0,   0,  50,   0,   0,  10, 
     34,   0,  16,   0,   0,   0, 
      0,   0,  58,   0,  16, 128, 
     65,   0,   0,   0,   3,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128,  63, 
     50,   0,   0,   9, 162,   0, 
     16,   0,   0,   0,   0,   0, 
      6,   4,  16,   0,   1,   0, 
      0,   0,  86,   5,  16,   0, 
      0,   0,   0,   0, 166,  14, 
     16,   0,   1,   0,   0,   0, 
     50,   0,   0,   9,  50,   0, 
     16,   0,   2,   0,   0,   0, 
     70,   0,  16,   0,   3,   0, 
      0,   0, 166,  10,  16,   0, 
      0,   0,   0,   0,  70,   0, 
     16,   0,   1,   0,   0,   0, 
     50,   0,   0,  10, 194,   0, 
     16,   0,   2,   0,   0,   0, 
      6,   4,  16, 128,  65,   0, 
      0,   0,   3,   0,   0,   0, 
    166,  10,  16,   0,   0,   0, 
      0,   0,   6,   4,  16,   0, 
      1,   0,   0,   0,  24,   0, 
      0,  11, 242,   0,  16,   0, 
      3,   0,   0,   0,   6, 128, 
     32,   0,   1,   0,   0,   0, 
      4,   0,   0,   0,   2,  64, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 128,  63,   0,   0, 
      0,  64,   0,   0,  64,  64, 
     56,   0,   0,   7, 194,   0, 
     16,   0,   1,   0,   0,   0, 
    166,  14,  16,   0,   1,   0, 
      0,   0,   6,   4,  16,   0, 
      1,   0,   0,   0,  55,   0, 
      0,   9,  50,   0,  16,   0, 
      1,   0,   0,   0, 246,  15, 
     16,   0,   3,   0,   0,   0, 
    230,  10,  16,   0,   1,   0, 
      0,   0,  70,   0,  16,   0, 
      1,   0,   0,   0,  55,   0, 
      0,   9,  50,   0,  16,   0, 
      1,   0,   0,   0, 166,  10, 
     16,   0,   3,   0,   0,   0, 
    230,  10,  16,   0,   2,   0, 
      0,   0,  70,   0,  16,   0, 
      1,   0,   0,   0,  55,   0, 
      0,   9,  50,   0,  16,   0, 
      1,   0,   0,   0,  86,   5, 
     16,   0,   3,   0,   0,   0, 
     70,   0,  16,   0,   2,   0, 
      0,   0,  70,   0,  16,   0, 
      1,   0,   0,   0,  55,   0, 
      0,   9,  98,   0,  16,   0, 
      0,   0,   0,   0,   6,   0, 
     16,   0,   3,   0,   0,   0, 
     86,   7,  16,   0,   0,   0, 
      0,   0,   6,   1,  16,   0, 
      1,   0,   0,   0,  14,   0, 
      0,   7,  50,   0,  16,   0, 
      1,   0,   0,   0,  70,  16, 
     16,   0,   2,   0,   0,   0, 
    246,  31,  16,   0,   2,   0, 
      0,   0,  14,   0,   0,   7, 
    194,   0,  16,   0,   1,   0, 
      0,   0,   6,  20,  16,   0, 
      3,   0,   0,   0, 246,  31, 
     16,   0,   3,   0,   0,   0, 
     14,   0,   0,   7,  50,   0, 
     16,   0,   2,   0,   0,   0, 
     70,  16,  16,   0,   4,   0, 
      0,   0, 246,  31,  16,   0, 
      4,   0,   0,   0,  50,   0, 
      0,  15,  98,   0,  16,   0, 
      0,   0,   0,   0,  86,   6, 
     16,   0,   0,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,  64, 
      0,   0,   0,  64,   0,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    128, 191,   0,   0, 128, 191, 
      0,   0,   0,   0,  56,   0, 
      0,   7,  98,   0,  16,   0, 
      0,   0,   0,   0,  86,   6, 
     16,   0,   0,   0,   0,   0, 
      6,  17,  16,   0,   0,   0, 
      0,   0,  56,   0,   0,   8, 
     98,   0,  16,   0,   0,   0, 
      0,   0,  86,   6,  16,   0, 
      0,   0,   0,   0,   6, 128, 
     32,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   8,  50,   0,  16,   0, 
      2,   0,   0,   0,  70,   0, 
     16, 128,  65,   0,   0,   0, 
      1,   0,   0,   0,  70,   0, 
     16,   0,   2,   0,   0,   0, 
     50,   0,   0,   9, 162,   0, 
     16,   0,   0,   0,   0,   0, 
      6,   4,  16,   0,   2,   0, 
      0,   0,  86,   5,  16,   0, 
      0,   0,   0,   0,   6,   4, 
     16,   0,   1,   0,   0,   0, 
      0,   0,   0,   8,  50,   0, 
     16,   0,   1,   0,   0,   0, 
     70,   0,  16, 128,  65,   0, 
      0,   0,   1,   0,   0,   0, 
    230,  10,  16,   0,   1,   0, 
      0,   0,  50,   0,   0,   9, 
     98,   0,  16,   0,   0,   0, 
      0,   0,   6,   1,  16,   0, 
      1,   0,   0,   0, 166,  10, 
     16,   0,   0,   0,   0,   0, 
     86,   7,  16,   0,   0,   0, 
      0,   0,   0,   0,   0,  10, 
     98,   0,  16,   0,   0,   0, 
      0,   0,  86,   6,  16,   0, 
      0,   0,   0,   0,   2,  64, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 128,  63,   0,   0, 
    128,  63,   0,   0,   0,   0, 
     56,   0,   0,   7,  18,   0, 
     16,   0,   1,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,  63,  50,   0, 
      0,  10,  34,   0,  16,   0, 
      0,   0,   0,   0,  42,   0, 
     16, 128,  65,   0,   0,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,  63, 
      1,  64,   0,   0,   0,   0, 
    128,  63,  50,   0,   0,  11, 
     66,   0,  16,   0,   1,   0, 
      0,   0,  26, 128,  32,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,  10, 128, 
     32,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,  69,   0, 
      0,   9, 242,   0,  16,   0, 
      1,   0,   0,   0, 134,   0, 
     16,   0,   1,   0,   0,   0, 
     70, 126,  16,   0,   1,   0, 
      0,   0,   0,  96,  16,   0, 
      1,   0,   0,   0,  54,   0, 
      0,   5, 114,  32,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
     54,   0,   0,   5, 130,  32, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  62,   0,   0,   1, 
     83,  84,  65,  84, 116,   0, 
      0,   0,  74,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,  10,   0,   0,   0, 
     29,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   9,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      6,   0,   0,   0,   6,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0
};
