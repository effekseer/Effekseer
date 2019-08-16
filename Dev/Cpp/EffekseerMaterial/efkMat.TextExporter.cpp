#include "efkMat.TextExporter.h"
#include "efkMat.Models.h"
#include "efkMat.Parameters.h"
#include "efkMat.Utils.h"

namespace EffekseerMaterial
{

TextExporterResult TextExporter::Export(std::shared_ptr<Material> material, std::shared_ptr<Node> outputNode)
{
	if (!(outputNode->OutputPins.size() != 0 || outputNode->Parameter->Type == NodeType::Output))
		return TextExporterResult();

	// Gather node
	std::vector<std::shared_ptr<Node>> nodes;
	std::unordered_set<std::shared_ptr<Node>> foundNodes;

	GatherNodes(material, outputNode, nodes, foundNodes);

	// Generate wrapper with variables
	std::reverse(nodes.begin(), nodes.end());

	// Generate exporter node
	std::vector<std::shared_ptr<TextExporterNode>> exportedNodes;
	std::unordered_map<std::shared_ptr<Node>, std::shared_ptr<TextExporterNode>> node2exportedNode;

	std::map<std::string, std::shared_ptr<TextExporterTexture>> extractedTextures;
	std::map<std::string, std::shared_ptr<TextExporterUniform>> extractedUniforms;

	int32_t variableInd = 0;
	for (auto node : nodes)
	{
		auto teNode = std::make_shared<TextExporterNode>();
		teNode->Target = node;

		for (auto pin : node->OutputPins)
		{
			TextExporterPin tePin;
			tePin.IsConnected = true;

			std::unordered_set<std::shared_ptr<Pin>> visited;
			auto type = material->GetDesiredPinType(pin, visited);

			if (type == ValueType::String || type == ValueType::Texture)
			{
				// these types cannot export into shader as varibales
				tePin.Name = "unused" + std::to_string(variableInd);
				tePin.Type = type;

				if (node->Parameter->Type == NodeType::ConstantTexture)
				{
					auto paramName = GetConstantTextureName(node->GUID);
					auto path = node->Properties[0]->Str;
					auto keyStr = paramName + "@" + path;

					std::shared_ptr<TextExporterTexture> extractedTexture;

					if (extractedTextures.count(keyStr) > 0)
					{
						extractedTexture = extractedTextures[keyStr];
					}
					else
					{
						extractedTexture = std::make_shared<TextExporterTexture>();
						extractedTexture->Name = paramName;
						extractedTexture->DefaultPath = path;
						extractedTexture->IsParam = false;
						extractedTextures[keyStr] = extractedTexture;
					}

					tePin.TextureValue = extractedTexture;
				}

				if (node->Parameter->Type == NodeType::ParamTexture)
				{
					auto paramName = EspcapeUserParamName(node->Properties[0]->Str.c_str());
					auto path = node->Properties[1]->Str;
					auto keyStr = paramName + "@" + path;

					std::shared_ptr<TextExporterTexture> extractedTexture;

					if (extractedTextures.count(keyStr) > 0)
					{
						extractedTexture = extractedTextures[keyStr];
					}
					else
					{
						extractedTexture = std::make_shared<TextExporterTexture>();
						extractedTexture->Name = paramName;
						extractedTexture->DefaultPath = path;
						extractedTexture->IsParam = true;
						extractedTextures[keyStr] = extractedTexture;
					}

					tePin.TextureValue = extractedTexture;
				}
			}
			else if (type == ValueType::Unknown || type == ValueType::FloatN || type == ValueType::Function)
			{
				// these types doesn't exists
				tePin.Name = "invalid" + std::to_string(variableInd);
				tePin.Type = type;
			}
			else
			{
				if (node->Parameter->Type == NodeType::Param1 || node->Parameter->Type == NodeType::Param4)
				{
					auto paramName = node->Properties[0]->Str;
					auto values = node->Properties[1]->Floats;
					auto keyStr = paramName;

					std::shared_ptr<TextExporterUniform> extractedUniform;

					if (extractedUniforms.count(keyStr) > 0)
					{
						extractedUniform = extractedUniforms[keyStr];
					}
					else
					{
						extractedUniform = std::make_shared<TextExporterUniform>();
						extractedUniform->Name = paramName;
						extractedUniform->DefaultConstants = values;

						if (node->Parameter->Type == NodeType::Param1)
						{
							extractedUniform->Type = ValueType::Float1;
						}
						else if (node->Parameter->Type == NodeType::Param4)
						{
							extractedUniform->Type = ValueType::Float4;
						}

						extractedUniforms[keyStr] = extractedUniform;
					}

					tePin.UniformValue = extractedUniform;
				}

				tePin.Name = "val" + std::to_string(variableInd);
				tePin.Type = type;
			}

			teNode->Outputs.push_back(tePin);
			variableInd++;
		}

		exportedNodes.push_back(teNode);
		node2exportedNode[node] = teNode;
	}

	// Assgin inputs and extract uniform and textures

	for (auto enode : exportedNodes)
	{
		auto node = enode->Target;
		for (int32_t i = 0; i < node->InputPins.size(); i++)
		{
			auto pin = node->InputPins[i];

			auto connectedPins = material->GetConnectedPins(pin);

			if (connectedPins.size() > 0)
			{
				// value from connected
				for (auto connectedPin : connectedPins)
				{
					auto connectedNode = connectedPin->Parent.lock();
					auto connectedExportedNode = node2exportedNode[connectedNode];

					auto outputPin = connectedExportedNode->Outputs[connectedPin->PinIndex];
					enode->Inputs.push_back(outputPin);
				}
			}
			else
			{
				std::unordered_set<std::shared_ptr<Pin>> visited;

				// value from self
				TextExporterPin tePin;
				tePin.IsConnected = false;
				tePin.Default = node->Parameter->InputPins[i]->Default;
				tePin.Type = material->GetDesiredPinType(pin, visited);

				auto path = std::string();

				if (tePin.Type == ValueType::Texture)
				{
					auto paramName = std::string();

					if (node->Parameter->Type == NodeType::SampleTexture)
					{
						path = node->Properties[0]->Str;
					}
					else
					{
						assert(0);
					}

					auto keyStr = paramName + "@" + path;

					std::shared_ptr<TextExporterTexture> extractedTexture;

					if (extractedTextures.count(keyStr) > 0)
					{
						extractedTexture = extractedTextures[keyStr];
					}
					else
					{
						extractedTexture = std::make_shared<TextExporterTexture>();
						extractedTexture->Name = paramName;
						extractedTexture->DefaultPath = path;
						extractedTextures[keyStr] = extractedTexture;
					}

					tePin.TextureValue = extractedTexture;
				}
				else
				{
					tePin.NumberValue = node->Parameter->InputPins[i]->DefaultValues;
				}

				enode->Inputs.push_back(tePin);
			}
		}
	}

	// get output node
	auto outputExportedNode = node2exportedNode[outputNode];

	std::unordered_set<std::string> usedNames;

	// Assign texture index

	{
		int id = 0;

		for (auto& extractedTexture : extractedTextures)
		{
			if (extractedTexture.second->Name == "")
			{
				extractedTexture.second->Name = "default_texture_" + std::to_string(id);
				usedNames.insert(extractedTexture.second->Name);
			}

			extractedTexture.second->Index = id;
			id++;
		}
	}

	// Assign Uniform
	{
		int32_t offset = 0;
		for (auto& extractedUniform : extractedUniforms)
		{
			std::string candidateName;

			candidateName = extractedUniform.second->Name;

			if (candidateName == "")
			{
				int id = 0;

				do
				{
					candidateName = "default_" + std::to_string(id);
					id++;
				} while (usedNames.count(candidateName) > 0);
			}

			extractedUniform.second->Name = candidateName;

			extractedUniform.second->Offset = offset;
			offset += sizeof(float) * 4;
		}
	}

		// for output
	TextExporterOutputOption option;
	if (outputNode->Parameter->Type == NodeType::Output)
	{
		auto refractionInd = outputNode->GetInputPinIndex("Refraction");
		option.HasRefraction = material->GetConnectedPins(outputNode->InputPins[refractionInd]).size() != 0;
	}

	// Generate outputs
	std::ostringstream ret;
	for (auto wn : exportedNodes)
	{
		ret << Export(wn);
	}

	ret << ExportOutputNode(material, outputExportedNode, option);

	std::ostringstream uniform_textures;

	std::vector<std::shared_ptr<TextExporterUniform>> uniforms;
	std::vector<std::shared_ptr<TextExporterTexture>> textures;

	for (auto& kv : extractedTextures)
	{
		textures.push_back(kv.second);
	}

	for (auto& kv : extractedUniforms)
	{
		uniforms.push_back(kv.second);
	}

	uniform_textures << ExportUniformAndTextures(uniforms, textures);

	TextExporterResult result;
	result.Code = MergeTemplate(ret.str(), uniform_textures.str());
	result.Uniforms = uniforms;
	result.Textures = textures;
	result.HasRefraction = option.HasRefraction;

	return result;
};

void TextExporter::GatherNodes(std::shared_ptr<Material> material,
							   std::shared_ptr<Node> node,
							   std::vector<std::shared_ptr<Node>>& nodes,
							   std::unordered_set<std::shared_ptr<Node>>& foundNodes)
{
	// already exists, so rearrange it
	if (foundNodes.count(node) > 0)
	{
		for (size_t i = 0; i < nodes.size(); i++)
		{
			if (node == nodes[i])
			{
				nodes.erase(nodes.begin() + i);
				break;
			}
		}
	}
	else
	{
		foundNodes.insert(node);
	}

	nodes.push_back(node);

	for (auto p : node->InputPins)
	{
		auto relatedPins = material->GetConnectedPins(p);

		for (auto relatedPin : relatedPins)
		{
			auto relatedNode = relatedPin->Parent.lock();
			GatherNodes(material, relatedNode, nodes, foundNodes);
		}
	}
}

std::string TextExporter::MergeTemplate(std::string code, std::string uniform_texture)
{
	const char template_[] = R"(

UNIFORM

TEXTURE

vec4 calc()
{

RETURN

}

void main()
{
	gl_color = calc();
}

)";

