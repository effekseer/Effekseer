
#pragma once

#include "../../Effekseer/Effekseer/Material/Effekseer.MaterialCompiler.h"
#include <vector>

namespace Effekseer
{

class MaterialCompilerGL : public MaterialCompiler, ReferenceObject
{
private:
public:
	MaterialCompilerGL() = default;

	virtual ~MaterialCompilerGL() = default;

	CompiledMaterialBinary* Compile(Material* material, int32_t maximumTextureCount);

	CompiledMaterialBinary* Compile(Material* material) override;

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
