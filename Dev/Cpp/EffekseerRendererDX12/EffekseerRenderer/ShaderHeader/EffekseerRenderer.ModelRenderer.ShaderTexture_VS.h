#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//
//   fxc /Zpc /Tvs_4_0 /D /Emain /Fh
//    ShaderHeader/EffekseerRenderer.ModelRenderer.ShaderTexture_VS.h
//    Shader/model_renderer_texture_VS.fx
//
//
// Buffer Definitions: 
//
// cbuffer VS_ConstantBuffer
// {
//
//   float4x4 _364_mCameraProj;         // Offset:    0 Size:    64
//   float4x4 _364_mModel[10];          // Offset:   64 Size:   640
//   float4 _364_fUV[10];               // Offset:  704 Size:   160
//   float4 _364_fAlphaUV[10];          // Offset:  864 Size:   160
//   float4 _364_fUVDistortionUV[10];   // Offset: 1024 Size:   160
//   float4 _364_fBlendUV[10];          // Offset: 1184 Size:   160
//   float4 _364_fBlendAlphaUV[10];     // Offset: 1344 Size:   160
//   float4 _364_fBlendUVDistortionUV[10];// Offset: 1504 Size:   160
//   float4 _364_fFlipbookParameter;    // Offset: 1664 Size:    16
//   float4 _364_fFlipbookIndexAndNextRate[10];// Offset: 1680 Size:   160
//   float4 _364_fModelAlphaThreshold[10];// Offset: 1840 Size:   160
//   float4 _364_fModelColor[10];       // Offset: 2000 Size:   160
//   float4 _364_fLightDirection;       // Offset: 2160 Size:    16 [unused]
//   float4 _364_fLightColor;           // Offset: 2176 Size:    16 [unused]
//   float4 _364_fLightAmbient;         // Offset: 2192 Size:    16 [unused]
//   float4 _364_mUVInversed;           // Offset: 2208 Size:    16
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
// TEXCOORD                 0   xyz         0     NONE  float   xyz 
// TEXCOORD                 1   xyz         1     NONE  float   xyz 
// TEXCOORD                 2   xyz         2     NONE  float       
// TEXCOORD                 3   xyz         3     NONE  float       
// TEXCOORD                 4   xy          4     NONE  float   xy  
// TEXCOORD                 5   xyzw        5     NONE  float   xyzw
// SV_InstanceID            0   x           6   INSTID   uint   x   
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue Format   Used
// -------------------- ----- ------ -------- -------- ------ ------
// TEXCOORD                 0   xy          0     NONE  float   xy  
// TEXCOORD                 1   xyz         1     NONE  float   xyz 
// TEXCOORD                 2   xyz         2     NONE  float   xyz 
// TEXCOORD                 3   xyz         3     NONE  float   xyz 
// TEXCOORD                 4   xyzw        4     NONE  float   xyzw
// TEXCOORD                 5   xyzw        5     NONE  float   xyzw
// TEXCOORD                 6   xyzw        6     NONE  float   xyzw
// TEXCOORD                 7   xyzw        7     NONE  float   xyzw
// TEXCOORD                 8   xy          8     NONE  float   xy  
// SV_Position              0   xyzw        9      POS  float   xyzw
//
vs_4_0
dcl_constantbuffer cb0[139], dynamicIndexed
dcl_input v0.xyz
dcl_input v1.xyz
dcl_input v4.xy
dcl_input v5.xyzw
dcl_input_sgv v6.x, instance_id
dcl_output o0.xy
dcl_output o1.xyz
dcl_output o2.xyz
dcl_output o3.xyz
dcl_output o4.xyzw
dcl_output o5.xyzw
dcl_output o6.xyzw
dcl_output o7.xyzw
dcl_output o8.xy
dcl_output_siv o9.xyzw, position
dcl_temps 7
ishl r0.x, v6.x, l(2)
mov r0.y, v6.x
mul o4.xyzw, v5.xyzw, cb0[r0.y + 125].xyzw
mul r1.xyzw, v0.yyyy, cb0[r0.x + 5].xyzw
mad r1.xyzw, cb0[r0.x + 4].xyzw, v0.xxxx, r1.xyzw
mad r1.xyzw, cb0[r0.x + 6].xyzw, v0.zzzz, r1.xyzw
add r1.xyzw, r1.xyzw, cb0[r0.x + 7].xyzw
mul r2.xyzw, r1.yyyy, cb0[1].xyzw
mad r2.xyzw, cb0[0].xyzw, r1.xxxx, r2.xyzw
mad r2.xyzw, cb0[2].xyzw, r1.zzzz, r2.xyzw
mad o9.xyzw, cb0[3].xyzw, r1.wwww, r2.xyzw
mad o0.x, v4.x, cb0[r0.y + 44].z, cb0[r0.y + 44].x
mad r0.z, v4.y, cb0[r0.y + 44].w, cb0[r0.y + 44].y
mul r1.xyzw, v1.yyyy, cb0[r0.x + 5].xyzw
mad r1.xyzw, cb0[r0.x + 4].xyzw, v1.xxxx, r1.xyzw
mad r1.xyzw, cb0[r0.x + 6].xyzw, v1.zzzz, r1.xyzw
dp4 r0.x, r1.xyzw, r1.xyzw
rsq r0.x, r0.x
mul o1.xyz, r0.xxxx, r1.xyzx
mad o0.y, cb0[138].y, r0.z, cb0[138].x
mad o5.x, v4.x, cb0[r0.y + 54].z, cb0[r0.y + 54].x
mad r0.x, v4.y, cb0[r0.y + 54].w, cb0[r0.y + 54].y
mad o5.z, v4.x, cb0[r0.y + 64].z, cb0[r0.y + 64].x
mad r0.z, v4.y, cb0[r0.y + 64].w, cb0[r0.y + 64].y
mad o7.x, v4.x, cb0[r0.y + 74].z, cb0[r0.y + 74].x
mad r0.w, v4.y, cb0[r0.y + 74].w, cb0[r0.y + 74].y
mad o6.x, v4.x, cb0[r0.y + 84].z, cb0[r0.y + 84].x
mad r1.x, v4.y, cb0[r0.y + 84].w, cb0[r0.y + 84].y
mad o6.z, v4.x, cb0[r0.y + 94].z, cb0[r0.y + 94].x
mad r1.y, v4.y, cb0[r0.y + 94].w, cb0[r0.y + 94].y
lt r1.z, l(0.000000), cb0[104].x
if_nz r1.z
  frc r2.x, cb0[r0.y + 105].x
  round_ni r3.x, cb0[r0.y + 105].x
  add r1.z, r3.x, l(1.000000)
  mul r1.w, cb0[104].w, cb0[104].z
  eq r2.z, cb0[104].y, l(0.000000)
  if_nz r2.z
    ge r2.z, r1.z, r1.w
    mad r2.w, cb0[104].z, cb0[104].w, l(-1.000000)
    add r3.y, r3.x, l(1.000000)
    movc r2.zw, r2.zzzz, r2.wwww, r3.xxxy
  else 
    div r3.y, r3.x, r1.w
    round_ni r3.y, r3.y
    mad r4.x, -r1.w, r3.y, r3.x
    div r3.w, r1.z, r1.w
    round_ni r3.w, r3.w
    mad r4.y, -r1.w, r3.w, r1.z
    eq r1.zw, cb0[104].yyyy, l(0.000000, 0.000000, 1.000000, 2.000000)
    mul r4.z, r3.y, l(0.500000)
    round_ni r4.z, r4.z
    mad r3.y, -r4.z, l(2.000000), r3.y
    eq r3.y, r3.y, l(1.000000)
    mad r4.z, cb0[104].z, cb0[104].w, l(-1.000000)
    round_ni r5.xy, r4.xyxx
    add r4.zw, r4.zzzz, -r5.xxxy
    movc r5.x, r3.y, r4.z, r4.x
    mul r3.y, r3.w, l(0.500000)
    round_ni r3.y, r3.y
    mad r3.y, -r3.y, l(2.000000), r3.w
    eq r3.y, r3.y, l(1.000000)
    movc r5.y, r3.y, r4.w, r4.y
    add r3.z, r3.x, l(1.000000)
    movc r3.xy, r1.wwww, r5.xyxx, r3.xzxx
    movc r2.zw, r1.zzzz, r4.xxxy, r3.xxxy
  endif 
  ftoi r1.zw, r2.zzzw
  ftoi r2.z, cb0[104].z
  and r3.xy, r1.zwzz, l(0x80000000, 0x80000000, 0, 0)
  imax r3.zw, r1.zzzw, -r1.zzzw
  imax r2.w, r2.z, -r2.z
  udiv r4.x, r5.x, r3.z, r2.w
  ineg r3.z, r5.x
  movc r3.x, r3.x, r3.z, r5.x
  itof r4.z, r3.x
  xor r1.zw, r1.zzzw, r2.zzzz
  ineg r2.z, r4.x
  and r1.zw, r1.zzzw, l(0, 0, 0x80000000, 0x80000000)
  movc r1.z, r1.z, r2.z, r4.x
  itof r4.w, r1.z
  div r3.xz, l(1.000000, 1.000000, 1.000000, 1.000000), cb0[104].zzwz
  mad r4.xy, -r4.zwzz, r3.xzxx, v4.xyxx
  mul r4.xy, r4.xyxx, cb0[104].zwzz
  udiv r5.x, r6.x, r3.w, r2.w
  ineg r1.z, r6.x
  movc r1.z, r3.y, r1.z, r6.x
  itof r2.z, r1.z
  ineg r1.z, r5.x
  movc r1.z, r1.w, r1.z, r5.x
  itof r2.w, r1.z
  mul r1.zw, r3.xxxz, r2.zzzw
  mad o7.zw, r4.xxxy, r3.xxxz, r1.zzzw
