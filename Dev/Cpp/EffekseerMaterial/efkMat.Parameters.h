
#pragma once

#include "efkMat.Base.h"

namespace EffekseerMaterial
{

struct ExtractedTextureParameter
{
	//! parameter GUID
	uint64_t GUID = 0;

	//! fixed path
	std::string Path;

	//! sampler
	TextureSamplerType Sampler;
};

//! extract output texture or sampled texture
bool ExtractTextureParameter(std::shared_ptr<Material> material, std::shared_ptr<Node> node, ExtractedTextureParameter& result);

enum class ShadingModelType
{
	Lit,
	Unlit,
};

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

class NodeParameterBehaviorComponent
{
public:
	bool IsGetHeaderInherited = false;

	virtual std::string
	GetHeader(std::shared_ptr<Material> material, std::shared_ptr<NodeParameter> parameter, std::shared_ptr<Node> node) const
	{
		return "";
	}
};

class NodeParameterBehaviorComponentTwoInputMath : public NodeParameterBehaviorComponent
{
public:
	NodeParameterBehaviorComponentTwoInputMath() { IsGetHeaderInherited = true; }

	std::string
	GetHeader(std::shared_ptr<Material> material, std::shared_ptr<NodeParameter> parameter, std::shared_ptr<Node> node) const override;
};

class NodeParameterBehaviorComponentMask : public NodeParameterBehaviorComponent
{
public:
	NodeParameterBehaviorComponentMask() { IsGetHeaderInherited = true; }

	std::string
	GetHeader(std::shared_ptr<Material> material, std::shared_ptr<NodeParameter> parameter, std::shared_ptr<Node> node) const override;
};

class NodeParameterBehaviorComponentName : public NodeParameterBehaviorComponent
{
public:
	NodeParameterBehaviorComponentName() { IsGetHeaderInherited = true; }

	std::string
	GetHeader(std::shared_ptr<Material> material, std::shared_ptr<NodeParameter> parameter, std::shared_ptr<Node> node) const override;
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

	//! get warning about a sampler
	WarningType GetWarningSampler(std::shared_ptr<Material> material, std::shared_ptr<Node> node) const;

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

		auto val1 = std::make_shared<NodePropertyParameter>();
		val1->Name = "ConstValue1";
		val1->Type = ValueType::Float1;
		Properties.push_back(val1);

		auto val2 = std::make_shared<NodePropertyParameter>();
		val2->Name = "ConstValue2";
		val2->Type = ValueType::Float1;
		Properties.push_back(val2);
	}

public:
	std::vector<std::shared_ptr<NodeParameterBehaviorComponent>> BehaviorComponents;

	NodeType Type;
	std::string TypeName;
	std::string Description;
	std::vector<std::string> Group;
	std::vector<std::string> Keywords;

	std::vector<std::shared_ptr<PinParameter>> InputPins;
	std::vector<std::shared_ptr<PinParameter>> OutputPins;

	std::vector<std::shared_ptr<NodePropertyParameter>> Properties;
	std::vector<std::shared_ptr<NodeFunctionParameter>> Funcs;

	//! is preview opened as default
	bool IsPreviewOpened = false;

	//! has a description for other editor
	bool HasDescription = false;

	int32_t GetPropertyIndex(const std::string& name)
	{
		for (size_t i = 0; i < Properties.size(); i++)
		{
			if (Properties[i]->Name == name)
			{
				return static_cast<int32_t>(i);
			}
		}
		return -1;
	}

	virtual ValueType
	GetOutputType(std::shared_ptr<Material> material, std::shared_ptr<Node> node, const std::vector<ValueType>& inputTypes) const
	{
		return ValueType::Unknown;
	}

