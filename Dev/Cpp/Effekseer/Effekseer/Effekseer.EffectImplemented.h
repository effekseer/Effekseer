
#ifndef	__EFFEKSEER_EFFECT_IMPLEMENTED_H__
#define	__EFFEKSEER_EFFECT_IMPLEMENTED_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"
#include "Effekseer.Effect.h"
#include "Effekseer.Vector3D.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

/**
	@brief	エフェクトパラメーター
	@note
	エフェクトに設定されたパラメーター。
*/

class EffectImplemented
	: public Effect
{
	friend class ManagerImplemented;
private:
	ManagerImplemented* m_pManager;

	Setting*	m_setting;

	int	m_reference;

	int	m_version;

	int	m_ImageCount;
	EFK_CHAR**		m_ImagePaths;
	void**			m_pImages;

	int	m_normalImageCount = 0;
	EFK_CHAR**		m_normalImagePaths = nullptr;
	void**			m_normalImages = nullptr;
	
	int	m_WaveCount;
	EFK_CHAR**		m_WavePaths;
	void**			m_pWaves;

	int32_t	m_modelCount;
	EFK_CHAR**		m_modelPaths;
	void**			m_pModels;

	std::basic_string<EFK_CHAR>		m_materialPath;

	/* 拡大率 */
	float	m_maginification;

	float	m_maginificationExternal;

	// 子ノード
	EffectNode* m_pRoot;

	/* カリング */
	struct
	{
		CullingShape	Shape;
		Vector3D		Location;

		union
		{
			struct
			{
			} None;

			struct
			{
				float Radius;
			} Sphere;
		};

	} Culling;


public:
	/**
		@brief	生成
	*/
	static Effect* Create( Manager* pManager, void* pData, int size, float magnification, const EFK_CHAR* materialPath = NULL );

	/**
		@brief	生成
	*/
	static Effect* Create( Setting* setting, void* pData, int size, float magnification, const EFK_CHAR* materialPath = NULL );

	// コンストラクタ
	EffectImplemented( Manager* pManager, void* pData, int size );

	// コンストラクタ
	EffectImplemented( Setting* setting, void* pData, int size );

	// デストラクタ
	virtual ~EffectImplemented();

	// Rootの取得
	EffectNode* GetRoot() const;

	/* 拡大率の取得 */
	float GetMaginification() const;

	/**
		@brief	読み込む。
	*/
	void Load( void* pData, int size, float mag, const EFK_CHAR* materialPath );

	/**
		@breif	何も読み込まれていない状態に戻す
	*/
	void Reset();

	/**
		@brief	参照カウンタ加算
	*/
	int AddRef();

	/**
		@brief	参照カウンタ減算
	*/
	int Release();

private:
	/**
		@brief	マネージャー取得
	*/
	Manager* GetManager() const;

public:
	/**
	@brief	設定取得
	*/
	Setting* GetSetting() const;
	
	/**
		@brief	エフェクトデータのバージョン取得
	*/
	int GetVersion() const;

	/**
		@brief	格納されている画像のポインタを取得する。
	*/
	void* GetColorImage(int n) const;

	/**
	@brief	格納されている画像のポインタを取得する。
	*/
	void* GetNormalImage(int n) const;
	
	/**
		@brief	格納されている音波形のポインタを取得する。
	*/
	void* GetWave( int n ) const;

	/**
		@brief	格納されているモデルのポインタを取得する。
	*/
	void* GetModel( int n ) const;

	/**
		@brief	エフェクトのリロードを行う。
	*/
	bool Reload( void* data, int32_t size, const EFK_CHAR* materialPath = NULL );

	/**
		@brief	エフェクトのリロードを行う。
	*/
	bool Reload( const EFK_CHAR* path, const EFK_CHAR* materialPath = NULL );

	/**
		@brief	エフェクトのリロードを行う。
	*/
	bool Reload( Manager* managers, int32_t managersCount, void* data, int32_t size, const EFK_CHAR* materialPath = NULL );

	/**
		@brief	エフェクトのリロードを行う。
	*/
	bool Reload( Manager* managers, int32_t managersCount, const EFK_CHAR* path, const EFK_CHAR* materialPath = NULL );

	/**
		@brief	画像等リソースの再読み込みを行う。
	*/
	void ReloadResources( const EFK_CHAR* materialPath );

	/**
		@brief	画像等リソースの破棄を行う。
	*/
	void UnloadResources();
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_EFFECT_IMPLEMENTED_H__
