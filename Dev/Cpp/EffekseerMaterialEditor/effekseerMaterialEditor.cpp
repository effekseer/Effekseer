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
#include "Dialog/Dialog.h"

#include "../IPC/IPC.h"

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

#ifdef WIN32
#include <Windows.h>
#include <direct.h>
#endif

namespace ed = ax::NodeEditor;

GLFWwindow* glfwMainWindow = nullptr;
ar::Manager* arManager = nullptr;
ar::Context* context = nullptr;

std::shared_ptr<EffekseerMaterial::Graphics> graphics;
std::shared_ptr<EffekseerMaterial::Editor> editor;
std::shared_ptr<EffekseerMaterial::Node> g_selectedNode;

std::array<bool, 512> keyState;
std::array<bool, 512> keyStatePre;

std::string GetDirectoryName(const std::string& path)
{
	const std::string::size_type pos = std::max<int32_t>(path.find_last_of('/'), path.find_last_of('\\'));
	return (pos == std::string::npos) ? std::string() : path.substr(0, pos + 1);
}

std::string GetExecutingDirectory()
{
	char buf[260];

#ifdef _WIN32
	int len = GetModuleFileNameA(NULL, buf, 260);
	if (len <= 0)
		return "";
#else

	char temp[32];
	sprintf(temp, "/proc/%d/exe", getpid());
	int bytes = std::min(readlink(temp, buf, 260), 260 - 1);
	if (bytes >= 0)
		buf[bytes] = '\0';
#endif

	return GetDirectoryName(buf);
}

void SetCurrentDir(const char* path)
{
#ifdef _WIN32
	_chdir(path);
#else
	chdir(path);
#endif
}