	virtual std::string GetHeader(std::shared_ptr<Material> material, std::shared_ptr<Node> node) const;

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

class NodeConstant3 : public NodeParameter
{
public:
	NodeConstant3()
	{
		Type = NodeType::Constant3;
		TypeName = "Constant3";
		Description = "Constant value...";
		Group = std::vector<std::string>{"Constant"};

		auto output = std::make_shared<PinParameter>();
		output->Name = "Output";
		output->Type = ValueType::Float3;
		OutputPins.push_back(output);

		auto param = std::make_shared<NodePropertyParameter>();
		param->Name = "Value";
		param->Type = ValueType::Float3;
		Properties.push_back(param);
	}
};

class NodeConstant4 : public NodeParameter
{
public:
	NodeConstant4()
	{
		Type = NodeType::Constant4;
		TypeName = "Constant4";
		Description = "Constant value...";
		Group = std::vector<std::string>{"Constant"};

		auto output = std::make_shared<PinParameter>();
		output->Name = "Output";
		output->Type = ValueType::Float4;
		OutputPins.push_back(output);

		auto param = std::make_shared<NodePropertyParameter>();
		param->Name = "Value";
		param->Type = ValueType::Float4;
		Properties.push_back(param);
	}
};

class NodeParameter1 : public NodeParameter
{
public:
	NodeParameter1()
	{
		Type = NodeType::Parameter1;
		TypeName = "Parameter1";
		Description = "Param value...";
		Group = std::vector<std::string>{"Parameter"};
		HasDescription = true;

		auto output = std::make_shared<PinParameter>();
		output->Name = "Output";
		output->Type = ValueType::Float1;
		OutputPins.push_back(output);

		auto paramName = std::make_shared<NodePropertyParameter>();
		paramName->Name = "Name";
		paramName->Type = ValueType::String;
		Properties.push_back(paramName);

		auto paramPriority = std::make_shared<NodePropertyParameter>();
		paramPriority->Name = "Priority";
		paramPriority->Type = ValueType::Int;
		paramPriority->DefaultValues[0] = 1;
		Properties.push_back(paramPriority);

		auto param = std::make_shared<NodePropertyParameter>();
		param->Name = "Value";
		param->Type = ValueType::Float1;
		Properties.push_back(param);

		BehaviorComponents = {std::make_shared<NodeParameterBehaviorComponentName>()};
	}
};

class NodeParameter4 : public NodeParameter
{
public:
	NodeParameter4()
	{
		Type = NodeType::Parameter4;
		TypeName = "Parameter4";
		Description = "Param value...";
		Group = std::vector<std::string>{"Parameter"};
		HasDescription = true;

		auto output = std::make_shared<PinParameter>();
		output->Name = "Output";
		output->Type = ValueType::Float4;
		OutputPins.push_back(output);

		auto paramName = std::make_shared<NodePropertyParameter>();
		paramName->Name = "Name";
		paramName->Type = ValueType::String;
		Properties.push_back(paramName);

		auto paramPriority = std::make_shared<NodePropertyParameter>();
		paramPriority->Name = "Priority";
		paramPriority->Type = ValueType::Int;
		paramPriority->DefaultValues[0] = 1;
		Properties.push_back(paramPriority);

		auto param = std::make_shared<NodePropertyParameter>();
		param->Name = "Value";
		param->Type = ValueType::Float4;
		Properties.push_back(param);

		BehaviorComponents = {std::make_shared<NodeParameterBehaviorComponentName>()};
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

		BehaviorComponents = {std::make_shared<NodeParameterBehaviorComponentMask>()};
	}

