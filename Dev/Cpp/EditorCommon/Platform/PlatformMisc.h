#pragma once

#include <stdint.h>
#include <string>

namespace Effekseer
{

enum class SystemLanguage : int32_t
{
	Japanese = 0,
	English = 1,
	Unknown = -1,
};

/**
	@brief	get current a language of operation system
*/
SystemLanguage GetSystemLanguage();

} // namespace Effekseer