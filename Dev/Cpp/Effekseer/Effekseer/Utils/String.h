#ifndef __EFFEKSEER_STRING_H__
#define __EFFEKSEER_STRING_H__

#include <array>
#include <stdint.h>
#include <string>
#include <vector>
#include <iterator>

#include "Effekseer.CustomAllocator.h"

namespace Effekseer
{

template <typename T>
class StringView
{
	using Traits = std::char_traits<T>;

public:
	StringView()
		: ptr_(nullptr)
		, size_(0)
	{
	}

	StringView(const T* ptr)
		: ptr_(ptr)
		, size_(Traits::length(ptr))
	{
	}

	StringView(const T* ptr, size_t size)
		: ptr_(ptr)
		, size_(size)
	{
	}

	template <size_t N>
	StringView(const T ptr[N])
		: ptr_(ptr)
		, size_(N)
	{
	}

	StringView(const CustomString<T>& str)
		: ptr_(str.data())
		, size_(str.size())
	{
	}

	const T* data() const
	{
		return ptr_;
	}

	size_t size() const
	{
		return size_;
	}

	bool operator==(const StringView<T>& rhs) const
	{
		return size() == rhs.size() && Traits::compare(data(), rhs.data(), size()) == 0;
	}

	bool operator!=(const StringView<T>& rhs) const
	{
		return size() != rhs.size() || Traits::compare(data(), rhs.data(), size()) != 0;
	}

	struct Hash
	{
		size_t operator()(const StringView<T>& key) const
		{
			constexpr size_t basis = (sizeof(size_t) == 8) ? 14695981039346656037ULL : 2166136261U;
			constexpr size_t prime = (sizeof(size_t) == 8) ? 1099511628211ULL : 16777619U;

			const uint8_t* data = reinterpret_cast<const uint8_t*>(key.data());
			size_t count = key.size() * sizeof(T);
			size_t val = basis;
			for (size_t i = 0; i < count; i++)
			{
				val ^= static_cast<size_t>(data[i]);
				val *= prime;
			}
			return val;
		}
	};

private:
	const T* ptr_;
	size_t size_;
};

template <typename T, int N>
class FixedSizeString
{
	using Traits = std::char_traits<T>;

public:
	FixedSizeString()
		: size_(0)
	{
		data_[0] = 0;
	}

	FixedSizeString(const T* ptr)
	{
		size_t original_length = Traits::length(ptr);
		size_ = (std::min)(data_.size() - 1, original_length);
		memcpy(data_.data(), ptr, size_ * sizeof(T));
		data_[size_] = 0;
	}

	FixedSizeString<T, N> substr(size_t pos = 0,
								 size_t n = std::string::npos) const
	{
		FixedSizeString<T, N> ret;
		if (n == std::string::npos)
		{
			ret.size_ = size_ - pos;
		}
		else
		{
			ret.size_ = n;
		}

		memcpy(ret.data_.data(), data_.data() + pos, ret.size_ * sizeof(T));
		ret.data_[ret.size_] = 0;

		return ret;
	}

	T& operator[](size_t i)
	{
		return data_[i];
	}

	T operator[](size_t i) const
	{
		return data_[i];
	}

	T back() const
	{
		return data_[size_ - 1];
	}

	const T* data() const
	{
		return data_.data();
	}

	size_t size() const
	{
		return size_;
	}

	FixedSizeString<T, N>& operator+=(const FixedSizeString<T, N>& c)
	{
		const auto new_size = (std::min)(this->size_ + c.size_, static_cast<size_t>(N));
		const auto copied_size = new_size - this->size_;

		memcpy(data_.data() + size_, c.data_.data(), copied_size * sizeof(T));
		size_ = new_size;
		data_[new_size] = 0;
		return *this;
	}

	FixedSizeString<T, N>& operator+=(const T& c)
	{
		if (size_ < data_.size() - 2)
		{
			data_[size_] = c;
			data_[size_ + 1] = 0;
			size_ += 1;
		}

		return *this;
	}

	bool operator==(const FixedSizeString<T, N>& rhs) const
	{
		return size() == rhs.size() && Traits::compare(data(), rhs.data(), size()) == 0;
	}

