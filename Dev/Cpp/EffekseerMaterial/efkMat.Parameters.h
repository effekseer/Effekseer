
#pragma once

#include "efkMat.Base.h"

namespace EffekseerMaterial
{
class PinParameter
{
public:
	std::string Name;
	std::string Description;
	ValueType Type;
	DefaultType Default;
	std::array<float, 4> DefaultValues;

	PinParameter() { DefaultValues.fill(0.0f); }
};

class NodePropertyParameter
{
public:
	std::string Name;
	std::string Description;
	ValueType Type;
	std::array<float, 4> DefaultValues;

	NodePropertyParameter() { DefaultValues.fill(0.0f); }
};

class NodeFunctionParameter
{
public:
	std::string Name;
	std::string Description;
	std::function<bool(std::shared_ptr<Material>, std::shared_ptr<Node>)> Func;
};

class NodeParameter
{
protected:
	//! input 1, output 1, input type and output type are same
	ValueType GetOutputTypeIn1Out1(const std::vector<ValueType>& inputTypes) const;

	/*!
	@note
		input 2, output 1, parameter 2.
		parameter types are float1
		input type and output type are floatN and float1.
		input type and output type are float1 and floatN.
		input type and output type are same.
	*/
	ValueType GetOutputTypeIn2Out1Param2(const std::vector<ValueType>& inputTypes) const;

	/*!
	@note
	input 2, output 1, parameter 2.
	parameter types are float1
	input type and output type are floatN and float1.
	input type and output type are float1 and floatN.
	input type and output type are same.
	*/
	WarningType GetWarningIn2Out1Param2(std::shared_ptr<Material> material, std::shared_ptr<Node> node) const;

	void InitializeAstOutputTypeIn1Out1()
	{
		auto input = std::make_shared<PinParameter>();
		input->Name = "Value";
		input->Type = ValueType::FloatN;
		InputPins.push_back(input);

		auto output = std::make_shared<PinParameter>();
		output->Name = "Output";
		output->Type = ValueType::FloatN;
		OutputPins.push_back(output);
	}

	void InitializeAsIn2Out1Param2()
	{
		auto input1 = std::make_shared<PinParameter>();
		input1->Name = "A";
		input1->Type = ValueType::FloatN;
		InputPins.push_back(input1);

		auto input2 = std::make_shared<PinParameter>();
		input2->Name = "B";
		input2->Type = ValueType::FloatN;
		InputPins.push_back(input2);

		auto output = std::make_shared<PinParameter>();
		output->Name = "Output";
		output->Type = ValueType::FloatN;
		OutputPins.push_back(output);

		auto val1 = std::make_shared<NodePropertyParameter>();
		val1->Name = "Value1";
		val1->Type = ValueType::Float1;
		Properties.push_back(val1);

		auto val2 = std::make_shared<NodePropertyParameter>();
		val2->Name = "Value2";
		val2->Type = ValueType::Float1;
		Properties.push_back(val2);
	}

public:
	NodeType Type;
	std::string TypeName;
	std::string Description;
	std::vector<std::string> Group;

	std::vector<std::shared_ptr<PinParameter>> InputPins;
	std::vector<std::shared_ptr<PinParameter>> OutputPins;

	std::vector<std::shared_ptr<NodePropertyParameter>> Properties;
	std::vector<std::shared_ptr<NodeFunctionParameter>> Funcs;

	//! default value
	bool IsOpened = false;

