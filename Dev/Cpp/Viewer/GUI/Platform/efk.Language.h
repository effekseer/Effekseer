
#pragma once

#include <stdio.h>
#include <stdint.h>

enum class EfkLanguage : int32_t
{
	Japanese,
	English,
	Unknown = -1,
};

EfkLanguage GetEfkLanguage();