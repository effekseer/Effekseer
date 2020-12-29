#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//
//   fxc /Zpc /Tvs_4_0 /Emain /Fh ShaderHeader/ad_sprite_unlit_vs.h
//    Shader/ad_sprite_unlit_vs.fx
//
//
// Buffer Definitions: 
//
// cbuffer VS_ConstantBuffer
// {
//
//   float4x4 mCamera;                  // Offset:    0 Size:    64
//   float4x4 mProj;                    // Offset:   64 Size:    64
//   float4 mUVInversed;                // Offset:  128 Size:    16
//   float4 mflipbookParameter;         // Offset:  144 Size:    16
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim Slot Elements
// ------------------------------ ---------- ------- ----------- ---- --------
// VS_ConstantBuffer                 cbuffer      NA          NA    0        1
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue Format   Used
// -------------------- ----- ------ -------- -------- ------ ------
// POSITION                 0   xyz         0     NONE  float   xyz 
// NORMAL                   0   xyzw        1     NONE  float   xyzw
// TEXCOORD                 0   xy          2     NONE  float   xy  
// TEXCOORD                 1   xyzw        3     NONE  float   xyzw
// TEXCOORD                 2   xy          4     NONE  float   xy  
// TEXCOORD                 3   xyzw        5     NONE  float   xyzw
// TEXCOORD                 4   x           6     NONE  float   x   
// TEXCOORD                 5   x           7     NONE  float   x   
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue Format   Used
// -------------------- ----- ------ -------- -------- ------ ------
// SV_POSITION              0   xyzw        0      POS  float   xyzw
// COLOR                    0   xyzw        1     NONE  float   xyzw
// TEXCOORD                 0   xyzw        2     NONE  float   xyzw
// TEXCOORD                 1   xyz         3     NONE  float   xyz 
// TEXCOORD                 4   xyzw        4     NONE  float   xyzw
// TEXCOORD                 5   xyzw        5     NONE  float   xyzw
// TEXCOORD                 6   xyzw        6     NONE  float   xyzw
// TEXCOORD                 7   xyzw        7     NONE  float   xyzw
//
vs_4_0
dcl_constantbuffer cb0[10], immediateIndexed
dcl_input v0.xyz
dcl_input v1.xyzw
dcl_input v2.xy
dcl_input v3.xyzw
dcl_input v4.xy
dcl_input v5.xyzw
dcl_input v6.x
dcl_input v7.x
dcl_output_siv o0.xyzw, position
dcl_output o1.xyzw
dcl_output o2.xyzw
dcl_output o3.xyz
dcl_output o4.xyzw
dcl_output o5.xyzw
dcl_output o6.xyzw
dcl_output o7.xyzw
dcl_temps 8
mul r0.xyzw, cb0[0].yyyy, cb0[5].xyzw
mad r0.xyzw, cb0[4].xyzw, cb0[0].xxxx, r0.xyzw
mad r0.xyzw, cb0[6].xyzw, cb0[0].zzzz, r0.xyzw
mad r0.xyzw, cb0[7].xyzw, cb0[0].wwww, r0.xyzw
mul r1.xyzw, cb0[1].yyyy, cb0[5].xyzw
mad r1.xyzw, cb0[4].xyzw, cb0[1].xxxx, r1.xyzw
mad r1.xyzw, cb0[6].xyzw, cb0[1].zzzz, r1.xyzw
mad r1.xyzw, cb0[7].xyzw, cb0[1].wwww, r1.xyzw
mul r2.xyzw, cb0[2].yyyy, cb0[5].xyzw
mad r2.xyzw, cb0[4].xyzw, cb0[2].xxxx, r2.xyzw
mad r2.xyzw, cb0[6].xyzw, cb0[2].zzzz, r2.xyzw
mad r2.xyzw, cb0[7].xyzw, cb0[2].wwww, r2.xyzw
mul r3.xyzw, cb0[3].yyyy, cb0[5].xyzw
mad r3.xyzw, cb0[4].xyzw, cb0[3].xxxx, r3.xyzw
mad r3.xyzw, cb0[6].xyzw, cb0[3].zzzz, r3.xyzw
mad r3.xyzw, cb0[7].xyzw, cb0[3].wwww, r3.xyzw
mul r1.xyzw, r1.xyzw, v0.yyyy
mad r0.xyzw, r0.xyzw, v0.xxxx, r1.xyzw
mad r0.xyzw, r2.xyzw, v0.zzzz, r0.xyzw
add r0.xyzw, r3.xyzw, r0.xyzw
mad r1.z, cb0[8].y, v2.y, cb0[8].x
mad o4.yw, cb0[8].yyyy, v3.yyyw, cb0[8].xxxx
mad r2.x, cb0[8].y, v4.y, cb0[8].x
mad o5.yw, cb0[8].yyyy, v5.yyyw, cb0[8].xxxx
lt r3.x, l(0.000000), cb0[9].x
if_nz r3.x
  frc r3.x, v6.x
  round_ni r4.x, v6.x
  add r3.y, r4.x, l(1.000000)
  mul r3.z, cb0[9].w, cb0[9].z
  eq r3.w, cb0[9].y, l(0.000000)
  if_nz r3.w
    ge r3.w, r3.y, r3.z
    mad r4.w, cb0[9].z, cb0[9].w, l(-1.000000)
    add r4.y, r4.x, l(1.000000)
    movc r4.yw, r3.wwww, r4.wwww, r4.xxxy
  else 
    eq r3.w, cb0[9].y, l(1.000000)
    if_nz r3.w
      mul r3.w, r3.z, r4.x
      ge r3.w, r3.w, -r3.w
      movc r3.w, r3.w, r3.z, -r3.z
      div r5.x, l(1.000000, 1.000000, 1.000000, 1.000000), r3.w
      mul r5.x, r4.x, r5.x
      frc r5.x, r5.x
      mul r4.y, r3.w, r5.x
      mul r3.w, r3.z, r3.y
      ge r3.w, r3.w, -r3.w
      movc r3.w, r3.w, r3.z, -r3.z
      div r5.x, l(1.000000, 1.000000, 1.000000, 1.000000), r3.w
      mul r5.x, r3.y, r5.x
      frc r5.x, r5.x
      mul r4.w, r3.w, r5.x
    else 
      eq r3.w, cb0[9].y, l(2.000000)
      div r5.x, r4.x, r3.z
      round_ni r5.x, r5.x
      add r5.y, r5.x, r5.x
      ge r5.y, r5.y, -r5.y
      movc r5.yz, r5.yyyy, l(0,2.000000,0.500000,0), l(0,-2.000000,-0.500000,0)
      mul r5.x, r5.z, r5.x
      frc r5.x, r5.x
      mul r5.x, r5.x, r5.y
      eq r5.x, r5.x, l(1.000000)
      mul r5.y, r3.z, r4.x
      ge r5.y, r5.y, -r5.y
      movc r5.y, r5.y, r3.z, -r3.z
      div r5.z, l(1.000000, 1.000000, 1.000000, 1.000000), r5.y
      mul r5.z, r4.x, r5.z
      frc r5.z, r5.z
      mul r5.y, r5.z, r5.y
      mad r5.z, cb0[9].z, cb0[9].w, l(-1.000000)
      round_ni r5.w, r5.y
      add r5.w, -r5.w, r5.z
      movc r5.x, r5.x, r5.w, r5.y
      div r5.w, r3.y, r3.z
      round_ni r5.w, r5.w
      add r6.x, r5.w, r5.w
      ge r6.x, r6.x, -r6.x
      movc r6.xy, r6.xxxx, l(2.000000,0.500000,0,0), l(-2.000000,-0.500000,0,0)
      mul r5.w, r5.w, r6.y
      frc r5.w, r5.w
      mul r5.w, r5.w, r6.x
      eq r5.w, r5.w, l(1.000000)
      mul r6.x, r3.z, r3.y
      ge r6.x, r6.x, -r6.x
      movc r3.z, r6.x, r3.z, -r3.z
      div r6.x, l(1.000000, 1.000000, 1.000000, 1.000000), r3.z
      mul r3.y, r3.y, r6.x
      frc r3.y, r3.y
      mul r3.y, r3.y, r3.z
      round_ni r3.z, r3.y
      add r3.z, -r3.z, r5.z
      movc r5.y, r5.w, r3.z, r3.y
      add r4.z, r4.x, l(1.000000)
      movc r4.yw, r3.wwww, r5.xxxy, r4.xxxz
    endif 
  endif 
  ftoi r3.yz, r4.yywy
  ftoi r3.w, cb0[9].z
  and r4.xy, r3.yzyy, l(0x80000000, 0x80000000, 0, 0)
  imax r4.zw, r3.yyyz, -r3.yyyz
  imax r5.x, r3.w, -r3.w
  udiv r6.x, r7.x, r4.z, r5.x
  ineg r4.z, r7.x
  movc r4.x, r4.x, r4.z, r7.x
  itof r7.x, r4.x
  xor r3.yz, r3.wwww, r3.yyzy
  ineg r3.w, r6.x
  and r3.yz, r3.yyzy, l(0, 0x80000000, 0x80000000, 0)
  movc r3.y, r3.y, r3.w, r6.x
  itof r7.y, r3.y
  div r3.yw, l(1.000000, 1.000000, 1.000000, 1.000000), cb0[9].zzzw
  mov r1.x, v2.x
  mad r4.xz, -r7.xxyx, r3.yywy, r1.xxzx
  mul r4.xz, r4.xxzx, cb0[9].zzwz
  udiv r5.x, r6.x, r4.w, r5.x
  ineg r4.w, r6.x
  movc r4.y, r4.y, r4.w, r6.x
  itof r6.x, r4.y
  ineg r4.y, r5.x
  movc r3.z, r3.z, r4.y, r5.x
  itof r6.y, r3.z
  mul r4.yw, r3.yyyw, r6.xxxy
  mad r2.yz, r4.xxzx, r3.yywy, r4.yywy
  mov r1.x, r3.x
