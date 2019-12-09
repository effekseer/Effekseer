#pragma once

#include <memory>
#include <string>
#include <vector>

namespace Effekseer
{
class FileSystem
{
public:
	static bool GetIsFile(const std::u16string& path);
	static bool GetIsDirectory(const std::u16string& path);
	static uint64_t GetLastWriteTime(const std::u16string& path);
	static int32_t GetFileSize(const std::u16string& path);
};
} // namespace altseed