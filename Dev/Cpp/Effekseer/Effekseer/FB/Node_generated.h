// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_NODE_EFFEKSEER_FB_H_
#define FLATBUFFERS_GENERATED_NODE_EFFEKSEER_FB_H_

#include "flatbuffers/flatbuffers.h"

#include "PositionSettings_generated.h"
#include "FCurve_generated.h"
#include "Easing_generated.h"
#include "BasicSettings_generated.h"
#include "CommonStructures_generated.h"

namespace Effekseer {
namespace FB {

struct Node;
struct NodeBuilder;

struct Node FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef NodeBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_TYPE = 4,
    VT_IS_RENDERED = 6,
    VT_BASIC_SETTINGS = 8,
    VT_POSITION_SETTINGS = 10,
    VT_CHILDREN = 12
  };
  Effekseer::FB::EffectNodeType type() const {
    return static_cast<Effekseer::FB::EffectNodeType>(GetField<int32_t>(VT_TYPE, 0));
  }
  bool is_rendered() const {
    return GetField<uint8_t>(VT_IS_RENDERED, 0) != 0;
  }
  const Effekseer::FB::BasicSettings *basic_settings() const {
    return GetPointer<const Effekseer::FB::BasicSettings *>(VT_BASIC_SETTINGS);
  }
  const Effekseer::FB::PositionSettings *position_settings() const {
    return GetPointer<const Effekseer::FB::PositionSettings *>(VT_POSITION_SETTINGS);
  }
  const flatbuffers::Vector<flatbuffers::Offset<Effekseer::FB::Node>> *children() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<Effekseer::FB::Node>> *>(VT_CHILDREN);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_TYPE) &&
           VerifyField<uint8_t>(verifier, VT_IS_RENDERED) &&
           VerifyOffset(verifier, VT_BASIC_SETTINGS) &&
           verifier.VerifyTable(basic_settings()) &&
           VerifyOffset(verifier, VT_POSITION_SETTINGS) &&
           verifier.VerifyTable(position_settings()) &&
           VerifyOffset(verifier, VT_CHILDREN) &&
           verifier.VerifyVector(children()) &&
           verifier.VerifyVectorOfTables(children()) &&
           verifier.EndTable();
  }
};

struct NodeBuilder {
  typedef Node Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_type(Effekseer::FB::EffectNodeType type) {
    fbb_.AddElement<int32_t>(Node::VT_TYPE, static_cast<int32_t>(type), 0);
  }
  void add_is_rendered(bool is_rendered) {
    fbb_.AddElement<uint8_t>(Node::VT_IS_RENDERED, static_cast<uint8_t>(is_rendered), 0);
  }
  void add_basic_settings(flatbuffers::Offset<Effekseer::FB::BasicSettings> basic_settings) {
    fbb_.AddOffset(Node::VT_BASIC_SETTINGS, basic_settings);
  }
  void add_position_settings(flatbuffers::Offset<Effekseer::FB::PositionSettings> position_settings) {
    fbb_.AddOffset(Node::VT_POSITION_SETTINGS, position_settings);
  }
  void add_children(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<Effekseer::FB::Node>>> children) {
    fbb_.AddOffset(Node::VT_CHILDREN, children);
  }
  explicit NodeBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<Node> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Node>(end);
    return o;
  }
};

inline flatbuffers::Offset<Node> CreateNode(
    flatbuffers::FlatBufferBuilder &_fbb,
    Effekseer::FB::EffectNodeType type = Effekseer::FB::EffectNodeType::EffectNodeType_NoneType,
    bool is_rendered = false,
    flatbuffers::Offset<Effekseer::FB::BasicSettings> basic_settings = 0,
    flatbuffers::Offset<Effekseer::FB::PositionSettings> position_settings = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<Effekseer::FB::Node>>> children = 0) {
  NodeBuilder builder_(_fbb);
  builder_.add_children(children);
  builder_.add_position_settings(position_settings);
  builder_.add_basic_settings(basic_settings);
  builder_.add_type(type);
  builder_.add_is_rendered(is_rendered);
  return builder_.Finish();
}

inline flatbuffers::Offset<Node> CreateNodeDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    Effekseer::FB::EffectNodeType type = Effekseer::FB::EffectNodeType::EffectNodeType_NoneType,
    bool is_rendered = false,
    flatbuffers::Offset<Effekseer::FB::BasicSettings> basic_settings = 0,
    flatbuffers::Offset<Effekseer::FB::PositionSettings> position_settings = 0,
    const std::vector<flatbuffers::Offset<Effekseer::FB::Node>> *children = nullptr) {
  auto children__ = children ? _fbb.CreateVector<flatbuffers::Offset<Effekseer::FB::Node>>(*children) : 0;
  return Effekseer::FB::CreateNode(
      _fbb,
      type,
      is_rendered,
      basic_settings,
      position_settings,
      children__);
}

}  // namespace FB
}  // namespace Effekseer

#endif  // FLATBUFFERS_GENERATED_NODE_EFFEKSEER_FB_H_