	virtual ValueType
	GetOutputType(std::shared_ptr<Material> material, std::shared_ptr<Node> node, const std::vector<ValueType>& inputTypes) const
	{
		return ValueType::Unknown;
	}
	virtual WarningType GetWarning(std::shared_ptr<Material> material, std::shared_ptr<Node> node) const { return WarningType::None; }
};

class NodeConstant1 : public NodeParameter
{
public:
	NodeConstant1();
};

class NodeConstant2 : public NodeParameter
{
public:
	NodeConstant2()
	{
		Type = NodeType::Constant2;
		TypeName = "Constant2";
		Description = "Constant value...";
		Group = std::vector<std::string>{"Constant"};

		auto output = std::make_shared<PinParameter>();
		output->Name = "Output";
		output->Type = ValueType::Float2;
		OutputPins.push_back(output);

		auto param = std::make_shared<NodePropertyParameter>();
		param->Name = "Value";
		param->Type = ValueType::Float2;
		Properties.push_back(param);
	}
};

class NodeParam1 : public NodeParameter
{
public:
	NodeParam1()
	{
		Type = NodeType::Param1;
		TypeName = "Param1";
		Description = "Param value...";
		Group = std::vector<std::string>{"Param"};

		auto output = std::make_shared<PinParameter>();
		output->Name = "Output";
		output->Type = ValueType::Float1;
		OutputPins.push_back(output);

		auto paramName = std::make_shared<NodePropertyParameter>();
		paramName->Name = "Name";
		paramName->Type = ValueType::String;
		Properties.push_back(paramName);

		auto param = std::make_shared<NodePropertyParameter>();
		param->Name = "Value";
		param->Type = ValueType::Float1;
		Properties.push_back(param);
	}
};

class NodeParam4 : public NodeParameter
{
public:
	NodeParam4()
	{
		Type = NodeType::Param4;
		TypeName = "Param4";
		Description = "Param value...";
		Group = std::vector<std::string>{"Param"};

		auto output = std::make_shared<PinParameter>();
		output->Name = "Output";
		output->Type = ValueType::Float4;
		OutputPins.push_back(output);

		auto paramName = std::make_shared<NodePropertyParameter>();
		paramName->Name = "Name";
		paramName->Type = ValueType::String;
		Properties.push_back(paramName);

		auto param = std::make_shared<NodePropertyParameter>();
		param->Name = "Value";
		param->Type = ValueType::Float4;
		Properties.push_back(param);
	}
};

class NodeAbs : public NodeParameter
{
public:
	NodeAbs()
	{
		Type = NodeType::Abs;
		TypeName = "Abs";
		Group = std::vector<std::string>{"Math"};

		InitializeAstOutputTypeIn1Out1();
	}

	ValueType
	GetOutputType(std::shared_ptr<Material> material, std::shared_ptr<Node> node, const std::vector<ValueType>& inputTypes) const override
	{
		return GetOutputTypeIn1Out1(inputTypes);
	}
};

class NodeOneMinus : public NodeParameter
{
public:
	NodeOneMinus()
	{
		Type = NodeType::OneMinus;
		TypeName = "OneMinus";
		Group = std::vector<std::string>{"Math"};

		InitializeAstOutputTypeIn1Out1();
	}

	ValueType
	GetOutputType(std::shared_ptr<Material> material, std::shared_ptr<Node> node, const std::vector<ValueType>& inputTypes) const override
	{
		return GetOutputTypeIn1Out1(inputTypes);
	}
};

class NodeSine : public NodeParameter
{
public:
	NodeSine()
	{
		Type = NodeType::Sine;
		TypeName = "Sine";
		Group = std::vector<std::string>{"Math"};

		InitializeAstOutputTypeIn1Out1();
	}

