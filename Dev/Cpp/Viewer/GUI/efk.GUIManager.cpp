#define NOMINMAX

#ifdef _WIN32
#define GLEW_STATIC 1
#include <GL/glew.h>
#endif

#ifdef _WIN32
#include "../Graphics/Platform/DX11/efk.GraphicsDX11.h"
#include <EffekseerRendererDX11/EffekseerRenderer/GraphicsDevice.h>
#endif

#include <EffekseerRendererGL/EffekseerRenderer/GraphicsDevice.h>

#include "../EffekseerRendererCommon/EffekseerRenderer.PngTextureLoader.h"
#include "../EffekseerTool/EffekseerTool.Renderer.h"

#include "NodeFrameTimeline.h"
#include "efk.GUIManager.h"

#include "../EditorCommon/GUI/JapaneseFont.h"

#include "../3rdParty/imgui_addon/fcurve/fcurve.h"

#include "../3rdParty/Boxer/boxer.h"

#include "../dll.h"

#include <GUI/Misc.h>

#ifdef __linux__
#include <gtk/gtk.h>
#endif
namespace ImGui
{
static ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)
{
	return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
}

static ImVec2 operator*(const ImVec2& lhs, const float& rhs)
{
	return ImVec2(lhs.x * rhs, lhs.y * rhs);
}

bool ImageButton_(ImTextureID user_texture_id,
				  const ImVec2& size,
				  const ImVec2& uv0,
				  const ImVec2& uv1,
				  int frame_padding,
				  const ImVec4& bg_col,
				  const ImVec4& tint_col)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;

	// Default to using texture ID as ID. User can still push string/integer prefixes.
	// We could hash the size/uv to create a unique ID but that would prevent the user from animating UV.
	PushID((void*)user_texture_id);
	const ImGuiID id = window->GetID("#image");
	PopID();

	const ImVec2 padding = (frame_padding >= 0) ? ImVec2((float)frame_padding, (float)frame_padding) : style.FramePadding;
	const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size + padding * 2);
	const ImRect image_bb(window->DC.CursorPos + padding, window->DC.CursorPos + padding + size);
	ItemSize(bb);
	if (!ItemAdd(bb, id))
		return false;

	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held, ImGuiButtonFlags_PressedOnClick);

	// Render
	const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
	RenderNavHighlight(bb, id);
	RenderFrame(bb.Min, bb.Max, col, true, ImClamp((float)ImMin(padding.x, padding.y), 0.0f, style.FrameRounding));
	if (bg_col.w > 0.0f)
		window->DrawList->AddRectFilled(image_bb.Min, image_bb.Max, GetColorU32(bg_col));
	window->DrawList->AddImage(user_texture_id, image_bb.Min, image_bb.Max, uv0, uv1, GetColorU32(tint_col));

	return pressed;
}

#define IM_F32_TO_INT8_UNBOUND(_VAL) ((int)((_VAL)*255.0f + ((_VAL) >= 0 ? 0.5f : -0.5f))) // Unsaturated, for display purpose

/**
	@brief support raw HSV
*/
#define RAW_HSV 1

bool ColorEdit4_(const char* label, float col[4], ImGuiColorEditFlags flags)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const float square_sz = GetFrameHeight();
	const float w_extra = (flags & ImGuiColorEditFlags_NoSmallPreview) ? 0.0f : (square_sz + style.ItemInnerSpacing.x);
	const float w_items_all = CalcItemWidth() - w_extra;
	const char* label_display_end = FindRenderedTextEnd(label);

	BeginGroup();
	PushID(label);

	// If we're not showing any slider there's no point in doing any HSV conversions
	const ImGuiColorEditFlags flags_untouched = flags;
	if (flags & ImGuiColorEditFlags_NoInputs)
		flags = (flags & (~ImGuiColorEditFlags__DisplayMask)) | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoOptions;

#ifdef RAW_HSV
	const bool alpha = (flags & ImGuiColorEditFlags_NoAlpha) == 0;
	float col_rgb_[4] = {col[0], col[1], col[2], alpha ? col[3] : 1.0f};
	if (flags & ImGuiColorEditFlags_HSV)
		ColorConvertHSVtoRGB(col_rgb_[0], col_rgb_[1], col_rgb_[2], col_rgb_[0], col_rgb_[1], col_rgb_[2]);

	// Context menu: display and modify options (before defaults are applied)
	if (!(flags & ImGuiColorEditFlags_NoOptions))
		ColorEditOptionsPopup(col_rgb_, flags);
#else
	// Context menu: display and modify options (before defaults are applied)
	if (!(flags & ImGuiColorEditFlags_NoOptions))
		ColorEditOptionsPopup(col, flags);
#endif

	// Read stored options
	if (!(flags & ImGuiColorEditFlags__DisplayMask))
		flags |= (g.ColorEditOptions & ImGuiColorEditFlags__DisplayMask);
	if (!(flags & ImGuiColorEditFlags__DataTypeMask))
		flags |= (g.ColorEditOptions & ImGuiColorEditFlags__DataTypeMask);
	if (!(flags & ImGuiColorEditFlags__PickerMask))
		flags |= (g.ColorEditOptions & ImGuiColorEditFlags__PickerMask);
	flags |=
		(g.ColorEditOptions & ~(ImGuiColorEditFlags__DisplayMask | ImGuiColorEditFlags__DataTypeMask | ImGuiColorEditFlags__PickerMask));

#ifndef RAW_HSV
	const bool alpha = (flags & ImGuiColorEditFlags_NoAlpha) == 0;
#endif
	const bool hdr = (flags & ImGuiColorEditFlags_HDR) != 0;
	const int components = alpha ? 4 : 3;

	// Convert to the formats we need
	float f[4] = {col[0], col[1], col[2], alpha ? col[3] : 1.0f};
#ifndef RAW_HSV
	if (flags & ImGuiColorEditFlags_HSV)
		ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);
