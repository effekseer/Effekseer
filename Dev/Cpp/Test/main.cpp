#if 1

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include <stdio.h>
#include <assert.h>
#include <vector>
#include <string>

#include "window.h"
#include "graphics.h"
#include "sound.h"

#if _WIN32

#if _DEBUG
#pragma comment(lib, "Effekseer.Debug.lib" )
#else
#pragma comment(lib, "Effekseer.Release.lib" )
#endif

#else

#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#define __NormalMode 1
#define __PerformanceCheckMode 0

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Loop();
void Init();

static const int g_window_width = 800;
static const int g_window_height = 600;

::Effekseer::Manager*					g_manager = NULL;
static ::Effekseer::Handle				g_handle = -1;
static ::Effekseer::Vector3D			g_position;
static ::Effekseer::Vector3D			g_focus;

static std::vector < ::Effekseer::Effect *>	g_effects;

#ifdef _WIN32
typedef wchar_t efchar;
typedef std::wstring efstring;
#else 
typedef uint16_t efchar;
typedef std::basic_string<uint16_t> efstring;
#endif

static efstring ToEFString(const wchar_t* src)
{
	if (sizeof(wchar_t)== 2)
	{
#ifdef _WIN32
		return efstring(src);
#else
		return efstring((uint16_t*)src);
#endif
	}
	if (sizeof(wchar_t)== 4)
	{
#ifndef _WIN32
		uint16_t temp[2048];
		int32_t length = 0;
		while (src[length] != 0 && length < 2047)
		{
			temp[length] = (uint16_t)src[length];
			length++;
		}
		temp[length] = 0;
		return efstring(temp);
#endif
	}
	return efstring();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int main()
{
	InitWindow(g_window_width, g_window_height);

	g_manager = ::Effekseer::Manager::Create( 2000 );

#if _WIN32
	InitGraphics(GetHandle(), NULL, g_window_width, g_window_height);
	InitSound( GetHandle(), NULL );
#else
	InitGraphics( GetDisplay(), GetWindow(), g_window_width, g_window_height);
	InitSound( GetDisplay(), GetWindow() );
#endif

	Init();

	while(DoEvent())
	{
		Loop();

		g_manager->Update();

		Rendering();
	}

	g_manager->Destroy();

	for (size_t i = 0; i < g_effects.size(); i++)
	{
		ES_SAFE_RELEASE(g_effects[i]);
	}

	TermSound();

	TermGraphics();

	ExitWindow();

	return 0;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void Init()
{
	g_position = ::Effekseer::Vector3D( 10.0f, 5.0f, 10.0f );
	g_focus = ::Effekseer::Vector3D( 0.0f, 0.0f, 0.0f );

	SetCameraMatrix( ::Effekseer::Matrix44().LookAtRH( g_position, g_focus, ::Effekseer::Vector3D( 0.0f, 1.0f, 0.0f ) ) );

	g_effects.push_back( Effekseer::Effect::Create( g_manager, (const EFK_CHAR*)ToEFString(L"Resource/Laser01.efk").c_str() ) );
	g_effects.push_back( Effekseer::Effect::Create( g_manager, (const EFK_CHAR*)ToEFString(L"Resource/Laser02.efk").c_str() ) );
	g_effects.push_back( Effekseer::Effect::Create( g_manager, (const EFK_CHAR*)ToEFString(L"Resource/Simple_Ribbon_Parent.efk").c_str() ) );
	g_effects.push_back( Effekseer::Effect::Create( g_manager, (const EFK_CHAR*)ToEFString(L"Resource/Simple_Ribbon_Sword.efk").c_str() ) );
	g_effects.push_back( Effekseer::Effect::Create( g_manager, (const EFK_CHAR*)ToEFString(L"Resource/Simple_Ring_Shape1.efk").c_str() ) );
	g_effects.push_back( Effekseer::Effect::Create( g_manager, (const EFK_CHAR*)ToEFString(L"Resource/Simple_Ring_Shape2.efk").c_str() ) );
	g_effects.push_back( Effekseer::Effect::Create( g_manager, (const EFK_CHAR*)ToEFString(L"Resource/Simple_Sprite_FixedYAxis.efk").c_str() ) );
	g_effects.push_back( Effekseer::Effect::Create( g_manager, (const EFK_CHAR*)ToEFString(L"Resource/Simple_Track1.efk").c_str() ) );
	g_effects.push_back( Effekseer::Effect::Create( g_manager, (const EFK_CHAR*)ToEFString(L"Resource/block.efk").c_str() ) );
	g_effects.push_back( Effekseer::Effect::Create( g_manager, (const EFK_CHAR*)ToEFString(L"Resource/block_simple.efk").c_str() ) );
	
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
	static int count = 0;

	if( count % 10 == 1 && count < 100 )
	{
		auto updateTime = g_manager->GetUpdateTime();
		auto drawTime = g_manager->GetDrawTime();

		printf("UpdateTime %d, DrawTime %d\n", updateTime, drawTime );
	}

	count++;
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void PlayEffect()
{
	if(g_effects.size() == 0) return;

#if __NormalMode
	static int target = 0;
	target = target % g_effects.size();
	g_handle = g_manager->Play( g_effects[target], 0, 0, 0 );
	target++;
	//g_manager->SetLocation( g_handle, -5.0f, 0.0f, -20.0f );

	::Effekseer::Matrix43 baseMat;
	baseMat.Scaling( 1.0f, 1.0f, 1.0f );
	g_manager->SetBaseMatrix( g_handle, baseMat );
#endif

#if __PerformanceCheckMode
	
	for( float y = -10.0f; y <= 10.0f; y += 2.0f )
	{
		for( float x = -10.0f; x <= 10.0f; x += 2.0f )
		{
			auto handle = g_manager->Play( g_effect, x, y, 0 );
			//g_manager->SetShown( handle, false );
		}
	}
	//g_manager->Play( g_effect, 0, 0, 0 );
#endif
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

#endif
