
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

enum class CompiledShaderPlatformType : int32_t
{
	DirectX9 = 0,
	// DirectX10 = 1,
	DirectX11 = 2,
	DirectX12 = 3,
	OpenGL2 = 10,
	OpenGL3 = 11,
	OpenGLES2 = 15,
	OpenGLES3 = 16,
	Metal = 20,
	Vulkan = 30,
	PS4 = 40,
	Switch = 50,
};

class CompiledShaderGenerator
{
	struct Platform
	{
		std::vector<uint8_t> standardVS;
		std::vector<uint8_t> standardPS;

		std::vector<uint8_t> modelVS;
		std::vector<uint8_t> modelPS;
	};

	std::map<CompiledShaderPlatformType, Platform> platforms;

public:
	bool Load(const uint8_t* data, int32_t size)
	{
		int offset = 0;

		// header
		char prefix[5];

		memcpy(prefix, data + offset, 4);
		offset += sizeof(int);

		prefix[4] = 0;

		if (std::string("eMCB") != std::string(prefix))
			return false;

		int version = 0;
		memcpy(&version, data + offset, 4);
		offset += sizeof(int);

		uint64_t guid = 0;
		memcpy(&guid, data + offset, 8);
		offset += sizeof(uint64_t);

		while (0 <= offset && offset < size)
		{
			int chunk;
			memcpy(&chunk, data + offset, 4);
			offset += sizeof(int);

			int chunk_size = 0;
			memcpy(&chunk_size, data + offset, 4);
			offset += sizeof(int);

			Platform platform;

			int standard_vs_size = 0;
			memcpy(&standard_vs_size, data + offset, 4);
			offset += sizeof(int);

			platform.standardVS.resize(standard_vs_size);
			memcpy(platform.standardVS.data(), data + offset, standard_vs_size);
			offset += standard_vs_size;

			int standard_ps_size = 0;
			memcpy(&standard_ps_size, data + offset, 4);
			offset += sizeof(int);

			platform.standardPS.resize(standard_ps_size);
			memcpy(platform.standardPS.data(), data + offset, standard_ps_size);
			offset += standard_ps_size;

			int model_vs_size = 0;
			memcpy(&model_vs_size, data + offset, 4);
			offset += sizeof(int);

			platform.modelVS.resize(model_vs_size);
			memcpy(platform.modelVS.data(), data + offset, model_vs_size);
			offset += model_vs_size;

			int model_ps_size = 0;
			memcpy(&model_ps_size, data + offset, 4);
			offset += sizeof(int);

			platform.modelPS.resize(model_ps_size);
			memcpy(platform.modelPS.data(), data + offset, model_ps_size);
			offset += model_ps_size;

			platforms[static_cast<CompiledShaderPlatformType>(chunk)] = platform;
		}

		return true;
	}

	void Save(std::vector<uint8_t>& dst, int version, uint64_t guid)
	{
		dst.reserve(1024 * 64);

		struct Header
		{
			char header[4];
			int version = 0;
			uint64_t guid = 0;
		};

		Header h;
		h.header[0] = 'e';
		h.header[1] = 'M';
		h.header[2] = 'C';
		h.header[3] = 'B';
		h.version = version;
		h.guid = guid;

		dst.resize(sizeof(Header));
		memcpy(dst.data(), &h, sizeof(Header));

		for (auto& kv : platforms)
		{
			int32_t offset = dst.size();
			int32_t bodySize = sizeof(int) * 4 + kv.second.standardVS.size() + kv.second.standardPS.size() + kv.second.modelVS.size() +
							   kv.second.modelPS.size();
			dst.resize(sizeof(int) * 2 + bodySize);

			memcpy(dst.data() + offset, &(kv.first), sizeof(int));
			offset += sizeof(int);

			memcpy(dst.data() + offset, &(bodySize), sizeof(int));
			offset += sizeof(int);

			std::array<std::vector<uint8_t>*, 4> bodies = {
				&(kv.second.standardVS),
				&(kv.second.standardPS),
				&(kv.second.modelVS),
				&(kv.second.modelPS),
			};

			for (auto& body : bodies)
			{
				int32_t bodySize = body->size();

				memcpy(dst.data() + offset, &(bodySize), sizeof(int));
				offset += sizeof(int);

				memcpy(dst.data() + offset, body->data(), bodySize);
				offset += bodySize;
			}
		}
	}

