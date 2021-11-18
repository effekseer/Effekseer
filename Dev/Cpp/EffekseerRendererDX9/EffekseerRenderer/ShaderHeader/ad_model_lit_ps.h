#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//   fxc /Zpc /Tps_3_0 /Emain /Fh ShaderHeader/ad_model_lit_ps.h
//    Shader/ad_model_lit_ps.fx
//
//
// Parameters:
//
//   sampler2D Sampler_sampler_alphaTex;
//   sampler2D Sampler_sampler_blendAlphaTex;
//   sampler2D Sampler_sampler_blendTex;
//   sampler2D Sampler_sampler_blendUVDistortionTex;
//   sampler2D Sampler_sampler_colorTex;
//   sampler2D Sampler_sampler_normalTex;
//   sampler2D Sampler_sampler_uvDistortionTex;
//   float4 _285_fBlendTextureParameter;
//   float4 _285_fCameraFrontDirection;
//   float4 _285_fEdgeColor;
//   float4 _285_fEdgeParameter;
//   float4 _285_fEmissiveScaling;
//   float4 _285_fFalloffBeginColor;
//   float4 _285_fFalloffEndColor;
//   float4 _285_fFalloffParameter;
//   float4 _285_fFlipbookParameter;
//   float4 _285_fLightAmbient;
//   float4 _285_fLightColor;
//   float4 _285_fLightDirection;
//   float4 _285_fUVDistortionParameter;
//   float4 _285_miscFlags;
//
//
// Registers:
//
//   Name                                 Reg   Size
//   ------------------------------------ ----- ----
//   _285_fLightDirection                 c0       1
//   _285_fLightColor                     c1       1
//   _285_fLightAmbient                   c2       1
//   _285_fFlipbookParameter              c3       1
//   _285_fUVDistortionParameter          c4       1
//   _285_fBlendTextureParameter          c5       1
//   _285_fCameraFrontDirection           c6       1
//   _285_fFalloffParameter               c7       1
//   _285_fFalloffBeginColor              c8       1
//   _285_fFalloffEndColor                c9       1
//   _285_fEmissiveScaling                c10      1
//   _285_fEdgeColor                      c11      1
//   _285_fEdgeParameter                  c12      1
//   _285_miscFlags                       c17      1
//   Sampler_sampler_colorTex             s0       1
//   Sampler_sampler_normalTex            s1       1
//   Sampler_sampler_alphaTex             s2       1
//   Sampler_sampler_uvDistortionTex      s3       1
//   Sampler_sampler_blendTex             s4       1
//   Sampler_sampler_blendAlphaTex        s5       1
//   Sampler_sampler_blendUVDistortionTex s6       1
//

    ps_3_0
    def c13, 2, -1, 1.1920929e-007, 0.416666657
    def c14, 1.05499995, -0.0549999997, 0, -0.5
    def c15, -1, -2, -3, -0
    def c16, 0.305306017, 0.682171106, 0.0125228781, 0
    dcl_texcoord_centroid v0
    dcl_texcoord1_centroid v1
    dcl_texcoord2 v2.xyz
    dcl_texcoord3 v3.xyz
    dcl_texcoord4 v4.xyz
    dcl_texcoord5 v5
    dcl_texcoord6 v6
    dcl_texcoord7 v7
    dcl_2d s0
    dcl_2d s1
    dcl_2d s2
    dcl_2d s3
    dcl_2d s4
    dcl_2d s5
    dcl_2d s6
    texld r0, v5.zwzw, s3
    mad r0.xy, r0, c13.x, c13.y
    mad r0.z, r0.y, -c4.w, c4.z
    mad r0.yw, r0.xxzz, c4.x, v1.xxzy
    texld r1, r0.ywzw, s0
    max r2.xyz, r1_abs, c13.z
    log r3.x, r2.x
    log r3.y, r2.y
    log r3.z, r2.z
    mul r2.xyz, r3, c13.w
    exp r3.x, r2.x
    exp r3.y, r2.y
    exp r3.z, r2.z
    mad r2.xyz, r3, c14.x, c14.y
    max r1.xyz, r2, c14.z
    mul r1, r1, v0
    texld r2, r0.ywzw, s1
    add r2.xyz, r2, c14.w
    add r2.xyz, r2, r2
    mul r3.xyz, r2.y, v3
    mad r2.xyw, r2.x, v4.xyzz, r3.xyzz
    mad r2.xyz, r2.z, v2, r2.xyww
    nrm r3.xyz, r2
    mad r0.yw, r0.xxzz, c4.x, v7.xzzw
    texld r2, r0.ywzw, s0
    mov r4.xy, c13
    add r0.y, r4.y, c3.y
    mad r2, r2, v0, -r1
    mad r2, v1.z, r2, r1
    cmp r2, -r0_abs.y, r2, r1
    cmp r1, -c3.x, r1, r2
    mad r0.xy, r0.xzzw, c4.x, v5
    texld r0, r0, s2
    mul r0.x, r0.w, r0.x
    mul r0.w, r0.x, r1.w
    texld r2, v6.zwzw, s6
    mad r2.xy, r2, c13.x, c13.y
    mad r2.z, r2.y, -c4.w, c4.z
    mad r2.yw, r2.xxzz, c4.y, v7.xxzy
    texld r5, r2.ywzw, s4
    max r6.xyz, r5_abs, c13.z
    log r5.x, r6.x
    log r5.y, r6.y
    log r5.z, r6.z
    mul r5.xyz, r5, c13.w
    exp r6.x, r5.x
    exp r6.y, r5.y
    exp r6.z, r5.z
    mad r5.xyz, r6, c14.x, c14.y
    mad r2.xy, r2.xzzw, c4.y, v6
    texld r2, r2, s5
    mul r1.w, r2.w, r2.x
    mul r2.x, r1.w, r5.w
    abs r2.y, c5.x
    mul r2.xzw, r2.x, r5.xyyz
    cmp r2.xzw, r5.xyyz, r2, c14.z
    mad r1.w, r5.w, -r1.w, -c13.y
    mad r5.xyz, r1, r1.w, r2.xzww
    mov r6.xyz, c15
    add r6.xyz, r6, c5.x
    add r7.xyz, r1, r2.xzww
    add r8.xyz, r1, -r2.xzww
    mul r2.xzw, r1.xyyz, r2
    cmp r1.xyz, -r6_abs.z, r2.xzww, r1
    cmp r1.xyz, -r6_abs.y, r8, r1
    cmp r1.xyz, -r6_abs.x, r7, r1
    cmp r1.xyz, -r2.y, r5, r1
    dp3 r1.w, c0, r3
    mul r2.xyz, r1.w, c1
    cmp r2.xyz, r1.w, r2, c14.z
    add r2.xyz, r2, c2
    mul r0.xyz, r1, r2
    add r4.xyz, -r4_abs.yyxw, c7.xyyw
    nrm r5.xyz, -c6
    dp3_sat r1.w, r5, r3
    pow r2.w, r1.w, c7.z
    mov r3, c9
    add r3, -r3, c8
    mad r3, r2.w, r3, c9
    abs r1.w, c7.y
    mad r5.xyz, r1, r2, r3
    mad r1.xyz, r1, r2, -r3
    mul r2.xyz, r0, r3
    cmp r2.xyz, -r4_abs.z, r2, r0
    cmp r1.xyz, -r4_abs.y, r1, r2
    cmp r1.xyz, -r1.w, r5, r1
    mul r1.w, r0.w, r3.w
    cmp r0, -r4_abs.x, r1, r0
    max r1.x, v1.w, c14.z
    add r1.x, -r0.w, r1.x
    cmp r1, r1.x, c15.x, c15.w
    texkill r1
    mov r1.y, c12.y
    mul r1.xyz, r1.y, c11
    add r1.w, r0.w, -v1.w
    add r1.w, r1.w, -c12.x
    frc r2.x, -r1.w
    add r1.w, r1.w, r2.x
    mad r0.xyz, r0, c10.x, -r1
    mad r0.xyz, r1.w, r0, r1
    abs r1.x, c17.x
    mad r1.yzw, r0.xxyz, c16.x, c16.y
    mad r1.yzw, r0.xxyz, r1, c16.z
    mul r1.yzw, r0.xxyz, r1
    cmp oC0.xyz, -r1.x, r0, r1.yzww
    mov oC0.w, r0.w