#endif
	int i[4] = {IM_F32_TO_INT8_UNBOUND(f[0]), IM_F32_TO_INT8_UNBOUND(f[1]), IM_F32_TO_INT8_UNBOUND(f[2]), IM_F32_TO_INT8_UNBOUND(f[3])};

	bool value_changed = false;
	bool value_changed_as_float = false;

	if ((flags & (ImGuiColorEditFlags_RGB | ImGuiColorEditFlags_HSV)) != 0 && (flags & ImGuiColorEditFlags_NoInputs) == 0)
	{
		// RGB/HSV 0..255 Sliders
		const float w_item_one =
			ImMax(1.0f, (float)(int)((w_items_all - (style.ItemInnerSpacing.x) * (components - 1)) / (float)components));
		const float w_item_last = ImMax(1.0f, (float)(int)(w_items_all - (w_item_one + style.ItemInnerSpacing.x) * (components - 1)));

		const bool hide_prefix = (w_item_one <= CalcTextSize((flags & ImGuiColorEditFlags_Float) ? "M:0.000" : "M:000").x);
		const char* ids[4] = {"##X", "##Y", "##Z", "##W"};
		const char* fmt_table_int[3][4] = {
			{"%3d", "%3d", "%3d", "%3d"},		  // Short display
			{"R:%3d", "G:%3d", "B:%3d", "A:%3d"}, // Long display for RGBA
			{"H:%3d", "S:%3d", "V:%3d", "A:%3d"}  // Long display for HSVA
		};
		const char* fmt_table_float[3][4] = {
			{"%0.3f", "%0.3f", "%0.3f", "%0.3f"},		  // Short display
			{"R:%0.3f", "G:%0.3f", "B:%0.3f", "A:%0.3f"}, // Long display for RGBA
			{"H:%0.3f", "S:%0.3f", "V:%0.3f", "A:%0.3f"}  // Long display for HSVA
		};
		const int fmt_idx = hide_prefix ? 0 : (flags & ImGuiColorEditFlags_HSV) ? 2 : 1;

		PushItemWidth(w_item_one);
		for (int n = 0; n < components; n++)
		{
			if (n > 0)
				SameLine(0, style.ItemInnerSpacing.x);
			if (n + 1 == components)
				PushItemWidth(w_item_last);
			if (flags & ImGuiColorEditFlags_Float)
				value_changed = value_changed_as_float =
					value_changed | DragFloat(ids[n], &f[n], 1.0f / 255.0f, 0.0f, hdr ? 0.0f : 1.0f, fmt_table_float[fmt_idx][n]);
			else
				value_changed |= DragInt(ids[n], &i[n], 1.0f, 0, hdr ? 0 : 255, fmt_table_int[fmt_idx][n]);
			if (!(flags & ImGuiColorEditFlags_NoOptions))
				OpenPopupOnItemClick("context");
		}
		PopItemWidth();
		PopItemWidth();
	}
	else if ((flags & ImGuiColorEditFlags_HEX) != 0 && (flags & ImGuiColorEditFlags_NoInputs) == 0)
	{
		// RGB Hexadecimal Input
		char buf[64];
		if (alpha)
			ImFormatString(buf,
						   IM_ARRAYSIZE(buf),
						   "#%02X%02X%02X%02X",
						   ImClamp(i[0], 0, 255),
						   ImClamp(i[1], 0, 255),
						   ImClamp(i[2], 0, 255),
						   ImClamp(i[3], 0, 255));
		else
			ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X", ImClamp(i[0], 0, 255), ImClamp(i[1], 0, 255), ImClamp(i[2], 0, 255));
		PushItemWidth(w_items_all);
		if (InputText("##Text", buf, IM_ARRAYSIZE(buf), ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase))
		{
			value_changed = true;
			char* p = buf;
			while (*p == '#' || ImCharIsBlankA(*p))
				p++;
			i[0] = i[1] = i[2] = i[3] = 0;
			if (alpha)
				sscanf(p,
					   "%02X%02X%02X%02X",
					   (unsigned int*)&i[0],
					   (unsigned int*)&i[1],
					   (unsigned int*)&i[2],
					   (unsigned int*)&i[3]); // Treat at unsigned (%X is unsigned)
			else
				sscanf(p, "%02X%02X%02X", (unsigned int*)&i[0], (unsigned int*)&i[1], (unsigned int*)&i[2]);
		}
		if (!(flags & ImGuiColorEditFlags_NoOptions))
			OpenPopupOnItemClick("context");
		PopItemWidth();
	}

	ImGuiWindow* picker_active_window = nullptr;
	if (!(flags & ImGuiColorEditFlags_NoSmallPreview))
	{
		if (!(flags & ImGuiColorEditFlags_NoInputs))
			SameLine(0, style.ItemInnerSpacing.x);

#ifdef RAW_HSV
		const ImVec4 col_v4(col_rgb_[0], col_rgb_[1], col_rgb_[2], alpha ? col_rgb_[3] : 1.0f);
#else
		const ImVec4 col_v4(col[0], col[1], col[2], alpha ? col[3] : 1.0f);
#endif
		if (ColorButton("##ColorButton", col_v4, flags))
		{
			if (!(flags & ImGuiColorEditFlags_NoPicker))
			{
				// Store current color and open a picker
				g.ColorPickerRef = col_v4;
				OpenPopup("picker");
				SetNextWindowPos(window->DC.LastItemRect.GetBL() + ImVec2(-1, style.ItemSpacing.y));
			}
		}
		if (!(flags & ImGuiColorEditFlags_NoOptions))
			OpenPopupOnItemClick("context");

		if (BeginPopup("picker"))
		{
			picker_active_window = g.CurrentWindow;
			if (label != label_display_end)
			{
				TextUnformatted(label, label_display_end);
				Separator();
			}
			ImGuiColorEditFlags picker_flags_to_forward = ImGuiColorEditFlags__DataTypeMask | ImGuiColorEditFlags__PickerMask |
														  ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha |
														  ImGuiColorEditFlags_AlphaBar;
			ImGuiColorEditFlags picker_flags = (flags_untouched & picker_flags_to_forward) | ImGuiColorEditFlags__DisplayMask |
											   ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf;
			PushItemWidth(square_sz * 12.0f); // Use 256 + bar sizes?

#ifdef RAW_HSV
			auto picker_changed = ColorPicker4("##picker", col_rgb_, picker_flags, &g.ColorPickerRef.x);
			value_changed |= picker_changed;

			if (picker_changed)
			{
				float col_raw[4] = {col_rgb_[0], col_rgb_[1], col_rgb_[2], col_rgb_[3]};

				if (flags & ImGuiColorEditFlags_HSV)
					ColorConvertRGBtoHSV(col_raw[0], col_raw[1], col_raw[2], col_raw[0], col_raw[1], col_raw[2]);

				f[0] = col_raw[0];
				f[1] = col_raw[1];
				f[2] = col_raw[2];
				f[3] = col_raw[3];

				col[0] = f[0];
				col[1] = f[1];
				col[2] = f[2];
				if (alpha)
					col[3] = f[3];
			}
#else
			value_changed |= ColorPicker4("##picker", col, picker_flags, &g.ColorPickerRef.x);
#endif
			PopItemWidth();
			EndPopup();
		}
	}

	if (label != label_display_end && !(flags & ImGuiColorEditFlags_NoLabel))
	{
		SameLine(0, style.ItemInnerSpacing.x);
		TextUnformatted(label, label_display_end);
	}

	// Convert back
	if (picker_active_window == nullptr)
	{
		if (!value_changed_as_float)
			for (int n = 0; n < 4; n++)
				f[n] = i[n] / 255.0f;

#ifdef RAW_HSV
		if (value_changed)
		{
			col[0] = f[0];
			col[1] = f[1];
			col[2] = f[2];
			if (alpha)
				col[3] = f[3];

			col_rgb_[0] = col[0];
			col_rgb_[1] = col[1];
			col_rgb_[2] = col[2];
			col_rgb_[3] = col[3];

			if (flags & ImGuiColorEditFlags_HSV)
				ColorConvertHSVtoRGB(col_rgb_[0], col_rgb_[1], col_rgb_[2], col_rgb_[0], col_rgb_[1], col_rgb_[2]);
		}
#else
		if (flags & ImGuiColorEditFlags_HSV)
			ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);
		if (value_changed)
		{
			col[0] = f[0];
			col[1] = f[1];
			col[2] = f[2];
			if (alpha)
				col[3] = f[3];
		}
