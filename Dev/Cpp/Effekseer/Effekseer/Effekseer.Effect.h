
#ifndef	__EFFEKSEER_EFFECT_H__
#define	__EFFEKSEER_EFFECT_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "Effekseer.Base.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace Effekseer
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

/**
	@brief	エフェクトパラメータークラス
	@note
	エフェクトに設定されたパラメーター。
*/
class Effect
	: public IReference
{
protected:
	Effect() {}
    virtual ~Effect() {}

public:

	/**
		@brief	エフェクトを生成する。
		@param	manager			[in]	管理クラス
		@param	data			[in]	データ配列の先頭のポインタ
		@param	size			[in]	データ配列の長さ
		@param	magnification	[in]	読み込み時の拡大率
		@param	materialPath	[in]	素材ロード時の基準パス
		@return	エフェクト。失敗した場合はNULLを返す。
	*/
	static Effect* Create( Manager* manager, void* data, int32_t size, float magnification = 1.0f, const EFK_CHAR* materialPath = NULL );

	/**
		@brief	エフェクトを生成する。
		@param	manager			[in]	管理クラス
		@param	path			[in]	読込元のパス
		@param	magnification	[in]	読み込み時の拡大率
		@param	materialPath	[in]	素材ロード時の基準パス
		@return	エフェクト。失敗した場合はNULLを返す。
	*/
	static Effect* Create( Manager* manager, const EFK_CHAR* path, float magnification = 1.0f, const EFK_CHAR* materialPath = NULL );

		/**
		@brief	エフェクトを生成する。
		@param	setting			[in]	設定クラス
		@param	data			[in]	データ配列の先頭のポインタ
		@param	size			[in]	データ配列の長さ
		@param	magnification	[in]	読み込み時の拡大率
		@param	materialPath	[in]	素材ロード時の基準パス
		@return	エフェクト。失敗した場合はNULLを返す。
	*/
	static Effect* Create( Setting*	setting, void* data, int32_t size, float magnification = 1.0f, const EFK_CHAR* materialPath = NULL );

	/**
		@brief	エフェクトを生成する。
		@param	setting			[in]	設定クラス
		@param	path			[in]	読込元のパス
		@param	magnification	[in]	読み込み時の拡大率
		@param	materialPath	[in]	素材ロード時の基準パス
		@return	エフェクト。失敗した場合はNULLを返す。
	*/
	static Effect* Create( Setting*	setting, const EFK_CHAR* path, float magnification = 1.0f, const EFK_CHAR* materialPath = NULL );

	/**
	@brief	標準のエフェクト読込インスタンスを生成する。
	*/
	static ::Effekseer::EffectLoader* CreateEffectLoader(::Effekseer::FileInterface* fileInterface = NULL);

	/**
	@brief	
	\~English	Get this effect's name. If this effect is loaded from file, default name is file name without extention.
	\~Japanese	エフェクトの名前を取得する。もしファイルからエフェクトを読み込んだ場合、名前は拡張子を除いたファイル名である。
	*/
	virtual const char16_t* GetName() const = 0;

	/**
		\~English	Set this effect's name
	\~Japanese	エフェクトの名前を設定する。
	*/
	virtual void SetName(const char16_t* name) = 0;

	/**
	@brief	設定を取得する。
	@return	設定
	*/
	virtual Setting* GetSetting() const = 0;

	/**
	@brief	\~English	Get the magnification multiplied by the magnification at the time of loaded and exported.
			\~Japanese	読み込み時と出力時の拡大率をかけた拡大率を取得する。
	*/
	virtual float GetMaginification() const = 0;
	
	/**
		@brief	エフェクトデータのバージョン取得
	*/
	virtual int GetVersion() const = 0;

	/**
		@brief	格納されている色画像のポインタを取得する。
		@param	n	[in]	画像のインデックス
		@return	画像のポインタ
	*/
	virtual TextureData* GetColorImage( int n ) const = 0;

	/**
	@brief	格納されている画像のポインタの個数を取得する。
	*/
	virtual int32_t GetColorImageCount() const = 0;

	/**
	@brief	格納されている法線画像のポインタを取得する。
	@param	n	[in]	画像のインデックス
	@return	画像のポインタ
	*/
	virtual TextureData* GetNormalImage(int n) const = 0;

	/**
	@brief	格納されている法線画像のポインタの個数を取得する。
	*/
	virtual int32_t GetNormalImageCount() const = 0;

	/**
	@brief	格納されている歪み画像のポインタを取得する。
	@param	n	[in]	画像のインデックス
	@return	画像のポインタ
	*/
	virtual TextureData* GetDistortionImage(int n) const = 0;

	/**
	@brief	格納されている歪み画像のポインタの個数を取得する。
	*/
	virtual int32_t GetDistortionImageCount() const = 0;

	/**
		@brief	格納されている音波形のポインタを取得する。
	*/
	virtual void* GetWave( int n ) const = 0;

	/**
	@brief	格納されている音波形のポインタの個数を取得する。
	*/
	virtual int32_t GetWaveCount() const = 0;

	/**
		@brief	格納されているモデルのポインタを取得する。
	*/
	virtual void* GetModel( int n ) const = 0;

	/**
	@brief	格納されているモデルのポインタの個数を取得する。
	*/
	virtual int32_t GetModelCount() const = 0;

	/**
		@brief
		\~English	Reload this effect
		\~Japanese	エフェクトのリロードを行う。
		@param	data
		\~English	An effect's data
		\~Japanese	エフェクトのデータ
		@param	size
		\~English	An effect's size
		\~Japanese	エフェクトのデータサイズ
		@param	materialPath
		\~English	A path where reaources are loaded
		\~Japanese	リソースの読み込み元
		@param	reloadingThreadType
		\~English	A thread where reload function is called
		\~Japanese	リロードの関数が呼ばれるスレッド
		@return
		\~English	Result
		\~Japanese	結果
		@note
		\~English
		If reloadingThreadType is RenderThread, new resources aren't loaded and old resources aren't disposed.
		\~Japanese
		もし、reloadingThreadType が RenderThreadの場合、新規のリソースは読み込まれず、古いリソースは破棄されない。
	*/
	virtual bool Reload( void* data, int32_t size, const EFK_CHAR* materialPath = nullptr, ReloadingThreadType reloadingThreadType = ReloadingThreadType::Main) = 0;

	/**
		@brief
		\~English	Reload this effect
		\~Japanese	エフェクトのリロードを行う。
		@param	path
		\~English	An effect's path
		\~Japanese	エフェクトのパス
		@param	materialPath
		\~English	A path where reaources are loaded
		\~Japanese	リソースの読み込み元
		@param	reloadingThreadType
		\~English	A thread where reload function is called
		\~Japanese	リロードの関数が呼ばれるスレッド
		@return
		\~English	Result
		\~Japanese	結果
		@note
		\~English
		If reloadingThreadType is RenderThread, new resources aren't loaded and old resources aren't disposed.
		\~Japanese
		もし、reloadingThreadType が RenderThreadの場合、新規のリソースは読み込まれず、古いリソースは破棄されない。
	*/
	virtual bool Reload( const EFK_CHAR* path, const EFK_CHAR* materialPath = nullptr, ReloadingThreadType reloadingThreadType = ReloadingThreadType::Main) = 0;

	/**
		@brief
		\~English	Reload this effect
		\~Japanese	エフェクトのリロードを行う。
		@param	managers
		\~English	An array of manager instances
		\~Japanese	マネージャーの配列
		@param	managersCount
		\~English	Length of array
		\~Japanese	マネージャーの個数
		@param	data
		\~English	An effect's data
		\~Japanese	エフェクトのデータ
		@param	size
		\~English	An effect's size
		\~Japanese	エフェクトのデータサイズ
		@param	materialPath
		\~English	A path where reaources are loaded
		\~Japanese	リソースの読み込み元
		@param	reloadingThreadType
		\~English	A thread where reload function is called
		\~Japanese	リロードの関数が呼ばれるスレッド
		@return
		\~English	Result
		\~Japanese	結果
		@note
		\~English
		If an effect is generated with Setting, the effect in managers is reloaded with managers
		If reloadingThreadType is RenderThread, new resources aren't loaded and old resources aren't disposed.
		\~Japanese
		Settingを用いてエフェクトを生成したときに、Managerを指定することで対象のManager内のエフェクトのリロードを行う。
		もし、reloadingThreadType が RenderThreadの場合、新規のリソースは読み込まれず、古いリソースは破棄されない。
	*/
	virtual bool Reload( Manager** managers, int32_t managersCount, void* data, int32_t size, const EFK_CHAR* materialPath = nullptr, ReloadingThreadType reloadingThreadType = ReloadingThreadType::Main) = 0;

	/**
		@brief
		\~English	Reload this effect
		\~Japanese	エフェクトのリロードを行う。
		@param	managers
		\~English	An array of manager instances
		\~Japanese	マネージャーの配列
		@param	managersCount
		\~English	Length of array
		\~Japanese	マネージャーの個数
		@param	path
		\~English	An effect's path
		\~Japanese	エフェクトのパス
		@param	materialPath
		\~English	A path where reaources are loaded
		\~Japanese	リソースの読み込み元
		@param	reloadingThreadType
		\~English	A thread where reload function is called
		\~Japanese	リロードの関数が呼ばれるスレッド
		@return
		\~English	Result
		\~Japanese	結果
		@note
		\~English
		If an effect is generated with Setting, the effect in managers is reloaded with managers
		If reloadingThreadType is RenderThread, new resources aren't loaded and old resources aren't disposed.
		\~Japanese
		Settingを用いてエフェクトを生成したときに、Managerを指定することで対象のManager内のエフェクトのリロードを行う。
		もし、reloadingThreadType が RenderThreadの場合、新規のリソースは読み込まれず、古いリソースは破棄されない。
	*/
	virtual bool Reload( Manager** managers, int32_t managersCount,const EFK_CHAR* path, const EFK_CHAR* materialPath = nullptr, ReloadingThreadType reloadingThreadType = ReloadingThreadType::Main) = 0;

	/**
		@brief	画像等リソースの再読み込みを行う。
	*/
	virtual void ReloadResources( const EFK_CHAR* materialPath = nullptr ) = 0;

	/**
		@brief	画像等リソースの破棄を行う。
	*/
	virtual void UnloadResources() = 0;

	/**
	@brief	Rootを取得する。
	*/
	virtual EffectNode* GetRoot() const = 0;
};

