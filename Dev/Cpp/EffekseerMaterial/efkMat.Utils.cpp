#include "efkMat.Utils.h"

namespace EffekseerMaterial
{

bool starts_with(const std::string& s, const std::string& prefix)
{
	auto size = prefix.size();
	if (s.size() < size)
		return false;
	return std::equal(std::begin(prefix), std::end(prefix), std::begin(s));
}

std::string Replace(std::string v, std::string pre, std::string past)
{
	auto ret = v;
	auto pos = ret.find(pre);

	while (pos != std::string::npos)
	{
		ret.replace(pos, pre.length(), past);
		pos = ret.find(pre);
	}

	return ret;
}

std::string EspcapeUserParamName(const char* name)
{

	auto name_ = std::string(name);
	auto prefix = std::string("_efk_");
	if (name_.size() < prefix.size())
		return name_;

	if (std::equal(std::begin(prefix), std::end(prefix), std::begin(name_)))
	{
		return name + 5;
	}

	return name;
}

std::string GetConstantTextureName(int64_t guid)
{
	return std::string("_ConstantTexture_") + std::to_string(guid);
}

bool IsValidName(const std::string& name)
{
	if (name.size() == 0)
	{
		return false;
	}

	if (std::isdigit(name[0]))
	{
		return false;
	}

	if (name[0] == '_')
	{
		return false;
	}

	for (size_t i = 0; i < name.size(); i++)
	{
		// only ascii
		if (name[i] <= 0)
		{
			return false;
		}

		if (!std::isdigit(name[i]) &&
			!std::isalpha(name[i]) && name[i] != '_')
		{
			return false;
		}
	}

	return true;
}

} // namespace EffekseerMaterial