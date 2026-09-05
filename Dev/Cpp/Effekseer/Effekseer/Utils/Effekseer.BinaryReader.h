
#ifndef __EFFEKSEER_BINARY_READER_H__
#define __EFFEKSEER_BINARY_READER_H__

#include "../Effekseer.Base.h"

#include <limits>

namespace Effekseer
{

enum class BinaryReaderStatus
{
	Reading,
	Complete,
	Failed,
};

/**
	@brief	utility for reading binary data
*/
template <bool IsValidationEnabled>
class BinaryReader
{
private:
	const uint8_t* data_ = nullptr;
	size_t size_ = 0;
	size_t offset = 0;
	BinaryReaderStatus status_ = BinaryReaderStatus::Reading;

	bool Fail()
	{
		status_ = BinaryReaderStatus::Failed;
		return false;
	}

public:
	void MarkFailed()
	{
		Fail();
	}

	// Inspect a serialized field without advancing past its enclosing block.
	// Validate the destination as well: block lengths come from the input file.
	template <typename T>
	bool Peek(T* value, size_t length)
	{
		if (length > sizeof(T) || !CanRead(length) || (length > 0 && (data_ == nullptr || value == nullptr)))
		{
			return Fail();
		}
		if (length > 0)
		{
			memcpy(value, data_ + offset, length);
		}
		return true;
	}

	BinaryReader(const uint8_t* data, size_t size)
	{
		data_ = data;
		size_ = size;
	}

	bool CanRead(size_t length) const
	{
		return status_ != BinaryReaderStatus::Failed && offset <= size_ && length <= size_ - offset;
	}

	bool CanReadElements(int32_t count, size_t elementSize) const
	{
		if (count < 0 || (elementSize > 0 && static_cast<size_t>(count) > std::numeric_limits<size_t>::max() / elementSize))
		{
			return false;
		}

		return CanRead(static_cast<size_t>(count) * elementSize);
	}

	bool ReadBytes(void* value, size_t length)
	{
		if (status_ == BinaryReaderStatus::Failed ||
			length > std::numeric_limits<size_t>::max() - offset ||
			(length > 0 && (data_ == nullptr || value == nullptr)) ||
			(IsValidationEnabled && !CanRead(length)))
		{
			return Fail();
		}

		if (length > 0)
		{
			memcpy(value, data_ + offset, length);
		}
		offset += length;
		return true;
	}

	template <typename T>
	bool Read(T& value)
	{
		return ReadBytes(&value, sizeof(T));
	}

	/**
@brief	read with validation
*/
	template <typename T>
	bool Read(T& value, const T& min_, const T& max_)
	{
		if (!Read(value))
		{
			return false;
		}

		if (IsValidationEnabled)
		{
			if (value < min_ || value > max_)
			{
				return Fail();
			}
		}

		return true;
	}

	/**
		@brief	read with validation
	*/
	template <typename T, typename U>
	bool Read(T& value, const U& min_, const U& max_)
	{
		if (!Read(value))
		{
			return false;
		}

		if (IsValidationEnabled)
		{
			if (static_cast<U>(value) < min_ || static_cast<U>(value) > max_)
			{
				return Fail();
			}
		}

		return true;
	}

	template <typename T>
	bool Read(T* value, int32_t count)
	{
		if (count < 0 || (sizeof(T) > 0 && static_cast<size_t>(count) > std::numeric_limits<size_t>::max() / sizeof(T)))
		{
			return Fail();
		}

		return ReadBytes(value, sizeof(T) * static_cast<size_t>(count));
	}

	template <typename T, typename _Alloc>
	bool Read(std::vector<T, _Alloc>& value, int32_t count)
	{
		if (count < 0 || (sizeof(T) > 0 && static_cast<size_t>(count) > std::numeric_limits<size_t>::max() / sizeof(T)))
		{
			return Fail();
		}

		const auto length = sizeof(T) * static_cast<size_t>(count);
		if (IsValidationEnabled && !CanRead(length))
		{
			return Fail();
		}

		value.resize(count);
		return ReadBytes(value.data(), length);
	}

	BinaryReaderStatus GetStatus() const
	{
		if (status_ == BinaryReaderStatus::Failed)
			return status_;

		return offset == size_ ? BinaryReaderStatus::Complete : BinaryReaderStatus::Reading;
	}

	bool Skip(size_t length)
	{
		if (IsValidationEnabled && !CanRead(length))
		{
			return Fail();
		}
		if (length > std::numeric_limits<size_t>::max() - offset)
		{
			return Fail();
		}

		offset += length;
		return true;
	}

	bool SetOffset(size_t newOffset)
	{
		if (IsValidationEnabled && newOffset > size_)
		{
			return Fail();
		}
		offset = newOffset;
		return true;
	}

	void AddOffset(size_t length)
	{
		Skip(length);
	}

	size_t GetOffset() const
	{
		return offset;
	}

	size_t GetRemainingSize() const
	{
		return CanRead(0) ? size_ - offset : 0;
	}

	const uint8_t* GetCurrentData() const
	{
		return CanRead(0) && data_ != nullptr ? data_ + offset : nullptr;
	}
};

} // namespace Effekseer

#endif // __EFFEKSEER_READER_H__
