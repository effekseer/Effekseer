#include "Misc.h"
#include "JapaneseFont.h"
#include <cmath>
#include <fstream>
#include <imgui.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "../Common/StringHelper.h"

namespace Effekseer
{
namespace Editor
{

void AddFontFromFileTTF(const char* fontFilepath, const char* commonCharacterTablePath, const char* characterTableName, float size_pixels, float dpi_scale)
{
	ImGuiIO& io = ImGui::GetIO();

	size_pixels = std::roundf(size_pixels * dpi_scale);

	const auto tableName = std::string(characterTableName);

	if (tableName == "japanese")
	{
		io.Fonts->AddFontFromFileTTF(fontFilepath, size_pixels, nullptr, glyphRangesJapanese);
	}
	else if (tableName == "chinese")
	{
		io.Fonts->AddFontFromFileTTF(fontFilepath, size_pixels, nullptr, io.Fonts->GetGlyphRangesChineseFull());
	}
	else if (tableName == "korean")
	{
		io.Fonts->AddFontFromFileTTF(fontFilepath, size_pixels, nullptr, io.Fonts->GetGlyphRangesKorean());
	}
	else if (tableName == "thai")
	{
		io.Fonts->AddFontFromFileTTF(fontFilepath, size_pixels, nullptr, io.Fonts->GetGlyphRangesThai());
	}
	else
	{
		io.Fonts->AddFontFromFileTTF(fontFilepath, size_pixels, nullptr, io.Fonts->GetGlyphRangesDefault());
	}

	{
		static std::vector<ImWchar> ranges;
		ranges.clear();

		std::ifstream f(commonCharacterTablePath);
		if (f.is_open())
		{
			auto str = std::string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
			auto str16 = Tool::StringHelper::ConvertUtf8ToUtf16(str);

			for (const auto c : str16)
			{
				ranges.emplace_back(c);
				ranges.emplace_back(c);
			}
			ranges.emplace_back(0);
			ImFontConfig config;
			config.MergeMode = true;
			io.Fonts->AddFontFromFileTTF(fontFilepath, size_pixels, &config, ranges.data());
		}
	}

	io.Fonts->Build();
}

std::string GetLanguageKey(SystemLanguage language)
{
	if (language == SystemLanguage::Japanese)
	{
		return "ja";
	}
	else if (language == SystemLanguage::English)
	{
		return "en";
	}

	// Fallback
	return "en";
}

SystemLanguage GetSystemLanguage(const std::string& key)
{
	if (key == "ja")
	{
		return SystemLanguage::Japanese;
	}
	else if (key == "en")
	{
		return SystemLanguage::English;
	}

	return SystemLanguage::Unknown;
}

} // namespace Editor
} // namespace Effekseer