	auto ret = Replace(template_, "RETURN", code);
	ret = Replace(ret.c_str(), "UNIFORM_TEXTURE", uniform_texture);

	return ret;
}

std::string TextExporter::ExportOutputNode(std::shared_ptr<Material> material,
										   std::shared_ptr<TextExporterNode> outputNode,
										   const TextExporterOutputOption& option)
{
	std::ostringstream ret;

	if (outputNode->Target->Parameter->Type == NodeType::Output)
	{
		ret <<  GetTypeName(ValueType::Float4) << " emissive = " << GetInputArg(ValueType::Float4, outputNode->Inputs[0]) << ";" << std::endl;

		if (option.HasRefraction)
		{
			auto refractionIndex = outputNode->Target->GetInputPinIndex("Refraction");
			auto normalIndex = outputNode->Target->GetInputPinIndex("Normal");

			ret << GetTypeName(ValueType::Float3) << " normalDir = " << GetInputArg(ValueType::Float4, outputNode->Inputs[normalIndex]) << ";"
				<< std::endl;

			ret << GetTypeName(ValueType::Float1) << " refraction = " << GetInputArg(ValueType::Float1, outputNode->Inputs[refractionIndex]) << ";"
				<< std::endl;

			// TODO add code

			ret << "return emissive;" << std::endl; 
		}
		else
		{
			ret << "return emissive;" << std::endl; 
		}
	}
	else
	{
		if (outputNode->Target->Parameter->Type == NodeType::ConstantTexture)
		{
			ret << "return "
				<< "texture(" << outputNode->Outputs[0].TextureValue->Name << ", " << GetUVName() << ");" << std::endl;
		}
		else if (outputNode->Target->Parameter->Type == NodeType::ParamTexture)
		{
			ret << "return "
				<< "texture(" << outputNode->Outputs[0].TextureValue->Name << ", " << GetUVName() << ");" << std::endl;
		}
		else
		{
			ret << "return " << ConvertType(ValueType::Float4, outputNode->Outputs[0].Type, outputNode->Outputs[0].Name) << ";"
				<< std::endl;
		}
	}

	return ret.str();
}

