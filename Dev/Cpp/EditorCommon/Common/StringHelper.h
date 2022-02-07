#pragma once

#include <array>
#include <string>
#include <vector>

namespace Effekseer
{
namespace Tool
{

class StringHelper
{
public:
	/**
		@brief    Convert UTF16 into UTF8
		@param    dst    a pointer to destination buffer
	    @param    dst_size    a length of destination buffer
	    @param    src            a source buffer
		@param    src_size    a length of source buffer
		@return    length except 0
	*/
	static int32_t ConvertUtf16ToUtf8(char* dst, int32_t dst_size, const char16_t* src, int32_t src_size = -1)
	{
		int32_t cnt = 0;
		int32_t src_pos = 0;

		char* cp = dst;

		if (dst_size == 0)
			return 0;

		dst_size -= 3;

		for (cnt = 0; cnt < dst_size;)
		{
			char16_t wc = src[src_pos];
			src_pos += 1;

			if (wc == 0)
			{
				break;
			}
			if ((wc & ~0x7f) == 0)
			{
				*cp++ = wc & 0x7f;
				cnt += 1;
			}
			else if ((wc & ~0x7ff) == 0)
			{
				*cp++ = ((wc >> 6) & 0x1f) | 0xc0;
				*cp++ = ((wc)&0x3f) | 0x80;
				cnt += 2;
			}
			else
			{
				*cp++ = ((wc >> 12) & 0xf) | 0xe0;
				*cp++ = ((wc >> 6) & 0x3f) | 0x80;
				*cp++ = ((wc)&0x3f) | 0x80;
				cnt += 3;
			}

			if (src_size > 0 && src_size <= src_pos)
			{
				break;
			}
		}
		*cp = '\0';
		return cnt;
	}

	/**
		@brief    Convert UTF8 into UTF16
		@param    dst    a pointer to destination buffer
		@param    dst_size    a length of destination buffer
		@param    src            a source buffer
		@param    src_size    a length of source buffer
		@return    length except 0
	*/
	static int32_t ConvertUtf8ToUtf16(char16_t* dst, int32_t dst_size, const char* src, int32_t src_size = -1)
	{
		int32_t i, code = 0;
		int32_t src_pos = 0;
		int8_t c0, c1, c2 = 0;
		int8_t* srci = reinterpret_cast<int8_t*>(const_cast<char*>(src));
		int8_t* srci0 = srci;
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
			// convert UTF8 to UTF16
			code = (uint8_t)c0 >> 4;
			if (code <= 7)
			{
				// 8bit character
				wc = c0;
			}
			else if (code >= 12 && code <= 13)
			{
				// 16bit  character
				c1 = *srci;
				srci++;
				wc = ((c0 & 0x1F) << 6) | (c1 & 0x3F);
			}
			else if (code == 14)
			{
				// 24bit character
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

			if (src_size > 0 && static_cast<int>(srci - srci0) >= src_size)
			{
				i++;
				break;
			}
		}
		dst[i] = 0;
		return i;
	}

	static std::u16string ConvertUtf8ToUtf16(const std::string& str)
	{
		const auto requiredBufferSize = str.size() * 2 + 1;

		if (requiredBufferSize < 1024)
		{
			std::array<char16_t, 1024> buffer;
			const auto size = ConvertUtf8ToUtf16(buffer.data(), static_cast<int32_t>(buffer.size()), str.data(), str.size());
			return std::u16string(buffer.data());
		}
		else
		{
			std::vector<char16_t> buffer;
			buffer.resize(requiredBufferSize);
			const auto size = ConvertUtf8ToUtf16(buffer.data(), static_cast<int32_t>(buffer.size()), str.data(), str.size());
			return std::u16string(buffer.data());
		}
	}

	static std::string ConvertUtf16ToUtf8(const std::u16string& str)
	{
		const auto requiredBufferSize = str.size() * 8 + 1;

		if (requiredBufferSize < 1024)
		{
			std::array<char, 1024> buffer;
			const auto size = ConvertUtf16ToUtf8(buffer.data(), static_cast<int32_t>(buffer.size()), str.data(), str.size());
			return std::string(buffer.data());
		}
		else
		{
			std::vector<char> buffer;
			buffer.resize(requiredBufferSize);
			const auto size = ConvertUtf16ToUtf8(buffer.data(), static_cast<int32_t>(buffer.size()), str.data(), str.size());
			return std::string(buffer.data());
		}
	}
};

} // namespace Tool
} // namespace Effekseer