#pragma once

#include "FileReader.h"

namespace Effekseer
{

class StaticFile
{
private:
	FileReader* reader;

public:
	StaticFile();
	~StaticFile();

	void* GetData();
	int GetSize();
};

} // namespace Effekseer