std::string TextExporter::Export(std::shared_ptr<TextExporterNode> node)
{
	auto exportInputOrProp = [this](ValueType type_, TextExporterPin& pin_, std::shared_ptr<NodeProperty>& prop_) -> std::string {
		if (pin_.IsConnected)
		{
			return GetInputArg(type_, pin_);
		}
		return GetInputArg(pin_.Type, prop_->Floats[0]);
	};

	std::ostringstream ret;

	auto exportIn2Out2Param2 = [&, this](const char* func, const char* op) -> void {
		ret << GetTypeName(node->Outputs[0].Type) << " " << node->Outputs[0].Name << "=" << func << "("
			<< exportInputOrProp(node->Outputs[0].Type, node->Inputs[0], node->Target->Properties[0]) << op
			<< exportInputOrProp(node->Outputs[0].Type, node->Inputs[1], node->Target->Properties[1]) << ");" << std::endl;
	};

	auto exportIn1Out1 = [&, this](const char* func) -> void {
		assert(node->Inputs.size() == 1);
		assert(node->Outputs.size() == 1);
		assert(node->Inputs[0].Type == node->Outputs[0].Type);
		ret << GetTypeName(node->Inputs[0].Type) << " " << node->Outputs[0].Name << "=" << func << "("
			<< GetInputArg(node->Inputs[0].Type, node->Inputs[0]) << ");" << std::endl;
	};

	if (node->Target->Parameter->Type == NodeType::Constant1)
	{
		ret << GetTypeName(ValueType::Float1) << " " << node->Outputs[0].Name << "=" << node->Target->Properties[0]->Floats[0] << ";"
			<< std::endl;
	}

	if (node->Target->Parameter->Type == NodeType::Constant2)
	{
		ret << GetTypeName(ValueType::Float2) << " " << node->Outputs[0].Name << "=" << GetTypeName(ValueType::Float2) << "("
			<< node->Target->Properties[0]->Floats[0] << "," << node->Target->Properties[0]->Floats[1] << ");" << std::endl;
	}

	if (node->Target->Parameter->Type == NodeType::Param1)
	{
		ret << GetTypeName(ValueType::Float1) << " " << node->Outputs[0].Name << "=" << node->Outputs[0].UniformValue->Name << ";"
			<< std::endl;
	}

	if (node->Target->Parameter->Type == NodeType::Param4)
	{
		ret << GetTypeName(ValueType::Float4) << " " << node->Outputs[0].Name << "=" << node->Outputs[0].UniformValue->Name << ";"
			<< std::endl;
	}

	if (node->Target->Parameter->Type == NodeType::Sine)
	{
		exportIn1Out1("sin");
	}

	if (node->Target->Parameter->Type == NodeType::Abs)
	{
		exportIn1Out1("abs");
	}

	if (node->Target->Parameter->Type == NodeType::Add)
	{
		exportIn2Out2Param2("", "+");
	}

	if (node->Target->Parameter->Type == NodeType::Subtract)
	{
		exportIn2Out2Param2("", "-");
	}

	if (node->Target->Parameter->Type == NodeType::Multiply)
	{
		exportIn2Out2Param2("", "*");
	}

	if (node->Target->Parameter->Type == NodeType::Divide)
	{
		exportIn2Out2Param2("", "/");
	}

	if (node->Target->Parameter->Type == NodeType::FMod)
	{
		exportIn2Out2Param2("mod", ",");
	}

	if (node->Target->Parameter->Type == NodeType::UV)
	{
		ret << GetTypeName(node->Outputs[0].Type) << " " << node->Outputs[0].Name << "=" << GetUVName() << ";" << std::endl;
	}

	if (node->Target->Parameter->Type == NodeType::Panner)
	{
		std::array<float, 2> speed_;
		speed_[0] = node->Target->Properties[0]->Floats[0];
		speed_[1] = node->Target->Properties[0]->Floats[1];

		ret << GetTypeName(node->Outputs[0].Type) << " " << node->Outputs[0].Name << "=" << GetInputArg(ValueType::Float2, node->Inputs[0])
			<< "+"
			<< (node->Inputs[2].IsConnected ? GetInputArg(ValueType::Float2, node->Inputs[2]) : GetInputArg(ValueType::Float2, speed_))
			<< "*" << GetInputArg(ValueType::Float1, node->Inputs[1]) << ";" << std::endl;
	}

	if (node->Target->Parameter->Type == NodeType::ComponentMask)
	{
		auto compName = node->Outputs[0].Name + "_CompMask";

		ret << GetTypeName(ValueType::Float4) << " " << compName << "=" << GetInputArg(ValueType::Float4, node->Inputs[0]) << ";"
			<< std::endl;

		ret << GetTypeName(node->Outputs[0].Type) << " " << node->Outputs[0].Name << "=" << compName << ".";

		if (node->Target->Properties[0]->Floats[0] > 0)
			ret << "x";

		if (node->Target->Properties[1]->Floats[0] > 0)
			ret << "y";

		if (node->Target->Properties[2]->Floats[0] > 0)
			ret << "z";

		if (node->Target->Properties[3]->Floats[0] > 0)
			ret << "w";

		ret << ";" << std::endl;
	}

	if (node->Target->Parameter->Type == NodeType::Append)
	{
		ret << GetTypeName(node->Outputs[0].Type) << " " << node->Outputs[0].Name << "=" << GetTypeName(node->Outputs[0].Type) << "(";

		auto getElmName = [](int n) -> std::string {
			if (n == 0)
				return ".x";
			if (n == 1)
				return ".y";
			if (n == 2)
				return ".z";
			if (n == 3)
				return ".w";
			return "";
		};

		auto allCount = GetElementCount(node->Outputs[0].Type);
		auto v1Count = GetElementCount(node->Inputs[0].Type);
		auto v2Count = allCount - v1Count;

		for (int i = 0; i < v1Count; i++)
		{
			if (node->Inputs[0].Type == ValueType::Float1)
			{
				ret << GetInputArg(node->Inputs[0].Type, node->Inputs[0]);
			}
			else
			{
				ret << GetInputArg(node->Inputs[0].Type, node->Inputs[0]) << getElmName(i);
			}

			if (i < allCount - 1)
				ret << ",";
		}

		for (int i = 0; i < v2Count; i++)
		{
			if (node->Inputs[1].Type == ValueType::Float1)
			{
				ret << GetInputArg(node->Inputs[1].Type, node->Inputs[1]);
			}
			else
			{
				ret << GetInputArg(node->Inputs[1].Type, node->Inputs[1]) << getElmName(i);
			}

			if (v1Count + i < allCount - 1)
				ret << ",";
		}

		ret << ");" << std::endl;
	}

	if (node->Target->Parameter->Type == NodeType::OneMinus)
	{
		ret << GetTypeName(node->Outputs[0].Type) << " " << node->Outputs[0].Name << "=" << GetInputArg(node->Inputs[0].Type, 1.0f) << "-"
			<< GetInputArg(node->Inputs[0].Type, node->Inputs[0]) << ";" << std::endl;
	}

	if (node->Target->Parameter->Type == NodeType::ConstantTexture)
	{
		// None
	}

	if (node->Target->Parameter->Type == NodeType::ParamTexture)
	{
		// None
	}

	if (node->Target->Parameter->Type == NodeType::SampleTexture)
	{
		assert(node->Inputs[0].TextureValue != nullptr);
		if (0 <= node->Inputs[0].TextureValue->Index)
		{
			ret << GetTypeName(node->Outputs[0].Type) << " " << node->Outputs[0].Name << "=texture(" << node->Inputs[0].TextureValue->Name
				<< "," << GetInputArg(ValueType::Float2, node->Inputs[1]) << ");" << std::endl;
		}
		else
		{
			ret << GetTypeName(node->Outputs[0].Type) << " " << node->Outputs[0].Name << "=" << GetTypeName(ValueType::Float4)
				<< "(1.0,1.0,1.0,1.0);" << std::endl;
		}
	}

	if (node->Target->Parameter->Type == NodeType::Time)
	{
		ret << GetTypeName(node->Outputs[0].Type) << " " << node->Outputs[0].Name << "=" << GetTimeName() << ";" << std::endl;
	}

	return ret.str();
}

