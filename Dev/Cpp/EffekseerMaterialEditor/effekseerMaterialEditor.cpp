#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "gdiplus.lib")

#include "Config.h"
#include "Dialog/Dialog.h"
#include "Graphics/efkMat.Graphics.h"

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

#include <efkMat.CommandManager.h>
#include <efkMat.StringContainer.h>

#include <Common/StringHelper.h>
#include <GUI/MainWindow.h>
#include <IO/IO.h>
#include <algorithm>

#include <GUI/Misc.h>
#include <IO/CSVReader.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <fstream>
#include <iostream>
#include <sstream>

#ifdef WIN32
#include <windows.h>
#include <direct.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif

#if defined(__APPLE__)
#include <mach-o/dyld.h>
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

class IOCallback : public Effekseer::IOCallback
{
public:
	void OnFileChanged(Effekseer::StaticFileType fileType, const char16_t* path)
	{
		auto pathu8 = Effekseer::utf16_to_utf8(path);
		EffekseerMaterial::TextureCache::NotifyFileChanged(pathu8.c_str());
	}
};

std::string GetDirectoryName(const std::string& path)
{
	const std::string::size_type pos = std::max<int32_t>(path.find_last_of('/'), path.find_last_of('\\'));
	return (pos == std::string::npos) ? std::string() : path.substr(0, pos + 1);
}

