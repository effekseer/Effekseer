#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//
//   fxc /Zpc /Tvs_4_0 /Emain /D __INST__=40 /Fh ShaderHeader/model_unlit_vs.h
//    Shader/model_unlit_vs.fx
//
//
// Buffer Definitions: 
//
// cbuffer VS_ConstantBuffer
// {
//
//   float4x4 mCameraProj;              // Offset:    0 Size:    64
//   float4x4 mModel[40];               // Offset:   64 Size:  2560
//   float4 fUV[40];                    // Offset: 2624 Size:   640
//   float4 fModelColor[40];            // Offset: 3264 Size:   640
//   float4 fLightDirection;            // Offset: 3904 Size:    16 [unused]
//   float4 fLightColor;                // Offset: 3920 Size:    16 [unused]
//   float4 fLightAmbient;              // Offset: 3936 Size:    16 [unused]
//   float4 mUVInversed;                // Offset: 3952 Size:    16
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
// NORMAL                   0   xyz         1     NONE  float       
// NORMAL                   1   xyz         2     NONE  float       
// NORMAL                   2   xyz         3     NONE  float       
// TEXCOORD                 0   xy          4     NONE  float   xy  
// NORMAL                   3   xyzw        5     NONE  float   xyzw
// SV_InstanceID            0   x           6   INSTID   uint   x   
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue Format   Used
// -------------------- ----- ------ -------- -------- ------ ------
// SV_POSITION              0   xyzw        0      POS  float   xyzw
// COLOR                    0   xyzw        1     NONE  float   xyzw
// TEXCOORD                 0   xy          2     NONE  float   xy  
// TEXCOORD                 4   xyzw        3     NONE  float   xyzw
//
vs_4_0
dcl_constantbuffer cb0[248], dynamicIndexed
dcl_input v0.xyz
dcl_input v4.xy
dcl_input v5.xyzw
dcl_input_sgv v6.x, instance_id
dcl_output_siv o0.xyzw, position
dcl_output o1.xyzw
dcl_output o2.xy
dcl_output o3.xyzw
dcl_temps 2
ishl r0.x, v6.x, l(2)
mul r1.xyzw, v0.yyyy, cb0[r0.x + 5].xyzw
mad r1.xyzw, cb0[r0.x + 4].xyzw, v0.xxxx, r1.xyzw
mad r1.xyzw, cb0[r0.x + 6].xyzw, v0.zzzz, r1.xyzw
add r0.xyzw, r1.xyzw, cb0[r0.x + 7].xyzw
mul r1.xyzw, r0.yyyy, cb0[1].xyzw
mad r1.xyzw, cb0[0].xyzw, r0.xxxx, r1.xyzw
mad r1.xyzw, cb0[2].xyzw, r0.zzzz, r1.xyzw
mad r0.xyzw, cb0[3].xyzw, r0.wwww, r1.xyzw
mov o0.xyzw, r0.xyzw
mov o3.xyzw, r0.xyzw
mov r0.x, v6.x
mul o1.xyzw, v5.xyzw, cb0[r0.x + 204].xyzw
mad r0.y, v4.y, cb0[r0.x + 164].w, cb0[r0.x + 164].y
mad o2.x, v4.x, cb0[r0.x + 164].z, cb0[r0.x + 164].x
mad o2.y, cb0[247].y, r0.y, cb0[247].x
ret 
// Approximately 17 instruction slots used
#endif

