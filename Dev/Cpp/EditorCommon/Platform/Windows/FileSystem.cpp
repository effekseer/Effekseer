#pragma once

#include <filesystem>
namespace fs = std::filesystem;

#include "../FileSystem.h"

namespace Effekseer
{
bool FileSystem::GetIsFile(const std::u16string& path) { return fs::is_regular_file(path); }

bool FileSystem::GetIsDirectory(const std::u16string& path) { return fs::is_directory(path); }

int32_t FileSystem::GetLastWriteTime(const std::u16string& path)
{
	std::error_code ec;
	return fs::last_write_time(path, ec).time_since_epoch().count();
}

int32_t FileSystem::GetFileSize(const std::u16string& path) { return fs::file_size(path); }
} // namespace altseed