#endif
	}

	PopID();
	EndGroup();

	// Drag and Drop Target
	// NB: The flag test is merely an optional micro-optimization, BeginDragDropTarget() does the same test.
	if ((window->DC.LastItemStatusFlags & ImGuiItemStatusFlags_HoveredRect) && !(flags & ImGuiColorEditFlags_NoDragDrop) &&
		BeginDragDropTarget())
	{
#ifdef RAW_HSV
		if (const ImGuiPayload* payload = AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
		{
			memcpy((float*)col_rgb_, payload->Data, sizeof(float) * 3);

			col[0] = col_rgb_[0];
			col[1] = col_rgb_[1];
			col[2] = col_rgb_[2];

			if (flags & ImGuiColorEditFlags_HSV)
				ColorConvertRGBtoHSV(col[0], col[1], col[2], col[0], col[1], col[2]);

			value_changed = true;
		}
		if (const ImGuiPayload* payload = AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
		{
			memcpy((float*)col_rgb_, payload->Data, sizeof(float) * components);

			col[0] = col_rgb_[0];
			col[1] = col_rgb_[1];
			col[2] = col_rgb_[2];
			col[3] = col_rgb_[3];

			if (flags & ImGuiColorEditFlags_HSV)
				ColorConvertRGBtoHSV(col[0], col[1], col[2], col[0], col[1], col[2]);

			value_changed = true;
		}
#else
		if (const ImGuiPayload* payload = AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
		{
			memcpy((float*)col, payload->Data, sizeof(float) * 3);
			value_changed = true;
		}
		if (const ImGuiPayload* payload = AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
		{
			memcpy((float*)col, payload->Data, sizeof(float) * components);
			value_changed = true;
		}
#endif
		EndDragDropTarget();
	}

	// When picker is being actively used, use its active id so IsItemActive() will function on ColorEdit4().
	if (picker_active_window && g.ActiveId != 0 && g.ActiveIdWindow == picker_active_window)
		window->DC.LastItemId = g.ActiveId;

	if (value_changed)
		MarkItemEdited(window->DC.LastItemId);

	return value_changed;
}
} // namespace ImGui

namespace efk
{
void ResizeBicubic(uint32_t* dst,
				   int32_t dstWidth,
				   int32_t dstHeight,
				   int32_t dstStride,
				   const uint32_t* src,
				   int32_t srcWidth,
				   int32_t srcHeight,
				   int32_t srcStride)
{
	float wf = (float)srcWidth / dstWidth;
	float hf = (float)srcHeight / dstHeight;

	// bicubic weight function
	auto weight = [](float d) -> float {
		const float a = -1.0f;
		return d <= 1.0f ? ((a + 2.0f) * d * d * d) - ((a + 3.0f) * d * d) + 1
						 : d <= 2.0f ? (a * d * d * d) - (5.0f * a * d * d) + (8.0f * a * d) - (4.0f * a)
									 : 0.0f;
	};

	for (int32_t iy = 0; iy < dstHeight; iy++)
	{
		for (int32_t ix = 0; ix < dstWidth; ix++)
		{
			float wfx = wf * ix, wfy = hf * iy;
			int32_t x = (int32_t)wfx;
			int32_t y = (int32_t)wfy;
			float r = 0.0f, g = 0.0f, b = 0.0f, a = 0.0f;

			for (int32_t jy = y - 1; jy <= y + 2; jy++)
			{
				for (int32_t jx = x - 1; jx <= x + 2; jx++)
				{
					float w = weight(std::abs(wfx - jx)) * weight(std::abs(wfy - jy));
					if (w == 0.0f)
						continue;
					int32_t sx = (jx >= 0 && jx < srcWidth) ? jx : x;
					int32_t sy = (jy >= 0 && jy < srcHeight) ? jy : y;
					uint32_t sc = src[sx + sy * srcStride];
					uint32_t alpha = ((sc >> 24) & 0xff);
					r += ((sc >> 0) & 0xff) * w;
					g += ((sc >> 8) & 0xff) * w;
					b += ((sc >> 16) & 0xff) * w;
					a += alpha * w;
				}
			}

			dst[ix + iy * dstStride] = (std::max(0, std::min(255, (int32_t)r)) << 0) | (std::max(0, std::min(255, (int32_t)g)) << 8) |
									   (std::max(0, std::min(255, (int32_t)b)) << 16) | (std::max(0, std::min(255, (int32_t)a)) << 24);
		}
	}
}

size_t ConvertUTF16ToUTF8(char* dst, size_t dstSize, const char16_t* src)
{
	const uint16_t* wp = (const uint16_t*)src;
	uint8_t* cp = (uint8_t*)dst;

	if (dstSize == 0)
		return 0;

	dstSize -= 3;

	size_t count = 0;
	for (count = 0; count < dstSize;)
	{
		uint16_t wc = *wp++;
		if (wc == 0)
		{
			break;
		}

		if ((wc & ~0x7f) == 0)
		{
			*cp++ = wc & 0x7f;
			count += 1;
		}
		else if ((wc & ~0x7ff) == 0)
		{
			*cp++ = ((wc >> 6) & 0x1f) | 0xc0;
			*cp++ = ((wc)&0x3f) | 0x80;
			count += 2;
		}
		else
		{
			*cp++ = ((wc >> 12) & 0xf) | 0xe0;
			*cp++ = ((wc >> 6) & 0x3f) | 0x80;
			*cp++ = ((wc)&0x3f) | 0x80;
			count += 3;
		}
	}
	*cp = '\0';
	return count;
}

template <size_t SIZE>
struct utf8str
{
	enum
	{
		size = SIZE
	};
	char data[SIZE];
	utf8str(const char16_t* u16str)
	{
		ConvertUTF16ToUTF8(data, SIZE, u16str);
	}
	operator const char*() const
	{
		return data;
	}
};

// http://hasenpfote36.blogspot.jp/2016/09/stdcodecvt.html
static constexpr std::codecvt_mode mode = std::codecvt_mode::little_endian;

static std::string utf16_to_utf8(const std::u16string& s)
{
#if defined(_WIN32)
	std::wstring_convert<std::codecvt_utf8_utf16<std::uint16_t, 0x10ffff, mode>, std::uint16_t> conv;
	auto p = reinterpret_cast<const std::uint16_t*>(s.c_str());
	return conv.to_bytes(p, p + s.length());
#else
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t, 0x10ffff, mode>, char16_t> conv;
	return conv.to_bytes(s);
#endif
}

static std::u16string utf8_to_utf16(const std::string& s)
{

#if defined(_WIN32)
	std::wstring_convert<std::codecvt_utf8_utf16<std::uint16_t, 0x10ffff, mode>, std::uint16_t> conv;
	auto p = reinterpret_cast<const std::uint16_t*>(s.c_str());
	return std::u16string((const char16_t*)conv.from_bytes(s).c_str());
#else
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t, 0x10ffff, mode>, char16_t> conv;
	return conv.from_bytes(s);
#endif
}

static ImTextureID ToImTextureID(ImageResource* image)
{
	if (image != nullptr)
	{
		Effekseer::TextureRef texture = image->GetTexture();
		if (texture != nullptr)
		{
#ifdef _WIN32
			if (image->GetDeviceType() == DeviceType::DirectX11)
			{
				auto t = static_cast<EffekseerRendererDX11::Backend::Texture*>(texture->GetBackend().Get());
				return reinterpret_cast<ImTextureID>(t->GetSRV());
			}
#endif
			if (image->GetDeviceType() == DeviceType::OpenGL)
			{
				auto t = static_cast<EffekseerRendererGL::Backend::Texture*>(texture->GetBackend().Get());
				return reinterpret_cast<ImTextureID>(static_cast<size_t>(t->GetBuffer()));
			}
		}
	}
	return nullptr;
}

bool DragFloatN(const char* label,
				float* v,
				int components,
				float v_speed,
				float* v_min,
				float* v_max,
				const char* display_format1,
				const char* display_format2,
				const char* display_format3,
				float power)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	bool value_changed = false;
	ImGui::BeginGroup();
	ImGui::PushID(label);
	ImGui::PushMultiItemsWidths(components, ImGui::CalcItemWidth());

	const char* display_formats[] = {display_format1, display_format2, display_format3};

	for (int i = 0; i < components; i++)
	{
		ImGui::PushID(i);
		value_changed |= ImGui::DragFloat("##v", &v[i], v_speed, v_min[i], v_max[i], display_formats[i], power);
		ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
		ImGui::PopID();
		ImGui::PopItemWidth();
	}
	ImGui::PopID();

	ImGui::TextUnformatted(label, ImGui::FindRenderedTextEnd(label));
	ImGui::EndGroup();

	return value_changed;
}

bool DragIntN(const char* label,
			  int* v,
			  int components,
			  int v_speed,
			  int* v_min,
			  int* v_max,
			  const char* display_format1,
			  const char* display_format2,
			  const char* display_format3)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	bool value_changed = false;
	ImGui::BeginGroup();
	ImGui::PushID(label);
	ImGui::PushMultiItemsWidths(components, ImGui::CalcItemWidth());

	const char* display_formats[] = {display_format1, display_format2, display_format3};

	for (int i = 0; i < components; i++)
	{
		ImGui::PushID(i);
		value_changed |= ImGui::DragInt("##v", &v[i], v_speed, v_min[i], v_max[i], display_formats[i]);
		ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
		ImGui::PopID();
		ImGui::PopItemWidth();
	}
	ImGui::PopID();

	ImGui::TextUnformatted(label, ImGui::FindRenderedTextEnd(label));
	ImGui::EndGroup();

	return value_changed;
}

void GUIManager::MarkdownLinkCallback(ImGui::MarkdownLinkCallbackData data)
{

	std::string url(data.link, data.linkLength);
	auto url16 = utf8_to_utf16(url);

	auto self = reinterpret_cast<GUIManager*>(data.userData);

	self->callback->ClickLink(url16.c_str());
}

GUIManager::GUIManager()
{
}

GUIManager::~GUIManager()
{
}

bool GUIManager::Initialize(std::shared_ptr<Effekseer::MainWindow> mainWindow, efk::DeviceType deviceType)
{
#ifdef __linux__
	gtk_disable_setlocale();
#endif
	window = new efk::Window();

	this->deviceType = deviceType;

	if (!window->Initialize(mainWindow, deviceType))
	{
		ES_SAFE_DELETE(window);
		return false;
	}

	mainWindow_ = mainWindow;

	window->Resized = [this](int x, int y) -> void {
		if (this->callback != nullptr)
		{
			this->callback->Resized(x, y);
		}
	};

	window->Focused = [this]() -> void {
		if (this->callback != nullptr)
		{
			this->callback->Focused();
		}
	};

	window->Droped = [this](const char* path) -> void {
		if (this->callback != nullptr)
		{
			this->callback->SetPath(utf8_to_utf16(path).c_str());
			this->callback->Droped();
		}
	};

	window->Closing = [this]() -> bool {
		if (this->callback != nullptr)
		{
			return this->callback->Closing();
		}

		return true;
	};

	window->Iconify = [this](int f) -> void {
		if (this->callback != nullptr)
		{
			this->callback->Iconify(f);
		}
	};

	window->DpiChanged = [this](float scale) -> void {
		this->ResetGUIStyle();

		if (this->callback != nullptr)
		{
			this->callback->DpiChanged(scale);
		}
	};

	if (deviceType == DeviceType::OpenGL)
	{
		window->MakeCurrent();

#ifdef _WIN32
		glewInit();
#endif
	}

	return true;
}

void GUIManager::InitializeGUI(Native* native)
{
	ImGui::CreateContext();
	ImGui::GetCurrentContext()->PlatformLocaleDecimalPoint = *localeconv()->decimal_point;

	ImGuiIO& io = ImGui::GetIO();

	if (deviceType == DeviceType::OpenGL)
	{
		// It causes bugs on some mac pc
		// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

#if __APPLE__
		// GL 3.2 + GLSL 150
		const char* glsl_version = "#version 150";
#else
		// GL 3.0 + GLSL 130
		const char* glsl_version = "#version 130";
#endif
		ImGui_ImplGlfw_InitForOpenGL(window->GetGLFWWindows(), true);
		ImGui_ImplOpenGL3_Init(glsl_version);
	}
#ifdef _WIN32
	else if (deviceType == DeviceType::DirectX11)
	{
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui_ImplGlfw_InitForVulkan(window->GetGLFWWindows(), true);
		auto r = (EffekseerRendererDX11::Renderer*)native->GetRenderer().Get();
		ImGui_ImplDX11_Init(r->GetDevice(), r->GetContext());
	}
	else
	{
		assert(0);
	}
#endif
	// Enable keyboard navication
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui::StyleColorsDark();
	ResetGUIStyle();

	markdownConfig_.userData = this;
	markdownConfig_.linkCallback = GUIManager::MarkdownLinkCallback;
}

void GUIManager::ResetGUIStyle()
{
	ImGuiStyle& style = ImGui::GetStyle();

	style = ImGuiStyle();
	style.ChildRounding = 3.f;
	style.GrabRounding = 3.f;
	style.WindowRounding = 3.f;
	style.ScrollbarRounding = 3.f;
	style.FrameRounding = 3.f;
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.ScaleAllSizes(mainWindow_->GetDPIScale());
	// mono tone

	for (int32_t i = 0; i < ImGuiCol_COUNT; i++)
	{
		auto v = (style.Colors[i].x + style.Colors[i].y + style.Colors[i].z) / 3.0f;
		style.Colors[i].x = v;
		style.Colors[i].y = v;
		style.Colors[i].z = v;
	}

	style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.9f);
	style.Colors[ImGuiCol_TitleBgActive] = style.Colors[ImGuiCol_TitleBg];
	style.Colors[ImGuiCol_Tab] = style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
}

void GUIManager::SetTitle(const char16_t* title)
{
	window->SetTitle(title);
}

void GUIManager::SetWindowIcon(const char16_t* iconPath)
{
	window->SetWindowIcon(iconPath);
}

Vec2 GUIManager::GetSize() const
{
	return window->GetSize();
}

void GUIManager::SetSize(int32_t width, int32_t height)
{
	window->SetSize(width, height);
}

void GUIManager::Terminate()
{
	if (deviceType == DeviceType::OpenGL)
	{
		ImGui_ImplOpenGL3_Shutdown();
	}
#ifdef _WIN32
	else if (deviceType == DeviceType::DirectX11)
	{
		ImGui_ImplDX11_Shutdown();
	}
	else
	{
		assert(0);
	}
#endif

	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	window->MakeNone();
	window->Terminate();
	ES_SAFE_DELETE(window);
}

bool GUIManager::DoEvents()
{
	if (window->DoEvents())
	{
		framerateController_.Update();
		return true;
	}

	return false;
}

