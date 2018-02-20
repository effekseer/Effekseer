
#include "Effekseer.h"
#include "EffekseerRenderer/EffekseerRendererDX9.Renderer.h"
#include "EffekseerTool/EffekseerTool.Renderer.h"

#include "Window/efk.Window.h"

int main()
{
	efk::Window* window = new efk::Window();

	window->Initialize(u"Effekseer", 960, 540, false, true);
	window->MakeCurrent();

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

	window->MakeNone();
	window->Terminate();
	ES_SAFE_DELETE(window);

	return 0;
}