/**
@brief	共通描画パラメーター
@note
大きく変更される可能性があります。
*/
struct EffectBasicRenderParameter
{
	int32_t				ColorTextureIndex;
	AlphaBlendType		AlphaBlend;
	TextureFilterType	FilterType;
	TextureWrapType		WrapType;
	bool				ZWrite;
	bool				ZTest;
	bool				Distortion;
	float				DistortionIntensity;
};

/**
@brief	
	\~English	Model parameter
	\~Japanese	モデルパラメーター
@note
	\~English	It may change greatly.
	\~Japanese	大きく変更される可能性があります。

*/
struct EffectModelParameter
{
	bool				Lighting;
};

/**
@brief	ノードインスタンス生成クラス
@note
エフェクトのノードの実体を生成する。
*/
class EffectNode
{
public:
	EffectNode() {}
	virtual ~EffectNode(){}

	/**
	@brief	ノードが所属しているエフェクトを取得する。
	*/
	virtual Effect* GetEffect() const = 0;

	/**
	@brief	子のノードの数を取得する。
	*/
	virtual int GetChildrenCount() const = 0;

	/**
	@brief	子のノードを取得する。
	*/
	virtual EffectNode* GetChild(int index) const = 0;

	/**
	@brief	共通描画パラメーターを取得する。
	*/
	virtual EffectBasicRenderParameter GetBasicRenderParameter() = 0;

	/**
	@brief	共通描画パラメーターを設定する。
	*/
	virtual void SetBasicRenderParameter(EffectBasicRenderParameter param) = 0;

	/**
	@brief	
	\~English	Get a model parameter
	\~Japanese	モデルパラメーターを取得する。
	*/
	virtual EffectModelParameter GetEffectModelParameter() = 0;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEER_EFFECT_H__