	ValueType
	GetOutputType(std::shared_ptr<Material> material, std::shared_ptr<Node> node, const std::vector<ValueType>& inputTypes) const override
	{
		return GetOutputTypeIn1Out1(inputTypes);
	}
};

class NodeComponentMask : public NodeParameter
{
public:
	NodeComponentMask()
	{
		Type = NodeType::ComponentMask;
		TypeName = "ComponentMask";
		Group = std::vector<std::string>{"Math"};

		auto input = std::make_shared<PinParameter>();
		input->Name = "Value";
		input->Type = ValueType::FloatN;
		InputPins.push_back(input);

		auto output = std::make_shared<PinParameter>();
		output->Name = "Output";
		output->Type = ValueType::FloatN;
		OutputPins.push_back(output);

		auto val1 = std::make_shared<NodePropertyParameter>();
		val1->Name = "R";
		val1->Type = ValueType::Bool;
		val1->DefaultValues[0] = 1.0f;
		Properties.push_back(val1);

		auto val2 = std::make_shared<NodePropertyParameter>();
		val2->Name = "G";
		val2->Type = ValueType::Bool;
		val2->DefaultValues[0] = 1.0f;
		Properties.push_back(val2);

		auto val3 = std::make_shared<NodePropertyParameter>();
		val3->Name = "B";
		val3->Type = ValueType::Bool;
		Properties.push_back(val3);

		auto val4 = std::make_shared<NodePropertyParameter>();
		val4->Name = "A";
		val4->Type = ValueType::Bool;
		Properties.push_back(val4);
	}

	ValueType
	GetOutputType(std::shared_ptr<Material> material, std::shared_ptr<Node> node, const std::vector<ValueType>& inputTypes) const override;
	WarningType GetWarning(std::shared_ptr<Material> material, std::shared_ptr<Node> node) const override;
};

class NodeAppend : public NodeParameter
{
public:
	NodeAppend()
	{
		Type = NodeType::Append;
		TypeName = "Append";
		Group = std::vector<std::string>{"Math"};

		auto input1 = std::make_shared<PinParameter>();
		input1->Name = "Value1";
		input1->Type = ValueType::FloatN;
		InputPins.push_back(input1);

		auto input2 = std::make_shared<PinParameter>();
		input2->Name = "Value2";
		input2->Type = ValueType::FloatN;
		InputPins.push_back(input2);

		auto output = std::make_shared<PinParameter>();
		output->Name = "Output";
		output->Type = ValueType::FloatN;
		OutputPins.push_back(output);
	}

	ValueType
	GetOutputType(std::shared_ptr<Material> material, std::shared_ptr<Node> node, const std::vector<ValueType>& inputTypes) const override;
	WarningType GetWarning(std::shared_ptr<Material> material, std::shared_ptr<Node> node) const override;
};

class NodeAdd : public NodeParameter
{
public:
	NodeAdd()
	{
		Type = NodeType::Add;
		TypeName = "Add";
		Group = std::vector<std::string>{"Math"};

		InitializeAsIn2Out1Param2();
	}

	ValueType
	GetOutputType(std::shared_ptr<Material> material, std::shared_ptr<Node> node, const std::vector<ValueType>& inputTypes) const override
	{
		return GetOutputTypeIn2Out1Param2(inputTypes);
	}
	WarningType GetWarning(std::shared_ptr<Material> material, std::shared_ptr<Node> node) const override
	{
		return GetWarningIn2Out1Param2(material, node);
	}
};

class NodeSubtract : public NodeParameter
{
public:
	NodeSubtract()
	{
		Type = NodeType::Subtract;
		TypeName = "Subtract";
		Group = std::vector<std::string>{"Math"};

		InitializeAsIn2Out1Param2();
	}

	ValueType
	GetOutputType(std::shared_ptr<Material> material, std::shared_ptr<Node> node, const std::vector<ValueType>& inputTypes) const override
	{
		return GetOutputTypeIn2Out1Param2(inputTypes);
	}
	WarningType GetWarning(std::shared_ptr<Material> material, std::shared_ptr<Node> node) const override
	{
		return GetWarningIn2Out1Param2(material, node);
	}
};

class NodeMultiply : public NodeParameter
{
public:
	NodeMultiply()
	{
		Type = NodeType::Multiply;
		TypeName = "Multiply";
		Group = std::vector<std::string>{"Math"};

		InitializeAsIn2Out1Param2();
	}

