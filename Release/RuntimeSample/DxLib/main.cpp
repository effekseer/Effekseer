
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include "DxLib.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib" )
#pragma comment(lib, "d3dx9.lib" )

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#include <Effekseer.h>
#include <EffekseerRendererDX9.h>

#if _DEBUG
#pragma comment(lib, "Effekseer.Debug.lib" )
#pragma comment(lib, "EffekseerRendererDX9.Debug.lib" )
#else
#pragma comment(lib, "Effekseer.Release.lib" )
#pragma comment(lib, "EffekseerRendererDX9.Release.lib" )
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
static int g_window_width = 640;
static int g_window_height = 480;
static ::Effekseer::Manager*			g_manager = NULL;
static ::EffekseerRenderer::Renderer*	g_renderer = NULL;
static ::Effekseer::Effect*				g_effect = NULL;
static ::Effekseer::Handle				g_handle = -1;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void DeviceLostFunction( void *Data )
{
	printf("Start LostFunction\n");
	// デバイスロストが発生した時に呼ぶ。
	g_renderer->OnLostDevice();

	// 読み込んだエフェクトのリソースは全て破棄する。
	g_effect->UnloadResources();

	// DXライブラリは内部でデバイス自体を消去しているのでNULLを設定する。
	g_renderer->ChangeDevice( NULL );

	printf("End LostFunction\n");
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void DeviceRestoreFunction( void *Data )
{
	printf("Start RestoreFunction\n");

	// DXライブラリは回復時に内部でデバイスを再生成するので新しく設定する。
	LPDIRECT3DDEVICE9 device = (LPDIRECT3DDEVICE9)GetUseDirect3DDevice9();
	g_renderer->ChangeDevice( device );

	// エフェクトのリソースを再読み込みする。
	g_effect->ReloadResources();

	// デバイスが復帰するときに呼ぶ
	g_renderer->OnResetDevice();
	printf("End RestoreFunction\n");
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
int main()
{
	// ウィンドウモードに変更
	ChangeWindowMode( true );					

	// 初期化
    if( DxLib_Init() == -1 || SetDrawScreen(DX_SCREEN_BACK) != 0 ) return -1;
	
	// 描画用インスタンスの生成
	LPDIRECT3DDEVICE9 device = (LPDIRECT3DDEVICE9)GetUseDirect3DDevice9();
	g_renderer = ::EffekseerRenderer::Renderer::Create( device, 2000 );

	// フルスクリーンウインドウの切り替えでリソースが消えるのを防ぐ
	SetChangeScreenModeGraphicsSystemResetFlag( FALSE );

	// エフェクト管理用インスタンスの生成
	g_manager = ::Effekseer::Manager::Create( 2000 );

	// 描画用インスタンスから描画機能を設定
	g_manager->SetSpriteRenderer( g_renderer->CreateSpriteRenderer() );
	g_manager->SetRibbonRenderer( g_renderer->CreateRibbonRenderer() );
	g_manager->SetRingRenderer( g_renderer->CreateRingRenderer() );
	g_manager->SetModelRenderer( g_renderer->CreateModelRenderer() );
	g_manager->SetTrackRenderer( g_renderer->CreateTrackRenderer() );

	// 描画用インスタンスからテクスチャの読込機能を設定
	// 独自拡張可能、現在はファイルから読み込んでいる。
	g_manager->SetTextureLoader( g_renderer->CreateTextureLoader() );
	g_manager->SetModelLoader( g_renderer->CreateModelLoader() );

	// デバイスロスト時のコールバック設定(フルスクリーンウインドウ切り替えのために必要)
	SetGraphicsDeviceLostCallbackFunction( DeviceLostFunction, NULL );
	SetGraphicsDeviceRestoreCallbackFunction( DeviceRestoreFunction, NULL );

	// 投影行列を設定
	g_renderer->SetProjectionMatrix(
		::Effekseer::Matrix44().PerspectiveFovRH( 90.0f / 180.0f * 3.14f, (float)g_window_width / (float)g_window_height, 1.0f, 50.0f ) );
	
	// カメラ行列を設定
	g_renderer->SetCameraMatrix(
		::Effekseer::Matrix44().LookAtRH( ::Effekseer::Vector3D( 10.0f, 5.0f, 20.0f ), ::Effekseer::Vector3D( 0.0f, 0.0f, 0.0f ), ::Effekseer::Vector3D( 0.0f, 1.0f, 0.0f ) ) );
	
	// エフェクトの読込
	g_effect = Effekseer::Effect::Create( g_manager, (const EFK_CHAR*)L"test.efk" );

	// 何でもいいから画像読込
	int grHandle = LoadGraph( L"Texture/Particle01.png" );

	// 時間の設定
	int time = 0;

	bool isFullScreen = false;

	while( !ProcessMessage() && !ClearDrawScreen() && !CheckHitKey(KEY_INPUT_ESCAPE) )
	{
		// 定期的にエフェクトを再生
		if( time % 60 == 0 )
		{
			// エフェクトの再生
			g_handle = g_manager->Play( g_effect, 0, 0, 0 );
		}

		// 何でもいいのでTransFragを有効にして画像を描画する。
		// こうして描画した後でないと、Effekseerは描画できない。
		DrawGraph( 0, 0, grHandle, TRUE );

		// 頂点バッファに溜まった頂点データを吐き出す
		RenderVertex();
		
		// エフェクトの移動処理を行う
		g_manager->AddLocation( g_handle, ::Effekseer::Vector3D( 0.2f, 0.0f, 0.0f ) );

		// エフェクトの更新処理を行う
		g_manager->Update();
			
		// エフェクトの描画開始処理を行う。
		g_renderer->BeginRendering();

		// エフェクトの描画を行う。
		g_manager->Draw();

		// エフェクトの描画終了処理を行う。
		g_renderer->EndRendering();
		
		// DXライブラリの設定を戻す。
		RefreshDxLibDirect3DSetting();

		// スクリーン入れ替え
		ScreenFlip();

		// 時間経過
		time++;

		// フルスクリーン切り替え
		if( CheckHitKey(KEY_INPUT_F1) && !isFullScreen )
		{
			ChangeWindowMode( FALSE );
			SetDrawScreen(DX_SCREEN_BACK);
			isFullScreen = true;
		}
		if( CheckHitKey(KEY_INPUT_F2) && isFullScreen )
		{
			ChangeWindowMode( TRUE );
			SetDrawScreen(DX_SCREEN_BACK);
			isFullScreen = false;
		}
	}

	// エフェクトの破棄
	ES_SAFE_RELEASE( g_effect );

	// エフェクト管理用インスタンスを破棄
	g_manager->Destroy();

	// 描画用インスタンスを破棄
	g_renderer->Destory();

	DxLib_End();

	return 0;
}
