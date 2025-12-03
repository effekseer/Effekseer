#include "efkMat.StringContainer.h"
#include "ThirdParty/picojson.h"

namespace EffekseerMaterial
{
std::unordered_map<std::string, std::string> StringContainer::values_;

std::string& StringContainer::GetValue(const char* key, const char* defaultValue)
{
	auto key_ = std::string(key);
	auto it = values_.find(key_);

	if (it != values_.end())
	{
		return it->second;
	}

	if (defaultValue != nullptr)
	{
		values_[key_] = defaultValue;
	}
	else
	{
		values_[key_] = key_;
	}

	return values_[key_];
}

bool StringContainer::AddValue(const char* key, const char* value)
{
	auto key_ = std::string(key);
	auto it = values_.find(key_);

	values_[key_] = value;

	return true;
}

void StringContainer::Clear()
{
	values_.clear();
}

} // namespace EffekseerMaterial