float GUIManager::GetDeltaSecond()
{
	return framerateController_.GetDeltaSecond();
}

void GUIManager::Present()
{
	window->Present();
}

void GUIManager::Close()
{
	window->Close();
}

bool GUIManager::IsWindowMaximized()
{
	return window->IsWindowMaximized();
}

void GUIManager::SetWindowMaximized(bool maximized)
{
	window->SetWindowMaximized(maximized);
}

bool GUIManager::IsWindowMinimized()
{
	return window->IsWindowMinimized();
}

void GUIManager::SetWindowMinimized(bool minimized)
{
	window->SetWindowMinimized(minimized);
}

Vec2 GUIManager::GetMousePosition()
{
	return window->GetMousePosition();
}

int GUIManager::GetMouseButton(int32_t mouseButton)
{
	return window->GetMouseButton(mouseButton);
}

int GUIManager::GetMouseWheel()
{
	return ImGui::GetIO().MouseWheel;
}

void GUIManager::SetCallback(GUIManagerCallback* callback)
{
	this->callback = callback;
}

void GUIManager::InvalidateFont()
{
	if (deviceType == DeviceType::OpenGL)
	{
		ImGui_ImplOpenGL3_DestroyFontsTexture();
	}
#if _WIN32
	else if (deviceType == DeviceType::DirectX11)
	{
		ImGui_ImplDX11_InvalidateDeviceObjects();
	}
	else
	{
		assert(0);
	}
#endif
}

void GUIManager::ResetGUI()
{
	if (deviceType == DeviceType::OpenGL)
	{
		ImGui_ImplOpenGL3_NewFrame();
	}
#if _WIN32
	else if (deviceType == DeviceType::DirectX11)
	{
		ImGui_ImplDX11_NewFrame();
	}
	else
	{
		assert(0);
	}
#endif

	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void GUIManager::RenderGUI(bool isValid)
{
	ImGui::EndFrame();

	if (isValid)
	{
		if (deviceType == DeviceType::OpenGL)
		{
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}
#if _WIN32
		else if (deviceType == DeviceType::DirectX11)
		{
			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}
		else
		{
			assert(0);
		}
#endif
	}
	else
	{
		if (ImGui::GetDrawData() != nullptr)
		{
			ImGui::GetDrawData()->Clear();
		}
	}

	{
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();

			if (isValid)
			{
				ImGui::RenderPlatformWindowsDefault();
			}
		}

		if (deviceType == DeviceType::OpenGL)
		{
			glfwMakeContextCurrent(window->GetGLFWWindows());
		}
	}
}

void* GUIManager::GetNativeHandle()
{
	return window->GetNativeHandle();
}

const char16_t* GUIManager::GetClipboardText()
{
	auto ret = glfwGetClipboardString(window->GetGLFWWindows());
	if (ret == nullptr)
	{
		static std::u16string empty;
		return empty.c_str();
	}
	clipboard = utf8_to_utf16(ret);
	return clipboard.c_str();
}

void GUIManager::SetClipboardText(const char16_t* text)
{
	glfwSetClipboardString(window->GetGLFWWindows(), utf16_to_utf8(text).c_str());
}

bool GUIManager::Begin(const char16_t* name, bool* p_open)
{
	return ImGui::Begin(utf8str<256>(name), p_open);
}

void GUIManager::End()
{
	ImGui::End();
}

bool GUIManager::BeginChild(const char* str_id, const Vec2& size_arg, bool border, WindowFlags extra_flags)
{
	return ImGui::BeginChild(str_id, ImVec2(size_arg.X, size_arg.Y), border, (ImGuiWindowFlags)extra_flags);
}

void GUIManager::EndChild()
{
	ImGui::EndChild();
}

Vec2 GUIManager::GetWindowSize()
{
	auto v = ImGui::GetWindowSize();
	return Vec2(v.x, v.y);
}

Vec2 GUIManager::GetContentRegionAvail()
{
	auto v = ImGui::GetContentRegionAvail();
	return Vec2(v.x, v.y);
}

void GUIManager::SetNextWindowPos(const Vec2& pos, Cond cond, const Vec2& pivot)
{
	ImGui::SetNextWindowPos(ImVec2(pos.X, pos.Y), (ImGuiCond)cond, ImVec2(pivot.X, pivot.Y));
}

void GUIManager::SetNextWindowSize(float size_x, float size_y, Cond cond)
{
	ImVec2 size;
	size.x = size_x;
	size.y = size_y;

	ImGui::SetNextWindowSize(size, (int)cond);
}

void GUIManager::PushStyleColor(ImGuiColFlags idx, uint32_t col)
{
	ImGui::PushStyleColor((int32_t)idx, col);
}

void GUIManager::PopStyleColor(int count)
{
	ImGui::PopStyleColor(count);
}

void GUIManager::PushStyleVar(ImGuiStyleVarFlags idx, float val)
{
	ImGui::PushStyleVar(idx, val);
}

void GUIManager::PushStyleVar(ImGuiStyleVarFlags idx, const Vec2& val)
{
	ImVec2 val_;
	val_.x = val.X;
	val_.y = val.Y;
	ImGui::PushStyleVar(idx, val_);
}

void GUIManager::PopStyleVar(int count)
{
	ImGui::PopStyleVar(count);
}

void GUIManager::PushItemWidth(float item_width)
{
	ImGui::PushItemWidth(item_width);
}

void GUIManager::PopItemWidth()
{
	ImGui::PopItemWidth();
}

void GUIManager::Separator()
{
	ImGui::Separator();
}

void GUIManager::HiddenSeparator(float thicknessDraw, float thicknessItem)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return;

	ImGuiSeparatorFlags flags =
		(window->DC.LayoutType == ImGuiLayoutType_Horizontal) ? ImGuiSeparatorFlags_Vertical : ImGuiSeparatorFlags_Horizontal;
	flags |= ImGuiSeparatorFlags_SpanAllColumns;

	ImGuiContext& g = *GImGui;
	IM_ASSERT(
		ImIsPowerOfTwo(flags & (ImGuiSeparatorFlags_Horizontal | ImGuiSeparatorFlags_Vertical))); // Check that only 1 option is selected

	float thickness_draw = 1.0f;
	float thickness_layout = 0.0f;
	if (flags & ImGuiSeparatorFlags_Vertical)
	{
		// Vertical separator, for menu bars (use current line height). Not exposed because it is misleading and it doesn't have an
		// effect on regular layout.
		float y1 = window->DC.CursorPos.y;
		float y2 = window->DC.CursorPos.y + window->DC.CurrLineSize.y;
		const ImRect bb(ImVec2(window->DC.CursorPos.x, y1), ImVec2(window->DC.CursorPos.x + thickness_draw, y2));
		ImGui::ItemSize(ImVec2(thickness_layout, 0.0f));
		if (!ImGui::ItemAdd(bb, 0))
			return;

		// Draw
		window->DrawList->AddLine(ImVec2(bb.Min.x, bb.Min.y), ImVec2(bb.Min.x, bb.Max.y), ImGui::GetColorU32(ImGuiCol_Separator));
		if (g.LogEnabled)
			ImGui::LogText(" |");
	}
	else if (flags & ImGuiSeparatorFlags_Horizontal)
	{
		// Horizontal Separator
		float x1 = window->Pos.x;
		float x2 = window->Pos.x + window->Size.x;

		// FIXME-WORKRECT: old hack (#205) until we decide of consistent behavior with WorkRect/Indent and Separator
		if (g.GroupStack.Size > 0 && g.GroupStack.back().WindowID == window->ID)
			x1 += window->DC.Indent.x;

		ImGuiOldColumns* columns = (flags & ImGuiSeparatorFlags_SpanAllColumns) ? window->DC.CurrentColumns : NULL;
		if (columns)
			ImGui::PushColumnsBackground();

		// We don't provide our width to the layout so that it doesn't get feed back into AutoFit
		const ImRect bb(ImVec2(x1, window->DC.CursorPos.y), ImVec2(x2, window->DC.CursorPos.y + thicknessDraw));
		ImGui::ItemSize(ImVec2(0.0f, thicknessItem));
		const bool item_visible = ImGui::ItemAdd(bb, 0);
		if (item_visible)
		{
			// Draw
			window->DrawList->AddLine(bb.Min, ImVec2(bb.Max.x, bb.Min.y), 0);
			if (g.LogEnabled)
				ImGui::LogRenderedText(&bb.Min, "--------------------------------\n");
		}
		if (columns)
		{
			ImGui::PopColumnsBackground();
			columns->LineMinY = window->DC.CursorPos.y;
		}

		/*
		// Horizontal Separator
		float x1 = window->Pos.x;
		float x2 = window->Pos.x + window->Size.x;
		if (!window->DC.GroupStack.empty())
			x1 += window->DC.Indent.x;

		ImGuiColumns* columns = (flags & ImGuiSeparatorFlags_SpanAllColumns) ? window->DC.CurrentColumns : nullptr;
		if (columns)
			ImGui::PushColumnsBackground();

		// We don't provide our width to the layout so that it doesn't get feed back into AutoFit
		const ImRect bb(ImVec2(x1, window->DC.CursorPos.y), ImVec2(x2, window->DC.CursorPos.y + thicknessDraw));
		ImGui::ItemSize(ImVec2(0.0f, thicknessItem));
		if (!ImGui::ItemAdd(bb, 0))
		{
			if (columns)
				ImGui::PopColumnsBackground();
			return;
		}

		// Draw
		window->DrawList->AddLine(bb.Min, ImVec2(bb.Max.x, bb.Min.y), 0);
		if (g.LogEnabled)
			ImGui::LogRenderedText(&bb.Min, "--------------------------------");

		if (columns)
		{
			ImGui::PopColumnsBackground();
			columns->LineMinY = window->DC.CursorPos.y;
		}
		*/
	}
}