	bool operator!=(const FixedSizeString<T, N>& rhs) const
	{
		return size() != rhs.size() || Traits::compare(data(), rhs.data(), size()) != 0;
	}

private:
	std::array<T, N + 1> data_;
	size_t size_;
};

class StringHelper
{
public:
	template <typename T>
	static std::vector<std::basic_string<T>> Split(const std::basic_string<T>& s, T delim)
	{
		std::vector<std::basic_string<T>> elems;

		size_t start = 0;

		for (size_t i = 0; i < s.size(); i++)
		{
			if (s[i] == delim)
			{
				elems.emplace_back(s.substr(start, i - start));
				start = i + 1;
			}
		}

		if (start == s.size())
		{
			elems.emplace_back(std::basic_string<T>());
		}
		else
		{
			elems.emplace_back(s.substr(start, s.size() - start));
		}

		return elems;
	}

	template <typename T>
	static std::basic_string<T> Replace(std::basic_string<T> target, std::basic_string<T> from_, std::basic_string<T> to_)
	{
		auto Pos = target.find(from_);

		while (Pos != std::basic_string<T>::npos)
		{
			target.replace(Pos, from_.length(), to_);
			Pos = target.find(from_, Pos + to_.length());
		}

		return target;
	}

	template <typename T, typename U>
	static std::basic_string<T> To(const U* str)
	{
		std::basic_string<T> ret;
		size_t len = 0;
		while (str[len] != 0)
		{
			len++;
		}

		ret.resize(len);

		for (size_t i = 0; i < len; i++)
		{
			ret[i] = static_cast<T>(str[i]);
		}

		return ret;
	}

	template <typename T>
	static std::basic_string<T> Join(const std::vector<std::basic_string<T>>& elems, std::basic_string<T> separator)
	{
		std::basic_string<T> ret;

		for (size_t i = 0; i < elems.size(); i++)
		{
			ret += elems[i];

			if (i != elems.size() - 1)
			{
				ret += separator;
			}
		}

		return ret;
	}
};

class PathHelper
{
private:
	template <typename T>
	static std::basic_string<T> Normalize(const std::vector<std::basic_string<T>>& paths)
	{
		std::vector<std::basic_string<T>> elems;

		for (size_t i = 0; i < paths.size(); i++)
		{
			if (paths[i] == StringHelper::To<T>(".."))
			{
				if (elems.size() > 0 && elems.back() != StringHelper::To<T>(".."))
				{
					elems.pop_back();
				}
				else
				{
					elems.push_back(StringHelper::To<T>(".."));
				}
			}
			else
			{
				elems.push_back(paths[i]);
			}
		}

		return StringHelper::Join(elems, StringHelper::To<T>("/"));
	}

	template <typename STRING, typename T>
	static STRING GetDirectoryName(const STRING& path)
	{
		if (path.size() == 0)
		{
			return STRING();
		}

		int last_separator = -1;
		for (size_t i = 0; i < path.size(); i++)
		{
			if (IsSeparator(path[i]))
			{
				last_separator = static_cast<int>(i);
			}
		}

		if (last_separator >= 0)
		{
			return path.substr(0, last_separator + 1);
		}
		return STRING();
	}

	template <typename STRING, typename T>
	static STRING GetFilenameWithoutExt(const STRING& path)
	{
		if (path.size() == 0)
		{
			return STRING();
		}

		int last_separator = -1;
		for (size_t i = 0; i < path.size(); i++)
		{
			if (IsSeparator(path[i]))
			{
				last_separator = static_cast<int>(i);
			}
		}
		last_separator++;

		int last_dot = -1;
		for (size_t i = last_separator; i < path.size(); i++)
		{
			if (path[i] == static_cast<T>('.'))
			{
				last_dot = static_cast<int>(i);
			}
		}

		if (last_dot >= 0)
		{
			return path.substr(last_separator, last_dot - last_separator);
		}
		return path.substr(last_separator, path.size() - last_separator);
	}

	template <typename STRING, typename T>
	static STRING Combine(const STRING& path1, const STRING& path2)
	{
		STRING ret = path1;
		if (ret.size() > 0 && !IsSeparator(ret.back()))
		{
			ret += static_cast<T>('/');
		}
		ret += path2;

		for (size_t i = 0; i < ret.size(); i++)
		{
			if (ret[i] == static_cast<T>('\\'))
			{
				ret[i] = static_cast<T>('/');
			}
		}

		return ret;
	}

public:
	template <typename T>
	static bool IsSeparator(T s)
	{
		return s == static_cast<T>('/') || s == static_cast<T>('\\');
	}

