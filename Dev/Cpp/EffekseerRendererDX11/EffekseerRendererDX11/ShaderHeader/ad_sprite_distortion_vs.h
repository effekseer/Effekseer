#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//
//   fxc /Zpc /Tvs_4_0 /Emain /Fh ShaderHeader/ad_sprite_distortion_vs.h
//    Shader/ad_sprite_distortion_vs.fx
//
//
// Buffer Definitions: 
//
// cbuffer VS_ConstantBuffer
// {
//
//   float4x4 mCamera;                  // Offset:    0 Size:    64 [unused]
//   float4x4 mCameraProj;              // Offset:   64 Size:    64
//   float4 mUVInversed;                // Offset:  128 Size:    16
//   float4 flipbookParameter1;         // Offset:  144 Size:    16
//   float4 flipbookParameter2;         // Offset:  160 Size:    16
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
// NORMAL                   1   xyzw        2     NONE  float   xyz 
// NORMAL                   2   xyzw        3     NONE  float   xyz 
// TEXCOORD                 0   xy          4     NONE  float   xy  
// TEXCOORD                 1   xy          5     NONE  float       
// TEXCOORD                 2   xyzw        6     NONE  float   xyzw
// TEXCOORD                 3   xy          7     NONE  float   xy  
// TEXCOORD                 4   xyzw        8     NONE  float   xyzw
// TEXCOORD                 5   x           9     NONE  float   x   
// TEXCOORD                 6   x          10     NONE  float   x   
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue Format   Used
// -------------------- ----- ------ -------- -------- ------ ------
// SV_POSITION              0   xyzw        0      POS  float   xyzw
// TEXCOORD                 0   xyzw        1     NONE  float   xyzw
// TEXCOORD                 1   xyzw        2     NONE  float   xyzw
// TEXCOORD                 2   xyzw        3     NONE  float   xyzw
// TEXCOORD                 3   xyzw        4     NONE  float   xyzw
// COLOR                    0   xyzw        5     NONE  float   xyzw
// TEXCOORD                 4   xyzw        6     NONE  float   xyzw
// TEXCOORD                 5   xyzw        7     NONE  float   xyzw
// TEXCOORD                 6   xyzw        8     NONE  float   xyzw
//
vs_4_0
dcl_constantbuffer cb0[11], immediateIndexed
dcl_input v0.xyz
dcl_input v1.xyzw
dcl_input v2.xyz
dcl_input v3.xyz
dcl_input v4.xy
dcl_input v6.xyzw
dcl_input v7.xy
dcl_input v8.xyzw
dcl_input v9.x
dcl_input v10.x
dcl_output_siv o0.xyzw, position
dcl_output o1.xyzw
dcl_output o2.xyzw
dcl_output o3.xyzw
dcl_output o4.xyzw
dcl_output o5.xyzw
dcl_output o6.xyzw
dcl_output o7.xyzw
dcl_output o8.xyzw
dcl_temps 7
add r0.xyz, v2.zxyz, l(-0.500000, -0.500000, -0.500000, 0.000000)
add r0.xyz, r0.xyzx, r0.xyzx
add r1.xyz, v3.xyzx, l(-0.500000, -0.500000, -0.500000, 0.000000)
add r2.xyz, r1.xyzx, r1.xyzx
mul r3.xyz, r0.xyzx, r2.yzxy
mad r0.xyz, r0.zxyz, r2.zxyz, -r3.xyzx
mad r2.x, cb0[8].y, v4.y, cb0[8].x
mul r3.xyzw, v0.yyyy, cb0[5].xyzw
mad r3.xyzw, cb0[4].xyzw, v0.xxxx, r3.xyzw
mad r3.xyzw, cb0[6].xyzw, v0.zzzz, r3.xyzw
add r3.xyzw, r3.xyzw, cb0[7].xyzw
mad r1.xyz, r1.xyzx, l(2.000000, 2.000000, 2.000000, 0.000000), v0.xyzx
mul r4.xyzw, r1.yyyy, cb0[5].xyzw
mad r4.xyzw, cb0[4].xyzw, r1.xxxx, r4.xyzw
mad r1.xyzw, cb0[6].xyzw, r1.zzzz, r4.xyzw
add o3.xyzw, r1.xyzw, cb0[7].xyzw
add r0.xyz, r0.xyzx, v0.xyzx
mul r1.xyzw, r0.yyyy, cb0[5].xyzw
mad r1.xyzw, cb0[4].xyzw, r0.xxxx, r1.xyzw
mad r0.xyzw, cb0[6].xyzw, r0.zzzz, r1.xyzw
add o2.xyzw, r0.xyzw, cb0[7].xyzw
mad o6.yw, cb0[8].yyyy, v6.yyyw, cb0[8].xxxx
mad r0.x, cb0[8].y, v7.y, cb0[8].x
mad o7.yw, cb0[8].yyyy, v8.yyyw, cb0[8].xxxx
lt r1.x, l(0.000000), cb0[9].x
if_nz r1.x
  frc r2.y, v9.x
  round_ni r1.x, v9.x
  add r1.w, r1.x, l(1.000000)
  mul r4.x, cb0[9].w, cb0[9].z
  eq r4.y, cb0[9].y, l(0.000000)
  if_nz r4.y
    ge r4.y, r1.w, r4.x
    mad r4.z, cb0[9].z, cb0[9].w, l(-1.000000)
    add r1.y, r1.x, l(1.000000)
    movc r4.yz, r4.yyyy, r4.zzzz, r1.xxyx
  else 
    eq r1.y, cb0[9].y, l(1.000000)
    if_nz r1.y
      mul r1.y, r1.x, r4.x
      ge r1.y, r1.y, -r1.y
      movc r1.y, r1.y, r4.x, -r4.x
      div r4.w, l(1.000000, 1.000000, 1.000000, 1.000000), r1.y
      mul r4.w, r1.x, r4.w
      frc r4.w, r4.w
      mul r4.y, r1.y, r4.w
      mul r1.y, r1.w, r4.x
      ge r1.y, r1.y, -r1.y
      movc r1.y, r1.y, r4.x, -r4.x
      div r4.w, l(1.000000, 1.000000, 1.000000, 1.000000), r1.y
      mul r4.w, r1.w, r4.w
      frc r4.w, r4.w
      mul r4.z, r1.y, r4.w
    else 
      eq r1.y, cb0[9].y, l(2.000000)
      div r4.w, r1.x, r4.x
      round_ni r4.w, r4.w
      add r5.x, r4.w, r4.w
      ge r5.x, r5.x, -r5.x
      movc r5.xy, r5.xxxx, l(2.000000,0.500000,0,0), l(-2.000000,-0.500000,0,0)
      mul r4.w, r4.w, r5.y
      frc r4.w, r4.w
      mul r4.w, r4.w, r5.x
      eq r4.w, r4.w, l(1.000000)
      mul r5.x, r1.x, r4.x
      ge r5.x, r5.x, -r5.x
      movc r5.x, r5.x, r4.x, -r4.x
      div r5.y, l(1.000000, 1.000000, 1.000000, 1.000000), r5.x
      mul r5.y, r1.x, r5.y
      frc r5.y, r5.y
      mul r5.x, r5.y, r5.x
      mad r5.y, cb0[9].z, cb0[9].w, l(-1.000000)
      round_ni r5.z, r5.x
      add r5.z, -r5.z, r5.y
      movc r6.x, r4.w, r5.z, r5.x
      div r4.w, r1.w, r4.x
      round_ni r4.w, r4.w
      add r5.x, r4.w, r4.w
      ge r5.x, r5.x, -r5.x
      movc r5.xz, r5.xxxx, l(2.000000,0,0.500000,0), l(-2.000000,0,-0.500000,0)
      mul r4.w, r4.w, r5.z
      frc r4.w, r4.w
      mul r4.w, r4.w, r5.x
      eq r4.w, r4.w, l(1.000000)
      mul r5.x, r1.w, r4.x
      ge r5.x, r5.x, -r5.x
      movc r4.x, r5.x, r4.x, -r4.x
      div r5.x, l(1.000000, 1.000000, 1.000000, 1.000000), r4.x
      mul r1.w, r1.w, r5.x
      frc r1.w, r1.w
      mul r1.w, r1.w, r4.x
      round_ni r4.x, r1.w
      add r4.x, -r4.x, r5.y
      movc r6.y, r4.w, r4.x, r1.w
      add r1.z, r1.x, l(1.000000)
      movc r4.yz, r1.yyyy, r6.xxyx, r1.xxzx
    endif 
  endif 
  mad r1.y, cb0[8].y, r2.x, cb0[8].x
  ftoi r1.zw, r4.yyyz
  ftoi r4.x, cb0[9].z
  and r4.yz, r1.zzwz, l(0, 0x80000000, 0x80000000, 0)
  imax r5.xy, r1.zwzz, -r1.zwzz
  imax r4.w, r4.x, -r4.x
  udiv r5.x, r6.x, r5.x, r4.w
  ineg r5.z, r6.x
  movc r4.y, r4.y, r5.z, r6.x
  itof r6.x, r4.y
  xor r1.zw, r1.zzzw, r4.xxxx
  ineg r4.x, r5.x
  and r1.zw, r1.zzzw, l(0, 0, 0x80000000, 0x80000000)
  movc r1.z, r1.z, r4.x, r5.x
  itof r6.y, r1.z
  mad r4.xy, r6.xyxx, cb0[10].xyxx, cb0[10].zwzz
  mov r1.x, v4.x
  add r1.xy, r1.xyxx, -r4.xyxx
  udiv r4.x, r5.x, r5.y, r4.w
  ineg r1.z, r5.x
  movc r1.z, r4.z, r1.z, r5.x
  itof r5.x, r1.z
  ineg r1.z, r4.x
  movc r1.z, r1.w, r1.z, r4.x
  itof r5.y, r1.z
  mad r1.xy, r5.xyxx, cb0[10].xyxx, r1.xyxx
  add r1.xy, r1.xyxx, cb0[10].zwzz
  mad r1.z, cb0[8].y, r1.y, cb0[8].x
  mov r0.yz, r1.xxzx
