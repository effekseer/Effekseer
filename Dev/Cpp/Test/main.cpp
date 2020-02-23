#if 1

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include <stdio.h>
#include <assert.h>
#include <vector>
#include <string>
#include <memory>

#include "graphics.h"
#include "sound.h"
#include "common.h"
#include "../Effekseer/Effekseer/IO/Effekseer.EfkEfcFactory.h"
#include "Runtime/RuntimeTest.h"

#if _WIN32

#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>
#include <crtdbg.h>

#ifndef __EFFEKSEER_TEST_BUILD_AS_CMAKE__
#if _DEBUG
#pragma comment(lib, "x86/Effekseer.Debug.lib" )
#else
#pragma comment(lib, "x86/Effekseer.Release.lib" )
#endif

//#if _DEBUG
//#pragma comment(lib, "x86/EffekseerRendererArea.Debug.lib")
//#else
//#pragma comment(lib, "x86/EffekseerRendererArea.Release.lib")
//#endif

#endif

#else

#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#define __NormalMode 0
#define __PerformanceCheckMode 1

#define __DDS_TEST 0

#define __CULLING_TEST 0

#define __SOUND_TEST 0

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Loop();
void Init();

struct TestManager
{
	std::vector < ::Effekseer::Effect *>	effects;
};

static const int g_window_width = 800;
static const int g_window_height = 600;

::Effekseer::Manager*					g_manager = NULL;
static ::Effekseer::Handle				g_handle = -1;
static ::Effekseer::Vector3D			g_position;
static ::Effekseer::Vector3D			g_focus;

std::unique_ptr<TestManager> testManager;

void TestManagerPlayAndStop()
{
	{
		auto manager = ::Effekseer::Manager::Create(2000);
		auto effect = Effekseer::Effect::Create(manager, EFK_LOCALFILE(u"Resource/Laser01.efk"));
		manager->Play(effect, Effekseer::Vector3D());
		manager->Destroy();
		effect->Release();
	}

	{
		auto manager = ::Effekseer::Manager::Create(2000);
		auto effect = Effekseer::Effect::Create(manager, EFK_LOCALFILE(u"Resource/Laser01.efk"));
		manager->Play(effect, Effekseer::Vector3D());
		manager->Flip();
		manager->Destroy();
		effect->Release();
	}
}