	ValueType
	GetOutputType(std::shared_ptr<Material> material, std::shared_ptr<Node> node, const std::vector<ValueType>& inputTypes) const override
	{
		return GetOutputTypeIn2Out1Param2(inputTypes);
	}
	WarningType GetWarning(std::shared_ptr<Material> material, std::shared_ptr<Node> node) const override
	{
		return GetWarningIn2Out1Param2(material, node);
	}
};

class NodeDivide : public NodeParameter
{
public:
	NodeDivide()
	{
		Type = NodeType::Divide;
		TypeName = "Divide";
		Group = std::vector<std::string>{"Math"};

		InitializeAsIn2Out1Param2();
	}

	ValueType
	GetOutputType(std::shared_ptr<Material> material, std::shared_ptr<Node> node, const std::vector<ValueType>& inputTypes) const override
	{
		return GetOutputTypeIn2Out1Param2(inputTypes);
	}
	WarningType GetWarning(std::shared_ptr<Material> material, std::shared_ptr<Node> node) const override
	{
		return GetWarningIn2Out1Param2(material, node);
	}
};

class NodeFMod : public NodeParameter
{
public:
	NodeFMod()
	{
		Type = NodeType::FMod;
		TypeName = "FMod";
		Group = std::vector<std::string>{"Math"};

		InitializeAsIn2Out1Param2();
	}

