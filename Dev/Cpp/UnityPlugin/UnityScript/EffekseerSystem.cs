using UnityEngine;
using UnityEngine.Rendering;
using System;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

#if UNITY_EDITOR
using UnityEditor;
#endif

public class EffekseerSystem : MonoBehaviour
{
	/// <summary>
	/// シーンビューに描画するかどうか
	/// </summary>
	public bool drawInSceneView = true;

	/// <summary>
	/// インスタンスの最大数
	/// </summary>
	public int maxInstances		= 800;

	/// <summary>
	/// 四角形の最大数
	/// </summary>
	public int maxSquares		= 1200;

	/// <summary>
	/// エフェクトの描画するタイミング
	/// </summary>
	public CameraEvent cameraEvent	= CameraEvent.AfterForwardAlpha;

	/// <summary>
	/// Effekseerのファイルを置く場所
	/// </summary>
	public static string resourcePath
	{
		get {
			return Path.Combine(Application.streamingAssetsPath, "Effekseer");
		}
	}

	/// <summary>
	/// エフェクトの再生
	/// </summary>
	/// <param name="name">エフェクト名</param>
	/// <param name="location">再生開始する位置</param>
	/// <returns>再生したエフェクトのハンドル</returns>
	public static EffekseerHandle PlayEffect(string name, Vector3 location)
	{
		IntPtr effect = Instance._GetEffect(name);
		if (effect != IntPtr.Zero) {
			int handle = Plugin.EffekseerPlayEffect(effect, location.x, location.y, location.z);
			return new EffekseerHandle(handle);
		}
		return new EffekseerHandle(-1);
	}
	
	/// <summary>
	/// 全エフェクトの再生停止
	/// </summary>
	public static void StopAllEffects()
	{
		Plugin.EffekseerStopAllEffects();
	}

	/// <summary>
	/// エフェクトのロード
	/// </summary>
	/// <param name="name">エフェクト名</param>
	public static void LoadEffect(string name)
	{
		Instance._LoadEffect(name);
	}

	/// <summary>
	/// エフェクトの解放
	/// </summary>
	/// <param name="name">エフェクト名</param>
	public static void ReleaseEffect(string name)
	{
		Instance._ReleaseEffect(name);
	}

	#region Internal Implimentation
	
	// シングルトンのインスタンス
	private static EffekseerSystem _Instance = null;
	public static EffekseerSystem Instance
	{
		get {
			if (_Instance == null) {
				// staticに無ければ探す
				var system = GameObject.FindObjectOfType<EffekseerSystem>();
				if (system != null) {
					// 有ればstaticにセット
					_Instance = system;
				} else {
					// 無ければ新しく作成
					var go = GameObject.Find("Effekseer");
					if (go == null) {
						go = new GameObject("Effekseer");
					}
					_Instance = go.AddComponent<EffekseerSystem>();
				}
			}
			return _Instance;
		}
	}
	
	// ロードしたエフェクト
	private Dictionary<string, IntPtr> effectList = new Dictionary<string, IntPtr>();
	
#if UNITY_EDITOR
	// ホットリロードの退避用
	private List<string> savedEffectList = new List<string>();
#endif

	// カメラごとのレンダーパス
	class RenderPath {
		public CommandBuffer commandBuffer;
		public CameraEvent cameraEvent;
		public int renderId;
	};
	private Dictionary<Camera, RenderPath> renderPaths = new Dictionary<Camera, RenderPath>();

	private IntPtr _GetEffect(string name) {
		// 拡張子を除外する
		name = Path.GetFileNameWithoutExtension(name);
		
		if (effectList.ContainsKey(name) == false) {
			return effectList[name];
		}
		
		// 存在しなかったらロード
		return _LoadEffect(name);
	}

	private IntPtr _LoadEffect(string name) {
		if (effectList.ContainsKey(name)) {
			return effectList[name];
		}

		// 拡張子を除外する
		name = Path.GetFileNameWithoutExtension(name);

		// パス解決
		string fullPath = Path.Combine(EffekseerSystem.resourcePath, Path.ChangeExtension(name, "efk"));
		fullPath += "\0";
		
		// UTF16に変換
		byte[] bytes = Encoding.Unicode.GetBytes(fullPath);
		
		// 文字列をメモリにマップしてロードを実行
		GCHandle ghc = GCHandle.Alloc(bytes, GCHandleType.Pinned);
		IntPtr effect = Plugin.EffekseerLoadEffect(ghc.AddrOfPinnedObject());
		ghc.Free();
		
		if (effect == IntPtr.Zero) {
			Debug.LogError("[Effekseer] Loading error: " + fullPath);
			return IntPtr.Zero;
		}

		effectList.Add(name, effect);
		return effect;
	}
	