	template <typename T>
	static std::basic_string<T> Normalize(const std::basic_string<T>& path)
	{
		if (path.size() == 0)
			return path;

		auto paths =
			StringHelper::Split(StringHelper::Replace(path, StringHelper::To<T>("\\"), StringHelper::To<T>("/")), static_cast<T>('/'));

		return Normalize(paths);
	}

	template <typename T>
	static std::basic_string<T> Relative(const std::basic_string<T>& targetPath, const std::basic_string<T>& basePath)
	{
		if (basePath.size() == 0 || targetPath.size() == 0)
		{
			return targetPath;
		}

		auto targetPaths = StringHelper::Split(StringHelper::Replace(targetPath, StringHelper::To<T>("\\"), StringHelper::To<T>("/")),
											   static_cast<T>('/'));
		auto basePaths =
			StringHelper::Split(StringHelper::Replace(basePath, StringHelper::To<T>("\\"), StringHelper::To<T>("/")), static_cast<T>('/'));

		if (*(basePath.end() - 1) != static_cast<T>('/') && *(basePath.end() - 1) != static_cast<T>('\\'))
		{
			basePaths.pop_back();
		}

		int32_t offset = 0;
		while (targetPaths.size() > offset && basePaths.size() > offset)
		{
			if (targetPaths[offset] == basePaths[offset])
			{
				offset++;
			}
			else
			{
				break;
			}
		}

		std::basic_string<T> ret;

		for (size_t i = offset; i < basePaths.size(); i++)
		{
			ret += StringHelper::To<T>("../");
		}

		for (size_t i = offset; i < targetPaths.size(); i++)
		{
			ret += targetPaths[i];

			if (i != targetPaths.size() - 1)
			{
				ret += StringHelper::To<T>("/");
			}
		}

		return ret;
	}

	template <typename T>
	static std::basic_string<T> Absolute(const std::basic_string<T>& targetPath, const std::basic_string<T>& basePath)
	{
		if (targetPath.size() == 0)
			return std::basic_string<T>();

		if (basePath.size() == 0)
			return targetPath;

		auto targetPaths = StringHelper::Split(StringHelper::Replace(targetPath, StringHelper::To<T>("\\"), StringHelper::To<T>("/")),
											   static_cast<T>('/'));
		auto basePaths =
			StringHelper::Split(StringHelper::Replace(basePath, StringHelper::To<T>("\\"), StringHelper::To<T>("/")), static_cast<T>('/'));

		if (*(basePath.end() - 1) != static_cast<T>('/') && *(basePath.end() - 1) != static_cast<T>('\\'))
		{
			basePaths.pop_back();
		}

		std::copy(targetPaths.begin(), targetPaths.end(), std::back_inserter(basePaths));

		return Normalize(basePaths);
	}

	template <typename T>
	static std::basic_string<T> GetDirectoryName(const std::basic_string<T>& path)
	{
		return GetDirectoryName<std::basic_string<T>, T>(path);
	}

	template <typename T, int N>
	static FixedSizeString<T, N> GetDirectoryName(const FixedSizeString<T, N>& path)
	{
		return GetDirectoryName<FixedSizeString<T, N>, T>(path);
	}

	template <typename T>
	static std::basic_string<T> GetFilenameWithoutExt(const std::basic_string<T>& path)
	{
		return GetFilenameWithoutExt<std::basic_string<T>, T>(path);
	}

	template <typename T, int N>
	static FixedSizeString<T, N> GetFilenameWithoutExt(const FixedSizeString<T, N>& path)
	{
		return GetFilenameWithoutExt<FixedSizeString<T, N>, T>(path);
	}

	template <typename T>
	static std::basic_string<T> Combine(const std::basic_string<T>& path1, const std::basic_string<T>& path2)
	{
		return Combine<std::basic_string<T>, T>(path1, path2);
	}

	template <typename T, int N>
	static FixedSizeString<T, N> Combine(const FixedSizeString<T, N>& path1, const FixedSizeString<T, N>& path2)
	{
		return Combine<FixedSizeString<T, N>, T>(path1, path2);
	}
};

} // namespace Effekseer

#endif
