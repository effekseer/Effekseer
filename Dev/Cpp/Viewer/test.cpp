
#ifdef _WIN32
#define GLEW_STATIC 1
#include <GL/glew.h>
#endif

#include "Effekseer.h"
#include "EffekseerTool/EffekseerTool.Renderer.h"

#include "dll.h"

#include "GUI/efk.Window.h"
#include "GUI/efk.GUIManager.h"

#include "3rdParty/imgui/imgui.h"
#include "3rdParty/imgui_platform/imgui_impl_opengl3.h"
#include "3rdParty/imgui_platform/imgui_impl_glfw.h"

#include "3rdParty/imgui_addon/imguidock/imguidock.h"

#include "3rdParty/nfd/nfd.h"


int main_()
{
	auto guiManager = new efk::GUIManager();
	guiManager->Initialize(u"Effekseer", 960, 540, efk::DeviceType::OpenGL, false);

	auto renderer = new ::EffekseerTool::Renderer(2000, false, efk::DeviceType::OpenGL);
	renderer->Initialize(guiManager->GetNativeHandle(), 960, 540);

	auto manager = ::Effekseer::Manager::Create(2000);
	manager->SetSpriteRenderer(renderer->GetRenderer()->CreateSpriteRenderer());
	manager->SetRibbonRenderer(renderer->GetRenderer()->CreateRibbonRenderer());
	manager->SetTextureLoader(renderer->GetRenderer()->CreateTextureLoader());

	auto position = ::Effekseer::Vector3D(10.0f, 5.0f, 20.0f);

	renderer->GetRenderer()->SetCameraMatrix(::Effekseer::Matrix44().LookAtRH(position, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));

	char* pData = NULL;
	FILE* fp = fopen("Resource/test.efk", "rb");
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
	efk::Window* window = new efk::Window();

	window->Initialize(u"Effekseer", 960, 540, false, efk::DeviceType::OpenGL);
	window->MakeCurrent();

#ifdef _WIN32
	glewInit();
#endif
    
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window->GetGLFWWindows(), true);
	ImGui_ImplOpenGL3_Init(nullptr);
	ImGui::StyleColorsClassic();
	

	auto renderer = new ::EffekseerTool::Renderer(2000, false, efk::DeviceType::OpenGL);
	renderer->Initialize(window->GetNativeHandle(), 960, 540);

	auto manager = ::Effekseer::Manager::Create(2000);
	manager->SetSpriteRenderer(renderer->GetRenderer()->CreateSpriteRenderer());
	manager->SetRibbonRenderer(renderer->GetRenderer()->CreateRibbonRenderer());
	manager->SetTextureLoader(renderer->GetRenderer()->CreateTextureLoader());

	auto position = ::Effekseer::Vector3D(10.0f, 5.0f, 20.0f);

	renderer->GetRenderer()->SetCameraMatrix(::Effekseer::Matrix44().LookAtRH(position, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));

    Effekseer::Effect* effect = nullptr;
    
	char* pData = NULL;
	FILE* fp = fopen("Resource/test.efk", "rb");
	
    if(fp != nullptr)
    {
        fseek(fp, 0, SEEK_END);
        size_t size = ftell(fp);
        pData = new char[size];
        fseek(fp, 0, SEEK_SET);
        fread(pData, 1, size, fp);
        fclose(fp);

        effect = Effekseer::Effect::Create(manager, (void*)pData, size);

        delete[] pData;
    }
    
	auto handle = manager->Play(effect, 0, 0, 0);

	bool show_another_window = true;

#ifndef _WIN32
    //GLuint vao;
    //glGenVertexArrays(1, &vao);
#endif
    
	while (window->DoEvents())
	{
	   	glClear(GL_COLOR_BUFFER_BIT);
         glEnable(GL_DEPTH_TEST);
         glDepthMask(GL_TRUE);
         
         auto bit = GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
         glClearDepth(1.0f);
         glClear(bit);
        
#ifndef _WIN32
         //glBindVertexArray(vao);
#endif
        
         manager->Update();
         
         renderer->BeginRendering();
         manager->Draw();
         renderer->EndRendering();
        
#ifndef _WIN32
         //glBindVertexArray(0);
#endif
        
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

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

        /*
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
		*/

		ImGui::Begin("Another Window", &show_another_window);
		if (ImGui::TreeNode("Tree"))
		{
			if (ImGui::TreeNode("Tree1"))
			{
				if (ImGui::BeginChild("Child1"))
				{
					for (int32_t i = 0; i < 10; i++)
					{
						ImGui::Text("aa");
					}
					
					ImGui::EndChild();
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Tree2"))
			{
				if (ImGui::BeginChildFrame(ImGui::GetID("Child2"), ImVec2(0,100), ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
				{
					for (int32_t i = 0; i < 10; i++)
					{
						ImGui::Text("aa");
					}

					ImGui::EndChildFrame();
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Tree3"))
			{
				ImGui::TreePop();
			}

			ImGui::TreePop();
		}
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
		renderer->Present();
		window->Present();
	}

	manager->StopEffect(handle);

	ES_SAFE_RELEASE(effect);

	manager->Destroy();

	ES_SAFE_DELETE(renderer);

	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();

	window->MakeNone();
	window->Terminate();
	ES_SAFE_DELETE(window);

	return 0;
}

int main__()
{
	efk::Window* window = new efk::Window();

	window->Initialize(u"Effekseer", 960, 540, false, efk::DeviceType::OpenGL);

	auto renderer = new ::EffekseerTool::Renderer(2000, false, efk::DeviceType::OpenGL);
	renderer->Initialize(window->GetNativeHandle(), 960, 540);

	auto manager = ::Effekseer::Manager::Create(2000);
	manager->SetSpriteRenderer(renderer->GetRenderer()->CreateSpriteRenderer());
	manager->SetRibbonRenderer(renderer->GetRenderer()->CreateRibbonRenderer());
	manager->SetTextureLoader(renderer->GetRenderer()->CreateTextureLoader());

	auto position = ::Effekseer::Vector3D(10.0f, 5.0f, 20.0f);

	renderer->GetRenderer()->SetCameraMatrix(::Effekseer::Matrix44().LookAtRH(position, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));

	char* pData = NULL;
	FILE* fp = fopen("Resource/test.efk", "rb");
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
