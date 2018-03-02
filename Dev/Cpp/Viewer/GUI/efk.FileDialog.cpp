
#include "efk.FileDialog.h"
#include "../3rdParty/nfd/nfd.h"

#include <codecvt>

namespace efk
{
	// http://hasenpfote36.blogspot.jp/2016/09/stdcodecvt.html
	static constexpr std::codecvt_mode mode = std::codecvt_mode::little_endian;

	static std::string utf16_to_utf8(const std::u16string& s)
	{
#if defined(_MSC_VER)
		std::wstring_convert<std::codecvt_utf8_utf16<std::uint16_t, 0x10ffff, mode>, std::uint16_t> conv;
		auto p = reinterpret_cast<const std::uint16_t*>(s.c_str());
		return conv.to_bytes(p, p + s.length());
#else
		std::wstring_convert<std::codecvt_utf8_utf16<char16_t, 0x10ffff, mode>, char16_t> conv;
		return conv.to_bytes(s);
#endif
	}

	static std::u16string utf8_to_utf16(const std::string& s)
	{

#if defined(_MSC_VER)
		std::wstring_convert<std::codecvt_utf8_utf16<std::uint16_t, 0x10ffff, mode>, std::uint16_t> conv;
		auto p = reinterpret_cast<const std::uint16_t*>(s.c_str());
		return std::u16string((const char16_t*)conv.from_bytes(s).c_str());
#else
		std::wstring_convert<std::codecvt_utf8_utf16<char16_t, 0x10ffff, mode>, char16_t> conv;
		return conv.from_bytes(s);
#endif
	}

	std::u16string FileDialog::temp;

	const char16_t* FileDialog::OpenDialog(const char16_t* filterList, const char16_t* defaultPath)
	{
		auto filterList_ = utf16_to_utf8(filterList);
		auto defaultPath_ = utf16_to_utf8(defaultPath);

		nfdchar_t* outPath = NULL;
		nfdresult_t result = NFD_OpenDialog(filterList_.c_str(), defaultPath_.c_str(), &outPath);

		if (result == NFD_OKAY)
		{
			temp = utf8_to_utf16(outPath);
			free(outPath);
			return temp.c_str();
		}
		else if (result == NFD_CANCEL)
		{
			temp = u"";
			return temp.c_str();
		}
		
		temp = u"";
		return temp.c_str();
	}

	const char16_t* FileDialog::SaveDialog(const char16_t* filterList, const char16_t* defaultPath)
	{
		auto filterList_ = utf16_to_utf8(filterList);
		auto defaultPath_ = utf16_to_utf8(defaultPath);

		nfdchar_t* outPath = NULL;
		nfdresult_t result = NFD_SaveDialog(filterList_.c_str(), defaultPath_.c_str(), &outPath);

		if (result == NFD_OKAY)
		{
			temp = utf8_to_utf16(outPath);
			free(outPath);
			return temp.c_str();
		}
		else if (result == NFD_CANCEL)
		{
			temp = u"";
			return temp.c_str();
		}

		temp = u"";
		return temp.c_str();
	}
}