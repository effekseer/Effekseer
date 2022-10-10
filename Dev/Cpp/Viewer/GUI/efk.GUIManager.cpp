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

#include "Image.h"
#include "NodeFrameTimeline.h"
#include "efk.GUIManager.h"

#include "../EditorCommon/GUI/JapaneseFont.h"

#include "../3rdParty/imgui_addon/fcurve/fcurve.h"
#include "../3rdParty/imgui_addon/implot/implot.h"

#include "../3rdParty/Boxer/boxer.h"

#include <GUI/Misc.h>

#ifdef __linux__
#include <gtk/gtk.h>
#endif

#include <Common/StringHelper.h>

#include "GradientHDRState.h"

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
	const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered
																					  : ImGuiCol_Button);
	RenderNavHighlight(bb, id);
	RenderFrame(bb.Min, bb.Max, col, true, ImClamp((float)ImMin(padding.x, padding.y), 0.0f, style.FrameRounding));
	if (bg_col.w > 0.0f)
		window->DrawList->AddRectFilled(image_bb.Min, image_bb.Max, GetColorU32(bg_col));
	window->DrawList->AddImage(user_texture_id, image_bb.Min, image_bb.Max, uv0, uv1, GetColorU32(tint_col));

	return pressed;
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
	auto weight = [](float d) -> float
	{
		const float a = -1.0f;
		return d <= 1.0f   ? ((a + 2.0f) * d * d * d) - ((a + 3.0f) * d * d) + 1
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
		data[0] = 0;
		if (u16str)
		{
			Effekseer::Tool::StringHelper::ConvertUtf16ToUtf8(data, SIZE, u16str);
		}
	}
	operator const char*() const
	{
		return data;
	}
};

