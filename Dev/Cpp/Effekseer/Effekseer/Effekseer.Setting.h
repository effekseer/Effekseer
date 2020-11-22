
#ifndef __EFFEKSEER_LOADER_H__
#define __EFFEKSEER_LOADER_H__

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

class EffectFactory;

/**
	@brief	設定クラス
	@note
	EffectLoader等、ファイル読み込みに関する設定することができる。
	Managerの代わりにエフェクト読み込み時に使用することで、Managerとは独立してEffectインスタンスを生成することができる。
*/
class Setting : public ReferenceObject
{
private:
	//! coordinate system
	CoordinateSystem m_coordinateSystem;

	EffectLoaderRef m_effectLoader;
	TextureLoaderRef m_textureLoader;
	SoundLoaderRef m_soundLoader;
	ModelLoaderRef m_modelLoader;
	MaterialLoaderRef m_materialLoader;
	CurveLoaderRef m_curveLoader;
	ProcedualModelGeneratorRef procedualMeshGenerator_;
	std::vector<RefPtr<EffectFactory>> effectFactories;

protected:
	Setting();

	~Setting();

public:
	/**
		@brief	設定インスタンスを生成する。
	*/
	static RefPtr<Setting> Create();

	/**
	@brief	座標系を取得する。
	@return	座標系
	*/
	CoordinateSystem GetCoordinateSystem() const;

	/**
	@brief	座標系を設定する。
	@param	coordinateSystem	[in]	座標系
	@note
	座標系を設定する。
	エフェクトファイルを読み込む前に設定する必要がある。
	*/
	void SetCoordinateSystem(CoordinateSystem coordinateSystem);

	/**
		@brief	エフェクトローダーを取得する。
		@return	エフェクトローダー
		*/
	EffectLoaderRef GetEffectLoader();

	/**
		@brief	エフェクトローダーを設定する。
		@param	loader	[in]		ローダー
		*/
	void SetEffectLoader(EffectLoaderRef loader);

	/**
		@brief	テクスチャローダーを取得する。
		@return	テクスチャローダー
		*/
	TextureLoaderRef GetTextureLoader();

	/**
		@brief	テクスチャローダーを設定する。
		@param	loader	[in]		ローダー
		*/
	void SetTextureLoader(TextureLoaderRef loader);

	/**
		@brief	モデルローダーを取得する。
		@return	モデルローダー
		*/
	ModelLoaderRef GetModelLoader();

	/**
		@brief	モデルローダーを設定する。
		@param	loader	[in]		ローダー
		*/
	void SetModelLoader(ModelLoaderRef loader);

	/**
		@brief	サウンドローダーを取得する。
		@return	サウンドローダー
		*/
	SoundLoaderRef GetSoundLoader();

	/**
		@brief	サウンドローダーを設定する。
		@param	loader	[in]		ローダー
		*/
	void SetSoundLoader(SoundLoaderRef loader);

	/**
		@brief
		\~English get a material loader
		\~Japanese マテリアルローダーを取得する。
		@return
		\~English	loader
		\~Japanese ローダー
	*/
	MaterialLoaderRef GetMaterialLoader();

	/**
		@brief
		\~English specfiy a material loader
		\~Japanese マテリアルローダーを設定する。
		@param	loader
		\~English	loader
		\~Japanese ローダー
		*/
	void SetMaterialLoader(MaterialLoaderRef loader);

	/**
		@brief
		\~English get a curve loader
		\~Japanese カーブローダーを取得する。
		@return
		\~English	loader
		\~Japanese ローダー
	*/
	CurveLoaderRef GetCurveLoader();

	/**
		@brief
		\~English specfiy a curve loader
		\~Japanese カーブローダーを設定する。
		@param	loader
		\~English	loader
		\~Japanese ローダー
	*/
	void SetCurveLoader(CurveLoaderRef loader);

	/**
		@brief
		\~English get a mesh generator
		\~Japanese メッシュジェネレーターを取得する。
		@return
		\~English	generator
		\~Japanese ローダー
	*/
	ProcedualModelGeneratorRef GetProcedualMeshGenerator() const;

	/**
		@brief
		\~English specfiy a mesh generator
		\~Japanese メッシュジェネレーターを設定する。
		@param	generator
		\~English	generator
		\~Japanese generator
	*/
	void SetProcedualMeshGenerator(ProcedualModelGeneratorRef generator);

	/**
		@brief
		\~English	Add effect factory
		\~Japanese Effect factoryを追加する。
	*/
	void AddEffectFactory(const RefPtr<EffectFactory>& effectFactory);

	/**
		@brief
		\~English	Get effect factory
		\~Japanese Effect Factoryを取得する。
	*/
	const RefPtr<EffectFactory>& GetEffectFactory(int32_t ind) const;

	/**
		@brief
		\~English	clear effect factories
		\~Japanese 全てのEffect Factoryを削除する。
	*/
	void ClearEffectFactory();

	/**
		@brief
		\~English	Get the number of effect factory
		\~Japanese Effect Factoryの数を取得する。
	*/
	int32_t GetEffectFactoryCount() const;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace Effekseer
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEER_LOADER_H__
