#include "../PlatformMisc.h"

#include <locale.h>
#include <string>

namespace Effekseer
{

SystemLanguage GetSystemLanguage()
{
	auto localeStr = std::string(setlocale(LC_ALL, NULL));

	if (localeStr.find("ja-") != std::string::npos)
		return SystemLanguage::Japanese;

	if (localeStr.find("en") != std::string::npos)
		return SystemLanguage::English;

	return SystemLanguage::Unknown;
}

} // namespace Effekseer