	private void _ReleaseEffect(string name) {
		// 拡張子を除外する
		name = Path.GetFileNameWithoutExtension(name);
		
		if (effectList.ContainsKey(name) == false) {
			var effect = effectList[name];
			Plugin.EffekseerReleaseEffect(effect);
			effectList.Remove(name);
		}
	}
	
	void Awake() {
		Plugin.EffekseerInit(maxInstances, maxSquares);
	}
	
	void OnDestroy() {
		foreach (var pair in effectList) {
			Plugin.EffekseerReleaseEffect(pair.Value);
		}
		effectList = null;
		Plugin.EffekseerTerm();
	}

	void OnEnable() {
#if UNITY_EDITOR
		// ホットリロード時はリジューム処理
		foreach (var effect in savedEffectList) {
			string[] tokens = effect.Split(',');
			if (tokens.Length == 2) {
				effectList.Add(tokens[0], (IntPtr)ulong.Parse(tokens[1]));
			}
		}
		savedEffectList.Clear();
#endif
		CleanUp();
		Camera.onPreCull += OnPreCullEvent;
	}

	void OnDisable() {
#if UNITY_EDITOR
		// Dictionaryは消えるので文字列にして退避
		foreach (var pair in effectList) {
			savedEffectList.Add(pair.Key + "," + pair.Value.ToString());
		}
		effectList.Clear();
#endif
		Camera.onPreCull -= OnPreCullEvent;
		CleanUp();
	}

	void CleanUp() {
		// レンダーパスの破棄
		foreach (var pair in renderPaths) {
			var camera = pair.Key;
			var path = pair.Value;
			if (camera != null) {
				camera.RemoveCommandBuffer(path.cameraEvent, path.commandBuffer);
			}
		}
		renderPaths.Clear();
	}
	
	void FixedUpdate() {
		// 1フレーム更新
		Plugin.EffekseerUpdate(1);
	}
	
	void OnPreCullEvent(Camera camera) {
#if UNITY_EDITOR
		if (Array.IndexOf<Camera>(SceneView.GetAllSceneCameras(), camera) >= 0) {
			// シーンビューのカメラはチェック
			if (this.drawInSceneView == false) {
				return;
			}
		} else if (Camera.current.isActiveAndEnabled == false) {
			// シーンビュー以外のエディタカメラは除外
			return;
		}
#endif
		RenderPath path;
		if (renderPaths.ContainsKey(camera)) {
			// レンダーパスが有れば使う
			path = renderPaths[camera];
		} else {
			// 無ければ作成
			path = new RenderPath();
			path.renderId = renderPaths.Count;
			path.cameraEvent = cameraEvent;
			// プラグイン描画するコマンドバッファを作成
			path.commandBuffer = new CommandBuffer();
			path.commandBuffer.IssuePluginEvent(Plugin.EffekseerGetRenderFunc(), path.renderId);
			// コマンドバッファをカメラに登録
			camera.AddCommandBuffer(path.cameraEvent, path.commandBuffer);
			renderPaths.Add(camera, path);
		}

		// ビュー関連の行列を更新
		SetCameraMatrix(path.renderId, camera);
		SetProjectionMatrix(path.renderId, camera);
	}

	private void SetProjectionMatrix(int renderId, Camera camera) {
		float[] projectionMatrixArray = Matrix2Array(GL.GetGPUProjectionMatrix(
			camera.projectionMatrix, RenderTexture.active));
		GCHandle ghc = GCHandle.Alloc(projectionMatrixArray, GCHandleType.Pinned);
		EffekseerSystem.Plugin.EffekseerSetProjectionMatrix(renderId, ghc.AddrOfPinnedObject());
		ghc.Free();
	}

	private void SetCameraMatrix(int renderId, Camera camera) {
		float[] cameraMatrixArray = Matrix2Array(camera.worldToCameraMatrix);
		GCHandle ghc = GCHandle.Alloc(cameraMatrixArray, GCHandleType.Pinned);
		Plugin.EffekseerSetCameraMatrix(renderId, ghc.AddrOfPinnedObject());
		ghc.Free();
	}

	private float[] Matrix2Array(Matrix4x4 mat) {
		float[] res = new float[16];
		res[ 0] = mat.m00;
		res[ 1] = mat.m01;
		res[ 2] = mat.m02;
		res[ 3] = mat.m03;
		res[ 4] = mat.m10;
		res[ 5] = mat.m11;
		res[ 6] = mat.m12;
		res[ 7] = mat.m13;
		res[ 8] = mat.m20;
		res[ 9] = mat.m21;
		res[10] = mat.m22;
		res[11] = mat.m23;
		res[12] = mat.m30;
		res[13] = mat.m31;
		res[14] = mat.m32;
		res[15] = mat.m33;
		return res;
	}
	
