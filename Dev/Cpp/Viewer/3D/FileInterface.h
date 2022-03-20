#pragma once

#include <Effekseer.h>

namespace Effekseer::Tool
{

class EffekseerFile : public Effekseer::FileInterface
{
public:
	EffekseerFile() = default;

	~EffekseerFile() override = default;

	Effekseer::FileReaderRef OpenRead(const EFK_CHAR* path);

	Effekseer::FileWriterRef OpenWrite(const EFK_CHAR* path);
};

} // namespace Effekseer::Tool