#pragma once

#include "../Platform/PlatformMisc.h"

namespace Effekseer
{
namespace Editor
{

void AddFontFromFileTTF(const char* fontFilepath, const char* commonCharacterTablePath, const char* characterTableName, float size_pixels, float dpi_scale);

std::string GetLanguageKey(SystemLanguage language);

SystemLanguage GetSystemLanguage(const std::string& key);

} // namespace Editor
} // namespace Effekseer