else 
  mov o7.zw, l(0,0,0,0)
  mov r2.x, l(0)
endif 
mad o5.y, cb0[138].y, r0.x, cb0[138].x
mad o5.w, cb0[138].y, r0.z, cb0[138].x
mad o7.y, cb0[138].y, r0.w, cb0[138].x
mad o6.y, cb0[138].y, r1.x, cb0[138].x
mad o6.w, cb0[138].y, r1.y, cb0[138].x
mov o2.xyz, l(0,0,0,0)
mov o3.xyz, l(0,0,0,0)
mov r2.y, cb0[r0.y + 115].x
mov o8.xy, r2.xyxx
ret 
// Approximately 107 instruction slots used
#endif

const BYTE g_main[] =
{
     68,  88,  66,  67, 140, 104, 
    126,  69,  27, 158, 181,  46, 
    108, 148, 115,  50,   5, 184, 
    255,  81,   1,   0,   0,   0, 
    224,  21,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
     92,   4,   0,   0,  44,   5, 
      0,   0,  68,   6,   0,   0, 
    100,  21,   0,   0,  82,  68, 
     69,  70,  32,   4,   0,   0, 
      1,   0,   0,   0,  80,   0, 
      0,   0,   1,   0,   0,   0, 
     28,   0,   0,   0,   0,   4, 
    254, 255,  16,   1,   0,   0, 
    238,   3,   0,   0,  60,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
     86,  83,  95,  67, 111, 110, 
    115, 116,  97, 110, 116,  66, 
    117, 102, 102, 101, 114,   0, 
    171, 171,  60,   0,   0,   0, 
     16,   0,   0,   0, 104,   0, 
      0,   0, 176,   8,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 232,   1,   0,   0, 
      0,   0,   0,   0,  64,   0, 
      0,   0,   2,   0,   0,   0, 
    252,   1,   0,   0,   0,   0, 
      0,   0,  12,   2,   0,   0, 
     64,   0,   0,   0, 128,   2, 
      0,   0,   2,   0,   0,   0, 
     24,   2,   0,   0,   0,   0, 
      0,   0,  40,   2,   0,   0, 
    192,   2,   0,   0, 160,   0, 
      0,   0,   2,   0,   0,   0, 
     52,   2,   0,   0,   0,   0, 
      0,   0,  68,   2,   0,   0, 
     96,   3,   0,   0, 160,   0, 
      0,   0,   2,   0,   0,   0, 
     84,   2,   0,   0,   0,   0, 
      0,   0, 100,   2,   0,   0, 
      0,   4,   0,   0, 160,   0, 
      0,   0,   2,   0,   0,   0, 
    124,   2,   0,   0,   0,   0, 
      0,   0, 140,   2,   0,   0, 
    160,   4,   0,   0, 160,   0, 
      0,   0,   2,   0,   0,   0, 
    156,   2,   0,   0,   0,   0, 
      0,   0, 172,   2,   0,   0, 
     64,   5,   0,   0, 160,   0, 
      0,   0,   2,   0,   0,   0, 
    192,   2,   0,   0,   0,   0, 
      0,   0, 208,   2,   0,   0, 
    224,   5,   0,   0, 160,   0, 
      0,   0,   2,   0,   0,   0, 
    236,   2,   0,   0,   0,   0, 
      0,   0, 252,   2,   0,   0, 
    128,   6,   0,   0,  16,   0, 
      0,   0,   2,   0,   0,   0, 
     20,   3,   0,   0,   0,   0, 
      0,   0,  36,   3,   0,   0, 
    144,   6,   0,   0, 160,   0, 
      0,   0,   2,   0,   0,   0, 
     68,   3,   0,   0,   0,   0, 
      0,   0,  84,   3,   0,   0, 
     48,   7,   0,   0, 160,   0, 
      0,   0,   2,   0,   0,   0, 
    112,   3,   0,   0,   0,   0, 
      0,   0, 128,   3,   0,   0, 
    208,   7,   0,   0, 160,   0, 
      0,   0,   2,   0,   0,   0, 
    148,   3,   0,   0,   0,   0, 
      0,   0, 164,   3,   0,   0, 
    112,   8,   0,   0,  16,   0, 
      0,   0,   0,   0,   0,   0, 
     20,   3,   0,   0,   0,   0, 
      0,   0, 185,   3,   0,   0, 
    128,   8,   0,   0,  16,   0, 
      0,   0,   0,   0,   0,   0, 
     20,   3,   0,   0,   0,   0, 
      0,   0, 202,   3,   0,   0, 
    144,   8,   0,   0,  16,   0, 
      0,   0,   0,   0,   0,   0, 
     20,   3,   0,   0,   0,   0, 
      0,   0, 221,   3,   0,   0, 
    160,   8,   0,   0,  16,   0, 
      0,   0,   2,   0,   0,   0, 
     20,   3,   0,   0,   0,   0, 
      0,   0,  95,  51,  54,  52, 
     95, 109,  67,  97, 109, 101, 
    114,  97,  80, 114, 111, 106, 
      0, 171, 171, 171,   3,   0, 
      3,   0,   4,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  95,  51,  54,  52, 
     95, 109,  77, 111, 100, 101, 
    108,   0,   3,   0,   3,   0, 
      4,   0,   4,   0,  10,   0, 
      0,   0,   0,   0,   0,   0, 
     95,  51,  54,  52,  95, 102, 
     85,  86,   0, 171, 171, 171, 
      1,   0,   3,   0,   1,   0, 
      4,   0,  10,   0,   0,   0, 
      0,   0,   0,   0,  95,  51, 
     54,  52,  95, 102,  65, 108, 
    112, 104,  97,  85,  86,   0, 
    171, 171,   1,   0,   3,   0, 
      1,   0,   4,   0,  10,   0, 
      0,   0,   0,   0,   0,   0, 
     95,  51,  54,  52,  95, 102, 
     85,  86,  68, 105, 115, 116, 
    111, 114, 116, 105, 111, 110, 
     85,  86,   0, 171, 171, 171, 
      1,   0,   3,   0,   1,   0, 
      4,   0,  10,   0,   0,   0, 
      0,   0,   0,   0,  95,  51, 
     54,  52,  95, 102,  66, 108, 
    101, 110, 100,  85,  86,   0, 
    171, 171,   1,   0,   3,   0, 
      1,   0,   4,   0,  10,   0, 
      0,   0,   0,   0,   0,   0, 
     95,  51,  54,  52,  95, 102, 
     66, 108, 101, 110, 100,  65, 
    108, 112, 104,  97,  85,  86, 
      0, 171,   1,   0,   3,   0, 
      1,   0,   4,   0,  10,   0, 
      0,   0,   0,   0,   0,   0, 
     95,  51,  54,  52,  95, 102, 
     66, 108, 101, 110, 100,  85, 
     86,  68, 105, 115, 116, 111, 
    114, 116, 105, 111, 110,  85, 
     86,   0, 171, 171,   1,   0, 
      3,   0,   1,   0,   4,   0, 
     10,   0,   0,   0,   0,   0, 
      0,   0,  95,  51,  54,  52, 
     95, 102,  70, 108, 105, 112, 
     98, 111, 111, 107,  80,  97, 
    114,  97, 109, 101, 116, 101, 
    114,   0,   1,   0,   3,   0, 
      1,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     95,  51,  54,  52,  95, 102, 
     70, 108, 105, 112,  98, 111, 
    111, 107,  73, 110, 100, 101, 
    120,  65, 110, 100,  78, 101, 
    120, 116,  82,  97, 116, 101, 
      0, 171,   1,   0,   3,   0, 
      1,   0,   4,   0,  10,   0, 
      0,   0,   0,   0,   0,   0, 
     95,  51,  54,  52,  95, 102, 
     77, 111, 100, 101, 108,  65, 
    108, 112, 104,  97,  84, 104, 
    114, 101, 115, 104, 111, 108, 
    100,   0, 171, 171,   1,   0, 
      3,   0,   1,   0,   4,   0, 
     10,   0,   0,   0,   0,   0, 
      0,   0,  95,  51,  54,  52, 
     95, 102,  77, 111, 100, 101, 
    108,  67, 111, 108, 111, 114, 
      0, 171, 171, 171,   1,   0, 
      3,   0,   1,   0,   4,   0, 
     10,   0,   0,   0,   0,   0, 
      0,   0,  95,  51,  54,  52, 
     95, 102,  76, 105, 103, 104, 
    116,  68, 105, 114, 101,  99, 
    116, 105, 111, 110,   0,  95, 
     51,  54,  52,  95, 102,  76, 
    105, 103, 104, 116,  67, 111, 
    108, 111, 114,   0,  95,  51, 
     54,  52,  95, 102,  76, 105, 
    103, 104, 116,  65, 109,  98, 
    105, 101, 110, 116,   0,  95, 
     51,  54,  52,  95, 109,  85, 
     86,  73, 110, 118, 101, 114, 
    115, 101, 100,   0,  77, 105, 
     99, 114, 111, 115, 111, 102, 
    116,  32,  40,  82,  41,  32, 
     72,  76,  83,  76,  32,  83, 
    104,  97, 100, 101, 114,  32, 
     67, 111, 109, 112, 105, 108, 
    101, 114,  32,  57,  46,  50, 
     57,  46,  57,  53,  50,  46, 
     51,  49,  49,  49,   0, 171, 
     73,  83,  71,  78, 200,   0, 
      0,   0,   7,   0,   0,   0, 
      8,   0,   0,   0, 176,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      7,   7,   0,   0, 176,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
      7,   7,   0,   0, 176,   0, 
      0,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   2,   0,   0,   0, 
      7,   0,   0,   0, 176,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   3,   0,   0,   0, 
      7,   0,   0,   0, 176,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   4,   0,   0,   0, 
      3,   3,   0,   0, 176,   0, 
      0,   0,   5,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   5,   0,   0,   0, 
     15,  15,   0,   0, 185,   0, 
      0,   0,   0,   0,   0,   0, 
      8,   0,   0,   0,   1,   0, 
      0,   0,   6,   0,   0,   0, 
      1,   1,   0,   0,  84,  69, 
     88,  67,  79,  79,  82,  68, 
      0,  83,  86,  95,  73, 110, 
    115, 116,  97, 110,  99, 101, 
     73,  68,   0, 171,  79,  83, 
     71,  78,  16,   1,   0,   0, 
     10,   0,   0,   0,   8,   0, 
      0,   0, 248,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   3,  12, 
      0,   0, 248,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      1,   0,   0,   0,   7,   8, 
      0,   0, 248,   0,   0,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      2,   0,   0,   0,   7,   8, 
      0,   0, 248,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      3,   0,   0,   0,   7,   8, 
      0,   0, 248,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      4,   0,   0,   0,  15,   0, 
      0,   0, 248,   0,   0,   0, 
      5,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      5,   0,   0,   0,  15,   0, 
      0,   0, 248,   0,   0,   0, 
      6,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      6,   0,   0,   0,  15,   0, 
      0,   0, 248,   0,   0,   0, 
      7,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      7,   0,   0,   0,  15,   0, 
      0,   0, 248,   0,   0,   0, 
      8,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      8,   0,   0,   0,   3,  12, 
      0,   0,   1,   1,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   3,   0,   0,   0, 
      9,   0,   0,   0,  15,   0, 
      0,   0,  84,  69,  88,  67, 
     79,  79,  82,  68,   0,  83, 
     86,  95,  80, 111, 115, 105, 
    116, 105, 111, 110,   0, 171, 
    171, 171,  83,  72,  68,  82, 
     24,  15,   0,   0,  64,   0, 
      1,   0, 198,   3,   0,   0, 
     89,   8,   0,   4,  70, 142, 
     32,   0,   0,   0,   0,   0, 
    139,   0,   0,   0,  95,   0, 
      0,   3, 114,  16,  16,   0, 
      0,   0,   0,   0,  95,   0, 
      0,   3, 114,  16,  16,   0, 
      1,   0,   0,   0,  95,   0, 
      0,   3,  50,  16,  16,   0, 
      4,   0,   0,   0,  95,   0, 
      0,   3, 242,  16,  16,   0, 
      5,   0,   0,   0,  96,   0, 
      0,   4,  18,  16,  16,   0, 
      6,   0,   0,   0,   8,   0, 
      0,   0, 101,   0,   0,   3, 
     50,  32,  16,   0,   0,   0, 
      0,   0, 101,   0,   0,   3, 
    114,  32,  16,   0,   1,   0, 
      0,   0, 101,   0,   0,   3, 
    114,  32,  16,   0,   2,   0, 
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
      0,   0, 101,   0,   0,   3, 
     50,  32,  16,   0,   8,   0, 
      0,   0, 103,   0,   0,   4, 
    242,  32,  16,   0,   9,   0, 
      0,   0,   1,   0,   0,   0, 
    104,   0,   0,   2,   7,   0, 
      0,   0,  41,   0,   0,   7, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  10,  16,  16,   0, 
      6,   0,   0,   0,   1,  64, 
      0,   0,   2,   0,   0,   0, 
     54,   0,   0,   5,  34,   0, 
     16,   0,   0,   0,   0,   0, 
     10,  16,  16,   0,   6,   0, 
      0,   0,  56,   0,   0,  10, 
    242,  32,  16,   0,   4,   0, 
      0,   0,  70,  30,  16,   0, 
      5,   0,   0,   0,  70, 142, 
     32,   6,   0,   0,   0,   0, 
    125,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     56,   0,   0,  10, 242,   0, 
     16,   0,   1,   0,   0,   0, 
     86,  21,  16,   0,   0,   0, 
      0,   0,  70, 142,  32,   6, 
      0,   0,   0,   0,   5,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  50,   0, 
      0,  12, 242,   0,  16,   0, 
      1,   0,   0,   0,  70, 142, 
     32,   6,   0,   0,   0,   0, 
      4,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
      6,  16,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      1,   0,   0,   0,  50,   0, 
      0,  12, 242,   0,  16,   0, 
      1,   0,   0,   0,  70, 142, 
     32,   6,   0,   0,   0,   0, 
      6,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
    166,  26,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      1,   0,   0,   0,   0,   0, 
      0,  10, 242,   0,  16,   0, 
      1,   0,   0,   0,  70,  14, 
     16,   0,   1,   0,   0,   0, 
     70, 142,  32,   6,   0,   0, 
      0,   0,   7,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  56,   0,   0,   8, 
    242,   0,  16,   0,   2,   0, 
      0,   0,  86,   5,  16,   0, 
      1,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,  50,   0, 
      0,  10, 242,   0,  16,   0, 
      2,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   6,   0, 
     16,   0,   1,   0,   0,   0, 
     70,  14,  16,   0,   2,   0, 
      0,   0,  50,   0,   0,  10, 
    242,   0,  16,   0,   2,   0, 
      0,   0,  70, 142,  32,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0, 166,  10,  16,   0, 
      1,   0,   0,   0,  70,  14, 
     16,   0,   2,   0,   0,   0, 
     50,   0,   0,  10, 242,  32, 
     16,   0,   9,   0,   0,   0, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
    246,  15,  16,   0,   1,   0, 
      0,   0,  70,  14,  16,   0, 
      2,   0,   0,   0,  50,   0, 
      0,  15,  18,  32,  16,   0, 
      0,   0,   0,   0,  10,  16, 
     16,   0,   4,   0,   0,   0, 
     42, 128,  32,   6,   0,   0, 
      0,   0,  44,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0,  10, 128,  32,   6, 
      0,   0,   0,   0,  44,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,  50,   0, 
      0,  15,  66,   0,  16,   0, 
      0,   0,   0,   0,  26,  16, 
     16,   0,   4,   0,   0,   0, 
     58, 128,  32,   6,   0,   0, 
      0,   0,  44,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0,  26, 128,  32,   6, 
      0,   0,   0,   0,  44,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,  56,   0, 
      0,  10, 242,   0,  16,   0, 
      1,   0,   0,   0,  86,  21, 
     16,   0,   1,   0,   0,   0, 
     70, 142,  32,   6,   0,   0, 
      0,   0,   5,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  50,   0,   0,  12, 
    242,   0,  16,   0,   1,   0, 
      0,   0,  70, 142,  32,   6, 
      0,   0,   0,   0,   4,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,   6,  16, 
     16,   0,   1,   0,   0,   0, 
     70,  14,  16,   0,   1,   0, 
      0,   0,  50,   0,   0,  12, 
    242,   0,  16,   0,   1,   0, 
      0,   0,  70, 142,  32,   6, 
      0,   0,   0,   0,   6,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0, 166,  26, 
     16,   0,   1,   0,   0,   0, 
     70,  14,  16,   0,   1,   0, 
      0,   0,  17,   0,   0,   7, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      1,   0,   0,   0,  70,  14, 
     16,   0,   1,   0,   0,   0, 
     68,   0,   0,   5,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  56,   0,   0,   7, 
    114,  32,  16,   0,   1,   0, 
      0,   0,   6,   0,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
     50,   0,   0,  11,  34,  32, 
     16,   0,   0,   0,   0,   0, 
     26, 128,  32,   0,   0,   0, 
      0,   0, 138,   0,   0,   0, 
     42,   0,  16,   0,   0,   0, 
      0,   0,  10, 128,  32,   0, 
      0,   0,   0,   0, 138,   0, 
      0,   0,  50,   0,   0,  15, 
     18,  32,  16,   0,   5,   0, 
      0,   0,  10,  16,  16,   0, 
      4,   0,   0,   0,  42, 128, 
     32,   6,   0,   0,   0,   0, 
     54,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     10, 128,  32,   6,   0,   0, 
      0,   0,  54,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0,  50,   0,   0,  15, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  26,  16,  16,   0, 
      4,   0,   0,   0,  58, 128, 
     32,   6,   0,   0,   0,   0, 
     54,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     26, 128,  32,   6,   0,   0, 
      0,   0,  54,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0,  50,   0,   0,  15, 
     66,  32,  16,   0,   5,   0, 
      0,   0,  10,  16,  16,   0, 
      4,   0,   0,   0,  42, 128, 
     32,   6,   0,   0,   0,   0, 
     64,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     10, 128,  32,   6,   0,   0, 
      0,   0,  64,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0,  50,   0,   0,  15, 
     66,   0,  16,   0,   0,   0, 
      0,   0,  26,  16,  16,   0, 
      4,   0,   0,   0,  58, 128, 
     32,   6,   0,   0,   0,   0, 
     64,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     26, 128,  32,   6,   0,   0, 
      0,   0,  64,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0,  50,   0,   0,  15, 
     18,  32,  16,   0,   7,   0, 
      0,   0,  10,  16,  16,   0, 
      4,   0,   0,   0,  42, 128, 
     32,   6,   0,   0,   0,   0, 
     74,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     10, 128,  32,   6,   0,   0, 
      0,   0,  74,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0,  50,   0,   0,  15, 
    130,   0,  16,   0,   0,   0, 
      0,   0,  26,  16,  16,   0, 
      4,   0,   0,   0,  58, 128, 
     32,   6,   0,   0,   0,   0, 
     74,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     26, 128,  32,   6,   0,   0, 
      0,   0,  74,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0,  50,   0,   0,  15, 
     18,  32,  16,   0,   6,   0, 
      0,   0,  10,  16,  16,   0, 
      4,   0,   0,   0,  42, 128, 
     32,   6,   0,   0,   0,   0, 
     84,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     10, 128,  32,   6,   0,   0, 
      0,   0,  84,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0,  50,   0,   0,  15, 
     18,   0,  16,   0,   1,   0, 
      0,   0,  26,  16,  16,   0, 
      4,   0,   0,   0,  58, 128, 
     32,   6,   0,   0,   0,   0, 
     84,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     26, 128,  32,   6,   0,   0, 
      0,   0,  84,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0,  50,   0,   0,  15, 
     66,  32,  16,   0,   6,   0, 
      0,   0,  10,  16,  16,   0, 
      4,   0,   0,   0,  42, 128, 
     32,   6,   0,   0,   0,   0, 
     94,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     10, 128,  32,   6,   0,   0, 
      0,   0,  94,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0,  50,   0,   0,  15, 
     34,   0,  16,   0,   1,   0, 
      0,   0,  26,  16,  16,   0, 
      4,   0,   0,   0,  58, 128, 
     32,   6,   0,   0,   0,   0, 
     94,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     26, 128,  32,   6,   0,   0, 
      0,   0,  94,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0,  49,   0,   0,   8, 
     66,   0,  16,   0,   1,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,  10, 128, 
     32,   0,   0,   0,   0,   0, 
    104,   0,   0,   0,  31,   0, 
      4,   3,  42,   0,  16,   0, 
      1,   0,   0,   0,  26,   0, 
      0,   8,  18,   0,  16,   0, 
      2,   0,   0,   0,  10, 128, 
     32,   6,   0,   0,   0,   0, 
    105,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     65,   0,   0,   8,  18,   0, 
     16,   0,   3,   0,   0,   0, 
     10, 128,  32,   6,   0,   0, 
      0,   0, 105,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0,   0,   0,   0,   7, 
     66,   0,  16,   0,   1,   0, 
      0,   0,  10,   0,  16,   0, 
      3,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128,  63, 
     56,   0,   0,   9, 130,   0, 
     16,   0,   1,   0,   0,   0, 
     58, 128,  32,   0,   0,   0, 
      0,   0, 104,   0,   0,   0, 
     42, 128,  32,   0,   0,   0, 
      0,   0, 104,   0,   0,   0, 
     24,   0,   0,   8,  66,   0, 
     16,   0,   2,   0,   0,   0, 
     26, 128,  32,   0,   0,   0, 
      0,   0, 104,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  31,   0,   4,   3, 
     42,   0,  16,   0,   2,   0, 
      0,   0,  29,   0,   0,   7, 
     66,   0,  16,   0,   2,   0, 
      0,   0,  42,   0,  16,   0, 
      1,   0,   0,   0,  58,   0, 
     16,   0,   1,   0,   0,   0, 
     50,   0,   0,  11, 130,   0, 
     16,   0,   2,   0,   0,   0, 
     42, 128,  32,   0,   0,   0, 
      0,   0, 104,   0,   0,   0, 
     58, 128,  32,   0,   0,   0, 
      0,   0, 104,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
    128, 191,   0,   0,   0,   7, 
     34,   0,  16,   0,   3,   0, 
      0,   0,  10,   0,  16,   0, 
      3,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128,  63, 
     55,   0,   0,   9, 194,   0, 
     16,   0,   2,   0,   0,   0, 
    166,  10,  16,   0,   2,   0, 
      0,   0, 246,  15,  16,   0, 
      2,   0,   0,   0,   6,   4, 
     16,   0,   3,   0,   0,   0, 
     18,   0,   0,   1,  14,   0, 
      0,   7,  34,   0,  16,   0, 
      3,   0,   0,   0,  10,   0, 
     16,   0,   3,   0,   0,   0, 
     58,   0,  16,   0,   1,   0, 
      0,   0,  65,   0,   0,   5, 
     34,   0,  16,   0,   3,   0, 
      0,   0,  26,   0,  16,   0, 
      3,   0,   0,   0,  50,   0, 
      0,  10,  18,   0,  16,   0, 
      4,   0,   0,   0,  58,   0, 
     16, 128,  65,   0,   0,   0, 
      1,   0,   0,   0,  26,   0, 
     16,   0,   3,   0,   0,   0, 
     10,   0,  16,   0,   3,   0, 
      0,   0,  14,   0,   0,   7, 
    130,   0,  16,   0,   3,   0, 
      0,   0,  42,   0,  16,   0, 
      1,   0,   0,   0,  58,   0, 
     16,   0,   1,   0,   0,   0, 
     65,   0,   0,   5, 130,   0, 
     16,   0,   3,   0,   0,   0, 
     58,   0,  16,   0,   3,   0, 
      0,   0,  50,   0,   0,  10, 
     34,   0,  16,   0,   4,   0, 
      0,   0,  58,   0,  16, 128, 
     65,   0,   0,   0,   1,   0, 
      0,   0,  58,   0,  16,   0, 
      3,   0,   0,   0,  42,   0, 
     16,   0,   1,   0,   0,   0, 
     24,   0,   0,  11, 194,   0, 
     16,   0,   1,   0,   0,   0, 
     86, 133,  32,   0,   0,   0, 
      0,   0, 104,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 128,  63,   0,   0, 
      0,  64,  56,   0,   0,   7, 
     66,   0,  16,   0,   4,   0, 
      0,   0,  26,   0,  16,   0, 
      3,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,  63, 
     65,   0,   0,   5,  66,   0, 
     16,   0,   4,   0,   0,   0, 
     42,   0,  16,   0,   4,   0, 
      0,   0,  50,   0,   0,  10, 
     34,   0,  16,   0,   3,   0, 
      0,   0,  42,   0,  16, 128, 
     65,   0,   0,   0,   4,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,  64,  26,   0, 
     16,   0,   3,   0,   0,   0, 
     24,   0,   0,   7,  34,   0, 
     16,   0,   3,   0,   0,   0, 
     26,   0,  16,   0,   3,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0, 128,  63,  50,   0, 
      0,  11,  66,   0,  16,   0, 
      4,   0,   0,   0,  42, 128, 
     32,   0,   0,   0,   0,   0, 
    104,   0,   0,   0,  58, 128, 
     32,   0,   0,   0,   0,   0, 
    104,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128, 191, 
     65,   0,   0,   5,  50,   0, 
     16,   0,   5,   0,   0,   0, 
     70,   0,  16,   0,   4,   0, 
      0,   0,   0,   0,   0,   8, 
    194,   0,  16,   0,   4,   0, 
      0,   0, 166,  10,  16,   0, 
      4,   0,   0,   0,   6,   4, 
     16, 128,  65,   0,   0,   0, 
      5,   0,   0,   0,  55,   0, 
      0,   9,  18,   0,  16,   0, 
      5,   0,   0,   0,  26,   0, 
     16,   0,   3,   0,   0,   0, 
     42,   0,  16,   0,   4,   0, 
      0,   0,  10,   0,  16,   0, 
      4,   0,   0,   0,  56,   0, 
      0,   7,  34,   0,  16,   0, 
      3,   0,   0,   0,  58,   0, 
     16,   0,   3,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,  63,  65,   0,   0,   5, 
     34,   0,  16,   0,   3,   0, 
      0,   0,  26,   0,  16,   0, 
      3,   0,   0,   0,  50,   0, 
      0,  10,  34,   0,  16,   0, 
      3,   0,   0,   0,  26,   0, 
     16, 128,  65,   0,   0,   0, 
      3,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,  64, 
     58,   0,  16,   0,   3,   0, 
      0,   0,  24,   0,   0,   7, 
     34,   0,  16,   0,   3,   0, 
      0,   0,  26,   0,  16,   0, 
      3,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128,  63, 
     55,   0,   0,   9,  34,   0, 
     16,   0,   5,   0,   0,   0, 
     26,   0,  16,   0,   3,   0, 
      0,   0,  58,   0,  16,   0, 
      4,   0,   0,   0,  26,   0, 
     16,   0,   4,   0,   0,   0, 
      0,   0,   0,   7,  66,   0, 
     16,   0,   3,   0,   0,   0, 
     10,   0,  16,   0,   3,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0, 128,  63,  55,   0, 
      0,   9,  50,   0,  16,   0, 
      3,   0,   0,   0, 246,  15, 
     16,   0,   1,   0,   0,   0, 
     70,   0,  16,   0,   5,   0, 
      0,   0, 134,   0,  16,   0, 
      3,   0,   0,   0,  55,   0, 
      0,   9, 194,   0,  16,   0, 
      2,   0,   0,   0, 166,  10, 
     16,   0,   1,   0,   0,   0, 
      6,   4,  16,   0,   4,   0, 
      0,   0,   6,   4,  16,   0, 
      3,   0,   0,   0,  21,   0, 
      0,   1,  27,   0,   0,   5, 
    194,   0,  16,   0,   1,   0, 
      0,   0, 166,  14,  16,   0, 
      2,   0,   0,   0,  27,   0, 
      0,   6,  66,   0,  16,   0, 
      2,   0,   0,   0,  42, 128, 
     32,   0,   0,   0,   0,   0, 
    104,   0,   0,   0,   1,   0, 
      0,  10,  50,   0,  16,   0, 
      3,   0,   0,   0, 230,  10, 
     16,   0,   1,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0, 128,   0,   0,   0, 128, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  36,   0,   0,   8, 
    194,   0,  16,   0,   3,   0, 
      0,   0, 166,  14,  16,   0, 
      1,   0,   0,   0, 166,  14, 
     16, 128,  65,   0,   0,   0, 
      1,   0,   0,   0,  36,   0, 
      0,   8, 130,   0,  16,   0, 
      2,   0,   0,   0,  42,   0, 
     16,   0,   2,   0,   0,   0, 
     42,   0,  16, 128,  65,   0, 
      0,   0,   2,   0,   0,   0, 
     78,   0,   0,   9,  18,   0, 
     16,   0,   4,   0,   0,   0, 
     18,   0,  16,   0,   5,   0, 
      0,   0,  42,   0,  16,   0, 
      3,   0,   0,   0,  58,   0, 
     16,   0,   2,   0,   0,   0, 
     40,   0,   0,   5,  66,   0, 
     16,   0,   3,   0,   0,   0, 
     10,   0,  16,   0,   5,   0, 
      0,   0,  55,   0,   0,   9, 
     18,   0,  16,   0,   3,   0, 
      0,   0,  10,   0,  16,   0, 
      3,   0,   0,   0,  42,   0, 
     16,   0,   3,   0,   0,   0, 
     10,   0,  16,   0,   5,   0, 
      0,   0,  43,   0,   0,   5, 
     66,   0,  16,   0,   4,   0, 
      0,   0,  10,   0,  16,   0, 
      3,   0,   0,   0,  87,   0, 
      0,   7, 194,   0,  16,   0, 
      1,   0,   0,   0, 166,  14, 
     16,   0,   1,   0,   0,   0, 
    166,  10,  16,   0,   2,   0, 
      0,   0,  40,   0,   0,   5, 
     66,   0,  16,   0,   2,   0, 
      0,   0,  10,   0,  16,   0, 
      4,   0,   0,   0,   1,   0, 
      0,  10, 194,   0,  16,   0, 
      1,   0,   0,   0, 166,  14, 
     16,   0,   1,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0, 128,   0,   0, 
      0, 128,  55,   0,   0,   9, 
     66,   0,  16,   0,   1,   0, 
      0,   0,  42,   0,  16,   0, 
      1,   0,   0,   0,  42,   0, 
     16,   0,   2,   0,   0,   0, 
     10,   0,  16,   0,   4,   0, 
      0,   0,  43,   0,   0,   5, 
    130,   0,  16,   0,   4,   0, 
      0,   0,  42,   0,  16,   0, 
      1,   0,   0,   0,  14,   0, 
      0,  11,  82,   0,  16,   0, 
      3,   0,   0,   0,   2,  64, 
      0,   0,   0,   0, 128,  63, 
      0,   0, 128,  63,   0,   0, 
    128,  63,   0,   0, 128,  63, 
    166, 139,  32,   0,   0,   0, 
      0,   0, 104,   0,   0,   0, 
     50,   0,   0,  10,  50,   0, 
     16,   0,   4,   0,   0,   0, 
    230,  10,  16, 128,  65,   0, 
      0,   0,   4,   0,   0,   0, 
    134,   0,  16,   0,   3,   0, 
      0,   0,  70,  16,  16,   0, 
      4,   0,   0,   0,  56,   0, 
      0,   8,  50,   0,  16,   0, 
      4,   0,   0,   0,  70,   0, 
     16,   0,   4,   0,   0,   0, 
    230, 138,  32,   0,   0,   0, 
      0,   0, 104,   0,   0,   0, 
     78,   0,   0,   9,  18,   0, 
     16,   0,   5,   0,   0,   0, 
     18,   0,  16,   0,   6,   0, 
      0,   0,  58,   0,  16,   0, 
      3,   0,   0,   0,  58,   0, 
     16,   0,   2,   0,   0,   0, 
     40,   0,   0,   5,  66,   0, 
     16,   0,   1,   0,   0,   0, 
     10,   0,  16,   0,   6,   0, 
      0,   0,  55,   0,   0,   9, 
     66,   0,  16,   0,   1,   0, 
      0,   0,  26,   0,  16,   0, 
      3,   0,   0,   0,  42,   0, 
     16,   0,   1,   0,   0,   0, 
     10,   0,  16,   0,   6,   0, 
      0,   0,  43,   0,   0,   5, 
     66,   0,  16,   0,   2,   0, 
      0,   0,  42,   0,  16,   0, 
      1,   0,   0,   0,  40,   0, 
      0,   5,  66,   0,  16,   0, 
      1,   0,   0,   0,  10,   0, 
     16,   0,   5,   0,   0,   0, 
     55,   0,   0,   9,  66,   0, 
     16,   0,   1,   0,   0,   0, 
     58,   0,  16,   0,   1,   0, 
      0,   0,  42,   0,  16,   0, 
      1,   0,   0,   0,  10,   0, 
     16,   0,   5,   0,   0,   0, 
     43,   0,   0,   5, 130,   0, 
     16,   0,   2,   0,   0,   0, 
     42,   0,  16,   0,   1,   0, 
      0,   0,  56,   0,   0,   7, 
    194,   0,  16,   0,   1,   0, 
      0,   0,   6,   8,  16,   0, 
      3,   0,   0,   0, 166,  14, 
     16,   0,   2,   0,   0,   0, 
     50,   0,   0,   9, 194,  32, 
     16,   0,   7,   0,   0,   0, 
      6,   4,  16,   0,   4,   0, 
      0,   0,   6,   8,  16,   0, 
      3,   0,   0,   0, 166,  14, 
     16,   0,   1,   0,   0,   0, 
     18,   0,   0,   1,  54,   0, 
      0,   8, 194,  32,  16,   0, 
      7,   0,   0,   0,   2,  64, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     54,   0,   0,   5,  18,   0, 
     16,   0,   2,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  21,   0,   0,   1, 
     50,   0,   0,  11,  34,  32, 
     16,   0,   5,   0,   0,   0, 
     26, 128,  32,   0,   0,   0, 
      0,   0, 138,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  10, 128,  32,   0, 
      0,   0,   0,   0, 138,   0, 
      0,   0,  50,   0,   0,  11, 
    130,  32,  16,   0,   5,   0, 
      0,   0,  26, 128,  32,   0, 
      0,   0,   0,   0, 138,   0, 
      0,   0,  42,   0,  16,   0, 
      0,   0,   0,   0,  10, 128, 
     32,   0,   0,   0,   0,   0, 
    138,   0,   0,   0,  50,   0, 
      0,  11,  34,  32,  16,   0, 
      7,   0,   0,   0,  26, 128, 
     32,   0,   0,   0,   0,   0, 
    138,   0,   0,   0,  58,   0, 
     16,   0,   0,   0,   0,   0, 
     10, 128,  32,   0,   0,   0, 
      0,   0, 138,   0,   0,   0, 
     50,   0,   0,  11,  34,  32, 
     16,   0,   6,   0,   0,   0, 
     26, 128,  32,   0,   0,   0, 
      0,   0, 138,   0,   0,   0, 
     10,   0,  16,   0,   1,   0, 
      0,   0,  10, 128,  32,   0, 
      0,   0,   0,   0, 138,   0, 
      0,   0,  50,   0,   0,  11, 
    130,  32,  16,   0,   6,   0, 
      0,   0,  26, 128,  32,   0, 
      0,   0,   0,   0, 138,   0, 
      0,   0,  26,   0,  16,   0, 
      1,   0,   0,   0,  10, 128, 
     32,   0,   0,   0,   0,   0, 
    138,   0,   0,   0,  54,   0, 
      0,   8, 114,  32,  16,   0, 
      2,   0,   0,   0,   2,  64, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     54,   0,   0,   8, 114,  32, 
     16,   0,   3,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  54,   0,   0,   8, 
     34,   0,  16,   0,   2,   0, 
      0,   0,  10, 128,  32,   6, 
      0,   0,   0,   0, 115,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   5,  50,  32,  16,   0, 
      8,   0,   0,   0,  70,   0, 
     16,   0,   2,   0,   0,   0, 
     62,   0,   0,   1,  83,  84, 
     65,  84, 116,   0,   0,   0, 
    107,   0,   0,   0,   7,   0, 
      0,   0,   0,   0,   0,   0, 
     15,   0,   0,   0,  33,   0, 
      0,   0,   7,   0,   0,   0, 
      3,   0,   0,   0,   3,   0, 
      0,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  14,   0, 
      0,   0,   9,   0,   0,   0, 
     13,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0
};
