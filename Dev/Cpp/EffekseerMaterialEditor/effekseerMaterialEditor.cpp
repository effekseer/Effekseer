#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "gdiplus.lib")

/*
#ifdef _DEBUG

#pragma comment(lib, "x86/Debug/AltseedRHI.lib")
#pragma comment(lib, "x86/Debug/glfw3.lib")
#pragma comment(lib, "x86/Debug/libglew32d.lib")

#else

#pragma comment(lib, "x86/Release/AltseedRHI.lib")
#pragma comment(lib, "x86/Release/glfw3.lib")
#pragma comment(lib, "x86/Release/libglew32d.lib")

#endif
*/

#include <AltseedRHI.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <efkMat.Base.h>
#include <efkMat.Models.h>
#include <efkMat.Parameters.h>
#include <efkMat.TextExporter.h>

#include "efkMat.Editor.h"
#include <Platform/efkMat.GLSL.h>
#include <Platform/efkMat.HLSL.h>

#include <efkMat.CommandManager.h>
#include <efkMat.StringContainer.h>

namespace ed = ax::NodeEditor;

static ed::EditorContext* g_Context = nullptr;
GLFWwindow* glfwMainWindow = nullptr;
ar::Manager* arManager = nullptr;
ar::Context* context = nullptr;

std::shared_ptr<EffekseerMaterial::Graphics> graphics;
std::shared_ptr<EffekseerMaterial::Editor> editor;
std::shared_ptr<EffekseerMaterial::Node> g_selectedNode;

std::array<bool, 512> keyState;
std::array<bool, 512> keyStatePre;

void Application_Initialize()
{
	FILE* fp = fopen("resources/efkmat_lang_ja.json", "rb");

	if (fp != nullptr)
	{
		fseek(fp, 0, SEEK_END);
		auto size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		std::vector<char> data;
		data.resize(size);
		fread(data.data(), size, 1, fp);
		fclose(fp);

		EffekseerMaterial::StringContainer::LoadFromJsonStr(data.data());
	}

	ed::Config config;
	config.SettingsFile = "Simple.json";
	g_Context = ed::CreateEditor(&config);

	editor = std::make_shared<EffekseerMaterial::Editor>(graphics);

	keyStatePre.fill(false);
	keyState.fill(false);
}

void Application_Finalize() { ed::DestroyEditor(g_Context); }

