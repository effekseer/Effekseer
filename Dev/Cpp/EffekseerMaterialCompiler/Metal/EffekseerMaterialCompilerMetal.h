
#pragma once

#include "../../Effekseer/Effekseer/Material/Effekseer.MaterialCompiler.h"
#include <vector>

namespace Effekseer
{

class MaterialCompilerMetal : public MaterialCompiler, public ReferenceObject
{
private:
public:
	MaterialCompilerMetal() = default;

	virtual ~MaterialCompilerMetal() = default;

	CompiledMaterialBinary* Compile(MaterialFile* materialFile, int32_t maximumUniformCount, int32_t maximumTextureCount);

	CompiledMaterialBinary* Compile(MaterialFile* materialFile) override;

	int AddRef() override
	{
		return ReferenceObject::AddRef();
	}

	int Release() override
	{
		return ReferenceObject::Release();
	}

	int GetRef() override
	{
		return ReferenceObject::GetRef();
	}
};

} // namespace Effekseer
