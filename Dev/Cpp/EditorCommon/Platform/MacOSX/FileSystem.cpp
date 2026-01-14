#pragma once

#include <sys/stat.h>
#include <stdlib.h>

#include "../../Common/StringHelper.h"
#include "../FileSystem.h"

namespace Effekseer
{
bool FileSystem::GetIsFile(const std::u16string& path) {
    struct stat st;

    if (stat(Tool::StringHelper::ConvertUtf16ToUtf8(path).c_str(), &st) != 0) {
        return false;
    }
    return (st.st_mode & S_IFMT) == S_IFREG;
}

bool FileSystem::GetIsDirectory(const std::u16string& path) {
    struct stat st;

    if (stat(Tool::StringHelper::ConvertUtf16ToUtf8(path).c_str(), &st) != 0) {
        return false;
    }
    return (st.st_mode & S_IFMT) == S_IFDIR;
}

uint64_t FileSystem::GetLastWriteTime(const std::u16string& path) {
    struct stat st;

    if (stat(Tool::StringHelper::ConvertUtf16ToUtf8(path).c_str(), &st) != 0) {
        return 0;
    }

    return st.st_mtime;
}

int32_t FileSystem::GetFileSize(const std::u16string& path) {
    struct stat st;

    if (stat(Tool::StringHelper::ConvertUtf16ToUtf8(path).c_str(), &st) != 0) {
        return 0;
    }

    return st.st_size;
}
}  // namespace altseed