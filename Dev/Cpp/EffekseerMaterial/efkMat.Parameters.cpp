#include "efkMat.Parameters.h"
#include "efkMat.Models.h"

namespace EffekseerMaterial
{

ValueType NodeParameter::GetOutputTypeIn1Out1(const std::vector<ValueType>& inputTypes) const { return inputTypes[0]; }

ValueType NodeParameter::GetOutputTypeIn2Out1Param2(const std::vector<ValueType>& inputTypes) const
{
	if (inputTypes[0] == ValueType::Float1)
		return inputTypes[1];

	if (inputTypes[1] == ValueType::Float1)
		return inputTypes[0];

	return inputTypes[0];
}

WarningType NodeParameter::GetWarningIn2Out1Param2(std::shared_ptr<Material> material, std::shared_ptr<Node> node) const
{
	auto type1 = material->GetDesiredPinType(node->InputPins[0], std::unordered_set<std::shared_ptr<Pin>>());
	auto type2 = material->GetDesiredPinType(node->InputPins[1], std::unordered_set<std::shared_ptr<Pin>>());

	if (type1 == ValueType::Float1)
		WarningType::None;

	if (type2 == ValueType::Float1)
		WarningType::None;

	return type1 == type2 ? WarningType::None : WarningType::WrongInputType;
}

NodeConstant1::NodeConstant1()
{
	Type = NodeType::Constant1;
	TypeName = "Constant1";
	Description = "Constant value...";
	Group = std::vector<std::string>{"Constant"};

	auto output = std::make_shared<PinParameter>();
	output->Name = "";
	output->Type = ValueType::Float1;
	OutputPins.push_back(output);

	auto param = std::make_shared<NodePropertyParameter>();
	param->Name = "Value";
	param->Type = ValueType::Float1;
	Properties.push_back(param);

	auto func1 = std::make_shared<NodeFunctionParameter>();
	func1->Name = "ConvertParam";
	func1->Func = [](std::shared_ptr<Material> material, std::shared_ptr<Node> node) -> bool {
		auto param = std::make_shared<NodeParam1>();
		auto new_node = material->CreateNode(param);
		auto links = material->GetConnectedPins(node->OutputPins[0]);

		for (auto link : links)
		{
			material->ConnectPin(new_node->OutputPins[0], link);
		}

		new_node->isPosDirty = true;
		new_node->Pos = node->Pos;
		new_node->Properties[1]->Floats = node->Properties[0]->Floats;
		material->RemoveNode(node);
		return true;
	};

	Funcs.push_back(func1);
}

ValueType NodeComponentMask::GetOutputType(std::shared_ptr<Material> material,
										   std::shared_ptr<Node> node,
										   const std::vector<ValueType>& inputTypes) const
{
	int counter = 0;
	for (int i = 0; i < 4; i++)
	{
		if (node->Properties[i]->Floats[0] > 0)
			counter++;
	}

	if (counter == 1)
		return ValueType::Float1;

	if (counter == 2)
		return ValueType::Float2;

	if (counter == 3)
		return ValueType::Float3;

	if (counter == 4)
		return ValueType::Float4;
	return ValueType::Float1;
}

WarningType NodeComponentMask::GetWarning(std::shared_ptr<Material> material, std::shared_ptr<Node> node) const
{
	int counter = 0;
	for (int i = 0; i < 4; i++)
	{
		if (node->Properties[i]->Floats[0] > 0)
			counter++;
	}
	if (counter == 0)
		return WarningType::WrongProperty;

	auto type1 = material->GetDesiredPinType(node->InputPins[0], std::unordered_set<std::shared_ptr<Pin>>());

	if (type1 == ValueType::Float1)
		return WarningType::None;

	if (type1 == ValueType::Float2 && (node->Properties[2]->Floats[2] > 0 || node->Properties[3]->Floats[3] > 0))
		return WarningType::WrongInputType;

	if (type1 == ValueType::Float3 && (node->Properties[1]->Floats[3] > 0))
		return WarningType::WrongInputType;

	return WarningType::None;
}

ValueType
NodeAppend::GetOutputType(std::shared_ptr<Material> material, std::shared_ptr<Node> node, const std::vector<ValueType>& inputTypes) const
{
	auto type1 = material->GetDesiredPinType(node->InputPins[0], std::unordered_set<std::shared_ptr<Pin>>());
	auto type2 = material->GetDesiredPinType(node->InputPins[1], std::unordered_set<std::shared_ptr<Pin>>());

	int counter = 0;

	auto count = std::min(4, GetElementCount(type1) + GetElementCount(type2));

	return static_cast<ValueType>(static_cast<int>(ValueType::Float1) + count - 1);
}

WarningType NodeAppend::GetWarning(std::shared_ptr<Material> material, std::shared_ptr<Node> node) const
{
	auto connected1 = material->GetConnectedPins(node->InputPins[0]);
	auto connected2 = material->GetConnectedPins(node->InputPins[1]);

	if (connected1.size() == 0)
		return WarningType::WrongInputType;

	if (connected2.size() == 0)
		return WarningType::WrongInputType;

	auto type1 = material->GetDesiredPinType(node->InputPins[0], std::unordered_set<std::shared_ptr<Pin>>());
	auto type2 = material->GetDesiredPinType(node->InputPins[1], std::unordered_set<std::shared_ptr<Pin>>());

	if (GetElementCount(type1) + GetElementCount(type2) > 4)
	{
		return WarningType::WrongInputType;
	}

	return WarningType::None;
}

} // namespace EffekseerMaterial