const BYTE g_main[] =
{
     68,  88,  66,  67, 126, 142, 
     49, 102,  73,  32, 254,  22, 
      9, 162, 254,  51, 165, 192, 
    194,  22,   1,   0,   0,   0, 
     40,   7,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
     64,   2,   0,   0,  32,   3, 
      0,   0, 172,   3,   0,   0, 
    172,   6,   0,   0,  82,  68, 
     69,  70,   4,   2,   0,   0, 
      1,   0,   0,   0,  80,   0, 
      0,   0,   1,   0,   0,   0, 
     28,   0,   0,   0,   0,   4, 
    254, 255,  16,   1,   0,   0, 
    210,   1,   0,   0,  60,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
     86,  83,  95,  67, 111, 110, 
    115, 116,  97, 110, 116,  66, 
    117, 102, 102, 101, 114,   0, 
    171, 171,  60,   0,   0,   0, 
      8,   0,   0,   0, 104,   0, 
      0,   0, 128,  15,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  40,   1,   0,   0, 
      0,   0,   0,   0,  64,   0, 
      0,   0,   2,   0,   0,   0, 
     52,   1,   0,   0,   0,   0, 
      0,   0,  68,   1,   0,   0, 
     64,   0,   0,   0,   0,  10, 
      0,   0,   2,   0,   0,   0, 
     76,   1,   0,   0,   0,   0, 
      0,   0,  92,   1,   0,   0, 
     64,  10,   0,   0, 128,   2, 
      0,   0,   2,   0,   0,   0, 
     96,   1,   0,   0,   0,   0, 
      0,   0, 112,   1,   0,   0, 
    192,  12,   0,   0, 128,   2, 
      0,   0,   2,   0,   0,   0, 
    124,   1,   0,   0,   0,   0, 
      0,   0, 140,   1,   0,   0, 
     64,  15,   0,   0,  16,   0, 
      0,   0,   0,   0,   0,   0, 
    156,   1,   0,   0,   0,   0, 
      0,   0, 172,   1,   0,   0, 
     80,  15,   0,   0,  16,   0, 
      0,   0,   0,   0,   0,   0, 
    156,   1,   0,   0,   0,   0, 
      0,   0, 184,   1,   0,   0, 
     96,  15,   0,   0,  16,   0, 
      0,   0,   0,   0,   0,   0, 
    156,   1,   0,   0,   0,   0, 
      0,   0, 198,   1,   0,   0, 
    112,  15,   0,   0,  16,   0, 
      0,   0,   2,   0,   0,   0, 
    156,   1,   0,   0,   0,   0, 
      0,   0, 109,  67,  97, 109, 
    101, 114,  97,  80, 114, 111, 
    106,   0,   3,   0,   3,   0, 
      4,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    109,  77, 111, 100, 101, 108, 
      0, 171,   3,   0,   3,   0, 
      4,   0,   4,   0,  40,   0, 
      0,   0,   0,   0,   0,   0, 
    102,  85,  86,   0,   1,   0, 
      3,   0,   1,   0,   4,   0, 
     40,   0,   0,   0,   0,   0, 
      0,   0, 102,  77, 111, 100, 
    101, 108,  67, 111, 108, 111, 
    114,   0,   1,   0,   3,   0, 
      1,   0,   4,   0,  40,   0, 
      0,   0,   0,   0,   0,   0, 
    102,  76, 105, 103, 104, 116, 
     68, 105, 114, 101,  99, 116, 
    105, 111, 110,   0,   1,   0, 
      3,   0,   1,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 102,  76, 105, 103, 
    104, 116,  67, 111, 108, 111, 
    114,   0, 102,  76, 105, 103, 
    104, 116,  65, 109,  98, 105, 
    101, 110, 116,   0, 109,  85, 
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
     73,  83,  71,  78, 216,   0, 
      0,   0,   7,   0,   0,   0, 
      8,   0,   0,   0, 176,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      7,   7,   0,   0, 185,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
      7,   0,   0,   0, 185,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   2,   0,   0,   0, 
      7,   0,   0,   0, 185,   0, 
      0,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   3,   0,   0,   0, 
      7,   0,   0,   0, 192,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   4,   0,   0,   0, 
      3,   3,   0,   0, 185,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   5,   0,   0,   0, 
     15,  15,   0,   0, 201,   0, 
      0,   0,   0,   0,   0,   0, 
      8,   0,   0,   0,   1,   0, 
      0,   0,   6,   0,   0,   0, 
      1,   1,   0,   0,  80,  79, 
     83,  73,  84,  73,  79,  78, 
      0,  78,  79,  82,  77,  65, 
     76,   0,  84,  69,  88,  67, 
     79,  79,  82,  68,   0,  83, 
     86,  95,  73, 110, 115, 116, 
     97, 110,  99, 101,  73,  68, 
      0, 171,  79,  83,  71,  78, 
    132,   0,   0,   0,   4,   0, 
      0,   0,   8,   0,   0,   0, 
    104,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  15,   0,   0,   0, 
    116,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   1,   0, 
      0,   0,  15,   0,   0,   0, 
    122,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   2,   0, 
      0,   0,   3,  12,   0,   0, 
    122,   0,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   3,   0, 
      0,   0,  15,   0,   0,   0, 
     83,  86,  95,  80,  79,  83, 
     73,  84,  73,  79,  78,   0, 
     67,  79,  76,  79,  82,   0, 
     84,  69,  88,  67,  79,  79, 
     82,  68,   0, 171,  83,  72, 
     68,  82, 248,   2,   0,   0, 
     64,   0,   1,   0, 190,   0, 
      0,   0,  89,   8,   0,   4, 
     70, 142,  32,   0,   0,   0, 
      0,   0, 248,   0,   0,   0, 
     95,   0,   0,   3, 114,  16, 
     16,   0,   0,   0,   0,   0, 
     95,   0,   0,   3,  50,  16, 
     16,   0,   4,   0,   0,   0, 
     95,   0,   0,   3, 242,  16, 
     16,   0,   5,   0,   0,   0, 
     96,   0,   0,   4,  18,  16, 
     16,   0,   6,   0,   0,   0, 
      8,   0,   0,   0, 103,   0, 
      0,   4, 242,  32,  16,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   1,   0, 
      0,   0, 101,   0,   0,   3, 
     50,  32,  16,   0,   2,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   3,   0, 
      0,   0, 104,   0,   0,   2, 
      2,   0,   0,   0,  41,   0, 
      0,   7,  18,   0,  16,   0, 
      0,   0,   0,   0,  10,  16, 
     16,   0,   6,   0,   0,   0, 
      1,  64,   0,   0,   2,   0, 
      0,   0,  56,   0,   0,  10, 
    242,   0,  16,   0,   1,   0, 
      0,   0,  86,  21,  16,   0, 
      0,   0,   0,   0,  70, 142, 
     32,   6,   0,   0,   0,   0, 
      5,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
     50,   0,   0,  12, 242,   0, 
     16,   0,   1,   0,   0,   0, 
     70, 142,  32,   6,   0,   0, 
      0,   0,   4,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,   6,  16,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   1,   0,   0,   0, 
     50,   0,   0,  12, 242,   0, 
     16,   0,   1,   0,   0,   0, 
     70, 142,  32,   6,   0,   0, 
      0,   0,   6,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0, 166,  26,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   1,   0,   0,   0, 
      0,   0,   0,  10, 242,   0, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   1,   0, 
      0,   0,  70, 142,  32,   6, 
      0,   0,   0,   0,   7,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  56,   0, 
      0,   8, 242,   0,  16,   0, 
      1,   0,   0,   0,  86,   5, 
     16,   0,   0,   0,   0,   0, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
     50,   0,   0,  10, 242,   0, 
     16,   0,   1,   0,   0,   0, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      6,   0,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      1,   0,   0,   0,  50,   0, 
      0,  10, 242,   0,  16,   0, 
      1,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      2,   0,   0,   0, 166,  10, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   1,   0, 
      0,   0,  50,   0,   0,  10, 
    242,   0,  16,   0,   0,   0, 
      0,   0,  70, 142,  32,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0, 246,  15,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   1,   0,   0,   0, 
     54,   0,   0,   5, 242,  32, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   0,   0, 
      0,   0,  54,   0,   0,   5, 
    242,  32,  16,   0,   3,   0, 
      0,   0,  70,  14,  16,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   5,  18,   0,  16,   0, 
      0,   0,   0,   0,  10,  16, 
     16,   0,   6,   0,   0,   0, 
     56,   0,   0,  10, 242,  32, 
     16,   0,   1,   0,   0,   0, 
     70,  30,  16,   0,   5,   0, 
      0,   0,  70, 142,  32,   6, 
      0,   0,   0,   0, 204,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  50,   0, 
      0,  15,  34,   0,  16,   0, 
      0,   0,   0,   0,  26,  16, 
     16,   0,   4,   0,   0,   0, 
     58, 128,  32,   6,   0,   0, 
      0,   0, 164,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  26, 128,  32,   6, 
      0,   0,   0,   0, 164,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  50,   0, 
      0,  15,  18,  32,  16,   0, 
      2,   0,   0,   0,  10,  16, 
     16,   0,   4,   0,   0,   0, 
     42, 128,  32,   6,   0,   0, 
      0,   0, 164,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  10, 128,  32,   6, 
      0,   0,   0,   0, 164,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  50,   0, 
      0,  11,  34,  32,  16,   0, 
      2,   0,   0,   0,  26, 128, 
     32,   0,   0,   0,   0,   0, 
    247,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     10, 128,  32,   0,   0,   0, 
      0,   0, 247,   0,   0,   0, 
     62,   0,   0,   1,  83,  84, 
     65,  84, 116,   0,   0,   0, 
     17,   0,   0,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
      8,   0,   0,   0,   4,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0
};
