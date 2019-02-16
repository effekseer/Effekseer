
#ifndef	__EFFEKSEER_EFFECT_IMPLEMENTED_H__
#define	__EFFEKSEER_EFFECT_IMPLEMENTED_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"
#include "Effekseer.Effect.h"
#include "Effekseer.Vector3D.h"

#include <assert.h>
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{

/**
	@brief	A class to backup resorces when effect is reloaded
*/
class EffectReloadingBackup
{
public:
	template<class T> 
	class Holder
	{
	public:
		T value;
		int counter = 0;
	};

	template<class T>
	class HolderCollection
	{
		std::map<std::u16string, Holder<T>> collection;

	public:
		void Push(const char16_t* key, T value)
		{
			auto key_ = std::u16string(key);
			auto it = collection.find(key_);
			
			if (it == collection.end())
			{
				collection[key_].value = value;
				collection[key_].counter = 1;
			}
			else
			{
				assert(it->second.value == value);
				it->second.counter++;
			}
		}

		bool Pop(const char16_t* key, T& value)
		{
			auto key_ = std::u16string(key);
			auto it = collection.find(key_);

			if (it == collection.end())
			{
				return false;
			}
			else
			{
				it->second.counter--;
				value = it->second.value;
				if (it->second.counter == 0)
				{
					collection.erase(it);
				}
				return true;
			}
		}

		std::map<std::u16string, Holder<T>>& GetCollection()
		{
			return collection;
		}
	};

	HolderCollection<TextureData*> images;
	HolderCollection<TextureData*> normalImages;
	HolderCollection<TextureData*> distortionImages;
	HolderCollection<void*> sounds;
	HolderCollection<void*> models;
};

/**
	@brief	エフェクトパラメーター
	@note
	エフェクトに設定されたパラメーター。
*/

class EffectImplemented
	: public Effect
	, public ReferenceObject
{
	friend class ManagerImplemented;
	friend class EffectNodeImplemented;
private:
	ManagerImplemented* m_pManager;

	Setting*	m_setting;

	mutable std::atomic<int32_t> m_reference;

	int	m_version;

	int	m_ImageCount;
	EFK_CHAR**		m_ImagePaths;
	TextureData**	m_pImages;

	int	m_normalImageCount;
	EFK_CHAR**		m_normalImagePaths;
	TextureData**	m_normalImages;
	
	int	m_distortionImageCount;
	EFK_CHAR**		m_distortionImagePaths;
	TextureData**	m_distortionImages;

	int	m_WaveCount;
	EFK_CHAR**		m_WavePaths;
	void**			m_pWaves;

	int32_t	m_modelCount;
	EFK_CHAR**		m_modelPaths;
	void**			m_pModels;

	std::u16string name_;
	std::basic_string<EFK_CHAR>		m_materialPath;

	int32_t			renderingNodesCount = 0;
	int32_t			renderingNodesThreshold = 0;

	/* 拡大率 */
	float	m_maginification;

	float	m_maginificationExternal;

	// default random seed
	int32_t	m_defaultRandomSeed;

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

	//! a flag to reload
	bool isReloadingOnRenderingThread = false;

	//! backup to reload on rendering thread
	std::unique_ptr<EffectReloadingBackup> reloadingBackup;

	void ResetReloadingBackup();

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
	EffectNode* GetRoot() const override;

	float GetMaginification() const override;

	bool Load( void* pData, int size, float mag, const EFK_CHAR* materialPath, ReloadingThreadType reloadingThreadType);

	/**
		@breif	何も読み込まれていない状態に戻す
	*/
	void Reset();

	/**
		@brief	Compatibility for magnification.
	*/
	bool IsDyanamicMagnificationValid() const;

private:
	/**
		@brief	マネージャー取得
	*/
	Manager* GetManager() const;

public:
	const char16_t* GetName() const override;

	void SetName(const char16_t* name) override;

	/**
	@brief	設定取得
	*/
	Setting* GetSetting() const override;
	
	/**
		@brief	エフェクトデータのバージョン取得
	*/
	int GetVersion() const override;

	/**
		@brief	格納されている画像のポインタを取得する。
	*/
	TextureData* GetColorImage(int n) const override;

	/**
		@brief	格納されている画像のポインタの個数を取得する。
	*/
	int32_t GetColorImageCount() const override;

	/**
	@brief	格納されている画像のポインタを取得する。
	*/
	TextureData* GetNormalImage(int n) const override;

	int32_t GetNormalImageCount() const override;

	TextureData* GetDistortionImage(int n) const override;

	int32_t GetDistortionImageCount() const override;

	/**
		@brief	格納されている音波形のポインタを取得する。
	*/
	void* GetWave( int n ) const override;

	int32_t GetWaveCount() const override;

	/**
		@brief	格納されているモデルのポインタを取得する。
	*/
	void* GetModel( int n ) const override;

	int32_t GetModelCount() const override;

	/**
		@brief	エフェクトのリロードを行う。
	*/
	bool Reload( void* data, int32_t size, const EFK_CHAR* materialPath, ReloadingThreadType reloadingThreadType) override;

	/**
		@brief	エフェクトのリロードを行う。
	*/
	bool Reload( const EFK_CHAR* path, const EFK_CHAR* materialPath, ReloadingThreadType reloadingThreadType) override;

	/**
		@brief	エフェクトのリロードを行う。
	*/
	bool Reload( Manager** managers, int32_t managersCount, void* data, int32_t size, const EFK_CHAR* materialPath, ReloadingThreadType reloadingThreadType) override;

	/**
		@brief	エフェクトのリロードを行う。
	*/
	bool Reload( Manager** managers, int32_t managersCount, const EFK_CHAR* path, const EFK_CHAR* materialPath, ReloadingThreadType reloadingThreadType) override;

	/**
		@brief	画像等リソースの再読み込みを行う。
	*/
	void ReloadResources( const EFK_CHAR* materialPath ) override;

	void UnloadResources(const EFK_CHAR* materialPath);

	void UnloadResources() override;

	virtual int GetRef() override { return ReferenceObject::GetRef(); }
	virtual int AddRef() override { return ReferenceObject::AddRef(); }
	virtual int Release() override { return ReferenceObject::Release(); }
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_EFFECT_IMPLEMENTED_H__
