#include "fcurve.h"
#include "imgui_internal.h"

#include <stdint.h>
#include <algorithm>
#include <array>

namespace ImGui
{

enum FCurveContextState : int32_t
{
	None,
	SelectingWithArea,
	FinishedSelectingWithArea,
};

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
	SELECTING_AREA_START_X,
	SELECTING_AREA_START_Y,
	SELECTING_AREA_END_X,
	SELECTING_AREA_END_Y,
	STATE,
	CONTEXT_MENU_ORIGIN_X,
	CONTEXT_MENU_ORIGIN_Y,
};

class FCurveContext
{

public:
	FCurveContextState State;
	float AreaStartingX;
	float AreaStartingY;
	float AreaEndingX;
	float AreaEndingY;

	void Load(ImGuiWindow* window)
	{
		State = (FCurveContextState)(int)window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::STATE);
		AreaStartingX = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::SELECTING_AREA_START_X);
		AreaStartingY = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::SELECTING_AREA_START_Y);
		AreaEndingX = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::SELECTING_AREA_END_X);
		AreaEndingY = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::SELECTING_AREA_END_Y);

	}

	void Store(ImGuiWindow* window)
	{
		window->StateStorage.SetFloat((ImGuiID)FCurveStorageValues::STATE, (int)State);
		window->StateStorage.SetFloat((ImGuiID)FCurveStorageValues::SELECTING_AREA_START_X, AreaStartingX);
		window->StateStorage.SetFloat((ImGuiID)FCurveStorageValues::SELECTING_AREA_START_Y, AreaStartingY);
		window->StateStorage.SetFloat((ImGuiID)FCurveStorageValues::SELECTING_AREA_END_X, AreaEndingX);
		window->StateStorage.SetFloat((ImGuiID)FCurveStorageValues::SELECTING_AREA_END_Y, AreaEndingY);
	}

	void GetSelectingArea(ImVec2& begin, ImVec2& end)
	{
		float xBegin = AreaStartingX;
		float yBegin = AreaStartingY;
		float xEnd = AreaEndingX;
		float yEnd = AreaEndingY;

		if (xBegin > xEnd)
			std::swap(xBegin, xEnd);

		if (yBegin > yEnd)
			std::swap(yBegin, yEnd);

		begin = ImVec2(xBegin, yBegin);
		end = ImVec2(xEnd, yEnd);
	}
};

}

namespace ImGui
{
	const float pointRadiusSelected = 5;
	const float pointRadiusUnselected = 3;

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

	static void NormalizeKeyValues(const ImVec2& k1, ImVec2& k1rh, ImVec2& k2lh, const ImVec2& k2)
	{
		std::array<float, 2> h1;
		std::array<float, 2> h2;

		h1[0] = k1[0] - k1rh[0];
		h1[1] = k1[1] - k1rh[1];

		h2[0] = k2[0] - k2lh[0];
		h2[1] = k2[1] - k2lh[1];

		auto len = k2[0] - k1[0];

		auto lenR = std::abs(h1[0]);

		auto lenL = std::abs(h2[0]);

		if (lenR == 0 && lenL == 0)
			return;

		if ((lenR + lenL) > len)
		{
			auto f = len / (lenR + lenL);

			k1rh[0] = (k1[0] - f * h1[0]);
			k1rh[1] = (k1[1] - f * h1[1]);

			k2lh[0] = (k2[0] - f * h2[0]);
			k2lh[1] = (k2[1] - f * h2[1]);
		}
	}

	struct FieldScreenConverter
	{
		float offset_x_;
		float offset_y_;
		float scale_x_;
		float scale_y_;
		ImRect innerRect_;
		float width_;
		float height_;

	public:
		FieldScreenConverter()
		{
			ImGuiWindow* window = GetCurrentWindow();

			offset_x_ = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::OFFSET_X, 0.0f);
			offset_y_ = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::OFFSET_Y, 0.0f);