	ValueType
	GetOutputType(std::shared_ptr<Material> material, std::shared_ptr<Node> node, const std::vector<ValueType>& inputTypes) const override;
	WarningType GetWarning(std::shared_ptr<Material> material, std::shared_ptr<Node> node) const override;
};

class NodeAppendVector : public NodeParameter
{
public:
	NodeAppendVector()
	{
		Type = NodeType::AppendVector;
		TypeName = "AppendVector";
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
		Keywords.emplace_back("+");

		InitializeAsIn2Out1Param2();

		BehaviorComponents = {std::make_shared<NodeParameterBehaviorComponentTwoInputMath>()};
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
		Keywords.emplace_back("-");

		InitializeAsIn2Out1Param2();

		BehaviorComponents = {std::make_shared<NodeParameterBehaviorComponentTwoInputMath>()};
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
		Keywords.emplace_back("*");

		InitializeAsIn2Out1Param2();

		BehaviorComponents = {std::make_shared<NodeParameterBehaviorComponentTwoInputMath>()};
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
		Keywords.emplace_back("/");

		InitializeAsIn2Out1Param2();

		BehaviorComponents = {std::make_shared<NodeParameterBehaviorComponentTwoInputMath>()};
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

class NodeFmod : public NodeParameter
{
public:
	NodeFmod()
	{
		Type = NodeType::FMod;
		TypeName = "Fmod";
		Group = std::vector<std::string>{"Math"};

		InitializeAsIn2Out1Param2();

		BehaviorComponents = {std::make_shared<NodeParameterBehaviorComponentTwoInputMath>()};
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

class NodeCeil : public NodeParameter
{
public:
	NodeCeil()
	{
		Type = NodeType::Ceil;
		TypeName = "Ceil";
		Group = std::vector<std::string>{"Math"};

		InitializeAstOutputTypeIn1Out1();
	}

	ValueType
	GetOutputType(std::shared_ptr<Material> material, std::shared_ptr<Node> node, const std::vector<ValueType>& inputTypes) const override
	{
		return GetOutputTypeIn1Out1(inputTypes);
	}
};

class NodeFloor : public NodeParameter
{
public:
	NodeFloor()
	{
		Type = NodeType::Floor;
		TypeName = "Floor";
		Group = std::vector<std::string>{"Math"};

		InitializeAstOutputTypeIn1Out1();
	}

	ValueType
	GetOutputType(std::shared_ptr<Material> material, std::shared_ptr<Node> node, const std::vector<ValueType>& inputTypes) const override
	{
		return GetOutputTypeIn1Out1(inputTypes);
	}
};

class NodeFrac : public NodeParameter
{
public:
	NodeFrac()
	{
		Type = NodeType::Frac;
		TypeName = "Frac";
		Group = std::vector<std::string>{"Math"};

		InitializeAstOutputTypeIn1Out1();
	}

	ValueType
	GetOutputType(std::shared_ptr<Material> material, std::shared_ptr<Node> node, const std::vector<ValueType>& inputTypes) const override
	{
		return GetOutputTypeIn1Out1(inputTypes);
	}
};

class NodeTextureCoordinate : public NodeParameter
{
public:
	NodeTextureCoordinate()
	{
		Type = NodeType::TextureCoordinate;
		TypeName = "TextureCoordinate";
		Group = std::vector<std::string>{"Model"};

		auto output = std::make_shared<PinParameter>();
		output->Name = "Output";
		output->Type = ValueType::Float2;
		OutputPins.push_back(output);

		auto param = std::make_shared<NodePropertyParameter>();
		param->Name = "UVIndex";
		param->Type = ValueType::Enum;
		param->DefaultValues[0] = 0;
		Properties.push_back(param);
	}
};

class NodePanner : public NodeParameter
{
public:
	NodePanner()
	{
		Type = NodeType::Panner;
		TypeName = "Panner";
		Group = std::vector<std::string>{"Model"};

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

		auto param = std::make_shared<NodePropertyParameter>();
		param->Name = "UVIndex";
		param->Type = ValueType::Enum;
		param->DefaultValues[0] = 0;
		Properties.push_back(param);
	}
};

class NodeTextureObject : public NodeParameter
{
public:
	NodeTextureObject()
	{
		Type = NodeType::TextureObject;
		TypeName = "TextureObject";
		Group = std::vector<std::string>{"Texture"};

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

class NodeTextureObjectParameter : public NodeParameter
{
public:
	NodeTextureObjectParameter()
	{
		Type = NodeType::TextureObjectParameter;
		TypeName = "TextureObjectParameter";
		Group = std::vector<std::string>{"Texture"};
		HasDescription = true;

		auto output = std::make_shared<PinParameter>();
		output->Name = "Output";
		output->Type = ValueType::Texture;
		OutputPins.push_back(output);

		auto paramName = std::make_shared<NodePropertyParameter>();
		paramName->Name = "Name";
		paramName->Type = ValueType::String;
		Properties.push_back(paramName);

		auto paramPriority = std::make_shared<NodePropertyParameter>();
		paramPriority->Name = "Priority";
		paramPriority->Type = ValueType::Int;
		paramPriority->DefaultValues[0] = 1;
		Properties.push_back(paramPriority);

		auto param = std::make_shared<NodePropertyParameter>();
		param->Name = "Value";
		param->Type = ValueType::Texture;
		Properties.push_back(param);

		BehaviorComponents = {std::make_shared<NodeParameterBehaviorComponentName>()};
	}
};

class NodeSampleTexture : public NodeParameter
{
public:
	NodeSampleTexture()
	{
		Type = NodeType::SampleTexture;
		TypeName = "SampleTexture";
		Group = std::vector<std::string>{"Texture"};

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

		auto paramSampler = std::make_shared<NodePropertyParameter>();
		paramSampler->Name = "Sampler";
		paramSampler->Type = ValueType::Enum;
		paramSampler->DefaultValues[0] = 0;
		Properties.push_back(paramSampler);

		IsPreviewOpened = true;
	}

	WarningType GetWarning(std::shared_ptr<Material> material, std::shared_ptr<Node> node) const
	{
		return GetWarningSampler(material, node);
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

class NodeVertexNormalWS : public NodeParameter
{
public:
	NodeVertexNormalWS()
	{
		Type = NodeType::VertexNormalWS;
		TypeName = "VertexNormalWS";
		Group = std::vector<std::string>{"Model"};

		auto output = std::make_shared<PinParameter>();
		output->Name = "Output";
		output->Type = ValueType::Float3;
		OutputPins.push_back(output);
	}
};

class NodePixelNormalWS : public NodeParameter
{
public:
	NodePixelNormalWS()
	{
		Type = NodeType::PixelNormalWS;
		TypeName = "PixelNormalWS";
		Group = std::vector<std::string>{"Model"};

		auto output = std::make_shared<PinParameter>();
		output->Name = "Output";
		output->Type = ValueType::Float3;
		OutputPins.push_back(output);
	}
};

#ifdef _DEBUG
class NodeVertexTangentWS : public NodeParameter
{
public:
	NodeVertexTangentWS()
	{
		Type = NodeType::VertexTangentWS;
		TypeName = "VertexTangentWS";
		Group = std::vector<std::string>{"Model"};

		auto output = std::make_shared<PinParameter>();
		output->Name = "Output";
		output->Type = ValueType::Float3;
		OutputPins.push_back(output);
	}
};
#endif

class NodeCustomData1 : public NodeParameter
{
public:
	NodeCustomData1()
	{
		Type = NodeType::CustomData1;
		TypeName = "CustomData1";
		Group = std::vector<std::string>{"Parameter"};

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

		BehaviorComponents = {std::make_shared<NodeParameterBehaviorComponentMask>()};
	}

	ValueType
	GetOutputType(std::shared_ptr<Material> material, std::shared_ptr<Node> node, const std::vector<ValueType>& inputTypes) const override;
};

class NodeCustomData2 : public NodeParameter
{
public:
	NodeCustomData2()
	{
		Type = NodeType::CustomData2;
		TypeName = "CustomData2";
		Group = std::vector<std::string>{"Parameter"};

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

		BehaviorComponents = {std::make_shared<NodeParameterBehaviorComponentMask>()};
	}

	ValueType
	GetOutputType(std::shared_ptr<Material> material, std::shared_ptr<Node> node, const std::vector<ValueType>& inputTypes) const override;
};

class NodeComment : public NodeParameter
{
public:
	NodeComment()
	{
		Type = NodeType::Comment;
		TypeName = "Comment";

		auto paramName = std::make_shared<NodePropertyParameter>();
		paramName->Name = "Comment";
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
		IsPreviewOpened = true;
		HasDescription = true;

		auto baseColor = std::make_shared<PinParameter>();
		baseColor->Name = "BaseColor";
		baseColor->Type = ValueType::Float3;
		baseColor->Default = DefaultType::Value;
		baseColor->DefaultValues.fill(0.5f);
		baseColor->DefaultValues[3] = 1.0f;
		InputPins.push_back(baseColor);

		auto emissive = std::make_shared<PinParameter>();
		emissive->Name = "Emissive";
		emissive->Type = ValueType::Float3;
		emissive->Default = DefaultType::Value;
		emissive->DefaultValues.fill(0.5f);
		emissive->DefaultValues[3] = 1.0f;
		InputPins.push_back(emissive);

		auto opacity = std::make_shared<PinParameter>();
		opacity->Name = "Opacity";
		opacity->Type = ValueType::Float1;
		opacity->Default = DefaultType::Value;
		opacity->DefaultValues.fill(1.0f);
		InputPins.push_back(opacity);

		auto opacityMask = std::make_shared<PinParameter>();
		opacityMask->Name = "OpacityMask";
		opacityMask->Type = ValueType::Float1;
		opacityMask->Default = DefaultType::Value;
		opacityMask->DefaultValues.fill(1.0f);
		InputPins.push_back(opacityMask);

		auto normal = std::make_shared<PinParameter>();
		normal->Name = "Normal";
		normal->Type = ValueType::Float3;
		normal->Default = DefaultType::Value;
		normal->DefaultValues.fill(0.5f);
		normal->DefaultValues[2] = 1.0f;
		InputPins.push_back(normal);

		auto metallic = std::make_shared<PinParameter>();
		metallic->Name = "Metallic";
		metallic->Type = ValueType::Float1;
		metallic->Default = DefaultType::Value;
		metallic->DefaultValues.fill(0.5f);
		InputPins.push_back(metallic);

		auto roughness = std::make_shared<PinParameter>();
		roughness->Name = "Roughness";
		roughness->Type = ValueType::Float1;
		roughness->Default = DefaultType::Value;
		roughness->DefaultValues.fill(0.5f);
		InputPins.push_back(roughness);

		auto ambientOcclusion = std::make_shared<PinParameter>();
		ambientOcclusion->Name = "AmbientOcclusion";
		ambientOcclusion->Type = ValueType::Float1;
		ambientOcclusion->Default = DefaultType::Value;
		ambientOcclusion->DefaultValues.fill(1.0f);
		InputPins.push_back(ambientOcclusion);

		auto refraction = std::make_shared<PinParameter>();
		refraction->Name = "Refraction";
		refraction->Type = ValueType::Float1;
		refraction->Default = DefaultType::Value;
		refraction->DefaultValues.fill(0.0f);
		InputPins.push_back(refraction);

		auto worldPositionOffset = std::make_shared<PinParameter>();
		worldPositionOffset->Name = "WorldPositionOffset";
		worldPositionOffset->Type = ValueType::Float3;
		worldPositionOffset->Default = DefaultType::Value;
		worldPositionOffset->DefaultValues.fill(0.0f);
		InputPins.push_back(worldPositionOffset);

		auto shadingProperty = std::make_shared<NodePropertyParameter>();
		shadingProperty->Name = "ShadingModel";
		shadingProperty->Type = ValueType::Enum;
		shadingProperty->DefaultValues.fill(1.0f);
		Properties.push_back(shadingProperty);
	}
};
} // namespace EffekseerMaterial