std::string TextExporter::ExportUniformAndTextures(const std::vector<std::shared_ptr<TextExporterUniform>>& uniformNodes,
												   const std::vector<std::shared_ptr<TextExporterTexture>>& textureNodes)
{

	std::ostringstream ret;

	for (auto node : uniformNodes)
	{
		ret << "uniform " << GetTypeName(node->Type) << " " << node->Name << ";" << std::endl;
	}

	for (auto node : textureNodes)
	{
		ret << "uniform sampler2D " << node->Name << ";" << std::endl;
	}

	return ret.str();
}

std::string TextExporter::GetInputArg(const ValueType& pinType, TextExporterPin& pin)
{
	if (pin.IsConnected)
	{
		if (pin.Type == pinType)
			return pin.Name;

		return ConvertType(pinType, pin.Type, pin.Name);
	}
	else
	{
		if (pin.Default == DefaultType::UV)
			return GetUVName();

		if (pin.Default == DefaultType::Time)
			return GetTimeName();

		std::ostringstream ret;

		if (pin.Type == ValueType::Float1)
		{
			ret << GetTypeName(pin.Type) << "(" << pin.NumberValue[0] << ")";
		}
		if (pin.Type == ValueType::Float2)
		{
			ret << GetTypeName(pin.Type) << "(" << pin.NumberValue[0] << "," << pin.NumberValue[1] << ")";
		}
		if (pin.Type == ValueType::Float3)
		{
			ret << GetTypeName(pin.Type) << "(" << pin.NumberValue[0] << "," << pin.NumberValue[1] << "," << pin.NumberValue[2] << ")";
		}
		if (pin.Type == ValueType::Float4)
		{
			ret << GetTypeName(pin.Type) << "(" << pin.NumberValue[0] << "," << pin.NumberValue[1] << "," << pin.NumberValue[2] << ","
				<< pin.NumberValue[3] << ")";
		}

		if (pin.Type == pinType)
			return ret.str();

		return ConvertType(pinType, pin.Type, ret.str());
	}
}

