#pragma once

#if __GNUC__ >= 9
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

#include "../FileSystem.h"

namespace Effekseer
{
bool FileSystem::GetIsFile(const std::u16string& path) { return fs::is_regular_file(path); }

bool FileSystem::GetIsDirectory(const std::u16string& path) { return fs::is_directory(path); }

uint64_t FileSystem::GetLastWriteTime(const std::u16string& path) {
    std::error_code ec;
    return fs::last_write_time(path, ec).time_since_epoch().count();
}

int32_t FileSystem::GetFileSize(const std::u16string& path) { return fs::file_size(path); }
}  // namespace altseed