void GUIManager::Indent(float indent_w)
{
	ImGui::Indent(indent_w);
}

void GUIManager::Spacing()
{
	ImGui::Spacing();
}

void GUIManager::SameLine(float offset_from_start_x, float spacing)
{
	ImGui::SameLine(offset_from_start_x, spacing);
}

void GUIManager::BeginGroup()
{
	ImGui::BeginGroup();
}

void GUIManager::EndGroup()
{
	ImGui::EndGroup();
}

void GUIManager::SetCursorPosX(float x)
{
	ImGui::SetCursorPosX(x);
}

void GUIManager::SetCursorPosY(float y)
{
	ImGui::SetCursorPosY(y);
}

float GUIManager::GetCursorPosX()
{
	return ImGui::GetCursorPosX();
}

float GUIManager::GetCursorPosY()
{
	return ImGui::GetCursorPosY();
}

float GUIManager::GetTextLineHeight()
{
	return ImGui::GetTextLineHeight();
}

float GUIManager::GetTextLineHeightWithSpacing()
{
	return ImGui::GetTextLineHeightWithSpacing();
}

float GUIManager::GetFrameHeight()
{
	return ImGui::GetFrameHeight();
}

float GUIManager::GetFrameHeightWithSpacing()
{
	return ImGui::GetFrameHeightWithSpacing();
}

float GUIManager::GetDpiScale() const
{
	return mainWindow_->GetDPIScale();
}

void GUIManager::Columns(int count, const char* id, bool border)
{
	ImGui::Columns(count, id, border);
}

void GUIManager::NextColumn()
{
	ImGui::NextColumn();
}

float GUIManager::GetColumnWidth(int column_index)
{
	return ImGui::GetColumnWidth(column_index);
}

void GUIManager::SetColumnWidth(int column_index, float width)
{
	ImGui::SetColumnWidth(column_index, width);
}

float GUIManager::GetColumnOffset(int column_index)
{
	return ImGui::GetColumnOffset(column_index);
}

void GUIManager::SetColumnOffset(int column_index, float offset_x)
{
	ImGui::SetColumnOffset(column_index, offset_x);
}

void CallWithEscaped(const std::function<void(const char*)>& f, const char16_t* text)
{
	bool isPersentFound = false;
	for (size_t i = 0;; i++)
	{
		if (text[i] == 0)
		{
			break;
		}

		if (text[i] == u'%')
		{
			isPersentFound = true;
			break;
		}
	}

	if (isPersentFound)
	{
		// HACK
		std::u16string text_;
		for (size_t i = 0;; i++)
		{
			if (text[i] == 0)
			{
				break;
			}

			if (text[i] == u'%')
			{
				text_ += u"%%";
			}
			else
			{
				text_ += text[i];
			}
		}

		if (std::char_traits<char16_t>::length(text_.c_str()) < 1024)
		{
			f(utf8str<1024>(text_.c_str()));
		}
		else
		{
			f(utf16_to_utf8(text_).c_str());
		}
	}
	else
	{
		if (std::char_traits<char16_t>::length(text) < 1024)
		{
			f(utf8str<1024>(text));
		}
		else
		{
			f(utf16_to_utf8(text).c_str());
		}
	}
}

void GUIManager::Text(const char16_t* text)
{
	auto func = [](const char* c) -> void { ImGui::Text(c); };
	CallWithEscaped(func, text);
}

void GUIManager::TextWrapped(const char16_t* text)
{
	if (std::char_traits<char16_t>::length(text) < 1024)
	{
		ImGui::TextWrapped(utf8str<1024>(text));
	}
	else
	{
		ImGui::TextWrapped(utf16_to_utf8(text).c_str());
	}
}

Vec2 GUIManager::CalcTextSize(const char16_t* text)
{
	ImVec2 result;

	if (std::char_traits<char16_t>::length(text) < 1024)
	{
		result = ImGui::CalcTextSize(utf8str<1024>(text));
	}
	else
	{
		result = ImGui::CalcTextSize(utf16_to_utf8(text).c_str());
	}

	return {result.x, result.y};
}

bool GUIManager::Button(const char16_t* label, float size_x, float size_y)
{
	return ImGui::Button(utf8str<256>(label), ImVec2(size_x, size_y));
}

void GUIManager::Image(ImageResource* user_texture_id, float x, float y)
{
	ImGui::Image(ToImTextureID(user_texture_id), ImVec2(x, y));
}

void GUIManager::Image(void* user_texture_id, float x, float y)
{
	if (deviceType != DeviceType::OpenGL)
	{
		ImGui::Image((ImTextureID)user_texture_id, ImVec2(x, y), ImVec2(0, 0), ImVec2(1, 1));
	}
	else
	{
		ImGui::Image((ImTextureID)user_texture_id, ImVec2(x, y), ImVec2(0, 1), ImVec2(1, 0));
	}
}

bool GUIManager::ImageButton(ImageResource* user_texture_id, float x, float y)
{
	return ImGui::ImageButton_(
		ToImTextureID(user_texture_id), ImVec2(x, y), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1));
}

bool GUIManager::ImageButtonOriginal(ImageResource* user_texture_id, float x, float y)
{
	return ImGui::ImageButton(ToImTextureID(user_texture_id), ImVec2(x, y), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1));
}

bool GUIManager::Checkbox(const char16_t* label, bool* v)
{
	return ImGui::Checkbox(utf8str<256>(label), v);
}

bool GUIManager::RadioButton(const char16_t* label, bool active)
{
	return ImGui::RadioButton(utf8str<256>(label), active);
}

bool GUIManager::ToggleButton(const char16_t* label, bool* p_checked)
{
	utf8str<256> labelU8(label);

	const auto& style = ImGui::GetStyle();

	ImVec2 cursorPos = ImGui::GetCursorScreenPos();
	ImDrawList* drawList = ImGui::GetWindowDrawList();

	float frameHeight = ImGui::GetFrameHeight();
	float height = frameHeight * 0.8f;
	float width = height * 2.5f;
	float radius = height * 0.50f;
	cursorPos.y += (frameHeight - height) * 0.5f;

	ImGui::SetCursorScreenPos(cursorPos);
	ImGui::InvisibleButton(labelU8, ImVec2(width, height));
	if (ImGui::IsItemClicked())
		*p_checked = !*p_checked;

	float t = *p_checked ? 1.0f : 0.0f;

	ImGuiContext& g = *GImGui;
	float ANIM_SPEED = 0.08f;
	if (g.LastActiveId == g.CurrentWindow->GetID(labelU8)) // && g.LastActiveIdTimer < ANIM_SPEED)
	{
		float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
		t = *p_checked ? (t_anim) : (1.0f - t_anim);
	}

	ImU32 col_bg;
	if (ImGui::IsItemHovered())
		col_bg = ImGui::GetColorU32(ImLerp(style.Colors[ImGuiCol_ButtonHovered], ImVec4(0.56f, 0.83f, 0.26f, 1.0f), t));
	else
		col_bg = ImGui::GetColorU32(ImLerp(style.Colors[ImGuiCol_Button], ImVec4(0.46f, 0.73f, 0.20f, 1.0f), t));

	drawList->AddRectFilled(cursorPos, ImVec2(cursorPos.x + width, cursorPos.y + height), col_bg, height * 0.5f);

	float textHeight = ImGui::GetTextLineHeight();
	float textOffsetY = (height - textHeight) * 0.5f - 1.0f;
	if (*p_checked)
		drawList->AddText(ImVec2(cursorPos.x + textHeight / 2, cursorPos.y + textOffsetY), IM_COL32(255, 255, 255, 255), "ON");
	else
		drawList->AddText(ImVec2(cursorPos.x + width / 2 - textHeight / 8, cursorPos.y + textOffsetY), IM_COL32(230, 230, 230, 255), "OFF");

	drawList->AddCircleFilled(ImVec2(cursorPos.x + radius + t * (width - radius * 2.0f), cursorPos.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));

	return *p_checked;
}

bool GUIManager::InputInt(const char16_t* label, int* v, int step, int step_fast)
{
	return ImGui::InputInt(utf8str<256>(label), v, step, step_fast);
}

bool GUIManager::SliderInt(const char16_t* label, int* v, int v_min, int v_max)
{
	return ImGui::SliderInt(utf8str<256>(label), v, v_min, v_max);
}

void GUIManager::ProgressBar(float fraction, const Vec2& size)
{
	ImGui::ProgressBar(fraction, ImVec2(size.X, size.Y));
}

bool GUIManager::BeginCombo(const char16_t* label, const char16_t* preview_value, ComboFlags flags, ImageResource* user_texture_id)
{
	return ImGui::BeginCombo(utf8str<256>(label), utf8str<256>(preview_value), (int)flags /*, ToImTextureID(user_texture_id)*/);
}

void GUIManager::EndCombo()
{
	ImGui::EndCombo();
}

bool GUIManager::DragFloat(
	const char16_t* label, float* v, float v_speed, float v_min, float v_max, const char* display_format, float power)
{
	return ImGui::DragFloat(utf8str<256>(label), v, v_speed, v_min, v_max, display_format);
}

bool GUIManager::DragFloat2(
	const char16_t* label, float* v, float v_speed, float v_min, float v_max, const char* display_format, float power)
{
	return ImGui::DragFloat2(utf8str<256>(label), v, v_speed, v_min, v_max, display_format);
}

bool GUIManager::DragFloat3(
	const char16_t* label, float* v, float v_speed, float v_min, float v_max, const char* display_format, float power)
{
	return ImGui::DragFloat3(utf8str<256>(label), v, v_speed, v_min, v_max, display_format);
}

