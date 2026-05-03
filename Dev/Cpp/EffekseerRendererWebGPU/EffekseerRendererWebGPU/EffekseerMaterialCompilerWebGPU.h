#pragma once

#include "../../Effekseer/Effekseer/Material/Effekseer.MaterialCompiler.h"

namespace Effekseer
{

class MaterialCompilerWebGPU : public MaterialCompiler, ReferenceObject
{
public:
	MaterialCompilerWebGPU() = default;
	~MaterialCompilerWebGPU() override = default;

	CompiledMaterialBinary* Compile(MaterialFile* material, int32_t maximumUniformCount, int32_t maximumTextureCount);
	CompiledMaterialBinary* Compile(MaterialFile* material) override;

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
