#include "efkMat.Models.h"
#include "Platform/efkMat.Generic.h"
#include "ThirdParty/picojson.h"
#include "efkMat.CommandManager.h"
#include "efkMat.Library.h"
#include "efkMat.Parameters.h"
#include "efkMat.TextExporter.h"

#include <filesystem>
namespace fs = std::experimental::filesystem;

std::vector<std::string> Split(const std::string& s, char delim)
{
	std::vector<std::string> elems;
	std::stringstream ss(s);
	std::string item;
	while (getline(ss, item, delim))
	{
		if (!item.empty())
		{
			elems.push_back(item);
		}
	}
	return elems;
}

std::string Replace(std::string target, std::string from_, std::string to_)
{
	std::string::size_type Pos(target.find(from_));

	while (Pos != std::string::npos)
	{
		target.replace(Pos, from_.length(), to_);
		Pos = target.find(from_, Pos + to_.length());
	}

	return target;
}

std::string Relative(const std::string& targetPath, const std::string& basePath)
{
	if (basePath.size() == 0)
	{
		return targetPath;
	}

	auto targetPaths = Split(Replace(targetPath, "\\", "/"), '/');
	auto basePaths = Split(Replace(basePath, "\\", "/"), '/');

	if (*(basePath.end() - 1) != '/' && *(basePath.end() - 1) != '\\')
	{
		basePaths.pop_back();
	}

	int32_t offset = 0;
	while (targetPaths.size() > offset && basePaths.size() > offset)
	{
		if (targetPaths[offset] == basePaths[offset])
		{
			offset++;
		}
		else
		{
			break;
		}
	}

	std::string ret;

	for (size_t i = offset; i < basePaths.size(); i++)
	{
		ret += "../";
	}

	for (size_t i = offset; i < targetPaths.size(); i++)
	{
		ret += targetPaths[i];

		if (i != targetPaths.size() - 1)
		{
			ret += "/";
		}
	}

	return ret;
}

std::string Absolute(const std::string& targetPath, const std::string& basePath)
{
	auto targetPaths = Split(Replace(targetPath, "\\", "/"), '/');
	auto basePaths = Split(Replace(basePath, "\\", "/"), '/');

	if (*(basePath.end() - 1) != '/' && *(basePath.end() - 1) != '\\')
	{
		basePaths.pop_back();
	}

	for (size_t i = 0; i < targetPaths.size(); i++)
	{
		if (targetPaths[i] == "..")
		{
			if (basePaths.size() > 0 && basePaths.back() != "..")
			{
				basePaths.pop_back();
			}
			else
			{
				basePaths.push_back("..");
			}
		}
		else
		{
			basePaths.push_back(targetPaths[i]);
		}
	}

	std::string ret;

	for (size_t i = 0; i < basePaths.size(); i++)
	{
		ret += basePaths[i];

		if (i != basePaths.size() - 1)
		{
			ret += "/";
		}
	}

	return ret;
}

namespace EffekseerMaterial
{

class BinaryWriter
{
	std::vector<uint8_t> buffer_;

public:
	template <typename T> void Push(T value)
	{
		auto offset = buffer_.size();
		buffer_.resize(offset + sizeof(T));
		memcpy(buffer_.data() + offset, &value, sizeof(T));
	}

	template <> void Push<bool>(bool value)
	{
		int32_t temp = value ? 1 : 0;
		Push(temp);
	}

	template <typename U> void Push(const std::vector<U>& value)
	{
		Push(value.size());
		auto offset = buffer_.size();
		buffer_.resize(offset + sizeof(U) * value.size());
		memcpy(buffer_.data() + offset, value.data(), sizeof(U) * value.size());
	}

	const std::vector<uint8_t>& GetBuffer() const { return buffer_; }
};

static const char* tag_changeNumberCommand = "ChangeNumberCommand";

static const char* tag_changeNodePosCommand = "ChangeNodePosCommand";

static std::vector<char> GetVectorFromStr(std::string& s)
{
	std::vector<char> ret;
	ret.resize(s.size() + 1);
	std::copy(s.begin(), s.end(), ret.begin());
	ret[ret.size() - 1] = 0;
	return ret;
}

class ChangeNumberCommand : public ICommand
{
private:
	std::shared_ptr<NodeProperty> prop_;
	std::array<float, 4> newValue_;
	std::array<float, 4> oldValue_;

public:
	ChangeNumberCommand(std::shared_ptr<NodeProperty> prop, std::array<float, 4> newValue, std::array<float, 4> oldValue)
		: prop_(prop), newValue_(newValue), oldValue_(oldValue)
	{
	}

	virtual ~ChangeNumberCommand() {}

	void Execute() override { prop_->Floats = newValue_; }

	void Unexecute() override
	{
		prop_->Floats = oldValue_;

		auto parent = prop_->Parent.lock();

		if (parent != nullptr)
		{
			auto parentMaterial = parent->Parent.lock();

			if (parentMaterial != nullptr)
			{
				parentMaterial->MakeDirty(parent);
			}
		}
	}

	bool Merge(ICommand* command)
	{

		if (command->GetTag() != this->GetTag())
			return false;

		auto command_ = static_cast<ChangeNumberCommand*>(command);
		if (command_->prop_ != this->prop_)
			return false;

		this->oldValue_ = command_->oldValue_;

		return true;
	}

	virtual const char* GetTag() { return tag_changeNumberCommand; }
};

class ChangeNodePosCommand : public ICommand
{
private:
	std::shared_ptr<Node> node_;
	Vector2DF newValue_;
	Vector2DF oldValue_;

public:
	ChangeNodePosCommand(std::shared_ptr<Node> node, Vector2DF newValue, Vector2DF oldValue)
		: node_(node), newValue_(newValue), oldValue_(oldValue)
	{
	}

