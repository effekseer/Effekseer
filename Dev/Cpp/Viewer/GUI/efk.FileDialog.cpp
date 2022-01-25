#include "efk.FileDialog.h"
#include "Effekseer.h"
#include <nfd.h>

#include <codecvt>
#include <iostream>

namespace efk
{

std::u16string FileDialog::temp = u"";

const char16_t* FileDialog::OpenDialog(const char16_t* filterList, const char16_t* defaultPath)
{
	char filterList_[256], defaultPath_[1024];
	Effekseer::ConvertUtf16ToUtf8(filterList_, sizeof(filterList_), filterList);
	Effekseer::ConvertUtf16ToUtf8(defaultPath_, sizeof(defaultPath_), defaultPath);

	nfdchar_t* outPath = nullptr;
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
	char filterList_[256], defaultPath_[1024];
	Effekseer::ConvertUtf16ToUtf8(filterList_, sizeof(filterList_), filterList);
	Effekseer::ConvertUtf16ToUtf8(defaultPath_, sizeof(defaultPath_), defaultPath);

	nfdchar_t* outPath = nullptr;
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

const char16_t* FileDialog::PickFolder(const char16_t* defaultPath)
{
	char defaultPath_[1024];
	Effekseer::ConvertUtf16ToUtf8(defaultPath_, sizeof(defaultPath_), defaultPath);

	nfdchar_t* outPath = nullptr;
	nfdresult_t result = NFD_PickFolder(defaultPath_, &outPath);

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