std::string GetExecutingDirectory()
{
	char buf[260];

#ifdef _WIN32
	int len = GetModuleFileNameA(nullptr, buf, 260);
	if (len <= 0)
		return "";
#elif defined(__APPLE__)
	uint32_t size = 260;
	if (_NSGetExecutablePath(buf, &size) != 0)
	{
		buf[0] = 0;
	}
#else

	char temp[32];
	sprintf(temp, "/proc/%d/exe", getpid());
	int bytes = std::min((int)readlink(temp, buf, 260), 260 - 1);
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
	spdlog::info("SetCurrentDir : {}", path);
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

void ChangeLanguage(const std::string& key)
{
	auto loadLanguage = [](const std::string& k, const std::string& filename) {
		const auto languageFilePath = GetExecutingDirectory() + "resources/languages/" + k + "/" + filename;

		std::ifstream f(languageFilePath);
		if (!f.is_open())
		{
			return;
		}

		auto str = std::string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
		auto csv = Effekseer::Editor::ReadCSV(str);

		for (const auto& line : csv)
		{
			if (line.size() < 2)
				continue;

			if (line[0] == "")
				continue;

			EffekseerMaterial::StringContainer::AddValue(line[0].c_str(), line[1].c_str());
		}
	};

	EffekseerMaterial::StringContainer::Clear();
	loadLanguage("en", "Base.csv");
	loadLanguage("en", "EffekseerMaterialEditor.csv");

	if (key != "en")
	{
		loadLanguage(key, "Base.csv");
		loadLanguage(key, "EffekseerMaterialEditor.csv");
	}
}

void ChangeLanguage(Effekseer::SystemLanguage language)
{
	if (language == Effekseer::SystemLanguage::Japanese)
	{
		ChangeLanguage("ja");
	}
	else
	{
		ChangeLanguage("en");
	}
}

int mainLoop(int argc, char* argv[])
{
	bool ipcMode = false;

	if (argc >= 2 && std::string(argv[1]) == "ipc")
	{
		ipcMode = true;
	}

	std::string languageKey;

	if (argc >= 2)
	{
		for (int i = 1; i < argc; i++)
		{
			if (std::string(argv[i - 1]) == "--language" || std::string(argv[i - 1]) == "-l")
			{
				languageKey = argv[i];
				break;
			}
		}
	}

	SetCurrentDir(GetExecutingDirectory().c_str());

	// check debug mode
	bool isDebugMode = false;
	{
		auto debugfp = fopen("debug.txt", "rb");
		if (debugfp != nullptr)
		{
			isDebugMode = true;
			fclose(debugfp);
		}
	}

#ifndef NDEBUG
	isDebugMode = true;
#endif

	if (isDebugMode)
	{
		auto fileLogger = spdlog::basic_logger_mt("logger", GetExecutingDirectory() + "EffekseerMaterialEditor.log.txt");
		spdlog::set_default_logger(fileLogger);
		spdlog::set_level(spdlog::level::trace);
	}

	spdlog::info("Start MaterialEditor");

	auto config = std::make_shared<EffekseerMaterial::Config>();
	config->Load("config.EffekseerMaterial.json");

	if (config->WindowWidth == 0)
	{
		config->WindowWidth = 1280;
		config->WindowHeight = 720;
	}

#if _DEBUG
	const char16_t* title = u"EffekseerMaterialEditor - debug";
#else
	const char16_t* title = u"EffekseerMaterialEditor";
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
	Effekseer::IO::Initialize(1000);
	Effekseer::IO::GetInstance()->AddCallback(std::make_shared<IOCallback>());

	auto commandQueueToMaterialEditor_ = std::make_shared<IPC::CommandQueue>();
	if (!commandQueueToMaterialEditor_->Start("EfkCmdToMatEdit", 1024 * 1024))
	{
		spdlog::warn("Failed to start EfkCmdToMatEdit");
	}

	auto commandQueueFromMaterialEditor_ = std::make_shared<IPC::CommandQueue>();
	if (!commandQueueFromMaterialEditor_->Start("EfkCmdFromMatEdit", 1024 * 1024))
	{
		spdlog::warn("Failed to start EfkCmdFromMatEdit");
	}

	uint64_t previousHistoryID = 0;

	glfwMainWindow = mainWindow->GetGLFWWindows();
	bool isDpiDirtied = true;
	bool isFontUpdated = true;

	mainWindow->DpiChanged = [&](float scale) -> void { isDpiDirtied = true; isFontUpdated = true; };

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
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();

	// Specify imgui setting
	std::string imguiConfigPath = GetExecutingDirectory() + "imgui.material.ini";
	ImGui::GetIO().IniFilename = imguiConfigPath.c_str();

	if (languageKey != "")
	{
		config->Language = languageKey;
	}
	ChangeLanguage(config->Language);

	editor = std::make_shared<EffekseerMaterial::Editor>(graphics);

	keyStatePre.fill(false);
	keyState.fill(false);

	bool isFirstFrame = true;
	int framecount = 0;
	int leftWidth = 220 * mainWindow->GetDPIScale();

	while (!glfwWindowShouldClose(glfwMainWindow))
	{
		Effekseer::IO::GetInstance()->Update();

		if (isDpiDirtied)
		{
			ImGuiStyle& style = ImGui::GetStyle();

			style = ImGuiStyle();
			style.ChildRounding = 3.f;
			style.GrabRounding = 3.f;
			style.WindowRounding = 3.f;
			style.ScrollbarRounding = 3.f;
			style.FrameRounding = 3.f;
			style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
			style.ScaleAllSizes(mainWindow->GetDPIScale());

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

			isDpiDirtied = false;
		}

		if (isFontUpdated)
		{
			ImGui_ImplOpenGL3_DestroyFontsTexture();
			io.Fonts->Clear();

			Effekseer::Editor::AddFontFromFileTTF(
				EffekseerMaterial::StringContainer::GetValue("Font_Normal").c_str(),
				"resources/languages/characterTable.txt",
				EffekseerMaterial::StringContainer::GetValue("CharacterTable").c_str(),
				20,
				mainWindow->GetDPIScale());

			isFontUpdated = false;
		}

		glfwPollEvents();

		// command event
		{
			IPC::CommandData commandDataTOMaterialEditor;
			if (commandQueueToMaterialEditor_->Dequeue(&commandDataTOMaterialEditor))
			{
				if (commandDataTOMaterialEditor.Type == IPC::CommandType::Terminate)
				{
					spdlog::trace("ICP - Receive - Terminate");
					break;
				}
				else if (commandDataTOMaterialEditor.Type == IPC::CommandType::OpenMaterial)
				{
					spdlog::trace("ICP - Receive - OpenMaterial : {}", (const char*)(commandDataTOMaterialEditor.str.data()));
					editor->LoadOrSelect(commandDataTOMaterialEditor.str.data());
				}
				else if (commandDataTOMaterialEditor.Type == IPC::CommandType::OpenOrCreateMaterial)
				{
					spdlog::trace("ICP - Receive - OpenOrCreateMaterial : {}", (const char*)(commandDataTOMaterialEditor.str.data()));
					if (editor->LoadOrSelect(commandDataTOMaterialEditor.str.data()) == EffekseerMaterial::ErrorCode::NotFound)
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
			spdlog::trace("ICP - Send - NotifyUpdate : {}", material->GetPath());
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
				if (!ImGui::IsAnyItemActive())
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

#ifdef __APPLE__
					if (ImGui::MenuItem(EffekseerMaterial::StringContainer::GetValue("Save").c_str(), "Command+S"))
					{
						editor->Save();
					}

					if (ImGui::MenuItem(EffekseerMaterial::StringContainer::GetValue("SaveAs").c_str(), "Command+S"))
					{
						editor->SaveAs();
					}
#else
					if (ImGui::MenuItem(EffekseerMaterial::StringContainer::GetValue("Save").c_str(), "Ctrl+S"))
					{
						editor->Save();
					}

					if (ImGui::MenuItem(EffekseerMaterial::StringContainer::GetValue("SaveAs").c_str(), "Ctrl+S"))
					{
						editor->SaveAs();
					}

#endif

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

				if (ImGui::BeginMenu("Language"))
				{
					if (ImGui::MenuItem("Japanese"))
					{
						ChangeLanguage(Effekseer::SystemLanguage::Japanese);
						isFontUpdated = true;
					}
					else if (ImGui::MenuItem("English"))
					{
						ChangeLanguage(Effekseer::SystemLanguage::English);
						isFontUpdated = true;
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

			const bool visible = ImGui::Begin("MaterialEditor", nullptr, flags);
			ImGui::GetStyle().WindowRounding = oldWindowRounding;

			if (visible)
			{
				if (ImGui::BeginTabBar("###MainTab"))
				{
					bool isSelectedNow = editor->GetIsSelectedDirty();

					editor->ClearDirtiedSelectedFlags();

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

								if (!ImGui::IsAnyItemActive())
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
					if (ImGui::TreeNode("VS"))
					{
						ImGui::Text(uobj->GetPreview()->VS.c_str());
						ImGui::TreePop();
					}

					if (ImGui::TreeNode("PS"))
					{
						ImGui::Text(uobj->GetPreview()->PS.c_str());
						ImGui::TreePop();
					}
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

	config->Save((GetExecutingDirectory() + "config.EffekseerMaterial.json").c_str());

	Effekseer::IO::Terminate();

	Effekseer::MainWindow::Terminate();
	mainWindow = nullptr;

	spdlog::info("End MaterialEditor");

	return 0;
}

#if defined(NDEBUG) && defined(_WIN32)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpszCmdLine, int nShowCmd)
{
	return mainLoop(__argc, __argv);
}
#else
int main(int argc, char* argv[])
{
	return mainLoop(argc, argv);
}
#endif
