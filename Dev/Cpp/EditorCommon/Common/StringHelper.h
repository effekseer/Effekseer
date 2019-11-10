#pragma once

#include <codecvt>
#include <locale>

namespace altseed {

// http://hasenpfote36.blogspot.jp/2016/09/stdcodecvt.html
static constexpr std::codecvt_mode mode = std::codecvt_mode::little_endian;

static std::string utf16_to_utf8(const std::u16string& s) {
#if defined(_MSC_VER)
    std::wstring_convert<std::codecvt_utf8_utf16<std::uint16_t, 0x10ffff, mode>, std::uint16_t> conv;
    auto p = reinterpret_cast<const std::uint16_t*>(s.c_str());
    return conv.to_bytes(p, p + s.length());
#else
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t, 0x10ffff, mode>, char16_t> conv;
    return conv.to_bytes(s);
#endif
}

}  // namespace altseed