
#ifndef __EFFEKSEER_MATERIAL_H__
#define __EFFEKSEER_MATERIAL_H__

#include <Effekseer.h>
#include <array>
#include <assert.h>
#include <map>
#include <sstream>
#include <string.h>
#include <vector>

namespace Effekseer
{

class Material
{
private:
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

	uint64_t guid_ = 0;

	std::string genericCode_;

	bool hasRefraction_ = false;

	bool isSimpleVertex_ = false;

	ShadingModelType shadingModel_;

	std::vector<Texture> textures_;

	std::vector<Uniform> uniforms_;

public:
	Material() = default;
	virtual ~Material() = default;

	virtual bool Load(const uint8_t* data, int32_t size);

	virtual ShadingModelType GetShadingModel() const;

	virtual bool GetIsSimpleVertex() const;

	virtual bool GetHasRefraction() const;

	virtual const char* GetGenericCode() const;

	virtual uint64_t GetGUID() const;

	virtual int32_t GetTextureIndex(int32_t index) const;

	virtual const char* GetTextureName(int32_t index) const;

	int32_t GetTextureCount() const;

	virtual int32_t GetUniformIndex(int32_t index) const;

	virtual const char* GetUniformName(int32_t index) const;

	int32_t GetUniformCount() const;
};

} // namespace Effekseer

#endif // __EFFEKSEER_MATERIAL_H__