bool GUIManager::DragFloat4(
	const char16_t* label, float* v, float v_speed, float v_min, float v_max, const char* display_format, float power)
{
	return ImGui::DragFloat4(utf8str<256>(label), v, v_speed, v_min, v_max, display_format);
}

bool GUIManager::DragFloatRange2(const char16_t* label,
								 float* v_current_min,
								 float* v_current_max,
								 float v_speed,
								 float v_min,
								 float v_max,
								 const char* display_format,
								 const char* display_format_max,
								 float power)
{
	return ImGui::DragFloatRange2(
		utf8str<256>(label), v_current_min, v_current_max, v_speed, v_min, v_max, display_format, display_format_max);
}

bool GUIManager::DragInt(const char16_t* label, int* v, float v_speed, int v_min, int v_max, const char* display_format)
{
	return ImGui::DragInt(utf8str<256>(label), v, v_speed, v_min, v_max, display_format);
}

bool GUIManager::DragInt2(const char16_t* label, int* v, float v_speed, int v_min, int v_max, const char* display_format)
{
	return ImGui::DragInt2(utf8str<256>(label), v, v_speed, v_min, v_max, display_format);
}

bool GUIManager::DragInt3(const char16_t* label, int* v, float v_speed, int v_min, int v_max, const char* display_format)
{
	return ImGui::DragInt3(utf8str<256>(label), v, v_speed, v_min, v_max, display_format);
}

bool GUIManager::DragInt4(const char16_t* label, int* v, float v_speed, int v_min, int v_max, const char* display_format)
{
	return ImGui::DragInt4(utf8str<256>(label), v, v_speed, v_min, v_max, display_format);
}

bool GUIManager::DragIntRange2(const char16_t* label,
							   int* v_current_min,
							   int* v_current_max,
							   float v_speed,
							   int v_min,
							   int v_max,
							   const char* display_format,
							   const char* display_format_max)
{
	return ImGui::DragIntRange2(
		utf8str<256>(label), v_current_min, v_current_max, v_speed, v_min, v_max, display_format, display_format_max);
}

bool GUIManager::DragFloat1EfkEx(
	const char16_t* label, float* v, float v_speed, float v_min, float v_max, const char16_t* display_format1, float power)
{
	float v_min_[3];
	float v_max_[3];
	v_min_[0] = v_min;
	v_max_[0] = v_max;

	return DragFloatN(utf8str<256>(label), v, 1, v_speed, v_min_, v_max_, utf8str<256>(display_format1), nullptr, nullptr, power);
}

bool GUIManager::DragFloat2EfkEx(const char16_t* label,
								 float* v,
								 float v_speed,
								 float v_min1,
								 float v_max1,
								 float v_min2,
								 float v_max2,
								 const char16_t* display_format1,
								 const char16_t* display_format2,
								 float power)
{
	float v_min_[3];
	float v_max_[3];
	v_min_[0] = v_min1;
	v_max_[0] = v_max1;
	v_min_[1] = v_min2;
	v_max_[1] = v_max2;

	return DragFloatN(
		utf8str<256>(label), v, 2, v_speed, v_min_, v_max_, utf8str<256>(display_format1), utf8str<256>(display_format2), nullptr, power);
}

bool GUIManager::DragFloat3EfkEx(const char16_t* label,
								 float* v,
								 float v_speed,
								 float v_min1,
								 float v_max1,
								 float v_min2,
								 float v_max2,
								 float v_min3,
								 float v_max3,
								 const char16_t* display_format1,
								 const char16_t* display_format2,
								 const char16_t* display_format3,
								 float power)
{
	float v_min_[3];
	float v_max_[3];
	v_min_[0] = v_min1;
	v_max_[0] = v_max1;
	v_min_[1] = v_min2;
	v_max_[1] = v_max2;
	v_min_[2] = v_min3;
	v_max_[2] = v_max3;

	return DragFloatN(utf8str<256>(label),
					  v,
					  3,
					  v_speed,
					  v_min_,
					  v_max_,
					  utf8str<256>(display_format1),
					  utf8str<256>(display_format2),
					  utf8str<256>(display_format3),
					  power);
}

bool GUIManager::DragInt2EfkEx(const char16_t* label,
							   int* v,
							   int v_speed,
							   int v_min1,
							   int v_max1,
							   int v_min2,
							   int v_max2,
							   const char16_t* display_format1,
							   const char16_t* display_format2)
{
	int v_min_[3];
	int v_max_[3];
	v_min_[0] = v_min1;
	v_max_[0] = v_max1;
	v_min_[1] = v_min2;
	v_max_[1] = v_max2;

	return DragIntN(
		utf8str<256>(label), v, 2, v_speed, v_min_, v_max_, utf8str<256>(display_format1), utf8str<256>(display_format2), nullptr);
}

static std::u16string inputTextResult;

bool GUIManager::InputText(const char16_t* label, const char16_t* text, InputTextFlags flags)
{
	auto text_ = utf8str<1024>(text);

	char buf[260];
	memcpy(buf, text_.data, std::min((int32_t)text_.size, 250));
	buf[std::min((int32_t)text_.size, 250)] = 0;

	auto ret = ImGui::InputText(utf8str<256>(label), buf, 260, (ImGuiWindowFlags)flags);

	inputTextResult = utf8_to_utf16(buf);

	return ret;
}

bool GUIManager::InputTextMultiline(const char16_t* label, const char16_t* text)
{
	auto text_ = utf8str<1024>(text);

	char buf[512];
	memcpy(buf, text_.data, std::min((int32_t)text_.size, 500));
	buf[std::min((int32_t)text_.size, 500)] = 0;

	auto ret = ImGui::InputTextMultiline(utf8str<512>(label), buf, 512);

	inputTextResult = utf8_to_utf16(buf);

	return ret;
}

const char16_t* GUIManager::GetInputTextResult()
{
	return inputTextResult.c_str();
}

bool GUIManager::ColorEdit4(const char16_t* label, float* col, ColorEditFlags flags)
{
	return ImGui::ColorEdit4_(utf8str<256>(label), col, (int)flags);
}

bool GUIManager::CollapsingHeader(const char16_t* label, TreeNodeFlags flags)
{
	return ImGui::CollapsingHeader(utf8str<256>(label), (int)flags);
}

bool GUIManager::CollapsingHeaderWithToggle(const char16_t* label, TreeNodeFlags flags, const char16_t* toggle_id, bool* p_checked)
{
	ImVec2 cursorPosBefore = ImGui::GetCursorScreenPos();
	ImVec2 region = ImGui::GetContentRegionAvail();

	bool result = CollapsingHeader(label, (TreeNodeFlags)((uint32_t)flags | ImGuiTreeNodeFlags_AllowItemOverlap));

	ImVec2 cursorPosAfter = ImGui::GetCursorScreenPos();

	float toggleHeight = ImGui::GetFrameHeight() * 0.8f;
	float toggleWidth = toggleHeight * 2.5f;
	ImGui::SetCursorScreenPos(ImVec2(cursorPosBefore.x + region.x - toggleWidth, cursorPosBefore.y));

	ToggleButton(toggle_id, p_checked);

	ImGui::SetCursorScreenPos(cursorPosAfter);

	return result;
}

bool GUIManager::TreeNode(const char16_t* label)
{
	return ImGui::TreeNode(utf8str<256>(label));
}

bool GUIManager::TreeNodeEx(const char16_t* label, TreeNodeFlags flags)
{
	return ImGui::TreeNodeEx(utf8str<256>(label), (int)flags);
}

void GUIManager::TreePop()
{
	ImGui::TreePop();
}

void GUIManager::SetNextTreeNodeOpen(bool is_open, Cond cond)
{
	ImGui::SetNextTreeNodeOpen(is_open, (ImGuiCond)cond);
}

bool GUIManager::TreeNodeEx(const char16_t* label, bool* v, TreeNodeFlags flags)
{
	return *v = ImGui::TreeNodeEx(utf8str<256>(label), (ImGuiTreeNodeFlags)flags);
}

bool GUIManager::Selectable(const char16_t* label, bool selected, SelectableFlags flags)
{
	return ImGui::Selectable(utf8str<256>(label), selected, (int)flags, ImVec2(0, 0));
}

void GUIManager::SetTooltip(const char16_t* text)
{
	auto func = [](const char* c) -> void { ImGui::SetTooltip(c); };
	CallWithEscaped(func, text);
}

void GUIManager::BeginTooltip()
{
	ImGui::BeginTooltip();
}

void GUIManager::EndTooltip()
{
	ImGui::EndTooltip();
}

bool GUIManager::BeginMainMenuBar()
{
	return ImGui::BeginMainMenuBar();
}

void GUIManager::EndMainMenuBar()
{
	ImGui::EndMainMenuBar();
}

bool GUIManager::BeginMenuBar()
{
	return ImGui::BeginMenuBar();
}

void GUIManager::EndMenuBar()
{
	return ImGui::EndMenuBar();
}

bool GUIManager::BeginMenu(const char16_t* label, bool enabled)
{
	return ImGui::BeginMenu(utf8str<256>(label), enabled);
}

void GUIManager::EndMenu()
{
	ImGui::EndMenu();
}

bool GUIManager::MenuItem(const char16_t* label, const char* shortcut, bool selected, bool enabled)
{
	return ImGui::MenuItem(utf8str<256>(label), shortcut, selected, enabled);
}

bool GUIManager::MenuItem(const char16_t* label, const char* shortcut, bool* p_selected, bool enabled)
{
	return ImGui::MenuItem(utf8str<256>(label), shortcut, p_selected, enabled);
}