			scale_x_ = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::SCALE_X, 1.0f);
			scale_y_ = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::SCALE_Y, 1.0f);

			innerRect_ = window->InnerRect;
			width_ = innerRect_.Max.x - innerRect_.Min.x;
			height_ = innerRect_.Max.y - innerRect_.Min.y;
		}

		ImVec2 F2S(const ImVec2& p) const
		{
			return ImVec2((p.x - offset_x_) * scale_x_ + innerRect_.Min.x, (-p.y - offset_y_) * scale_y_ + innerRect_.Min.y + height_ / 2);
		}

		ImVec2 S2F(const ImVec2& p) const
		{
			return ImVec2((p.x - innerRect_.Min.x) / scale_x_ + offset_x_, -((p.y - innerRect_.Min.y - height_ / 2) / scale_y_ + offset_y_));
		}
	};


	bool IsHovered(const ImVec2& v1, const ImVec2& v2, float radius)
	{
		ImVec2 diff;
		diff.x = v1.x - v2.x;
		diff.y = v1.y - v2.y;
		return diff.x * diff.x + diff.y * diff.y < radius * radius;
	}

	bool IsHoveredOnLine(const ImVec2& v, ImGuiWindow* window, const ImVec2& a, const ImVec2& b, ImU32 col, float thickness, ImVec2* tangent = nullptr)
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

			const float eps = 0.000001f;
			if (ImLength(p1) < eps)
				continue;

			auto n = ImNorm(p1);
			auto component1 = n * ImDot(n, p0);
			auto component2 = p0 - component1;

			if (ImDot(component1, p1) < 0) continue;
			if (ImLength(component1) > ImLength(p1)) continue;
			if (ImLength(component2) > thickness) continue;
			isHovered = true;
			if (tangent) *tangent = v - component2 - ImVec2(0.5f, 0.5f);
			break;
		}
	
		window->DrawList->PathClear();

		return isHovered;
	}

	bool IsHoveredOnBezierCurve(const ImVec2& v, ImGuiWindow* window, const ImVec2& pos0, const ImVec2& cp0, const ImVec2& cp1, const ImVec2& pos1, ImU32 col, float thickness, ImVec2* tangent = nullptr)
	{
		if ((col & IM_COL32_A_MASK) == 0)
			return false;

		auto cp0m = cp0;
		auto cp1m = cp1;
		NormalizeKeyValues(pos0, cp0m, cp1m, pos1);

		window->DrawList->PathLineTo(pos0);
		window->DrawList->PathBezierCurveTo(cp0m, cp1m, pos1, 0);

		bool isHovered = false;

		for (int32_t i = 0; i < window->DrawList->_Path.size() - 1; i++)
		{
			auto v1 = window->DrawList->_Path[i + 0];
			auto v2 = window->DrawList->_Path[i + 1];

			auto p0 = v - v1;
			auto p1 = v2 - v1;

			const float eps = 0.000001f;
			if (ImLength(p1) < eps)
				continue;

			auto n = ImNorm(p1);
			auto component1 = n * ImDot(n, p0);
			auto component2 = p0 - component1;

			if (ImDot(component1, p1) < 0) continue;
			if (ImLength(component1) > ImLength(p1)) continue;
			if (ImLength(component2) > thickness) continue;
			isHovered = true;
			if (tangent) *tangent = v - component2;
			break;
		}

		window->DrawList->PathClear();

		return isHovered;
	}

	bool IsHoveredOnFCurve(
		float* keys, float* values,
		float* leftHandleKeys, float* leftHandleValues,
		float* rightHandleKeys, float* rightHandleValues,
		ImFCurveInterporationType* interporations,
		ImFCurveEdgeType startEdge,
		ImFCurveEdgeType endEdge,
		ImU32 col, int count, ImVec2* tangent)
	{
		ImGuiWindow* window = GetCurrentWindow();

		float offset_x = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::OFFSET_X, 0.0f);
		float offset_y = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::OFFSET_Y, 0.0f);

		float scale_x = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::SCALE_X, 1.0f);
		float scale_y = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::SCALE_Y, 1.0f);

		const ImRect innerRect = window->InnerRect;
		float width = innerRect.Max.x - innerRect.Min.x;
		float height = innerRect.Max.y - innerRect.Min.y;

		if (!innerRect.Contains(GetMousePos()))
		{
			return false;
		}

		auto transform_f2s = [&](const ImVec2& p) -> ImVec2
		{
			return ImVec2((p.x - offset_x) * scale_x + innerRect.Min.x, (-p.y - offset_y) * scale_y + innerRect.Min.y + height / 2);
		};

		auto transform_s2f = [&](const ImVec2& p) -> ImVec2
		{
			return ImVec2((p.x - innerRect.Min.x) / scale_x + offset_x, -((p.y - innerRect.Min.y - height / 2) / scale_y + offset_y));
		};

		bool isLineHovered = false;

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
						isLineHovered = isLineHovered || IsHoveredOnBezierCurve(
							GetMousePos(), window,
							transform_f2s(v1),
							transform_f2s(cp1),
							transform_f2s(cp2),
							transform_f2s(v2),
							col, 2, tangent);
					}
					else
					{
						isLineHovered = isLineHovered || IsHoveredOnLine(
							GetMousePos(), window,
							transform_f2s(v1),
							transform_f2s(v2),
							col, 2, tangent);
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
						isLineHovered = isLineHovered || IsHoveredOnBezierCurve(
							GetMousePos(), window,
							transform_f2s(v1),
							transform_f2s(cp1),
							transform_f2s(cp2),
							transform_f2s(v2),
							col, 2, tangent);
					}
					else
					{
						isLineHovered = isLineHovered || IsHoveredOnLine(
							GetMousePos(), window,
							transform_f2s(v1),
							transform_f2s(v2),
							col, 2, tangent);
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
				isLineHovered = isLineHovered || IsHoveredOnLine(
					GetMousePos(), window,
					transform_f2s(v1),
					transform_f2s(v2),
					col, 2, tangent);
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
				isLineHovered = isLineHovered || IsHoveredOnLine(
					GetMousePos(), window,
					transform_f2s(v1),
					transform_f2s(v2),
					col, 2, tangent);
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

		return isLineHovered;
	}

	bool IsHoveredOnFCurvePoint(const float* keys, const float* values, int count, int* hovered)
	{
		assert(hovered != nullptr);

		const FieldScreenConverter fsc;
		*hovered = -1;

		for (int i = 0; i < count; i++)
		{
			PushID(i + 1);

			auto isChanged = false;
			const float pointSize = pointRadiusSelected;

			auto pos = fsc.F2S(ImVec2(keys[i], values[i]));
			auto cursorPos = GetCursorPos();

			ImVec2 p(pos.x - pointSize, pos.y - pointSize);
			SetCursorScreenPos(p);

			InvisibleButton("", ImVec2(pointSize * 2, pointSize * 2));

			bool isDrawPositionRequired = false;

			bool isActive = IsItemActive();
			bool itemHovered = IsItemHovered();

			if (itemHovered)
			{
				*hovered = i;
			}

			PopID();
		}

		return *hovered >= 0;
	}

	void DrawMaker(ImGuiWindow* window, ImVec2 pos, float size, uint32_t color, int32_t thickness)
	{
		window->DrawList->AddCircleFilled(pos, size + thickness, 0xAA000000, 8);
		window->DrawList->AddCircleFilled(pos, size, color, 8);
	}

	void ClampHandles(
		float* keys, float* values,
		float* leftHandleKeys, float* leftHandleValues,
		float* rightHandleKeys, float* rightHandleValues,
		int count)
	{
		return;
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

	bool IsFCurvePanning()
	{
		ImGuiWindow* window = GetCurrentWindow();
		return window->StateStorage.GetBool((ImGuiID)FCurveStorageValues::IS_PANNING, false);
	}

	ImVec2 GetCurrentFCurveFieldPosition()
	{
		const FieldScreenConverter fsc;
		auto cursorPos = GetMousePos();
		return fsc.S2F(cursorPos);
	}

	bool BeginFCurve(int id, const ImVec2& size, float current, const ImVec2& scale, const ImVec2& min_kv, const ImVec2& max_kv)
	{
		const bool isValueAutoZoomMode = min_kv.y <= max_kv.y;
		const bool isKeyAutoZoomMode = min_kv.x <= max_kv.x;
		const auto isAutoZoomMode = isKeyAutoZoomMode || isValueAutoZoomMode;

		if (!BeginChildFrame(id, size, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
		{
			return false;
		}

		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
		{
			return false;
		}

		FCurveContext context;
		context.Load(window);

		bool isGridShown = true;
		float offset_x = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::OFFSET_X, 0.0f);
		float offset_y = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::OFFSET_Y, 0.0f);

		float scale_x = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::SCALE_X, scale.x);
		float scale_y = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::SCALE_Y, scale.y);
		window->StateStorage.SetFloat((ImGuiID)FCurveStorageValues::SCALE_X, scale_x);
		window->StateStorage.SetFloat((ImGuiID)FCurveStorageValues::SCALE_Y, scale_y);

		const ImRect innerRect = window->InnerRect;
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
		if (ImGui::GetIO().MouseWheel != 0 && ImGui::IsWindowHovered() && ImGui::GetIO().KeyAlt && !isAutoZoomMode)
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
		if (isKeyAutoZoomMode)
		{
			auto max_value = max_kv.x;
			auto min_value = min_kv.x;

			auto range = max_value - min_value;

			if (range == 0.0f)
			{
				max_value = +width / 2;
				min_value = -width / 2;
				range = width;
				scale_x = scale.x;
			}
			else
			{
				scale_x = width / (range);
			}

			offset_x = (max_value + min_value) / 2.0f - range / 2;
			window->StateStorage.SetFloat((ImGuiID)FCurveStorageValues::SCALE_X, scale_x);
			window->StateStorage.SetFloat((ImGuiID)FCurveStorageValues::OFFSET_X, offset_x);
		}

		if (isValueAutoZoomMode)
		{
			auto max_value = max_kv.y;
			auto min_value = min_kv.y;

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
		if (window->StateStorage.GetBool((ImGuiID)FCurveStorageValues::IS_PANNING, false))
		{
			ImVec2 drag_offset = ImGui::GetMouseDragDelta(1);
			offset_x = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::START_X) - drag_offset.x / scale_x;

			if (!isValueAutoZoomMode)
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

			fieldGridSizeX  = pow(5, (int32_t)round(logn(5, fieldGridSizeX)));
			fieldGridSizeY = pow(5, (int32_t)round(logn(5, fieldGridSizeY)));

			auto sx = (int)(upperLeft_f.x / fieldGridSizeX) * fieldGridSizeX;
			auto sy = (int)(upperLeft_f.y / fieldGridSizeY) * fieldGridSizeY;
			auto ex = (int)(lowerRight_f.x / fieldGridSizeX) * fieldGridSizeX + fieldGridSizeX;
			auto ey = (int)(lowerRight_f.y / fieldGridSizeY) * fieldGridSizeY - fieldGridSizeY;

			for (auto y = sy + fieldGridSizeY; y >= ey; y -= fieldGridSizeY)
			{
				for (int i = 1; i < 5; i++)
				{
					window->DrawList->AddLine(transform_f2s(ImVec2(upperLeft_f.x, y - fieldGridSizeY / 5.0f * i)), transform_f2s(ImVec2(lowerRight_f.x, y - fieldGridSizeY / 5.0f * i)), 0x25000000);
				}
				window->DrawList->AddLine(transform_f2s(ImVec2(upperLeft_f.x, y)), transform_f2s(ImVec2(lowerRight_f.x, y)), 0x55000000);
			}

			for (auto x = sx - fieldGridSizeX; x <= ex; x += fieldGridSizeX)
			{
				for (int i = 1; i < 5; i++)
				{
					window->DrawList->AddLine(transform_f2s(ImVec2(x + fieldGridSizeX / 5.0f * i, upperLeft_f.y)), transform_f2s(ImVec2(x + fieldGridSizeX / 5.0f * i, lowerRight_f.y)), 0x25000000);
				}
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

		if (context.State == FCurveContextState::SelectingWithArea)
		{
			auto fpos = transform_s2f(ImGui::GetMousePos());
			context.AreaEndingX = fpos.x;
			context.AreaEndingY = fpos.y;

			if (ImGui::IsMouseReleased(0))
			{
				context.State = FCurveContextState::FinishedSelectingWithArea;
			}
			else
			{
				// draw area
				ImVec2 begin;
				ImVec2 end;
				context.GetSelectingArea(begin, end);

				begin = transform_f2s(begin);
				end = transform_f2s(end);

				window->DrawList->AddRectFilled(begin, end, ImColor(220, 220, 220, 32));
				window->DrawList->AddRect(begin, end, ImColor(220, 220, 220, 255));
			}
		}

		context.Store(window);

		return true;
	}

	void EndFCurve()
	{
		ImGuiWindow* window = GetCurrentWindow();

		if (ImGui::IsMouseReleased(1))
		{
			window->StateStorage.SetBool((ImGuiID)FCurveStorageValues::IS_PANNING, false);
		}

		FCurveContext context;
		context.Load(window);

		if (context.State == FCurveContextState::FinishedSelectingWithArea)
		{
			context.State = FCurveContextState::None;
		}

		context.Store(window);

		EndChildFrame();
	}

	bool StartSelectingAreaFCurve()
	{
		ImGuiWindow* window = GetCurrentWindow();

		if (!IsMouseDragging(0))
		{	
			return false;
		}

		FCurveContext context;
		context.Load(window);

		if (context.State == FCurveContextState::SelectingWithArea)
		{
			return true;
		}

		float offset_x = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::OFFSET_X, 0.0f);
		float offset_y = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::OFFSET_Y, 0.0f);

		float scale_x = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::SCALE_X, 1.0f);
		float scale_y = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::SCALE_Y, 1.0f);

		const ImRect innerRect = window->InnerRect;
		float width = innerRect.Max.x - innerRect.Min.x;
		float height = innerRect.Max.y - innerRect.Min.y;

		if (!innerRect.Contains(ImGui::GetMousePos()))
		{
			return false;
		}

		auto transform_f2s = [&](const ImVec2& p) -> ImVec2 {
			return ImVec2((p.x - offset_x) * scale_x + innerRect.Min.x, (-p.y - offset_y) * scale_y + innerRect.Min.y + height / 2);
		};

		auto transform_s2f = [&](const ImVec2& p) -> ImVec2 {
			return ImVec2((p.x - innerRect.Min.x) / scale_x + offset_x, -((p.y - innerRect.Min.y - height / 2) / scale_y + offset_y));
		};

		auto fpos = transform_s2f(ImGui::GetMousePos());

		context.State = FCurveContextState::SelectingWithArea;
		context.AreaStartingX = fpos.x;
		context.AreaStartingY = fpos.y;
		context.Store(window);
		return true;
	}

	bool AddFCurvePoint(ImVec2 v,
		float* keys, float* values,
		float* leftHandleKeys, float* leftHandleValues,
		float* rightHandleKeys, float* rightHandleValues,
		ImFCurveInterporationType* interporations,
		bool* kv_selected, int count, int* newCount)
	{
		bool result = false;

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

			const auto ctrlLength = (keys[1] - v.x) / 2.0f;
			keys[0] = v.x;
			values[0] = v.y;
			leftHandleKeys[0] = v.x - ctrlLength;
			leftHandleValues[0] = v.y;
			rightHandleKeys[0] = v.x + ctrlLength;
			rightHandleValues[0] = v.y;
			kv_selected[0] = false;
			interporations[0] = ImFCurveInterporationType::Cubic;

			(*newCount) = count + 1;
			result = true;
		}
		else if (v.x >= keys[count - 1])
		{
			const auto ctrlLength = (v.x - keys[count - 1]) / 2.0f;
			keys[count] = v.x;
			values[count] = v.y;
			leftHandleKeys[count] = v.x - ctrlLength;
			leftHandleValues[count] = v.y;
			rightHandleKeys[count] = v.x + ctrlLength;
			rightHandleValues[count] = v.y;
			kv_selected[count] = false;
			interporations[count] = ImFCurveInterporationType::Cubic;

			(*newCount) = count + 1;
			result = true;
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

					const auto leftCtrlLength = (keys[i + 1] - keys[i]) / 2.0f;
					const auto rightCtrlLength = (keys[i + 2] - keys[i + 1]) / 2.0f;

					keys[i + 1] = v.x;
					values[i + 1] = v.y;
					leftHandleKeys[i + 1] = v.x - leftCtrlLength;
					leftHandleValues[i + 1] = v.y;
					rightHandleKeys[i + 1] = v.x + rightCtrlLength;
					rightHandleValues[i + 1] = v.y;
					kv_selected[i + 1] = false;
					interporations[i + 1] = ImFCurveInterporationType::Cubic;

					(*newCount) = count + 1;
					result = true;
					break;
				}
			}
		}

		ClampHandles(keys, values, leftHandleKeys, leftHandleValues, rightHandleKeys, rightHandleValues, count);

		return result;
	}

	bool RemoveFCurvePoint(ImVec2 v,
		float* keys, float* values,
		float* leftHandleKeys, float* leftHandleValues,
		float* rightHandleKeys, float* rightHandleValues,
		ImFCurveInterporationType* interporations,
		bool* kv_selected, int count, int* newCount)
	{
		bool result = false;

		for (int i = 0; i < count; i++)
		{
			if (IsHovered(v, ImVec2(keys[i], values[i]), 3))
			{
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
				return true;
			}
		}
		return false;
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

		const ImRect innerRect = window->InnerRect;
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

		ImVec2 lineHoveredPos = {};
		bool isLineHovered = false;
		{
			auto v1 = ImVec2(transform_s2f(innerRect.Min).x, defaultValue);
			auto v2 = ImVec2(transform_s2f(innerRect.Max).x, defaultValue);

			isLineHovered = isLineHovered || IsHoveredOnLine(
				GetMousePos(), window,
				transform_f2s(v1),
				transform_f2s(v2),
				col, 2, &lineHoveredPos);
			lineHoveredPos = transform_s2f(lineHoveredPos);
		}

		// Add point
		if (!hasControlled && selected)
		{
			if (isLineHovered && IsMouseDoubleClicked(0))
			{
				auto mousePos = GetMousePos();
				auto v = transform_s2f(mousePos);

				keys[0] = v.x;
				values[0] = defaultValue;
				leftHandleKeys[0] = v.x - 10;
				leftHandleValues[0] = v.y;
				rightHandleKeys[0] = v.x + 10;
				rightHandleValues[0] = v.y;
				kv_selected[0] = false;
				interporations[0] = ImFCurveInterporationType::Cubic;

				(*newCount) = count + 1;
				hasControlled = true;
			}
		}

		// is line selected
		if (newSelected != nullptr && isLineHovered && 
			!hasControlled && !isLocked && IsMouseClicked(0))
		{
			(*newSelected) = true;
			hasControlled = true;
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

		const ImRect innerRect = window->InnerRect;
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

		FCurveContext context;
		context.Load(window);

		if (context.State == FCurveContextState::SelectingWithArea)
		{
			hasControlled = true;
		}
		else if (context.State == FCurveContextState::FinishedSelectingWithArea && selected)
		{
			hasControlled = true;
			
			// select area
			ImVec2 begin;
			ImVec2 end;
			context.GetSelectingArea(begin, end);

			if (!GetIO().KeyCtrl && !GetIO().KeyAlt)
			{
				for (int j = 0; j < count; j++)
				{
					kv_selected[j] = false;
				}
			}

			for (int i = 0; i < count; i++)
			{
				if (begin.x <= keys[i] && keys[i] <= end.x && begin.y <= values[i] && values[i] <= end.y)
				{
					kv_selected[i] = true;
				}
			}

		}

		auto dx = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::DELTA_X, 0.0f);
		auto dy = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::DELTA_Y, 0.0f);
		auto over_y = window->StateStorage.GetFloat((ImGuiID)FCurveStorageValues::OVER_Y, 0.0f);
		
		bool isOnPoint = false;
		int hoveredPoint = -1;

		// move points
		if (!hasControlled && selected)
		{
			int32_t movedIndex = -1;

			for (int i = 0; i < count; i++)
			{
				PushID(i + 1);

				auto isChanged = false;
				const float pointSize = pointRadiusSelected;

				auto pos = transform_f2s(ImVec2(keys[i], values[i]));
				auto cursorPos = GetCursorPos();

				ImVec2 p(pos.x - pointSize, pos.y - pointSize);
				SetCursorScreenPos(p);

				InvisibleButton("", ImVec2(pointSize * 2, pointSize * 2));

				bool isDrawPositionRequired = false;

				bool isActive = IsItemActive();
				bool itemHovered = IsItemHovered();

				if (itemHovered)
				{
					isDrawPositionRequired = true;
					isOnPoint = true;
					hoveredPoint = i;
				}

				if (itemHovered && IsMouseClicked(0))
				{
					if (!GetIO().KeyShift && !GetIO().KeyAlt)
					{
						for (int j = 0; j < count; j++)
						{
							kv_selected[j] = false;
						}
					}

					if (GetIO().KeyAlt)
					{
						kv_selected[i] = true;
					}
					else
					{
						kv_selected[i] = !kv_selected[i];
					}

					hasControlled = true;
				}

				// Remove
				if (itemHovered && IsMouseDoubleClicked(0))
				{
					RemoveFCurvePoint(transform_s2f(p), keys, values, 
						leftHandleKeys, leftHandleValues, rightHandleKeys, rightHandleValues,
						interporations, kv_selected, count, newCount);
					count = *newCount;
				}

				if (isActive && IsMouseClicked(0))
				{
				}

				if (isActive && IsMouseDragging(0))
				{
					movedIndex = i;
					hasControlled = true;
					isChanged = true;
					isDrawPositionRequired = true;
				}

				if (isDrawPositionRequired)
				{
					//DrawMaker(window, pos, pointSize, 0xAAFFFFFF, 2);

					char text[255];
					sprintf(text, "(%.3f, %.3f)", keys[i], values[i]);
					window->DrawList->AddText(pos + ImVec2(0, -5), 0x55FFFFFF, text);
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
			if (movedIndex >= 0 && kv_selected[movedIndex])
			{
				// calculate dx, dy on a field.
				auto fd = moveFWithS(ImVec2(0, 0), ImVec2(dx, dy));

				// make int
				auto mousePos = GetMousePos();
				auto f_mousePos = transform_s2f(mousePos);
				if (keys[movedIndex] + 0.5f <= f_mousePos.x && 0 < fd.x)
				{
					fd.x = (int)(f_mousePos.x - keys[movedIndex]) + 1;
				}
				else if (keys[movedIndex] - 0.5f >= f_mousePos.x && 0 > fd.x)
				{
					fd.x = (int)(f_mousePos.x - keys[movedIndex]) - 1;
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

				// Sort points
				for (int i = 1; i < count; i++)
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

						if (i - 1 == movedIndex)
						{
							// To change button
							const auto activeIdMouseButton = ImGui::GetCurrentContext()->ActiveIdMouseButton;

							PushID(i + 1);
							auto id = GetID("");
							PopID();

							SetActiveID(id, window);

							ImGui::GetCurrentContext()->ActiveIdMouseButton = activeIdMouseButton;

							movedIndex = i + 1;
						}
						else if (i == movedIndex)
						{
							// To change button
							const auto activeIdMouseButton = ImGui::GetCurrentContext()->ActiveIdMouseButton;

							PushID(i);
							auto id = GetID("");
							PopID();

							SetActiveID(id, window);

							ImGui::GetCurrentContext()->ActiveIdMouseButton = activeIdMouseButton;

							movedIndex = i - 1;
						}
					}
				}

				ClampHandles(keys, values, leftHandleKeys, leftHandleValues, rightHandleKeys, rightHandleValues, count);
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
				float pointSize = pointRadiusSelected;

				auto pos = transform_f2s(ImVec2(leftHandleKeys[i], leftHandleValues[i]));
				auto cursorPos = GetCursorPos();

				SetCursorScreenPos(ImVec2(pos.x - pointSize, pos.y - pointSize));
				PushID(i + 0x1f0);

				InvisibleButton("", ImVec2(pointSize * 2, pointSize * 2));

				bool isDrawPositionRequired = false;

				if (IsItemHovered())
				{
					isDrawPositionRequired = true;
					isOnPoint = true;
				}

				if (IsItemActive() && IsMouseClicked(0))
				{
					hasControlled = true;
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
					DrawMaker(window, pos, pointSize, 0xAAFFFFFF, 1);

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
				float pointSize = pointRadiusSelected;

				auto pos = transform_f2s(ImVec2(rightHandleKeys[i], rightHandleValues[i]));
				auto cursorPos = GetCursorPos();

				SetCursorScreenPos(ImVec2(pos.x - pointSize, pos.y - pointSize));
				PushID(i + 0xaf0);

				InvisibleButton("", ImVec2(pointSize * 2, pointSize * 2));

				bool isDrawPositionRequired = false;

				if (IsItemHovered())
				{
					isDrawPositionRequired = true;
					isOnPoint = true;
				}

				if (IsItemActive() && IsMouseClicked(0))
				{
					hasControlled = true;
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
					DrawMaker(window, pos, pointSize, 0xAAFFFFFF, 1);

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

		ImVec2 lineHoveredPos = {};
		bool isLineHovered = !isOnPoint && IsHoveredOnFCurve(keys, values, 
			leftHandleKeys, leftHandleValues, 
			rightHandleKeys, rightHandleValues, 
			interporations, startEdge, endEdge, col, count, &lineHoveredPos);
		lineHoveredPos = transform_s2f(lineHoveredPos);

		auto lineThiness = 1.5f;
		if (selected) lineThiness++;
		if (isLineHovered) lineThiness++;

		// Add point
		if (!hasControlled && selected && !isOnPoint)
		{
			if (isLineHovered && IsMouseDoubleClicked(0))
			{
				hasControlled = AddFCurvePoint(lineHoveredPos, 
					keys, values, leftHandleKeys, leftHandleValues, 
					rightHandleKeys, rightHandleValues, interporations,
					kv_selected, count, newCount);
				count = *newCount;
			}
		}

		// is line selected
		if (newSelected != nullptr && isLineHovered && 
			!hasControlled && !isLocked && !isOnPoint && IsMouseClicked(0))
		{
			(*newSelected) = true;
			hasControlled = true;

			for (int i = 0; i < count; i++)
			{
				kv_selected[i] = false;
			}
		}

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
						auto v1s = transform_f2s(v1);
						auto cp1s = transform_f2s(cp1);
						auto cp2s = transform_f2s(cp2);
						auto v2s = transform_f2s(v2);
						NormalizeKeyValues(v1s, cp1s, cp2s, v2s);

						window->DrawList->AddBezierCurve(
							v1s,
							cp1s,
							cp2s,
							v2s,
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
						auto v1s = transform_f2s(v1);
						auto cp1s = transform_f2s(cp1);
						auto cp2s = transform_f2s(cp2);
						auto v2s = transform_f2s(v2);
						NormalizeKeyValues(v1s, cp1s, cp2s, v2s);

						window->DrawList->AddBezierCurve(
							v1s,
							cp1s,
							cp2s,
							v2s,
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
					const float pointSize = pointRadiusSelected;
					const float thickness = 3 + ((i == hoveredPoint) ? 1 : 0);
					auto pos = transform_f2s(ImVec2(keys[i], values[i]));
					DrawMaker(window, pos, pointSize, 0xEEFFFFFF, thickness);

					window->DrawList->AddLine(transform_f2s(ImVec2(leftHandleKeys[i], leftHandleValues[i])), pos, 0xEEFFFFFF);
					window->DrawList->AddLine(transform_f2s(ImVec2(rightHandleKeys[i], rightHandleValues[i])), pos, 0xEEFFFFFF);
				}
				else
				{
					const float pointSize = pointRadiusUnselected;
					const float thickness = 2 + ((i == hoveredPoint) ? 1 : 0);
					auto pos = transform_f2s(ImVec2(keys[i], values[i]));
					DrawMaker(window, pos, pointSize, 0xCCFFFFFF, thickness);
				}
			}
		}

		PopID();

		return hasControlled;
	}
}