void TestShowEfcAssets() 
{ 
	Effekseer::EfkEfcProperty prop;
	
	    {
		FILE* fp = nullptr;

#if _WIN32
		fopen_s(&fp, "Resource/em.efkefc", "rb");
#else
		fp = fopen("Resource/em.efkefc", "rb");
#endif
		if (fp == nullptr)
			return;

		fseek(fp, 0, SEEK_END);
		auto size = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		std::vector<uint8_t> data;
		data.resize(size);
		fread(data.data(), size, 1, fp);
		fclose(fp);

		prop.Load(data.data(), data.size());
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int main()
{
#if _WIN32
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

#ifdef __EFFEKSEER_TEST_BUILD_AS_CMAKE__
	StringAndPathHelperTest();
	StartingFrameTest();
	UpdateHandleTest();
	PlaybackSpeedTest();
	CustomAllocatorTest();
	//SIMDTest();
	BasicRuntimeTest();
	UpdateHandleTest();
	//BasicRuntimeDeviceLostTest();
#else

	//TestShowEfcAssets();
	
	testManager = std::unique_ptr<TestManager>(new TestManager());
	
	TestManagerPlayAndStop();
	
	g_manager = ::Effekseer::Manager::Create(32768);

#if __CULLING_TEST
	g_manager->CreateCullingWorld(200, 200, 200, 4);
#endif

#if _WIN32
	InitGraphics(g_window_width, g_window_height);
#if __SOUND_TEST
	InitSound();
#endif

#else
	InitGraphics( g_window_width, g_window_height);

#if __SOUND_TEST
	InitSound();
#endif

#endif

	Init();

	while(DoEvent())
	{
		Loop();

		g_manager->Update();

		Rendering();
	}

	for (size_t i = 0; i < testManager->effects.size(); i++)
	{
		ES_SAFE_RELEASE(testManager->effects[i]);
	}

#if __SOUND_TEST
	TermSound();
#endif

	TermGraphics();

	g_manager->Destroy();

	testManager.reset();
#endif

#if _WIN32
	//_CrtDumpMemoryLeaks();
#endif

	return 0;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Init()
{
	g_position = ::Effekseer::Vector3D( 10.0f, 5.0f, 10.0f ) / 1.0f;
	g_focus = ::Effekseer::Vector3D( 0.0f, 0.0f, 0.0f );

	SetCameraMatrix( ::Effekseer::Matrix44().LookAtRH( g_position, g_focus, ::Effekseer::Vector3D( 0.0f, 1.0f, 0.0f ) ) );

#if __DDS_TEST
	testManager->effects.push_back( Effekseer::Effect::Create( g_manager, EFK_LOCALFILE(u"Resource/Laser01_dds.efk") ) );
#elif __CULLING_TEST
	testManager->effects.push_back( Effekseer::Effect::Create( g_manager, EFK_LOCALFILE(u"Resource/culling.efk") ) );
#elif __PerformanceCheckMode
	testManager->effects.push_back( Effekseer::Effect::Create( g_manager, EFK_LOCALFILE(u"Resource/Benediction.efk") ) );
#else
	testManager->effects.push_back( Effekseer::Effect::Create( g_manager, EFK_LOCALFILE(u"Resource/Laser01.efk") ) );
	testManager->effects.push_back( Effekseer::Effect::Create( g_manager, EFK_LOCALFILE(u"Resource/Laser02.efk") ) );
	testManager->effects.push_back( Effekseer::Effect::Create( g_manager, EFK_LOCALFILE(u"Resource/Simple_Ribbon_Parent.efk") ) );
	testManager->effects.push_back( Effekseer::Effect::Create( g_manager, EFK_LOCALFILE(u"Resource/Simple_Ribbon_Sword.efk") ) );
	testManager->effects.push_back( Effekseer::Effect::Create( g_manager, EFK_LOCALFILE(u"Resource/Simple_Ring_Shape1.efk") ) );
	testManager->effects.push_back( Effekseer::Effect::Create( g_manager, EFK_LOCALFILE(u"Resource/Simple_Ring_Shape2.efk") ) );
	testManager->effects.push_back( Effekseer::Effect::Create( g_manager, EFK_LOCALFILE(u"Resource/Simple_Sprite_FixedYAxis.efk") ) );
	testManager->effects.push_back( Effekseer::Effect::Create( g_manager, EFK_LOCALFILE(u"Resource/Simple_Track1.efk") ) );
	testManager->effects.push_back( Effekseer::Effect::Create( g_manager, EFK_LOCALFILE(u"Resource/block.efk") ) );
	testManager->effects.push_back( Effekseer::Effect::Create( g_manager, EFK_LOCALFILE(u"Resource/block_simple.efk") ) );
	testManager->effects.push_back( Effekseer::Effect::Create( g_manager, EFK_LOCALFILE(u"Resource/Simple_Distortion.efk") ) );
#endif

	PlayEffect();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Loop()
{
#if __NormalMode
	//g_manager->AddLocation( g_handle, ::Effekseer::Vector3D( 0.2f, 0.0f, 0.0f ) );
#endif

#if __PerformanceCheckMode
	int32_t instanceCount = g_manager->GetTotalInstanceCount();
	if( instanceCount > 0 )
	{
		int32_t updateTime = g_manager->GetUpdateTime();
		int32_t drawTime = g_manager->GetDrawTime();

		printf("%d\t%d\t%d\n", instanceCount, updateTime, drawTime );
	}
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void PlayEffect()
{
	if(testManager->effects.size() == 0) return;

#if __NormalMode
	static int target = 0;
	target = target % testManager->effects.size();
	g_handle = g_manager->Play(testManager->effects[target], 0, 0, 0 );
	target++;
	//g_manager->SetLocation( g_handle, -5.0f, 0.0f, -20.0f );

	::Effekseer::Matrix43 baseMat;
	baseMat.Scaling( 1.0f, 1.0f, 1.0f );
	g_manager->SetBaseMatrix( g_handle, baseMat );
#endif

#if __PerformanceCheckMode
	static int target = 0;
	printf("-------- Performance Mesuring --------\n");
	printf("Instances\tUpdate\tDraw\n");
	for( float y = -2.0f; y <= 2.0f; y += 2.0f )
	{
		for( float x = -2.0f; x <= 2.0f; x += 2.0f )
		{
			auto handle = g_manager->Play( testManager->effects[target], x, y, 0 );
			//g_manager->SetShown( handle, false );
		}
	}
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void CreateCheckeredPattern( int width, int height, uint32_t* pixels )
{
	const uint32_t color[2]= { 0x00202020, 0x00808080 };

	for( int y = 0; y < height; y++ )
	{
		for( int x = 0; x < width; x++ )
		{
			*pixels++ = color[(x / 20 % 2) ^ (y / 20 % 2)];
		}
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

#endif
