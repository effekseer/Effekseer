#include "Effekseer.Material.h"

namespace Effekseer
{

bool Material::Load(const uint8_t* data, int32_t size)
{
	int offset = 0;

	// header
	char prefix[5];

	memcpy(prefix, data + offset, 4);
	offset += sizeof(int);

	prefix[4] = 0;

	if (std::string("EFKM") != std::string(prefix))
		return false;

	int version = 0;
	memcpy(&version, data + offset, 4);
	offset += sizeof(int);

	memcpy(&guid_, data + offset, 8);
	offset += sizeof(uint64_t);

	while (0 <= offset && offset < size)
	{
		char chunk[5];
		memcpy(chunk, data + offset, 4);
		offset += sizeof(int);
		chunk[4] = 0;

		int chunk_size = 0;
		memcpy(&chunk_size, data + offset, 4);
		offset += sizeof(int);

		if (std::string("PRM_") == std::string(chunk))
		{
			memcpy(&shadingModel_, data + offset, 4);
			offset += sizeof(int);

			int hasNormal = 0;
			memcpy(&hasNormal, data + offset, 4);
			offset += sizeof(int);

			int hasRefraction = 0;
			memcpy(&hasRefraction, data + offset, 4);
			offset += sizeof(int);

			hasRefraction_ = hasRefraction > 0;

			memcpy(&customData1Count_, data + offset, 4);
			offset += sizeof(int);

			memcpy(&customData2Count_, data + offset, 4);
			offset += sizeof(int);

			int textureCount = 0;
			memcpy(&textureCount, data + offset, 4);
			offset += sizeof(int);

			for (auto i = 0; i < textureCount; i++)
			{
				int strNameLength = 0;
				memcpy(&strNameLength, data + offset, 4);
				offset += sizeof(int);

				auto name = std::string((const char*)(data + offset));
				offset += strNameLength;

				// name is for human, uniformName is a variable name after 3
				if (version >= 3)
				{
					int strUniformNameLength = 0;
					memcpy(&strUniformNameLength, data + offset, 4);
					offset += sizeof(int);

					name = std::string((const char*)(data + offset));
					offset += strUniformNameLength;
				}

				int strDefaultPathLength = 0;
				memcpy(&strDefaultPathLength, data + offset, 4);
				offset += sizeof(int);

				// defaultpath
				offset += strDefaultPathLength;

				int index = 0;
				memcpy(&index, data + offset, 4);
				offset += sizeof(int);

				// priority
				offset += sizeof(int);

				// param
				offset += sizeof(int);

				// valuetexture
				offset += sizeof(int);

				// sampler
				int sampler = 0;
				memcpy(&sampler, data + offset, 4);
				offset += sizeof(int);

				Texture texture;
				texture.Name = name;
				texture.Index = index;
				texture.Wrap = static_cast<TextureWrapType>(sampler);
				textures_.push_back(texture);
			}

			int uniformCount = 0;
			memcpy(&uniformCount, data + offset, 4);
			offset += sizeof(int);

			for (auto i = 0; i < uniformCount; i++)
			{
				int strLength = 0;
				memcpy(&strLength, data + offset, 4);
				offset += sizeof(int);

				auto name = std::string((const char*)(data + offset));
				offset += strLength;

				// name is for human, uniformName is a variable name after 3
				if (version >= 3)
				{
					int strUniformNameLength = 0;
					memcpy(&strUniformNameLength, data + offset, 4);
					offset += sizeof(int);

					name = std::string((const char*)(data + offset));
					offset += strUniformNameLength;
				}

				// offset
				offset += sizeof(int);

				// priority
				offset += sizeof(int);

				int type = 0;
				memcpy(&type, data + offset, 4);
				offset += sizeof(int);

				// default values
				offset += sizeof(int) * 4;

				Uniform uniform;
				uniform.Name = name;
				uniform.Index = type;
				uniforms_.push_back(uniform);
			}
		}
		else if (std::string("GENE") == std::string(chunk))
		{
			int codeLength = 0;
			memcpy(&codeLength, data + offset, 4);
			offset += sizeof(int);

			auto str = std::string((const char*)(data + offset));
			genericCode_ = str;
			offset += codeLength;
		}
		else
		{
			offset += chunk_size;
		}
	}

	return true;
}

ShadingModelType Material::GetShadingModel() const
{
	return shadingModel_;
}

void Material::SetShadingModel(ShadingModelType shadingModel)
{
	shadingModel_ = shadingModel;
}

bool Material::GetIsSimpleVertex() const
{
	return isSimpleVertex_;
}

void Material::SetIsSimpleVertex(bool isSimpleVertex)
{
	isSimpleVertex_ = isSimpleVertex;
}

bool Material::GetHasRefraction() const
{
	return hasRefraction_;
}

void Material::SetHasRefraction(bool hasRefraction)
{
	hasRefraction_ = hasRefraction;
}

const char* Material::GetGenericCode() const
{
	return genericCode_.c_str();
}

void Material::SetGenericCode(const char* code)
{
	genericCode_ = code;
}

uint64_t Material::GetGUID() const
{
	return guid_;
}

void Material::SetGUID(uint64_t guid)
{
	guid_ = guid;
}

TextureWrapType Material::GetTextureWrap(int32_t index) const
{
	return textures_.at(index).Wrap;
}

void Material::SetTextureWrap(int32_t index, TextureWrapType value)
{
	textures_.at(index).Wrap = value;
}

int32_t Material::GetTextureIndex(int32_t index) const
{
	return textures_.at(index).Index;
}

void Material::SetTextureIndex(int32_t index, int32_t value)
{
	textures_.at(index).Index = value;
}

const char* Material::GetTextureName(int32_t index) const
{
	return textures_.at(index).Name.c_str();
}

void Material::SetTextureName(int32_t index, const char* name)
{
	textures_.at(index).Name = name;
}

int32_t Material::GetTextureCount() const
{
	return static_cast<int32_t>(textures_.size());
}

void Material::SetTextureCount(int32_t count)
{
	textures_.resize(count);
}

int32_t Material::GetUniformIndex(int32_t index) const
{
	return uniforms_.at(index).Index;
}

void Material::SetUniformIndex(int32_t index, int32_t value)
{
	uniforms_.at(index).Index = value;
}

const char* Material::GetUniformName(int32_t index) const
{
	return uniforms_.at(index).Name.c_str();
}

void Material::SetUniformName(int32_t index, const char* name)
{
	uniforms_.at(index).Name = name;
}

int32_t Material::GetUniformCount() const
{
	return static_cast<int32_t>(uniforms_.size());
}

void Material::SetUniformCount(int32_t count)
{
	uniforms_.resize(count);
}

int32_t Material::GetCustomData1Count() const
{
	if (customData1Count_ == 0)
		return 0;

	// because opengl doesn't support swizzle with float
	return std::max(customDataMinCount_, customData1Count_);
}

void Material::SetCustomData1Count(int32_t count)
{
	customData1Count_ = count;
}

int32_t Material::GetCustomData2Count() const
{
	if (customData2Count_ == 0)
		return 0;

	// because opengl doesn't support swizzle with float
	return std::max(customDataMinCount_, customData2Count_);
}

void Material::SetCustomData2Count(int32_t count)
{
	customData2Count_ = count;
}

} // namespace Effekseer