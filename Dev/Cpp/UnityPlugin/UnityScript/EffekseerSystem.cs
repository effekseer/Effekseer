using UnityEngine;
using System;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using UnityEditor;

public class EffekseerSystem : MonoBehaviour
{
	public static string resourcePath
	{
		get {
			return Path.Combine(Application.streamingAssetsPath, "Effekseer");
		}
	}

	public static EffekseerHandle PlayEffect(string name, Vector3 location)
	{
		IntPtr effect = Instance._GetEffect(name);
		if (effect != IntPtr.Zero) {
			int handle = Plugin.EffekseerPlayEffect(effect, location.x, location.y, location.z);
			return new EffekseerHandle(handle);
		}
		return new EffekseerHandle(-1);
	}
	
	public static void StopAllEffects()
	{
		Plugin.EffekseerStopAllEffects();
	}

	public static void LoadEffect(string name)
	{
		Instance._GetEffect(name);
	}

	public static void ReleaseEffect(string name)
	{
		Instance._ReleaseEffect(name);
	}

	#region Implimentation
	private static EffekseerSystem _Instance = null;
	
	public static EffekseerSystem Instance
	{
		get {
			if (_Instance == null) {
				var go = new GameObject("Effekseer");
				_Instance = go.AddComponent<EffekseerSystem>();
			}
			return _Instance;
		}
	}
	
	public bool drawInSceneView = true;
	public int maxInstances		= 800;
	public int maxSquares		= 1200;

	private const int renderEventId = 0x2040;
	private Dictionary<string, IntPtr> effects = null;
	
	private IntPtr _GetEffect(string name) {
		if (effects.ContainsKey(name) == false) {
			string path = Path.ChangeExtension(name, "efk");
			string fullPath = Path.Combine(EffekseerSystem.resourcePath, path);
			
			byte[] bytes = Encoding.Unicode.GetBytes(fullPath);
			GCHandle ghc = GCHandle.Alloc(bytes, GCHandleType.Pinned);
			IntPtr effect = Plugin.EffekseerLoadEffect(ghc.AddrOfPinnedObject());
			ghc.Free();
			if (effect == IntPtr.Zero) {
				Debug.LogError("[Effekseer] Error loading " + fullPath);
				return IntPtr.Zero;
			}
			effects.Add(name, effect);
			return effect;
		} else {
			return effects[name];
		}
	}
	
	private void _ReleaseEffect(string name) {
		if (effects.ContainsKey(name) == false) {
			var effect = effects[name];
			Plugin.EffekseerReleaseEffect(effect);
			effects.Remove(name);
		}
	}
	
	void Awake() {
		effects = new Dictionary<string, IntPtr>();
		Plugin.EffekseerInit(maxInstances, maxSquares);
	}
	
	void OnDestroy() {
		foreach (var pair in effects) {
			Plugin.EffekseerReleaseEffect(pair.Value);
		}
		effects = null;
		Plugin.EffekseerTerm();
	}
	
	void FixedUpdate() {
		Plugin.EffekseerUpdate(1);
	}
	
	void OnRenderObject() {
		#if UNITY_EDITOR
			if (drawInSceneView == false && 
				SceneView.currentDrawingSceneView != null && 
				Camera.current == SceneView.currentDrawingSceneView.camera
			) {
				return;
			}
		#endif
		
		{
			float[] projectionMatrixArray = Matrix2Array(Camera.current.projectionMatrix);
			GCHandle ghc = GCHandle.Alloc(projectionMatrixArray, GCHandleType.Pinned);
			Plugin.EffekseerSetProjectionMatrix(ghc.AddrOfPinnedObject());
			ghc.Free();
		}
		{
			float[] cameraMatrixArray = Matrix2Array(Camera.current.worldToCameraMatrix);
			GCHandle ghc = GCHandle.Alloc(cameraMatrixArray, GCHandleType.Pinned);
			Plugin.EffekseerSetCameraMatrix(ghc.AddrOfPinnedObject());
			ghc.Free();
		}

		GL.IssuePluginEvent(renderEventId);
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
		public static extern void EffekseerSetProjectionMatrix(IntPtr matrix);
	
		[DllImport(pluginName)]
		public static extern void EffekseerSetCameraMatrix(IntPtr matrix);
		
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

public struct EffekseerHandle
{
	int handle;

	public EffekseerHandle(int _handle)
	{
		handle = _handle;
	}
	
	public void Stop()
	{
		EffekseerSystem.Plugin.EffekseerStopEffect(handle);
	}
	
	public void SetLocation(Vector3 location)
	{
		EffekseerSystem.Plugin.EffekseerSetLocation(handle, location.x, location.y, location.z);
	}
	
	public void SetRotation(Quaternion rotation)
	{
		Vector3 axis;
		float angle;
		rotation.ToAngleAxis(out angle, out axis);
		EffekseerSystem.Plugin.EffekseerSetRotation(handle, axis.x, axis.y, axis.z, angle * Mathf.Deg2Rad);
	}
	
	public void SetScale(Vector3 scale)
	{
		EffekseerSystem.Plugin.EffekseerSetScale(handle, scale.x, scale.y, scale.z);
	}
	
	public bool enable
	{
		get {
			return handle >= 0;
		}
	}
	
	public bool exists
	{
		get {
			return EffekseerSystem.Plugin.EffekseerExists(handle);
		}
	}
}