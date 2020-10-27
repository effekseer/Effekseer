
#ifndef __EFFEKSEER_EFFECT_H__
#define __EFFEKSEER_EFFECT_H__

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
@brief
\~English	Terms where an effect exists
\~Japanese	エフェクトが存在する期間
*/
struct EffectTerm
{
	/**
@brief
\~English	Minimum end time that the effect may exist
\~Japanese	エフェクトが存在する可能性のある最小の終了時間
*/
	int32_t TermMin;

	/**
	@brief
	\~English	Maximum end time that the effect may exist
	\~Japanese	エフェクトが存在する可能性のある最大の終了時間
	*/
	int32_t TermMax;
};

/**
@brief
\~English	Terms where instances exists
\~Japanese	インスタンスが存在する期間
*/
struct EffectInstanceTerm
{
	/**
	@brief
	\~English	Minimum start time that the first instance may exist
	\~Japanese	最初のインスタンスが存在する可能性のある最小の開始時間
	*/
	int32_t FirstInstanceStartMin = 0;

	/**
	@brief
	\~English	Maximum start time that the first instance may exist
	\~Japanese	最初のインスタンスが存在する可能性のある最大の開始時間
	*/
	int32_t FirstInstanceStartMax = 0;

	/**
	@brief
	\~English	Minimum end time that the first instance may exist
	\~Japanese	最初のインスタンスが存在する可能性のある最小の終了時間
	*/
	int32_t FirstInstanceEndMin = INT_MAX;

	/**
	@brief
	\~English	Maximum end time that the first instance may exist
	\~Japanese	最初のインスタンスが存在する可能性のある最大の終了時間
	*/
	int32_t FirstInstanceEndMax = INT_MAX;

	/**
	@brief
	\~English	Minimum start time that the last instance may exist
	\~Japanese	最後のインスタンスが存在する可能性のある最小の開始時間
	*/
	int32_t LastInstanceStartMin = 0;

	/**
	@brief
	\~English	Maximum start time that the last instance may exist
	\~Japanese	最後のインスタンスが存在する可能性のある最大の開始時間
	*/
	int32_t LastInstanceStartMax = 0;

	/**
	@brief
	\~English	Minimum end time that the last instance may exist
	\~Japanese	最後のインスタンスが存在する可能性のある最小の終了時間
	*/
	int32_t LastInstanceEndMin = INT_MAX;

	/**
	@brief
	\~English	Maximum end time that the last instance may exist
	\~Japanese	最後のインスタンスが存在する可能性のある最大の終了時間
	*/
	int32_t LastInstanceEndMax = INT_MAX;
};

/**
	@brief
	\~English A class to edit an instance of EffectParameter for supporting original format when a binary is loaded.
	\~Japanese	独自フォーマットをサポートするための、バイナリが読み込まれた時にEffectParameterのインスタンスを編集するクラス
*/
class EffectFactory : public ReferenceObject
{
public:
	EffectFactory();

	virtual ~EffectFactory();

	/**
	@brief
	\~English load body data(parameters of effect) from a binary
	\~Japanese	バイナリから本体(エフェクトのパラメーター)を読み込む。
	*/
	bool LoadBody(Effect* effect, const void* data, int32_t size, float magnification, const EFK_CHAR* materialPath);

	/**
	@brief
	\~English set texture data into specified index
	\~Japanese	指定されたインデックスにテクスチャを設定する。
	*/
	void SetTexture(Effect* effect, int32_t index, TextureType type, TextureData* data);

	/**
	@brief
	\~English set sound data into specified index
	\~Japanese	指定されたインデックスに音を設定する。
	*/

	void SetSound(Effect* effect, int32_t index, void* data);

	/**
	@brief
	\~English set model data into specified index
	\~Japanese	指定されたインデックスにモデルを設定する。
	*/
	void SetModel(Effect* effect, int32_t index, void* data);

	/**
	@brief
	\~English set material data into specified index
	\~Japanese	指定されたインデックスにマテリアルを設定する。
	*/
	void SetMaterial(Effect* effect, int32_t index, MaterialData* data);

	/**
	@brief
	\~English set loading data
	\~Japanese	ロード用データを設定する。
	*/
	void SetLoadingParameter(Effect* effect, ReferenceObject* obj);

	/**
		@brief
		\~English this method is called to check whether loaded binary are supported. 
		\~Japanese	バイナリがサポートされているか確認するためにこのメソッドが呼ばれる。
	*/
	virtual bool OnCheckIsBinarySupported(const void* data, int32_t size);

	/**
		@brief
		\~English this method is called to check whether reloading are supported.
		\~Japanese	リロードがサポートされているか確認するためにこのメソッドが呼ばれる。
	*/
	virtual bool OnCheckIsReloadSupported();

