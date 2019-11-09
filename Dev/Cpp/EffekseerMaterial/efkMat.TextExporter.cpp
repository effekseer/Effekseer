#include "efkMat.TextExporter.h"
#include "efkMat.Models.h"
#include "efkMat.Parameters.h"
#include "efkMat.Utils.h"

namespace EffekseerMaterial
{

TextExporterResult TextExporter::Export(std::shared_ptr<Material> material, std::shared_ptr<Node> outputNode, std::string suffix)
{
	if (!(outputNode->OutputPins.size() != 0 || outputNode->Parameter->Type == NodeType::Output))
		return TextExporterResult();

	// Gather node
	std::vector<std::shared_ptr<Node>> nodes;
	std::unordered_set<std::shared_ptr<Node>> foundNodes;

	GatherNodes(material, outputNode, nodes, foundNodes);

	// Generate wrapper with variables
	std::reverse(nodes.begin(), nodes.end());

	// Check custom data
	int32_t customData1Count = 0;
	int32_t customData2Count = 0;

	for (auto& node : nodes)
	{
		if (node->Parameter->Type == NodeType::CustomData1)
		{
			for (int32_t i = 0; i < 4; i++)
			{
				if (node->Properties[i]->Floats[0] > 0)
				{
					customData1Count = std::max(customData1Count, i + 1);
				}
			}
		}

		if (node->Parameter->Type == NodeType::CustomData2)
		{
			for (int32_t i = 0; i < 4; i++)
			{
				if (node->Properties[i]->Floats[0] > 0)
				{
					customData2Count = std::max(customData2Count, i + 1);
				}
			}
		}
	}

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

				if (node->Parameter->Type == NodeType::TextureObject)
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
						extractedTexture->Type = material->FindTexture(path.c_str())->Type;
						extractedTextures[keyStr] = extractedTexture;
					}

