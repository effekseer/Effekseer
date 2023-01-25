// <auto-generated>
//  automatically generated by the FlatBuffers compiler, do not modify
// </auto-generated>

namespace Effekseer.FB
{

using global::System;
using global::System.Collections.Generic;
using global::FlatBuffers;

public struct Node : IFlatbufferObject
{
  private Table __p;
  public ByteBuffer ByteBuffer { get { return __p.bb; } }
  public static void ValidateVersion() { FlatBufferConstants.FLATBUFFERS_2_0_0(); }
  public static Node GetRootAsNode(ByteBuffer _bb) { return GetRootAsNode(_bb, new Node()); }
  public static Node GetRootAsNode(ByteBuffer _bb, Node obj) { return (obj.__assign(_bb.GetInt(_bb.Position) + _bb.Position, _bb)); }
  public void __init(int _i, ByteBuffer _bb) { __p = new Table(_i, _bb); }
  public Node __assign(int _i, ByteBuffer _bb) { __init(_i, _bb); return this; }

  public Effekseer.FB.EffectNodeType Type { get { int o = __p.__offset(4); return o != 0 ? (Effekseer.FB.EffectNodeType)__p.bb.GetInt(o + __p.bb_pos) : Effekseer.FB.EffectNodeType.EffectNodeType_NoneType; } }
  public bool IsRendered { get { int o = __p.__offset(6); return o != 0 ? 0!=__p.bb.Get(o + __p.bb_pos) : (bool)false; } }
  public Effekseer.FB.BasicSettings? BasicSettings { get { int o = __p.__offset(8); return o != 0 ? (Effekseer.FB.BasicSettings?)(new Effekseer.FB.BasicSettings()).__assign(__p.__indirect(o + __p.bb_pos), __p.bb) : null; } }
  public Effekseer.FB.PositionSettings? PositionSettings { get { int o = __p.__offset(10); return o != 0 ? (Effekseer.FB.PositionSettings?)(new Effekseer.FB.PositionSettings()).__assign(__p.__indirect(o + __p.bb_pos), __p.bb) : null; } }
  public Effekseer.FB.Node? Children(int j) { int o = __p.__offset(12); return o != 0 ? (Effekseer.FB.Node?)(new Effekseer.FB.Node()).__assign(__p.__indirect(__p.__vector(o) + j * 4), __p.bb) : null; }
  public int ChildrenLength { get { int o = __p.__offset(12); return o != 0 ? __p.__vector_len(o) : 0; } }

  public static Offset<Effekseer.FB.Node> CreateNode(FlatBufferBuilder builder,
      Effekseer.FB.EffectNodeType type = Effekseer.FB.EffectNodeType.EffectNodeType_NoneType,
      bool is_rendered = false,
      Offset<Effekseer.FB.BasicSettings> basic_settingsOffset = default(Offset<Effekseer.FB.BasicSettings>),
      Offset<Effekseer.FB.PositionSettings> position_settingsOffset = default(Offset<Effekseer.FB.PositionSettings>),
      VectorOffset childrenOffset = default(VectorOffset)) {
    builder.StartTable(5);
    Node.AddChildren(builder, childrenOffset);
    Node.AddPositionSettings(builder, position_settingsOffset);
    Node.AddBasicSettings(builder, basic_settingsOffset);
    Node.AddType(builder, type);
    Node.AddIsRendered(builder, is_rendered);
    return Node.EndNode(builder);
  }

  public static void StartNode(FlatBufferBuilder builder) { builder.StartTable(5); }
  public static void AddType(FlatBufferBuilder builder, Effekseer.FB.EffectNodeType type) { builder.AddInt(0, (int)type, 0); }
  public static void AddIsRendered(FlatBufferBuilder builder, bool isRendered) { builder.AddBool(1, isRendered, false); }
  public static void AddBasicSettings(FlatBufferBuilder builder, Offset<Effekseer.FB.BasicSettings> basicSettingsOffset) { builder.AddOffset(2, basicSettingsOffset.Value, 0); }
  public static void AddPositionSettings(FlatBufferBuilder builder, Offset<Effekseer.FB.PositionSettings> positionSettingsOffset) { builder.AddOffset(3, positionSettingsOffset.Value, 0); }
  public static void AddChildren(FlatBufferBuilder builder, VectorOffset childrenOffset) { builder.AddOffset(4, childrenOffset.Value, 0); }
  public static VectorOffset CreateChildrenVector(FlatBufferBuilder builder, Offset<Effekseer.FB.Node>[] data) { builder.StartVector(4, data.Length, 4); for (int i = data.Length - 1; i >= 0; i--) builder.AddOffset(data[i].Value); return builder.EndVector(); }
  public static VectorOffset CreateChildrenVectorBlock(FlatBufferBuilder builder, Offset<Effekseer.FB.Node>[] data) { builder.StartVector(4, data.Length, 4); builder.Add(data); return builder.EndVector(); }
  public static void StartChildrenVector(FlatBufferBuilder builder, int numElems) { builder.StartVector(4, numElems, 4); }
  public static Offset<Effekseer.FB.Node> EndNode(FlatBufferBuilder builder) {
    int o = builder.EndTable();
    return new Offset<Effekseer.FB.Node>(o);
  }
  public NodeT UnPack() {
    var _o = new NodeT();
    this.UnPackTo(_o);
    return _o;
  }
  public void UnPackTo(NodeT _o) {
    _o.Type = this.Type;
    _o.IsRendered = this.IsRendered;
    _o.BasicSettings = this.BasicSettings.HasValue ? this.BasicSettings.Value.UnPack() : null;
    _o.PositionSettings = this.PositionSettings.HasValue ? this.PositionSettings.Value.UnPack() : null;
    _o.Children = new List<Effekseer.FB.NodeT>();
    for (var _j = 0; _j < this.ChildrenLength; ++_j) {_o.Children.Add(this.Children(_j).HasValue ? this.Children(_j).Value.UnPack() : null);}
  }
  public static Offset<Effekseer.FB.Node> Pack(FlatBufferBuilder builder, NodeT _o) {
    if (_o == null) return default(Offset<Effekseer.FB.Node>);
    var _basic_settings = _o.BasicSettings == null ? default(Offset<Effekseer.FB.BasicSettings>) : Effekseer.FB.BasicSettings.Pack(builder, _o.BasicSettings);
    var _position_settings = _o.PositionSettings == null ? default(Offset<Effekseer.FB.PositionSettings>) : Effekseer.FB.PositionSettings.Pack(builder, _o.PositionSettings);
    var _children = default(VectorOffset);
    if (_o.Children != null) {
      var __children = new Offset<Effekseer.FB.Node>[_o.Children.Count];
      for (var _j = 0; _j < __children.Length; ++_j) { __children[_j] = Effekseer.FB.Node.Pack(builder, _o.Children[_j]); }
      _children = CreateChildrenVector(builder, __children);
    }
    return CreateNode(
      builder,
      _o.Type,
      _o.IsRendered,
      _basic_settings,
      _position_settings,
      _children);
  }
};

public class NodeT
{
  public Effekseer.FB.EffectNodeType Type { get; set; }
  public bool IsRendered { get; set; }
  public Effekseer.FB.BasicSettingsT BasicSettings { get; set; }
  public Effekseer.FB.PositionSettingsT PositionSettings { get; set; }
  public List<Effekseer.FB.NodeT> Children { get; set; }

  public NodeT() {
    this.Type = Effekseer.FB.EffectNodeType.EffectNodeType_NoneType;
    this.IsRendered = false;
    this.BasicSettings = null;
    this.PositionSettings = null;
    this.Children = null;
  }
}


}
