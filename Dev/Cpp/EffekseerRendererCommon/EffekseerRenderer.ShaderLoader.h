
#ifndef __EFFEKSEERRENDERER_SHADER_LOADER_H__
#define __EFFEKSEERRENDERER_SHADER_LOADER_H__

#include <Effekseer.h>
#include <array>
#include <assert.h>
#include <map>
#include <sstream>
#include <string.h>
#include <vector>

namespace EffekseerRenderer
{

struct ShaderData
{
	std::string CodeVS;
	std::string CodePS;
};

class ShaderLoader
{
protected:
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

public:
	enum class ShaderType : int32_t
	{
		Standard,
		Model,
		Refraction,
		RefractionModel,
	};

	ShaderLoader() = default;
	virtual ~ShaderLoader() = default;

	// set uniform

	struct Texture
	{
		std::string Name;
		int32_t Index;
	};

	struct Uniform
	{
		std::string Name;
		int32_t Index;
	};

	std::vector<Texture> Textures;

	std::vector<Uniform> Uniforms;

	::Effekseer::ShadingModelType ShadingModel;

	bool HasRefraction = false;

	bool IsSimpleVertex = false;

	std::string GenericCode;

	uint64_t GUID = 0;

	virtual bool Load(const uint8_t* data, int32_t size)
	{
		int offset = 0;

		// header
		char prefix[5];

		memcpy(prefix, data + offset, 4);
		offset += sizeof(int);

		prefix[4] = 0;

		if (std::string("efkM") != std::string(prefix))
			return false;

		int version = 0;
		memcpy(&version, data + offset, 4);
		offset += sizeof(int);

		memcpy(&GUID, data + offset, 8);
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

			if (std::string("para") == std::string(chunk))
			{
				memcpy(&ShadingModel, data + offset, 4);
				offset += sizeof(int);

				int hasNormal = 0;
				memcpy(&hasNormal, data + offset, 4);
				offset += sizeof(int);

				int hasRefraction = 0;
				memcpy(&hasRefraction, data + offset, 4);
				offset += sizeof(int);

				HasRefraction = hasRefraction > 0;

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

					int strDefaultPathLength = 0;
					memcpy(&strDefaultPathLength, data + offset, 4);
					offset += sizeof(int);

					// defaultpath
					offset += strDefaultPathLength;

					int index = 0;
					memcpy(&index, data + offset, 4);
					offset += sizeof(int);

					// param
					offset += sizeof(int);

					// valuetexture
					offset += sizeof(int);

					Texture texture;
					texture.Name = name;
					texture.Index = index;
					Textures.push_back(texture);
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

					// offset
					offset += sizeof(int);

					int type = 0;
					memcpy(&type, data + offset, 4);
					offset += sizeof(int);

					// default values
					offset += sizeof(int) * 4;

					Uniform uniform;
					uniform.Name = name;
					uniform.Index = type;
					Uniforms.push_back(uniform);
				}
			}
			else if (std::string("gene") == std::string(chunk))
			{
				int codeLength = 0;
				memcpy(&codeLength, data + offset, 4);
				offset += sizeof(int);

				auto str = std::string((const char*)(data + offset));
				GenericCode = str;
				offset += codeLength;
			}
			else
			{
				offset += chunk_size;
				break;
			}
		}

		return true;
	}

	/**
		@brief	generate a shader
		@note
		TODO : support distortion etc
	*/
	virtual ShaderData GenerateShader(ShaderType shaderType) { return ShaderData(); }
};

} // namespace EffekseerRenderer

#endif // __EFFEKSEERRENDERER_SHADER_BASE_H__