else 
  mov r2.yz, l(0,0,0,0)
  mov r1.x, l(0)
endif 
mov o0.xyzw, r0.xyzw
mov o1.xyzw, v1.xyzw
mov r1.y, v2.x
mov r1.w, v7.x
mov o2.xyzw, r1.yzxw
mov o4.xz, v3.xxzx
mov o5.xz, v5.xxzx
mov r2.w, v4.x
mov o6.xyzw, r2.wxyz
mov o7.xyzw, r0.xyzw
mov o3.xyz, l(0,0,0,0)
ret 
// Approximately 142 instruction slots used
#endif

const BYTE g_main[] =
{
     68,  88,  66,  67, 158, 219, 
     77,  16,  33, 157,  71, 195, 
    251, 201, 142, 123,  51,  96, 
     99, 211,   1,   0,   0,   0, 
     40,  21,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
    132,   1,   0,   0, 112,   2, 
      0,   0,  92,   3,   0,   0, 
    172,  20,   0,   0,  82,  68, 
     69,  70,  72,   1,   0,   0, 
      1,   0,   0,   0,  80,   0, 
      0,   0,   1,   0,   0,   0, 
     28,   0,   0,   0,   0,   4, 
    254, 255,  16,   1,   0,   0, 
     23,   1,   0,   0,  60,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
     86,  83,  95,  67, 111, 110, 
    115, 116,  97, 110, 116,  66, 
    117, 102, 102, 101, 114,   0, 
    171, 171,  60,   0,   0,   0, 
      4,   0,   0,   0, 104,   0, 
      0,   0, 160,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 200,   0,   0,   0, 
      0,   0,   0,   0,  64,   0, 
      0,   0,   2,   0,   0,   0, 
    208,   0,   0,   0,   0,   0, 
      0,   0, 224,   0,   0,   0, 
     64,   0,   0,   0,  64,   0, 
      0,   0,   2,   0,   0,   0, 
    208,   0,   0,   0,   0,   0, 
      0,   0, 230,   0,   0,   0, 
    128,   0,   0,   0,  16,   0, 
      0,   0,   2,   0,   0,   0, 
    244,   0,   0,   0,   0,   0, 
      0,   0,   4,   1,   0,   0, 
    144,   0,   0,   0,  16,   0, 
      0,   0,   2,   0,   0,   0, 
    244,   0,   0,   0,   0,   0, 
      0,   0, 109,  67,  97, 109, 
    101, 114,  97,   0,   3,   0, 
      3,   0,   4,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 109,  80, 114, 111, 
    106,   0, 109,  85,  86,  73, 
    110, 118, 101, 114, 115, 101, 
    100,   0, 171, 171,   1,   0, 
      3,   0,   1,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 109, 102, 108, 105, 
    112,  98, 111, 111, 107,  80, 
     97, 114,  97, 109, 101, 116, 
    101, 114,   0,  77, 105,  99, 
    114, 111, 115, 111, 102, 116, 
     32,  40,  82,  41,  32,  72, 
     76,  83,  76,  32,  83, 104, 
     97, 100, 101, 114,  32,  67, 
    111, 109, 112, 105, 108, 101, 
    114,  32,  57,  46,  50,  57, 
     46,  57,  53,  50,  46,  51, 
     49,  49,  49,   0,  73,  83, 
     71,  78, 228,   0,   0,   0, 
      8,   0,   0,   0,   8,   0, 
      0,   0, 200,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   7,   7, 
      0,   0, 209,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      1,   0,   0,   0,  15,  15, 
      0,   0, 216,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      2,   0,   0,   0,   3,   3, 
      0,   0, 216,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      3,   0,   0,   0,  15,  15, 
      0,   0, 216,   0,   0,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      4,   0,   0,   0,   3,   3, 
      0,   0, 216,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      5,   0,   0,   0,  15,  15, 
      0,   0, 216,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      6,   0,   0,   0,   1,   1, 
      0,   0, 216,   0,   0,   0, 
      5,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      7,   0,   0,   0,   1,   1, 
      0,   0,  80,  79,  83,  73, 
     84,  73,  79,  78,   0,  78, 
     79,  82,  77,  65,  76,   0, 
     84,  69,  88,  67,  79,  79, 
     82,  68,   0, 171, 171, 171, 
     79,  83,  71,  78, 228,   0, 
      0,   0,   8,   0,   0,   0, 
      8,   0,   0,   0, 200,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     15,   0,   0,   0, 212,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
     15,   0,   0,   0, 218,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   2,   0,   0,   0, 
     15,   0,   0,   0, 218,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   3,   0,   0,   0, 
      7,   8,   0,   0, 218,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   4,   0,   0,   0, 
     15,   0,   0,   0, 218,   0, 
      0,   0,   5,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   5,   0,   0,   0, 
     15,   0,   0,   0, 218,   0, 
      0,   0,   6,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   6,   0,   0,   0, 
     15,   0,   0,   0, 218,   0, 
      0,   0,   7,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   7,   0,   0,   0, 
     15,   0,   0,   0,  83,  86, 
     95,  80,  79,  83,  73,  84, 
     73,  79,  78,   0,  67,  79, 
     76,  79,  82,   0,  84,  69, 
     88,  67,  79,  79,  82,  68, 
      0, 171,  83,  72,  68,  82, 
     72,  17,   0,   0,  64,   0, 
      1,   0,  82,   4,   0,   0, 
     89,   0,   0,   4,  70, 142, 
     32,   0,   0,   0,   0,   0, 
     10,   0,   0,   0,  95,   0, 
      0,   3, 114,  16,  16,   0, 
      0,   0,   0,   0,  95,   0, 
      0,   3, 242,  16,  16,   0, 
      1,   0,   0,   0,  95,   0, 
      0,   3,  50,  16,  16,   0, 
      2,   0,   0,   0,  95,   0, 
      0,   3, 242,  16,  16,   0, 
      3,   0,   0,   0,  95,   0, 
      0,   3,  50,  16,  16,   0, 
      4,   0,   0,   0,  95,   0, 
      0,   3, 242,  16,  16,   0, 
      5,   0,   0,   0,  95,   0, 
      0,   3,  18,  16,  16,   0, 
      6,   0,   0,   0,  95,   0, 
      0,   3,  18,  16,  16,   0, 
      7,   0,   0,   0, 103,   0, 
      0,   4, 242,  32,  16,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   1,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   2,   0, 
      0,   0, 101,   0,   0,   3, 
    114,  32,  16,   0,   3,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   4,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   5,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   6,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   7,   0, 
      0,   0, 104,   0,   0,   2, 
      8,   0,   0,   0,  56,   0, 
      0,   9, 242,   0,  16,   0, 
      0,   0,   0,   0,  86, 133, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      5,   0,   0,   0,  50,   0, 
      0,  11, 242,   0,  16,   0, 
      0,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      4,   0,   0,   0,   6, 128, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   0,   0,   0,   0, 
     50,   0,   0,  11, 242,   0, 
     16,   0,   0,   0,   0,   0, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   6,   0,   0,   0, 
    166, 138,  32,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   0,   0, 
      0,   0,  50,   0,   0,  11, 
    242,   0,  16,   0,   0,   0, 
      0,   0,  70, 142,  32,   0, 
      0,   0,   0,   0,   7,   0, 
      0,   0, 246, 143,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      0,   0,   0,   0,  56,   0, 
      0,   9, 242,   0,  16,   0, 
      1,   0,   0,   0,  86, 133, 
     32,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      5,   0,   0,   0,  50,   0, 
      0,  11, 242,   0,  16,   0, 
      1,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      4,   0,   0,   0,   6, 128, 
     32,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,  70,  14, 
     16,   0,   1,   0,   0,   0, 
     50,   0,   0,  11, 242,   0, 
     16,   0,   1,   0,   0,   0, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   6,   0,   0,   0, 
    166, 138,  32,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
     70,  14,  16,   0,   1,   0, 
      0,   0,  50,   0,   0,  11, 
    242,   0,  16,   0,   1,   0, 
      0,   0,  70, 142,  32,   0, 
      0,   0,   0,   0,   7,   0, 
      0,   0, 246, 143,  32,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,  70,  14,  16,   0, 
      1,   0,   0,   0,  56,   0, 
      0,   9, 242,   0,  16,   0, 
      2,   0,   0,   0,  86, 133, 
     32,   0,   0,   0,   0,   0, 
      2,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      5,   0,   0,   0,  50,   0, 
      0,  11, 242,   0,  16,   0, 
      2,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      4,   0,   0,   0,   6, 128, 
     32,   0,   0,   0,   0,   0, 
      2,   0,   0,   0,  70,  14, 
     16,   0,   2,   0,   0,   0, 
     50,   0,   0,  11, 242,   0, 
     16,   0,   2,   0,   0,   0, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   6,   0,   0,   0, 
    166, 138,  32,   0,   0,   0, 
      0,   0,   2,   0,   0,   0, 
     70,  14,  16,   0,   2,   0, 
      0,   0,  50,   0,   0,  11, 
    242,   0,  16,   0,   2,   0, 
      0,   0,  70, 142,  32,   0, 
      0,   0,   0,   0,   7,   0, 
      0,   0, 246, 143,  32,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,  70,  14,  16,   0, 
      2,   0,   0,   0,  56,   0, 
      0,   9, 242,   0,  16,   0, 
      3,   0,   0,   0,  86, 133, 
     32,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      5,   0,   0,   0,  50,   0, 
      0,  11, 242,   0,  16,   0, 
      3,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      4,   0,   0,   0,   6, 128, 
     32,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,  70,  14, 
     16,   0,   3,   0,   0,   0, 
     50,   0,   0,  11, 242,   0, 
     16,   0,   3,   0,   0,   0, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   6,   0,   0,   0, 
    166, 138,  32,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
     70,  14,  16,   0,   3,   0, 
      0,   0,  50,   0,   0,  11, 
    242,   0,  16,   0,   3,   0, 
      0,   0,  70, 142,  32,   0, 
      0,   0,   0,   0,   7,   0, 
      0,   0, 246, 143,  32,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,  70,  14,  16,   0, 
      3,   0,   0,   0,  56,   0, 
      0,   7, 242,   0,  16,   0, 
      1,   0,   0,   0,  70,  14, 
     16,   0,   1,   0,   0,   0, 
     86,  21,  16,   0,   0,   0, 
      0,   0,  50,   0,   0,   9, 
    242,   0,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      0,   0,   0,   0,   6,  16, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   1,   0, 
      0,   0,  50,   0,   0,   9, 
    242,   0,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      2,   0,   0,   0, 166,  26, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   0,   0, 
      0,   0,   0,   0,   0,   7, 
    242,   0,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      3,   0,   0,   0,  70,  14, 
     16,   0,   0,   0,   0,   0, 
     50,   0,   0,  11,  66,   0, 
     16,   0,   1,   0,   0,   0, 
     26, 128,  32,   0,   0,   0, 
      0,   0,   8,   0,   0,   0, 
     26,  16,  16,   0,   2,   0, 
      0,   0,  10, 128,  32,   0, 
      0,   0,   0,   0,   8,   0, 
      0,   0,  50,   0,   0,  11, 
    162,  32,  16,   0,   4,   0, 
      0,   0,  86, 133,  32,   0, 
      0,   0,   0,   0,   8,   0, 
      0,   0,  86,  29,  16,   0, 
      3,   0,   0,   0,   6, 128, 
     32,   0,   0,   0,   0,   0, 
      8,   0,   0,   0,  50,   0, 
      0,  11,  18,   0,  16,   0, 
      2,   0,   0,   0,  26, 128, 
     32,   0,   0,   0,   0,   0, 
      8,   0,   0,   0,  26,  16, 
     16,   0,   4,   0,   0,   0, 
     10, 128,  32,   0,   0,   0, 
      0,   0,   8,   0,   0,   0, 
     50,   0,   0,  11, 162,  32, 
     16,   0,   5,   0,   0,   0, 
     86, 133,  32,   0,   0,   0, 
      0,   0,   8,   0,   0,   0, 
     86,  29,  16,   0,   5,   0, 
      0,   0,   6, 128,  32,   0, 
      0,   0,   0,   0,   8,   0, 
      0,   0,  49,   0,   0,   8, 
     18,   0,  16,   0,   3,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,  10, 128, 
     32,   0,   0,   0,   0,   0, 
      9,   0,   0,   0,  31,   0, 
      4,   3,  10,   0,  16,   0, 
      3,   0,   0,   0,  26,   0, 
      0,   5,  18,   0,  16,   0, 
      3,   0,   0,   0,  10,  16, 
     16,   0,   6,   0,   0,   0, 
     65,   0,   0,   5,  18,   0, 
     16,   0,   4,   0,   0,   0, 
     10,  16,  16,   0,   6,   0, 
      0,   0,   0,   0,   0,   7, 
     34,   0,  16,   0,   3,   0, 
      0,   0,  10,   0,  16,   0, 
      4,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128,  63, 
     56,   0,   0,   9,  66,   0, 
     16,   0,   3,   0,   0,   0, 
     58, 128,  32,   0,   0,   0, 
      0,   0,   9,   0,   0,   0, 
     42, 128,  32,   0,   0,   0, 
      0,   0,   9,   0,   0,   0, 
     24,   0,   0,   8, 130,   0, 
     16,   0,   3,   0,   0,   0, 
     26, 128,  32,   0,   0,   0, 
      0,   0,   9,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  31,   0,   4,   3, 
     58,   0,  16,   0,   3,   0, 
      0,   0,  29,   0,   0,   7, 
    130,   0,  16,   0,   3,   0, 
      0,   0,  26,   0,  16,   0, 
      3,   0,   0,   0,  42,   0, 
     16,   0,   3,   0,   0,   0, 
     50,   0,   0,  11, 130,   0, 
     16,   0,   4,   0,   0,   0, 
     42, 128,  32,   0,   0,   0, 
      0,   0,   9,   0,   0,   0, 
     58, 128,  32,   0,   0,   0, 
      0,   0,   9,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
    128, 191,   0,   0,   0,   7, 
     34,   0,  16,   0,   4,   0, 
      0,   0,  10,   0,  16,   0, 
      4,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128,  63, 
     55,   0,   0,   9, 162,   0, 
     16,   0,   4,   0,   0,   0, 
    246,  15,  16,   0,   3,   0, 
      0,   0, 246,  15,  16,   0, 
      4,   0,   0,   0,   6,   4, 
     16,   0,   4,   0,   0,   0, 
     18,   0,   0,   1,  24,   0, 
      0,   8, 130,   0,  16,   0, 
      3,   0,   0,   0,  26, 128, 
     32,   0,   0,   0,   0,   0, 
      9,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128,  63, 
     31,   0,   4,   3,  58,   0, 
     16,   0,   3,   0,   0,   0, 
     56,   0,   0,   7, 130,   0, 
     16,   0,   3,   0,   0,   0, 
     42,   0,  16,   0,   3,   0, 
      0,   0,  10,   0,  16,   0, 
      4,   0,   0,   0,  29,   0, 
      0,   8, 130,   0,  16,   0, 
      3,   0,   0,   0,  58,   0, 
     16,   0,   3,   0,   0,   0, 
     58,   0,  16, 128,  65,   0, 
      0,   0,   3,   0,   0,   0, 
     55,   0,   0,  10, 130,   0, 
     16,   0,   3,   0,   0,   0, 
     58,   0,  16,   0,   3,   0, 
      0,   0,  42,   0,  16,   0, 
      3,   0,   0,   0,  42,   0, 
     16, 128,  65,   0,   0,   0, 
      3,   0,   0,   0,  14,   0, 
      0,  10,  18,   0,  16,   0, 
      5,   0,   0,   0,   2,  64, 
      0,   0,   0,   0, 128,  63, 
      0,   0, 128,  63,   0,   0, 
    128,  63,   0,   0, 128,  63, 
     58,   0,  16,   0,   3,   0, 
      0,   0,  56,   0,   0,   7, 
     18,   0,  16,   0,   5,   0, 
      0,   0,  10,   0,  16,   0, 
      4,   0,   0,   0,  10,   0, 
     16,   0,   5,   0,   0,   0, 
     26,   0,   0,   5,  18,   0, 
     16,   0,   5,   0,   0,   0, 
     10,   0,  16,   0,   5,   0, 
      0,   0,  56,   0,   0,   7, 
     34,   0,  16,   0,   4,   0, 
      0,   0,  58,   0,  16,   0, 
      3,   0,   0,   0,  10,   0, 
     16,   0,   5,   0,   0,   0, 
     56,   0,   0,   7, 130,   0, 
     16,   0,   3,   0,   0,   0, 
     42,   0,  16,   0,   3,   0, 
      0,   0,  26,   0,  16,   0, 
      3,   0,   0,   0,  29,   0, 
      0,   8, 130,   0,  16,   0, 
      3,   0,   0,   0,  58,   0, 
     16,   0,   3,   0,   0,   0, 
     58,   0,  16, 128,  65,   0, 
      0,   0,   3,   0,   0,   0, 
     55,   0,   0,  10, 130,   0, 
     16,   0,   3,   0,   0,   0, 
     58,   0,  16,   0,   3,   0, 
      0,   0,  42,   0,  16,   0, 
      3,   0,   0,   0,  42,   0, 
     16, 128,  65,   0,   0,   0, 
      3,   0,   0,   0,  14,   0, 
      0,  10,  18,   0,  16,   0, 
      5,   0,   0,   0,   2,  64, 
      0,   0,   0,   0, 128,  63, 
      0,   0, 128,  63,   0,   0, 
    128,  63,   0,   0, 128,  63, 
     58,   0,  16,   0,   3,   0, 
      0,   0,  56,   0,   0,   7, 
     18,   0,  16,   0,   5,   0, 
      0,   0,  26,   0,  16,   0, 
      3,   0,   0,   0,  10,   0, 
     16,   0,   5,   0,   0,   0, 
     26,   0,   0,   5,  18,   0, 
     16,   0,   5,   0,   0,   0, 
     10,   0,  16,   0,   5,   0, 
      0,   0,  56,   0,   0,   7, 
    130,   0,  16,   0,   4,   0, 
      0,   0,  58,   0,  16,   0, 
      3,   0,   0,   0,  10,   0, 
     16,   0,   5,   0,   0,   0, 
     18,   0,   0,   1,  24,   0, 
      0,   8, 130,   0,  16,   0, 
      3,   0,   0,   0,  26, 128, 
     32,   0,   0,   0,   0,   0, 
      9,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,  64, 
     14,   0,   0,   7,  18,   0, 
     16,   0,   5,   0,   0,   0, 
     10,   0,  16,   0,   4,   0, 
      0,   0,  42,   0,  16,   0, 
      3,   0,   0,   0,  65,   0, 
      0,   5,  18,   0,  16,   0, 
      5,   0,   0,   0,  10,   0, 
     16,   0,   5,   0,   0,   0, 
      0,   0,   0,   7,  34,   0, 
     16,   0,   5,   0,   0,   0, 
     10,   0,  16,   0,   5,   0, 
      0,   0,  10,   0,  16,   0, 
      5,   0,   0,   0,  29,   0, 
      0,   8,  34,   0,  16,   0, 
      5,   0,   0,   0,  26,   0, 
     16,   0,   5,   0,   0,   0, 
     26,   0,  16, 128,  65,   0, 
      0,   0,   5,   0,   0,   0, 
     55,   0,   0,  15,  98,   0, 
     16,   0,   5,   0,   0,   0, 
     86,   5,  16,   0,   5,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,  64,   0,   0,   0,  63, 
      0,   0,   0,   0,   2,  64, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0, 192,   0,   0, 
      0, 191,   0,   0,   0,   0, 
     56,   0,   0,   7,  18,   0, 
     16,   0,   5,   0,   0,   0, 
     42,   0,  16,   0,   5,   0, 
      0,   0,  10,   0,  16,   0, 
      5,   0,   0,   0,  26,   0, 
      0,   5,  18,   0,  16,   0, 
      5,   0,   0,   0,  10,   0, 
     16,   0,   5,   0,   0,   0, 
     56,   0,   0,   7,  18,   0, 
     16,   0,   5,   0,   0,   0, 
     10,   0,  16,   0,   5,   0, 
      0,   0,  26,   0,  16,   0, 
      5,   0,   0,   0,  24,   0, 
      0,   7,  18,   0,  16,   0, 
      5,   0,   0,   0,  10,   0, 
     16,   0,   5,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
    128,  63,  56,   0,   0,   7, 
     34,   0,  16,   0,   5,   0, 
      0,   0,  42,   0,  16,   0, 
      3,   0,   0,   0,  10,   0, 
     16,   0,   4,   0,   0,   0, 
     29,   0,   0,   8,  34,   0, 
     16,   0,   5,   0,   0,   0, 
     26,   0,  16,   0,   5,   0, 
      0,   0,  26,   0,  16, 128, 
     65,   0,   0,   0,   5,   0, 
      0,   0,  55,   0,   0,  10, 
     34,   0,  16,   0,   5,   0, 
      0,   0,  26,   0,  16,   0, 
      5,   0,   0,   0,  42,   0, 
     16,   0,   3,   0,   0,   0, 
     42,   0,  16, 128,  65,   0, 
      0,   0,   3,   0,   0,   0, 
     14,   0,   0,  10,  66,   0, 
     16,   0,   5,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
    128,  63,   0,   0, 128,  63, 
      0,   0, 128,  63,   0,   0, 
    128,  63,  26,   0,  16,   0, 
      5,   0,   0,   0,  56,   0, 
      0,   7,  66,   0,  16,   0, 
      5,   0,   0,   0,  10,   0, 
     16,   0,   4,   0,   0,   0, 
     42,   0,  16,   0,   5,   0, 
      0,   0,  26,   0,   0,   5, 
     66,   0,  16,   0,   5,   0, 
      0,   0,  42,   0,  16,   0, 
      5,   0,   0,   0,  56,   0, 
      0,   7,  34,   0,  16,   0, 
      5,   0,   0,   0,  42,   0, 
     16,   0,   5,   0,   0,   0, 
     26,   0,  16,   0,   5,   0, 
      0,   0,  50,   0,   0,  11, 
     66,   0,  16,   0,   5,   0, 
      0,   0,  42, 128,  32,   0, 
      0,   0,   0,   0,   9,   0, 
      0,   0,  58, 128,  32,   0, 
      0,   0,   0,   0,   9,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0, 128, 191,  65,   0, 
      0,   5, 130,   0,  16,   0, 
      5,   0,   0,   0,  26,   0, 
     16,   0,   5,   0,   0,   0, 
      0,   0,   0,   8, 130,   0, 
     16,   0,   5,   0,   0,   0, 
     58,   0,  16, 128,  65,   0, 
      0,   0,   5,   0,   0,   0, 
     42,   0,  16,   0,   5,   0, 
      0,   0,  55,   0,   0,   9, 
     18,   0,  16,   0,   5,   0, 
      0,   0,  10,   0,  16,   0, 
      5,   0,   0,   0,  58,   0, 
     16,   0,   5,   0,   0,   0, 
     26,   0,  16,   0,   5,   0, 
      0,   0,  14,   0,   0,   7, 
    130,   0,  16,   0,   5,   0, 
      0,   0,  26,   0,  16,   0, 
      3,   0,   0,   0,  42,   0, 
     16,   0,   3,   0,   0,   0, 
     65,   0,   0,   5, 130,   0, 
     16,   0,   5,   0,   0,   0, 
     58,   0,  16,   0,   5,   0, 
      0,   0,   0,   0,   0,   7, 
     18,   0,  16,   0,   6,   0, 
      0,   0,  58,   0,  16,   0, 
      5,   0,   0,   0,  58,   0, 
     16,   0,   5,   0,   0,   0, 
     29,   0,   0,   8,  18,   0, 
     16,   0,   6,   0,   0,   0, 
     10,   0,  16,   0,   6,   0, 
      0,   0,  10,   0,  16, 128, 
     65,   0,   0,   0,   6,   0, 
      0,   0,  55,   0,   0,  15, 
     50,   0,  16,   0,   6,   0, 
      0,   0,   6,   0,  16,   0, 
      6,   0,   0,   0,   2,  64, 
      0,   0,   0,   0,   0,  64, 
      0,   0,   0,  63,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0, 192,   0,   0,   0, 191, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  56,   0,   0,   7, 
    130,   0,  16,   0,   5,   0, 
      0,   0,  58,   0,  16,   0, 
      5,   0,   0,   0,  26,   0, 
     16,   0,   6,   0,   0,   0, 
     26,   0,   0,   5, 130,   0, 
     16,   0,   5,   0,   0,   0, 
     58,   0,  16,   0,   5,   0, 
      0,   0,  56,   0,   0,   7, 
    130,   0,  16,   0,   5,   0, 
      0,   0,  58,   0,  16,   0, 
      5,   0,   0,   0,  10,   0, 
     16,   0,   6,   0,   0,   0, 
     24,   0,   0,   7, 130,   0, 
     16,   0,   5,   0,   0,   0, 
     58,   0,  16,   0,   5,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0, 128,  63,  56,   0, 
      0,   7,  18,   0,  16,   0, 
      6,   0,   0,   0,  42,   0, 
     16,   0,   3,   0,   0,   0, 
     26,   0,  16,   0,   3,   0, 
      0,   0,  29,   0,   0,   8, 
     18,   0,  16,   0,   6,   0, 
      0,   0,  10,   0,  16,   0, 
      6,   0,   0,   0,  10,   0, 
     16, 128,  65,   0,   0,   0, 
      6,   0,   0,   0,  55,   0, 
      0,  10,  66,   0,  16,   0, 
      3,   0,   0,   0,  10,   0, 
     16,   0,   6,   0,   0,   0, 
     42,   0,  16,   0,   3,   0, 
      0,   0,  42,   0,  16, 128, 
     65,   0,   0,   0,   3,   0, 
      0,   0,  14,   0,   0,  10, 
     18,   0,  16,   0,   6,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0, 128,  63,   0,   0, 
    128,  63,   0,   0, 128,  63, 
      0,   0, 128,  63,  42,   0, 
     16,   0,   3,   0,   0,   0, 
     56,   0,   0,   7,  34,   0, 
     16,   0,   3,   0,   0,   0, 
     26,   0,  16,   0,   3,   0, 
      0,   0,  10,   0,  16,   0, 
      6,   0,   0,   0,  26,   0, 
      0,   5,  34,   0,  16,   0, 
      3,   0,   0,   0,  26,   0, 
     16,   0,   3,   0,   0,   0, 
     56,   0,   0,   7,  34,   0, 
     16,   0,   3,   0,   0,   0, 
     26,   0,  16,   0,   3,   0, 
      0,   0,  42,   0,  16,   0, 
      3,   0,   0,   0,  65,   0, 
      0,   5,  66,   0,  16,   0, 
      3,   0,   0,   0,  26,   0, 
     16,   0,   3,   0,   0,   0, 
      0,   0,   0,   8,  66,   0, 
     16,   0,   3,   0,   0,   0, 
     42,   0,  16, 128,  65,   0, 
      0,   0,   3,   0,   0,   0, 
     42,   0,  16,   0,   5,   0, 
      0,   0,  55,   0,   0,   9, 
     34,   0,  16,   0,   5,   0, 
      0,   0,  58,   0,  16,   0, 
      5,   0,   0,   0,  42,   0, 
     16,   0,   3,   0,   0,   0, 
     26,   0,  16,   0,   3,   0, 
      0,   0,   0,   0,   0,   7, 
     66,   0,  16,   0,   4,   0, 
      0,   0,  10,   0,  16,   0, 
      4,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128,  63, 
     55,   0,   0,   9, 162,   0, 
     16,   0,   4,   0,   0,   0, 
    246,  15,  16,   0,   3,   0, 
      0,   0,   6,   4,  16,   0, 
      5,   0,   0,   0,   6,   8, 
     16,   0,   4,   0,   0,   0, 
     21,   0,   0,   1,  21,   0, 
      0,   1,  27,   0,   0,   5, 
     98,   0,  16,   0,   3,   0, 
      0,   0,  86,   7,  16,   0, 
      4,   0,   0,   0,  27,   0, 
      0,   6, 130,   0,  16,   0, 
      3,   0,   0,   0,  42, 128, 
     32,   0,   0,   0,   0,   0, 
      9,   0,   0,   0,   1,   0, 
      0,  10,  50,   0,  16,   0, 
      4,   0,   0,   0, 150,   5, 
     16,   0,   3,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0, 128,   0,   0,   0, 128, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  36,   0,   0,   8, 
    194,   0,  16,   0,   4,   0, 
      0,   0,  86,   9,  16,   0, 
      3,   0,   0,   0,  86,   9, 
     16, 128,  65,   0,   0,   0, 
      3,   0,   0,   0,  36,   0, 
      0,   8,  18,   0,  16,   0, 
      5,   0,   0,   0,  58,   0, 
     16,   0,   3,   0,   0,   0, 
     58,   0,  16, 128,  65,   0, 
      0,   0,   3,   0,   0,   0, 
     78,   0,   0,   9,  18,   0, 
     16,   0,   6,   0,   0,   0, 
     18,   0,  16,   0,   7,   0, 
      0,   0,  42,   0,  16,   0, 
      4,   0,   0,   0,  10,   0, 
     16,   0,   5,   0,   0,   0, 
     40,   0,   0,   5,  66,   0, 
     16,   0,   4,   0,   0,   0, 
     10,   0,  16,   0,   7,   0, 
      0,   0,  55,   0,   0,   9, 
     18,   0,  16,   0,   4,   0, 
      0,   0,  10,   0,  16,   0, 
      4,   0,   0,   0,  42,   0, 
     16,   0,   4,   0,   0,   0, 
     10,   0,  16,   0,   7,   0, 
      0,   0,  43,   0,   0,   5, 
     18,   0,  16,   0,   7,   0, 
      0,   0,  10,   0,  16,   0, 
      4,   0,   0,   0,  87,   0, 
      0,   7,  98,   0,  16,   0, 
      3,   0,   0,   0, 246,  15, 
     16,   0,   3,   0,   0,   0, 
     86,   6,  16,   0,   3,   0, 
      0,   0,  40,   0,   0,   5, 
    130,   0,  16,   0,   3,   0, 
      0,   0,  10,   0,  16,   0, 
      6,   0,   0,   0,   1,   0, 
      0,  10,  98,   0,  16,   0, 
      3,   0,   0,   0,  86,   6, 
     16,   0,   3,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0,   0, 128, 
      0,   0,   0, 128,   0,   0, 
      0,   0,  55,   0,   0,   9, 
     34,   0,  16,   0,   3,   0, 
      0,   0,  26,   0,  16,   0, 
      3,   0,   0,   0,  58,   0, 
     16,   0,   3,   0,   0,   0, 
     10,   0,  16,   0,   6,   0, 
      0,   0,  43,   0,   0,   5, 
     34,   0,  16,   0,   7,   0, 
      0,   0,  26,   0,  16,   0, 
      3,   0,   0,   0,  14,   0, 
      0,  11, 162,   0,  16,   0, 
      3,   0,   0,   0,   2,  64, 
      0,   0,   0,   0, 128,  63, 
      0,   0, 128,  63,   0,   0, 
    128,  63,   0,   0, 128,  63, 
    166, 142,  32,   0,   0,   0, 
      0,   0,   9,   0,   0,   0, 
     54,   0,   0,   5,  18,   0, 
     16,   0,   1,   0,   0,   0, 
     10,  16,  16,   0,   2,   0, 
      0,   0,  50,   0,   0,  10, 
     82,   0,  16,   0,   4,   0, 
      0,   0,   6,   1,  16, 128, 
     65,   0,   0,   0,   7,   0, 
      0,   0,  86,   7,  16,   0, 
      3,   0,   0,   0,   6,   2, 
     16,   0,   1,   0,   0,   0, 
     56,   0,   0,   8,  82,   0, 
     16,   0,   4,   0,   0,   0, 
      6,   2,  16,   0,   4,   0, 
      0,   0, 166, 139,  32,   0, 
      0,   0,   0,   0,   9,   0, 
      0,   0,  78,   0,   0,   9, 
     18,   0,  16,   0,   5,   0, 
      0,   0,  18,   0,  16,   0, 
      6,   0,   0,   0,  58,   0, 
     16,   0,   4,   0,   0,   0, 
     10,   0,  16,   0,   5,   0, 
      0,   0,  40,   0,   0,   5, 
    130,   0,  16,   0,   4,   0, 
      0,   0,  10,   0,  16,   0, 
      6,   0,   0,   0,  55,   0, 
      0,   9,  34,   0,  16,   0, 
      4,   0,   0,   0,  26,   0, 
     16,   0,   4,   0,   0,   0, 
     58,   0,  16,   0,   4,   0, 
      0,   0,  10,   0,  16,   0, 
      6,   0,   0,   0,  43,   0, 
      0,   5,  18,   0,  16,   0, 
      6,   0,   0,   0,  26,   0, 
     16,   0,   4,   0,   0,   0, 
     40,   0,   0,   5,  34,   0, 
     16,   0,   4,   0,   0,   0, 
     10,   0,  16,   0,   5,   0, 
      0,   0,  55,   0,   0,   9, 
     66,   0,  16,   0,   3,   0, 
      0,   0,  42,   0,  16,   0, 
      3,   0,   0,   0,  26,   0, 
     16,   0,   4,   0,   0,   0, 
     10,   0,  16,   0,   5,   0, 
      0,   0,  43,   0,   0,   5, 
     34,   0,  16,   0,   6,   0, 
      0,   0,  42,   0,  16,   0, 
      3,   0,   0,   0,  56,   0, 
      0,   7, 162,   0,  16,   0, 
      4,   0,   0,   0,  86,  13, 
     16,   0,   3,   0,   0,   0, 
      6,   4,  16,   0,   6,   0, 
      0,   0,  50,   0,   0,   9, 
     98,   0,  16,   0,   2,   0, 
      0,   0,   6,   2,  16,   0, 
      4,   0,   0,   0,  86,   7, 
     16,   0,   3,   0,   0,   0, 
     86,   7,  16,   0,   4,   0, 
      0,   0,  54,   0,   0,   5, 
     18,   0,  16,   0,   1,   0, 
      0,   0,  10,   0,  16,   0, 
      3,   0,   0,   0,  18,   0, 
      0,   1,  54,   0,   0,   8, 
     98,   0,  16,   0,   2,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   5,  18,   0,  16,   0, 
      1,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
     21,   0,   0,   1,  54,   0, 
      0,   5, 242,  32,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   0,   0,   0,   0, 
     54,   0,   0,   5, 242,  32, 
     16,   0,   1,   0,   0,   0, 
     70,  30,  16,   0,   1,   0, 
      0,   0,  54,   0,   0,   5, 
     34,   0,  16,   0,   1,   0, 
      0,   0,  10,  16,  16,   0, 
      2,   0,   0,   0,  54,   0, 
      0,   5, 130,   0,  16,   0, 
      1,   0,   0,   0,  10,  16, 
     16,   0,   7,   0,   0,   0, 
     54,   0,   0,   5, 242,  32, 
     16,   0,   2,   0,   0,   0, 
    150,  12,  16,   0,   1,   0, 
      0,   0,  54,   0,   0,   5, 
     82,  32,  16,   0,   4,   0, 
      0,   0,   6,  18,  16,   0, 
      3,   0,   0,   0,  54,   0, 
      0,   5,  82,  32,  16,   0, 
      5,   0,   0,   0,   6,  18, 
     16,   0,   5,   0,   0,   0, 
     54,   0,   0,   5, 130,   0, 
     16,   0,   2,   0,   0,   0, 
     10,  16,  16,   0,   4,   0, 
      0,   0,  54,   0,   0,   5, 
    242,  32,  16,   0,   6,   0, 
      0,   0,  54,   9,  16,   0, 
      2,   0,   0,   0,  54,   0, 
      0,   5, 242,  32,  16,   0, 
      7,   0,   0,   0,  70,  14, 
     16,   0,   0,   0,   0,   0, 
     54,   0,   0,   8, 114,  32, 
     16,   0,   3,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  62,   0,   0,   1, 
     83,  84,  65,  84, 116,   0, 
      0,   0, 142,   0,   0,   0, 
      8,   0,   0,   0,   0,   0, 
      0,   0,  16,   0,   0,   0, 
     64,   0,   0,   0,   6,   0, 
      0,   0,   3,   0,   0,   0, 
      4,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     29,   0,   0,   0,  14,   0, 
      0,   0,  18,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0
};
