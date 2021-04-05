#include "Misc.h"
#include "JapaneseFont.h"
#include <cmath>
#include <imgui.h>
#include <string>

namespace Effekseer
{
namespace Editor
{

void AddFontFromFileTTF(const char* filename, const char* characterSet, float size_pixels, float dpi_scale)
{
	ImGuiIO& io = ImGui::GetIO();

	size_pixels = std::roundf(size_pixels * dpi_scale);

	if (std::string(characterSet) == "japanese")
	{
		io.Fonts->AddFontFromFileTTF(filename, size_pixels, nullptr, glyphRangesJapanese);
	}
	else if (std::string(characterSet) == "simplified_chinese")
	{
		io.Fonts->AddFontFromFileTTF(filename, size_pixels, nullptr, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());	
	}
	else if (std::string(characterSet) == "chinese")
	{
		io.Fonts->AddFontFromFileTTF(filename, size_pixels, nullptr, io.Fonts->GetGlyphRangesChineseFull());
	}
	else if (std::string(characterSet) == "korean")
	{
		io.Fonts->AddFontFromFileTTF(filename, size_pixels, nullptr, io.Fonts->GetGlyphRangesKorean());
	}
	else if (std::string(characterSet) == "thai")
	{
		io.Fonts->AddFontFromFileTTF(filename, size_pixels, nullptr, io.Fonts->GetGlyphRangesThai());
	}
	else
	{
		io.Fonts->AddFontFromFileTTF(filename, size_pixels, nullptr, io.Fonts->GetGlyphRangesDefault());	
	}
	io.Fonts->Build();
}

} // namespace Editor
} // namespace Effekseer