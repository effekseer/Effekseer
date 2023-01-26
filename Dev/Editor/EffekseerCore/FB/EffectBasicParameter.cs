// <auto-generated>
//  automatically generated by the FlatBuffers compiler, do not modify
// </auto-generated>

namespace Effekseer.FB.Effect
{

using global::System;
using global::System.Collections.Generic;
using global::FlatBuffers;

public struct BasicParameter : IFlatbufferObject
{
  private Table __p;
  public ByteBuffer ByteBuffer { get { return __p.bb; } }
  public static void ValidateVersion() { FlatBufferConstants.FLATBUFFERS_2_0_0(); }
  public static BasicParameter GetRootAsBasicParameter(ByteBuffer _bb) { return GetRootAsBasicParameter(_bb, new BasicParameter()); }
  public static BasicParameter GetRootAsBasicParameter(ByteBuffer _bb, BasicParameter obj) { return (obj.__assign(_bb.GetInt(_bb.Position) + _bb.Position, _bb)); }
  public void __init(int _i, ByteBuffer _bb) { __p = new Table(_i, _bb); }
  public BasicParameter __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public Effekseer.FB.IntRef? MaxGeneration { get { int o = __p.__offset(4); return o != 0 ? (Effekseer.FB.IntRef?)(new Effekseer.FB.IntRef()).__assign(o + __p.bb_pos, __p.bb) : null; } }
  public Effekseer.FB.Effect.TranslationParentBindType TranslationBindType { get { int o = __p.__offset(6); return o != 0 ? (Effekseer.FB.Effect.TranslationParentBindType)__p.bb.GetInt(o + __p.bb_pos) : Effekseer.FB.Effect.TranslationParentBindType.TranslationParentBindType_Always; } }
  public Effekseer.FB.Effect.BindType RotationBindType { get { int o = __p.__offset(8); return o != 0 ? (Effekseer.FB.Effect.BindType)__p.bb.GetInt(o + __p.bb_pos) : Effekseer.FB.Effect.BindType.BindType_Always; } }
  public Effekseer.FB.Effect.BindType ScalingBindType { get { int o = __p.__offset(10); return o != 0 ? (Effekseer.FB.Effect.BindType)__p.bb.GetInt(o + __p.bb_pos) : Effekseer.FB.Effect.BindType.BindType_Always; } }
  public int RemoveWhenLifeIsExtinct { get { int o = __p.__offset(12); return o != 0 ? __p.bb.GetInt(o + __p.bb_pos) : (int)1; } }
  public int RemoveWhenParentIsRemoved { get { int o = __p.__offset(14); return o != 0 ? __p.bb.GetInt(o + __p.bb_pos) : (int)0; } }
  public int RemoveWhenChildrenIsExtinct { get { int o = __p.__offset(16); return o != 0 ? __p.bb.GetInt(o + __p.bb_pos) : (int)0; } }
  public Effekseer.FB.IntRange? Life { get { int o = __p.__offset(18); return o != 0 ? (Effekseer.FB.IntRange?)(new Effekseer.FB.IntRange()).__assign(o + __p.bb_pos, __p.bb) : null; } }
  public Effekseer.FB.FloatRange? GenerationTime { get { int o = __p.__offset(20); return o != 0 ? (Effekseer.FB.FloatRange?)(new Effekseer.FB.FloatRange()).__assign(o + __p.bb_pos, __p.bb) : null; } }
  public Effekseer.FB.FloatRange? GenerationTimeOffset { get { int o = __p.__offset(22); return o != 0 ? (Effekseer.FB.FloatRange?)(new Effekseer.FB.FloatRange()).__assign(o + __p.bb_pos, __p.bb) : null; } }

  public static Offset<Effekseer.FB.Effect.BasicParameter> CreateBasicParameter(FlatBufferBuilder builder,
      Effekseer.FB.IntRefT max_generation = null,
      Effekseer.FB.Effect.TranslationParentBindType translation_bind_type = Effekseer.FB.Effect.TranslationParentBindType.TranslationParentBindType_Always,
      Effekseer.FB.Effect.BindType rotation_bind_type = Effekseer.FB.Effect.BindType.BindType_Always,
      Effekseer.FB.Effect.BindType scaling_bind_type = Effekseer.FB.Effect.BindType.BindType_Always,
      int remove_when_life_is_extinct = 1,
      int remove_when_parent_is_removed = 0,
      int remove_when_children_is_extinct = 0,
      Effekseer.FB.IntRangeT life = null,
      Effekseer.FB.FloatRangeT generation_time = null,
      Effekseer.FB.FloatRangeT generation_time_offset = null) {
    builder.StartTable(10);
    BasicParameter.AddGenerationTimeOffset(builder, Effekseer.FB.FloatRange.Pack(builder, generation_time_offset));
    BasicParameter.AddGenerationTime(builder, Effekseer.FB.FloatRange.Pack(builder, generation_time));
    BasicParameter.AddLife(builder, Effekseer.FB.IntRange.Pack(builder, life));
    BasicParameter.AddRemoveWhenChildrenIsExtinct(builder, remove_when_children_is_extinct);
    BasicParameter.AddRemoveWhenParentIsRemoved(builder, remove_when_parent_is_removed);
    BasicParameter.AddRemoveWhenLifeIsExtinct(builder, remove_when_life_is_extinct);
    BasicParameter.AddScalingBindType(builder, scaling_bind_type);
    BasicParameter.AddRotationBindType(builder, rotation_bind_type);
    BasicParameter.AddTranslationBindType(builder, translation_bind_type);
    BasicParameter.AddMaxGeneration(builder, Effekseer.FB.IntRef.Pack(builder, max_generation));
    return BasicParameter.EndBasicParameter(builder);
  }

