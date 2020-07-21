#include "efk.FileDialog.h"
#include "../3rdParty/nfd/nfd.h"
#include "Effekseer.h"

#include <codecvt>
#include <iostream>

namespace efk
{
#if 0
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
#endif

std::u16string FileDialog::temp = u"";

int32_t ConvertUtf8ToUtf16_(char16_t* dst, int32_t dst_size, const char* src)
{
    int32_t i, code = 0;
    int8_t c0, c1, c2 = 0;
    int8_t* srci = reinterpret_cast<int8_t*>(const_cast<char*>(src));
    if (dst_size == 0)
        return 0;

    dst_size -= 1;

    for (i = 0; i < dst_size; i++)
    {
        uint16_t wc;

        c0 = *srci;
        srci++;
        if (c0 == '\0')
        {
            break;
        }
        // UTF8からUTF16に変換
        code = (uint8_t)c0 >> 4;
        if (code <= 7)
        {
            // 8bit文字
            wc = c0;
        }
        else if (code >= 12 && code <= 13)
        {
            // 16bit文字
            c1 = *srci;
            srci++;
            wc = ((c0 & 0x1F) << 6) | (c1 & 0x3F);
        }
        else if (code == 14)
        {
            // 24bit文字
            c1 = *srci;
            srci++;
            c2 = *srci;
            srci++;
            wc = ((c0 & 0x0F) << 12) | ((c1 & 0x3F) << 6) | (c2 & 0x3F);
        }
        else
        {
            continue;
        }
        dst[i] = wc;
    }
    dst[i] = 0;
    return i;
}

const char16_t* FileDialog::OpenDialog(const char16_t* filterList, const char16_t* defaultPath)
{
	char filterList_[256], defaultPath_[1024];
	Effekseer::ConvertUtf16ToUtf8((int8_t*)filterList_, sizeof(filterList_), (const int16_t*)filterList);
	Effekseer::ConvertUtf16ToUtf8((int8_t*)defaultPath_, sizeof(defaultPath_), (const int16_t*)defaultPath);

	nfdchar_t* outPath = NULL;
	nfdresult_t result = NFD_OpenDialog(filterList_, defaultPath_, &outPath);

	if (result == NFD_OKAY)
	{
		std::array<char16_t, 1024> outPath_;
        outPath_.fill(0);

		Effekseer::ConvertUtf8ToUtf16(outPath_.data(), 1024, outPath);

		temp = std::u16string(outPath_.data());
        
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
	// auto filterList_ = utf16_to_utf8(filterList);
	// auto defaultPath_ = utf16_to_utf8(defaultPath);
	char filterList_[256], defaultPath_[1024];
	Effekseer::ConvertUtf16ToUtf8((int8_t*)filterList_, sizeof(filterList_), (const int16_t*)filterList);
	Effekseer::ConvertUtf16ToUtf8((int8_t*)defaultPath_, sizeof(defaultPath_), (const int16_t*)defaultPath);

	nfdchar_t* outPath = NULL;
	nfdresult_t result = NFD_SaveDialog(filterList_, defaultPath_, &outPath);

	if (result == NFD_OKAY)
	{
		char16_t outPath_[1024];
		Effekseer::ConvertUtf8ToUtf16(outPath_, 1024, outPath);
		temp = outPath_;
		// temp = utf8_to_utf16(outPath);
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
} // namespace efk
