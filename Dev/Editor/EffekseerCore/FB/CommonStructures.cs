// <auto-generated>
//  automatically generated by the FlatBuffers compiler, do not modify
// </auto-generated>

namespace Effekseer.FB
{

using global::System;
using global::System.Collections.Generic;
using global::FlatBuffers;

public enum EffectNodeType : int
{
  EffectNodeType_Root = -1,
  EffectNodeType_NoneType = 0,
  EffectNodeType_Sprite = 2,
  EffectNodeType_Ribbon = 3,
  EffectNodeType_Ring = 4,
  EffectNodeType_Model = 5,
  EffectNodeType_Track = 6,
};

public enum TranslationParentBindType : int
{
  TranslationParentBindType_NotBind = 0,
  TranslationParentBindType_WhenCreating = 1,
  TranslationParentBindType_Always = 2,
  TranslationParentBindType_NotBind_Root = 3,
  TranslationParentBindType_NotBind_FollowParent = 4,
  TranslationParentBindType_WhenCreating_FollowParent = 5,
};

public enum BindType : int
{
  BindType_NotBind = 0,
  BindType_WhenCreating = 1,
  BindType_Always = 2,
  BindType_NotBind_Root = 3,
};

public struct IntRange : IFlatbufferObject
{
  private Struct __p;
  public ByteBuffer ByteBuffer { get { return __p.bb; } }
  public void __init(int _i, ByteBuffer _bb) { __p = new Struct(_i, _bb); }
  public IntRange __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public int Min { get { return __p.bb.GetInt(__p.bb_pos + 0); } }
  public int Max { get { return __p.bb.GetInt(__p.bb_pos + 4); } }

  public static Offset<Effekseer.FB.IntRange> CreateIntRange(FlatBufferBuilder builder, int Min, int Max) {
    builder.Prep(4, 8);
    builder.PutInt(Max);
    builder.PutInt(Min);
    return new Offset<Effekseer.FB.IntRange>(builder.Offset);
  }
};

public struct FloatRange : IFlatbufferObject
{
  private Struct __p;
  public ByteBuffer ByteBuffer { get { return __p.bb; } }
  public void __init(int _i, ByteBuffer _bb) { __p = new Struct(_i, _bb); }
  public FloatRange __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public float Min { get { return __p.bb.GetFloat(__p.bb_pos + 0); } }
  public float Max { get { return __p.bb.GetFloat(__p.bb_pos + 4); } }

  public static Offset<Effekseer.FB.FloatRange> CreateFloatRange(FlatBufferBuilder builder, float Min, float Max) {
    builder.Prep(4, 8);
    builder.PutFloat(Max);
    builder.PutFloat(Min);
    return new Offset<Effekseer.FB.FloatRange>(builder.Offset);
  }
};

public struct Vec3F : IFlatbufferObject
{
  private Struct __p;
  public ByteBuffer ByteBuffer { get { return __p.bb; } }
  public void __init(int _i, ByteBuffer _bb) { __p = new Struct(_i, _bb); }
  public Vec3F __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public float X { get { return __p.bb.GetFloat(__p.bb_pos + 0); } }
  public float Y { get { return __p.bb.GetFloat(__p.bb_pos + 4); } }
  public float Z { get { return __p.bb.GetFloat(__p.bb_pos + 8); } }

  public static Offset<Effekseer.FB.Vec3F> CreateVec3F(FlatBufferBuilder builder, float X, float Y, float Z) {
    builder.Prep(4, 12);
    builder.PutFloat(Z);
    builder.PutFloat(Y);
    builder.PutFloat(X);
    return new Offset<Effekseer.FB.Vec3F>(builder.Offset);
  }
};

public struct Vec3FRange : IFlatbufferObject
{
  private Struct __p;
  public ByteBuffer ByteBuffer { get { return __p.bb; } }
  public void __init(int _i, ByteBuffer _bb) { __p = new Struct(_i, _bb); }
  public Vec3FRange __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public Effekseer.FB.Vec3F Min { get { return (new Effekseer.FB.Vec3F()).__assign(__p.bb_pos + 0, __p.bb); } }
  public Effekseer.FB.Vec3F Max { get { return (new Effekseer.FB.Vec3F()).__assign(__p.bb_pos + 12, __p.bb); } }

  public static Offset<Effekseer.FB.Vec3FRange> CreateVec3FRange(FlatBufferBuilder builder, float min_X, float min_Y, float min_Z, float max_X, float max_Y, float max_Z) {
    builder.Prep(4, 24);
    builder.Prep(4, 12);
    builder.PutFloat(max_Z);
    builder.PutFloat(max_Y);
    builder.PutFloat(max_X);
    builder.Prep(4, 12);
    builder.PutFloat(min_Z);
    builder.PutFloat(min_Y);
    builder.PutFloat(min_X);
    return new Offset<Effekseer.FB.Vec3FRange>(builder.Offset);
  }
};

public struct Vec3FEasing : IFlatbufferObject
{
  private Struct __p;
  public ByteBuffer ByteBuffer { get { return __p.bb; } }
  public void __init(int _i, ByteBuffer _bb) { __p = new Struct(_i, _bb); }
  public Vec3FEasing __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public Effekseer.FB.Vec3FRange Start { get { return (new Effekseer.FB.Vec3FRange()).__assign(__p.bb_pos + 0, __p.bb); } }
  public Effekseer.FB.Vec3FRange End { get { return (new Effekseer.FB.Vec3FRange()).__assign(__p.bb_pos + 24, __p.bb); } }
  public float EasingA { get { return __p.bb.GetFloat(__p.bb_pos + 48); } }
  public float EasingB { get { return __p.bb.GetFloat(__p.bb_pos + 52); } }
  public float EasingC { get { return __p.bb.GetFloat(__p.bb_pos + 56); } }