else 
  mov r0.yz, l(0,0,0,0)
  mov r2.y, l(0)
endif 
mov o0.xyzw, r3.xyzw
mov r2.z, v4.x
mov r2.w, v10.x
mov o1.xyzw, r2.zxyw
mov o4.xyzw, r3.xyzw
mov o5.xyzw, v1.xyzw
mov o6.xz, v6.xxzx
mov o7.xz, v8.xxzx
mov r0.w, v7.x
mov o8.xyzw, r0.wxyz
ret 
// Approximately 142 instruction slots used
#endif

const BYTE g_main[] =
{
     68,  88,  66,  67,  76,  20, 
    171, 187, 123,  82,  94, 140, 
    246, 112, 142,  46, 222, 170, 
     63, 210,   1,   0,   0,   0, 
    156,  21,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
    180,   1,   0,   0, 232,   2, 
      0,   0, 236,   3,   0,   0, 
     32,  21,   0,   0,  82,  68, 
     69,  70, 120,   1,   0,   0, 
      1,   0,   0,   0,  80,   0, 
      0,   0,   1,   0,   0,   0, 
     28,   0,   0,   0,   0,   4, 
    254, 255,  16,   1,   0,   0, 
     70,   1,   0,   0,  60,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
     86,  83,  95,  67, 111, 110, 
    115, 116,  97, 110, 116,  66, 
    117, 102, 102, 101, 114,   0, 
    171, 171,  60,   0,   0,   0, 
      5,   0,   0,   0, 104,   0, 
      0,   0, 176,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 224,   0,   0,   0, 
      0,   0,   0,   0,  64,   0, 
      0,   0,   0,   0,   0,   0, 
    232,   0,   0,   0,   0,   0, 
      0,   0, 248,   0,   0,   0, 
     64,   0,   0,   0,  64,   0, 
      0,   0,   2,   0,   0,   0, 
    232,   0,   0,   0,   0,   0, 
      0,   0,   4,   1,   0,   0, 
    128,   0,   0,   0,  16,   0, 
      0,   0,   2,   0,   0,   0, 
     16,   1,   0,   0,   0,   0, 
      0,   0,  32,   1,   0,   0, 
    144,   0,   0,   0,  16,   0, 
      0,   0,   2,   0,   0,   0, 
     16,   1,   0,   0,   0,   0, 
      0,   0,  51,   1,   0,   0, 
    160,   0,   0,   0,  16,   0, 
      0,   0,   2,   0,   0,   0, 
     16,   1,   0,   0,   0,   0, 
      0,   0, 109,  67,  97, 109, 
    101, 114,  97,   0,   3,   0, 
      3,   0,   4,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 109,  67,  97, 109, 
    101, 114,  97,  80, 114, 111, 
    106,   0, 109,  85,  86,  73, 
    110, 118, 101, 114, 115, 101, 
    100,   0,   1,   0,   3,   0, 
      1,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    102, 108, 105, 112,  98, 111, 
    111, 107,  80,  97, 114,  97, 
    109, 101, 116, 101, 114,  49, 
      0, 102, 108, 105, 112,  98, 
    111, 111, 107,  80,  97, 114, 
     97, 109, 101, 116, 101, 114, 
     50,   0,  77, 105,  99, 114, 
    111, 115, 111, 102, 116,  32, 
     40,  82,  41,  32,  72,  76, 
     83,  76,  32,  83, 104,  97, 
    100, 101, 114,  32,  67, 111, 
    109, 112, 105, 108, 101, 114, 
     32,  57,  46,  50,  57,  46, 
     57,  53,  50,  46,  51,  49, 
     49,  49,   0, 171,  73,  83, 
     71,  78,  44,   1,   0,   0, 
     11,   0,   0,   0,   8,   0, 
      0,   0,  16,   1,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   7,   7, 
      0,   0,  25,   1,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      1,   0,   0,   0,  15,  15, 
      0,   0,  25,   1,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      2,   0,   0,   0,  15,   7, 
      0,   0,  25,   1,   0,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      3,   0,   0,   0,  15,   7, 
      0,   0,  32,   1,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      4,   0,   0,   0,   3,   3, 
      0,   0,  32,   1,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      5,   0,   0,   0,   3,   0, 
      0,   0,  32,   1,   0,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      6,   0,   0,   0,  15,  15, 
      0,   0,  32,   1,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      7,   0,   0,   0,   3,   3, 
      0,   0,  32,   1,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      8,   0,   0,   0,  15,  15, 
      0,   0,  32,   1,   0,   0, 
      5,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      9,   0,   0,   0,   1,   1, 
      0,   0,  32,   1,   0,   0, 
      6,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
     10,   0,   0,   0,   1,   1, 
      0,   0,  80,  79,  83,  73, 
     84,  73,  79,  78,   0,  78, 
     79,  82,  77,  65,  76,   0, 
     84,  69,  88,  67,  79,  79, 
     82,  68,   0, 171, 171, 171, 
     79,  83,  71,  78, 252,   0, 
      0,   0,   9,   0,   0,   0, 
      8,   0,   0,   0, 224,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     15,   0,   0,   0, 236,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
     15,   0,   0,   0, 236,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   2,   0,   0,   0, 
     15,   0,   0,   0, 236,   0, 
      0,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   3,   0,   0,   0, 
     15,   0,   0,   0, 236,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   4,   0,   0,   0, 
     15,   0,   0,   0, 245,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   5,   0,   0,   0, 
     15,   0,   0,   0, 236,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   6,   0,   0,   0, 
     15,   0,   0,   0, 236,   0, 
      0,   0,   5,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   7,   0,   0,   0, 
     15,   0,   0,   0, 236,   0, 
      0,   0,   6,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   8,   0,   0,   0, 
     15,   0,   0,   0,  83,  86, 
     95,  80,  79,  83,  73,  84, 
     73,  79,  78,   0,  84,  69, 
     88,  67,  79,  79,  82,  68, 
      0,  67,  79,  76,  79,  82, 
      0, 171,  83,  72,  68,  82, 
     44,  17,   0,   0,  64,   0, 
      1,   0,  75,   4,   0,   0, 
     89,   0,   0,   4,  70, 142, 
     32,   0,   0,   0,   0,   0, 
     11,   0,   0,   0,  95,   0, 
      0,   3, 114,  16,  16,   0, 
      0,   0,   0,   0,  95,   0, 
      0,   3, 242,  16,  16,   0, 
      1,   0,   0,   0,  95,   0, 
      0,   3, 114,  16,  16,   0, 
      2,   0,   0,   0,  95,   0, 
      0,   3, 114,  16,  16,   0, 
      3,   0,   0,   0,  95,   0, 
      0,   3,  50,  16,  16,   0, 
      4,   0,   0,   0,  95,   0, 
      0,   3, 242,  16,  16,   0, 
      6,   0,   0,   0,  95,   0, 
      0,   3,  50,  16,  16,   0, 
      7,   0,   0,   0,  95,   0, 
      0,   3, 242,  16,  16,   0, 
      8,   0,   0,   0,  95,   0, 
      0,   3,  18,  16,  16,   0, 
      9,   0,   0,   0,  95,   0, 
      0,   3,  18,  16,  16,   0, 
     10,   0,   0,   0, 103,   0, 
      0,   4, 242,  32,  16,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   1,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   2,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   3,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   4,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   5,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   6,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   7,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   8,   0, 
      0,   0, 104,   0,   0,   2, 
      7,   0,   0,   0,   0,   0, 
      0,  10, 114,   0,  16,   0, 
      0,   0,   0,   0,  38,  25, 
     16,   0,   2,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0, 191,   0,   0,   0, 191, 
      0,   0,   0, 191,   0,   0, 
      0,   0,   0,   0,   0,   7, 
    114,   0,  16,   0,   0,   0, 
      0,   0,  70,   2,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   0,   0,   0,   0, 
      0,   0,   0,  10, 114,   0, 
     16,   0,   1,   0,   0,   0, 
     70,  18,  16,   0,   3,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0, 191,   0,   0, 
      0, 191,   0,   0,   0, 191, 
      0,   0,   0,   0,   0,   0, 
      0,   7, 114,   0,  16,   0, 
      2,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0,  56,   0,   0,   7, 
    114,   0,  16,   0,   3,   0, 
      0,   0,  70,   2,  16,   0, 
      0,   0,   0,   0, 150,   4, 
     16,   0,   2,   0,   0,   0, 
     50,   0,   0,  10, 114,   0, 
     16,   0,   0,   0,   0,   0, 
     38,   9,  16,   0,   0,   0, 
      0,   0,  38,   9,  16,   0, 
      2,   0,   0,   0,  70,   2, 
     16, 128,  65,   0,   0,   0, 
      3,   0,   0,   0,  50,   0, 
      0,  11,  18,   0,  16,   0, 
      2,   0,   0,   0,  26, 128, 
     32,   0,   0,   0,   0,   0, 
      8,   0,   0,   0,  26,  16, 
     16,   0,   4,   0,   0,   0, 
     10, 128,  32,   0,   0,   0, 
      0,   0,   8,   0,   0,   0, 
     56,   0,   0,   8, 242,   0, 
     16,   0,   3,   0,   0,   0, 
     86,  21,  16,   0,   0,   0, 
      0,   0,  70, 142,  32,   0, 
      0,   0,   0,   0,   5,   0, 
      0,   0,  50,   0,   0,  10, 
    242,   0,  16,   0,   3,   0, 
      0,   0,  70, 142,  32,   0, 
      0,   0,   0,   0,   4,   0, 
      0,   0,   6,  16,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   3,   0,   0,   0, 
     50,   0,   0,  10, 242,   0, 
     16,   0,   3,   0,   0,   0, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   6,   0,   0,   0, 
    166,  26,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   8, 242,   0,  16,   0, 
      3,   0,   0,   0,  70,  14, 
     16,   0,   3,   0,   0,   0, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   7,   0,   0,   0, 
     50,   0,   0,  12, 114,   0, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,  64,   0,   0, 
      0,  64,   0,   0,   0,  64, 
      0,   0,   0,   0,  70,  18, 
     16,   0,   0,   0,   0,   0, 
     56,   0,   0,   8, 242,   0, 
     16,   0,   4,   0,   0,   0, 
     86,   5,  16,   0,   1,   0, 
      0,   0,  70, 142,  32,   0, 
      0,   0,   0,   0,   5,   0, 
      0,   0,  50,   0,   0,  10, 
    242,   0,  16,   0,   4,   0, 
      0,   0,  70, 142,  32,   0, 
      0,   0,   0,   0,   4,   0, 
      0,   0,   6,   0,  16,   0, 
      1,   0,   0,   0,  70,  14, 
     16,   0,   4,   0,   0,   0, 
     50,   0,   0,  10, 242,   0, 
     16,   0,   1,   0,   0,   0, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   6,   0,   0,   0, 
    166,  10,  16,   0,   1,   0, 
      0,   0,  70,  14,  16,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   8, 242,  32,  16,   0, 
      3,   0,   0,   0,  70,  14, 
     16,   0,   1,   0,   0,   0, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   7,   0,   0,   0, 
      0,   0,   0,   7, 114,   0, 
     16,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   0,   0, 
      0,   0,  70,  18,  16,   0, 
      0,   0,   0,   0,  56,   0, 
      0,   8, 242,   0,  16,   0, 
      1,   0,   0,   0,  86,   5, 
     16,   0,   0,   0,   0,   0, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   5,   0,   0,   0, 
     50,   0,   0,  10, 242,   0, 
     16,   0,   1,   0,   0,   0, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   4,   0,   0,   0, 
      6,   0,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      1,   0,   0,   0,  50,   0, 
      0,  10, 242,   0,  16,   0, 
      0,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      6,   0,   0,   0, 166,  10, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   1,   0, 
      0,   0,   0,   0,   0,   8, 
    242,  32,  16,   0,   2,   0, 
      0,   0,  70,  14,  16,   0, 
      0,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      7,   0,   0,   0,  50,   0, 
      0,  11, 162,  32,  16,   0, 
      6,   0,   0,   0,  86, 133, 
     32,   0,   0,   0,   0,   0, 
      8,   0,   0,   0,  86,  29, 
     16,   0,   6,   0,   0,   0, 
      6, 128,  32,   0,   0,   0, 
      0,   0,   8,   0,   0,   0, 
     50,   0,   0,  11,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     26, 128,  32,   0,   0,   0, 
      0,   0,   8,   0,   0,   0, 
     26,  16,  16,   0,   7,   0, 
      0,   0,  10, 128,  32,   0, 
      0,   0,   0,   0,   8,   0, 
      0,   0,  50,   0,   0,  11, 
    162,  32,  16,   0,   7,   0, 
      0,   0,  86, 133,  32,   0, 
      0,   0,   0,   0,   8,   0, 
      0,   0,  86,  29,  16,   0, 
      8,   0,   0,   0,   6, 128, 
     32,   0,   0,   0,   0,   0, 
      8,   0,   0,   0,  49,   0, 
      0,   8,  18,   0,  16,   0, 
      1,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
     10, 128,  32,   0,   0,   0, 
      0,   0,   9,   0,   0,   0, 
     31,   0,   4,   3,  10,   0, 
     16,   0,   1,   0,   0,   0, 
     26,   0,   0,   5,  34,   0, 
     16,   0,   2,   0,   0,   0, 
     10,  16,  16,   0,   9,   0, 
      0,   0,  65,   0,   0,   5, 
     18,   0,  16,   0,   1,   0, 
      0,   0,  10,  16,  16,   0, 
      9,   0,   0,   0,   0,   0, 
      0,   7, 130,   0,  16,   0, 
      1,   0,   0,   0,  10,   0, 
     16,   0,   1,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
    128,  63,  56,   0,   0,   9, 
     18,   0,  16,   0,   4,   0, 
      0,   0,  58, 128,  32,   0, 
      0,   0,   0,   0,   9,   0, 
      0,   0,  42, 128,  32,   0, 
      0,   0,   0,   0,   9,   0, 
      0,   0,  24,   0,   0,   8, 
     34,   0,  16,   0,   4,   0, 
      0,   0,  26, 128,  32,   0, 
      0,   0,   0,   0,   9,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,  31,   0, 
      4,   3,  26,   0,  16,   0, 
      4,   0,   0,   0,  29,   0, 
      0,   7,  34,   0,  16,   0, 
      4,   0,   0,   0,  58,   0, 
     16,   0,   1,   0,   0,   0, 
     10,   0,  16,   0,   4,   0, 
      0,   0,  50,   0,   0,  11, 
     66,   0,  16,   0,   4,   0, 
      0,   0,  42, 128,  32,   0, 
      0,   0,   0,   0,   9,   0, 
      0,   0,  58, 128,  32,   0, 
      0,   0,   0,   0,   9,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0, 128, 191,   0,   0, 
      0,   7,  34,   0,  16,   0, 
      1,   0,   0,   0,  10,   0, 
     16,   0,   1,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
    128,  63,  55,   0,   0,   9, 
     98,   0,  16,   0,   4,   0, 
      0,   0,  86,   5,  16,   0, 
      4,   0,   0,   0, 166,  10, 
     16,   0,   4,   0,   0,   0, 
      6,   1,  16,   0,   1,   0, 
      0,   0,  18,   0,   0,   1, 
     24,   0,   0,   8,  34,   0, 
     16,   0,   1,   0,   0,   0, 
     26, 128,  32,   0,   0,   0, 
      0,   0,   9,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
    128,  63,  31,   0,   4,   3, 
     26,   0,  16,   0,   1,   0, 
      0,   0,  56,   0,   0,   7, 
     34,   0,  16,   0,   1,   0, 
      0,   0,  10,   0,  16,   0, 
      1,   0,   0,   0,  10,   0, 
     16,   0,   4,   0,   0,   0, 
     29,   0,   0,   8,  34,   0, 
     16,   0,   1,   0,   0,   0, 
     26,   0,  16,   0,   1,   0, 
      0,   0,  26,   0,  16, 128, 
     65,   0,   0,   0,   1,   0, 
      0,   0,  55,   0,   0,  10, 
     34,   0,  16,   0,   1,   0, 
      0,   0,  26,   0,  16,   0, 
      1,   0,   0,   0,  10,   0, 
     16,   0,   4,   0,   0,   0, 
     10,   0,  16, 128,  65,   0, 
      0,   0,   4,   0,   0,   0, 
     14,   0,   0,  10, 130,   0, 
     16,   0,   4,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
    128,  63,   0,   0, 128,  63, 
      0,   0, 128,  63,   0,   0, 
    128,  63,  26,   0,  16,   0, 
      1,   0,   0,   0,  56,   0, 
      0,   7, 130,   0,  16,   0, 
      4,   0,   0,   0,  10,   0, 
     16,   0,   1,   0,   0,   0, 
     58,   0,  16,   0,   4,   0, 
      0,   0,  26,   0,   0,   5, 
    130,   0,  16,   0,   4,   0, 
      0,   0,  58,   0,  16,   0, 
      4,   0,   0,   0,  56,   0, 
      0,   7,  34,   0,  16,   0, 
      4,   0,   0,   0,  26,   0, 
     16,   0,   1,   0,   0,   0, 
     58,   0,  16,   0,   4,   0, 
      0,   0,  56,   0,   0,   7, 
     34,   0,  16,   0,   1,   0, 
      0,   0,  58,   0,  16,   0, 
      1,   0,   0,   0,  10,   0, 
     16,   0,   4,   0,   0,   0, 
     29,   0,   0,   8,  34,   0, 
     16,   0,   1,   0,   0,   0, 
     26,   0,  16,   0,   1,   0, 
      0,   0,  26,   0,  16, 128, 
     65,   0,   0,   0,   1,   0, 
      0,   0,  55,   0,   0,  10, 
     34,   0,  16,   0,   1,   0, 
      0,   0,  26,   0,  16,   0, 
      1,   0,   0,   0,  10,   0, 
     16,   0,   4,   0,   0,   0, 
     10,   0,  16, 128,  65,   0, 
      0,   0,   4,   0,   0,   0, 
     14,   0,   0,  10, 130,   0, 
     16,   0,   4,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
    128,  63,   0,   0, 128,  63, 
      0,   0, 128,  63,   0,   0, 
    128,  63,  26,   0,  16,   0, 
      1,   0,   0,   0,  56,   0, 
      0,   7, 130,   0,  16,   0, 
      4,   0,   0,   0,  58,   0, 
     16,   0,   1,   0,   0,   0, 
     58,   0,  16,   0,   4,   0, 
      0,   0,  26,   0,   0,   5, 
    130,   0,  16,   0,   4,   0, 
      0,   0,  58,   0,  16,   0, 
      4,   0,   0,   0,  56,   0, 
      0,   7,  66,   0,  16,   0, 
      4,   0,   0,   0,  26,   0, 
     16,   0,   1,   0,   0,   0, 
     58,   0,  16,   0,   4,   0, 
      0,   0,  18,   0,   0,   1, 
     24,   0,   0,   8,  34,   0, 
     16,   0,   1,   0,   0,   0, 
     26, 128,  32,   0,   0,   0, 
      0,   0,   9,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,  64,  14,   0,   0,   7, 
    130,   0,  16,   0,   4,   0, 
      0,   0,  10,   0,  16,   0, 
      1,   0,   0,   0,  10,   0, 
     16,   0,   4,   0,   0,   0, 
     65,   0,   0,   5, 130,   0, 
     16,   0,   4,   0,   0,   0, 
     58,   0,  16,   0,   4,   0, 
      0,   0,   0,   0,   0,   7, 
     18,   0,  16,   0,   5,   0, 
      0,   0,  58,   0,  16,   0, 
      4,   0,   0,   0,  58,   0, 
     16,   0,   4,   0,   0,   0, 
     29,   0,   0,   8,  18,   0, 
     16,   0,   5,   0,   0,   0, 
     10,   0,  16,   0,   5,   0, 
      0,   0,  10,   0,  16, 128, 
     65,   0,   0,   0,   5,   0, 
      0,   0,  55,   0,   0,  15, 
     50,   0,  16,   0,   5,   0, 
      0,   0,   6,   0,  16,   0, 
      5,   0,   0,   0,   2,  64, 
      0,   0,   0,   0,   0,  64, 
      0,   0,   0,  63,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0, 192,   0,   0,   0, 191, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  56,   0,   0,   7, 
    130,   0,  16,   0,   4,   0, 
      0,   0,  58,   0,  16,   0, 
      4,   0,   0,   0,  26,   0, 
     16,   0,   5,   0,   0,   0, 
     26,   0,   0,   5, 130,   0, 
     16,   0,   4,   0,   0,   0, 
     58,   0,  16,   0,   4,   0, 
      0,   0,  56,   0,   0,   7, 
    130,   0,  16,   0,   4,   0, 
      0,   0,  58,   0,  16,   0, 
      4,   0,   0,   0,  10,   0, 
     16,   0,   5,   0,   0,   0, 
     24,   0,   0,   7, 130,   0, 
     16,   0,   4,   0,   0,   0, 
     58,   0,  16,   0,   4,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0, 128,  63,  56,   0, 
      0,   7,  18,   0,  16,   0, 
      5,   0,   0,   0,  10,   0, 
     16,   0,   1,   0,   0,   0, 
     10,   0,  16,   0,   4,   0, 
      0,   0,  29,   0,   0,   8, 
     18,   0,  16,   0,   5,   0, 
      0,   0,  10,   0,  16,   0, 
      5,   0,   0,   0,  10,   0, 
     16, 128,  65,   0,   0,   0, 
      5,   0,   0,   0,  55,   0, 
      0,  10,  18,   0,  16,   0, 
      5,   0,   0,   0,  10,   0, 
     16,   0,   5,   0,   0,   0, 
     10,   0,  16,   0,   4,   0, 
      0,   0,  10,   0,  16, 128, 
     65,   0,   0,   0,   4,   0, 
      0,   0,  14,   0,   0,  10, 
     34,   0,  16,   0,   5,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0, 128,  63,   0,   0, 
    128,  63,   0,   0, 128,  63, 
      0,   0, 128,  63,  10,   0, 
     16,   0,   5,   0,   0,   0, 
     56,   0,   0,   7,  34,   0, 
     16,   0,   5,   0,   0,   0, 
     10,   0,  16,   0,   1,   0, 
      0,   0,  26,   0,  16,   0, 
      5,   0,   0,   0,  26,   0, 
      0,   5,  34,   0,  16,   0, 
      5,   0,   0,   0,  26,   0, 
     16,   0,   5,   0,   0,   0, 
     56,   0,   0,   7,  18,   0, 
     16,   0,   5,   0,   0,   0, 
     26,   0,  16,   0,   5,   0, 
      0,   0,  10,   0,  16,   0, 
      5,   0,   0,   0,  50,   0, 
      0,  11,  34,   0,  16,   0, 
      5,   0,   0,   0,  42, 128, 
     32,   0,   0,   0,   0,   0, 
      9,   0,   0,   0,  58, 128, 
     32,   0,   0,   0,   0,   0, 
      9,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128, 191, 
     65,   0,   0,   5,  66,   0, 
     16,   0,   5,   0,   0,   0, 
     10,   0,  16,   0,   5,   0, 
      0,   0,   0,   0,   0,   8, 
     66,   0,  16,   0,   5,   0, 
      0,   0,  42,   0,  16, 128, 
     65,   0,   0,   0,   5,   0, 
      0,   0,  26,   0,  16,   0, 
      5,   0,   0,   0,  55,   0, 
      0,   9,  18,   0,  16,   0, 
      6,   0,   0,   0,  58,   0, 
     16,   0,   4,   0,   0,   0, 
     42,   0,  16,   0,   5,   0, 
      0,   0,  10,   0,  16,   0, 
      5,   0,   0,   0,  14,   0, 
      0,   7, 130,   0,  16,   0, 
      4,   0,   0,   0,  58,   0, 
     16,   0,   1,   0,   0,   0, 
     10,   0,  16,   0,   4,   0, 
      0,   0,  65,   0,   0,   5, 
    130,   0,  16,   0,   4,   0, 
      0,   0,  58,   0,  16,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   7,  18,   0,  16,   0, 
      5,   0,   0,   0,  58,   0, 
     16,   0,   4,   0,   0,   0, 
     58,   0,  16,   0,   4,   0, 
      0,   0,  29,   0,   0,   8, 
     18,   0,  16,   0,   5,   0, 
      0,   0,  10,   0,  16,   0, 
      5,   0,   0,   0,  10,   0, 
     16, 128,  65,   0,   0,   0, 
      5,   0,   0,   0,  55,   0, 
      0,  15,  82,   0,  16,   0, 
      5,   0,   0,   0,   6,   0, 
     16,   0,   5,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,  64,   0,   0,   0,   0, 
      0,   0,   0,  63,   0,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0, 192,   0,   0, 
      0,   0,   0,   0,   0, 191, 
      0,   0,   0,   0,  56,   0, 
      0,   7, 130,   0,  16,   0, 
      4,   0,   0,   0,  58,   0, 
     16,   0,   4,   0,   0,   0, 
     42,   0,  16,   0,   5,   0, 
      0,   0,  26,   0,   0,   5, 
    130,   0,  16,   0,   4,   0, 
      0,   0,  58,   0,  16,   0, 
      4,   0,   0,   0,  56,   0, 
      0,   7, 130,   0,  16,   0, 
      4,   0,   0,   0,  58,   0, 
     16,   0,   4,   0,   0,   0, 
     10,   0,  16,   0,   5,   0, 
      0,   0,  24,   0,   0,   7, 
    130,   0,  16,   0,   4,   0, 
      0,   0,  58,   0,  16,   0, 
      4,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128,  63, 
     56,   0,   0,   7,  18,   0, 
     16,   0,   5,   0,   0,   0, 
     58,   0,  16,   0,   1,   0, 
      0,   0,  10,   0,  16,   0, 
      4,   0,   0,   0,  29,   0, 
      0,   8,  18,   0,  16,   0, 
      5,   0,   0,   0,  10,   0, 
     16,   0,   5,   0,   0,   0, 
     10,   0,  16, 128,  65,   0, 
      0,   0,   5,   0,   0,   0, 
     55,   0,   0,  10,  18,   0, 
     16,   0,   4,   0,   0,   0, 
     10,   0,  16,   0,   5,   0, 
      0,   0,  10,   0,  16,   0, 
      4,   0,   0,   0,  10,   0, 
     16, 128,  65,   0,   0,   0, 
      4,   0,   0,   0,  14,   0, 
      0,  10,  18,   0,  16,   0, 
      5,   0,   0,   0,   2,  64, 
      0,   0,   0,   0, 128,  63, 
      0,   0, 128,  63,   0,   0, 
    128,  63,   0,   0, 128,  63, 
     10,   0,  16,   0,   4,   0, 
      0,   0,  56,   0,   0,   7, 
    130,   0,  16,   0,   1,   0, 
      0,   0,  58,   0,  16,   0, 
      1,   0,   0,   0,  10,   0, 
     16,   0,   5,   0,   0,   0, 
     26,   0,   0,   5, 130,   0, 
     16,   0,   1,   0,   0,   0, 
     58,   0,  16,   0,   1,   0, 
      0,   0,  56,   0,   0,   7, 
    130,   0,  16,   0,   1,   0, 
      0,   0,  58,   0,  16,   0, 
      1,   0,   0,   0,  10,   0, 
     16,   0,   4,   0,   0,   0, 
     65,   0,   0,   5,  18,   0, 
     16,   0,   4,   0,   0,   0, 
     58,   0,  16,   0,   1,   0, 
      0,   0,   0,   0,   0,   8, 
     18,   0,  16,   0,   4,   0, 
      0,   0,  10,   0,  16, 128, 
     65,   0,   0,   0,   4,   0, 
      0,   0,  26,   0,  16,   0, 
      5,   0,   0,   0,  55,   0, 
      0,   9,  34,   0,  16,   0, 
      6,   0,   0,   0,  58,   0, 
     16,   0,   4,   0,   0,   0, 
     10,   0,  16,   0,   4,   0, 
      0,   0,  58,   0,  16,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   7,  66,   0,  16,   0, 
      1,   0,   0,   0,  10,   0, 
     16,   0,   1,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
    128,  63,  55,   0,   0,   9, 
     98,   0,  16,   0,   4,   0, 
      0,   0,  86,   5,  16,   0, 
      1,   0,   0,   0,   6,   1, 
     16,   0,   6,   0,   0,   0, 
      6,   2,  16,   0,   1,   0, 
      0,   0,  21,   0,   0,   1, 
     21,   0,   0,   1,  50,   0, 
      0,  11,  34,   0,  16,   0, 
      1,   0,   0,   0,  26, 128, 
     32,   0,   0,   0,   0,   0, 
      8,   0,   0,   0,  10,   0, 
     16,   0,   2,   0,   0,   0, 
     10, 128,  32,   0,   0,   0, 
      0,   0,   8,   0,   0,   0, 
     27,   0,   0,   5, 194,   0, 
     16,   0,   1,   0,   0,   0, 
     86,   9,  16,   0,   4,   0, 
      0,   0,  27,   0,   0,   6, 
     18,   0,  16,   0,   4,   0, 
      0,   0,  42, 128,  32,   0, 
      0,   0,   0,   0,   9,   0, 
      0,   0,   1,   0,   0,  10, 
     98,   0,  16,   0,   4,   0, 
      0,   0, 166,  11,  16,   0, 
      1,   0,   0,   0,   2,  64, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0, 128,   0,   0, 
      0, 128,   0,   0,   0,   0, 
     36,   0,   0,   8,  50,   0, 
     16,   0,   5,   0,   0,   0, 
    230,  10,  16,   0,   1,   0, 
      0,   0, 230,  10,  16, 128, 
     65,   0,   0,   0,   1,   0, 
      0,   0,  36,   0,   0,   8, 
    130,   0,  16,   0,   4,   0, 
      0,   0,  10,   0,  16,   0, 
      4,   0,   0,   0,  10,   0, 
     16, 128,  65,   0,   0,   0, 
      4,   0,   0,   0,  78,   0, 
      0,   9,  18,   0,  16,   0, 
      5,   0,   0,   0,  18,   0, 
     16,   0,   6,   0,   0,   0, 
     10,   0,  16,   0,   5,   0, 
      0,   0,  58,   0,  16,   0, 
      4,   0,   0,   0,  40,   0, 
      0,   5,  66,   0,  16,   0, 
      5,   0,   0,   0,  10,   0, 
     16,   0,   6,   0,   0,   0, 
     55,   0,   0,   9,  34,   0, 
     16,   0,   4,   0,   0,   0, 
     26,   0,  16,   0,   4,   0, 
      0,   0,  42,   0,  16,   0, 
      5,   0,   0,   0,  10,   0, 
     16,   0,   6,   0,   0,   0, 
     43,   0,   0,   5,  18,   0, 
     16,   0,   6,   0,   0,   0, 
     26,   0,  16,   0,   4,   0, 
      0,   0,  87,   0,   0,   7, 
    194,   0,  16,   0,   1,   0, 
      0,   0, 166,  14,  16,   0, 
      1,   0,   0,   0,   6,   0, 
     16,   0,   4,   0,   0,   0, 
     40,   0,   0,   5,  18,   0, 
     16,   0,   4,   0,   0,   0, 
     10,   0,  16,   0,   5,   0, 
      0,   0,   1,   0,   0,  10, 
    194,   0,  16,   0,   1,   0, 
      0,   0, 166,  14,  16,   0, 
      1,   0,   0,   0,   2,  64, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0, 128,   0,   0,   0, 128, 
     55,   0,   0,   9,  66,   0, 
     16,   0,   1,   0,   0,   0, 
     42,   0,  16,   0,   1,   0, 
      0,   0,  10,   0,  16,   0, 
      4,   0,   0,   0,  10,   0, 
     16,   0,   5,   0,   0,   0, 
     43,   0,   0,   5,  34,   0, 
     16,   0,   6,   0,   0,   0, 
     42,   0,  16,   0,   1,   0, 
      0,   0,  50,   0,   0,  11, 
     50,   0,  16,   0,   4,   0, 
      0,   0,  70,   0,  16,   0, 
      6,   0,   0,   0,  70, 128, 
     32,   0,   0,   0,   0,   0, 
     10,   0,   0,   0, 230, 138, 
     32,   0,   0,   0,   0,   0, 
     10,   0,   0,   0,  54,   0, 
      0,   5,  18,   0,  16,   0, 
      1,   0,   0,   0,  10,  16, 
     16,   0,   4,   0,   0,   0, 
      0,   0,   0,   8,  50,   0, 
     16,   0,   1,   0,   0,   0, 
     70,   0,  16,   0,   1,   0, 
      0,   0,  70,   0,  16, 128, 
     65,   0,   0,   0,   4,   0, 
      0,   0,  78,   0,   0,   9, 
     18,   0,  16,   0,   4,   0, 
      0,   0,  18,   0,  16,   0, 
      5,   0,   0,   0,  26,   0, 
     16,   0,   5,   0,   0,   0, 
     58,   0,  16,   0,   4,   0, 
      0,   0,  40,   0,   0,   5, 
     66,   0,  16,   0,   1,   0, 
      0,   0,  10,   0,  16,   0, 
      5,   0,   0,   0,  55,   0, 
      0,   9,  66,   0,  16,   0, 
      1,   0,   0,   0,  42,   0, 
     16,   0,   4,   0,   0,   0, 
     42,   0,  16,   0,   1,   0, 
      0,   0,  10,   0,  16,   0, 
      5,   0,   0,   0,  43,   0, 
      0,   5,  18,   0,  16,   0, 
      5,   0,   0,   0,  42,   0, 
     16,   0,   1,   0,   0,   0, 
     40,   0,   0,   5,  66,   0, 
     16,   0,   1,   0,   0,   0, 
     10,   0,  16,   0,   4,   0, 
      0,   0,  55,   0,   0,   9, 
     66,   0,  16,   0,   1,   0, 
      0,   0,  58,   0,  16,   0, 
      1,   0,   0,   0,  42,   0, 
     16,   0,   1,   0,   0,   0, 
     10,   0,  16,   0,   4,   0, 
      0,   0,  43,   0,   0,   5, 
     34,   0,  16,   0,   5,   0, 
      0,   0,  42,   0,  16,   0, 
      1,   0,   0,   0,  50,   0, 
      0,  10,  50,   0,  16,   0, 
      1,   0,   0,   0,  70,   0, 
     16,   0,   5,   0,   0,   0, 
     70, 128,  32,   0,   0,   0, 
      0,   0,  10,   0,   0,   0, 
     70,   0,  16,   0,   1,   0, 
      0,   0,   0,   0,   0,   8, 
     50,   0,  16,   0,   1,   0, 
      0,   0,  70,   0,  16,   0, 
      1,   0,   0,   0, 230, 138, 
     32,   0,   0,   0,   0,   0, 
     10,   0,   0,   0,  50,   0, 
      0,  11,  66,   0,  16,   0, 
      1,   0,   0,   0,  26, 128, 
     32,   0,   0,   0,   0,   0, 
      8,   0,   0,   0,  26,   0, 
     16,   0,   1,   0,   0,   0, 
     10, 128,  32,   0,   0,   0, 
      0,   0,   8,   0,   0,   0, 
     54,   0,   0,   5,  98,   0, 
     16,   0,   0,   0,   0,   0, 
      6,   2,  16,   0,   1,   0, 
      0,   0,  18,   0,   0,   1, 
     54,   0,   0,   8,  98,   0, 
     16,   0,   0,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  54,   0,   0,   5, 
     34,   0,  16,   0,   2,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,  21,   0, 
      0,   1,  54,   0,   0,   5, 
    242,  32,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      3,   0,   0,   0,  54,   0, 
      0,   5,  66,   0,  16,   0, 
      2,   0,   0,   0,  10,  16, 
     16,   0,   4,   0,   0,   0, 
     54,   0,   0,   5, 130,   0, 
     16,   0,   2,   0,   0,   0, 
     10,  16,  16,   0,  10,   0, 
      0,   0,  54,   0,   0,   5, 
    242,  32,  16,   0,   1,   0, 
      0,   0,  38,  13,  16,   0, 
      2,   0,   0,   0,  54,   0, 
      0,   5, 242,  32,  16,   0, 
      4,   0,   0,   0,  70,  14, 
     16,   0,   3,   0,   0,   0, 
     54,   0,   0,   5, 242,  32, 
     16,   0,   5,   0,   0,   0, 
     70,  30,  16,   0,   1,   0, 
      0,   0,  54,   0,   0,   5, 
     82,  32,  16,   0,   6,   0, 
      0,   0,   6,  18,  16,   0, 
      6,   0,   0,   0,  54,   0, 
      0,   5,  82,  32,  16,   0, 
      7,   0,   0,   0,   6,  18, 
     16,   0,   8,   0,   0,   0, 
     54,   0,   0,   5, 130,   0, 
     16,   0,   0,   0,   0,   0, 
     10,  16,  16,   0,   7,   0, 
      0,   0,  54,   0,   0,   5, 
    242,  32,  16,   0,   8,   0, 
      0,   0,  54,   9,  16,   0, 
      0,   0,   0,   0,  62,   0, 
      0,   1,  83,  84,  65,  84, 
    116,   0,   0,   0, 142,   0, 
      0,   0,   7,   0,   0,   0, 
      0,   0,   0,   0,  19,   0, 
      0,   0,  69,   0,   0,   0, 
      6,   0,   0,   0,   3,   0, 
      0,   0,   4,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  27,   0,   0,   0, 
     14,   0,   0,   0,  18,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0
};