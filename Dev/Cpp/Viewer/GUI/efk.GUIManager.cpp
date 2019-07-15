#define NOMINMAX

#ifdef _WIN32
#define GLEW_STATIC 1
#include <GL/glew.h>
#endif

#include "../EffekseerTool/EffekseerTool.Renderer.h"

#include "efk.GUIManager.h"

#include "Platform/efk.Language.h"

#include "efk.JapaneseFont.h"

#include "../3rdParty/imgui_addon/fcurve/fcurve.h"

#include "../3rdParty/Boxer/boxer.h"

#include "../dll.h"

namespace ImGui
{
	static ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)
	{
		return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
	}

	static ImVec2 operator * (const ImVec2& lhs, const float& rhs)
	{
		return ImVec2(lhs.x * rhs, lhs.y * rhs);
	}

	bool TreeNodeBehavior(ImGuiID id, ImGuiTreeNodeFlags flags, bool* v, ImTextureID user_texture, const char* label, const char* label_end)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const bool display_frame = (flags & ImGuiTreeNodeFlags_Framed) != 0;
		const ImVec2 padding =
			(display_frame || (flags & ImGuiTreeNodeFlags_FramePadding)) ? style.FramePadding : ImVec2(style.FramePadding.x, 0.0f);

		if (!label_end)
			label_end = FindRenderedTextEnd(label);
		const ImVec2 label_size = CalcTextSize(label, label_end, false);

		// We vertically grow up to current line height up the typical widget height.
		const float text_base_offset_y = ImMax(padding.y, window->DC.CurrLineTextBaseOffset); // Latch before ItemSize changes it
		const float frame_height =
			ImMax(ImMin(window->DC.CurrLineSize.y, g.FontSize + style.FramePadding.y * 2), label_size.y + padding.y * 2);
		const float icon_size = frame_height;

		ImRect frame_bb = ImRect(window->DC.CursorPos, ImVec2(window->WorkRect.Max.x, window->DC.CursorPos.y + frame_height));
		if (display_frame)
		{
			// Framed header expand a little outside the default padding
			frame_bb.Min.x -= (float)(int)(window->WindowPadding.x * 0.5f - 1.0f);
			frame_bb.Max.x += (float)(int)(window->WindowPadding.x * 0.5f);
		}

		const float text_offset_x =
			(g.FontSize + icon_size + (display_frame ? padding.x * 3 : padding.x * 2)); // Collapser arrow width + Spacing
		const float text_width = g.FontSize + icon_size + (label_size.x > 0.0f ? label_size.x + padding.x * 2 : 0.0f); // Include collapser
		ItemSize(ImVec2(text_width, frame_height), text_base_offset_y);

		// For regular tree nodes, we arbitrary allow to click past 2 worth of ItemSpacing
		// (Ideally we'd want to add a flag for the user to specify if we want the hit test to be done up to the right side of the content
		// or not)
		const ImRect interact_bb =
			display_frame ? frame_bb
						  : ImRect(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Min.x + text_width + style.ItemSpacing.x * 2, frame_bb.Max.y);
		bool is_open = TreeNodeBehaviorIsOpen(id, flags);
		bool is_leaf = (flags & ImGuiTreeNodeFlags_Leaf) != 0;

		// Store a flag for the current depth to tell if we will allow closing this node when navigating one of its child.
		// For this purpose we essentially compare if g.NavIdIsAlive went from 0 to 1 between TreeNode() and TreePop().
		// This is currently only support 32 level deep and we are fine with (1 << Depth) overflowing into a zero.
		if (is_open && !g.NavIdIsAlive && (flags & ImGuiTreeNodeFlags_NavLeftJumpsBackHere) &&
			!(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
			window->DC.TreeStoreMayJumpToParentOnPop |= (1 << window->DC.TreeDepth);

		bool item_add = ItemAdd(interact_bb, id);
		window->DC.LastItemStatusFlags |= ImGuiItemStatusFlags_HasDisplayRect;
		window->DC.LastItemDisplayRect = frame_bb;

		if (!item_add)
		{
			if (is_open && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
				TreePushOverrideID(id);
			IMGUI_TEST_ENGINE_ITEM_INFO(window->DC.LastItemId,
										label,
										window->DC.ItemFlags | (is_leaf ? 0 : ImGuiItemStatusFlags_Openable) |
											(is_open ? ImGuiItemStatusFlags_Opened : 0));
			return is_open;
		}

		// Flags that affects opening behavior:
		// - 0 (default) .................... single-click anywhere to open
		// - OpenOnDoubleClick .............. double-click anywhere to open
		// - OpenOnArrow .................... single-click on arrow to open
		// - OpenOnDoubleClick|OpenOnArrow .. single-click on arrow or double-click anywhere to open
		ImGuiButtonFlags button_flags = ImGuiButtonFlags_NoKeyModifiers;
		if (flags & ImGuiTreeNodeFlags_AllowItemOverlap)
			button_flags |= ImGuiButtonFlags_AllowItemOverlap;
		if (flags & ImGuiTreeNodeFlags_OpenOnDoubleClick)
			button_flags |= ImGuiButtonFlags_PressedOnDoubleClick |
							((flags & ImGuiTreeNodeFlags_OpenOnArrow) ? ImGuiButtonFlags_PressedOnClickRelease : 0);
		if (!is_leaf)
			button_flags |= ImGuiButtonFlags_PressedOnDragDropHold;

		bool selected = (flags & ImGuiTreeNodeFlags_Selected) != 0;
		const bool was_selected = selected;

		bool hovered, held;
		bool pressed = ButtonBehavior(interact_bb, id, &hovered, &held, button_flags);
		bool toggled = false;
		if (!is_leaf)
		{
			if (pressed)
			{
				toggled = !(flags & (ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick)) || (g.NavActivateId == id);
				if (flags & ImGuiTreeNodeFlags_OpenOnArrow)
					toggled |= IsMouseHoveringRect(interact_bb.Min, ImVec2(interact_bb.Min.x + text_offset_x, interact_bb.Max.y)) &&
							   (!g.NavDisableMouseHover);
				if (flags & ImGuiTreeNodeFlags_OpenOnDoubleClick)
					toggled |= g.IO.MouseDoubleClicked[0];
				if (g.DragDropActive && is_open) // When using Drag and Drop "hold to open" we keep the node highlighted after opening, but
												 // never close it again.
					toggled = false;
			}

			if (g.NavId == id && g.NavMoveRequest && g.NavMoveDir == ImGuiDir_Left && is_open)
			{
				toggled = true;
				NavMoveRequestCancel();
			}
			if (g.NavId == id && g.NavMoveRequest && g.NavMoveDir == ImGuiDir_Right &&
				!is_open) // If there's something upcoming on the line we may want to give it the priority?
			{
				toggled = true;
				NavMoveRequestCancel();
			}

			if (toggled)
			{
				is_open = !is_open;
				window->DC.StateStorage->SetInt(id, is_open);
			}
		}
		if (flags & ImGuiTreeNodeFlags_AllowItemOverlap)
			SetItemAllowOverlap();

		// In this branch, TreeNodeBehavior() cannot toggle the selection so this will never trigger.
		if (selected != was_selected) //-V547
			window->DC.LastItemStatusFlags |= ImGuiItemStatusFlags_ToggledSelection;

		// Render
		const ImU32 bg_col = GetColorU32((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
		const ImU32 text_col = GetColorU32(ImGuiCol_Text);
		const ImVec2 text_pos = frame_bb.Min + ImVec2(text_offset_x, text_base_offset_y);
		ImGuiNavHighlightFlags nav_highlight_flags = ImGuiNavHighlightFlags_TypeThin;
		if (display_frame)
		{
			// Framed type
			RenderFrame(frame_bb.Min, frame_bb.Max, bg_col, true, style.FrameRounding);
			RenderNavHighlight(frame_bb, id, nav_highlight_flags);
			RenderArrow(window->DrawList,
						frame_bb.Min + ImVec2(padding.x, text_base_offset_y),
						text_col,
						is_open ? ImGuiDir_Down : ImGuiDir_Right,
						1.0f);
			if (flags & ImGuiTreeNodeFlags_ClipLabelForTrailingButton)
				frame_bb.Max.x -= g.FontSize + style.FramePadding.x;

			window->DrawList->AddImage(user_texture,
									   frame_bb.Min + ImVec2(padding.x + g.FontSize, text_base_offset_y),
									   frame_bb.Min + ImVec2(padding.x + g.FontSize, text_base_offset_y) + ImVec2(icon_size, icon_size),
									   ImVec2(0, 0),
									   ImVec2(1, 1));

			if (g.LogEnabled)
			{
				// NB: '##' is normally used to hide text (as a library-wide feature), so we need to specify the text range to make sure the
				// ## aren't stripped out here.
				const char log_prefix[] = "\n##";
				const char log_suffix[] = "##";
				LogRenderedText(&text_pos, log_prefix, log_prefix + 3);
				RenderTextClipped(text_pos, frame_bb.Max, label, label_end, &label_size);
				LogRenderedText(&text_pos, log_suffix, log_suffix + 2);
			}
			else
			{
				RenderTextClipped(text_pos, frame_bb.Max, label, label_end, &label_size);
			}
		}
		else
		{
			// Unframed typed for tree nodes
			if (hovered || selected)
			{
				RenderFrame(frame_bb.Min, frame_bb.Max, bg_col, false);
				RenderNavHighlight(frame_bb, id, nav_highlight_flags);
			}

			if (flags & ImGuiTreeNodeFlags_Bullet)
				RenderBullet(
					window->DrawList, frame_bb.Min + ImVec2(text_offset_x * 0.5f, g.FontSize * 0.50f + text_base_offset_y), text_col);
			else if (!is_leaf)
				RenderArrow(window->DrawList,
							frame_bb.Min + ImVec2(padding.x, g.FontSize * 0.15f + text_base_offset_y),
							text_col,
							is_open ? ImGuiDir_Down : ImGuiDir_Right,
							0.70f);

			window->DrawList->AddImage(user_texture,
									   frame_bb.Min + ImVec2(padding.x + g.FontSize, text_base_offset_y),
									   frame_bb.Min + ImVec2(padding.x + g.FontSize, text_base_offset_y) + ImVec2(icon_size, icon_size),
									   ImVec2(0, 0),
									   ImVec2(1, 1));

			if (g.LogEnabled)
				LogRenderedText(&text_pos, ">");
			RenderText(text_pos, label, label_end, false);
		}

		if (is_open && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
			TreePushOverrideID(id);
		IMGUI_TEST_ENGINE_ITEM_INFO(
			id, label, window->DC.ItemFlags | (is_leaf ? 0 : ImGuiItemStatusFlags_Openable) | (is_open ? ImGuiItemStatusFlags_Opened : 0));
		return is_open;
	}

	bool TreeNodeEx(const char* label, ImGuiTreeNodeFlags flags, bool* v, ImTextureID user_texture)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		return TreeNodeBehavior(window->GetID(label), flags, v, user_texture, label, NULL);
	}

	bool ImageButton_(ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;

		// Default to using texture ID as ID. User can still push string/integer prefixes.
		// We could hash the size/uv to create a unique ID but that would prevent the user from animating UV.
		PushID((void *)user_texture_id);
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

	#define IM_F32_TO_INT8_UNBOUND(_VAL)    ((int)((_VAL) * 255.0f + ((_VAL)>=0 ? 0.5f : -0.5f)))   // Unsaturated, for display purpose

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
		float col_rgb_[4] = { col[0], col[1], col[2], alpha ? col[3] : 1.0f };
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
		flags |= (g.ColorEditOptions & ~(ImGuiColorEditFlags__DisplayMask | ImGuiColorEditFlags__DataTypeMask | ImGuiColorEditFlags__PickerMask));

#ifndef RAW_HSV
		const bool alpha = (flags & ImGuiColorEditFlags_NoAlpha) == 0;
#endif
		const bool hdr = (flags & ImGuiColorEditFlags_HDR) != 0;
		const int components = alpha ? 4 : 3;

		// Convert to the formats we need
		float f[4] = { col[0], col[1], col[2], alpha ? col[3] : 1.0f };
#ifndef RAW_HSV
		if (flags & ImGuiColorEditFlags_HSV)
			ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);
#endif
		int i[4] = { IM_F32_TO_INT8_UNBOUND(f[0]), IM_F32_TO_INT8_UNBOUND(f[1]), IM_F32_TO_INT8_UNBOUND(f[2]), IM_F32_TO_INT8_UNBOUND(f[3]) };

		bool value_changed = false;
		bool value_changed_as_float = false;

		if ((flags & (ImGuiColorEditFlags_RGB | ImGuiColorEditFlags_HSV)) != 0 && (flags & ImGuiColorEditFlags_NoInputs) == 0)
		{
			// RGB/HSV 0..255 Sliders
			const float w_item_one = ImMax(1.0f, (float)(int)((w_items_all - (style.ItemInnerSpacing.x) * (components - 1)) / (float)components));
			const float w_item_last = ImMax(1.0f, (float)(int)(w_items_all - (w_item_one + style.ItemInnerSpacing.x) * (components - 1)));

			const bool hide_prefix = (w_item_one <= CalcTextSize((flags & ImGuiColorEditFlags_Float) ? "M:0.000" : "M:000").x);
			const char* ids[4] = { "##X", "##Y", "##Z", "##W" };
			const char* fmt_table_int[3][4] =
			{
				{ "%3d",   "%3d",   "%3d",   "%3d" }, // Short display
				{ "R:%3d", "G:%3d", "B:%3d", "A:%3d" }, // Long display for RGBA
				{ "H:%3d", "S:%3d", "V:%3d", "A:%3d" }  // Long display for HSVA
			};
			const char* fmt_table_float[3][4] =
			{
				{ "%0.3f",   "%0.3f",   "%0.3f",   "%0.3f" }, // Short display
				{ "R:%0.3f", "G:%0.3f", "B:%0.3f", "A:%0.3f" }, // Long display for RGBA
				{ "H:%0.3f", "S:%0.3f", "V:%0.3f", "A:%0.3f" }  // Long display for HSVA
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
					value_changed = value_changed_as_float = value_changed | DragFloat(ids[n], &f[n], 1.0f / 255.0f, 0.0f, hdr ? 0.0f : 1.0f, fmt_table_float[fmt_idx][n]);
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
				ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X%02X", ImClamp(i[0], 0, 255), ImClamp(i[1], 0, 255), ImClamp(i[2], 0, 255), ImClamp(i[3], 0, 255));
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
					sscanf(p, "%02X%02X%02X%02X", (unsigned int*)&i[0], (unsigned int*)&i[1], (unsigned int*)&i[2], (unsigned int*)&i[3]); // Treat at unsigned (%X is unsigned)
				else
					sscanf(p, "%02X%02X%02X", (unsigned int*)&i[0], (unsigned int*)&i[1], (unsigned int*)&i[2]);
			}
			if (!(flags & ImGuiColorEditFlags_NoOptions))
				OpenPopupOnItemClick("context");
			PopItemWidth();
		}

		ImGuiWindow* picker_active_window = NULL;
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
				ImGuiColorEditFlags picker_flags_to_forward = ImGuiColorEditFlags__DataTypeMask | ImGuiColorEditFlags__PickerMask | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaBar;
				ImGuiColorEditFlags picker_flags = (flags_untouched & picker_flags_to_forward) | ImGuiColorEditFlags__DisplayMask | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf;
				PushItemWidth(square_sz * 12.0f); // Use 256 + bar sizes?

#ifdef RAW_HSV
				auto picker_changed = ColorPicker4("##picker", col_rgb_, picker_flags, &g.ColorPickerRef.x);
				value_changed |= picker_changed;

				if (picker_changed)
				{
					float col_raw[4] = { col_rgb_[0], col_rgb_[1], col_rgb_[2], col_rgb_[3] };

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
		if (picker_active_window == NULL)
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
		if ((window->DC.LastItemStatusFlags & ImGuiItemStatusFlags_HoveredRect) && !(flags & ImGuiColorEditFlags_NoDragDrop) && BeginDragDropTarget())
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
}

namespace efk
{
	template <size_t size_>
	struct utf8str {
		enum {size = size_};
		char data[size];
		utf8str(const char16_t* u16str) {
			Effekseer::ConvertUtf16ToUtf8((int8_t*)data, size, (const int16_t*)u16str);
		}
		operator const char*() const {
			return data;
		}
	};

	// http://hasenpfote36.blogspot.jp/2016/09/stdcodecvt.html
	static constexpr std::codecvt_mode mode = std::codecvt_mode::little_endian;

	static std::string utf16_to_utf8(const std::u16string& s)
	{
#if defined(_MSC_VER)
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

#if defined(_MSC_VER)
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
			Effekseer::TextureData* texture = image->GetTextureData();
			if (texture != nullptr)
			{
				if (texture->UserPtr != nullptr)
				{
					return (ImTextureID)texture->UserPtr;
				}
				else
				{
					return (ImTextureID)texture->UserID;
				}
			}
		}
		return nullptr;
	}

	bool DragFloatN(const char* label, float* v, int components, float v_speed, float* v_min, float* v_max, 
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

		const char* display_formats[] = {
			display_format1,
			display_format2,
			display_format3
		};

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

	bool DragIntN(const char* label, int* v, int components, int v_speed, int* v_min, int* v_max,
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

		const char* display_formats[] = {
			display_format1,
			display_format2,
			display_format3
		};

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



	GUIManager::GUIManager()
	{}

	GUIManager::~GUIManager()
	{}

	bool GUIManager::Initialize(const char16_t* title, int32_t width, int32_t height, efk::DeviceType deviceType, bool isSRGBMode)
	{
		window = new efk::Window();

		this->deviceType = deviceType;

		if (!window->Initialize(title, width, height, isSRGBMode, deviceType))
		{
			ES_SAFE_DELETE(window);
			return false;
		}

		window->Resized = [this](int x, int y) -> void
		{
			if (this->callback != nullptr)
			{
				this->callback->Resized(x, y);
			}
		};

		window->Focused = [this]() -> void
		{
			if (this->callback != nullptr)
			{
				this->callback->Focused();
			}
		};

		window->Droped = [this](const char* path) -> void
		{
			if (this->callback != nullptr)
			{
				this->callback->SetPath(utf8_to_utf16(path).c_str());
				this->callback->Droped();
			}
		};

		window->Closing = [this]() -> bool
		{
			if (this->callback != nullptr)
			{
				return this->callback->Closing();
			}

			return true;
		};

		window->Iconify = [this](float f) -> void
		{
			if (this->callback != nullptr)
			{
				this->callback->Iconify(f);
			}
		};

		if (deviceType == DeviceType::OpenGL)
		{
			window->MakeCurrent();

#ifdef _WIN32
			glewInit();
#endif
		}

#ifdef _WIN32
		// Calculate font scale from DPI
		HDC screen = GetDC(0);
		int dpiX = GetDeviceCaps(screen, LOGPIXELSX);
		fontScale = (float)dpiX / 96.0f;
#endif
		
		return true;
	}

	void GUIManager::InitializeGUI(Native* native)
	{
		ImGui::CreateContext();
		
		if (deviceType == DeviceType::OpenGL)
		{
			ImGuiIO& io = ImGui::GetIO();
			// It causes bugs on some mac pc
			//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

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
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

			ImGui_ImplGlfw_InitForVulkan(window->GetGLFWWindows(), true);
			auto r = (EffekseerRendererDX11::Renderer*)native->GetRenderer();
			ImGui_ImplDX11_Init(r->GetDevice(), r->GetContext());
		}
		else
		{
			ImGui_ImplGlfw_InitForOpenGL(window->GetGLFWWindows(), true);
			auto r = (EffekseerRendererDX9::Renderer*)native->GetRenderer();
			ImGui_ImplDX9_Init(r->GetDevice());
		}
#endif

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();

		style.ChildRounding = 3.f;
		style.GrabRounding = 3.f;
		style.WindowRounding = 3.f;
		style.ScrollbarRounding = 3.f;
		style.FrameRounding = 3.f;
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

		// mono tone

		for (int32_t i = 0; i < ImGuiCol_COUNT; i++)
		{
			auto v = (style.Colors[i].x + style.Colors[i].y + style.Colors[i].z) / 3.0f;
			style.Colors[i].x = v;
			style.Colors[i].y = v;
			style.Colors[i].z = v;
		}

		style.Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.9f);

	}

	void GUIManager::SetTitle(const char16_t* title)
	{
		window->SetTitle(title);
	}

	void GUIManager::SetWindowIcon(const char16_t * iconPath)
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
			ImGui_ImplDX9_Shutdown();
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
		return window->DoEvents();
	}

	void GUIManager::Present()
	{
		window->Present();
	}

	void GUIManager::Close()
	{
		window->Close();
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
			ImGui_ImplDX9_NewFrame();
		}
#endif

		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void GUIManager::RenderGUI(bool isValid)
	{
		ImGui::EndFrame();
		ImGui::Render();

		if (isValid)
		{
			if (deviceType == DeviceType::OpenGL)
			{
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			}
#if _WIN32
			else if (deviceType == DeviceType::DirectX11)
			{
				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			}
			else
			{
				ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
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
				ImGui::RenderPlatformWindowsDefault();
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

	void GUIManager::HiddenSeparator()
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return;

		    ImGuiSeparatorFlags flags =
			(window->DC.LayoutType == ImGuiLayoutType_Horizontal) ? ImGuiSeparatorFlags_Vertical : ImGuiSeparatorFlags_Horizontal;
		flags |= ImGuiSeparatorFlags_SpanAllColumns;

		ImGuiContext& g = *GImGui;
		IM_ASSERT(ImIsPowerOfTwo(flags &
								 (ImGuiSeparatorFlags_Horizontal | ImGuiSeparatorFlags_Vertical))); // Check that only 1 option is selected

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
			if (!window->DC.GroupStack.empty())
				x1 += window->DC.Indent.x;

			ImGuiColumns* columns = (flags & ImGuiSeparatorFlags_SpanAllColumns) ? window->DC.CurrentColumns : NULL;
			if (columns)
				ImGui::PushColumnsBackground();

			// We don't provide our width to the layout so that it doesn't get feed back into AutoFit
			const ImRect bb(ImVec2(x1, window->DC.CursorPos.y), ImVec2(x2, window->DC.CursorPos.y + thickness_draw));
			ImGui::ItemSize(ImVec2(0.0f, thickness_layout));
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
		}
	}

	void GUIManager::SameLine()
	{
		ImGui::SameLine();
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

	void GUIManager::Text(const char16_t* text)
	{
		if (std::char_traits<char16_t>::length(text) < 1024)
		{
			ImGui::Text(utf8str<1024>(text));
		}
		else
		{
			ImGui::Text(utf16_to_utf8(text).c_str());
		}
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
		return ImGui::ImageButton_(ToImTextureID(user_texture_id), ImVec2(x, y), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1));
	}

	bool GUIManager::Checkbox(const char16_t* label, bool* v)
	{
		return ImGui::Checkbox(utf8str<256>(label), v);
	}

	bool GUIManager::RadioButton(const char16_t* label, bool active)
	{
		return ImGui::RadioButton(utf8str<256>(label), active);
	}

	bool GUIManager::InputInt(const char16_t* label, int* v, int step, int step_fast)
	{
		return ImGui::InputInt(utf8str<256>(label), v, step, step_fast);
	}

	bool GUIManager::SliderInt(const char16_t* label, int* v, int v_min, int v_max)
	{
		return ImGui::SliderInt(utf8str<256>(label), v, v_min, v_max);
	}

	bool GUIManager::BeginCombo(const char16_t* label, const char16_t* preview_value, ComboFlags flags, ImageResource* user_texture_id)
	{
		return ImGui::BeginCombo(
			utf8str<256>(label),
			utf8str<256>(preview_value),
			(int)flags, ToImTextureID(user_texture_id));
	}

	void GUIManager::EndCombo()
	{
		ImGui::EndCombo();
	}

	bool GUIManager::DragFloat(const char16_t* label, float* v, float v_speed, float v_min, float v_max, const char* display_format, float power)
	{
		return ImGui::DragFloat(utf8str<256>(label), v, v_speed, v_min, v_max, display_format);
	}

	bool GUIManager::DragFloat2(const char16_t* label, float* v, float v_speed, float v_min, float v_max, const char* display_format, float power)
	{
		return ImGui::DragFloat2(utf8str<256>(label), v, v_speed, v_min, v_max, display_format);
	}

	bool GUIManager::DragFloat3(const char16_t* label, float* v, float v_speed, float v_min, float v_max, const char* display_format, float power)
	{
		return ImGui::DragFloat3(utf8str<256>(label), v, v_speed, v_min, v_max, display_format);
	}

	bool GUIManager::DragFloat4(const char16_t* label, float* v, float v_speed, float v_min, float v_max, const char* display_format, float power)
	{
		return ImGui::DragFloat4(utf8str<256>(label), v, v_speed, v_min, v_max, display_format);
	}

	bool GUIManager::DragFloatRange2(const char16_t* label, float* v_current_min, float* v_current_max, float v_speed, float v_min, float v_max, const char* display_format, const char* display_format_max, float power)
	{
		return ImGui::DragFloatRange2(utf8str<256>(label), v_current_min, v_current_max, v_speed, v_min, v_max, display_format, display_format_max);
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

	bool GUIManager::DragIntRange2(const char16_t* label, int* v_current_min, int* v_current_max, float v_speed, int v_min, int v_max, const char* display_format, const char* display_format_max)
	{
		return ImGui::DragIntRange2(utf8str<256>(label), v_current_min, v_current_max, v_speed, v_min, v_max, display_format, display_format_max);
	}

	bool GUIManager::DragFloat1EfkEx(const char16_t* label, float* v, float v_speed, float v_min, float v_max, const char16_t* display_format1, float power)
	{
		float v_min_[3];
		float v_max_[3];
		v_min_[0] = v_min;
		v_max_[0] = v_max;

		return DragFloatN(
			utf8str<256>(label), v, 1, v_speed, v_min_, v_max_, 
			utf8str<256>(display_format1), 
			nullptr, 
			nullptr,
			power);
	}

	bool GUIManager::DragFloat2EfkEx(const char16_t* label, float* v, float v_speed, float v_min1, float v_max1, float v_min2, float v_max2, const char16_t* display_format1, const char16_t* display_format2, float power)
	{
		float v_min_[3];
		float v_max_[3];
		v_min_[0] = v_min1;
		v_max_[0] = v_max1;
		v_min_[1] = v_min2;
		v_max_[1] = v_max2;

		return DragFloatN(
			utf8str<256>(label), v, 2, v_speed, v_min_, v_max_,
			utf8str<256>(display_format1),
			utf8str<256>(display_format2),
			nullptr,
			power);
	}

	bool GUIManager::DragFloat3EfkEx(const char16_t* label, float* v, float v_speed, float v_min1, float v_max1, float v_min2, float v_max2, float v_min3, float v_max3, const char16_t* display_format1, const char16_t* display_format2, const char16_t* display_format3, float power)
	{
		float v_min_[3];
		float v_max_[3];
		v_min_[0] = v_min1;
		v_max_[0] = v_max1;
		v_min_[1] = v_min2;
		v_max_[1] = v_max2;
		v_min_[2] = v_min3;
		v_max_[2] = v_max3;

		return DragFloatN(
			utf8str<256>(label), v, 3, v_speed, v_min_, v_max_,
			utf8str<256>(display_format1),
			utf8str<256>(display_format2),
			utf8str<256>(display_format3),
			power);
	}

	bool GUIManager::DragInt2EfkEx(const char16_t* label, int* v, int v_speed, int v_min1, int v_max1, int v_min2, int v_max2, const char16_t* display_format1, const char16_t* display_format2)
	{
		int v_min_[3];
		int v_max_[3];
		v_min_[0] = v_min1;
		v_max_[0] = v_max1;
		v_min_[1] = v_min2;
		v_max_[1] = v_max2;

		return DragIntN(
			utf8str<256>(label), v, 2, v_speed, v_min_, v_max_,
			utf8str<256>(display_format1),
			utf8str<256>(display_format2),
			nullptr);
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

	bool GUIManager::TreeNodeEx(const char16_t* label, bool* v, ImageResource* user_texture_id, TreeNodeFlags flags)
	{
		return ImGui::TreeNodeEx(utf8str<256>(label), (ImGuiTreeNodeFlags)flags, v, ToImTextureID(user_texture_id));
	}

	bool GUIManager::Selectable(const char16_t* label, bool selected, SelectableFlags flags, ImageResource* user_texture_id)
	{
		return ImGui::Selectable(utf8str<256>(label), selected, (int)flags, ImVec2(0, 0), ToImTextureID(user_texture_id));
	}

	void GUIManager::SetTooltip(const char16_t* text)
	{
		ImGui::SetTooltip(utf8str<256>(text));
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

	bool GUIManager::MenuItem(const char16_t* label, const char* shortcut, bool selected, bool enabled, ImageResource* icon)
	{
		return ImGui::MenuItem(utf8str<256>(label), shortcut, selected, enabled, ToImTextureID(icon));
	}

	bool GUIManager::MenuItem(const char16_t* label, const char* shortcut, bool* p_selected, bool enabled, ImageResource* icon)
	{
		return ImGui::MenuItem(utf8str<256>(label), shortcut, p_selected, enabled, ToImTextureID(icon));
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

	void GUIManager::AddFontFromFileTTF(const char16_t* filename, float size_pixels)
	{
		ImGuiIO& io = ImGui::GetIO();
		
		size_pixels = roundf(size_pixels * fontScale);

		io.Fonts->AddFontFromFileTTF(utf8str<280>(filename), size_pixels, nullptr, glyphRangesJapanese);
	}

	bool GUIManager::BeginChildFrame(uint32_t id, const Vec2& size, WindowFlags flags)
	{
		return ImGui::BeginChildFrame(id, ImVec2(size.X, size.Y), (int32_t)flags);
	}

	void GUIManager::EndChildFrame()
	{
		ImGui::EndChildFrame();
	}

	bool GUIManager::IsKeyDown(int user_key_index)
	{
		return ImGui::IsKeyDown(user_key_index);
	}

	bool GUIManager::IsMouseDown(int button)
	{
		return ImGui::IsMouseDown(button);
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

	bool GUIManager::IsWindowHovered()
	{
		return ImGui::IsWindowHovered();
	}

	bool GUIManager::IsAnyWindowHovered()
	{
		return ImGui::IsAnyWindowHovered();
	}

	MouseCursor GUIManager::GetMouseCursor()
	{
		return (MouseCursor)ImGui::GetMouseCursor();
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
		ImVec2 windowSize;
		windowSize.x = ImGui::GetIO().DisplaySize.x;
		windowSize.y = ImGui::GetIO().DisplaySize.y - 25;

		ImGui::SetNextWindowSize(windowSize);
		
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			auto pos = ImGui::GetMainViewport()->Pos;
			pos.y += 25;
			ImGui::SetNextWindowPos(pos);
		}
		else
		{
			ImGui::SetNextWindowPos(ImVec2(0, 25));
		}
		
		const ImGuiWindowFlags flags = (ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
		const float oldWindowRounding = ImGui::GetStyle().WindowRounding; ImGui::GetStyle().WindowRounding = 0;
		const bool visible = ImGui::Begin(utf8str<256>(label), NULL, ImVec2(0, 0), 1.0f, flags);
		ImGui::GetStyle().WindowRounding = oldWindowRounding;
		return visible;
	}

	void GUIManager::SetNextDock(DockSlot slot)
	{
		ImGui::SetNextDock((ImGuiDockSlot)slot);
	}

	void GUIManager::BeginDockspace()
	{
		ImGui::BeginDockspace();
	}

	void GUIManager::EndDockspace()
	{
		ImGui::EndDockspace();
	}

	bool GUIManager::BeginDock(const char16_t* label, bool* p_open, WindowFlags extra_flags, Vec2 default_size)
	{
		return ImGui::BeginDock(utf8str<256>(label), p_open, (int32_t)extra_flags, ImVec2(default_size.X, default_size.Y));
	}

	void GUIManager::EndDock()
	{
		ImGui::EndDock();
	}

	void GUIManager::SetNextDockRate(float rate)
	{
		ImGui::SetNextDockRate(rate);
	}

	void GUIManager::ResetNextParentDock()
	{
		ImGui::ResetNextParentDock();
	}

	void GUIManager::SaveDock(const char16_t* path)
	{
		ImGui::SaveDock(utf8str<280>(path));
	}
	
	void GUIManager::LoadDock(const char16_t* path)
	{
		ImGui::LoadDock(utf8str<280>(path));
	}

	void GUIManager::ShutdownDock()
	{
		ImGui::ShutdownDock();
	}

	void GUIManager::SetNextDockIcon(ImageResource* icon, Vec2 iconSize)
	{
		ImGui::SetNextDockIcon(ToImTextureID(icon), ImVec2(iconSize.X, iconSize.Y));
	}

	void GUIManager::SetNextDockTabToolTip(const char16_t* popup)
	{
		ImGui::SetNextDockTabToolTip(utf8str<256>(popup));
	}

	bool GUIManager::GetDockActive()
	{
		return ImGui::GetDockActive();
	}

	void GUIManager::SetDockActive()
	{
		ImGui::SetDockActive();
	}

	bool GUIManager::BeginFCurve(int id, const Vec2& size, float current, const Vec2& scale, float min_value, float max_value)
	{
		return ImGui::BeginFCurve(id, ImVec2(size.X, size.Y), current, ImVec2(scale.X, scale.Y), min_value, max_value);
	}

	void GUIManager::EndFCurve()
	{
		ImGui::EndFCurve();
	}

	bool GUIManager::FCurve(
		int fcurve_id,
		float* keys, float* values,
		float* leftHandleKeys, float* leftHandleValues,
		float* rightHandleKeys, float* rightHandleValues,
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
		return ImGui::FCurve(
			fcurve_id,
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
        return (DialogSelection)boxer::show(
                                            utf8str<256>(message),
                                            utf8str<256>(title),
                                            (boxer::Style)style,
                                            (boxer::Buttons)buttons);
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

	int GUIManager::GetLanguage()
	{
		return (int32_t)GetEfkLanguage();
	}
}