void GUIManager::OpenPopup(const char* str_id)
{
	ImGui::OpenPopup(str_id);
}

bool GUIManager::BeginPopup(const char* str_id, WindowFlags extra_flags)
{
	return ImGui::BeginPopup(str_id, (int)extra_flags);
}

bool GUIManager::BeginPopupModal(const char16_t* name, bool* p_open, WindowFlags extra_flags)
{
	return ImGui::BeginPopupModal(utf8str<256>(name), p_open, (int)extra_flags);
}

bool GUIManager::BeginPopupContextItem(const char* str_id, int mouse_button)
{
	return ImGui::BeginPopupContextItem(str_id, mouse_button);
}

void GUIManager::EndPopup()
{
	ImGui::EndPopup();
}

bool GUIManager::IsPopupOpen(const char* str_id)
{
	return ImGui::IsPopupOpen(str_id);
}

void GUIManager::CloseCurrentPopup()
{
	ImGui::CloseCurrentPopup();
}

void GUIManager::SetItemDefaultFocus()
{
	ImGui::SetItemDefaultFocus();
}

void GUIManager::SetKeyboardFocusHere(int offset)
{
	ImGui::SetKeyboardFocusHere(offset);
}

void GUIManager::ClearAllFonts()
{
	ImGuiIO& io = ImGui::GetIO();

	io.Fonts->Clear();
}

void GUIManager::AddFontFromFileTTF(const char16_t* fontFilepath, const char16_t* commonCharacterTablePath, const char16_t* characterTableName, float size_pixels)
{
	Effekseer::Editor::AddFontFromFileTTF(utf8str<280>(fontFilepath), utf8str<280>(commonCharacterTablePath), utf8str<280>(characterTableName), size_pixels, mainWindow_->GetDPIScale());
}

void GUIManager::AddFontFromAtlasImage(const char16_t* filename, uint16_t baseCode, int sizeX, int sizeY, int countX, int countY)
{
	ImGuiIO& io = ImGui::GetIO();

	Effekseer::DefaultFileInterface fi;
	std::unique_ptr<Effekseer::FileReader> reader(fi.OpenRead(filename));
	if (!reader)
	{
		return;
	}

	std::vector<uint8_t> buffer(reader->GetLength());
	reader->Read(&buffer[0], buffer.size());
	reader.reset();

	EffekseerRenderer::PngTextureLoader pngloader;
	if (!pngloader.Load(&buffer[0], (int32_t)buffer.size(), false))
	{
		return;
	}

	auto imagePixels = pngloader.GetData();
	int32_t imageWidth = pngloader.GetWidth();
	int32_t imageHeight = pngloader.GetHeight();

	ImFontConfig config;
	config.MergeMode = true;
	ImFont* font = io.Fonts->AddFontDefault(&config);

	int glyphSizeX = font->FontSize;
	int glyphSizeY = font->FontSize;
	float offsetX = 2 * GetDpiScale();

	std::vector<int> rectIDs;
	for (int32_t i = 0; i < countX * countY; i++)
	{
		rectIDs.push_back(io.Fonts->AddCustomRectFontGlyph(font, (ImWchar)baseCode + i, glyphSizeX, glyphSizeY, glyphSizeX + offsetX));
	}

	io.Fonts->Build();

	uint8_t* texturePixels = nullptr;
	int textureWidth = 0, textureHeight = 0;
	io.Fonts->GetTexDataAsRGBA32(&texturePixels, &textureWidth, &textureHeight);

	for (size_t i = 0; i < rectIDs.size(); i++)
	{
		if (auto* rect = io.Fonts->GetCustomRectByIndex(rectIDs[i]))
		{
			ImU32* rectPixels = (ImU32*)texturePixels + rect->Y * textureWidth + rect->X;
			const ImU32* atlasPixels = (const ImU32*)&imagePixels[0] + (sizeY * (i / countX) * imageWidth) + (sizeX * (i % countX));

			// for (int posY = 0; posY < sizeY; posY++)
			//{
			//	// copy a line
			//	memcpy(rectPixels + posY * textureWidth, atlasPixels + posY * imageWidth, sizeX * sizeof(ImU32));
			//}
			ResizeBicubic(rectPixels, glyphSizeX, glyphSizeY, textureWidth, atlasPixels, sizeX, sizeY, imageWidth);
		}
	}
}

bool GUIManager::BeginChildFrame(uint32_t id, const Vec2& size, WindowFlags flags)
{
	return ImGui::BeginChildFrame(id, ImVec2(size.X, size.Y), (int32_t)flags);
}

void GUIManager::EndChildFrame()
{
	ImGui::EndChildFrame();
}

int GUIManager::GetKeyIndex(Key key)
{
	return ImGui::GetKeyIndex((ImGuiKey)key);
}

bool GUIManager::IsKeyDown(int user_key_index)
{
	return ImGui::IsKeyDown(user_key_index);
}

bool GUIManager::IsKeyPressed(int user_key_index)
{
	return ImGui::IsKeyPressed(user_key_index);
}

bool GUIManager::IsKeyReleased(int user_key_index)
{
	return ImGui::IsKeyReleased(user_key_index);
}

bool GUIManager::IsShiftKeyDown()
{
	return ImGui::GetIO().KeyShift;
}

bool GUIManager::IsCtrlKeyDown()
{
	return ImGui::GetIO().KeyCtrl;
}

bool GUIManager::IsAltKeyDown()
{
	return ImGui::GetIO().KeyAlt;
}

bool GUIManager::IsMouseDown(int button)
{
	return ImGui::IsMouseDown(button);
}

bool GUIManager::IsMouseClicked(int button, bool repeat)
{
	return ImGui::IsMouseClicked(button, repeat);
}

bool GUIManager::IsMouseReleased(int button)
{
	return ImGui::IsMouseReleased(button);
}

bool GUIManager::IsMouseDoubleClicked(int button)
{
	return ImGui::IsMouseDoubleClicked(button);
}

bool GUIManager::IsItemHovered()
{
	return ImGui::IsItemHovered();
}

bool GUIManager::IsItemActive()
{
	return ImGui::IsItemActive();
}

bool GUIManager::IsItemFocused()
{
	return ImGui::IsItemFocused();
}

bool GUIManager::IsItemClicked(int mouse_button)
{
	return ImGui::IsItemClicked(mouse_button);
}

bool GUIManager::IsItemEdited()
{
	return ImGui::IsItemEdited();
}

bool GUIManager::IsItemActivated()
{
	return ImGui::IsItemActivated();
}

bool GUIManager::IsItemDeactivated()
{
	return ImGui::IsItemDeactivated();
}

bool GUIManager::IsItemDeactivatedAfterEdit()
{
	return ImGui::IsItemDeactivatedAfterEdit();
}

bool GUIManager::IsAnyItemActive()
{
	return ImGui::IsAnyItemActive();
}

bool GUIManager::IsWindowHovered()
{
	return ImGui::IsWindowHovered();
}

bool GUIManager::IsWindowFocused()
{
	return ImGui::IsWindowFocused();
}

bool GUIManager::IsAnyWindowHovered()
{
	return ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);
}

bool GUIManager::IsAnyWindowFocused()
{
	return ImGui::IsWindowFocused(ImGuiHoveredFlags_AnyWindow);
}

MouseCursor GUIManager::GetMouseCursor()
{
	return (MouseCursor)ImGui::GetMouseCursor();
}

float GUIManager::GetHoveredIDTimer()
{
	return ImGui::GetCurrentContext()->HoveredIdTimer;
}

void GUIManager::DrawLineBackground(float height, uint32_t col)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	auto cursorPos = ImGui::GetCursorPos();

	cursorPos.x = window->Pos.x;
	cursorPos.y = window->DC.CursorPos.y;
	ImVec2 size;
	size.x = window->Size.x;
	size.y = height;

	window->DrawList->AddRectFilled(cursorPos, ImVec2(cursorPos.x + size.x, cursorPos.y + size.y), col);
}

bool GUIManager::BeginFullscreen(const char16_t* label)
{
	ImGuiWindow* mainMenu = ImGui::FindWindowByName("##MainMenuBar");
	float offsetY = (mainMenu) ? (mainMenu->Size.y) : (ImGui::GetTextLineHeightWithSpacing() + 1 * GetDpiScale());

	ImVec2 windowSize;
	windowSize.x = ImGui::GetIO().DisplaySize.x;
	windowSize.y = ImGui::GetIO().DisplaySize.y - offsetY;

	ImGui::SetNextWindowSize(windowSize);

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		auto pos = ImGui::GetMainViewport()->Pos;
		pos.y += offsetY;
		ImGui::SetNextWindowPos(pos);
	}
	else
	{
		ImGui::SetNextWindowPos(ImVec2(0, offsetY));
	}

	const ImGuiWindowFlags flags = (ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize |
									ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(1.0f, 1.0f));
	const bool visible = ImGui::Begin(utf8str<256>(label), nullptr, flags);
	ImGui::PopStyleVar(2);

	imguiWindowID = ImGui::GetID(utf8str<256>(label));
	ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_NoCloseButton;
	ImGui::DockSpace(imguiWindowID, ImVec2(0.0f, 0.0f), dockFlags);

	// if (!ImGui::DockBuilderGetNode(imguiWindowID))
	//{
	//	SetDefaultDockLayout();
	//}

	return visible;
}

bool GUIManager::BeginDock(const char16_t* label, const char16_t* tabLabel, bool* p_open, bool allowClose, WindowFlags extra_flags)
{
	if (!allowClose)
	{
		p_open = nullptr;
	}

	utf8str<256> utf8Label(label);
	utf8str<256> utf8TabLabel(tabLabel);

	bool result = ImGui::Begin(utf8Label, p_open, (ImGuiWindowFlags)extra_flags);

	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window && !window->DockTabLabel[0])
	{
		strcpy(window->DockTabLabel, utf8TabLabel);
	}

	return result;
}