std::string TextExporter::GetInputArg(const ValueType& pinType, float value)
{
	std::ostringstream ret;

	if (pinType == ValueType::Float1)
	{
		ret << GetTypeName(pinType) << "(" << value << ")";
	}
	if (pinType == ValueType::Float2)
	{
		ret << GetTypeName(pinType) << "(" << value << "," << value << ")";
	}
	if (pinType == ValueType::Float3)
	{
		ret << GetTypeName(pinType) << "(" << value << "," << value << "," << value << ")";
	}
	if (pinType == ValueType::Float4)
	{
		ret << GetTypeName(pinType) << "(" << value << "," << value << "," << value << "," << value << ")";
	}

	return ret.str();
}

std::string TextExporter::GetInputArg(const ValueType& pinType, std::array<float, 2> value)
{
	std::ostringstream ret;

	if (pinType == ValueType::Float1)
	{
		ret << GetTypeName(pinType) << "(" << value[0] << ")";
	}
	if (pinType == ValueType::Float2)
	{
		ret << GetTypeName(pinType) << "(" << value[0] << "," << value[1] << ")";
	}
	if (pinType == ValueType::Float3)
	{
		ret << GetTypeName(pinType) << "(" << value[0] << ".x ," << value[1] << ".y ,"
			<< "0.0"
			<< ")";
	}
	if (pinType == ValueType::Float4)
	{
		ret << GetTypeName(pinType) << "(" << value[0] << ".x ," << value[1] << ".y ,"
			<< "0.0, 1.0"
			<< ")";
	}

	return ret.str();
}

