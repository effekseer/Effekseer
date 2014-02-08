using UnityEngine;
using System;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

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
	
	private Dictionary<string, IntPtr> effects = null;
	
	private IntPtr _GetEffect(string name) {
		if (effects.ContainsKey(name) == false) {
			string path = Path.ChangeExtension(name, "efk");
			string fullPath = Path.Combine(EffekseerSystem.resourcePath, path);
			IntPtr effect = Plugin.EffekseerLoadEffect(fullPath);
			if (effect == IntPtr.Zero) {
				Debug.LogError(fullPath + " is not found.");
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
	
	void OnEnable() {
		effects = new Dictionary<string, IntPtr>();
	}
	
	void OnDisable() {
		foreach (var pair in effects) {
			Plugin.EffekseerReleaseEffect(pair.Value);
		}
		effects = null;
	}
	
	void FixedUpdate() {
		Plugin.EffekseerUpdate(Time.deltaTime / 0.016666666f);
	}
	
	void OnRenderObject() {
		Camera cam = Camera.current;
		Plugin.EffekseerSetProjection(cam.fieldOfView, cam.aspect, cam.nearClipPlane, cam.farClipPlane);
		
		Vector3 eye = cam.transform.position;
		Vector3 at = cam.transform.position + cam.transform.forward;
		Vector3 up = cam.transform.up;
		Plugin.EffekseerSetCamera(eye.x, eye.y, eye.z, at.x, at.y, at.z, up.x, up.y, up.z);
		
		GL.IssuePluginEvent(1);
	}
	
	public static class Plugin
	{
		public const string pluginName = "EffekseerUnity";
	
		[DllImport(pluginName)]
		public static extern void EffekseerUpdate(float deltaTime);
	
		[DllImport(pluginName)]
		public static extern void EffekseerSetProjection(float fov, float aspect, float nearZ, float farZ);
	
		[DllImport(pluginName)]
		public static extern void EffekseerSetCamera(float ex, float ey, float ez, 
			float ax, float ay, float az, float ux, float uy, float uz);
	
		[DllImport(pluginName)]
		public static extern IntPtr EffekseerLoadEffect(string path);
	
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