	/**
		@brief
		\~English this method is called when load a effect from binary
		\~Japanese	バイナリからエフェクトを読み込む時に、このメソッドが呼ばれる。
	*/
	virtual bool OnLoading(Effect* effect, const void* data, int32_t size, float magnification, const EFK_CHAR* materialPath);

	/**
		@brief
		\~English this method is called when load resources
		\~Japanese	リソースを読み込む時に、このメソッドが呼ばれる。
	*/
	virtual void OnLoadingResource(Effect* effect, const void* data, int32_t size, const EFK_CHAR* materialPath);

	/**
	@brief
	\~English this method is called when unload resources
	\~Japanese	リソースを廃棄される時に、このメソッドが呼ばれる。
	*/
	virtual void OnUnloadingResource(Effect* effect);

	/**
	\~English get factory's name
	\~Japanese	ファクトリーの名称を取得する。
	*/
	virtual const char* GetName() const;

	/**
	\~English get whether resources are loaded automatically when a binary is loaded
	\~Japanese	バイナリを読み込んだときに自動的にリソースを読み込むか取得する。
	*/
	virtual bool GetIsResourcesLoadedAutomatically() const;
};

/**
	@brief	
	\~English	Effect parameters
	\~Japanese	エフェクトパラメータークラス
*/
class Effect
	: public IReference
{
protected:
	Effect()
	{
	}
	virtual ~Effect()
	{
	}

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
	static Effect* Create(Manager* manager, void* data, int32_t size, float magnification = 1.0f, const EFK_CHAR* materialPath = NULL);

	/**
		@brief	エフェクトを生成する。
		@param	manager			[in]	管理クラス
		@param	path			[in]	読込元のパス
		@param	magnification	[in]	読み込み時の拡大率
		@param	materialPath	[in]	素材ロード時の基準パス
		@return	エフェクト。失敗した場合はNULLを返す。
	*/
	static Effect* Create(Manager* manager, const EFK_CHAR* path, float magnification = 1.0f, const EFK_CHAR* materialPath = NULL);

	/**
		@brief	エフェクトを生成する。
		@param	setting			[in]	設定クラス
		@param	data			[in]	データ配列の先頭のポインタ
		@param	size			[in]	データ配列の長さ
		@param	magnification	[in]	読み込み時の拡大率
		@param	materialPath	[in]	素材ロード時の基準パス
		@return	エフェクト。失敗した場合はNULLを返す。
	*/
	static Effect* Create(Setting* setting, void* data, int32_t size, float magnification = 1.0f, const EFK_CHAR* materialPath = NULL);

	/**
		@brief	エフェクトを生成する。
		@param	setting			[in]	設定クラス
		@param	path			[in]	読込元のパス
		@param	magnification	[in]	読み込み時の拡大率
		@param	materialPath	[in]	素材ロード時の基準パス
		@return	エフェクト。失敗した場合はNULLを返す。
	*/
	static Effect* Create(Setting* setting, const EFK_CHAR* path, float magnification = 1.0f, const EFK_CHAR* materialPath = NULL);

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
		@brief
		\~English	Get loading parameter supecfied by EffectFactory. This parameter is not used unless EffectFactory is used
		\~Japanese	EffectFactoryによって指定されたロード用パラメーターを取得する。EffectFactoryを使用しない限り、子のパラメーターは使用しない。
	*/
	virtual ReferenceObject* GetLoadingParameter() const = 0;

	/**
		@brief	格納されている色画像のポインタを取得する。
		@param	n	[in]	画像のインデックス
		@return	画像のポインタ
	*/
	virtual TextureData* GetColorImage(int n) const = 0;

	/**
	@brief	格納されている画像のポインタの個数を取得する。
	*/
	virtual int32_t GetColorImageCount() const = 0;

	/**
	@brief	\~English	Get a color image's path
	\~Japanese	色画像のパスを取得する。
	*/
	virtual const EFK_CHAR* GetColorImagePath(int n) const = 0;

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
	@brief	\~English	Get a normal image's path
	\~Japanese	法線画像のパスを取得する。
	*/
	virtual const EFK_CHAR* GetNormalImagePath(int n) const = 0;

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
	@brief	\~English	Get a distortion image's path
	\~Japanese	歪み画像のパスを取得する。
	*/
	virtual const EFK_CHAR* GetDistortionImagePath(int n) const = 0;

	/**
		@brief	格納されている音波形のポインタを取得する。
	*/
	virtual void* GetWave(int n) const = 0;

	/**
	@brief	格納されている音波形のポインタの個数を取得する。
	*/
	virtual int32_t GetWaveCount() const = 0;

	/**
	@brief	\~English	Get a wave's path
	\~Japanese	音波形のパスを取得する。
	*/
	virtual const EFK_CHAR* GetWavePath(int n) const = 0;

	/**
		@brief	格納されているモデルのポインタを取得する。
	*/
	virtual void* GetModel(int n) const = 0;

	/**
	@brief	格納されているモデルのポインタの個数を取得する。
	*/
	virtual int32_t GetModelCount() const = 0;

	/**
	@brief	\~English	Get a model's path
	\~Japanese	モデルのパスを取得する。
	*/
	virtual const EFK_CHAR* GetModelPath(int n) const = 0;

	/**
	@brief	\~English	Get a material's pointer
	\~Japanese	格納されているマテリアルのポインタを取得する。
	*/
	virtual MaterialData* GetMaterial(int n) const = 0;

	/**
	@brief	\~English	Get the number of stored material pointer 
	\~Japanese	格納されているマテリアルのポインタの個数を取得する。
	*/
	virtual int32_t GetMaterialCount() const = 0;

	/**
	@brief	\~English	Get a material's path
	\~Japanese	マテリアルのパスを取得する。
	*/
	virtual const EFK_CHAR* GetMaterialPath(int n) const = 0;

	/**
		@brief
		\~English set texture data into specified index
		\~Japanese	指定されたインデックスにテクスチャを設定する。
	*/
	virtual void SetTexture(int32_t index, TextureType type, TextureData* data) = 0;

	/**
		@brief
		\~English set sound data into specified index
		\~Japanese	指定されたインデックスに音を設定する。
	*/

	virtual void SetSound(int32_t index, void* data) = 0;

	/**
		@brief
		\~English set model data into specified index
		\~Japanese	指定されたインデックスにモデルを設定する。
	*/
	virtual void SetModel(int32_t index, void* data) = 0;

	/**
		@brief
		\~English set material data into specified index
		\~Japanese	指定されたインデックスにマテリアルを設定する。
	*/
	virtual void SetMaterial(int32_t index, MaterialData* data) = 0;

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
	virtual bool Reload(void* data, int32_t size, const EFK_CHAR* materialPath = nullptr, ReloadingThreadType reloadingThreadType = ReloadingThreadType::Main) = 0;

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
	virtual bool Reload(const EFK_CHAR* path, const EFK_CHAR* materialPath = nullptr, ReloadingThreadType reloadingThreadType = ReloadingThreadType::Main) = 0;

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
	virtual bool Reload(Manager** managers, int32_t managersCount, void* data, int32_t size, const EFK_CHAR* materialPath = nullptr, ReloadingThreadType reloadingThreadType = ReloadingThreadType::Main) = 0;

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
	virtual bool Reload(Manager** managers, int32_t managersCount, const EFK_CHAR* path, const EFK_CHAR* materialPath = nullptr, ReloadingThreadType reloadingThreadType = ReloadingThreadType::Main) = 0;

	/**
		@brief	画像等リソースの再読み込みを行う。
	*/
	virtual void ReloadResources(const void* data = nullptr, int32_t size = 0, const EFK_CHAR* materialPath = nullptr) = 0;

	/**
		@brief	画像等リソースの破棄を行う。
	*/
	virtual void UnloadResources() = 0;

	/**
	@brief	Rootを取得する。
	*/
	virtual EffectNode* GetRoot() const = 0;

	/**
		@brief
	\~English	Calculate a term of instances where the effect exists
	\~Japanese	エフェクトが存在する期間を計算する。
	*/
	virtual EffectTerm CalculateTerm() const = 0;
};

