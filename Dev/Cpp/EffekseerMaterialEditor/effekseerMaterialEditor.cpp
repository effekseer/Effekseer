#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "gdiplus.lib")

#include "Config.h"
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

#include <GUI/JapaneseFont.h>
#include <efkMat.CommandManager.h>
#include <efkMat.StringContainer.h>

#include <GUI/MainWindow.h>

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

bool g_showDebugWindow = false;

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

std::vector<std::shared_ptr<EffekseerMaterial::Dialog>> newDialogs;
std::vector<std::shared_ptr<EffekseerMaterial::Dialog>> dialogs;

void GLFLW_CloseCallback(GLFWwindow* w)
{
	bool isChanged = false;

	for (size_t i = 0; i < editor->GetContents().size(); i++)
	{
		if (editor->GetContents()[i]->GetIsChanged())
		{
			auto closeIfCan = [w]() -> void {
				bool isChanged = false;

				for (size_t i = 0; i < editor->GetContents().size(); i++)
				{
					if (editor->GetContents()[i]->GetIsChanged())
					{
						isChanged = true;
					}
				}

				if (!isChanged)
				{
					glfwSetWindowShouldClose(w, GL_TRUE);
				}
			};

			auto closeDialog =
				std::make_shared<EffekseerMaterial::SaveOrCloseDialog>(editor->GetContents()[i], [closeIfCan]() { closeIfCan(); });
			newDialogs.push_back(closeDialog);
			isChanged = true;
		}
	}

	if (isChanged)
	{
		glfwSetWindowShouldClose(w, GL_FALSE);
	}
}

int mainLoop(int argc, char* argv[])
{
	bool ipcMode = false;

	if (argc >= 2 && std::string(argv[1]) == "ipc")
	{
		ipcMode = true;
	}

	SetCurrentDir(GetExecutingDirectory().c_str());

	auto config = std::make_shared<EffekseerMaterial::Config>();
	config->Load("config.EffekseerMaterial.json");

	if (config->WindowWidth == 0)
	{
		config->WindowWidth = 1280;
		config->WindowHeight = 720;
	}

#if _DEBUG
	char16_t* title = u"EffekseerMaterialEditor - debug";
#else
	char16_t* title = u"EffekseerMaterialEditor";
#endif
	
	Effekseer::MainWindowState mainWindowState;
	mainWindowState.Width = config->WindowWidth;
	mainWindowState.Height = config->WindowHeight;
	mainWindowState.PosX = config->WindowPosX;
	mainWindowState.PosY = config->WindowPosY;
	mainWindowState.IsMaximumMode = config->WindowIsMaximumMode;
	if (!Effekseer::MainWindow::Initialize(title, mainWindowState, false, true))
	{
		return 0;
	}

	auto mainWindow = Effekseer::MainWindow::GetInstance();

	auto commandQueueToMaterialEditor_ = std::make_shared<IPC::CommandQueue>();
	commandQueueToMaterialEditor_->Start("EffekseerCommandToMaterialEditor", 1024 * 1024);

	auto commandQueueFromMaterialEditor_ = std::make_shared<IPC::CommandQueue>();
	commandQueueFromMaterialEditor_->Start("EffekseerCommandFromMaterialEditor", 1024 * 1024);

	uint64_t previousHistoryID = 0;

	glfwMainWindow = mainWindow->GetGLFWWindows();

	glfwSetWindowCloseCallback(glfwMainWindow, GLFLW_CloseCallback);

	glfwMakeContextCurrent(glfwMainWindow);
	glfwSwapInterval(1);

	graphics = std::make_shared<EffekseerMaterial::Graphics>();
	graphics->Initialize(config->WindowWidth, config->WindowHeight);
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
	ImFont* font1 = io.Fonts->AddFontFromFileTTF("resources/GenShinGothic-Monospace-Normal.ttf", 20, nullptr, glyphRangesJapanese);

	ImGui::StyleColorsDark();

	FILE* fp = nullptr;

	if (config->Language == Effekseer::SystemLanguage::Japanese)
	{
		fp = fopen("resources/languages/effekseer_material_ja.json", "rb");
	}
	else
	{
		fp = fopen("resources/languages/effekseer_material_en.json", "rb");
	}

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

	keyStatePre.fill(false);
	keyState.fill(false);

	bool isFirstFrame = true;
	int framecount = 0;
	int leftWidth = 220;

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

#ifdef _DEBUG
				if (ImGui::BeginMenu("Debug"))
				{
					if (ImGui::MenuItem("DebugWindow"))
					{
						g_showDebugWindow = true;
					}

					ImGui::EndMenu();
				}
#endif

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

							if (i == editor->GetSelectedContentIndex())
							{

								ImGui::Columns(2);

								if (editor->GetContents()[i]->IsLoading)
								{
									ImGui::SetColumnWidth(0, leftWidth);
								}
								else
								{
									leftWidth = ImGui::GetColumnWidth(0);
								}

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
							}

							ImGui::EndTabItem();
						}

						if (!isOpen)
						{
							// close item
							if (editor->GetContents()[i]->GetIsChanged())
							{
								auto closeDialog =
									std::make_shared<EffekseerMaterial::SaveOrCloseDialog>(editor->GetContents()[i], []() {});
								// ImGui::OpenPopup(closeDialog->GetID());
								newDialogs.push_back(closeDialog);
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

			// HACK because of imgui specification
			if (framecount == 3)
			{
				if (!ipcMode)
				{
					auto creatDialog = std::make_shared<EffekseerMaterial::NewOrOpenDialog>(editor);
					// ImGui::OpenPopup(creatDialog->GetID());
					newDialogs.push_back(creatDialog);
				}
			}

			// popup
			if (dialogs.size() == 0 && newDialogs.size() > 0)
			{
				ImGui::OpenPopup(newDialogs[0]->GetID());
				dialogs.push_back(newDialogs[0]);
				newDialogs.erase(newDialogs.begin(), newDialogs.begin() + 1);
			}

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

			framecount++;
			isFirstFrame = false;
		}

		if (g_showDebugWindow && ImGui::Begin("Code_Debug"))
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
		mainWindowState = mainWindow->GetState();

		config->WindowWidth = mainWindowState.Width;
		config->WindowHeight = mainWindowState.Height;
		config->WindowPosX = mainWindowState.PosX;
		config->WindowPosY = mainWindowState.PosY;
		config->WindowIsMaximumMode = mainWindowState.IsMaximumMode;
	}

	commandQueueToMaterialEditor_->Stop();
	commandQueueFromMaterialEditor_->Stop();

	config->Save("config.EffekseerMaterial.json");

	Effekseer::MainWindow::Terminate();
	mainWindow = nullptr;

	return 0;
}

#if defined(NDEBUG) && defined(_WIN32)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpszCmdLine, int nShowCmd) { return mainLoop(__argc, __argv); }
#else
int main(int argc, char* argv[]) { return mainLoop(argc, argv); }
#endif
