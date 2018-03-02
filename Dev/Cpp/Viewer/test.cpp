
#define GLEW_STATIC 1
#include <GL/glew.h>

#include "Effekseer.h"
#include "EffekseerTool/EffekseerTool.Renderer.h"

#include "dll.h"

#include "GUI/efk.Window.h"
#include "GUI/efk.GUIManager.h"

#include "3rdParty\imgui\imgui.h"
#include "3rdParty\imgui_glfw_gl3\imgui_impl_glfw_gl3.h"
#include "3rdParty/imgui_addon/imguidock/imguidock.h"

#include "3rdParty/nfd/nfd.h"

#ifdef _DEBUG
#pragma comment (lib,"x86/Debug/libglew32d.lib")
#else
#pragma comment (lib,"x86/Release/libglew32dlib")
#endif

int main_()
{
	auto guiManager = new efk::GUIManager();
	guiManager->Initialize(u"Effekseer", 960, 540, false);

	auto renderer = new ::EffekseerTool::Renderer(2000, false, true);
	renderer->Initialize((HWND)guiManager->GetNativeHandle(), 960, 540);

	auto manager = ::Effekseer::Manager::Create(2000);
	manager->SetSpriteRenderer(renderer->GetRenderer()->CreateSpriteRenderer());
	manager->SetRibbonRenderer(renderer->GetRenderer()->CreateRibbonRenderer());
	manager->SetTextureLoader(renderer->GetRenderer()->CreateTextureLoader());

	auto position = ::Effekseer::Vector3D(10.0f, 5.0f, 20.0f);

	renderer->GetRenderer()->SetCameraMatrix(::Effekseer::Matrix44().LookAtRH(position, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));

	char* pData = NULL;
	FILE* fp = _wfopen(L"Resource/test.efk", L"rb");
	fseek(fp, 0, SEEK_END);
	size_t size = ftell(fp);
	pData = new char[size];
	fseek(fp, 0, SEEK_SET);
	fread(pData, 1, size, fp);
	fclose(fp);

	auto effect = Effekseer::Effect::Create(manager, (void*)pData, size);

	delete[] pData;

	//auto handle = manager->Play(effect, 0, 0, 0);

	bool show_another_window = true;

	while (guiManager->DoEvents())
	{
		glClear(GL_COLOR_BUFFER_BIT);

		guiManager->ResetGUI();

		static bool open = true;
		guiManager->Begin(u"Test", &open);

		guiManager->Text(u"nya-n");

		static int v = 0;
		guiManager->InputInt(u"nn", &v);

		guiManager->Button(u"ボタン");

		guiManager->DragInt(u"nnn", &v);

		guiManager->SameLine();

		guiManager->DragInt(u"nnn", &v);

		guiManager->SliderInt(u"nnn", &v, -100, 100);

		guiManager->End();

		guiManager->RenderGUI();

		manager->Update();

		renderer->BeginRendering();
		manager->Draw();
		renderer->EndRendering();


		renderer->Present();
		guiManager->Present();
	}

	//manager->StopEffect(handle);

	ES_SAFE_RELEASE(effect);

	manager->Destroy();

	ES_SAFE_DELETE(renderer);

	guiManager->Terminate();
	ES_SAFE_DELETE(guiManager);

	return 0;
}