void GUIManager::EndDock()
{
	ImGui::End();
}

uint32_t GUIManager::BeginDockLayout()
{
	ImGui::DockBuilderRemoveNode(imguiWindowID);
	ImGui::DockBuilderAddNode(imguiWindowID, ImGuiDockNodeFlags_None);
	ImGui::DockBuilderSetNodeSize(imguiWindowID, ImGui::GetMainViewport()->Size);
	return imguiWindowID;
}

void GUIManager::EndDockLayout()
{
	ImGui::DockBuilderFinish(imguiWindowID);
}

void GUIManager::DockSplitNode(uint32_t nodeId, DockSplitDir dir, float sizeRatio, uint32_t* outId1, uint32_t* outId2)
{
	ImGui::DockBuilderSplitNode(nodeId, (ImGuiDir)dir, sizeRatio, outId1, outId2);
}

void GUIManager::DockSetNodeFlags(uint32_t nodeId, DockNodeFlags flags)
{
	auto node = ImGui::DockBuilderGetNode(nodeId);
	if (node == nullptr)
	{
		return;
	}

	node->LocalFlags |= ((uint32_t)flags & (uint32_t)DockNodeFlags::NoTabBar) ? ImGuiDockNodeFlags_NoTabBar : 0;
	node->LocalFlags |= ((uint32_t)flags & (uint32_t)DockNodeFlags::HiddenTabBar) ? ImGuiDockNodeFlags_HiddenTabBar : 0;
	node->LocalFlags |= ((uint32_t)flags & (uint32_t)DockNodeFlags::NoWindowMenuButton) ? ImGuiDockNodeFlags_NoWindowMenuButton : 0;
	node->LocalFlags |= ((uint32_t)flags & (uint32_t)DockNodeFlags::NoCloseButton) ? ImGuiDockNodeFlags_NoCloseButton : 0;
	node->LocalFlags |= ((uint32_t)flags & (uint32_t)DockNodeFlags::NoDocking) ? ImGuiDockNodeFlags_NoDocking : 0;
}

void GUIManager::DockSetWindow(uint32_t nodeId, const char* windowName)
{
	ImGui::DockBuilderDockWindow(windowName, nodeId);
}

bool GUIManager::IsDockFocused()
{
	return ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);
}

bool GUIManager::IsDockVisibled()
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	return (window && (window->DockTabIsVisible || !window->DockIsActive));
}

bool GUIManager::IsDockWindowed()
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	return (window && !window->DockIsActive);
}

void GUIManager::SetDockFocus(const char16_t* label)
{
	utf8str<256> utf8label(label);
	ImGui::SetWindowFocus(utf8label);
}

bool GUIManager::IsHoveredOnFCurve(float* keys, float* values, float* leftHandleKeys, float* leftHandleValues, float* rightHandleKeys, float* rightHandleValues, int* interporations, FCurveEdgeType startEdge, FCurveEdgeType endEdge, uint32_t col, int count)
{
	return ImGui::IsHoveredOnFCurve(
		keys,
		values,
		leftHandleKeys,
		leftHandleValues,
		rightHandleKeys,
		rightHandleValues,
		reinterpret_cast<ImGui::ImFCurveInterporationType*>(interporations),
		static_cast<ImGui::ImFCurveEdgeType>(startEdge),
		static_cast<ImGui::ImFCurveEdgeType>(endEdge),
		static_cast<ImU32>(col),
		count,
		nullptr);
}

bool GUIManager::IsHoveredOnFCurvePoint(const float* keys, const float* values, int count, int* hovered)
{
	return ImGui::IsHoveredOnFCurvePoint(keys, values, count, hovered);
}

bool GUIManager::IsFCurvePanning()
{
	return ImGui::IsFCurvePanning();
}

Vec2 GUIManager::GetCurrentFCurveFieldPosition()
{
	const auto pos = ImGui::GetCurrentFCurveFieldPosition();
	return Vec2{pos.x, pos.y};
}

bool GUIManager::BeginFCurve(int id, const Vec2& size, float current, const Vec2& scale, const Vec2& min_kv, const Vec2& max_kv)
{
	return ImGui::BeginFCurve(id, ImVec2(size.X, size.Y), current, ImVec2(scale.X, scale.Y), ImVec2(min_kv.X, min_kv.Y), ImVec2(max_kv.X, max_kv.Y));
}

void GUIManager::EndFCurve()
{
	ImGui::EndFCurve();
}

bool GUIManager::FCurve(int fcurve_id,
						float* keys,
						float* values,
						float* leftHandleKeys,
						float* leftHandleValues,
						float* rightHandleKeys,
						float* rightHandleValues,
						int* interporations,
						FCurveEdgeType startEdge,
						FCurveEdgeType endEdge,
						uint8_t* kv_selected,
						int count,
						float defaultValue,
						bool isLocked,
						bool canControl,
						uint32_t col,
						bool selected,
						float v_min,
						float v_max,
						int* newCount,
						bool* newSelected,
						float* movedX,
						float* movedY,
						int* changedType)
{
	return ImGui::FCurve(fcurve_id,
						 keys,
						 values,
						 leftHandleKeys,
						 leftHandleValues,
						 rightHandleKeys,
						 rightHandleValues,
						 (ImGui::ImFCurveInterporationType*)interporations,
						 (ImGui::ImFCurveEdgeType)startEdge,
						 (ImGui::ImFCurveEdgeType)endEdge,
						 (bool*)kv_selected,
						 count,
						 defaultValue,
						 isLocked,
						 canControl,
						 col,
						 selected,
						 v_min,
						 v_max,
						 newCount,
						 newSelected,
						 movedX,
						 movedY,
						 changedType);
}

bool GUIManager::StartSelectingAreaFCurve()
{
	return ImGui::StartSelectingAreaFCurve();
}

bool GUIManager::AddFCurvePoint(const Vec2& v,
								float* keys,
								float* values,
								float* leftHandleKeys,
								float* leftHandleValues,
								float* rightHandleKeys,
								float* rightHandleValues,
								int* interporations,
								uint8_t* kv_selected,
								int count,
								int* newCount)
{
	return ImGui::AddFCurvePoint(ImVec2{v.X, v.Y},
								 keys,
								 values,
								 leftHandleKeys,
								 leftHandleValues,
								 rightHandleKeys,
								 rightHandleValues,
								 (ImGui::ImFCurveInterporationType*)interporations,
								 (bool*)kv_selected,
								 count,
								 newCount);
}

bool GUIManager::RemoveFCurvePoint(Vec2 v,
								   float* keys,
								   float* values,
								   float* leftHandleKeys,
								   float* leftHandleValues,
								   float* rightHandleKeys,
								   float* rightHandleValues,
								   int* interporations,
								   uint8_t* kv_selected,
								   int count,
								   int* newCount)
{
	return ImGui::RemoveFCurvePoint(ImVec2{v.X, v.Y},
									keys,
									values,
									leftHandleKeys,
									leftHandleValues,
									rightHandleKeys,
									rightHandleValues,
									(ImGui::ImFCurveInterporationType*)interporations,
									(bool*)kv_selected,
									count,
									newCount);
}

bool GUIManager::BeginDragDropSource()
{
	return ImGui::BeginDragDropSource();
}

bool GUIManager::SetDragDropPayload(const char* type, uint8_t* data, int size)
{
	return ImGui::SetDragDropPayload(type, data, size);
}

void GUIManager::EndDragDropSource()
{
	ImGui::EndDragDropSource();
}

bool GUIManager::BeginDragDropTarget()
{
	return ImGui::BeginDragDropTarget();
}

bool GUIManager::AcceptDragDropPayload(const char* type, uint8_t* data_output, int data_output_size, int* size)
{
	auto pyload = ImGui::AcceptDragDropPayload(type);
	if (pyload == nullptr)
	{
		*size = 0;
		return false;
	}

	auto max_size = std::min(data_output_size, pyload->DataSize);
	memcpy(data_output, pyload->Data, max_size);
	*size = max_size;

	return true;
}

void GUIManager::EndDragDropTarget()
{
	ImGui::EndDragDropTarget();
}

DialogSelection GUIManager::show(const char16_t* message, const char16_t* title, DialogStyle style, DialogButtons buttons)
{
	return (DialogSelection)boxer::show(utf8str<256>(message), utf8str<256>(title), (boxer::Style)style, (boxer::Buttons)buttons);
}

bool GUIManager::IsMacOSX()
{
#if __APPLE__
	return true;
#else
	return false;
#endif
}

void GUIManager::SetIniFilename(const char16_t* filename)
{
	static std::string filename_ = std::string(utf8str<256>(filename));
	ImGui::GetIO().IniFilename = filename_.c_str();
}

void GUIManager::Markdown(const char16_t* text)
{
	utf8str<2048> textUtf8(text);

	::ImGui::Markdown(textUtf8, strlen(textUtf8), markdownConfig_);
}

bool GUIManager::BeginNodeFrameTimeline()
{
	return NodeFrameTimeline::BeginNodeFrameTimeline();
}

void GUIManager::TimelineNode(const char16_t* title, int frameStart, int frameEnd)
{
	utf8str<256> utf8Label(title);
	NodeFrameTimeline::TimelineNode(utf8Label, frameStart, frameEnd);
}

void GUIManager::EndNodeFrameTimeline(int* frameMin, int* frameMax, int* currentFrame, int* selectedEntry, int* firstFrame)
{
	NodeFrameTimeline::EndNodeFrameTimeline(frameMin, frameMax, currentFrame, selectedEntry, firstFrame);
}

} // namespace efk