int main(int argc, char* argv[])
{
	bool ipcMode = false;

	if (argc >= 2 && std::string(argv[1]) == "ipc")
	{
		ipcMode = true;
	}

	SetCurrentDir(GetExecutingDirectory().c_str());

	int32_t width = 1280;
	int32_t height = 720;

#if _DEBUG
	char* title = "EffekseerMaterialEditor - debug";
#else
	char* title = "EffekseerMaterialEditor";
#endif
	if (!glfwInit())
	{
		return false;
	}

	auto commandQueueToMaterialEditor_ = std::make_shared<IPC::CommandQueue>();
	commandQueueToMaterialEditor_->Start("EffekseerCommandToMaterialEditor", 1024 * 1024);

	auto commandQueueFromMaterialEditor_ = std::make_shared<IPC::CommandQueue>();
	commandQueueFromMaterialEditor_->Start("EffekseerCommandFromMaterialEditor", 1024 * 1024);

	uint64_t previousHistoryID = 0;

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

	editor = std::make_shared<EffekseerMaterial::Editor>(graphics);

	std::vector<std::shared_ptr<EffekseerMaterial::Dialog>> dialogs;

	keyStatePre.fill(false);
	keyState.fill(false);

	bool isFirstFrame = true;
	int framecount = 0;

	while (!glfwWindowShouldClose(glfwMainWindow))
	{
		glfwPollEvents();

		// command event
		{
			IPC::CommandData commandDataTOMaterialEditor;
			if (commandQueueToMaterialEditor_->Dequeue(&commandDataTOMaterialEditor))
			{
				if (commandDataTOMaterialEditor.Type == IPC::CommandType::Terminate)
				{
					break;
				}
				else if (commandDataTOMaterialEditor.Type == IPC::CommandType::OpenMaterial)
				{
					editor->LoadOrSelect(commandDataTOMaterialEditor.str.data());
				}
				else if (commandDataTOMaterialEditor.Type == IPC::CommandType::OpenOrCreateMaterial)
				{
#ifdef _DEBUG
					std::cout << "OpenOrCreateMaterial : " << commandDataTOMaterialEditor.str.data() << std::endl;
#endif
					if (!editor->LoadOrSelect(commandDataTOMaterialEditor.str.data()))
					{
						editor->New();
						editor->SaveAs(commandDataTOMaterialEditor.str.data());
					}
				}
			}
		}

		auto material = editor->GetSelectedMaterial();
		if (material != nullptr && material->GetCommandManager()->GetHistoryID() != previousHistoryID)
		{
			auto content = editor->GetContents()[editor->GetSelectedContentIndex()];
#ifdef _DEBUG
			std::cout << "NotifyUpdate : " << material->GetPath() << std::endl;
#endif

			content->UpdateBinary();

			previousHistoryID = material->GetCommandManager()->GetHistoryID();
			IPC::CommandData commandDataFromMaterialEditor;
			commandDataFromMaterialEditor.Type = IPC::CommandType::NotifyUpdate;
			commandDataFromMaterialEditor.SetStr(content->GetPath().c_str());
			commandQueueFromMaterialEditor_->Enqueue(&commandDataFromMaterialEditor);
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		{
			keyState = keyStatePre;
			for (int i = 0; i < 512; i++)
			{
				keyStatePre[i] = ImGui::GetIO().KeysDown[i];
			}

			if (material != nullptr)
			{
				if (ImGui::GetIO().KeyCtrl && keyStatePre[GLFW_KEY_Z] && !keyState[GLFW_KEY_Z])
				{
					material->GetCommandManager()->Undo();
				}

				if (ImGui::GetIO().KeyCtrl && keyStatePre[GLFW_KEY_Y] && !keyState[GLFW_KEY_Y])
				{
					material->GetCommandManager()->Redo();
				}
			}

			// Menu bar
			if (ImGui::BeginMainMenuBar())
			{
				if (ImGui::BeginMenu(EffekseerMaterial::StringContainer::GetValue("File").c_str()))
				{
					if (!ipcMode)
					{
						if (ImGui::MenuItem(EffekseerMaterial::StringContainer::GetValue("New").c_str()))
						{
							editor->New();
						}

						if (ImGui::MenuItem(EffekseerMaterial::StringContainer::GetValue("Load").c_str()))
						{
							editor->Load();
						}
					}

					if (ImGui::MenuItem(EffekseerMaterial::StringContainer::GetValue("Save").c_str()))
					{
						editor->Save();
					}

					if (ImGui::MenuItem(EffekseerMaterial::StringContainer::GetValue("SaveAs").c_str()))
					{
						editor->SaveAs();
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
				if (ImGui::BeginTabBar("###MainTab"))
				{
					bool isSelectedNow = editor->GetIsSelectedDirtyAndClear();

					for (size_t i = 0; i < editor->GetContents().size(); i++)
					{
						std::string tabName = editor->GetContents()[i]->GetName();
						tabName += "###tab" + std::to_string(i);

						bool isSelected = editor->GetSelectedContentIndex() == i;

						ImGuiTabItemFlags tabItemFlags = 0;
						if (isSelected && isSelectedNow)
						{
							tabItemFlags |= ImGuiTabItemFlags_SetSelected;
						}

						bool isOpen = true;
						if (ImGui::BeginTabItem(tabName.c_str(), &isOpen, tabItemFlags))
						{
							// avoid to select when selected contents is changed
							if (!isSelectedNow)
							{
								editor->SelectContent(i);
							}

							ImGui::Columns(2);
							ImGui::SetColumnWidth(0, 200);

							ImGui::BeginChild("###Left");

							editor->UpdatePreview();

							ImGui::Separator();

							if (g_selectedNode != nullptr)
							{
								editor->UpdateParameterEditor(g_selectedNode);
							}

							ImGui::EndChild();

							ImGui::NextColumn();

							ImGui::BeginChild("###Right");

							auto& io = ImGui::GetIO();

							if (!io.MouseDown[0])
							{
								editor->GetContents()[i]->GetMaterial()->GetCommandManager()->MakeMergeDisabled();
							}

							ed::SetCurrentEditor(editor->GetContents()[i]->GetEditorContext());
							ed::Begin("###MainEditor", ImVec2(0.0, 0.0f));
							// ed::Suspend();

							editor->Update();

							ed::End();

							// Find selected node
							ax::NodeEditor::NodeId nodeIDs[2];
							g_selectedNode = nullptr;
							if (ed::GetSelectedNodes(nodeIDs, 2) > 0)
							{
								for (auto node : editor->GetContents()[i]->GetMaterial()->GetNodes())
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

							ImGui::EndTabItem();
						}

						if (!isOpen)
						{
							// close item
							if (editor->GetContents()[i]->GetIsChanged())
							{
								editor->GetContents()[i]->WillShowClosingDialog = true;
							}
							else
							{
								editor->GetContents()[i]->IsClosing = true;
							}
						}
					}

					ImGui::EndTabBar();
				}

				ImGui::End();
			}

			for (size_t i = 0; i < editor->GetContents().size(); i++)
			{
				if (editor->GetContents()[i]->WillShowClosingDialog)
				{
					auto closeDialog = std::make_shared<EffekseerMaterial::SaveOrCloseDialog>(editor->GetContents()[i]);
					ImGui::OpenPopup(closeDialog->GetID());
					dialogs.push_back(closeDialog);
					editor->GetContents()[i]->WillShowClosingDialog = false;
				}
			}

			framecount++;
			if (isFirstFrame && framecount > 1)
			{
				if (!ipcMode)
				{
					auto closeDialog = std::make_shared<EffekseerMaterial::NewOrOpenDialog>(editor);
					ImGui::OpenPopup(closeDialog->GetID());
					dialogs.push_back(closeDialog);
				}
				isFirstFrame = false;
			}

			// popup
			for (auto dialog : dialogs)
			{
				if (!dialog->Update())
				{
					dialog->IsClosing = true;
				}
			}

			auto removed_it = std::remove_if(
				dialogs.begin(), dialogs.end(), [](std::shared_ptr<EffekseerMaterial::Dialog> d) -> bool { return d->IsClosing; });

			dialogs.erase(removed_it, dialogs.end());
		}

		if (ImGui::Begin("Code_Debug"))
		{
			if (material != nullptr && g_selectedNode != nullptr)
			{
				auto uobj = (EffekseerMaterial::NodeUserDataObject*)g_selectedNode->UserObj.get();

				if (uobj != nullptr)
				{
					ImGui::Text(uobj->GetPreview()->VS.c_str());
					ImGui::Text(uobj->GetPreview()->PS.c_str());
				}
			}

			ImGui::End();
		}

		if (material != nullptr)
		{
			auto nodes = material->GetNodes();
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

		editor->CloseContents();

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

	editor.reset();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	ar::SafeDelete(context);

	if (glfwMainWindow != nullptr)
	{
		glfwDestroyWindow(glfwMainWindow);
		glfwTerminate();
	}

	commandQueueToMaterialEditor_->Stop();
	commandQueueFromMaterialEditor_->Stop();
}