  public static Offset<Effekseer.FB.Vec3FEasing> CreateVec3FEasing(FlatBufferBuilder builder, float start_min_X, float start_min_Y, float start_min_Z, float start_max_X, float start_max_Y, float start_max_Z, float end_min_X, float end_min_Y, float end_min_Z, float end_max_X, float end_max_Y, float end_max_Z, float EasingA, float EasingB, float EasingC) {
    builder.Prep(4, 60);
    builder.PutFloat(EasingC);
    builder.PutFloat(EasingB);
    builder.PutFloat(EasingA);
    builder.Prep(4, 24);
    builder.Prep(4, 12);
    builder.PutFloat(end_max_Z);
    builder.PutFloat(end_max_Y);
    builder.PutFloat(end_max_X);
    builder.Prep(4, 12);
    builder.PutFloat(end_min_Z);
    builder.PutFloat(end_min_Y);
    builder.PutFloat(end_min_X);
    builder.Prep(4, 24);
    builder.Prep(4, 12);
    builder.PutFloat(start_max_Z);
    builder.PutFloat(start_max_Y);
    builder.PutFloat(start_max_X);
    builder.Prep(4, 12);
    builder.PutFloat(start_min_Z);
    builder.PutFloat(start_min_Y);
    builder.PutFloat(start_min_X);
    return new Offset<Effekseer.FB.Vec3FEasing>(builder.Offset);
  }
};

public struct RefMinMax : IFlatbufferObject
{
  private Table __p;
  public ByteBuffer ByteBuffer { get { return __p.bb; } }
  public static void ValidateVersion() { FlatBufferConstants.FLATBUFFERS_2_0_0(); }
  public static RefMinMax GetRootAsRefMinMax(ByteBuffer _bb) { return GetRootAsRefMinMax(_bb, new RefMinMax()); }
  public static RefMinMax GetRootAsRefMinMax(ByteBuffer _bb, RefMinMax obj) { return (obj.__assign(_bb.GetInt(_bb.Position) + _bb.Position, _bb)); }
  public void __init(int _i, ByteBuffer _bb) { __p = new Table(_i, _bb); }
  public RefMinMax __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public int Min { get { int o = __p.__offset(4); return o != 0 ? __p.bb.GetInt(o + __p.bb_pos) : (int)-1; } }
  public int Max { get { int o = __p.__offset(6); return o != 0 ? __p.bb.GetInt(o + __p.bb_pos) : (int)-1; } }

  public static Offset<Effekseer.FB.RefMinMax> CreateRefMinMax(FlatBufferBuilder builder,
      int min = -1,
      int max = -1) {
    builder.StartTable(2);
    RefMinMax.AddMax(builder, max);
    RefMinMax.AddMin(builder, min);
    return RefMinMax.EndRefMinMax(builder);
  }

  public static void StartRefMinMax(FlatBufferBuilder builder) { builder.StartTable(2); }
  public static void AddMin(FlatBufferBuilder builder, int min) { builder.AddInt(0, min, -1); }
  public static void AddMax(FlatBufferBuilder builder, int max) { builder.AddInt(1, max, -1); }
  public static Offset<Effekseer.FB.RefMinMax> EndRefMinMax(FlatBufferBuilder builder) {
    int o = builder.EndTable();
    return new Offset<Effekseer.FB.RefMinMax>(o);
  }
};

public struct TextureProperty : IFlatbufferObject
{
  private Table __p;
  public ByteBuffer ByteBuffer { get { return __p.bb; } }
  public static void ValidateVersion() { FlatBufferConstants.FLATBUFFERS_2_0_0(); }
  public static TextureProperty GetRootAsTextureProperty(ByteBuffer _bb) { return GetRootAsTextureProperty(_bb, new TextureProperty()); }
  public static TextureProperty GetRootAsTextureProperty(ByteBuffer _bb, TextureProperty obj) { return (obj.__assign(_bb.GetInt(_bb.Position) + _bb.Position, _bb)); }
  public void __init(int _i, ByteBuffer _bb) { __p = new Table(_i, _bb); }
  public TextureProperty __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public string Path { get { int o = __p.__offset(4); return o != 0 ? __p.__string(o + __p.bb_pos) : null; } }
#if ENABLE_SPAN_T
  public Span<byte> GetPathBytes() { return __p.__vector_as_span<byte>(4, 1); }
#else
  public ArraySegment<byte>? GetPathBytes() { return __p.__vector_as_arraysegment(4); }
#endif
  public byte[] GetPathArray() { return __p.__vector_as_array<byte>(4); }

  public static Offset<Effekseer.FB.TextureProperty> CreateTextureProperty(FlatBufferBuilder builder,
      StringOffset pathOffset = default(StringOffset)) {
    builder.StartTable(1);
    TextureProperty.AddPath(builder, pathOffset);
    return TextureProperty.EndTextureProperty(builder);
  }

  public static void StartTextureProperty(FlatBufferBuilder builder) { builder.StartTable(1); }
  public static void AddPath(FlatBufferBuilder builder, StringOffset pathOffset) { builder.AddOffset(0, pathOffset.Value, 0); }
  public static Offset<Effekseer.FB.TextureProperty> EndTextureProperty(FlatBufferBuilder builder) {
    int o = builder.EndTable();
    return new Offset<Effekseer.FB.TextureProperty>(o);
  }
};


}
