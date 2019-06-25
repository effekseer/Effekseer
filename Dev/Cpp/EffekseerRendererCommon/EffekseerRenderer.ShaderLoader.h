
#ifndef __EFFEKSEERRENDERER_SHADER_LOADER_H__
#define __EFFEKSEERRENDERER_SHADER_LOADER_H__

#include <Effekseer.h>
#include <assert.h>
#include <sstream>
#include <string.h>

namespace EffekseerRenderer
{

class ShaderLoader
{
private:
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

	bool HasRefraction = false;

	std::string GenericCode;

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

		uint64_t guid = 0;
		memcpy(&guid, data + offset, 8);
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

			if (std::string("gene") == std::string(chunk))
			{
				int hasRefraction = 0;
				memcpy(&hasRefraction, data + offset, 4);
				offset += sizeof(int);

				HasRefraction = hasRefraction > 0;

				int textureCount = 0;
				memcpy(&textureCount, data + offset, 4);
				offset += sizeof(int);

				for (auto i = 0; i < textureCount; i++)
				{
					int strLength = 0;
					memcpy(&strLength, data + offset, 4);
					offset += sizeof(int);

					auto name = std::string((const char*)(data + offset));
					offset += strLength;

					int index = 0;
					memcpy(&index, data + offset, 4);
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

					int type = 0;
					memcpy(&type, data + offset, 4);
					offset += sizeof(int);

					Uniform uniform;
					uniform.Name = name;
					uniform.Index = type;
					Uniforms.push_back(uniform);
				}

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
	}

	/**
		@brief	generate a shader
		@note
		TODO : support distortion etc
	*/
	std::string GenerateShader()
	{
		std::ostringstream maincode;

		auto preCode = R"(
IN lowp vec4 vaColor;
IN mediump vec4 vaTexCoord;

)";
		maincode << preCode;

		for (size_t i = 0; i < Textures.size(); i++)
		{
			auto& texture = Textures[i];
			maincode << "uniform sampler2D " << texture.Name << ";" << std::endl;
		}

		// predefined

		maincode << "uniform vec4 predefined_uniform;" << std::endl;

		for (size_t i = 0; i < Uniforms.size(); i++)
		{
			auto& uniform = Uniforms[i];
			if (uniform.Index == 0)
				maincode << "uniform float " << uniform.Name << ";" << std::endl;
			if (uniform.Index == 1)
				maincode << "uniform vec2 " << uniform.Name << ";" << std::endl;
			if (uniform.Index == 2)
				maincode << "uniform vec3 " << uniform.Name << ";" << std::endl;
			if (uniform.Index == 3)
				maincode << "uniform vec4 " << uniform.Name << ";" << std::endl;
		}

		auto baseCode = GenericCode;
		baseCode = Replace(baseCode, "$F1$", "float");
		baseCode = Replace(baseCode, "$F2$", "vec2");
		baseCode = Replace(baseCode, "$F3$", "vec3");
		baseCode = Replace(baseCode, "$F4$", "vec4");
		baseCode = Replace(baseCode, "$TIME$", "predefined_uniform.x");
		baseCode = Replace(baseCode, "$UV$", "vaTexCoord.xy");
		baseCode = Replace(baseCode, "$INPUT$", "");

		// replace textures
		for (size_t i = 0; i < Textures.size(); i++)
		{
			auto& texture = Textures[i];
			std::string keyP = "$TEX_P" + std::to_string(texture.Index) + "$";
			std::string keyS = "$TEX_S" + std::to_string(texture.Index) + "$";

			baseCode = Replace(baseCode, keyP, "TEX2D(" + texture.Name + ",");
			baseCode = Replace(baseCode, keyS, ")");
		}

		maincode << baseCode;

		auto postCode = R"(

void main()
{
	FRAGCOLOR = Calculate();
}
)";
		maincode << postCode;

		return maincode.str();
	}
};

} // namespace EffekseerRenderer

#endif // __EFFEKSEERRENDERER_SHADER_BASE_H__