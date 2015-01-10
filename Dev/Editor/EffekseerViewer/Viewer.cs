using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer
{
	public class Viewer : IViewer
	{
		Native native = null;
		string backgroundImagePath = string.Empty;

		public Viewer()
		{
			native = new Native();
		}

		public bool CreateWindow(IntPtr handle, int width, int height)
		{
			return native.CreateWindow_Effekseer(handle, width, height);
		}

		public bool UpdateWindow()
		{
			return native.UpdateWindow();
		}

		public bool ResizeWindow(int width, int height)
		{
			return native.ResizeWindow(width, height);
		}

		public bool DestroyWindow()
		{
			return native.DestroyWindow();
		}

		public bool LoadEffect(IntPtr data, int size, string path)
		{
			return native.LoadEffect(data, size, path);
		}

		public bool RemoveEffect()
		{
			return native.RemoveEffect();
		}

		public bool PlayEffect()
		{
			return native.PlayEffect();
		}

		public bool StopEffect()
		{
			return native.StopEffect();
		}

		public bool StepEffect(int frame)
		{
			return native.StepEffect( frame );
		}

		public bool Rotate(float x, float y)
		{
			return native.Rotate(x, y);
		}

		public bool Slide(float x, float y)
		{
			return native.Slide(x, y);
		}

		public bool Zoom(float zoom)
		{
			return native.Zoom(zoom);
		}

		public bool SetRandomSeed(int seed)
		{
			return native.SetRandomSeed(seed);
		}

		public bool Record(string path, int xCount, int yCount, int offsetFrame, int frameSkip, bool isTranslucent)
		{
			return native.Record(path, xCount, yCount, offsetFrame, frameSkip, isTranslucent);
		}

		public ViewerParamater GetViewerParamater()
		{
			return native.GetViewerParamater();
		}

		public void SetViewerParamater(ViewerParamater paramater)
		{
			native.SetViewerParamater(paramater);
		}

		public Func<bool> LoadEffectFunc
		{
			get;
			set;
		}

        public void SetSoundMute(bool mute)
        {
            native.SetSoundMute(mute);
        }

        public void SetSoundVolume(float volume)
        {
            native.SetSoundVolume(volume);
        }

        public void InvalidateTextureCache()
        {
            native.InvalidateTextureCache();
        }

		public void SetLotation(float x, float y, float z)
		{
			var behavior = native.GetEffectBehavior();
			behavior.PositionX = x;
			behavior.PositionY = y;
			behavior.PositionZ = z;
			native.SetViewerEffectBehavior(behavior);
		}

		public void SetRotation(float x, float y, float z)
		{
			var behavior = native.GetEffectBehavior();
			behavior.RotationX = x;
			behavior.RotationY = y;
			behavior.RotationZ = z;
			native.SetViewerEffectBehavior(behavior);
		}

		public void SetScale(float x, float y, float z)
		{
			var behavior = native.GetEffectBehavior();
			behavior.ScaleX = x;
			behavior.ScaleY = y;
			behavior.ScaleZ = z;
			native.SetViewerEffectBehavior(behavior);
		}

		public void SetLotationVelocity(float x, float y, float z)
		{
			var behavior = native.GetEffectBehavior();
			behavior.PositionVelocityX = x;
			behavior.PositionVelocityY = y;
			behavior.PositionVelocityZ = z;
			native.SetViewerEffectBehavior(behavior);
		}

		public void SetRotationVelocity(float x, float y, float z)
		{
			var behavior = native.GetEffectBehavior();
			behavior.RotationVelocityX = x;
			behavior.RotationVelocityY = y;
			behavior.RotationVelocityZ = z;
			native.SetViewerEffectBehavior(behavior);
		}

		public void SetScaleVelocity(float x, float y, float z)
		{
			var behavior = native.GetEffectBehavior();
			behavior.ScaleVelocityX = x;
			behavior.ScaleVelocityY = y;
			behavior.ScaleVelocityZ = z;
			native.SetViewerEffectBehavior(behavior);
		}
		
		public void SetTargetLocation(float x, float y, float z)
		{
			var behavior = native.GetEffectBehavior();
			behavior.TargetPositionX = x;
			behavior.TargetPositionY = y;
			behavior.TargetPositionZ = z;
			native.SetViewerEffectBehavior(behavior);
		}

		public void SetRemovedTime(int time)
		{
			var behavior = native.GetEffectBehavior();
			behavior.RemovedTime = time;
			native.SetViewerEffectBehavior(behavior);
		}

		public void SetEffectCount(int x, int y, int z)
		{
			var behavior = native.GetEffectBehavior();
			behavior.CountX = x;
			behavior.CountY = y;
			behavior.CountZ = z;
			native.SetViewerEffectBehavior(behavior);
		}

		public void SetEffectDistance(float distance)
		{
			var behavior = native.GetEffectBehavior();
			behavior.Distance = distance;
			native.SetViewerEffectBehavior(behavior);
		}

		public void SetIsGridShown(bool value, bool xy, bool xz, bool yz)
		{
			native.SetIsGridShown(value, xy, xz, yz);
		}

		public void SetGridLength(float length)
		{
			native.SetGridLength(length);
		}

		public void SetBackgroundColor(byte r, byte g, byte b)
		{
			native.SetBackgroundColor(r, g, b);
		}

		public void SetBackgroundImage(string path)
		{
			if (backgroundImagePath == path) return;

			native.SetBackgroundImage(path);
			backgroundImagePath = path;
		}

		public void SetGridColor(byte r, byte g, byte b, byte a)
		{
			native.SetGridColor(r, g, b, a);
		}

		public void SetStep(int step)
		{
			native.SetStep(step);
		}

		public void SetCullingParameter(bool isCullingShown, float cullingRadius, float cullingX, float cullingY, float cullingZ)
		{
			native.SetCullingParameter(isCullingShown, cullingRadius, cullingX, cullingY, cullingZ);
		}

		public bool Connect(string target, int port)
		{
			return native.StartNetwork(target, (ushort)port);
		}

		public bool IsConnected()
		{
			return native.IsConnectingNetwork();
		}

		/// <summary>
		/// サーバーから切断する。
		/// </summary>
		/// <returns></returns>
		public void Disconnect()
		{
			native.StopNetwork();
		}

		/// <summary>
		/// サーバーにデータを送信する。
		/// </summary>
		/// <param name="key"></param>
		/// <param name="data"></param>
		/// <param name="size"></param>
		/// <param name="path"></param>
		/// <returns></returns>
		public void SendEffect(string key, IntPtr data, int size, string path)
		{
			native.SendDataByNetwork(key, data, size, path);
		}
		
		public void SetLightDirection(float x, float y, float z)
		{
			native.SetLightDirection(x, y, z);
		}

		public void SetLightColor(byte r, byte g, byte b, byte a)
		{
			native.SetLightColor(r, g, b, a);
		}

		public void SetLightAmbientColor(byte r, byte g, byte b, byte a)
		{
			native.SetLightAmbientColor(r, g, b, a);
		}

		public void SetIsRightHand(bool value)
		{
			native.SetIsRightHand(value);
		}

		public void SetMouseInverseFlag(bool rotX, bool rotY, bool slideX, bool slideY)
		{
			native.SetMouseInverseFlag(rotX, rotY, slideX, slideY);
		}
	}
}
