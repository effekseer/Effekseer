
#pragma once

#include <string>

namespace efk
{
class FileDialog
{
	static std::u16string temp;

public:
	static const char16_t* OpenDialog(const char16_t* filterList, const char16_t* defaultPath);

	static const char16_t* SaveDialog(const char16_t* filterList, const char16_t* defaultPath);

	static const char16_t* PickFolder(const char16_t* defaultPath);
};
} // namespace efk