int main()
{
	{
		nfdchar_t *outPath = NULL;
		nfdresult_t result = NFD_OpenDialog(NULL, NULL, &outPath);

		if (result == NFD_OKAY) {
			puts("Success!");
			puts(outPath);
			free(outPath);
		}
		else if (result == NFD_CANCEL) {
			puts("User pressed cancel.");
		}
		else {
			printf("Error: %s\n", NFD_GetError());
		}

		return 0;
	}

	efk::Window* window = new efk::Window();

	window->Initialize(u"Effekseer", 960, 540, false, true);
	window->MakeCurrent();

	glewInit();

	ImGui_ImplGlfwGL3_Init(window->GetGLFWWindows(), true);
	ImGui::StyleColorsClassic();
	

	auto renderer = new ::EffekseerTool::Renderer(2000, false, true);
	renderer->Initialize((HWND)window->GetNativeHandle(), 960, 540);

	auto manager = ::Effekseer::Manager::Create(2000);
	manager->SetSpriteRenderer(renderer->GetRenderer()->CreateSpriteRenderer());
	manager->SetRibbonRenderer(renderer->GetRenderer()->CreateRibbonRenderer());
	manager->SetTextureLoader(renderer->GetRenderer()->CreateTextureLoader());

	auto position = ::Effekseer::Vector3D(10.0f, 5.0f, 20.0f);

	renderer->GetRenderer()->SetCameraMatrix(::Effekseer::Matrix44().LookAtRH(position, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));

	char* pData = NULL;
	FILE* fp = _wfopen(L"Resource/test.efk", L"rb");
	fseek(fp, 0, SEEK_END);
	size_t size = ftell(fp);
	pData = new char[size];
	fseek(fp, 0, SEEK_SET);
	fread(pData, 1, size, fp);
	fclose(fp);

	auto effect = Effekseer::Effect::Create(manager, (void*)pData, size);

	delete[] pData;

	auto handle = manager->Play(effect, 0, 0, 0);

	bool show_another_window = true;

	while (window->DoEvents())
	{
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplGlfwGL3_NewFrame();

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
				if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
				ImGui::Separator();
				if (ImGui::MenuItem("Cut", "CTRL+X")) {}
				if (ImGui::MenuItem("Copy", "CTRL+C")) {}
				if (ImGui::MenuItem("Paste", "CTRL+V")) {}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
		ImGui::SetNextWindowPos(ImVec2(0, 30));
		const ImGuiWindowFlags flags = (ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
		const float oldWindowRounding = ImGui::GetStyle().WindowRounding; ImGui::GetStyle().WindowRounding = 0;
		const bool visible = ImGui::Begin("imguidock window (= lumix engine's dock system)", NULL, ImVec2(0, 0), 1.0f, flags);
		ImGui::GetStyle().WindowRounding = oldWindowRounding;
		if (visible) {
			ImGui::BeginDockspace();
			static char tmp[128];
			for (int i = 0; i<10; i++) {
				sprintf(tmp, "Dock %d", i);
				if (i == 9) ImGui::SetNextDock(ImGuiDockSlot_Bottom);// optional
				if (ImGui::BeginDock(tmp)) {
					ImGui::Text("Content of dock window %d goes here", i);
				}
				ImGui::EndDock();
			}
			ImGui::EndDockspace();
		}
		ImGui::End();
		

		ImGui::Begin("Another Window", &show_another_window);
		ImGui::Text("Hello from another window!");
		ImGui::End();

		ImGui::Render();

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);

		auto bit = GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
		glClearDepth(1.0f);
		glClear(bit);

		//manager->Update();
		//
		//renderer->BeginRendering();
		//manager->Draw();
		//renderer->EndRendering();


		renderer->Present();
		window->Present();
	}

	manager->StopEffect(handle);

	ES_SAFE_RELEASE(effect);

	manager->Destroy();

	ES_SAFE_DELETE(renderer);

	ImGui_ImplGlfwGL3_Shutdown();

	window->MakeNone();
	window->Terminate();
	ES_SAFE_DELETE(window);

	return 0;
}

int main__()
{
	efk::Window* window = new efk::Window();

	window->Initialize(u"Effekseer", 960, 540, false, false);

	auto renderer = new ::EffekseerTool::Renderer(2000, false, false);
	renderer->Initialize((HWND)window->GetNativeHandle(), 960, 540);

	auto manager = ::Effekseer::Manager::Create(2000);
	manager->SetSpriteRenderer(renderer->GetRenderer()->CreateSpriteRenderer());
	manager->SetRibbonRenderer(renderer->GetRenderer()->CreateRibbonRenderer());
	manager->SetTextureLoader(renderer->GetRenderer()->CreateTextureLoader());

	auto position = ::Effekseer::Vector3D(10.0f, 5.0f, 20.0f);

	renderer->GetRenderer()->SetCameraMatrix(::Effekseer::Matrix44().LookAtRH(position, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));

	char* pData = NULL;
	FILE* fp = _wfopen(L"Resource/test.efk", L"rb");
	fseek(fp, 0, SEEK_END);
	size_t size = ftell(fp);
	pData = new char[size];
	fseek(fp, 0, SEEK_SET);
	fread(pData, 1, size, fp);
	fclose(fp);

	auto effect = Effekseer::Effect::Create(manager, (void*)pData, size);

	delete[] pData;

	auto handle = manager->Play(effect, 0, 0, 0);

	bool show_another_window = true;

	while (window->DoEvents())
	{
		manager->Update();

		renderer->BeginRendering();
		manager->Draw();
		renderer->EndRendering();


		renderer->Present();
		window->Present();
	}

	manager->StopEffect(handle);

	ES_SAFE_RELEASE(effect);

	manager->Destroy();

	ES_SAFE_DELETE(renderer);

	window->Terminate();
	ES_SAFE_DELETE(window);

	return 0;
}