	virtual ~ChangeNodePosCommand() {}

	void Execute() override
	{
		node_->Pos = newValue_;
		node_->isPosDirty = true;
	}

	void Unexecute() override
	{
		node_->Pos = oldValue_;
		node_->isPosDirty = true;
	}

	bool Merge(ICommand* command)
	{

		if (command->GetTag() != this->GetTag())
			return false;

		auto command_ = static_cast<ChangeNodePosCommand*>(command);
		if (command_->node_ != this->node_)
			return false;

		this->oldValue_ = command_->oldValue_;

		return true;
	}

	virtual const char* GetTag() { return tag_changeNodePosCommand; }
};

int32_t Node::GetInputPinIndex(const std::string& name)
{

	for (size_t i = 0; i < InputPins.size(); i++)
	{
		if (Parameter->InputPins[i]->Name == name)
			return i;
	}

	return -1;
}

int32_t Node::GetOutputPinIndex(const std::string& name)
{

	for (size_t i = 0; i < OutputPins.size(); i++)
	{
		if (Parameter->OutputPins[i]->Name == name)
			return i;
	}

	return -1;
}

void Node::UpdatePos(const Vector2DF& pos)
{

	if (pos.X == pos.X && Pos.Y == pos.Y)
		return;

	Pos = pos;

	auto value_old = Pos;
	auto value_new = pos;

	auto command = std::make_shared<ChangeNodePosCommand>(shared_from_this(), value_new, value_old);

	CommandManager::GetInstance()->Execute(command);
}

uint64_t Material::GetIDAndNext()
{
	auto ret = nextGUID;
	nextGUID++;

	if (nextGUID > 0xefff)
	{
		nextGUID = 0xff;
	}

	return ret;
}

bool Material::FindLoop(std::shared_ptr<Pin> pin1, std::shared_ptr<Pin> pin2)
{
	auto p1 = pin1;
	auto p2 = pin2;

	if (p1->PinDirection == PinDirectionType::Output)
	{
		std::swap(p1, p2);
	}

	std::unordered_set<std::shared_ptr<Pin>> visited;
	visited.insert(p2);

	std::function<bool(std::weak_ptr<Node>)> visit;

	visit = [&](std::weak_ptr<Node> node) -> bool {
		auto locked_node = node.lock();

		for (auto p : locked_node->OutputPins)
		{
			if (visited.count(p) > 0)
				return true;
			visited.insert(p);

			auto connected_pins = GetConnectedPins(p);
			for (auto p2 : connected_pins)
			{
				if (visit(p2->Parent))
					return true;
			}
		}

		return false;
	};

	return visit(p1->Parent);
}

Material::Material() {}

Material::~Material() {}

const std::string& Material::GetPath() const { return path_; }

void Material::SetPath(const std::string& path) { path_ = path; }

void Material::Initialize()
{
	auto outputNodeParam = std::make_shared<NodeOutput>();
	auto outputNode = CreateNode(outputNodeParam, true);
}

std::vector<std::shared_ptr<Pin>> Material::GetConnectedPins(std::shared_ptr<Pin> pin)
{
	std::vector<std::shared_ptr<Pin>> ret;

	if (pin->PinDirection == PinDirectionType::Input)
	{
		for (auto link : links)
		{
			if (link->InputPin == pin)
			{
				ret.push_back(link->OutputPin);
			}
		}
	}
	else
	{
		for (auto link : links)
		{
			if (link->OutputPin == pin)
			{
				ret.push_back(link->InputPin);
			}
		}
	}

	return ret;
}

ValueType Material::GetPinType(DefaultType type)
{
	if (type == DefaultType::UV)
		return ValueType::Float2;
	return ValueType::Float1;
}

ValueType Material::GetDesiredPinType(std::shared_ptr<Pin> pin, std::unordered_set<std::shared_ptr<Pin>>& visited)
{
	if (pin->Parameter->Type != ValueType::FloatN)
		return pin->Parameter->Type;

	if (visited.count(pin) != 0)
		return ValueType::Unknown;
	visited.insert(pin);

	// self node
	auto selfNode = pin->Parent;

	if (pin->PinDirection == PinDirectionType::Output)
	{
		std::vector<ValueType> inputTypes;

		for (int i = 0; i < selfNode.lock()->InputPins.size(); i++)
		{
			auto type = GetDesiredPinType(selfNode.lock()->InputPins[i], visited);
			inputTypes.push_back(type);
		}

		auto type = selfNode.lock()->Parameter->GetOutputType(shared_from_this(), selfNode.lock(), inputTypes);

		if (type != ValueType::Unknown && type != ValueType::FloatN)
			return type;
	}

	if (pin->PinDirection == PinDirectionType::Input)
	{
		auto relatedPins = GetConnectedPins(pin);

		for (auto relatedPin : relatedPins)
		{
			if (visited.count(relatedPin) == 0)
			{
				auto type = GetDesiredPinType(relatedPin, visited);
				if (type != ValueType::Unknown && type != ValueType::FloatN)
					return type;
			}
		}
	}

	return GetPinType(pin->Parameter->Default);
}

std::shared_ptr<Node> Material::CreateNode(std::shared_ptr<NodeParameter> parameter, bool isDirectly)
{
	auto node = std::make_shared<Node>();
	node->Parameter = parameter;
	node->GUID = GetIDAndNext();

	for (auto i = 0; i < parameter->InputPins.size(); i++)
	{
		auto pp = parameter->InputPins[i];

		auto p = std::make_shared<Pin>();
		p->Parameter = pp;
		p->Parent = node;
		p->PinDirection = PinDirectionType::Input;
		p->PinIndex = i;
		p->GUID = GetIDAndNext();

		node->InputPins.push_back(p);
	}

	for (auto i = 0; i < parameter->OutputPins.size(); i++)
	{
		auto pp = parameter->OutputPins[i];

		auto p = std::make_shared<Pin>();
		p->Parameter = pp;
		p->Parent = node;
		p->PinDirection = PinDirectionType::Output;
		p->PinIndex = i;
		p->GUID = GetIDAndNext();

		node->OutputPins.push_back(p);
	}

	for (auto i = 0; i < parameter->Properties.size(); i++)
	{
		auto np = std::make_shared<NodeProperty>();
		np->Floats = parameter->Properties[i]->DefaultValues;
		np->Parent = node;

		node->Properties.push_back(np);
	}

	node->Parent = shared_from_this();

	auto val_old = nodes;
	auto val_new = nodes;
	val_new.push_back(node);

	if (isDirectly)
	{
		this->nodes = val_new;
	}
	else
	{
		auto command = std::make_shared<DelegateCommand>([this, val_new]() -> void { this->nodes = val_new; },
														 [this, val_old]() -> void { this->nodes = val_old; });

		CommandManager::GetInstance()->Execute(command);
	}

	return node;
}

void Material::RemoveNode(std::shared_ptr<Node> node)
{
	auto nodes_old = nodes;
	auto links_old = links;

	auto nodes_new = nodes;
	auto links_new = links;

	nodes_new.erase(std::remove(nodes_new.begin(), nodes_new.end(), node), nodes_new.end());

	std::vector<std::shared_ptr<Link>> removing;
	for (auto link : links_new)
	{
		auto beginNode = link->InputPin->Parent.lock();
		auto endNode = link->OutputPin->Parent.lock();

		if (beginNode == nullptr || beginNode == node || endNode == nullptr || endNode == node)
		{
			removing.push_back(link);
		}
	}

	for (auto link : removing)
	{
		links_new.erase(std::remove(links_new.begin(), links_new.end(), link), links_new.end());
	}

	auto command = std::make_shared<DelegateCommand>(
		[this, nodes_new, links_new]() -> void {
			this->nodes = nodes_new;
			this->links = links_new;
		},
		[this, nodes_old, links_old]() -> void {
			this->nodes = nodes_old;
			this->links = links_old;
		});

	CommandManager::GetInstance()->Execute(command);
}

ConnectResultType Material::CanConnectPin(std::shared_ptr<Pin> pin1, std::shared_ptr<Pin> pin2)
{
	if (pin1 == pin2)
		return ConnectResultType::SamePin;
	if (pin1->PinDirection == pin2->PinDirection)
		return ConnectResultType::SameDirection;
	if (pin1->Parent.lock() == pin2->Parent.lock())
		return ConnectResultType::SameNode;

	std::unordered_set<std::shared_ptr<Pin>> visited1;
	std::unordered_set<std::shared_ptr<Pin>> visited2;
	auto pin1Type = GetDesiredPinType(pin1, visited1);
	auto pin2Type = GetDesiredPinType(pin2, visited2);

	// All type can be connected in this version.
	if (pin1Type != pin2Type)
	{
		if (IsFloatValueType(pin1Type) && IsFloatValueType(pin2Type))
		{
			// OK
		}
		// else if (pin1->Parameter->Type == ValueType::FloatN && IsFloatValueType(pin2Type))
		//{
		//	// OK
		//}
		else
		{
			return ConnectResultType::Type;
		}
	}

	// Loop has been detected
	if (FindLoop(pin1, pin2))
	{
		return ConnectResultType::Loop;
	}

	return ConnectResultType::OK;
}

ConnectResultType Material::ConnectPin(std::shared_ptr<Pin> pin1, std::shared_ptr<Pin> pin2)
{
	if (pin1 == pin2)
		return ConnectResultType::SamePin;
	if (pin1->PinDirection == pin2->PinDirection)
		return ConnectResultType::SameDirection;
	if (pin1->Parent.lock() == pin2->Parent.lock())
		return ConnectResultType::SameNode;

	std::unordered_set<std::shared_ptr<Pin>> visited1;
	std::unordered_set<std::shared_ptr<Pin>> visited2;
	auto pin1Type = GetDesiredPinType(pin1, visited1);
	auto pin2Type = GetDesiredPinType(pin2, visited2);

	// All type can be connected in this version.
	if (pin1Type != pin2Type)
	{
		if (IsFloatValueType(pin1Type) && IsFloatValueType(pin2Type))
		{
			// OK
		}
		// else if (pin1->Parameter->Type == ValueType::FloatN && IsFloatValueType(pin2Type))
		//{
		//	// OK
		//}
		else
		{
			return ConnectResultType::Type;
		}
	}

	// Loop has been detected
	if (FindLoop(pin1, pin2))
	{
		return ConnectResultType::Loop;
	}

	auto p1 = pin1;
	auto p2 = pin2;

	if (p1->PinDirection == PinDirectionType::Output)
	{
		std::swap(p1, p2);
	}

	auto links_old = links;

	// Find multiple connect
	std::shared_ptr<Link> removingLink;
	for (auto link : links)
	{
		if (link->InputPin == p1)
		{
			removingLink = link;
			break;
		}
	}

	if (removingLink != nullptr)
	{
		BreakPin(removingLink);
	}

	auto links_new = links;

	auto link = std::make_shared<Link>();
	link->InputPin = p1;
	link->OutputPin = p2;
	link->GUID = GetIDAndNext();

	links_new.push_back(link);

	MakeDirty(p1->Parent.lock());

	auto command = std::make_shared<DelegateCommand>([this, links_new]() -> void { this->links = links_new; },
													 [this, links_old]() -> void { this->links = links_old; });

	CommandManager::GetInstance()->Execute(command);

	return ConnectResultType::OK;
}

bool Material::BreakPin(std::shared_ptr<Link> link)
{
	auto links_old = links;
	auto links_new = links;

	links_new.erase(std::remove(links_new.begin(), links_new.end(), link), links_new.end());

	auto inputNode = link->InputPin->Parent.lock();
	if (inputNode != nullptr)
	{
		MakeDirty(inputNode);
	}

	auto command = std::make_shared<DelegateCommand>([this, links_new]() -> void { this->links = links_new; },
													 [this, links_old]() -> void { this->links = links_old; });

	CommandManager::GetInstance()->Execute(command);

	return true;
}

const std::vector<std::shared_ptr<Node>>& Material::GetNodes() const { return nodes; }

const std::vector<std::shared_ptr<Link>>& Material::GetLinks() const { return links; }

std::shared_ptr<Node> Material::FindNode(uint64_t guid)
{
	for (auto node : nodes)
	{
		if (node->GUID == guid)
			return node;
	}
	return nullptr;
}

std::shared_ptr<Link> Material::FindLink(uint64_t guid)
{
	for (auto link : links)
	{
		if (link->GUID == guid)
			return link;
	}
	return nullptr;
}

std::shared_ptr<Pin> Material::FindPin(uint64_t guid)
{
	for (auto node : nodes)
	{
		for (auto pin : node->InputPins)
		{
			if (pin->GUID == guid)
				return pin;
		}

		for (auto pin : node->OutputPins)
		{
			if (pin->GUID == guid)
				return pin;
		}
	}

	return nullptr;
}

std::string Material::Copy(std::vector<std::shared_ptr<Node>> nodes, const char* basePath)
{
	// calculate left pos
	Vector2DF upperLeftPos = Vector2DF(FLT_MAX, FLT_MAX);

	for (auto node : nodes)
	{
		upperLeftPos.X = std::min(upperLeftPos.X, node->Pos.X);
		upperLeftPos.Y = std::min(upperLeftPos.Y, node->Pos.Y);
	}

	// TODO : remove copy codes
	picojson::object root_;
	picojson::array nodes_;
	picojson::array links_;

	root_.insert(std::make_pair("Project", picojson::value("EffekseerMaterial")));

	std::unordered_set<std::shared_ptr<Node>> setNodes;

	for (auto node : nodes)
	{
		setNodes.insert(node);

		picojson::object node_;
		node_.insert(std::make_pair("GUID", picojson::value((double)node->GUID)));
		node_.insert(std::make_pair("Type", picojson::value(node->Parameter->TypeName.c_str())));
		node_.insert(std::make_pair("PosX", picojson::value(node->Pos.X - upperLeftPos.X)));
		node_.insert(std::make_pair("PosY", picojson::value(node->Pos.Y - upperLeftPos.Y)));

		picojson::array props_;

		for (size_t i = 0; i < node->Parameter->Properties.size(); i++)
		{
			picojson::object prop_;

			auto pp = node->Parameter->Properties[i];
			auto p = node->Properties[i];

			if (pp->Type == ValueType::Float1)
			{
				prop_.insert(std::make_pair("Value1", picojson::value((double)p->Floats[0])));
			}
			else if (pp->Type == ValueType::Float2)
			{
				prop_.insert(std::make_pair("Value1", picojson::value((double)p->Floats[0])));
				prop_.insert(std::make_pair("Value2", picojson::value((double)p->Floats[1])));
			}
			else if (pp->Type == ValueType::Float3)
			{
				prop_.insert(std::make_pair("Value1", picojson::value((double)p->Floats[0])));
				prop_.insert(std::make_pair("Value2", picojson::value((double)p->Floats[1])));
				prop_.insert(std::make_pair("Value3", picojson::value((double)p->Floats[2])));
			}
			else if (pp->Type == ValueType::Float4)
			{
				prop_.insert(std::make_pair("Value1", picojson::value((double)p->Floats[0])));
				prop_.insert(std::make_pair("Value2", picojson::value((double)p->Floats[1])));
				prop_.insert(std::make_pair("Value3", picojson::value((double)p->Floats[2])));
				prop_.insert(std::make_pair("Value4", picojson::value((double)p->Floats[3])));
			}
			else if (pp->Type == ValueType::Bool)
			{
				prop_.insert(std::make_pair("Value", picojson::value(p->Floats[0] > 0)));
			}
			else if (pp->Type == ValueType::String)
			{
				prop_.insert(std::make_pair("Value", picojson::value(p->Str)));
			}
			else if (pp->Type == ValueType::Texture)
			{
				auto absStr = p->Str;
				auto relative = Relative(absStr, basePath);
				prop_.insert(std::make_pair("Value", picojson::value(relative)));
			}

			props_.push_back(picojson::value(prop_));
		}

		node_.insert(std::make_pair("Props", picojson::value(props_)));

		nodes_.push_back(picojson::value(node_));
	}

	root_.insert(std::make_pair("Nodes", picojson::value(nodes_)));

	std::unordered_set<std::shared_ptr<Link>> collectedLinks;

	for (auto node : nodes)
	{
		for (auto link : links)
		{
			auto input = link->InputPin->Parent.lock();
			auto output = link->OutputPin->Parent.lock();

			if (setNodes.find(input) != setNodes.end() && setNodes.find(output) != setNodes.end())
			{
				collectedLinks.insert(link);
			}
		}
	}

	for (auto link : collectedLinks)
	{
		picojson::object link_;
		link_.insert(std::make_pair("GUID", picojson::value((double)link->GUID)));

		auto inputNode = link->InputPin->Parent.lock();
		auto inputName = inputNode->Parameter->InputPins[link->InputPin->PinIndex]->Name;

		auto outputNode = link->OutputPin->Parent.lock();
		auto outputName = outputNode->Parameter->OutputPins[link->OutputPin->PinIndex]->Name;

		link_.insert(std::make_pair("InputGUID", picojson::value((double)link->InputPin->Parent.lock()->GUID)));
		link_.insert(std::make_pair("InputPin", picojson::value(inputName)));
		link_.insert(std::make_pair("OutputGUID", picojson::value((double)link->OutputPin->Parent.lock()->GUID)));
		link_.insert(std::make_pair("OutputPin", picojson::value(outputName)));

		links_.push_back(picojson::value(link_));
	}

	root_.insert(std::make_pair("Links", picojson::value(links_)));

	auto str_main = picojson::value(root_).serialize();

	return str_main;
}

void Material::Paste(std::string content, const Vector2DF& pos, std::shared_ptr<Library> library, const char* basePath)
{
	picojson::value root_;
	auto err = picojson::parse(root_, content);
	if (!err.empty())
	{
		std::cerr << err << std::endl;
		return;
	}

	// check project
	picojson::value project_name_obj = root_.get("Project");
	auto project_name = project_name_obj.get<std::string>();
	if (project_name != "EffekseerMaterial")
		return;

	picojson::value nodes_obj = root_.get("Nodes");
	picojson::value links_obj = root_.get("Links");

	picojson::array nodes_ = nodes_obj.get<picojson::array>();
	picojson::array links_ = links_obj.get<picojson::array>();

	std::map<uint64_t, uint64_t> oldIDToNewID;

	CommandManager::GetInstance()->StartCollection();

	for (auto node_ : nodes_)
	{
		auto guid_obj = node_.get("GUID");
		auto guid = (uint64_t)guid_obj.get<double>();

		auto type_obj = node_.get("Type");
		auto type = type_obj.get<std::string>();

		auto node_library = library->FindContentWithTypeName(type.c_str());
		if (node_library == nullptr)
			continue;

		auto node_parameter = node_library->Create();
		auto node = CreateNode(node_parameter);

		auto pos_x_obj = node_.get("PosX");
		node->Pos.X = (float)pos_x_obj.get<double>() + pos.X;

		auto pos_y_obj = node_.get("PosY");
		node->Pos.Y = (float)pos_y_obj.get<double>() + pos.Y;

		oldIDToNewID[guid] = node->GUID;
		// node->GUID = guid;

		auto props_obj = node_.get("Props");
		auto props_ = props_obj.get<picojson::array>();

		for (int32_t i = 0; i < props_.size(); i++)
		{
			if (node->Parameter->Properties[i]->Type == ValueType::Float1)
			{
				node->Properties[i]->Floats[0] = props_[i].get("Value1").get<double>();
			}
			else if (node->Parameter->Properties[i]->Type == ValueType::Float2)
			{
				node->Properties[i]->Floats[0] = props_[i].get("Value1").get<double>();
				node->Properties[i]->Floats[1] = props_[i].get("Value2").get<double>();
			}
			else if (node->Parameter->Properties[i]->Type == ValueType::Float3)
			{
				node->Properties[i]->Floats[0] = props_[i].get("Value1").get<double>();
				node->Properties[i]->Floats[1] = props_[i].get("Value2").get<double>();
				node->Properties[i]->Floats[2] = props_[i].get("Value3").get<double>();
			}
			else if (node->Parameter->Properties[i]->Type == ValueType::Float4)
			{
				node->Properties[i]->Floats[0] = props_[i].get("Value1").get<double>();
				node->Properties[i]->Floats[1] = props_[i].get("Value2").get<double>();
				node->Properties[i]->Floats[2] = props_[i].get("Value3").get<double>();
				node->Properties[i]->Floats[3] = props_[i].get("Value4").get<double>();
			}
			else if (node->Parameter->Properties[i]->Type == ValueType::Bool)
			{
				node->Properties[i]->Floats[0] = props_[i].get("Value").get<bool>() ? 1.0f : 0.0f;
			}
			else if (node->Parameter->Properties[i]->Type == ValueType::String)
			{
				auto str = props_[i].get("Value").get<std::string>();
				node->Properties[i]->Str = str;
			}
			else if (node->Parameter->Properties[i]->Type == ValueType::Texture)
			{
				auto str = props_[i].get("Value").get<std::string>();
				auto absolute = Absolute(str, basePath);
				node->Properties[i]->Str = absolute;
			}
		}
	}

	for (auto link_ : links_)
	{
		auto guid_obj = link_.get("GUID");
		auto guid = (uint64_t)guid_obj.get<double>();

		auto InputGUID_obj = link_.get("InputGUID");
		auto InputGUID = (uint64_t)InputGUID_obj.get<double>();

		auto InputPin_obj = link_.get("InputPin");
		auto InputPin = InputPin_obj.get<std::string>();

		auto OutputGUID_obj = link_.get("OutputGUID");
		auto OutputGUID = (uint64_t)OutputGUID_obj.get<double>();

		auto OutputPin_obj = link_.get("OutputPin");
		auto OutputPin = OutputPin_obj.get<std::string>();

		auto inputNode = FindNode(oldIDToNewID[InputGUID]);
		auto outputNode = FindNode(oldIDToNewID[OutputGUID]);
		auto InputPinIndex = inputNode->GetInputPinIndex(InputPin);
		auto OutputPinIndex = outputNode->GetOutputPinIndex(OutputPin);

		if (InputPinIndex >= 0 && OutputPinIndex >= 0)
		{
			ConnectPin(inputNode->InputPins[InputPinIndex], outputNode->OutputPins[OutputPinIndex]);
		}
	}

	CommandManager::GetInstance()->EndCollection();
}

void Material::ChangeValue(std::shared_ptr<NodeProperty> prop, std::array<float, 4> value)
{
	auto value_old = prop->Floats;
	auto value_new = value;

	auto command = std::make_shared<ChangeNumberCommand>(prop, value_new, value_old);

	CommandManager::GetInstance()->Execute(command);
}

void Material::ChangeValue(std::shared_ptr<NodeProperty> prop, std::string value)
{
	auto value_old = prop->Str;
	auto value_new = value;

	auto command = std::make_shared<DelegateCommand>([prop, value_new]() -> void { prop->Str = value_new; },
													 [prop, value_old]() -> void { prop->Str = value_old; });

	CommandManager::GetInstance()->Execute(command);
}

void Material::MakeDirty(std::shared_ptr<Node> node)
{
	node->isDirtied = true;

	for (auto o : node->OutputPins)
	{
		auto connected = GetConnectedPins(o);

		for (auto c : connected)
		{
			MakeDirty(c->Parent.lock());
		}
	}
}

void Material::ClearDirty(std::shared_ptr<Node> node) { node->isDirtied = false; }

void Material::MakeContentDirty(std::shared_ptr<Node> node)
{
	node->isContentDirtied = true;

	for (auto o : node->OutputPins)
	{
		auto connected = GetConnectedPins(o);

		for (auto c : connected)
		{
			MakeContentDirty(c->Parent.lock());
		}
	}
}

void Material::ClearContentDirty(std::shared_ptr<Node> node) { node->isContentDirtied = false; }

void Material::LoadFromStr(const char* json, std::shared_ptr<Library> library, const char* basePath)
{
	picojson::value root_;
	auto err = picojson::parse(root_, json);
	if (!err.empty())
	{
		std::cerr << err << std::endl;
		return;
	}

	// check project
	picojson::value project_name_obj = root_.get("Project");
	auto project_name = project_name_obj.get<std::string>();
	if (project_name != "EffekseerMaterial")
		return;

	nodes.clear();
	links.clear();

	picojson::value nodes_obj = root_.get("Nodes");
	picojson::value links_obj = root_.get("Links");

	picojson::array nodes_ = nodes_obj.get<picojson::array>();
	picojson::array links_ = links_obj.get<picojson::array>();

	for (auto node_ : nodes_)
	{
		auto guid_obj = node_.get("GUID");
		auto guid = (uint64_t)guid_obj.get<double>();

		auto type_obj = node_.get("Type");
		auto type = type_obj.get<std::string>();

		auto node_library = library->FindContentWithTypeName(type.c_str());
		if (node_library == nullptr)
			continue;

		auto node_parameter = node_library->Create();
		auto node = CreateNode(node_parameter);

		auto pos_x_obj = node_.get("PosX");
		node->Pos.X = (float)pos_x_obj.get<double>();

		auto pos_y_obj = node_.get("PosY");
		node->Pos.Y = (float)pos_y_obj.get<double>();

		node->GUID = guid;

		auto props_obj = node_.get("Props");
		auto props_ = props_obj.get<picojson::array>();

		for (int32_t i = 0; i < props_.size(); i++)
		{
			if (node->Parameter->Properties[i]->Type == ValueType::Float1)
			{
				node->Properties[i]->Floats[0] = props_[i].get("Value1").get<double>();
			}
			else if (node->Parameter->Properties[i]->Type == ValueType::Float2)
			{
				node->Properties[i]->Floats[0] = props_[i].get("Value1").get<double>();
				node->Properties[i]->Floats[1] = props_[i].get("Value2").get<double>();
			}
			else if (node->Parameter->Properties[i]->Type == ValueType::Float3)
			{
				node->Properties[i]->Floats[0] = props_[i].get("Value1").get<double>();
				node->Properties[i]->Floats[1] = props_[i].get("Value2").get<double>();
				node->Properties[i]->Floats[2] = props_[i].get("Value3").get<double>();
			}
			else if (node->Parameter->Properties[i]->Type == ValueType::Float4)
			{
				node->Properties[i]->Floats[0] = props_[i].get("Value1").get<double>();
				node->Properties[i]->Floats[1] = props_[i].get("Value2").get<double>();
				node->Properties[i]->Floats[2] = props_[i].get("Value3").get<double>();
				node->Properties[i]->Floats[3] = props_[i].get("Value4").get<double>();
			}
			else if (node->Parameter->Properties[i]->Type == ValueType::Bool)
			{
				node->Properties[i]->Floats[0] = props_[i].get("Value").get<bool>() ? 1.0f : 0.0f;
			}
			else if (node->Parameter->Properties[i]->Type == ValueType::String)
			{
				auto str = props_[i].get("Value").get<std::string>();
				node->Properties[i]->Str = str;
			}
			else if (node->Parameter->Properties[i]->Type == ValueType::Texture)
			{
				auto str = props_[i].get("Value").get<std::string>();
				auto absolute = Absolute(str, basePath);
				node->Properties[i]->Str = absolute;
			}
		}
	}

	for (auto link_ : links_)
	{
		auto guid_obj = link_.get("GUID");
		auto guid = (uint64_t)guid_obj.get<double>();

		auto InputGUID_obj = link_.get("InputGUID");
		auto InputGUID = (uint64_t)InputGUID_obj.get<double>();

		auto InputPin_obj = link_.get("InputPin");
		auto InputPin = InputPin_obj.get<std::string>();

		auto OutputGUID_obj = link_.get("OutputGUID");
		auto OutputGUID = (uint64_t)OutputGUID_obj.get<double>();

		auto OutputPin_obj = link_.get("OutputPin");
		auto OutputPin = OutputPin_obj.get<std::string>();

		auto link = std::make_shared<Link>();

		auto inputNode = FindNode(InputGUID);
		auto outputNode = FindNode(OutputGUID);
		auto InputPinIndex = inputNode->GetInputPinIndex(InputPin);
		auto OutputPinIndex = outputNode->GetOutputPinIndex(OutputPin);

		if (InputPinIndex >= 0 && OutputPinIndex >= 0)
		{
			link->InputPin = inputNode->InputPins[InputPinIndex];
			link->OutputPin = outputNode->OutputPins[OutputPinIndex];
			link->GUID = GetIDAndNext();
			links.push_back(link);
		}
	}
}

std::string Material::SaveAsStr(const char* basePath)
{
	picojson::object root_;
	picojson::array nodes_;

	root_.insert(std::make_pair("Project", picojson::value("EffekseerMaterial")));

	// param by run textexporter
	/*
	{
		std::shared_ptr<Node> outputNode;
		for (auto node : nodes)
		{
			if (node->Parameter->Type == NodeType::Output)
			{
				outputNode = node;
			}
		}

		TextExporter textExporter;
		auto result = textExporter.Export(shared_from_this(), outputNode);

		picojson::object params_;
		picojson::array textures_;
		picojson::array uniforms_;

		for (auto& param : result.Textures)
		{
			picojson::object node_;
			node_.insert(std::make_pair("Name", picojson::value(param->Name)));
			node_.insert(std::make_pair("Index", picojson::value((double)param->Index)));

			{
				auto absStr = param->DefaultPath;
				if (absStr == "")
				{
					node_.insert(std::make_pair("DefaultPath", picojson::value("")));
				}
				else
				{
					auto relative = Relative(absStr, basePath);
					node_.insert(std::make_pair("DefaultPath", picojson::value(relative)));
				}
			}

			node_.insert(std::make_pair("IsParam", picojson::value(param->IsParam)));
			node_.insert(std::make_pair("IsValueTexture", picojson::value(param->IsValueTexture)));
			textures_.push_back(picojson::value(node_));
		}

		for (auto& param : result.Uniforms)
		{
			picojson::object node_;
			node_.insert(std::make_pair("Name", picojson::value(param->Name)));
			node_.insert(std::make_pair("Offset", picojson::value((double)param->Offset)));
			node_.insert(std::make_pair("Type", picojson::value((double)param->Type)));
			node_.insert(std::make_pair("DefaultValue1", picojson::value(param->DefaultConstants[0])));
			node_.insert(std::make_pair("DefaultValue2", picojson::value(param->DefaultConstants[1])));
			node_.insert(std::make_pair("DefaultValue3", picojson::value(param->DefaultConstants[2])));
			node_.insert(std::make_pair("DefaultValue4", picojson::value(param->DefaultConstants[3])));
			uniforms_.push_back(picojson::value(node_));
		}

		params_.insert(std::make_pair("Textures", picojson::value(textures_)));
		params_.insert(std::make_pair("Uniforms", picojson::value(uniforms_)));
		root_.insert(std::make_pair("Params", picojson::value(params_)));
	}
	*/

	for (auto node : nodes)
	{
		picojson::object node_;
		node_.insert(std::make_pair("GUID", picojson::value((double)node->GUID)));
		node_.insert(std::make_pair("Type", picojson::value(node->Parameter->TypeName.c_str())));
		node_.insert(std::make_pair("PosX", picojson::value(node->Pos.X)));
		node_.insert(std::make_pair("PosY", picojson::value(node->Pos.Y)));

		picojson::array props_;

		for (size_t i = 0; i < node->Parameter->Properties.size(); i++)
		{
			picojson::object prop_;

			auto pp = node->Parameter->Properties[i];
			auto p = node->Properties[i];

			if (pp->Type == ValueType::Float1)
			{
				prop_.insert(std::make_pair("Value1", picojson::value((double)p->Floats[0])));
			}
			else if (pp->Type == ValueType::Float2)
			{
				prop_.insert(std::make_pair("Value1", picojson::value((double)p->Floats[0])));
				prop_.insert(std::make_pair("Value2", picojson::value((double)p->Floats[1])));
			}
			else if (pp->Type == ValueType::Float3)
			{
				prop_.insert(std::make_pair("Value1", picojson::value((double)p->Floats[0])));
				prop_.insert(std::make_pair("Value2", picojson::value((double)p->Floats[1])));
				prop_.insert(std::make_pair("Value3", picojson::value((double)p->Floats[2])));
			}
			else if (pp->Type == ValueType::Float4)
			{
				prop_.insert(std::make_pair("Value1", picojson::value((double)p->Floats[0])));
				prop_.insert(std::make_pair("Value2", picojson::value((double)p->Floats[1])));
				prop_.insert(std::make_pair("Value3", picojson::value((double)p->Floats[2])));
				prop_.insert(std::make_pair("Value4", picojson::value((double)p->Floats[3])));
			}
			else if (pp->Type == ValueType::Bool)
			{
				prop_.insert(std::make_pair("Value", picojson::value(p->Floats[0] > 0)));
			}
			else if (pp->Type == ValueType::String)
			{
				prop_.insert(std::make_pair("Value", picojson::value(p->Str)));
			}
			else if (pp->Type == ValueType::Texture)
			{
				auto absStr = p->Str;
				auto relative = Relative(absStr, basePath);
				prop_.insert(std::make_pair("Value", picojson::value(relative)));
			}

			props_.push_back(picojson::value(prop_));
		}

		node_.insert(std::make_pair("Props", picojson::value(props_)));

		nodes_.push_back(picojson::value(node_));
	}

	root_.insert(std::make_pair("Nodes", picojson::value(nodes_)));

	picojson::array links_;

	for (auto link : links)
	{
		picojson::object link_;
		link_.insert(std::make_pair("GUID", picojson::value((double)link->GUID)));

		auto inputNode = link->InputPin->Parent.lock();
		auto inputName = inputNode->Parameter->InputPins[link->InputPin->PinIndex]->Name;

		auto outputNode = link->OutputPin->Parent.lock();
		auto outputName = outputNode->Parameter->OutputPins[link->OutputPin->PinIndex]->Name;

		link_.insert(std::make_pair("InputGUID", picojson::value((double)link->InputPin->Parent.lock()->GUID)));
		link_.insert(std::make_pair("InputPin", picojson::value(inputName)));
		link_.insert(std::make_pair("OutputGUID", picojson::value((double)link->OutputPin->Parent.lock()->GUID)));
		link_.insert(std::make_pair("OutputPin", picojson::value(outputName)));

		links_.push_back(picojson::value(link_));
	}

	root_.insert(std::make_pair("Links", picojson::value(links_)));

	auto str_main = picojson::value(root_).serialize();

	return str_main;
}

bool Material::Load(std::vector<uint8_t>& data, std::shared_ptr<Library> library, const char* basePath)
{

	int offset = 0;

	// header
	char prefix[5];

	memcpy(prefix, data.data() + offset, 4);
	offset += sizeof(int);

	prefix[4] = 0;

	if (std::string("efkM") != std::string(prefix))
		return false;

	int version = 0;
	memcpy(&version, data.data() + offset, 4);
	offset += sizeof(int);

	uint64_t guid = 0;
	memcpy(&guid, data.data() + offset, 8);
	offset += sizeof(uint64_t);

	while (offset < data.size())
	{
		char chunk[5];
		memcpy(chunk, data.data() + offset, 4);
		offset += sizeof(int);
		chunk[4] = 0;

		int chunk_size = 0;
		memcpy(&chunk_size, data.data() + offset, 4);
		offset += sizeof(int);

		if (std::string("data") == std::string(chunk))
		{
			LoadFromStr((const char*)(data.data() + offset), library, basePath);
		}

		offset += chunk_size;
	}

	return true;
}

bool Material::Save(std::vector<uint8_t>& data, const char* basePath)
{
	// header

	char* prefix = "efkM";
	int version = 1;

	int offset = 0;

	offset = data.size();
	data.resize(data.size() + 4);
	memcpy(data.data() + offset, prefix, 4);

	offset = data.size();
	data.resize(data.size() + 4);
	memcpy(data.data() + offset, &version, 4);

	auto internalJson = SaveAsStr(basePath);
	auto hash_str = std::hash<std::string>();
	auto guid = (uint64_t)hash_str(internalJson);

	offset = data.size();
	data.resize(data.size() + sizeof(uint64_t));
	memcpy(data.data() + offset, &guid, sizeof(uint64_t));

	// generic

	std::shared_ptr<Node> outputNode;
	for (auto node : nodes)
	{
		if (node->Parameter->Type == NodeType::Output)
		{
			outputNode = node;
		}
	}

	std::shared_ptr<TextExporter> textExporter = std::make_shared<TextExporterGeneric>();
	auto result = textExporter->Export(shared_from_this(), outputNode);

	
	// flag

	// Lit or Unlit(Shading type)
	// Change normal
	// refraction
	BinaryWriter bwParam;
	bwParam.Push(1);
	bwParam.Push(false);
	bwParam.Push(result.HasRefraction);

	bwParam.Push(result.Textures.size());

	for (size_t i = 0; i < result.Textures.size(); i++)
	{
		auto& param = result.Textures[i];
		auto name_ = GetVectorFromStr(param->Name);
		bwParam.Push(name_);

		auto defaultPath_ = GetVectorFromStr(param->DefaultPath);
		bwParam.Push(defaultPath_);
		bwParam.Push(param->Index);
		bwParam.Push(param->IsParam);
		bwParam.Push(param->IsValueTexture);
	}

	bwParam.Push(result.Uniforms.size());

	for (size_t i = 0; i < result.Uniforms.size(); i++)
	{
		auto& param = result.Uniforms[i];

		auto name_ = GetVectorFromStr(param->Name);
		bwParam.Push(name_);

		bwParam.Push(param->Offset);

		int type = (int)(param->Type);
		bwParam.Push(type);

		bwParam.Push(param->DefaultConstants[0]);
		bwParam.Push(param->DefaultConstants[1]);
		bwParam.Push(param->DefaultConstants[2]);
		bwParam.Push(param->DefaultConstants[3]);
	}

	char* chunk_para = "para";
	auto size_para = bwParam.GetBuffer().size();

	offset = data.size();
	data.resize(data.size() + 4);
	memcpy(data.data() + offset, chunk_para, 4);

	offset = data.size();
	data.resize(data.size() + sizeof(int32_t));
	memcpy(data.data() + offset, &size_para, sizeof(int32_t));

	offset = data.size();
	data.resize(data.size() + bwParam.GetBuffer().size());
	memcpy(data.data() + offset, bwParam.GetBuffer().data(), bwParam.GetBuffer().size());

	BinaryWriter bwGene;

	{
		auto code_ = GetVectorFromStr(result.Code);
		bwGene.Push(code_);
	}

	char* chunk_gene = "gene";
	auto size_gene = bwGene.GetBuffer().size();

	offset = data.size();
	data.resize(data.size() + 4);
	memcpy(data.data() + offset, chunk_gene, 4);

	offset = data.size();
	data.resize(data.size() + sizeof(int32_t));
	memcpy(data.data() + offset, &size_gene, sizeof(int32_t));

	offset = data.size();
	data.resize(data.size() + bwGene.GetBuffer().size());
	memcpy(data.data() + offset, bwGene.GetBuffer().data(), bwGene.GetBuffer().size());


	// data
	auto internalJsonVec = GetVectorFromStr(internalJson);
	char* chunk_data = "data";
	auto size_data = internalJsonVec.size();

	offset = data.size();
	data.resize(data.size() + 4);
	memcpy(data.data() + offset, chunk_data, 4);

	offset = data.size();
	data.resize(data.size() + sizeof(int32_t));
	memcpy(data.data() + offset, &size_data, sizeof(int32_t));

	offset = data.size();
	data.resize(data.size() + internalJsonVec.size());
	memcpy(data.data() + offset, internalJsonVec.data(), internalJsonVec.size());

	return true;
}

} // namespace EffekseerMaterial
