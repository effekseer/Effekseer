using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer
{
	/// <summary>
	/// 描画画面クラス
	/// </summary>
	public interface IViewer
	{
		/// <summary>
		/// 描画画面を生成する。
		/// 同時に1つの画面しか存在しない。
		/// 画面サイズ変更はCreateとDestroyを組み合わせて行われる。
		/// </summary>
		/// <param name="handle"></param>
		/// <param name="width"></param>
		/// <param name="height"></param>
		/// <param name="isLinearMode"></param>
		/// <returns></returns>
		bool CreateWindow(IntPtr handle, int width, int height, bool isLinearMode);

		/// <summary>
		/// 描画画面を更新する。
		/// 必ず毎フレーム実行される。
		/// PlayEffect、StopEffect等のイベントが起きてから実行される。
		/// </summary>
		/// <returns></returns>
		bool UpdateWindow();

		/// <summary>
		/// 描画画面のサイズを変更する。
		/// </summary>
		/// <param name="width"></param>
		/// <param name="height"></param>
		/// <returns></returns>
		bool ResizeWindow(int width, int height);

		/// <summary>
		/// 描画画面を破棄する。
		/// </summary>
		/// <returns></returns>
		bool DestroyWindow();

		/// <summary>
		/// エフェクトを読み込む。
		/// RemoveEffectをされずに実行されることもある。
		/// 読み込むことだけを行う。
		/// </summary>
		/// <param name="data"></param>
		/// <param name="size"></param>
		/// <param name="path"></param>
		/// <returns></returns>
		bool LoadEffect(IntPtr data, int size, string path);

		/// <summary>
		/// 読み込み済みのエフェクトを破棄する。
		/// </summary>
		/// <returns></returns>
		bool RemoveEffect();

		/// <summary>
		/// 再生を開始する。
		/// </summary>
		/// <returns></returns>
		bool PlayEffect();

		/// <summary>
		/// エフェクトの再生を停止する。
		/// </summary>
		/// <returns></returns>
		bool StopEffect();

		/// <summary>
		/// 指定したフレーム分進める。
		/// </summary>
		/// <param name="frame">進めるフレーム数。1以上。</param>
		/// <returns></returns>
		bool StepEffect(int frame);

		/// <summary>
		/// 画面を中心からx,y回転
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <returns></returns>
		bool Rotate(float x, float y);

		/// <summary>
		/// 画面を平行移動
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <returns></returns>
		bool Slide(float x, float y);

		/// <summary>
		/// ズーム
		/// </summary>
		/// <param name="zoom"></param>
		/// <returns></returns>
		bool Zoom(float zoom);

		/// <summary>
		/// ランダムを初期化するシードを設定
		/// </summary>
		/// <param name="seed"></param>
		/// <returns></returns>
		bool SetRandomSeed(int seed);

		/// <summary>
		/// 既存のエフェクトのデータを読み込むメソッド
		/// </summary>
		Func<bool> LoadEffectFunc
		{
			get;
			set;
		}

		/// <summary>
		/// キャッシュ上のテクスチャを破棄する
		/// </summary>
		void InvalidateTextureCache();

		/// <summary>
		/// ルートの初期位置を設定する。
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="z"></param>
		void SetLotation(float x, float y, float z);

		/// <summary>
		/// ルートの初期角度を設定する。
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="z"></param>
		void SetRotation(float x, float y, float z);

		/// <summary>
		/// ルートの初期拡大率を設定する。
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="z"></param>
		void SetScale(float x, float y, float z);

		/// <summary>
		/// ルートの移動速度を設定する。
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="z"></param>
		void SetLotationVelocity(float x, float y, float z);

		/// <summary>
		/// ルートの回転速度を設定する。
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="z"></param>
		void SetRotationVelocity(float x, float y, float z);

		/// <summary>
		/// ルートの拡大速度を設定する。
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="z"></param>
		void SetScaleVelocity(float x, float y, float z);
		
		/// <summary>
		/// ターゲットの位置を設定する。
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="z"></param>
		void SetTargetLocation(float x, float y, float z);

		/// <summary>
		/// ルートを削除するフレームを設定する。
		/// </summary>
		/// <param name="time"></param>
		void SetRemovedTime(int time);

		/// <summary>
		/// エフェクトの表示する個数を設定する。
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="z"></param>
		void SetEffectCount(int x, int y, int z);

		/// <summary>
		/// エフェクト間の距離を設定する。
		/// </summary>
		/// <param name="distance"></param>
		void SetEffectDistance(float distance);

		void SetAllColor(byte r, byte g, byte b, byte a);

		void SetEffectTimeSpan(int timeSpan);

		/// <summary>
		/// グリッドを表示するか設定する。
		/// </summary>
		/// <param name="value"></param>
		/// <param name="xy"></param>
		/// <param name="xz"></param>
		void SetIsGridShown(bool value, bool xy, bool xz, bool yz);

		/// <summary>
		/// グリッドの長さを設定する。
		/// </summary>
		/// <param name="length"></param>
		void SetGridLength(float length);

		/// <summary>
		/// 背景色を設定する。
		/// </summary>
		/// <param name="r"></param>
		/// <param name="g"></param>
		/// <param name="b"></param>
		void SetBackgroundColor(byte r, byte g, byte b);

		/// <summary>
		/// 背景画像を設定する。
		/// </summary>
		/// <param name="path"></param>
		void SetBackgroundImage(string path);

		/// <summary>
		/// グリッド色を設定する。
		/// </summary>
		/// <param name="r"></param>
		/// <param name="g"></param>
		/// <param name="b"></param>
		void SetGridColor(byte r, byte g, byte b, byte a);

		/// <summary>
		/// FPS出力のためのステップ数を出力する。
		/// </summary>
		/// <param name="step"></param>
		void SetStep(int step);

		/// <summary>
		/// カリング情報を設定する。
		/// </summary>
		/// <param name="isCullingShown"></param>
		/// <param name="cullingRadius"></param>
		/// <param name="cullingX"></param>
		/// <param name="cullingY"></param>
		/// <param name="cullingZ"></param>
		void SetCullingParameter(bool isCullingShown, float cullingRadius, float cullingX, float cullingY, float cullingZ);

		/// <summary>
		/// サーバーに接続する。
		/// </summary>
		/// <param name="target"></param>
		/// <param name="port"></param>
		bool Connect(string target, int port);

		/// <summary>
		/// サーバーに接続されているか取得する。
		/// </summary>
		/// <returns></returns>
		bool IsConnected();

		/// <summary>
		/// サーバーから切断する。
		/// </summary>
		/// <returns></returns>
		void Disconnect();

		/// <summary>
		/// サーバーにデータを送信する。
		/// </summary>
		/// <param name="key"></param>
		/// <param name="data"></param>
		/// <param name="size"></param>
		/// <param name="path"></param>
		/// <returns></returns>
		void SendEffect(string key, IntPtr data, int size, string path);

		/// <summary>
		/// ライト方向を設定。
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="z"></param>
		/// <returns></returns>
		void SetLightDirection(float x, float y, float z);
		
		/// <summary>
		/// ライト色を設定。
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="z"></param>
		/// <returns></returns>
		void SetLightColor(byte r, byte g, byte b, byte a);
		
		/// <summary>
		/// アンビエント色を設定。
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="z"></param>
		/// <returns></returns>
		void SetLightAmbientColor(byte r, byte g, byte b, byte a);

		/// <summary>
		/// 右手系かどうか？
		/// </summary>
		/// <param name="value"></param>
		void SetIsRightHand(bool value);

		/// <summary>
		/// Set distortion type
		/// </summary>
		/// <param name="type"></param>
		void SetDistortionType(int type);

		/// <summary>
		/// マウスでの画面回転方向を逆にするか?
		/// </summary>
		/// <param name="value"></param>
		void SetMouseInverseFlag(bool rotX, bool rotY, bool slideX, bool slideY);
	}
}
