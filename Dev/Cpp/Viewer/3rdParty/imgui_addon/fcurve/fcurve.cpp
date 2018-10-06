#include "fcurve.h"
#include "imgui_internal.h"

#include <stdint.h>
#include <algorithm>

namespace ImGui
{
	static ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)
	{
		return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
	}

	static ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs)
	{
		return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y);
	}

	static ImVec2 operator * (const ImVec2& lhs, float rhs)
	{
		return ImVec2(lhs.x * rhs, lhs.y * rhs);
	}

	static ImVec2 operator / (const ImVec2& lhs, float rhs)
	{
		return ImVec2(lhs.x / rhs, lhs.y / rhs);
	}

	static ImVec2& operator+=(ImVec2& lhs, const ImVec2& rhs)
	{
		lhs.x += rhs.x;
		lhs.y += rhs.y;
		return lhs;
	}

	static float Dot(const ImVec2& lhs, const ImVec2& rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y;
	}

	static float ImLength(const ImVec2& lhs)
	{
		return sqrt(ImLengthSqr(lhs));
	}

	static ImVec2 ImNorm(const ImVec2& lhs)
	{
		return lhs / ImLength(lhs);
	}

	static double logn(float base, float x)
	{
		return log(x) / log(base);
	}

	enum class FCurveStorageValues : ImGuiID
	{
		SCALE_X = 100,
		SCALE_Y,
		OFFSET_X,
		OFFSET_Y,
		IS_PANNING,
		START_X,
		START_Y,
		DELTA_X,
		DELTA_Y,
		OVER_Y,
	};

	bool IsHovered(const ImVec2& v1, const ImVec2& v2, float radius)
	{
		ImVec2 diff;
		diff.x = v1.x - v2.x;
		diff.y = v1.y - v2.y;
		return diff.x * diff.x + diff.y * diff.y < radius * radius;
	}

	bool IsHoveredOnLine(const ImVec2& v, ImGuiWindow* window, const ImVec2& a, const ImVec2& b, ImU32 col, float thickness)
	{
		if ((col & IM_COL32_A_MASK) == 0)
			return false;

		window->DrawList->PathLineTo(a + ImVec2(0.5f, 0.5f));
		window->DrawList->PathLineTo(b + ImVec2(0.5f, 0.5f));

		bool isHovered = false;

		for (int32_t i = 0; i < window->DrawList->_Path.size() - 1; i++)
		{
			auto v1 = window->DrawList->_Path[i + 0];
			auto v2 = window->DrawList->_Path[i + 1];

			auto p0 = v - v1;
			auto p1 = v2 - v1;

			auto n = ImNorm(p1);
			auto component1 = n * ImDot(n, p0);
			auto component2 = p0 - component1;

			if (ImDot(component1, p1) < 0) continue;
			if (ImLength(component1) > ImLength(p1)) continue;
			if (ImLength(component2) > thickness) continue;
			isHovered = true;
			break;
		}
	
		window->DrawList->PathClear();

		return isHovered;
	}

	bool IsHoveredOnBezierCurve(const ImVec2& v, ImGuiWindow* window, const ImVec2& pos0, const ImVec2& cp0, const ImVec2& cp1, const ImVec2& pos1, ImU32 col, float thickness)
	{
		if ((col & IM_COL32_A_MASK) == 0)
			return false;

		window->DrawList->PathLineTo(pos0);
		window->DrawList->PathBezierCurveTo(cp0, cp1, pos1, 0);

		bool isHovered = false;

		for (int32_t i = 0; i < window->DrawList->_Path.size() - 1; i++)
		{
			auto v1 = window->DrawList->_Path[i + 0];
			auto v2 = window->DrawList->_Path[i + 1];

			auto p0 = v - v1;
			auto p1 = v2 - v1;

			auto n = ImNorm(p1);
			auto component1 = n * ImDot(n, p0);
			auto component2 = p0 - component1;

			if (ImDot(component1, p1) < 0) continue;
			if (ImLength(component1) > ImLength(p1)) continue;
			if (ImLength(component2) > thickness) continue;
			isHovered = true;
			break;
		}

		window->DrawList->PathClear();

		return isHovered;
	}

	void DrawMaker(ImGuiWindow* window, ImVec2 pos, int32_t size, uint32_t color, int32_t thickness)
	{
		window->DrawList->AddLine(ImVec2(pos.x + size, pos.y), ImVec2(pos.x, pos.y - size), color, thickness);
		window->DrawList->AddLine(ImVec2(pos.x - size, pos.y), ImVec2(pos.x, pos.y + size), color, thickness);
		window->DrawList->AddLine(ImVec2(pos.x + size, pos.y), ImVec2(pos.x, pos.y + size), color, thickness);
		window->DrawList->AddLine(ImVec2(pos.x - size, pos.y), ImVec2(pos.x, pos.y - size), color, thickness);
	}

	void ClampHandles(
		float* keys, float* values,
		float* leftHandleKeys, float* leftHandleValues,
		float* rightHandleKeys, float* rightHandleValues,
		int count)
	{
		for (int i = 0; i < count; i++)
		{
			if (0 < i)
			{
				leftHandleKeys[i] = std::max(leftHandleKeys[i], keys[i - 1]);
			}

			leftHandleKeys[i] = std::min(leftHandleKeys[i], keys[i]);

			if (i < count - 1)
			{
				rightHandleKeys[i] = std::min(rightHandleKeys[i], keys[i + 1]);
			}

			rightHandleKeys[i] = std::max(rightHandleKeys[i], keys[i]);
		}
	}

	bool BeginFCurve(int id, const ImVec2& size, float current, const ImVec2& scale, float min_value, float max_value)
	{
		bool isAutoZoomMode = min_value <= max_value;

		if (!BeginChildFrame(id, size, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
		{
			return false;
		}

		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
		{
			return false;
		}

		bool isGridShown = true;
		float offset_x = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::OFFSET_X, 0.0f);
		float offset_y = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::OFFSET_Y, 0.0f);

		float scale_x = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::SCALE_X, scale.x);
		float scale_y = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::SCALE_Y, scale.y);
		window->StateStorage.SetFloat((ImGuiID)FCurveStorageValues::SCALE_X, scale_x);
		window->StateStorage.SetFloat((ImGuiID)FCurveStorageValues::SCALE_Y, scale_y);

		const ImRect innerRect = window->InnerMainRect;
		float width = innerRect.Max.x - innerRect.Min.x;
		float height = innerRect.Max.y - innerRect.Min.y;

		auto transform_f2s = [&](const ImVec2& p) -> ImVec2
		{
			return ImVec2((p.x - offset_x) * scale_x + innerRect.Min.x, (- p.y - offset_y) * scale_y + innerRect.Min.y + height / 2);
		};

		auto transform_s2f = [&](const ImVec2& p) -> ImVec2
		{
			return ImVec2((p.x - innerRect.Min.x) / scale_x + offset_x, -((p.y - innerRect.Min.y - height / 2) / scale_y + offset_y));
		};

		bool isZoomed = false;

		// Horizontal
		if (ImGui::GetIO().MouseWheel != 0 && ImGui::IsWindowHovered() && ImGui::GetIO().KeyAlt)
		{
			auto mousePos = GetMousePos();
			auto mousePos_f_pre = transform_s2f(mousePos);

			auto s = powf(2, ImGui::GetIO().MouseWheel / 10.0);
			scale_x *= s;

			auto mousePos_f_now = transform_s2f(mousePos);

			offset_x += (mousePos_f_pre.x - mousePos_f_now.x);
			offset_y -= (mousePos_f_pre.y - mousePos_f_now.y);

			window->StateStorage.SetFloat((ImGuiID)FCurveStorageValues::SCALE_X, scale_x);
			window->StateStorage.SetFloat((ImGuiID)FCurveStorageValues::OFFSET_X, offset_x);
			window->StateStorage.SetFloat((ImGuiID)FCurveStorageValues::OFFSET_Y, offset_y);
			isZoomed = true;
		}

		// Vertial
		if (ImGui::GetIO().MouseWheel != 0 && ImGui::IsWindowHovered() && !isZoomed && ImGui::GetIO().KeyCtrl && !isAutoZoomMode)
		{
			auto mousePos = GetMousePos();
			auto mousePos_f_pre = transform_s2f(mousePos);

			auto s = powf(2, ImGui::GetIO().MouseWheel / 10.0);
			scale_y *= s;

			auto mousePos_f_now = transform_s2f(mousePos);

			offset_x += (mousePos_f_pre.x - mousePos_f_now.x);
			offset_y -= (mousePos_f_pre.y - mousePos_f_now.y);

			window->StateStorage.SetFloat((ImGuiID)FCurveStorageValues::SCALE_Y, scale_y);
			window->StateStorage.SetFloat((ImGuiID)FCurveStorageValues::OFFSET_X, offset_x);
			window->StateStorage.SetFloat((ImGuiID)FCurveStorageValues::OFFSET_Y, offset_y);
			isZoomed = true;
		}

		// move
		if (ImGui::GetIO().MouseWheel != 0 && ImGui::IsWindowHovered() && !isZoomed && !isAutoZoomMode)
		{
			float offset = ImGui::GetIO().MouseWheel * 5;
			offset_y = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::OFFSET_Y) - offset / scale_y;
			window->StateStorage.SetFloat((ImGuiID)FCurveStorageValues::OFFSET_X, offset_x);
			window->StateStorage.SetFloat((ImGuiID)FCurveStorageValues::OFFSET_Y, offset_y);
		}

		// AutoZoom
		if (isAutoZoomMode)
		{
			auto range = max_value - min_value;

			if (range == 0.0f)
			{
				max_value = +height / 2;
				min_value = -height / 2;
				range = height;
				scale_y = scale.y;
			}
			else
			{
				scale_y = height / (range);
			}
			
			offset_y = -(max_value + min_value) / 2.0f;
			window->StateStorage.SetFloat((ImGuiID)FCurveStorageValues::SCALE_Y, scale_y);
			window->StateStorage.SetFloat((ImGuiID)FCurveStorageValues::OFFSET_Y, offset_y);
		}

		// pan with user
		if (ImGui::IsMouseReleased(1))
		{
			window->StateStorage.SetBool((ImGuiID)FCurveStorageValues::IS_PANNING, false);
		}
		if (window->StateStorage.GetBool((ImGuiID)FCurveStorageValues::IS_PANNING, false))
		{
			ImVec2 drag_offset = ImGui::GetMouseDragDelta(1);
			offset_x = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::START_X) - drag_offset.x / scale_x;

			if (!isAutoZoomMode)
			{
				offset_y = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::START_Y) - drag_offset.y / scale_y;
			}
			
			window->StateStorage.SetFloat((ImGuiID)FCurveStorageValues::OFFSET_X, offset_x);
			window->StateStorage.SetFloat((ImGuiID)FCurveStorageValues::OFFSET_Y, offset_y);
		}
		else if (ImGui::IsMouseDragging(1) && ImGui::IsWindowHovered())
		{
			window->StateStorage.SetBool((ImGuiID)FCurveStorageValues::IS_PANNING, true);
			window->StateStorage.SetFloat((ImGuiID)FCurveStorageValues::START_X, offset_x);
			window->StateStorage.SetFloat((ImGuiID)FCurveStorageValues::START_Y, offset_y);
		}

		// render grid
		if (isGridShown)
		{
			ImVec2 upperLeft_s = ImVec2(innerRect.Min.x, innerRect.Min.y);
			ImVec2 lowerRight_s = ImVec2(innerRect.Max.x, innerRect.Max.y);
			
			auto upperLeft_f = transform_s2f(upperLeft_s);
			auto lowerRight_f = transform_s2f(lowerRight_s);

			auto kByPixel = (lowerRight_f.x - upperLeft_f.x) / width;
			auto vByPixel = -(lowerRight_f.y - upperLeft_f.y) / height;
			auto screenGridSize = 50.0f;
			auto fieldGridSizeX = screenGridSize * kByPixel;
			auto fieldGridSizeY = screenGridSize * vByPixel;

			fieldGridSizeX  = pow(5, (int32_t)logn(5, fieldGridSizeX) + 1);
			fieldGridSizeY = pow(5, (int32_t)logn(5, fieldGridSizeY) + 1);

			auto sx = (int)(upperLeft_f.x / fieldGridSizeX) * fieldGridSizeX;
			auto sy = (int)(upperLeft_f.y / fieldGridSizeY) * fieldGridSizeY;
			auto ex = (int)(lowerRight_f.x / fieldGridSizeX) * fieldGridSizeX + fieldGridSizeX;
			auto ey = (int)(lowerRight_f.y / fieldGridSizeY) * fieldGridSizeY - fieldGridSizeY;

			for (auto y = sy; y >= ey; y -= fieldGridSizeY)
			{
				window->DrawList->AddLine(transform_f2s(ImVec2(upperLeft_f.x, y)), transform_f2s(ImVec2(lowerRight_f.x, y)), 0x55000000);
			}

			for (auto x = sx; x <= ex; x += fieldGridSizeX)
			{
				window->DrawList->AddLine(transform_f2s(ImVec2(x, upperLeft_f.y)), transform_f2s(ImVec2(x, lowerRight_f.y)), 0x55000000);
			}

			for (auto y = sy; y >= ey; y -= fieldGridSizeY)
			{
				char text[200];
				sprintf(text, "%.2f", y);
				window->DrawList->AddText(
					transform_f2s(ImVec2(upperLeft_f.x, y)),
					0xff000000,
					text);
			}

			for (auto x = sx; x <= ex; x += fieldGridSizeX)
			{
				char text[200];
				sprintf(text, "%.2f", x);
				window->DrawList->AddText(
					transform_f2s(ImVec2(x, upperLeft_f.y)),
					0xff000000,
					text);
			}
		}

		// Render current
		{
			ImVec2 upperLeft_s = ImVec2(innerRect.Min.x, innerRect.Min.y);
			ImVec2 lowerRight_s = ImVec2(innerRect.Max.x, innerRect.Max.y);

			auto upperLeft_f = transform_s2f(upperLeft_s);
			auto lowerRight_f = transform_s2f(lowerRight_s);

			window->DrawList->AddLine(transform_f2s(ImVec2(current, upperLeft_f.y)), transform_f2s(ImVec2(current, lowerRight_f.y)), 0x55aaaaaa);
		}

		// get left button drag
		if (IsMouseDragging(0))
		{
			window->StateStorage.SetFloat((ImGuiID)FCurveStorageValues::DELTA_X, GetIO().MouseDelta.x);
			window->StateStorage.SetFloat((ImGuiID)FCurveStorageValues::DELTA_Y, GetIO().MouseDelta.y);
		}
		else
		{
			window->StateStorage.SetFloat((ImGuiID)FCurveStorageValues::DELTA_X, 0);
			window->StateStorage.SetFloat((ImGuiID)FCurveStorageValues::DELTA_Y, 0);
		}

		if (ImGui::GetMousePos().y < innerRect.Min.y)
		{
			window->StateStorage.SetFloat((ImGuiID)FCurveStorageValues::OVER_Y, ImGui::GetMousePos().y - innerRect.Min.y);
		}
		else if (ImGui::GetMousePos().y > innerRect.Max.y)
		{
			window->StateStorage.SetFloat((ImGuiID)FCurveStorageValues::OVER_Y, ImGui::GetMousePos().y - innerRect.Max.y);
		}
		else
		{
			window->StateStorage.SetFloat((ImGuiID)FCurveStorageValues::OVER_Y, 0);
		}

		return true;
	}

	void EndFCurve()
	{
		EndChildFrame();
	}

	bool NoPointFCurve(
		int fcurve_id,
		float* keys, float* values,
		float* leftHandleKeys, float* leftHandleValues,
		float* rightHandleKeys, float* rightHandleValues,
		ImFCurveInterporationType* interporations,
		bool* kv_selected,
		int count,
		float defaultValue,
		bool isLocked,
		bool canControl,
		ImU32 col,
		bool selected,
		int* newCount,
		bool* newSelected)
	{
		PushID(fcurve_id);

		ImGuiWindow* window = GetCurrentWindow();

		float offset_x = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::OFFSET_X, 0.0f);
		float offset_y = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::OFFSET_Y, 0.0f);

		float scale_x = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::SCALE_X, 1.0f);
		float scale_y = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::SCALE_Y, 1.0f);

		const ImRect innerRect = window->InnerMainRect;
		float width = innerRect.Max.x - innerRect.Min.x;
		float height = innerRect.Max.y - innerRect.Min.y;

		auto transform_f2s = [&](const ImVec2& p) -> ImVec2
		{
			return ImVec2((p.x - offset_x) * scale_x + innerRect.Min.x, (-p.y - offset_y) * scale_y + innerRect.Min.y + height / 2);
		};

		auto transform_s2f = [&](const ImVec2& p) -> ImVec2
		{
			return ImVec2((p.x - innerRect.Min.x) / scale_x + offset_x, -((p.y - innerRect.Min.y - height / 2) / scale_y + offset_y));
		};


		bool hasControlled = !canControl;

		if (isLocked)
		{
			hasControlled = true;
		}

		bool isLineHovered = false;
		{
			auto v1 = ImVec2(transform_s2f(innerRect.Min).x, defaultValue);
			auto v2 = ImVec2(transform_s2f(innerRect.Max).x, defaultValue);

			isLineHovered = isLineHovered | IsHoveredOnLine(
				GetMousePos(), window,
				transform_f2s(v1),
				transform_f2s(v2),
				col,
				2);
		}

		// Add point
		if (!hasControlled && selected)
		{
			if (isLineHovered && IsMouseDoubleClicked(0))
			{
				auto mousePos = GetMousePos();
				auto v = transform_s2f(mousePos);

				keys[0] = v.x;
				values[0] = v.y;
				leftHandleKeys[0] = v.x;
				leftHandleValues[0] = v.y;
				rightHandleKeys[0] = v.x;
				rightHandleValues[0] = v.y;
				kv_selected[0] = false;
				interporations[0] = ImFCurveInterporationType::Linear;

				(*newCount) = count + 1;
				hasControlled = true;
			}
		}

		// is line selected
		if (newSelected != nullptr && !hasControlled && !isLocked && IsMouseClicked(0))
		{
			if (isLineHovered)
			{
				(*newSelected) = (*newSelected) | isLineHovered;
				hasControlled = true;
			}
		}

		// render curve

		auto lineThiness = 1;
		if (selected) lineThiness++;
		if (isLineHovered) lineThiness++;

		// start
		{
			auto v1 = ImVec2(transform_s2f(innerRect.Min).x, defaultValue);
			auto v2 = ImVec2(transform_s2f(innerRect.Max).x, defaultValue);

			window->DrawList->AddLine(
				transform_f2s(v1),
				transform_f2s(v2),
				col,
				lineThiness);
		}

		PopID();

		return hasControlled;
	}

	bool AddPointFCurve(
		float* keys, float* values,
		float* leftHandleKeys, float* leftHandleValues,
		float* rightHandleKeys, float* rightHandleValues,
		ImFCurveInterporationType* interporations)
	{
		return false;
	}

	bool FCurve(
		int fcurve_id,
		float* keys, float* values,
		float* leftHandleKeys, float* leftHandleValues,
		float* rightHandleKeys, float* rightHandleValues,
		ImFCurveInterporationType* interporations,
		ImFCurveEdgeType startEdge,
		ImFCurveEdgeType endEdge,
		bool* kv_selected,
		int count,
		float defaultValue,
		bool isLocked,
		bool canControl,
		ImU32 col,
		bool selected,
		float v_min,
		float v_max,
		int* newCount,
		bool* newSelected,
		float* movedX,
		float* movedY,
		int* changedType)
	{
		// non point
		if (count == 0)
		{
			return NoPointFCurve(
				fcurve_id,
				keys, values,
				leftHandleKeys, leftHandleValues,
				rightHandleKeys, rightHandleValues,
				interporations,
				kv_selected,
				count,
				defaultValue,
				isLocked,
				canControl,
				col,
				selected,
				newCount,
				newSelected);
		}

		PushID(fcurve_id);

		ImGuiWindow* window = GetCurrentWindow();

		float offset_x = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::OFFSET_X, 0.0f);
		float offset_y = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::OFFSET_Y, 0.0f);

		float scale_x = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::SCALE_X, 1.0f);
		float scale_y = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::SCALE_Y, 1.0f);

		const ImRect innerRect = window->InnerMainRect;
		float width = innerRect.Max.x - innerRect.Min.x;
		float height = innerRect.Max.y - innerRect.Min.y;

		auto transform_f2s = [&](const ImVec2& p) -> ImVec2
		{
			return ImVec2((p.x - offset_x) * scale_x + innerRect.Min.x, (-p.y - offset_y) * scale_y + innerRect.Min.y + height / 2);
		};

		auto transform_s2f = [&](const ImVec2& p) -> ImVec2
		{
			return ImVec2((p.x - innerRect.Min.x) / scale_x + offset_x, -((p.y - innerRect.Min.y - height / 2) / scale_y + offset_y));
		};

		auto moveFWithS = [&](const ImVec2& p, const ImVec2& d) -> ImVec2
		{
			auto sp = transform_f2s(p);
			sp += d;
			return transform_s2f(sp);
		};

		bool hasControlled = !canControl;

		if (isLocked)
		{
			hasControlled = true;
		}

		auto dx = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::DELTA_X, 0.0f);
		auto dy = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::DELTA_Y, 0.0f);
		auto over_y = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::OVER_Y, 0.0f);

		// move points
		if (!hasControlled && selected)
		{
			int32_t movedIndex = -1;

			for (int i = 0; i < count; i++)
			{
				PushID(i + 1);

				auto isChanged = false;
				float pointSize = 4;

				auto pos = transform_f2s(ImVec2(keys[i], values[i]));
				auto cursorPos = GetCursorPos();

				SetCursorScreenPos(ImVec2(pos.x - pointSize, pos.y - pointSize));

				InvisibleButton("", ImVec2(pointSize * 2, pointSize * 2));

				bool isDrawPositionRequired = false;

				if (IsItemHovered())
				{
					isDrawPositionRequired = true;
				}

				if (IsItemActive() && IsMouseClicked(0))
				{
					if (!GetIO().KeyShift)
					{
						for (int j = 0; j < count; j++)
						{
							kv_selected[j] = false;
						}
					}

					kv_selected[i] = !kv_selected[i];
				}

				if (IsItemActive() && IsMouseDragging(0))
				{
					movedIndex = i;
					hasControlled = true;
					isChanged = true;
					isDrawPositionRequired = true;
				}

				if (isDrawPositionRequired)
				{
					DrawMaker(window, pos, pointSize, 0xAAFFFFFF, 2);

					char text[255];
					sprintf(text, "(%.3f, %.3f)", keys[i], values[i]);
					window->DrawList->AddText(pos + ImVec2(20, 5), 0x55FFFFFF, text);
				}

				PopID();
				SetCursorScreenPos(cursorPos);

				// out of window
				if (isChanged && over_y != 0.0f)
				{
					dy = over_y * 0.1;
				}

				if (isChanged)
				{
					if (changedType != nullptr)
					{
						(*changedType) = 0;
					}

					break;
				}
			}

			// move points acctually
			if (movedIndex >= 0)
			{
				// calculate dx, dy on a field.
				auto fd = moveFWithS(ImVec2(0, 0), ImVec2(dx, dy));

				// make int
				auto mousePos = GetMousePos();
				auto f_mousePos = transform_s2f(mousePos);
				if (keys[movedIndex] <= f_mousePos.x && 0 < fd.x)
				{
					fd.x = 1;
				}
				else if (keys[movedIndex] >= f_mousePos.x && 0 > fd.x)
				{
					fd.x = -1;
				}
				else
				{
					fd.x = 0;
				}
				
				for (int i = 0; i < count; i++)
				{
					if (!kv_selected[i]) continue;

					auto pre_center_v = ImVec2(keys[i], values[i]);
					auto center_v = moveFWithS(ImVec2(keys[i], values[i]), ImVec2(dx, dy));
					center_v.y = std::max(std::min(center_v.y, v_max), v_min);

					keys[i] += fd.x;
					values[i] += fd.y;
					leftHandleKeys[i] += fd.x;
					leftHandleValues[i] += fd.y;
					rightHandleKeys[i] += fd.x;
					rightHandleValues[i] += fd.y;
				}

				if (movedX != nullptr) (*movedX) = fd.x;
				if (movedY != nullptr) (*movedY) = fd.y;

				ClampHandles(keys, values, leftHandleKeys, leftHandleValues, rightHandleKeys, rightHandleValues, count);

				// check values
				bool isChanged = true;

				while (isChanged)
				{
					isChanged = false;
					auto i = movedIndex;

					if (0 < i)
					{
						if (keys[i - 1] > keys[i])
						{
							std::swap(keys[i], keys[i - 1]);
							std::swap(values[i], values[i - 1]);
							std::swap(kv_selected[i], kv_selected[i - 1]);
							std::swap(leftHandleKeys[i], leftHandleKeys[i - 1]);
							std::swap(leftHandleValues[i], leftHandleValues[i - 1]);
							std::swap(rightHandleKeys[i], rightHandleKeys[i - 1]);
							std::swap(rightHandleValues[i], rightHandleValues[i - 1]);
							std::swap(interporations[i], interporations[i - 1]);

							// To change button
							PushID(i - 1 + 1);
							auto id = GetID("");
							PopID();

							SetActiveID(id, window);

							movedIndex = i - 1;
							isChanged = true;
						}
					}

					i = movedIndex;

					if (i + 1 < count)
					{
						if (keys[i] > keys[i + 1])
						{
							std::swap(keys[i], keys[i + 1]);
							std::swap(values[i], values[i + 1]);
							std::swap(kv_selected[i], kv_selected[i + 1]);
							std::swap(leftHandleKeys[i], leftHandleKeys[i + 1]);
							std::swap(leftHandleValues[i], leftHandleValues[i + 1]);
							std::swap(rightHandleKeys[i], rightHandleKeys[i + 1]);
							std::swap(rightHandleValues[i], rightHandleValues[i + 1]);
							std::swap(interporations[i], interporations[i + 1]);

							// To change button
							PushID(i + 1 + 1);
							auto id = GetID("");
							PopID();

							SetActiveID(id, window);

							movedIndex = i + 1;
							isChanged = true;
						}
					}
				}
			}
		}

		// move left handles
		if (!hasControlled && selected)
		{
			int32_t movedLIndex = -1;
			for (int i = 0; i < count; i++)
			{
				if (!kv_selected[i]) continue;

				auto isChanged = false;
				float pointSize = 4;

				auto pos = transform_f2s(ImVec2(leftHandleKeys[i], leftHandleValues[i]));
				auto cursorPos = GetCursorPos();

				SetCursorScreenPos(ImVec2(pos.x - pointSize, pos.y - pointSize));
				PushID(i + 0x1f0);

				InvisibleButton("", ImVec2(pointSize * 2, pointSize * 2));

				bool isDrawPositionRequired = false;

				if (IsItemHovered())
				{
					isDrawPositionRequired = true;
				}

				if (IsItemActive() && IsMouseClicked(0))
				{
				}

				if (IsItemActive() && IsMouseDragging(0))
				{
					isChanged = true;
					hasControlled = true;
					movedLIndex = i;
					isDrawPositionRequired = true;
				}

				if (isDrawPositionRequired)
				{
					DrawMaker(window, pos, pointSize, 0xAAFFFFFF, 2);

					char text[255];
					sprintf(text, "(%.3f, %.3f)", leftHandleKeys[i], leftHandleValues[i]);
					window->DrawList->AddText(pos + ImVec2(20, 5), 0x55FFFFFF, text);
				}

				PopID();
				SetCursorScreenPos(cursorPos);

				// out of window
				if (isChanged && over_y != 0.0f)
				{
					dy = over_y * 0.1;
				}

				if (isChanged)
				{
					if (changedType != nullptr)
					{
						(*changedType) = 1;

						if (movedX != nullptr) (*movedX) = dx / scale_x;
						if (movedY != nullptr) (*movedY) = -dy / scale_y;
					}

					break;
				}
			}

			if (movedLIndex >= 0)
			{
				for (int i = 0; i < count; i++)
				{
					if (!kv_selected[i]) continue;

					auto v = moveFWithS(ImVec2(leftHandleKeys[i], leftHandleValues[i]), ImVec2(dx, dy));
					leftHandleKeys[i] = v.x;
					leftHandleValues[i] = v.y;

					// movable area is limited
					if (0 < i)
					{
						leftHandleKeys[i] = std::max(leftHandleKeys[i], keys[i - 1]);
					}

					leftHandleKeys[i] = std::min(leftHandleKeys[i], keys[i]);
				}
			}
		}

		// move right handles
		if (!hasControlled && selected)
		{
			int32_t movedRIndex = -1;
			for (int i = 0; i < count; i++)
			{
				if (!kv_selected[i]) continue;

				auto isChanged = false;
				float pointSize = 4;

				auto pos = transform_f2s(ImVec2(rightHandleKeys[i], rightHandleValues[i]));
				auto cursorPos = GetCursorPos();

				SetCursorScreenPos(ImVec2(pos.x - pointSize, pos.y - pointSize));
				PushID(i + 0xaf0);

				InvisibleButton("", ImVec2(pointSize * 2, pointSize * 2));

				bool isDrawPositionRequired = false;

				if (IsItemHovered())
				{
					isDrawPositionRequired = true;
				}

				if (IsItemActive() && IsMouseClicked(0))
				{
				}

				if (IsItemActive() && IsMouseDragging(0))
				{
					isChanged = true;
					hasControlled = true;
					movedRIndex = i;
					isDrawPositionRequired = true;
				}

				if (isDrawPositionRequired)
				{
					DrawMaker(window, pos, pointSize, 0xAAFFFFFF, 2);

					char text[255];
					sprintf(text, "(%.3f, %.3f)", rightHandleKeys[i], rightHandleValues[i]);
					window->DrawList->AddText(pos + ImVec2(20, 5), 0x55FFFFFF, text);
				}

				PopID();
				SetCursorScreenPos(cursorPos);

				// out of window
				if (isChanged && over_y != 0.0f)
				{
					dy = over_y * 0.1;
				}

				if (isChanged)
				{
					if (changedType != nullptr)
					{
						(*changedType) = 2;

						if (movedX != nullptr) (*movedX) = dx / scale_x;
						if (movedY != nullptr) (*movedY) = -dy / scale_y;
					}

					break;
				}
			}

			if (movedRIndex >= 0)
			{
				for (int i = 0; i < count; i++)
				{
					if (!kv_selected[i]) continue;

					auto v = moveFWithS(ImVec2(rightHandleKeys[i], rightHandleValues[i]), ImVec2(dx, dy));
					rightHandleKeys[i] = v.x;
					rightHandleValues[i] = v.y;

					// movable area is limited
					if (i < count - 1)
					{
						rightHandleKeys[i] = std::min(rightHandleKeys[i], keys[i + 1]);
					}

					rightHandleKeys[i] = std::max(rightHandleKeys[i], keys[i]);
				}
			}
		}


		bool isLineHovered = false;
		{
			auto checkHovered = [&](float offset, bool isReversed) -> void
			{
				if (isReversed)
				{
					auto distance = keys[count - 1] - keys[0];
					offset += distance;

					for (int i = 0; i < count - 1; i++)
					{
						auto v1 = ImVec2(-keys[i + 0] + offset, values[i + 0]);
						auto v2 = ImVec2(-keys[i + 1] + offset, values[i + 1]);

						auto cp1 = ImVec2(-rightHandleKeys[i + 0] + offset, rightHandleValues[i + 0]);
						auto cp2 = ImVec2(-leftHandleKeys[i + 1] + offset, leftHandleValues[i + 1]);

						if (interporations[i] == ImFCurveInterporationType::Cubic)
						{
							isLineHovered = isLineHovered | IsHoveredOnBezierCurve(
								GetMousePos(), window,
								transform_f2s(v1),
								transform_f2s(cp1),
								transform_f2s(cp2),
								transform_f2s(v2),
								col,
								2);
						}
						else
						{
							isLineHovered = isLineHovered | IsHoveredOnLine(
								GetMousePos(), window,
								transform_f2s(v1),
								transform_f2s(v2),
								col,
								2);
						}
						
						if (isLineHovered) break;
					}
				}
				else
				{
					for (int i = 0; i < count - 1; i++)
					{
						auto v1 = ImVec2(keys[i + 0] + offset, values[i + 0]);
						auto v2 = ImVec2(keys[i + 1] + offset, values[i + 1]);

						auto cp1 = ImVec2(rightHandleKeys[i + 0] + offset, rightHandleValues[i + 0]);
						auto cp2 = ImVec2(leftHandleKeys[i + 1] + offset, leftHandleValues[i + 1]);

						if (interporations[i] == ImFCurveInterporationType::Cubic)
						{
							isLineHovered = isLineHovered | IsHoveredOnBezierCurve(
								GetMousePos(), window,
								transform_f2s(v1),
								transform_f2s(cp1),
								transform_f2s(cp2),
								transform_f2s(v2),
								col,
								2);
						}
						else
						{
							isLineHovered = isLineHovered | IsHoveredOnLine(
								GetMousePos(), window,
								transform_f2s(v1),
								transform_f2s(v2),
								col,
								2);
						}

						if (isLineHovered) break;
					}
				}
			};

			// start
			{
				auto v1 = ImVec2(keys[0], values[0]);
				auto v2 = ImVec2(transform_s2f(innerRect.Min).x, values[0]);

				if (startEdge == ImFCurveEdgeType::Constant || count == 1 || keys[0] == keys[count - 1])
				{
					isLineHovered = isLineHovered | IsHoveredOnLine(
						GetMousePos(), window,
						transform_f2s(v1),
						transform_f2s(v2),
						col,
						2);
				}
				else if (startEdge == ImFCurveEdgeType::Loop)
				{
					auto distance = keys[count - 1] - keys[0];
					while (v1.x > v2.x)
					{
						v1.x -= distance;
						checkHovered(v1.x - keys[0], startEdge == ImFCurveEdgeType::LoopInversely);
					}
				}
				else
				{
					auto distance = keys[count - 1] - keys[0];
					while (v1.x > v2.x)
					{
						v1.x -= distance;
						checkHovered(v1.x + keys[0], startEdge == ImFCurveEdgeType::LoopInversely);
					}
				}
			}

			// center
			checkHovered(0, false);

			// end
			{
				auto v1 = ImVec2(keys[count - 1], values[count - 1]);
				auto v2 = ImVec2(transform_s2f(innerRect.Max).x, values[count - 1]);

				if (endEdge == ImFCurveEdgeType::Constant || count == 1 || keys[0] == keys[count - 1])
				{
					isLineHovered = isLineHovered | IsHoveredOnLine(
						GetMousePos(), window,
						transform_f2s(v1),
						transform_f2s(v2),
						col,
						2);
				}
				else if (endEdge == ImFCurveEdgeType::Loop)
				{
					auto distance = keys[count - 1] - keys[0];
					while (v1.x < v2.x)
					{
						checkHovered(v1.x - keys[0], endEdge == ImFCurveEdgeType::LoopInversely);
						v1.x += distance;
					}
				}
				else
				{
					auto distance = keys[count - 1] - keys[0];
					while (v1.x < v2.x)
					{
						checkHovered(v1.x + keys[0], endEdge == ImFCurveEdgeType::LoopInversely);
						v1.x += distance;
					}
				}
			}
		}

		// remove point
		if (!hasControlled && selected && IsMouseDoubleClicked(0))
		{
			auto mousePos = GetMousePos();
			auto v = transform_s2f(mousePos);

			for (int i = 0; i < count; i++)
			{
				auto p = transform_f2s(ImVec2(keys[i], values[i]));

				if (!IsHovered(mousePos, p, 3)) continue;


				for (int j = i; j < count; j++)
				{
					keys[j] = keys[j + 1];
					values[j] = values[j + 1];
					leftHandleKeys[j] = leftHandleKeys[j + 1];
					leftHandleValues[j] = leftHandleValues[j + 1];
					rightHandleKeys[j] = rightHandleKeys[j + 1];
					rightHandleValues[j] = rightHandleValues[j + 1];
					kv_selected[j] = kv_selected[j + 1];
					interporations[j] = interporations[j + 1];
				}

				(*newCount) = count - 1;
				count = count - 1;
				hasControlled = true;
				break;

			}
		}

		// Add point
		if(!hasControlled && selected)
		{
			if (isLineHovered && IsMouseDoubleClicked(0))
			{
				auto mousePos = GetMousePos();
				auto v = transform_s2f(mousePos);

				// make int
				v.x = (int32_t)(v.x + 0.5);

				if (v.x < keys[0])
				{
					for (int j = count; j > 0; j--)
					{
						keys[j] = keys[j - 1];
						values[j] = values[j - 1];
						leftHandleKeys[j] = leftHandleKeys[j - 1];
						leftHandleValues[j] = leftHandleValues[j - 1];
						rightHandleKeys[j] = rightHandleKeys[j - 1];
						rightHandleValues[j] = rightHandleValues[j - 1];
						kv_selected[j] = kv_selected[j - 1];
						interporations[j] = interporations[j - 1];
					}

					keys[0] = v.x;
					values[0] = v.y;
					leftHandleKeys[0] = v.x;
					leftHandleValues[0] = v.y;
					rightHandleKeys[0] = v.x;
					rightHandleValues[0] = v.y;
					kv_selected[0] = false;
					interporations[0] = ImFCurveInterporationType::Linear;

					(*newCount) = count + 1;
					count = count + 1;
					hasControlled = true;
				}
				else if (v.x >= keys[count - 1])
				{
					keys[count] = v.x;
					values[count] = v.y;
					leftHandleKeys[count] = v.x;
					leftHandleValues[count] = v.y;
					rightHandleKeys[count] = v.x;
					rightHandleValues[count] = v.y;
					kv_selected[count] = false;
					interporations[count] = ImFCurveInterporationType::Linear;

					(*newCount) = count + 1;
					count = count + 1;
					hasControlled = true;
				}
				else
				{
					for (int i = 0; i < count - 1; i++)
					{
						if (keys[i] <= v.x && v.x < keys[i + 1])
						{
							for (int j = count; j > i; j--)
							{
								keys[j] = keys[j - 1];
								values[j] = values[j - 1];
								leftHandleKeys[j] = leftHandleKeys[j - 1];
								leftHandleValues[j] = leftHandleValues[j - 1];
								rightHandleKeys[j] = rightHandleKeys[j - 1];
								rightHandleValues[j] = rightHandleValues[j - 1];
								kv_selected[j] = kv_selected[j - 1];
								interporations[j] = interporations[j - 1];
							}

							keys[i + 1] = v.x;
							values[i + 1] = v.y;
							leftHandleKeys[i + 1] = v.x;
							leftHandleValues[i + 1] = v.y;
							rightHandleKeys[i + 1] = v.x;
							rightHandleValues[i + 1] = v.y;
							kv_selected[i + 1] = false;
							interporations[i + 1] = ImFCurveInterporationType::Linear;

							(*newCount) = count + 1;
							count = count + 1;
							hasControlled = true;
							break;
						}
					}
				}

				ClampHandles(keys, values, leftHandleKeys, leftHandleValues, rightHandleKeys, rightHandleValues, count);
			}
		}

		// is line selected
		if (newSelected != nullptr && !hasControlled && !isLocked && IsMouseClicked(0))
		{
			if (isLineHovered)
			{
				(*newSelected) = (*newSelected) | isLineHovered;
				hasControlled = true;
			}
		}

		auto lineThiness = 1;
		if (selected) lineThiness++;
		if (isLineHovered) lineThiness++;


		// render curve
		auto renderCurve = [&](float offset, bool isReversed) -> void
		{
			if (isReversed)
			{
				auto distance = keys[count - 1] - keys[0];
				offset += distance;

				for (int i = 0; i < count - 1; i++)
				{
					auto v1 = ImVec2(-keys[i + 0] + offset, values[i + 0]);
					auto v2 = ImVec2(-keys[i + 1] + offset, values[i + 1]);

					auto cp1 = ImVec2(-rightHandleKeys[i + 0] + offset, rightHandleValues[i + 0]);
					auto cp2 = ImVec2(-leftHandleKeys[i + 1] + offset, leftHandleValues[i + 1]);

					if (interporations[i] == ImFCurveInterporationType::Cubic)
					{
						window->DrawList->AddBezierCurve(
							transform_f2s(v1),
							transform_f2s(cp1),
							transform_f2s(cp2),
							transform_f2s(v2),
							col,
							lineThiness);
					}
					else
					{
						window->DrawList->AddLine(
							transform_f2s(v1),
							transform_f2s(v2),
							col,
							lineThiness);
					}
				}
			}
			else
			{
				for (int i = 0; i < count - 1; i++)
				{
					auto v1 = ImVec2(keys[i + 0] + offset, values[i + 0]);
					auto v2 = ImVec2(keys[i + 1] + offset, values[i + 1]);

					auto cp1 = ImVec2(rightHandleKeys[i + 0] + offset, rightHandleValues[i + 0]);
					auto cp2 = ImVec2(leftHandleKeys[i + 1] + offset, leftHandleValues[i + 1]);

					if (interporations[i] == ImFCurveInterporationType::Cubic)
					{
						window->DrawList->AddBezierCurve(
							transform_f2s(v1),
							transform_f2s(cp1),
							transform_f2s(cp2),
							transform_f2s(v2),
							col,
							lineThiness);
					}
					else
					{
						window->DrawList->AddLine(
							transform_f2s(v1),
							transform_f2s(v2),
							col,
							lineThiness);

					}
				}
			}
		};
		// start
		{
			auto v1 = ImVec2(keys[0], values[0]);
			auto v2 = ImVec2(transform_s2f(innerRect.Min).x, values[0]);

			if (startEdge == ImFCurveEdgeType::Constant || count == 1 || keys[0] == keys[count - 1])
			{
				window->DrawList->AddLine(
					transform_f2s(v1),
					transform_f2s(v2),
					col,
					lineThiness);
			}
			else if (startEdge == ImFCurveEdgeType::Loop)
			{
				auto distance = keys[count - 1] - keys[0];
				while (v1.x > v2.x)
				{
					v1.x -= distance;
					renderCurve(v1.x - keys[0], startEdge == ImFCurveEdgeType::LoopInversely);
				}
			}
			else
			{
				auto distance = keys[count - 1] - keys[0];
				while (v1.x > v2.x)
				{
					v1.x -= distance;
					renderCurve(v1.x + keys[0], startEdge == ImFCurveEdgeType::LoopInversely);
				}
			}
		}



		// center
		renderCurve(0, false);

		// end
		{
			auto v1 = ImVec2(keys[count - 1], values[count - 1]);
			auto v2 = ImVec2(transform_s2f(innerRect.Max).x, values[count - 1]);

			if (endEdge == ImFCurveEdgeType::Constant || count == 1 || keys[0] == keys[count - 1])
			{
				window->DrawList->AddLine(
					transform_f2s(v1),
					transform_f2s(v2),
					col,
					lineThiness);
			}
			else if(endEdge == ImFCurveEdgeType::Loop)
			{
				auto distance = keys[count - 1] - keys[0];
				while (v1.x < v2.x)
				{
					renderCurve(v1.x - keys[0], endEdge == ImFCurveEdgeType::LoopInversely);
					v1.x += distance;
				}
			}
			else
			{
				auto distance = keys[count - 1] - keys[0];
				while (v1.x < v2.x)
				{
					renderCurve(v1.x + keys[0], endEdge == ImFCurveEdgeType::LoopInversely);
					v1.x += distance;
				}
			}
		}

		// render points
		if (selected)
		{
			for (int i = 0; i < count; i++)
			{
				if (kv_selected[i])
				{
					int pointSize = 3;
					auto pos = transform_f2s(ImVec2(keys[i], values[i]));

					DrawMaker(window, pos, pointSize, 0xEEFFFFFF, 3);

					window->DrawList->AddLine(transform_f2s(ImVec2(leftHandleKeys[i], leftHandleValues[i])), pos, 0xEEFFFFFF);
					window->DrawList->AddLine(transform_f2s(ImVec2(rightHandleKeys[i], rightHandleValues[i])), pos, 0xEEFFFFFF);
				}
				else
				{
					int pointSize = 2;
					auto pos = transform_f2s(ImVec2(keys[i], values[i]));

					DrawMaker(window, pos, pointSize, 0xAAFFFFFF, 2);
				}
			}
		}

		PopID();

		return hasControlled;
	}
}