
#pragma once

#include "efkMat.Base.h"
#include "efkMat.Utils.h"

namespace EffekseerMaterial
{

class UserObject
{
public:
	UserObject() = default;
	virtual ~UserObject() = default;
};

class Link
{
public:
	std::shared_ptr<Pin> InputPin;
	std::shared_ptr<Pin> OutputPin;

	//! For visualization
	uint64_t GUID = 0;
};

class Pin
{
public:
	std::shared_ptr<PinParameter> Parameter;
	std::weak_ptr<Node> Parent;

	//! to make easy to access
	PinDirectionType PinDirection;

	//! to make easy to access
	int32_t PinIndex;

	//! For visualization
	uint64_t GUID = 0;
};

class NodeProperty
{
public:
	//! Values, bool, isTextureValue(Normal)
	std::array<float, 4> Floats;

	//! String, Texture Path
	std::string Str;

	//! Function
	std::shared_ptr<Material> MaterialFunction;

	std::weak_ptr<Node> Parent;
};

class Node : public std::enable_shared_from_this<Node>
{
private:
	friend class Material;
	bool isDirtied = true;
	bool isContentDirtied = false;

public:
	std::shared_ptr<NodeParameter> Parameter;
	std::vector<std::shared_ptr<Pin>> InputPins;
	std::vector<std::shared_ptr<Pin>> OutputPins;
	std::vector<std::shared_ptr<NodeProperty>> Properties;

	//! For visualization
	uint64_t GUID = 0;

	//! For visualization
	Vector2DF Pos;

	//! is node opened and show a preview
	bool IsOpened = false;

	bool isPosDirty = true;

	//! For gui
	std::shared_ptr<UserObject> UserObj;

	std::weak_ptr<Material> Parent;

	int32_t GetInputPinIndex(const std::string& name);

	int32_t GetOutputPinIndex(const std::string& name);

	void UpdatePos(const Vector2DF& pos);

	bool GetIsDirtied() const { return isDirtied; }

	bool GetIsContentDirtied() const { return isContentDirtied; }

	Node() {}
};

class Material : public std::enable_shared_from_this<Material>
{
private:
	std::string path_;

	std::vector<std::shared_ptr<Node>> nodes;
	std::vector<std::shared_ptr<Link>> links;
	uint64_t nextGUID = 0xff;

	uint64_t GetIDAndNext();

	bool FindLoop(std::shared_ptr<Pin> pin1, std::shared_ptr<Pin> pin2);

public:
	Material();

	virtual ~Material();

	const std::string& GetPath() const;

	void SetPath(const std::string& path);

	void Initialize();

	std::vector<std::shared_ptr<Pin>> GetConnectedPins(std::shared_ptr<Pin> pin);

	ValueType GetPinType(DefaultType type);

	ValueType GetDesiredPinType(std::shared_ptr<Pin> pin, std::unordered_set<std::shared_ptr<Pin>>& visited);

	std::shared_ptr<Node> CreateNode(std::shared_ptr<NodeParameter> parameter, bool isDirectly = false);

	void RemoveNode(std::shared_ptr<Node> node);

	ConnectResultType CanConnectPin(std::shared_ptr<Pin> pin1, std::shared_ptr<Pin> pin2);

	ConnectResultType ConnectPin(std::shared_ptr<Pin> pin1, std::shared_ptr<Pin> pin2);

	bool BreakPin(std::shared_ptr<Link> link);

	const std::vector<std::shared_ptr<Node>>& GetNodes() const;

	const std::vector<std::shared_ptr<Link>>& GetLinks() const;

	std::shared_ptr<Node> FindNode(uint64_t guid);

	std::shared_ptr<Link> FindLink(uint64_t guid);

	std::shared_ptr<Pin> FindPin(uint64_t guid);

	std::string Copy(std::vector<std::shared_ptr<Node>> nodes, const char* basePath);

	void Paste(std::string content, const Vector2DF& pos, std::shared_ptr<Library> library, const char* basePath);

	void ChangeValue(std::shared_ptr<NodeProperty> prop, std::array<float, 4> value);

	void ChangeValue(std::shared_ptr<NodeProperty> prop, std::string value);

	void MakeDirty(std::shared_ptr<Node> node);

	void ClearDirty(std::shared_ptr<Node> node);

	void MakeContentDirty(std::shared_ptr<Node> node);

	void ClearContentDirty(std::shared_ptr<Node> node);

	void LoadFromStr(const char* json, std::shared_ptr<Library> library, const char* basePath);

	std::string SaveAsStr(const char* basePath);

	bool Load(std::vector<uint8_t>& data, std::shared_ptr<Library> library, const char* basePath);

	bool Save(std::vector<uint8_t>& data, const char* basePath);
};

} // namespace EffekseerMaterial