// approximately 112 instruction slots used (8 texture, 104 arithmetic)
#endif

const BYTE g_ps30_main[] =
{
      0,   3, 255, 255, 254, 255, 
     34,   1,  67,  84,  65,  66, 
     28,   0,   0,   0,  81,   4, 
      0,   0,   0,   3, 255, 255, 
     21,   0,   0,   0,  28,   0, 
      0,   0,  16,   1,   0,   0, 
     74,   4,   0,   0, 192,   1, 
      0,   0,   3,   0,   2,   0, 
      1,   0,  10,   0, 220,   1, 
      0,   0,   0,   0,   0,   0, 
    236,   1,   0,   0,   3,   0, 
      5,   0,   1,   0,  22,   0, 
     12,   2,   0,   0,   0,   0, 
      0,   0,  28,   2,   0,   0, 
      3,   0,   4,   0,   1,   0, 
     18,   0,  56,   2,   0,   0, 
      0,   0,   0,   0,  72,   2, 
      0,   0,   3,   0,   6,   0, 
      1,   0,  26,   0, 112,   2, 
      0,   0,   0,   0,   0,   0, 
    128,   2,   0,   0,   3,   0, 
      0,   0,   1,   0,   2,   0, 
    156,   2,   0,   0,   0,   0, 
      0,   0, 172,   2,   0,   0, 
      3,   0,   1,   0,   1,   0, 
      6,   0, 200,   2,   0,   0, 
      0,   0,   0,   0, 216,   2, 
      0,   0,   3,   0,   3,   0, 
      1,   0,  14,   0, 248,   2, 
      0,   0,   0,   0,   0,   0, 
      8,   3,   0,   0,   2,   0, 
      5,   0,   1,   0,  22,   0, 
     36,   3,   0,   0,   0,   0, 
      0,   0,  52,   3,   0,   0, 
      2,   0,   6,   0,   1,   0, 
     26,   0,  36,   3,   0,   0, 
      0,   0,   0,   0,  79,   3, 
      0,   0,   2,   0,  11,   0, 
      1,   0,  46,   0,  36,   3, 
      0,   0,   0,   0,   0,   0, 
     95,   3,   0,   0,   2,   0, 
     12,   0,   1,   0,  50,   0, 
     36,   3,   0,   0,   0,   0, 
      0,   0, 115,   3,   0,   0, 
      2,   0,  10,   0,   1,   0, 
     42,   0,  36,   3,   0,   0, 
      0,   0,   0,   0, 137,   3, 
      0,   0,   2,   0,   8,   0, 
      1,   0,  34,   0,  36,   3, 
      0,   0,   0,   0,   0,   0, 
    161,   3,   0,   0,   2,   0, 
      9,   0,   1,   0,  38,   0, 
     36,   3,   0,   0,   0,   0, 
      0,   0, 183,   3,   0,   0, 
      2,   0,   7,   0,   1,   0, 
     30,   0,  36,   3,   0,   0, 
      0,   0,   0,   0, 206,   3, 
      0,   0,   2,   0,   3,   0, 
      1,   0,  14,   0,  36,   3, 
      0,   0,   0,   0,   0,   0, 
    230,   3,   0,   0,   2,   0, 
      2,   0,   1,   0,  10,   0, 
     36,   3,   0,   0,   0,   0, 
      0,   0, 249,   3,   0,   0, 
      2,   0,   1,   0,   1,   0, 
      6,   0,  36,   3,   0,   0, 
      0,   0,   0,   0,  10,   4, 
      0,   0,   2,   0,   0,   0, 
      1,   0,   2,   0,  36,   3, 
      0,   0,   0,   0,   0,   0, 
     31,   4,   0,   0,   2,   0, 
      4,   0,   1,   0,  18,   0, 
     36,   3,   0,   0,   0,   0, 
      0,   0,  59,   4,   0,   0, 
      2,   0,  17,   0,   1,   0, 
     70,   0,  36,   3,   0,   0, 
      0,   0,   0,   0,  83,  97, 
    109, 112, 108, 101, 114,  95, 
    115,  97, 109, 112, 108, 101, 
    114,  95,  97, 108, 112, 104, 
     97,  84, 101, 120,   0, 171, 
    171, 171,   4,   0,  12,   0, 
      1,   0,   1,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
     83,  97, 109, 112, 108, 101, 
    114,  95, 115,  97, 109, 112, 
    108, 101, 114,  95,  98, 108, 
    101, 110, 100,  65, 108, 112, 
    104,  97,  84, 101, 120,   0, 
    171, 171,   4,   0,  12,   0, 
      1,   0,   1,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
     83,  97, 109, 112, 108, 101, 
    114,  95, 115,  97, 109, 112, 
    108, 101, 114,  95,  98, 108, 
    101, 110, 100,  84, 101, 120, 
      0, 171, 171, 171,   4,   0, 
     12,   0,   1,   0,   1,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,  83,  97, 109, 112, 
    108, 101, 114,  95, 115,  97, 
    109, 112, 108, 101, 114,  95, 
     98, 108, 101, 110, 100,  85, 
     86,  68, 105, 115, 116, 111, 
    114, 116, 105, 111, 110,  84, 
    101, 120,   0, 171, 171, 171, 
      4,   0,  12,   0,   1,   0, 
      1,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,  83,  97, 
    109, 112, 108, 101, 114,  95, 
    115,  97, 109, 112, 108, 101, 
    114,  95,  99, 111, 108, 111, 
    114,  84, 101, 120,   0, 171, 
    171, 171,   4,   0,  12,   0, 
      1,   0,   1,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
     83,  97, 109, 112, 108, 101, 
    114,  95, 115,  97, 109, 112, 
    108, 101, 114,  95, 110, 111, 
    114, 109,  97, 108,  84, 101, 
    120,   0, 171, 171,   4,   0, 
     12,   0,   1,   0,   1,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,  83,  97, 109, 112, 
    108, 101, 114,  95, 115,  97, 
    109, 112, 108, 101, 114,  95, 
    117, 118,  68, 105, 115, 116, 
    111, 114, 116, 105, 111, 110, 
     84, 101, 120,   0,   4,   0, 
     12,   0,   1,   0,   1,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,  95,  50,  56,  53, 
     95, 102,  66, 108, 101, 110, 
    100,  84, 101, 120, 116, 117, 
    114, 101,  80,  97, 114,  97, 
    109, 101, 116, 101, 114,   0, 
      1,   0,   3,   0,   1,   0, 
      4,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,  95,  50, 
     56,  53,  95, 102,  67,  97, 
    109, 101, 114,  97,  70, 114, 
    111, 110, 116,  68, 105, 114, 
    101,  99, 116, 105, 111, 110, 
      0,  95,  50,  56,  53,  95, 
    102,  69, 100, 103, 101,  67, 
    111, 108, 111, 114,   0,  95, 
     50,  56,  53,  95, 102,  69, 
    100, 103, 101,  80,  97, 114, 
     97, 109, 101, 116, 101, 114, 
      0,  95,  50,  56,  53,  95, 
    102,  69, 109, 105, 115, 115, 
    105, 118, 101,  83,  99,  97, 
    108, 105, 110, 103,   0,  95, 
     50,  56,  53,  95, 102,  70, 
     97, 108, 108, 111, 102, 102, 
     66, 101, 103, 105, 110,  67, 
    111, 108, 111, 114,   0,  95, 
     50,  56,  53,  95, 102,  70, 
     97, 108, 108, 111, 102, 102, 
     69, 110, 100,  67, 111, 108, 
    111, 114,   0,  95,  50,  56, 
     53,  95, 102,  70,  97, 108, 
    108, 111, 102, 102,  80,  97, 
    114,  97, 109, 101, 116, 101, 
    114,   0,  95,  50,  56,  53, 
     95, 102,  70, 108, 105, 112, 
     98, 111, 111, 107,  80,  97, 
    114,  97, 109, 101, 116, 101, 
    114,   0,  95,  50,  56,  53, 
     95, 102,  76, 105, 103, 104, 
    116,  65, 109,  98, 105, 101, 
    110, 116,   0,  95,  50,  56, 
     53,  95, 102,  76, 105, 103, 
    104, 116,  67, 111, 108, 111, 
    114,   0,  95,  50,  56,  53, 
     95, 102,  76, 105, 103, 104, 
    116,  68, 105, 114, 101,  99, 
    116, 105, 111, 110,   0,  95, 
     50,  56,  53,  95, 102,  85, 
     86,  68, 105, 115, 116, 111, 
    114, 116, 105, 111, 110,  80, 
     97, 114,  97, 109, 101, 116, 
    101, 114,   0,  95,  50,  56, 
     53,  95, 109, 105, 115,  99, 
     70, 108,  97, 103, 115,   0, 
    112, 115,  95,  51,  95,  48, 
      0,  77, 105,  99, 114, 111, 
    115, 111, 102, 116,  32,  40, 
     82,  41,  32,  72,  76,  83, 
     76,  32,  83, 104,  97, 100, 
    101, 114,  32,  67, 111, 109, 
    112, 105, 108, 101, 114,  32, 
     57,  46,  50,  57,  46,  57, 
     53,  50,  46,  51,  49,  49, 
     49,   0, 171, 171,  81,   0, 
      0,   5,  13,   0,  15, 160, 
      0,   0,   0,  64,   0,   0, 
    128, 191,   0,   0,   0,  52, 
     85,  85, 213,  62,  81,   0, 
      0,   5,  14,   0,  15, 160, 
     61,  10, 135,  63, 174,  71, 
     97, 189,   0,   0,   0,   0, 
      0,   0,   0, 191,  81,   0, 
      0,   5,  15,   0,  15, 160, 
      0,   0, 128, 191,   0,   0, 
      0, 192,   0,   0,  64, 192, 
      0,   0,   0, 128,  81,   0, 
      0,   5,  16,   0,  15, 160, 
     18,  81, 156,  62, 196, 162, 
     46,  63, 194,  44,  77,  60, 
      0,   0,   0,   0,  31,   0, 
      0,   2,   5,   0,   0, 128, 
      0,   0,  79, 144,  31,   0, 
      0,   2,   5,   0,   1, 128, 
      1,   0,  79, 144,  31,   0, 
      0,   2,   5,   0,   2, 128, 
      2,   0,   7, 144,  31,   0, 
      0,   2,   5,   0,   3, 128, 
      3,   0,   7, 144,  31,   0, 
      0,   2,   5,   0,   4, 128, 
      4,   0,   7, 144,  31,   0, 
      0,   2,   5,   0,   5, 128, 
      5,   0,  15, 144,  31,   0, 
      0,   2,   5,   0,   6, 128, 
      6,   0,  15, 144,  31,   0, 
      0,   2,   5,   0,   7, 128, 
      7,   0,  15, 144,  31,   0, 
      0,   2,   0,   0,   0, 144, 
      0,   8,  15, 160,  31,   0, 
      0,   2,   0,   0,   0, 144, 
      1,   8,  15, 160,  31,   0, 
      0,   2,   0,   0,   0, 144, 
      2,   8,  15, 160,  31,   0, 
      0,   2,   0,   0,   0, 144, 
      3,   8,  15, 160,  31,   0, 
      0,   2,   0,   0,   0, 144, 
      4,   8,  15, 160,  31,   0, 
      0,   2,   0,   0,   0, 144, 
      5,   8,  15, 160,  31,   0, 
      0,   2,   0,   0,   0, 144, 
      6,   8,  15, 160,  66,   0, 
      0,   3,   0,   0,  15, 128, 
      5,   0, 238, 144,   3,   8, 
    228, 160,   4,   0,   0,   4, 
      0,   0,   3, 128,   0,   0, 
    228, 128,  13,   0,   0, 160, 
     13,   0,  85, 160,   4,   0, 
      0,   4,   0,   0,   4, 128, 
      0,   0,  85, 128,   4,   0, 
    255, 161,   4,   0, 170, 160, 
      4,   0,   0,   4,   0,   0, 
     10, 128,   0,   0, 160, 128, 
      4,   0,   0, 160,   1,   0, 
     96, 144,  66,   0,   0,   3, 
      1,   0,  15, 128,   0,   0, 
    237, 128,   0,   8, 228, 160, 
     11,   0,   0,   3,   2,   0, 
      7, 128,   1,   0, 228, 139, 
     13,   0, 170, 160,  15,   0, 
      0,   2,   3,   0,   1, 128, 
      2,   0,   0, 128,  15,   0, 
      0,   2,   3,   0,   2, 128, 
      2,   0,  85, 128,  15,   0, 
      0,   2,   3,   0,   4, 128, 
      2,   0, 170, 128,   5,   0, 
      0,   3,   2,   0,   7, 128, 
      3,   0, 228, 128,  13,   0, 
    255, 160,  14,   0,   0,   2, 
      3,   0,   1, 128,   2,   0, 
      0, 128,  14,   0,   0,   2, 
      3,   0,   2, 128,   2,   0, 
     85, 128,  14,   0,   0,   2, 
      3,   0,   4, 128,   2,   0, 
    170, 128,   4,   0,   0,   4, 
      2,   0,   7, 128,   3,   0, 
    228, 128,  14,   0,   0, 160, 
     14,   0,  85, 160,  11,   0, 
      0,   3,   1,   0,   7, 128, 
      2,   0, 228, 128,  14,   0, 
    170, 160,   5,   0,   0,   3, 
      1,   0,  15, 128,   1,   0, 
    228, 128,   0,   0, 228, 144, 
     66,   0,   0,   3,   2,   0, 
     15, 128,   0,   0, 237, 128, 
      1,   8, 228, 160,   2,   0, 
      0,   3,   2,   0,   7, 128, 
      2,   0, 228, 128,  14,   0, 
    255, 160,   2,   0,   0,   3, 
      2,   0,   7, 128,   2,   0, 
    228, 128,   2,   0, 228, 128, 
      5,   0,   0,   3,   3,   0, 
      7, 128,   2,   0,  85, 128, 
      3,   0, 228, 144,   4,   0, 
      0,   4,   2,   0,  11, 128, 
      2,   0,   0, 128,   4,   0, 
    164, 144,   3,   0, 164, 128, 
      4,   0,   0,   4,   2,   0, 
      7, 128,   2,   0, 170, 128, 
      2,   0, 228, 144,   2,   0, 
    244, 128,  36,   0,   0,   2, 
      3,   0,   7, 128,   2,   0, 
    228, 128,   4,   0,   0,   4, 
      0,   0,  10, 128,   0,   0, 
    160, 128,   4,   0,   0, 160, 
      7,   0, 232, 144,  66,   0, 
      0,   3,   2,   0,  15, 128, 
      0,   0, 237, 128,   0,   8, 
    228, 160,   1,   0,   0,   2, 
      4,   0,   3, 128,  13,   0, 
    228, 160,   2,   0,   0,   3, 
      0,   0,   2, 128,   4,   0, 
     85, 128,   3,   0,  85, 160, 
      4,   0,   0,   4,   2,   0, 
     15, 128,   2,   0, 228, 128, 
      0,   0, 228, 144,   1,   0, 
    228, 129,   4,   0,   0,   4, 
      2,   0,  15, 128,   1,   0, 
    170, 144,   2,   0, 228, 128, 
      1,   0, 228, 128,  88,   0, 
      0,   4,   2,   0,  15, 128, 
      0,   0,  85, 140,   2,   0, 
    228, 128,   1,   0, 228, 128, 
     88,   0,   0,   4,   1,   0, 
     15, 128,   3,   0,   0, 161, 
      1,   0, 228, 128,   2,   0, 
    228, 128,   4,   0,   0,   4, 
      0,   0,   3, 128,   0,   0, 
    232, 128,   4,   0,   0, 160, 
      5,   0, 228, 144,  66,   0, 
      0,   3,   0,   0,  15, 128, 
      0,   0, 228, 128,   2,   8, 
    228, 160,   5,   0,   0,   3, 
      0,   0,   1, 128,   0,   0, 
    255, 128,   0,   0,   0, 128, 
      5,   0,   0,   3,   0,   0, 
      8, 128,   0,   0,   0, 128, 
      1,   0, 255, 128,  66,   0, 
      0,   3,   2,   0,  15, 128, 
      6,   0, 238, 144,   6,   8, 
    228, 160,   4,   0,   0,   4, 
      2,   0,   3, 128,   2,   0, 
    228, 128,  13,   0,   0, 160, 
     13,   0,  85, 160,   4,   0, 
      0,   4,   2,   0,   4, 128, 
      2,   0,  85, 128,   4,   0, 
    255, 161,   4,   0, 170, 160, 
      4,   0,   0,   4,   2,   0, 
     10, 128,   2,   0, 160, 128, 
      4,   0,  85, 160,   7,   0, 
     96, 144,  66,   0,   0,   3, 
      5,   0,  15, 128,   2,   0, 
    237, 128,   4,   8, 228, 160, 
     11,   0,   0,   3,   6,   0, 
      7, 128,   5,   0, 228, 139, 
     13,   0, 170, 160,  15,   0, 
      0,   2,   5,   0,   1, 128, 
      6,   0,   0, 128,  15,   0, 
      0,   2,   5,   0,   2, 128, 
      6,   0,  85, 128,  15,   0, 
      0,   2,   5,   0,   4, 128, 
      6,   0, 170, 128,   5,   0, 
      0,   3,   5,   0,   7, 128, 
      5,   0, 228, 128,  13,   0, 
    255, 160,  14,   0,   0,   2, 
      6,   0,   1, 128,   5,   0, 
      0, 128,  14,   0,   0,   2, 
      6,   0,   2, 128,   5,   0, 
     85, 128,  14,   0,   0,   2, 
      6,   0,   4, 128,   5,   0, 
    170, 128,   4,   0,   0,   4, 
      5,   0,   7, 128,   6,   0, 
    228, 128,  14,   0,   0, 160, 
     14,   0,  85, 160,   4,   0, 
      0,   4,   2,   0,   3, 128, 
      2,   0, 232, 128,   4,   0, 
     85, 160,   6,   0, 228, 144, 
     66,   0,   0,   3,   2,   0, 
     15, 128,   2,   0, 228, 128, 
      5,   8, 228, 160,   5,   0, 
      0,   3,   1,   0,   8, 128, 
      2,   0, 255, 128,   2,   0, 
      0, 128,   5,   0,   0,   3, 
      2,   0,   1, 128,   1,   0, 
    255, 128,   5,   0, 255, 128, 
     35,   0,   0,   2,   2,   0, 
      2, 128,   5,   0,   0, 160, 
      5,   0,   0,   3,   2,   0, 
     13, 128,   2,   0,   0, 128, 
      5,   0, 148, 128,  88,   0, 
      0,   4,   2,   0,  13, 128, 
      5,   0, 148, 128,   2,   0, 
    228, 128,  14,   0, 170, 160, 
      4,   0,   0,   4,   1,   0, 
      8, 128,   5,   0, 255, 128, 
      1,   0, 255, 129,  13,   0, 
     85, 161,   4,   0,   0,   4, 
      5,   0,   7, 128,   1,   0, 
    228, 128,   1,   0, 255, 128, 
      2,   0, 248, 128,   1,   0, 
      0,   2,   6,   0,   7, 128, 
     15,   0, 228, 160,   2,   0, 
      0,   3,   6,   0,   7, 128, 
      6,   0, 228, 128,   5,   0, 
      0, 160,   2,   0,   0,   3, 
      7,   0,   7, 128,   1,   0, 
    228, 128,   2,   0, 248, 128, 
      2,   0,   0,   3,   8,   0, 
      7, 128,   1,   0, 228, 128, 
      2,   0, 248, 129,   5,   0, 
      0,   3,   2,   0,  13, 128, 
      1,   0, 148, 128,   2,   0, 
    228, 128,  88,   0,   0,   4, 
      1,   0,   7, 128,   6,   0, 
    170, 140,   2,   0, 248, 128, 
      1,   0, 228, 128,  88,   0, 
      0,   4,   1,   0,   7, 128, 
      6,   0,  85, 140,   8,   0, 
    228, 128,   1,   0, 228, 128, 
     88,   0,   0,   4,   1,   0, 
      7, 128,   6,   0,   0, 140, 
      7,   0, 228, 128,   1,   0, 
    228, 128,  88,   0,   0,   4, 
      1,   0,   7, 128,   2,   0, 
     85, 129,   5,   0, 228, 128, 
      1,   0, 228, 128,   8,   0, 
      0,   3,   1,   0,   8, 128, 
      0,   0, 228, 160,   3,   0, 
    228, 128,   5,   0,   0,   3, 
      2,   0,   7, 128,   1,   0, 
    255, 128,   1,   0, 228, 160, 
     88,   0,   0,   4,   2,   0, 
      7, 128,   1,   0, 255, 128, 
      2,   0, 228, 128,  14,   0, 
    170, 160,   2,   0,   0,   3, 
      2,   0,   7, 128,   2,   0, 
    228, 128,   2,   0, 228, 160, 
      5,   0,   0,   3,   0,   0, 
      7, 128,   1,   0, 228, 128, 
      2,   0, 228, 128,   2,   0, 
      0,   3,   4,   0,   7, 128, 
      4,   0, 197, 140,   7,   0, 
    212, 160,  36,   0,   0,   2, 
      5,   0,   7, 128,   6,   0, 
    228, 161,   8,   0,   0,   3, 
      1,   0,  24, 128,   5,   0, 
    228, 128,   3,   0, 228, 128, 
     32,   0,   0,   3,   2,   0, 
      8, 128,   1,   0, 255, 128, 
      7,   0, 170, 160,   1,   0, 
      0,   2,   3,   0,  15, 128, 
      9,   0, 228, 160,   2,   0, 
      0,   3,   3,   0,  15, 128, 
      3,   0, 228, 129,   8,   0, 
    228, 160,   4,   0,   0,   4, 
      3,   0,  15, 128,   2,   0, 
    255, 128,   3,   0, 228, 128, 
      9,   0, 228, 160,  35,   0, 
      0,   2,   1,   0,   8, 128, 
      7,   0,  85, 160,   4,   0, 
      0,   4,   5,   0,   7, 128, 
      1,   0, 228, 128,   2,   0, 
    228, 128,   3,   0, 228, 128, 
      4,   0,   0,   4,   1,   0, 
      7, 128,   1,   0, 228, 128, 
      2,   0, 228, 128,   3,   0, 
    228, 129,   5,   0,   0,   3, 
      2,   0,   7, 128,   0,   0, 
    228, 128,   3,   0, 228, 128, 
     88,   0,   0,   4,   2,   0, 
      7, 128,   4,   0, 170, 140, 
      2,   0, 228, 128,   0,   0, 
    228, 128,  88,   0,   0,   4, 
      1,   0,   7, 128,   4,   0, 
     85, 140,   1,   0, 228, 128, 
      2,   0, 228, 128,  88,   0, 
      0,   4,   1,   0,   7, 128, 
      1,   0, 255, 129,   5,   0, 
    228, 128,   1,   0, 228, 128, 
      5,   0,   0,   3,   1,   0, 
      8, 128,   0,   0, 255, 128, 
      3,   0, 255, 128,  88,   0, 
      0,   4,   0,   0,  15, 128, 
      4,   0,   0, 140,   1,   0, 
    228, 128,   0,   0, 228, 128, 
     11,   0,   0,   3,   1,   0, 
      1, 128,   1,   0, 255, 144, 
     14,   0, 170, 160,   2,   0, 
      0,   3,   1,   0,   1, 128, 
      0,   0, 255, 129,   1,   0, 
      0, 128,  88,   0,   0,   4, 
      1,   0,  15, 128,   1,   0, 
      0, 128,  15,   0,   0, 160, 
     15,   0, 255, 160,  65,   0, 
      0,   1,   1,   0,  15, 128, 
      1,   0,   0,   2,   1,   0, 
      2, 128,  12,   0,  85, 160, 
      5,   0,   0,   3,   1,   0, 
      7, 128,   1,   0,  85, 128, 
     11,   0, 228, 160,   2,   0, 
      0,   3,   1,   0,   8, 128, 
      0,   0, 255, 128,   1,   0, 
    255, 145,   2,   0,   0,   3, 
      1,   0,   8, 128,   1,   0, 
    255, 128,  12,   0,   0, 161, 
     19,   0,   0,   2,   2,   0, 
      1, 128,   1,   0, 255, 129, 
      2,   0,   0,   3,   1,   0, 
      8, 128,   1,   0, 255, 128, 
      2,   0,   0, 128,   4,   0, 
      0,   4,   0,   0,   7, 128, 
      0,   0, 228, 128,  10,   0, 
      0, 160,   1,   0, 228, 129, 
      4,   0,   0,   4,   0,   0, 
      7, 128,   1,   0, 255, 128, 
      0,   0, 228, 128,   1,   0, 
    228, 128,  35,   0,   0,   2, 
      1,   0,   1, 128,  17,   0, 
      0, 160,   4,   0,   0,   4, 
      1,   0,  14, 128,   0,   0, 
    144, 128,  16,   0,   0, 160, 
     16,   0,  85, 160,   4,   0, 
      0,   4,   1,   0,  14, 128, 
      0,   0, 144, 128,   1,   0, 
    228, 128,  16,   0, 170, 160, 
      5,   0,   0,   3,   1,   0, 
     14, 128,   0,   0, 144, 128, 
      1,   0, 228, 128,  88,   0, 
      0,   4,   0,   8,   7, 128, 
      1,   0,   0, 129,   0,   0, 
    228, 128,   1,   0, 249, 128, 
      1,   0,   0,   2,   0,   8, 
      8, 128,   0,   0, 255, 128, 
    255, 255,   0,   0
};