					tePin.TextureValue = extractedTexture;
				}

				if (node->Parameter->Type == NodeType::TextureObjectParameter)
				{
					auto paramName = EspcapeUserParamName(node->GetProperty("Name")->Str.c_str());
					auto path = node->GetProperty("Texture")->Str;
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
						extractedTexture->Type = material->FindTexture(path.c_str())->Type;
						extractedTexture->Priority = static_cast<int32_t>(node->GetProperty("Priority")->Floats[0]);
						extractedTexture->Descriptions = node->Descriptions;
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
				if (node->Parameter->Type == NodeType::Parameter1 || node->Parameter->Type == NodeType::Parameter4)
				{
					auto paramName = EspcapeUserParamName(node->GetProperty("Name")->Str.c_str());
					auto values = node->GetProperty("Value")->Floats;
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
						extractedUniform->Priority = static_cast<int32_t>(node->GetProperty("Priority")->Floats[0]);
						extractedUniform->Descriptions = node->Descriptions;

						if (node->Parameter->Type == NodeType::Parameter1)
						{
							extractedUniform->Type = ValueType::Float1;
						}
						else if (node->Parameter->Type == NodeType::Parameter4)
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
						extractedTexture->Type = material->FindTexture(path.c_str())->Type;
						extractedTextures[keyStr] = extractedTexture;
					}

					// assign a sampler
					extractedTexture->Sampler =
						static_cast<TextureSamplerType>((int)node->Properties[node->Parameter->GetPropertyIndex("Sampler")]->Floats[0]);

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
				extractedTexture.second->Name = "default_texture_" + std::to_string(id) + suffix;
				usedNames.insert(extractedTexture.second->Name);
			}
			else
			{
				extractedTexture.second->Name += suffix;
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
					candidateName = "default_" + std::to_string(id) + suffix;
					id++;
				} while (usedNames.count(candidateName) > 0);
				extractedUniform.second->Name = candidateName;
			}
			else
			{
				extractedUniform.second->Name += suffix;
			}

			usedNames.insert(extractedUniform.second->Name);
			extractedUniform.second->Offset = offset;
			offset += sizeof(float) * 4;
		}
	}

	// for output
	TextExporterOutputOption option;
	if (outputNode->Parameter->Type == NodeType::Output)
	{
		auto worldPositionOffsetInd = outputNode->GetInputPinIndex("WorldPositionOffset");
		option.HasWorldPositionOffset = material->GetConnectedPins(outputNode->InputPins[worldPositionOffsetInd]).size() != 0;

		auto refractionInd = outputNode->GetInputPinIndex("Refraction");
		option.HasRefraction = material->GetConnectedPins(outputNode->InputPins[refractionInd]).size() != 0;
		option.ShadingModel = outputNode->Properties[0]->Floats[0];
	}
	else
	{
		option.HasRefraction = false;
		option.ShadingModel = 1;
	}

	// Generate outputs
	std::ostringstream ret;

	// collect pixelNormalDir and export it first.
	// it is able to use pixelNormalDir for other calculating
	if (outputExportedNode->Target->Parameter->Type == NodeType::Output)
	{
		auto normalIndex = outputExportedNode->Target->GetInputPinIndex("Normal");
		if (outputExportedNode->Inputs[normalIndex].IsConnected)
		{
			std::vector<std::shared_ptr<Node>> pnNodes;
			std::unordered_set<std::shared_ptr<Node>> pnFoundNodes;

			GatherNodes(material, outputExportedNode->Target->InputPins[normalIndex], pnNodes, pnFoundNodes);

			// nodes to calculate pixelNormalDir
			std::vector<std::shared_ptr<TextExporterNode>> pnExportedNodes;
			std::vector<std::shared_ptr<TextExporterNode>> tempExportedNodes;

			for (auto wn : exportedNodes)
			{
				if (pnFoundNodes.find(wn->Target) != pnFoundNodes.end())
				{
					pnExportedNodes.push_back(wn);
				}
				else
				{
					tempExportedNodes.push_back(wn);
				}
			}

			exportedNodes = tempExportedNodes;

			// export pixelnormaldir
			for (auto wn : pnExportedNodes)
			{
				ret << ExportNode(wn);
			}

			ret << " pixelNormalDir = " << GetInputArg(ValueType::Float3, outputExportedNode->Inputs[normalIndex]) << ";" << std::endl;
		}
	}

	for (auto wn : exportedNodes)
	{
		ret << ExportNode(wn);
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
	result.ShadingModel = option.ShadingModel;
	result.Uniforms = uniforms;
	result.Textures = textures;
	result.HasRefraction = option.HasRefraction;
	result.CustomData1 = customData1Count;
	result.CustomData2 = customData2Count;
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

void TextExporter::GatherNodes(std::shared_ptr<Material> material,
							   std::shared_ptr<Pin> pin,
							   std::vector<std::shared_ptr<Node>>& nodes,
							   std::unordered_set<std::shared_ptr<Node>>& foundNodes)
{
	auto relatedPins = material->GetConnectedPins(pin);

	for (auto relatedPin : relatedPins)
	{
		auto relatedNode = relatedPin->Parent.lock();
		GatherNodes(material, relatedNode, nodes, foundNodes);
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
		auto worldPositionOffsetIndex = outputNode->Target->GetInputPinIndex("WorldPositionOffset");
		auto baseColorIndex = outputNode->Target->GetInputPinIndex("BaseColor");
		auto emissiveIndex = outputNode->Target->GetInputPinIndex("Emissive");
		auto refractionIndex = outputNode->Target->GetInputPinIndex("Refraction");
		auto normalIndex = outputNode->Target->GetInputPinIndex("Normal");
		auto roughnessIndex = outputNode->Target->GetInputPinIndex("Roughness");
		auto metallicIndex = outputNode->Target->GetInputPinIndex("Metallic");
		auto ambientOcclusionIndex = outputNode->Target->GetInputPinIndex("AmbientOcclusion");
		auto opacityIndex = outputNode->Target->GetInputPinIndex("Opacity");
		auto opacityMaskIndex = outputNode->Target->GetInputPinIndex("OpacityMask");

		ret << GetTypeName(ValueType::Float3) << " normalDir = " << GetInputArg(ValueType::Float3, outputNode->Inputs[normalIndex]) << ";"
			<< std::endl;

		ret << GetTypeName(ValueType::Float3) << " tempNormalDir = ((normalDir -" << GetTypeName(ValueType::Float3)
			<< " (0.5, 0.5, 0.5)) * 2.0);" << std::endl;

		ret << "pixelNormalDir = tempNormalDir.x * worldTangent + tempNormalDir.y * worldBinormal + tempNormalDir.z * worldNormal;"
			<< std::endl;

		ret << GetTypeName(ValueType::Float3)
			<< " worldPositionOffset = " << GetInputArg(ValueType::Float3, outputNode->Inputs[worldPositionOffsetIndex]) << ";"
			<< std::endl;

		ret << GetTypeName(ValueType::Float3) << " baseColor = " << GetInputArg(ValueType::Float3, outputNode->Inputs[baseColorIndex])
			<< ";" << std::endl;

		ret << GetTypeName(ValueType::Float3) << " emissive = " << GetInputArg(ValueType::Float3, outputNode->Inputs[emissiveIndex]) << ";"
			<< std::endl;

		ret << GetTypeName(ValueType::Float1) << " metallic = " << GetInputArg(ValueType::Float1, outputNode->Inputs[metallicIndex]) << ";"
			<< std::endl;

		ret << GetTypeName(ValueType::Float1) << " roughness = " << GetInputArg(ValueType::Float1, outputNode->Inputs[roughnessIndex])
			<< ";" << std::endl;

		ret << GetTypeName(ValueType::Float1)
			<< " ambientOcclusion = " << GetInputArg(ValueType::Float1, outputNode->Inputs[ambientOcclusionIndex]) << ";" << std::endl;

		ret << GetTypeName(ValueType::Float1) << " opacity = " << GetInputArg(ValueType::Float1, outputNode->Inputs[opacityIndex]) << ";"
			<< std::endl;

		ret << GetTypeName(ValueType::Float1) << " opacityMask = " << GetInputArg(ValueType::Float1, outputNode->Inputs[opacityMaskIndex])
			<< ";" << std::endl;

		ret << GetTypeName(ValueType::Float1) << " refraction = " << GetInputArg(ValueType::Float1, outputNode->Inputs[refractionIndex])
			<< ";" << std::endl;
	}
	else
	{
		ret << GetTypeName(ValueType::Float3) << " worldPositionOffset = " << GetTypeName(ValueType::Float3) << "(0, 0, 0);" << std::endl;

		if (outputNode->Target->Parameter->Type == NodeType::TextureObject)
		{
			ret << GetTypeName(ValueType::Float4) << " emissive_temp = "
				<< "texture(" << outputNode->Outputs[0].TextureValue->Name << ", " << GetUVName(0) << ");" << std::endl;
			ret << GetTypeName(ValueType::Float3) << " emissive = emissive_temp.xyz;" << std::endl;
			ret << "float opacity = emissive_temp.w;" << std::endl;

			ret << "float opacityMask = 1.0;" << std::endl;
		}
		else if (outputNode->Target->Parameter->Type == NodeType::TextureObjectParameter)
		{
			ret << GetTypeName(ValueType::Float4) << " emissive_temp = "
				<< "texture(" << outputNode->Outputs[0].TextureValue->Name << ", " << GetUVName(0) << ");" << std::endl;
			ret << GetTypeName(ValueType::Float3) << " emissive = emissive_temp.xyz;" << std::endl;
			ret << "float opacity = emissive_temp.w;" << std::endl;

			ret << "float opacityMask = 1.0;" << std::endl;
		}
		else
		{
			ret << GetTypeName(ValueType::Float3)
				<< " emissive = " << ConvertType(ValueType::Float3, outputNode->Outputs[0].Type, outputNode->Outputs[0].Name) << ";"
				<< std::endl;
			ret << "float opacity = 1.0;" << std::endl;

			ret << "float opacityMask = 1.0;" << std::endl;
		}
	}

	return ret.str();
}

std::string TextExporter::ExportNode(std::shared_ptr<TextExporterNode> node)
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

	// for opengl es
	auto getNum = [](float f) -> std::string {
		std::ostringstream ret;
		if (f == (int)f)
		{
			ret << f << ".0";
		}
		else
		{
			ret << f;
		}

		return ret.str();
	};

	if (node->Target->Parameter->Type == NodeType::Constant1)
	{
		ret << GetTypeName(ValueType::Float1) << " " << node->Outputs[0].Name << "=" << getNum(node->Target->Properties[0]->Floats[0])
			<< ";" << std::endl;
	}

	if (node->Target->Parameter->Type == NodeType::Constant2)
	{
		ret << GetTypeName(ValueType::Float2) << " " << node->Outputs[0].Name << "=" << GetTypeName(ValueType::Float2) << "("
			<< getNum(node->Target->Properties[0]->Floats[0]) << "," << getNum(node->Target->Properties[0]->Floats[1]) << ");" << std::endl;
	}

	if (node->Target->Parameter->Type == NodeType::Constant3)
	{
		auto& floats = node->Target->Properties[0]->Floats;
		ret << GetTypeName(ValueType::Float3) << " " << node->Outputs[0].Name << "=" << GetTypeName(ValueType::Float3) << "("
			<< getNum(floats[0]) << "," << getNum(floats[1]) << "," << getNum(floats[2]) << ");" << std::endl;
	}

	if (node->Target->Parameter->Type == NodeType::Constant4)
	{
		auto& floats = node->Target->Properties[0]->Floats;
		ret << GetTypeName(ValueType::Float4) << " " << node->Outputs[0].Name << "=" << GetTypeName(ValueType::Float4) << "("
			<< getNum(floats[0]) << "," << getNum(floats[1]) << "," << getNum(floats[2]) << "," << getNum(floats[3]) << ");" << std::endl;
	}

	if (node->Target->Parameter->Type == NodeType::Parameter1)
	{
		ret << GetTypeName(ValueType::Float1) << " " << node->Outputs[0].Name << "=" << node->Outputs[0].UniformValue->Name << ";"
			<< std::endl;
	}

	if (node->Target->Parameter->Type == NodeType::Parameter4)
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

	if (node->Target->Parameter->Type == NodeType::Ceil)
	{
		exportIn1Out1("ceil");
	}

	if (node->Target->Parameter->Type == NodeType::Floor)
	{
		exportIn1Out1("floor");
	}

	if (node->Target->Parameter->Type == NodeType::Frac)
	{
		exportIn1Out1("fract");
	}

	if (node->Target->Parameter->Type == NodeType::Min)
	{
		exportIn2Out2Param2("min", ",");
	}

	if (node->Target->Parameter->Type == NodeType::Max)
	{
		exportIn2Out2Param2("max", ",");
	}

	if (node->Target->Parameter->Type == NodeType::Power)
	{
		assert(node->Inputs[1].Type == ValueType::Float1);

		ret << GetTypeName(node->Outputs[0].Type) << " " << node->Outputs[0].Name << "= pow("
			<< GetInputArg(node->Outputs[0].Type, node->Inputs[0]) << ","
			<< exportInputOrProp(node->Inputs[1].Type, node->Inputs[1], node->Target->Properties[0]) << ");" << std::endl;
	}

	if (node->Target->Parameter->Type == NodeType::SquareRoot)
	{
		exportIn1Out1("sqrt");
	}

	if (node->Target->Parameter->Type == NodeType::Clamp)
	{
		ret << GetTypeName(node->Outputs[0].Type) << " " << node->Outputs[0].Name << "= clamp("
			<< GetInputArg(node->Outputs[0].Type, node->Inputs[0]) << ","
			<< exportInputOrProp(node->Outputs[0].Type, node->Inputs[1], node->Target->Properties[0]) << ","
			<< exportInputOrProp(node->Outputs[0].Type, node->Inputs[2], node->Target->Properties[1]) << ");" << std::endl;
	}

	if (node->Target->Parameter->Type == NodeType::DotProduct)
	{
		ret << GetTypeName(node->Outputs[0].Type) << " " << node->Outputs[0].Name << "= dot("
			<< GetInputArg(node->Inputs[0].Type, node->Inputs[0]) << "," << GetInputArg(node->Inputs[0].Type, node->Inputs[1]) << ");"
			<< std::endl;
	}

	if (node->Target->Parameter->Type == NodeType::CrossProduct)
	{
		ret << GetTypeName(node->Outputs[0].Type) << " " << node->Outputs[0].Name << "= cross("
			<< GetInputArg(node->Inputs[0].Type, node->Inputs[0]) << "," << GetInputArg(node->Inputs[1].Type, node->Inputs[1]) << ");"
			<< std::endl;
	}

	if (node->Target->Parameter->Type == NodeType::LinearInterpolate)
	{
		assert(node->Inputs[2].Type == ValueType::Float1);

		ret << GetTypeName(node->Outputs[0].Type) << " " << node->Outputs[0].Name << "= mix("
			<< GetInputArg(node->Inputs[0].Type, node->Inputs[0]) << "," << GetInputArg(node->Inputs[0].Type, node->Inputs[1]) << ","
			<< GetInputArg(node->Inputs[2].Type, node->Inputs[2]) << ");" << std::endl;
	}

	if (node->Target->Parameter->Type == NodeType::TextureCoordinate)
	{
		ret << GetTypeName(node->Outputs[0].Type) << " " << node->Outputs[0].Name << "="
			<< GetUVName(node->Target->Properties[0]->Floats[0]) << ";" << std::endl;
	}

	if (node->Target->Parameter->Type == NodeType::Panner)
	{
		auto index = node->Target->GetProperty("UVIndex");

		std::array<float, 2> speed_;
		speed_[0] = node->Target->Properties[0]->Floats[0];
		speed_[1] = node->Target->Properties[0]->Floats[1];

		ret << GetTypeName(node->Outputs[0].Type) << " " << node->Outputs[0].Name << "="
			<< (node->Inputs[0].IsConnected ? GetInputArg(ValueType::Float2, node->Inputs[0]) : GetUVName(index->Floats[0])) << "+"
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

	if (node->Target->Parameter->Type == NodeType::AppendVector)
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

	if (node->Target->Parameter->Type == NodeType::TextureObject)
	{
		// None
	}

	if (node->Target->Parameter->Type == NodeType::TextureObjectParameter)
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

	if (node->Target->Parameter->Type == NodeType::VertexNormalWS)
	{
		ret << GetTypeName(node->Outputs[0].Type) << " " << node->Outputs[0].Name << "="
			<< "worldNormal"
			<< ";" << std::endl;
	}

#ifdef _DEBUG
	if (node->Target->Parameter->Type == NodeType::VertexTangentWS)
	{
		ret << GetTypeName(node->Outputs[0].Type) << " " << node->Outputs[0].Name << "="
			<< "worldTangent"
			<< ";" << std::endl;
	}
#endif

	if (node->Target->Parameter->Type == NodeType::PixelNormalWS)
	{
		ret << GetTypeName(node->Outputs[0].Type) << " " << node->Outputs[0].Name << "="
			<< "pixelNormalDir"
			<< ";" << std::endl;
	}

	if (node->Target->Parameter->Type == NodeType::CustomData1 || node->Target->Parameter->Type == NodeType::CustomData2)
	{
		std::string dstName;

		if (node->Target->Parameter->Type == NodeType::CustomData1)
		{
			dstName = "customData1";
		}
		else if (node->Target->Parameter->Type == NodeType::CustomData2)
		{
			dstName = "customData2";
		}

		ret << GetTypeName(node->Outputs[0].Type) << " " << node->Outputs[0].Name << "=" << dstName << ".";

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
		// for opengl es
		auto getNum = [&pin](int i) -> std::string {
			auto f = pin.NumberValue[i];

			std::ostringstream ret;
			if (f == (int)f)
			{
				ret << f << ".0";
			}
			else
			{
				ret << f;
			}

			return ret.str();
		};

		if (pin.Default == DefaultType::UV)
			return GetUVName(0);

		if (pin.Default == DefaultType::Time)
			return GetTimeName();

		std::ostringstream ret;

		if (pin.Type == ValueType::Float1)
		{
			ret << GetTypeName(pin.Type) << "(" << getNum(0) << ")";
		}
		if (pin.Type == ValueType::Float2)
		{
			ret << GetTypeName(pin.Type) << "(" << getNum(0) << "," << getNum(1) << ")";
		}
		if (pin.Type == ValueType::Float3)
		{
			ret << GetTypeName(pin.Type) << "(" << getNum(0) << "," << getNum(1) << "," << getNum(2) << ")";
		}
		if (pin.Type == ValueType::Float4)
		{
			ret << GetTypeName(pin.Type) << "(" << getNum(0) << "," << getNum(1) << "," << getNum(2) << "," << getNum(3) << ")";
		}

		if (pin.Type == pinType)
			return ret.str();

		return ConvertType(pinType, pin.Type, ret.str());
	}
}

std::string TextExporter::GetInputArg(const ValueType& pinType, float value)
{
	std::ostringstream ret;

	// for opengl es
	auto getNum = [](float f) -> std::string {
		std::ostringstream ret;
		if (f == (int)f)
		{
			ret << f << ".0";
		}
		else
		{
			ret << f;
		}

		return ret.str();
	};

	auto valueStr = getNum(value);

	if (pinType == ValueType::Float1)
	{
		ret << GetTypeName(pinType) << "(" << valueStr << ")";
	}
	if (pinType == ValueType::Float2)
	{
		ret << GetTypeName(pinType) << "(" << valueStr << "," << valueStr << ")";
	}
	if (pinType == ValueType::Float3)
	{
		ret << GetTypeName(pinType) << "(" << valueStr << "," << valueStr << "," << valueStr << ")";
	}
	if (pinType == ValueType::Float4)
	{
		ret << GetTypeName(pinType) << "(" << valueStr << "," << valueStr << "," << valueStr << "," << valueStr << ")";
	}

	return ret.str();
}

std::string TextExporter::GetInputArg(const ValueType& pinType, std::array<float, 2> value)
{
	// for opengl es
	auto getNum = [](float f) -> std::string {
		std::ostringstream ret;
		if (f == (int)f)
		{
			ret << f << ".0";
		}
		else
		{
			ret << f;
		}

		return ret.str();
	};

	std::ostringstream ret;

	if (pinType == ValueType::Float1)
	{
		ret << GetTypeName(pinType) << "(" << getNum(value[0]) << ")";
	}
	if (pinType == ValueType::Float2)
	{
		ret << GetTypeName(pinType) << "(" << getNum(value[0]) << "," << getNum(value[1]) << ")";
	}
	if (pinType == ValueType::Float3)
	{
		ret << GetTypeName(pinType) << "(" << getNum(value[0]) << ".x ," << getNum(value[1]) << ".y ,"
			<< "0.0"
			<< ")";
	}
	if (pinType == ValueType::Float4)
	{
		ret << GetTypeName(pinType) << "(" << getNum(value[0]) << ".x ," << getNum(value[1]) << ".y ,"
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

std::string TextExporter::GetUVName(int32_t ind) const
{
	if (ind == 0)
	{
		return "uv1";
	}
	else
	{
		return "uv2";
	}
}

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