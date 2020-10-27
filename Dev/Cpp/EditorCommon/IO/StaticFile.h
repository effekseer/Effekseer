#pragma once

#include "FileReader.h"
#include <memory>
#include <vector>

namespace Effekseer
{

class StaticFile
{
private:
	std::shared_ptr<StaticFileReader> reader_;
	std::vector<uint8_t> buffer_;

public:
	// Dummy
	StaticFile() = default;

#ifndef SWIG
	StaticFile(std::shared_ptr<StaticFileReader>& reader);
	~StaticFile();
#endif

	void* GetData();
	int GetSize();
	std::u16string GetPath();
	StaticFileType GetFileType();
};

} // namespace Effekseer