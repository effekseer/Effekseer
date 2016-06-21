using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using System.IO;
using System.Runtime.Serialization;

namespace Effekseer
{
	[Serializable]
	internal abstract class Resource
	{
		public string path {get; protected set;}
		public AssetBundle assetBundle {get; protected set;}
		
		public abstract bool Load(string path, AssetBundle assetBundle);
		public abstract void Unload();

		protected T LoadAsset<T>(string path, bool removeExtension, AssetBundle assetBundle) where T : UnityEngine.Object {
			this.path = path;
			this.assetBundle = assetBundle;
			if (assetBundle != null) {
				return assetBundle.LoadAsset<T>(
					Utility.ResourcePath(path, removeExtension));
			} else {
				return Resources.Load<T>(
					Utility.ResourcePath(path, removeExtension));
			}
		}
		protected void UnloadAsset(UnityEngine.Object asset) {
			if (asset != null) {
				if (assetBundle != null) {
				} else {
					Resources.UnloadAsset(asset);
				}
			}
		}
	}

	[Serializable]
	internal class TextureResource : Resource
	{
		public Texture2D texture;
		public override bool Load(string path, AssetBundle assetBundle) {
			texture = LoadAsset<Texture2D>(path, true, assetBundle);
			if (texture == null) {
				Debug.LogError("[Effekseer] Failed to load Texture: " + path);
				return false;
			}
			return true;
		}
		public override void Unload() {
			UnloadAsset(texture);
			texture = null;
		}
		public IntPtr GetNativePtr() {
			return texture.GetNativeTexturePtr();
		}
	}
	[Serializable]
	internal class ModelResource : Resource
	{
		public TextAsset modelData;
		public override bool Load(string path, AssetBundle assetBundle) {
			modelData = LoadAsset<TextAsset>(path, false, assetBundle);
			if (modelData == null) {
				Debug.LogError("[Effekseer] Failed to load Model: " + path);
				return false;
			}
			return true;
		}
		public override void Unload() {
			UnloadAsset(modelData);
			modelData = null;
		}
		public bool Copy(IntPtr buffer, int bufferSize) {
			if (modelData.bytes.Length < bufferSize) {
				Marshal.Copy(modelData.bytes, 0, buffer, modelData.bytes.Length);
				return true;
			}
			return false;
		}
	}
	[Serializable]
	internal class SoundResource : Resource
	{
		public AudioClip audio;
		public override bool Load(string path, AssetBundle assetBundle) {
			audio = LoadAsset<AudioClip>(path, true, assetBundle);
			if (audio == null) {
				Debug.LogError("[Effekseer] Failed to load Sound: " + path);
				return false;
			}
			return true;
		}
		public override void Unload() {
			UnloadAsset(audio);
			audio = null;
		}
	}

	internal static class Utility
	{
		public static float[] Matrix2Array(Matrix4x4 mat) {
			float[] res = new float[16];
			res[ 0] = mat.m00; res[ 1] = mat.m01; res[ 2] = mat.m02; res[ 3] = mat.m03;
			res[ 4] = mat.m10; res[ 5] = mat.m11; res[ 6] = mat.m12; res[ 7] = mat.m13;
			res[ 8] = mat.m20; res[ 9] = mat.m21; res[10] = mat.m22; res[11] = mat.m23;
			res[12] = mat.m30; res[13] = mat.m31; res[14] = mat.m32; res[15] = mat.m33;
			return res;
		}

		public static string StrPtr16ToString(IntPtr strptr16, int len) {
			byte[] strarray = new byte[len * 2];
			Marshal.Copy(strptr16, strarray, 0, len * 2);
			return Encoding.Unicode.GetString(strarray);
		}

		public static string ResourcePath(string path, bool removeExtension) {
			string dir = Path.GetDirectoryName(path);
			string file = (removeExtension) ? 
				Path.GetFileNameWithoutExtension(path) : 
				Path.GetFileName(path);
			return "Effekseer/" + ((dir == String.Empty) ? file : dir + "/" + file);
		}
	}
	
	internal static class Plugin
	{
		#if !UNITY_EDITOR && (UNITY_IPHONE || UNITY_WEBGL)
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
		public static extern void EffekseerSetProjectionMatrix(int renderId, float[] matrix);
	