	ValueType
	GetOutputType(std::shared_ptr<Material> material, std::shared_ptr<Node> node, const std::vector<ValueType>& inputTypes) const override
	{
		return GetOutputTypeIn2Out1Param2(inputTypes);
	}
	WarningType GetWarning(std::shared_ptr<Material> material, std::shared_ptr<Node> node) const override
	{
		return GetWarningIn2Out1Param2(material, node);
	}
};

class NodeUV : public NodeParameter
{
public:
	NodeUV()
	{
		Type = NodeType::UV;
		TypeName = "UV";
		Group = std::vector<std::string>{"Coordinate"};

		auto output = std::make_shared<PinParameter>();
		output->Name = "Output";
		output->Type = ValueType::Float2;
		OutputPins.push_back(output);
	}
};

class NodePanner : public NodeParameter
{
public:
	NodePanner()
	{
		Type = NodeType::Panner;
		TypeName = "Panner";
		Group = std::vector<std::string>{"Coordinate"};

		auto input1 = std::make_shared<PinParameter>();
		input1->Name = "UV";
		input1->Type = ValueType::Float2;
		input1->Default = DefaultType::UV;
		InputPins.push_back(input1);

		auto input2 = std::make_shared<PinParameter>();
		input2->Name = "Time";
		input2->Type = ValueType::Float1;
		input2->Default = DefaultType::Time;
		InputPins.push_back(input2);

		auto input3 = std::make_shared<PinParameter>();
		input3->Name = "Speed";
		input3->Type = ValueType::Float2;
		InputPins.push_back(input3);

		auto output = std::make_shared<PinParameter>();
		output->Name = "Output";
		output->Type = ValueType::Float2;
		OutputPins.push_back(output);

		auto val1 = std::make_shared<NodePropertyParameter>();
		val1->Name = "Speed";
		val1->Type = ValueType::Float2;
		Properties.push_back(val1);
	}
};

class NodeConstantTexture : public NodeParameter
{
public:
	NodeConstantTexture()
	{
		Type = NodeType::ConstantTexture;
		TypeName = "ConstantTexture";

		auto output = std::make_shared<PinParameter>();
		output->Name = "Output";
		output->Type = ValueType::Texture;
		OutputPins.push_back(output);

		auto param = std::make_shared<NodePropertyParameter>();
		param->Name = "Value";
		param->Type = ValueType::Texture;
		Properties.push_back(param);
	}
};

class NodeParamTexture : public NodeParameter
{
public:
	NodeParamTexture()
	{
		Type = NodeType::ParamTexture;
		TypeName = "ParamTexture";

		auto output = std::make_shared<PinParameter>();
		output->Name = "Output";
		output->Type = ValueType::Texture;
		OutputPins.push_back(output);

		auto paramName = std::make_shared<NodePropertyParameter>();
		paramName->Name = "Name";
		paramName->Type = ValueType::String;
		Properties.push_back(paramName);

		auto param = std::make_shared<NodePropertyParameter>();
		param->Name = "Value";
		param->Type = ValueType::Texture;
		Properties.push_back(param);
	}
};

class NodeSampleTexture : public NodeParameter
{
public:
	NodeSampleTexture()
	{
		Type = NodeType::SampleTexture;
		TypeName = "SampleTexture";

		auto inputTexture = std::make_shared<PinParameter>();
		inputTexture->Name = "Texture";
		inputTexture->Type = ValueType::Texture;
		InputPins.push_back(inputTexture);

		auto inputUV = std::make_shared<PinParameter>();
		inputUV->Name = "UV";
		inputUV->Type = ValueType::Float2;
		inputUV->Default = DefaultType::UV;
		InputPins.push_back(inputUV);

		auto output = std::make_shared<PinParameter>();
		output->Name = "Output";
		output->Type = ValueType::Float4;
		OutputPins.push_back(output);

		auto param = std::make_shared<NodePropertyParameter>();
		param->Name = "Texture";
		param->Type = ValueType::Texture;
		Properties.push_back(param);

		IsOpened = true;
	}
};

class NodeTime : public NodeParameter
{
public:
	NodeTime()
	{
		Type = NodeType::Time;
		TypeName = "Time";
		Group = std::vector<std::string>{"Constant"};

		auto output = std::make_shared<PinParameter>();
		output->Name = "Output";
		output->Type = ValueType::Float1;
		OutputPins.push_back(output);
	}
};

class NodeComment : public NodeParameter
{
public:
	NodeComment()
	{
		Type = NodeType::Comment;
		TypeName = "Comment";

		auto paramName = std::make_shared<NodePropertyParameter>();
		paramName->Name = "Name";
		paramName->Type = ValueType::String;
		Properties.push_back(paramName);
	}
};

class NodeFunction : public NodeParameter
{
public:
	NodeFunction()
	{
		Type = NodeType::Function;
		TypeName = "Function";

		auto paramName = std::make_shared<NodePropertyParameter>();
		paramName->Name = "Name";
		paramName->Type = ValueType::Function;
		Properties.push_back(paramName);
	}
};

class NodeOutput : public NodeParameter
{
public:
	NodeOutput()
	{
		Type = NodeType::Output;
		TypeName = "Output";

		auto emissive = std::make_shared<PinParameter>();
		emissive->Name = "Emissive";
		emissive->Type = ValueType::Float4;
		emissive->Default = DefaultType::Value;
		emissive->DefaultValues.fill(0.5f);
		emissive->DefaultValues[3] = 1.0f;
		InputPins.push_back(emissive);

		auto normal = std::make_shared<PinParameter>();
		normal->Name = "Normal";
		normal->Type = ValueType::Float3;
		normal->Default = DefaultType::Value;
		normal->DefaultValues.fill(0.0f);
		normal->DefaultValues[2] = 1.0f;
		InputPins.push_back(normal);

		auto refraction = std::make_shared<PinParameter>();
		refraction->Name = "Refraction";
		refraction->Type = ValueType::Float1;
		refraction->Default = DefaultType::Value;
		refraction->DefaultValues.fill(0.0f);
		InputPins.push_back(refraction);

		auto worldOffset = std::make_shared<PinParameter>();
		worldOffset->Name = "WorldOffset";
		worldOffset->Type = ValueType::Float1;
		worldOffset->Default = DefaultType::Value;
		worldOffset->DefaultValues.fill(0.0f);
		InputPins.push_back(worldOffset);
	}
};
} // namespace EffekseerMaterial
