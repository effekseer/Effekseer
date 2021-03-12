#include "boxer.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <codecvt>
#include <locale>
#include <string>

namespace boxer {
	// http://hasenpfote36.blogspot.jp/2016/09/stdcodecvt.html
	static constexpr std::codecvt_mode mode = std::codecvt_mode::little_endian;

	static std::u16string utf8_to_utf16(const std::string& s)
	{

#if defined(_MSC_VER)
		std::wstring_convert<std::codecvt_utf8_utf16<std::uint16_t, 0x10ffff, mode>, std::uint16_t> conv;
		auto p = reinterpret_cast<const std::uint16_t*>(s.c_str());
		return std::u16string((const char16_t*)conv.from_bytes(s).c_str());
#else
		std::wstring_convert<std::codecvt_utf8_utf16<char16_t, 0x10ffff, mode>, char16_t> conv;
		return conv.from_bytes(s);
#endif
	}

namespace {

UINT getIcon(Style style) {
   switch (style) {
      case Style::Info:
         return MB_ICONINFORMATION;
      case Style::Warning:
         return MB_ICONWARNING;
      case Style::Error:
         return MB_ICONERROR;
      case Style::Question:
         return MB_ICONQUESTION;
      default:
         return MB_ICONINFORMATION;
   }
}

UINT getButtons(Buttons buttons) {
   switch (buttons) {
      case Buttons::OK:
      case Buttons::Quit: // There is no 'Quit' button on Windows :(
         return MB_OK;
      case Buttons::OKCancel:
         return MB_OKCANCEL;
      case Buttons::YesNo:
         return MB_YESNO;
      default:
         return MB_OK;
   }
}

Selection getSelection(int response, Buttons buttons) {
   switch (response) {
      case IDOK:
         return buttons == Buttons::Quit ? Selection::Quit : Selection::OK;
      case IDCANCEL:
         return Selection::Cancel;
      case IDYES:
         return Selection::Yes;
      case IDNO:
         return Selection::No;
      default:
         return Selection::None;
   }
}

} // namespace

Selection show(const char *message, const char *title, Style style, Buttons buttons) {
   UINT flags = MB_TASKMODAL;

   flags |= getIcon(style);
   flags |= getButtons(buttons);

   return getSelection(MessageBoxW(nullptr, (wchar_t*)utf8_to_utf16(message).c_str(), (wchar_t*)utf8_to_utf16(title).c_str(), flags), buttons);
}

} // namespace boxer