		[DllImport(pluginName)]
		public static extern void EffekseerSetCameraMatrix(int renderId, float[] matrix);
		
		[DllImport(pluginName)]
		public static extern void EffekseerSetRenderSettings(int renderId, bool renderIntoTexture);

		[DllImport(pluginName)]
		public static extern IntPtr EffekseerLoadEffect(IntPtr path);
		
		[DllImport(pluginName)]
		public static extern IntPtr EffekseerLoadEffectOnMemory(IntPtr data, int size);
	
		[DllImport(pluginName)]
		public static extern void EffekseerReleaseEffect(IntPtr effect);
	
		[DllImport(pluginName)]
		public static extern int EffekseerPlayEffect(IntPtr effect, float x, float y, float z);
	
		[DllImport(pluginName)]
		public static extern void EffekseerStopEffect(int handle);
	
		[DllImport(pluginName)]
		public static extern void EffekseerStopRoot(int handle);
	
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

		[DllImport(pluginName)]
		public static extern void EffekseerSetTargetLocation(int handle, float x, float y, float z);

		[DllImport(pluginName)]
		public static extern void EffekseerSetTextureLoaderEvent(
			EffekseerTextureLoaderLoad load,
			EffekseerTextureLoaderUnload unload);
		public delegate IntPtr EffekseerTextureLoaderLoad(IntPtr path);
		public delegate void EffekseerTextureLoaderUnload(IntPtr path);
		
		[DllImport(pluginName)]
		public static extern void EffekseerSetModelLoaderEvent(
			EffekseerModelLoaderLoad load,
			EffekseerModelLoaderUnload unload);
		public delegate int EffekseerModelLoaderLoad(IntPtr path, IntPtr buffer, int bufferSize);
		public delegate void EffekseerModelLoaderUnload(IntPtr path);
		
		[DllImport(pluginName)]
		public static extern void EffekseerSetSoundLoaderEvent(
			EffekseerSoundLoaderLoad load,
			EffekseerSoundLoaderUnload unload);
		public delegate int EffekseerSoundLoaderLoad(IntPtr path);
		public delegate void EffekseerSoundLoaderUnload(IntPtr path);
		
		[DllImport(pluginName)]
		public static extern void EffekseerSetSoundPlayerEvent(
			EffekseerSoundPlayerPlay play, 
			EffekseerSoundPlayerStopTag stopTag, 
			EffekseerSoundPlayerPauseTag pauseTag, 
			EffekseerSoundPlayerCheckPlayingTag checkPlayingTag, 
			EffekseerSoundPlayerStopAll atopAll);
		public delegate void EffekseerSoundPlayerPlay(IntPtr tag, 
			int data, float volume, float pan, float pitch, 
			bool mode3D, float x, float y, float z, float distance);
		public delegate void EffekseerSoundPlayerStopTag(IntPtr tag);
		public delegate void EffekseerSoundPlayerPauseTag(IntPtr tag, bool pause);
		public delegate bool EffekseerSoundPlayerCheckPlayingTag(IntPtr tag);
		public delegate void EffekseerSoundPlayerStopAll();
	}

	public class SoundInstance : MonoBehaviour {
		new private AudioSource audio;
		public string AudioTag;

		void Awake() {
			audio = gameObject.AddComponent<AudioSource>();
			audio.playOnAwake = false;
		}
		void Update() {
			if (audio.clip && !audio.isPlaying) {
				audio.clip = null;
			}
		}
		public void Play(string tag, AudioClip clip, 
			float volume, float pan, float pitch, 
			bool mode3D, float x, float y, float z, float distance)
		{
			this.AudioTag = tag;
			transform.position = new Vector3(x, y, z);
			audio.spatialBlend = (mode3D) ? 1.0f : 0.0f;
			audio.volume = volume;
			audio.pitch = Mathf.Pow(2.0f, pitch);
			audio.panStereo = pan;
			audio.minDistance = distance;
			audio.maxDistance = distance * 2;
			audio.clip = clip;
			audio.Play();
		}
		public void Stop() {
			audio.Stop();
		}
		public void Pause(bool paused) {
			if (paused) audio.Pause();
			else audio.UnPause();
		}
		public bool CheckPlaying() {
			return audio.isPlaying;
		}
	}
}