std::string TextExporter::GetTypeName(ValueType type) const
{
	if (type == ValueType::Float1)
		return "float";
	if (type == ValueType::Float2)
		return "vec2";
	if (type == ValueType::Float3)
		return "vec3";
	if (type == ValueType::Float4)
		return "vec4";
	return "";
}

std::string TextExporter::GetUVName() const { return "uv"; }

std::string TextExporter::GetTimeName() const { return "time"; }

std::string TextExporter::ConvertType(ValueType dst, ValueType src, const std::string& name) const
{
	if (dst == src)
		return name;

	if (dst == ValueType::Float1)
	{
		return name + ".x";
	}
	else if (dst == ValueType::Float2)
	{
		if (src == ValueType::Float1)
		{
			return GetTypeName(ValueType::Float2) + "(" + name + "," + name + ")";
		}
		return GetTypeName(ValueType::Float2) + "(" + name + ".x," + name + ".y)";
	}
	else if (dst == ValueType::Float3)
	{
		if (src == ValueType::Float1)
		{
			return GetTypeName(ValueType::Float3) + "(" + name + "," + name + "," + name + ")";
		}
		else if (src == ValueType::Float2)
		{
			return GetTypeName(ValueType::Float3) + "(" + name + ".x, " + name + ".y, 0.0)";
		}
		return GetTypeName(ValueType::Float3) + "(" + name + ".x," + name + ".y," + name + ".z)";
	}
	else if (dst == ValueType::Float4)
	{
		if (src == ValueType::Float1)
		{
			return GetTypeName(ValueType::Float4) + "(" + name + "," + name + "," + name + ", 1.0)";
		}
		else if (src == ValueType::Float2)
		{
			return GetTypeName(ValueType::Float4) + "(" + name + ".x," + name + ".y, 0.0, 1.0)";
		}
		else if (src == ValueType::Float3)
		{
			return GetTypeName(ValueType::Float4) + "(" + name + ".x," + name + ".y," + name + ".z, 1.0)";
		}
	}
	return "";
}

} // namespace EffekseerMaterial