  public static void StartBasicParameter(FlatBufferBuilder builder) { builder.StartTable(10); }
  public static void AddMaxGeneration(FlatBufferBuilder builder, Offset<Effekseer.FB.IntRef> maxGenerationOffset) { builder.AddStruct(0, maxGenerationOffset.Value, 0); }
  public static void AddTranslationBindType(FlatBufferBuilder builder, Effekseer.FB.Effect.TranslationParentBindType translationBindType) { builder.AddInt(1, (int)translationBindType, 2); }
  public static void AddRotationBindType(FlatBufferBuilder builder, Effekseer.FB.Effect.BindType rotationBindType) { builder.AddInt(2, (int)rotationBindType, 2); }
  public static void AddScalingBindType(FlatBufferBuilder builder, Effekseer.FB.Effect.BindType scalingBindType) { builder.AddInt(3, (int)scalingBindType, 2); }
  public static void AddRemoveWhenLifeIsExtinct(FlatBufferBuilder builder, int removeWhenLifeIsExtinct) { builder.AddInt(4, removeWhenLifeIsExtinct, 1); }
  public static void AddRemoveWhenParentIsRemoved(FlatBufferBuilder builder, int removeWhenParentIsRemoved) { builder.AddInt(5, removeWhenParentIsRemoved, 0); }
  public static void AddRemoveWhenChildrenIsExtinct(FlatBufferBuilder builder, int removeWhenChildrenIsExtinct) { builder.AddInt(6, removeWhenChildrenIsExtinct, 0); }
  public static void AddLife(FlatBufferBuilder builder, Offset<Effekseer.FB.IntRange> lifeOffset) { builder.AddStruct(7, lifeOffset.Value, 0); }
  public static void AddGenerationTime(FlatBufferBuilder builder, Offset<Effekseer.FB.FloatRange> generationTimeOffset) { builder.AddStruct(8, generationTimeOffset.Value, 0); }
  public static void AddGenerationTimeOffset(FlatBufferBuilder builder, Offset<Effekseer.FB.FloatRange> generationTimeOffsetOffset) { builder.AddStruct(9, generationTimeOffsetOffset.Value, 0); }
  public static Offset<Effekseer.FB.Effect.BasicParameter> EndBasicParameter(FlatBufferBuilder builder) {
    int o = builder.EndTable();
    return new Offset<Effekseer.FB.Effect.BasicParameter>(o);
  }
  public BasicParameterT UnPack() {
    var _o = new BasicParameterT();
    this.UnPackTo(_o);
    return _o;
  }
  public void UnPackTo(BasicParameterT _o) {
    _o.MaxGeneration = this.MaxGeneration.HasValue ? this.MaxGeneration.Value.UnPack() : null;
    _o.TranslationBindType = this.TranslationBindType;
    _o.RotationBindType = this.RotationBindType;
    _o.ScalingBindType = this.ScalingBindType;
    _o.RemoveWhenLifeIsExtinct = this.RemoveWhenLifeIsExtinct;
    _o.RemoveWhenParentIsRemoved = this.RemoveWhenParentIsRemoved;
    _o.RemoveWhenChildrenIsExtinct = this.RemoveWhenChildrenIsExtinct;
    _o.Life = this.Life.HasValue ? this.Life.Value.UnPack() : null;
    _o.GenerationTime = this.GenerationTime.HasValue ? this.GenerationTime.Value.UnPack() : null;
    _o.GenerationTimeOffset = this.GenerationTimeOffset.HasValue ? this.GenerationTimeOffset.Value.UnPack() : null;
  }
  public static Offset<Effekseer.FB.Effect.BasicParameter> Pack(FlatBufferBuilder builder, BasicParameterT _o) {
    if (_o == null) return default(Offset<Effekseer.FB.Effect.BasicParameter>);
    return CreateBasicParameter(
      builder,
      _o.MaxGeneration,
      _o.TranslationBindType,
      _o.RotationBindType,
      _o.ScalingBindType,
      _o.RemoveWhenLifeIsExtinct,
      _o.RemoveWhenParentIsRemoved,
      _o.RemoveWhenChildrenIsExtinct,
      _o.Life,
      _o.GenerationTime,
      _o.GenerationTimeOffset);
  }
};

public class BasicParameterT
{
  public Effekseer.FB.IntRefT MaxGeneration { get; set; }
  public Effekseer.FB.Effect.TranslationParentBindType TranslationBindType { get; set; }
  public Effekseer.FB.Effect.BindType RotationBindType { get; set; }
  public Effekseer.FB.Effect.BindType ScalingBindType { get; set; }
  public int RemoveWhenLifeIsExtinct { get; set; }
  public int RemoveWhenParentIsRemoved { get; set; }
  public int RemoveWhenChildrenIsExtinct { get; set; }
  public Effekseer.FB.IntRangeT Life { get; set; }
  public Effekseer.FB.FloatRangeT GenerationTime { get; set; }
  public Effekseer.FB.FloatRangeT GenerationTimeOffset { get; set; }

  public BasicParameterT() {
    this.MaxGeneration = new Effekseer.FB.IntRefT();
    this.TranslationBindType = Effekseer.FB.Effect.TranslationParentBindType.TranslationParentBindType_Always;
    this.RotationBindType = Effekseer.FB.Effect.BindType.BindType_Always;
    this.ScalingBindType = Effekseer.FB.Effect.BindType.BindType_Always;
    this.RemoveWhenLifeIsExtinct = 1;
    this.RemoveWhenParentIsRemoved = 0;
    this.RemoveWhenChildrenIsExtinct = 0;
    this.Life = new Effekseer.FB.IntRangeT();
    this.GenerationTime = new Effekseer.FB.FloatRangeT();
    this.GenerationTimeOffset = new Effekseer.FB.FloatRangeT();
  }
}


}