	public static class Plugin
	{
		#if UNITY_IPHONE
			public const string pluginName = "__Internal";
		#else
			public const string pluginName = "EffekseerUnity";
		#endif

		[DllImport(pluginName)]
		public static extern void EffekseerInit(int maxInstances, int maxSquares);
		
		[DllImport(pluginName)]
		public static extern void EffekseerTerm();
		
		[DllImport(pluginName)]
		public static extern void EffekseerUpdate(float deltaTime);
		
		[DllImport(pluginName)]
		public static extern IntPtr EffekseerGetRenderFunc();
		
		[DllImport(pluginName)]
		public static extern void EffekseerSetProjectionMatrix(int renderId, IntPtr matrix);
	
		[DllImport(pluginName)]
		public static extern void EffekseerSetCameraMatrix(int renderId, IntPtr matrix);
		
		[DllImport(pluginName)]
		public static extern IntPtr EffekseerLoadEffect(IntPtr path);
	
		[DllImport(pluginName)]
		public static extern void EffekseerReleaseEffect(IntPtr effect);
	
		[DllImport(pluginName)]
		public static extern int EffekseerPlayEffect(IntPtr effect, float x, float y, float z);
	
		[DllImport(pluginName)]
		public static extern void EffekseerStopEffect(int handle);
	
		[DllImport(pluginName)]
		public static extern void EffekseerStopAllEffects();
	
		[DllImport(pluginName)]
		public static extern void EffekseerSetShown(int handle, bool shown);
	
		[DllImport(pluginName)]
		public static extern void EffekseerSetPaused(int handle, bool paused);
	
		[DllImport(pluginName)]
		public static extern bool EffekseerExists(int handle);

		[DllImport(pluginName)]
		public static extern void EffekseerSetLocation(int handle, float x, float y, float z);

		[DllImport(pluginName)]
		public static extern void EffekseerSetRotation(int handle, float x, float y, float z, float angle);

		[DllImport(pluginName)]
		public static extern void EffekseerSetScale(int handle, float x, float y, float z);
	}
	#endregion
}

/// <summary>
/// 再生したエフェクトのインスタンスハンドル
/// </summary>
public struct EffekseerHandle
{
	private int m_handle;
	private bool m_paused;
	private bool m_shown;

	public EffekseerHandle(int handle)
	{
		m_handle = handle;
		m_paused = false;
		m_shown = false;
	}
	
	/// <summary>
	/// エフェクトを停止する
	/// </summary>
	public void Stop()
	{
		EffekseerSystem.Plugin.EffekseerStopEffect(m_handle);
	}
	
	/// <summary>
	/// エフェクトの位置を設定
	/// </summary>
	/// <param name="location">位置</param>
	public void SetLocation(Vector3 location)
	{
		EffekseerSystem.Plugin.EffekseerSetLocation(m_handle, location.x, location.y, location.z);
	}
	
	/// <summary>
	/// エフェクトの回転を設定
	/// </summary>
	/// <param name="rotation">回転</param>
	public void SetRotation(Quaternion rotation)
	{
		Vector3 axis;
		float angle;
		rotation.ToAngleAxis(out angle, out axis);
		EffekseerSystem.Plugin.EffekseerSetRotation(m_handle, axis.x, axis.y, axis.z, angle * Mathf.Deg2Rad);
	}
	
	/// <summary>
	/// エフェクトの拡縮を設定
	/// </summary>
	/// <param name="scale">拡縮</param>
	public void SetScale(Vector3 scale)
	{
		EffekseerSystem.Plugin.EffekseerSetScale(m_handle, scale.x, scale.y, scale.z);
	}

	/// <summary>
	/// Update時に更新するか
	/// </summary>
	public bool paused
	{
		set {
			EffekseerSystem.Plugin.EffekseerSetPaused(m_handle, value);
			m_paused = value;
		}
		get {
			return m_paused;
		}
	}
	
	/// <summary>
	/// Draw時で描画されるか
	/// </summary>
	public bool shown
	{
		set {
			EffekseerSystem.Plugin.EffekseerSetShown(m_handle, value);
			m_shown = value;
		}
		get {
			return m_shown;
		}
	}
	
	/// <summary>
	/// ハンドルが有効かどうか<br/>
	/// <para>true:有効</para>
	/// <para>false:無効</para>
	/// </summary>
	public bool enable
	{
		get {
			return m_handle >= 0;
		}
	}
	
	/// <summary>
	/// エフェクトのインスタンスが存在しているかどうか
	/// <para>true:存在している</para>
	/// <para>false:再生終了で破棄。もしくはStopで停止された</para>
	/// </summary>
	public bool exists
	{
		get {
			return EffekseerSystem.Plugin.EffekseerExists(m_handle);
		}
	}
}