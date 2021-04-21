
#pragma once

#include "../Effekseer/Effekseer/Material/Effekseer.CompiledMaterial.h"
#include "../Effekseer/Effekseer/Material/Effekseer.MaterialCompiler.h"
#include "../Effekseer/Effekseer/Material/Effekseer.MaterialFile.h"
#include "DynamicLinkLibrary.h"
#include <map>

class CompiledMaterialGenerator
{
private:
	std::map<Effekseer::CompiledMaterialPlatformType, std::shared_ptr<DynamicLinkLibrary>> dlls_;

public:
	CompiledMaterialGenerator();

	virtual ~CompiledMaterialGenerator();

	bool Initialize(const char* directory);

	bool Compile(const char* dstPath, const char* srcPath);
};