void Application_Frame()
{
	keyState = keyStatePre;
	for (int i = 0; i < 512; i++)
	{
		keyStatePre[i] = ImGui::GetIO().KeysDown[i];
	}

	if (ImGui::GetIO().KeyCtrl && keyStatePre[GLFW_KEY_Z] && !keyState[GLFW_KEY_Z])
	{
		EffekseerMaterial::CommandManager::GetInstance()->Undo();
	}

	if (ImGui::GetIO().KeyCtrl && keyStatePre[GLFW_KEY_Y] && !keyState[GLFW_KEY_Y])
	{
		EffekseerMaterial::CommandManager::GetInstance()->Redo();
	}

	// Menu bar
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu(EffekseerMaterial::StringContainer::GetValue("File").c_str()))
		{
			if (ImGui::MenuItem(EffekseerMaterial::StringContainer::GetValue("Load").c_str()))
			{
				editor->Load();
			}

			if (ImGui::MenuItem(EffekseerMaterial::StringContainer::GetValue("Save").c_str()))
			{
				editor->Save();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	// show with fullscreen
	ImVec2 windowSize;
	windowSize.x = ImGui::GetIO().DisplaySize.x;
	windowSize.y = ImGui::GetIO().DisplaySize.y - 25;

	ImGui::SetNextWindowSize(windowSize);

	ImGuiIO& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(ImVec2(0, 25));
	
	const ImGuiWindowFlags flags = (ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize |
									ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
	const float oldWindowRounding = ImGui::GetStyle().WindowRounding;
	ImGui::GetStyle().WindowRounding = 0;
	const bool visible = ImGui::Begin("MaterialEditor", NULL, ImVec2(0, 0), 1.0f, flags);
	ImGui::GetStyle().WindowRounding = oldWindowRounding;

	if (visible)
	{
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 200);

		ImGui::BeginChild("##Left");

		editor->UpdatePreview();

		ImGui::Separator();

		if (g_selectedNode != nullptr)
		{
			editor->UpdateParameterEditor(g_selectedNode);
		}

		ImGui::EndChild();

		ImGui::NextColumn();

		ImGui::BeginChild("##Right");

		auto& io = ImGui::GetIO();

		if (!io.MouseDown[0])
		{
			EffekseerMaterial::CommandManager::GetInstance()->MakeMergeDisabled();
		}

		ed::SetCurrentEditor(g_Context);
		ed::Begin("My Editor", ImVec2(0.0, 0.0f));
		//ed::Suspend();

		editor->Update();

		ed::End();

		// Find selected node
		ax::NodeEditor::NodeId nodeIDs[2];
		g_selectedNode = nullptr;
		if (ed::GetSelectedNodes(nodeIDs, 2) > 0)
		{
			for (auto node : editor->material->GetNodes())
			{
				if (node->GUID == nodeIDs[0].Get())
				{
					g_selectedNode = node;
				}
			}
		}

		ed::SetCurrentEditor(nullptr);

		ImGui::EndChild();

		ImGui::Columns(1);

		ImGui::End();
	}

	if (ImGui::Begin("Code_Debug"))
	{
		if (g_selectedNode != nullptr)
		{
			{
				EffekseerMaterial::TextExporterGLSL exporter_;
				EffekseerMaterial::TextExporter* exporter = &exporter_;
				auto code = exporter->Export(editor->material, g_selectedNode);
				ImGui::Text(code.Code.c_str());
			}
		}

		ImGui::End();
	}
}

int main()
{
	int32_t width = 1280;
	int32_t height = 720;
	char* title = "node editor";

	if (!glfwInit())
	{
		return false;
	}

#if __APPLE__
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwMainWindow = glfwCreateWindow(width, height, title, NULL, NULL);
	glfwMakeContextCurrent(glfwMainWindow);
	glfwSwapInterval(1);

	graphics = std::make_shared<EffekseerMaterial::Graphics>();
	graphics->Initialize(width, height);
	context = ar::Context::Create(graphics->GetManager());
	auto arManager = graphics->GetManager();

#if __APPLE__
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
#endif

	// Init imgui
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(glfwMainWindow, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	ImGuiIO& io = ImGui::GetIO();
	ImFont* font1 =
		io.Fonts->AddFontFromFileTTF("resources/GenShinGothic-Monospace-Normal.ttf", 20, nullptr, io.Fonts->GetGlyphRangesJapanese());

	ImGui::StyleColorsDark();

	Application_Initialize();

	while (!glfwWindowShouldClose(glfwMainWindow))
	{

		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		Application_Frame();

		{
			auto nodes = editor->material->GetNodes();
			for (auto node : nodes)
			{
				if (node->UserObj != nullptr)
				{
					auto preview = (EffekseerMaterial::NodeUserDataObject*)node->UserObj.get();
					preview->GetPreview()->Render();
				}
			}

			editor->preview_->Render();
		}

		arManager->BeginRendering();

		ar::SceneParameter sparam;

		arManager->BeginScene(sparam);

		ar::Color clearColor;
		clearColor.R = 100;
		clearColor.G = 50;
		clearColor.B = 50;
		clearColor.A = 0;

		arManager->Clear(true, true, clearColor);

		arManager->EndScene();

		arManager->EndRendering();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (glfwMainWindow != nullptr)
		{
			glfwSwapBuffers(glfwMainWindow);
		}
	}

	Application_Finalize();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	ar::SafeDelete(context);

	if (glfwMainWindow != nullptr)
	{
		glfwDestroyWindow(glfwMainWindow);
		glfwTerminate();
	}
}