	void UpdateData(const std::vector<uint8_t>& standardVS,
					const std::vector<uint8_t>& standardPS,
					const std::vector<uint8_t>& modelVS,
					const std::vector<uint8_t>& modelPS,
					CompiledShaderPlatformType type)
	{
		Platform platform;
		platform.standardVS = standardVS;
		platform.standardPS = standardPS;
		platform.modelVS = modelVS;
		platform.modelPS = modelPS;
		platforms[type] = platform;
	}
};

class CompiledShader
{
	std::vector<uint8_t> standardVS;
	std::vector<uint8_t> standardPS;

	std::vector<uint8_t> modelVS;
	std::vector<uint8_t> modelPS;

public:
	bool Load(const uint8_t* data, int32_t size, CompiledShaderPlatformType type)
	{
		int offset = 0;

		// header
		char prefix[5];

		memcpy(prefix, data + offset, 4);
		offset += sizeof(int);

		prefix[4] = 0;

		if (std::string("eMCB") != std::string(prefix))
			return false;

		int version = 0;
		memcpy(&version, data + offset, 4);
		offset += sizeof(int);

		uint64_t guid = 0;
		memcpy(&guid, data + offset, 8);
		offset += sizeof(uint64_t);

		while (0 <= offset && offset < size)
		{
			int chunk;
			memcpy(&chunk, data + offset, 4);
			offset += sizeof(int);

			int chunk_size = 0;
			memcpy(&chunk_size, data + offset, 4);
			offset += sizeof(int);

			if (chunk != static_cast<int32_t>(type))
			{
				offset += chunk_size;
				continue;
			}

			int standard_vs_size = 0;
			memcpy(&standard_vs_size, data + offset, 4);
			offset += sizeof(int);

			standardVS.resize(standard_vs_size);
			memcpy(standardVS.data(), data + offset, standard_vs_size);
			offset += standard_vs_size;

			int standard_ps_size = 0;
			memcpy(&standard_ps_size, data + offset, 4);
			offset += sizeof(int);

			standardPS.resize(standard_ps_size);
			memcpy(standardPS.data(), data + offset, standard_ps_size);
			offset += standard_ps_size;

			int model_vs_size = 0;
			memcpy(&model_vs_size, data + offset, 4);
			offset += sizeof(int);

			modelVS.resize(model_vs_size);
			memcpy(modelVS.data(), data + offset, model_vs_size);
			offset += model_vs_size;

			int model_ps_size = 0;
			memcpy(&model_ps_size, data + offset, 4);
			offset += sizeof(int);

			modelPS.resize(model_ps_size);
			memcpy(modelPS.data(), data + offset, model_ps_size);
			offset += model_ps_size;

			return true;
		}

		return false;
	}

	const std::vector<uint8_t>& GetStandardVS() const { return standardVS; }
	const std::vector<uint8_t>& GetStandardPS() const { return standardPS; }
	const std::vector<uint8_t>& GetModelVS() const { return modelVS; }
	const std::vector<uint8_t>& GetModelPS() const { return modelPS; }
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
	enum class ShadingModelType : int32_t
	{
		Lit,
		Unlit,
	};

	enum class ShaderType : int32_t
	{
		Standard,
		Model,
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

	ShadingModelType ShadingModel;

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
	virtual std::string GenerateShader(ShaderType shaderType) { return std::string(); }
};

} // namespace EffekseerRenderer

#endif // __EFFEKSEERRENDERER_SHADER_BASE_H__