/**
@brief	共通描画パラメーター
@note
大きく変更される可能性があります。
*/
struct EffectBasicRenderParameter
{
	RendererMaterialType MaterialType;
	int32_t ColorTextureIndex;
	AlphaBlendType AlphaBlend;
	TextureFilterType FilterType;
	TextureWrapType WrapType;
	bool ZWrite;
	bool ZTest;
	bool Distortion;
	float DistortionIntensity;
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
	bool Lighting;
};

/**
@brief	ノードインスタンス生成クラス
@note
エフェクトのノードの実体を生成する。
*/
class EffectNode
{
public:
	EffectNode()
	{
	}
	virtual ~EffectNode()
	{
	}

	/**
	@brief	ノードが所属しているエフェクトを取得する。
	*/
	virtual Effect* GetEffect() const = 0;

	/**
	@brief	
	\~English	Get a generation in the node tree. The generation increases by 1 as it moves a child node.
	\~Japanese	ノードツリーの世代を取得する。世代は子のノードになるにしたがって1増える。
	*/
	virtual int GetGeneration() const = 0;

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

	/**
	@brief
	\~English	Calculate a term of instances where instances exists
	\~Japanese	インスタンスが存在する期間を計算する。
	*/
	virtual EffectInstanceTerm CalculateInstanceTerm(EffectInstanceTerm& parentTerm) const = 0;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEER_EFFECT_H__