static ImTextureID ToImTextureID(std::shared_ptr<Effekseer::Tool::Image> image)
{
	if (image != nullptr)
	{
		auto texture = image->GetTexture();
		if (texture != nullptr)
		{
#ifdef _WIN32
			auto t_dx11 = dynamic_cast<EffekseerRendererDX11::Backend::Texture*>(texture.Get());
			if (t_dx11 != nullptr)
			{
				return reinterpret_cast<ImTextureID>(t_dx11->GetSRV());
			}
#endif
			auto t_gl = dynamic_cast<EffekseerRendererGL::Backend::Texture*>(texture.Get());
			if (t_gl != nullptr)
			{
				auto buffer = t_gl->GetBuffer();
				GLint bound;
				glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound);
				glBindTexture(GL_TEXTURE_2D, buffer);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glBindTexture(GL_TEXTURE_2D, bound);

				return reinterpret_cast<ImTextureID>(static_cast<size_t>(buffer));
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
	auto url16 = Effekseer::Tool::StringHelper::ConvertUtf8ToUtf16(url);

	auto self = reinterpret_cast<GUIManager*>(data.userData);

	self->callback->ClickLink(url16.c_str());
}

GUIManager::GUIManager()
{
}

GUIManager::~GUIManager()
{
}

bool GUIManager::Initialize(std::shared_ptr<Effekseer::MainWindow> mainWindow, Effekseer::Tool::DeviceType deviceType)
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
			this->callback->SetPath(Effekseer::Tool::StringHelper::ConvertUtf8ToUtf16(path).c_str());
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

	window->Iconify = [this](int f) -> void
	{
		if (this->callback != nullptr)
		{
			this->callback->Iconify(f);
		}
	};

	window->DpiChanged = [this](float scale) -> void
	{
		this->ResetGUIStyle();

		if (this->callback != nullptr)
		{
			this->callback->DpiChanged(scale);
		}
	};

	if (deviceType == Effekseer::Tool::DeviceType::OpenGL)
	{
		window->MakeCurrent();

#ifdef _WIN32
		glewInit();
#endif
	}

	return true;
}

void GUIManager::InitializeGUI(std::shared_ptr<Effekseer::Tool::GraphicsDevice> graphicsDevice)
{
	ImGui::CreateContext();
	ImGui::GetCurrentContext()->PlatformLocaleDecimalPoint = *localeconv()->decimal_point;

	ImGuiIO& io = ImGui::GetIO();

	if (deviceType == Effekseer::Tool::DeviceType::OpenGL)
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
	else if (deviceType == Effekseer::Tool::DeviceType::DirectX11)
	{
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui_ImplGlfw_InitForVulkan(window->GetGLFWWindows(), true);

		auto gd = graphicsDevice->GetGraphics()->GetGraphicsDevice().DownCast<EffekseerRendererDX11::Backend::GraphicsDevice>();
		ImGui_ImplDX11_Init(gd->GetDevice(), gd->GetContext());
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

	ImPlot::CreateContext();
	ImPlot::GetStyle().AntiAliasedLines = true;
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
	ImPlot::DestroyContext();

	if (deviceType == Effekseer::Tool::DeviceType::OpenGL)
	{
		ImGui_ImplOpenGL3_Shutdown();
	}
#ifdef _WIN32
	else if (deviceType == Effekseer::Tool::DeviceType::DirectX11)
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
	if (deviceType == Effekseer::Tool::DeviceType::OpenGL)
	{
		ImGui_ImplOpenGL3_DestroyFontsTexture();
	}
#if _WIN32
	else if (deviceType == Effekseer::Tool::DeviceType::DirectX11)
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
	if (deviceType == Effekseer::Tool::DeviceType::OpenGL)
	{
		ImGui_ImplOpenGL3_NewFrame();
	}
#if _WIN32
	else if (deviceType == Effekseer::Tool::DeviceType::DirectX11)
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
		if (deviceType == Effekseer::Tool::DeviceType::OpenGL)
		{
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}
#if _WIN32
		else if (deviceType == Effekseer::Tool::DeviceType::DirectX11)
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

		if (deviceType == Effekseer::Tool::DeviceType::OpenGL)
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
	clipboard = Effekseer::Tool::StringHelper::ConvertUtf8ToUtf16(ret);
	return clipboard.c_str();
}

void GUIManager::SetClipboardText(const char16_t* text)
{
	glfwSetClipboardString(window->GetGLFWWindows(), Effekseer::Tool::StringHelper::ConvertUtf16ToUtf8(text).c_str());
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

Vec2 GUIManager::GetWindowPos()
{
	auto v = ImGui::GetWindowPos();
	return Vec2(v.x, v.y);
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

float GUIManager::GetStyleVar(ImGuiStyleVarFlags idx)
{
	auto& style = ImGui::GetStyle();
	switch (idx)
	{
	case Alpha:
		return style.Alpha;
	case WindowRounding:
		return style.WindowRounding;
	case WindowBorderSize:
		return style.WindowBorderSize;
	case ChildRounding:
		return style.ChildRounding;
	case ChildBorderSize:
		return style.ChildBorderSize;
	case PopupRounding:
		return style.PopupRounding;
	case PopupBorderSize:
		return style.PopupBorderSize;
	case FrameRounding:
		return style.FrameRounding;
	case FrameBorderSize:
		return style.FrameBorderSize;
	case IndentSpacing:
		return style.IndentSpacing;
	case ScrollbarSize:
		return style.ScrollbarSize;
	case ScrollbarRounding:
		return style.ScrollbarRounding;
	case GrabMinSize:
		return style.GrabMinSize;
	case GrabRounding:
		return style.GrabRounding;
	default:
		return 0.0f;
	}
}

Vec2 GUIManager::GetStyleVar2(ImGuiStyleVarFlags idx)
{
	auto& style = ImGui::GetStyle();
	switch (idx)
	{
	case WindowPadding:
		return *(Vec2*)&style.WindowPadding;
	case WindowMinSize:
		return *(Vec2*)&style.WindowMinSize;
	case WindowTitleAlign:
		return *(Vec2*)&style.WindowTitleAlign;
	case FramePadding:
		return *(Vec2*)&style.FramePadding;
	case ItemSpacing:
		return *(Vec2*)&style.ItemSpacing;
	case ItemInnerSpacing:
		return *(Vec2*)&style.ItemInnerSpacing;
	case ButtonTextAlign:
		return *(Vec2*)&style.ButtonTextAlign;
	default:
		return Vec2();
	}
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

void GUIManager::Indent(float indent_w)
{
	ImGui::Indent(indent_w);
}

void GUIManager::Spacing()
{
	ImGui::Spacing();
}

void GUIManager::Dummy(const Effekseer::Tool::Vector2I& size)
{
	ImGui::Dummy({static_cast<float>(size.X), static_cast<float>(size.Y)});
}

void GUIManager::SameLine(float offset_from_start_x, float spacing)
{
	ImGui::SameLine(offset_from_start_x, spacing);
}

void GUIManager::BeginDisabled(bool disabled)
{
	ImGui::BeginDisabled(disabled);
}

void GUIManager::EndDisabled()
{
	ImGui::EndDisabled();
}

void GUIManager::AddRectFilled(float minX, float minY, float maxX, float maxY, uint32_t color, float rounding, int flags)
{
	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(minX, minY), ImVec2(maxX, maxY), color, rounding, flags);
}

void GUIManager::BeginGroup()
{
	ImGui::BeginGroup();
}

void GUIManager::EndGroup()
{
	ImGui::EndGroup();
}

void GUIManager::AlignTextToFramePadding()
{
	ImGui::AlignTextToFramePadding();
}

void GUIManager::SetNextItemWidth(float width)
{
	ImGui::SetNextItemWidth(width);
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

float GUIManager::GetCursorScreenPosX()
{
	return ImGui::GetCursorScreenPos().x;
}

float GUIManager::GetCursorScreenPosY()
{
	return ImGui::GetCursorScreenPos().y;
}

float GUIManager::GetItemRectMinX()
{
	return ImGui::GetItemRectMin().x;
}

float GUIManager::GetItemRectMinY()
{
	return ImGui::GetItemRectMin().y;
}

float GUIManager::GetItemRectMaxX()
{
	return ImGui::GetItemRectMax().x;
}

float GUIManager::GetItemRectMaxY()
{
	return ImGui::GetItemRectMax().y;
}
float GUIManager::GetItemRectSizeX()
{
	return ImGui::GetItemRectSize().x;
}
float GUIManager::GetItemRectSizeY()
{
	return ImGui::GetItemRectSize().y;
}

Vec2 GUIManager::GetItemSpacing()
{
	auto& style = ImGui::GetStyle();
	return Vec2(style.ItemSpacing.x, style.ItemSpacing.y);
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

int GUIManager::GetItemID()
{
	return ImGui::GetItemID();
}

void GUIManager::SetFocusID(int id)
{
	ImGui::SetFocusID(id, ImGui::GetCurrentWindow());
	// GImGui->NavDisableHighlight = false;
}

float GUIManager::GetScrollX()
{
	return ImGui::GetScrollX();
}

float GUIManager::GetScrollY()
{
	return ImGui::GetScrollY();
}

void GUIManager::SetScrollX(float scroll_x)
{
	ImGui::SetScrollX(scroll_x);
}

void GUIManager::SetScrollY(float scroll_y)
{
	ImGui::SetScrollY(scroll_y);
}

float GUIManager::GetScrollMaxX()
{
	return ImGui::GetScrollMaxX();
}

float GUIManager::GetScrollMaxY()
{
	return ImGui::GetScrollMaxY();
}

void GUIManager::ScrollToRect(Vec2 rect_min, Vec2 rect_max)
{
	auto window = ImGui::GetCurrentWindow();
	auto rect = window->Rect();
	rect.Max.x = rect.Min.x + rect_max.X;
	rect.Max.y = rect.Min.y + rect_max.Y;
	rect.Min.x += rect_min.X;
	rect.Min.y += rect_min.Y;

	ImGui::ScrollToRect(window, rect, ImGuiScrollFlags_KeepVisibleEdgeY);
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
			f(Effekseer::Tool::StringHelper::ConvertUtf16ToUtf8(text_).c_str());
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
			f(Effekseer::Tool::StringHelper::ConvertUtf16ToUtf8(text).c_str());
		}
	}
}

void GUIManager::Text(const char16_t* text)
{
	auto func = [](const char* c) -> void
	{ ImGui::Text(c); };
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
		ImGui::TextWrapped(Effekseer::Tool::StringHelper::ConvertUtf16ToUtf8(text).c_str());
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
		result = ImGui::CalcTextSize(Effekseer::Tool::StringHelper::ConvertUtf16ToUtf8(text).c_str());
	}

	return {result.x, result.y};
}

bool GUIManager::Button(const char16_t* label, float size_x, float size_y)
{
	return ImGui::Button(utf8str<256>(label), ImVec2(size_x, size_y));
}

void GUIManager::ImageData(std::shared_ptr<Effekseer::Tool::Image> user_texture_id, float x, float y, float uv0_x, float uv0_y, float uv1_x, float uv1_y)
{
	ImGui::Image(ToImTextureID(user_texture_id), ImVec2(x, y), {uv0_x, uv0_y}, {uv1_x, uv1_y});
}

bool GUIManager::ImageButton(std::shared_ptr<Effekseer::Tool::Image> user_texture_id, float x, float y)
{
	return ImGui::ImageButton_(
		ToImTextureID(user_texture_id), ImVec2(x, y), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1));
}

bool GUIManager::ImageButtonOriginal(std::shared_ptr<Effekseer::Tool::Image> user_texture_id, float x, float y)
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

bool GUIManager::BeginCombo(const char16_t* label, const char16_t* preview_value, ComboFlags flags, std::shared_ptr<Effekseer::Tool::Image> user_texture_id)
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

	inputTextResult = Effekseer::Tool::StringHelper::ConvertUtf8ToUtf16(buf);

	return ret;
}

bool GUIManager::InputTextMultiline(const char16_t* label, const char16_t* text)
{
	auto text_ = utf8str<1024>(text);

	char buf[512];
	memcpy(buf, text_.data, std::min((int32_t)text_.size, 500));
	buf[std::min((int32_t)text_.size, 500)] = 0;

	auto ret = ImGui::InputTextMultiline(utf8str<512>(label), buf, 512);

	inputTextResult = Effekseer::Tool::StringHelper::ConvertUtf8ToUtf16(buf);

	return ret;
}

bool GUIManager::InputTextWithHint(const char16_t* label, const char16_t* text, const char16_t* hint, InputTextFlags flags)
{
	auto text_ = utf8str<1024>(text);

	char buf[260];
	memcpy(buf, text_.data, std::min((int32_t)text_.size, 250));
	buf[std::min((int32_t)text_.size, 250)] = 0;

	auto ret = ImGui::InputTextWithHint(utf8str<256>(label), utf8str<256>(hint), buf, 260, (ImGuiWindowFlags)flags);

	inputTextResult = Effekseer::Tool::StringHelper::ConvertUtf8ToUtf16(buf);

	return ret;
}

const char16_t* GUIManager::GetInputTextResult()
{
	return inputTextResult.c_str();
}

bool GUIManager::ColorEdit4(const char16_t* label, float* col, ColorEditFlags flags)
{
	// return ImGui::ColorEdit4_(utf8str<256>(label), col, (int)flags);
	return ImGui::ColorEdit4(utf8str<256>(label), col, (int)flags);
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

void GUIManager::SetNextItemOpen(bool is_open, Cond cond)
{
	ImGui::SetNextItemOpen(is_open, (ImGuiCond)cond);
}

bool GUIManager::TreeNodeEx(const char16_t* label, bool* v, TreeNodeFlags flags)
{
	return *v = ImGui::TreeNodeEx(utf8str<256>(label), (ImGuiTreeNodeFlags)flags);
}

bool GUIManager::Selectable(const char16_t* label, bool selected, SelectableFlags flags)
{
	return ImGui::Selectable(utf8str<256>(label), selected, (int)flags, ImVec2(0, 0));
}

bool GUIManager::SelectableContent(const char16_t* idstr, const char16_t* label, bool selected, std::shared_ptr<Effekseer::Tool::Image> thumbnail, float size_x, float size_y, SelectableFlags flags)
{
	ImVec2 cursorPos = ImGui::GetCursorScreenPos();
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	const auto& style = ImGui::GetStyle();

	ImVec2 screenPos = ImGui::GetCursorScreenPos();
	float labelHeight = ImGui::GetTextLineHeight() * 2.0f;
	ImVec4 clipRect = {screenPos.x, screenPos.y, screenPos.x + size_x, screenPos.y + size_y + labelHeight};

	if (thumbnail)
	{
		drawList->AddImage(ToImTextureID(thumbnail), cursorPos, ImVec2(cursorPos.x + size_x, cursorPos.y + size_y));
	}

	bool result = ImGui::Selectable(utf8str<256>(idstr), selected, (int)flags, ImVec2(size_x, size_y + labelHeight));

	utf8str<256> labelU8(label);
	size_t labelLen = strlen(labelU8);
	ImVec2 labelSize = ImGui::CalcTextSize(labelU8, labelU8 + labelLen, false, size_x);

	cursorPos.x += (size_x - labelSize.x) * 0.5f;
	cursorPos.y += size_y;
	drawList->AddText(GImGui->Font, GImGui->FontSize, ImVec2(cursorPos.x + 1, cursorPos.y + 1), ImGui::GetColorU32(ImGuiCol_WindowBg, 0.8f), labelU8, labelU8 + labelLen, size_x, &clipRect);
	drawList->AddText(GImGui->Font, GImGui->FontSize, cursorPos, ImGui::GetColorU32(ImGuiCol_Text), labelU8, labelU8 + labelLen, size_x, &clipRect);

	return result;
}

void GUIManager::SetTooltip(const char16_t* text)
{
	auto func = [](const char* c) -> void
	{ ImGui::SetTooltip(c); };
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

bool GUIManager::BeginTabBar(const char16_t* idstr)
{
	return ImGui::BeginTabBar(utf8str<256>(idstr));
}

void GUIManager::EndTabBar()
{
	ImGui::EndTabBar();
}

bool GUIManager::BeginTabItem(const char16_t* label)
{
	return ImGui::BeginTabItem(utf8str<256>(label));
}

void GUIManager::EndTabItem()
{
	ImGui::EndTabItem();
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

	auto fi = Effekseer::MakeRefPtr<Effekseer::DefaultFileInterface>();
	auto reader = fi->OpenRead(filename);
	if (reader == nullptr)
	{
		return;
	}

	std::vector<uint8_t> buffer(reader->GetLength());
	reader->Read(&buffer[0], buffer.size());

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

int GUIManager::GetPressedKeyIndex(bool repeat)
{
	for (int i = 0; i < 512; i++)
	{
		if (ImGui::IsKeyPressed(i, repeat))
		{
			return i;
		}
	}
	return -1;
}

const char16_t* GUIManager::GetInputCharacters()
{
	inputTextResult.clear();
	for (auto c : ImGui::GetIO().InputQueueCharacters)
	{
		inputTextResult.push_back(c);
	}
	return inputTextResult.c_str();
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

void GUIManager::PushTextWrapPos(float pos)
{
	ImGui::PushTextWrapPos(pos);
}

void GUIManager::PopTextWrapPos()
{
	ImGui::PopTextWrapPos();
}

bool GUIManager::IsItemHovered(float f)
{
	return ImGui::IsItemHovered(f);
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

void GUIManager::PushID(int int_id)
{
	ImGui::PushID(int_id);
}

void GUIManager::PopID()
{
	ImGui::PopID();
}

bool GUIManager::BeginTable(const char* str_id, int column, TableFlags flags, const Vec2& outer_size, float inner_width)
{
	return ImGui::BeginTable(str_id, column, static_cast<ImGuiTabBarFlags>(flags), ImVec2{outer_size.X, outer_size.Y}, inner_width);
}

void GUIManager::EndTable()
{
	ImGui::EndTable();
}

void GUIManager::TableNextRow(TableRowFlags row_flags, float min_row_height)
{
	ImGui::TableNextRow(static_cast<ImGuiTableRowFlags>(row_flags), min_row_height);
}

bool GUIManager::TableNextColumn()
{
	return ImGui::TableNextColumn();
}

bool GUIManager::TableSetColumnIndex(int column_n)
{
	return ImGui::TableSetColumnIndex(column_n);
}

void GUIManager::TableSetupColumn(const char* label, TableColumnFlags flags, float init_width_or_weight, uint32_t user_id)
{
	ImGui::TableSetupColumn(label, static_cast<ImGuiTableColumnFlags>(flags), init_width_or_weight, user_id);
}

void GUIManager::TableSetupScrollFreeze(int cols, int rows)
{
	ImGui::TableSetupScrollFreeze(cols, rows);
}

void GUIManager::TableHeadersRow()
{
	ImGui::TableHeadersRow();
}

void GUIManager::TableHeader(const char* label)
{
	ImGui::TableHeader(label);
}

int GUIManager::TableGetColumnCount()
{
	return ImGui::TableGetColumnCount();
}

int GUIManager::TableGetColumnIndex()
{
	return ImGui::TableGetColumnIndex();
}

int GUIManager::TableGetRowIndex()
{
	return ImGui::TableGetRowIndex();
}

const char* GUIManager::TableGetColumnName(int column_n)
{
	return ImGui::TableGetColumnName(column_n);
}

TableColumnFlags GUIManager::TableGetColumnFlags(int column_n)
{
	return static_cast<TableColumnFlags>(ImGui::TableGetColumnFlags(column_n));
}

void GUIManager::TableSetColumnEnabled(int column_n, bool v)
{
	ImGui::TableSetColumnEnabled(column_n, v);
}

void GUIManager::TableSetBgColor(TableBgTarget target, uint32_t color, int column_n)
{
	ImGui::TableSetBgColor(static_cast<ImGuiTableBgTarget>(target), color, column_n);
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

bool GUIManager::GradientHDR(int32_t gradientID, Effekseer::Tool::GradientHDRState& state, Effekseer::Tool::GradientHDRGUIState& guiState, bool isMarkerShown)
{
	return ImGradientHDR(gradientID, state.GetState(), guiState.GetTemporaryState(), isMarkerShown);
}

bool GUIManager::BeginPlot(const char16_t* label, const Vec2& size, PlotFlags flags)
{
	return ImPlot::BeginPlot(utf8str<256>(label), ImVec2(size.X, size.Y), (ImPlotFlags)flags);;
}

void GUIManager::EndPlot()
{
	ImPlot::EndPlot();
}

void GUIManager::SetupPlotAxis(PlotAxis axis, const char16_t* label, PlotAxisFlags flags)
{
	ImPlot::SetupAxis((ImAxis)axis, utf8str<256>(label), (ImPlotAxisFlags)flags);
}

void GUIManager::SetupPlotAxisLimits(PlotAxis axis, double vMin, double vMax, Cond cond)
{
	ImPlot::SetupAxisLimits((ImAxis)axis, vMin, vMax, (ImPlotCond)cond);
}

void GUIManager::SetupPlotAxisFormat(PlotAxis axis, const char16_t* fmt)
{
	ImPlot::SetupAxisFormat((ImAxis)axis, utf8str<256>(fmt));
}

void GUIManager::SetupPlotAxisTicks(PlotAxis axis, double vMin, double vMax, int nTicks)
{
	ImPlot::SetupAxisTicks((ImAxis)axis, vMin, vMax, nTicks);
}

void GUIManager::PlotLine(const char16_t* label, const double* xValues, const double* yValues, int32_t count, int32_t offset)
{
	ImPlot::PlotLine(utf8str<256>(label), xValues, yValues, count, offset);
}

void GUIManager::PlotShaded(const char16_t* label, const double* xValues, const double* yValues, int32_t count, double yRef, int32_t offset)
{
	ImPlot::PlotShaded(utf8str<256>(label), xValues, yValues, count, yRef, offset);
}

void GUIManager::SetNextPlotLineStyle(const Vec4& color, float weight)
{
	ImPlot::SetNextLineStyle(ImVec4(color.X, color.Y, color.Z, color.W), weight);
}

void GUIManager::SetNextPlotFillStyle(const Vec4& color, float alphaMod)
{
	ImPlot::SetNextFillStyle(ImVec4(color.X, color.Y, color.Z, color.W), alphaMod);
}

} // namespace efk
