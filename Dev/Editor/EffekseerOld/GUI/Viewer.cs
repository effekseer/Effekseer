using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.GUI
{
	public class Viewer
	{
		swig.Native native = null;
		string backgroundImagePath = string.Empty;
		bool isViewerShown = false;

		int random_seed = 0;
		Random rand = new Random();

		int current = 0;

		/// <summary>
		/// Is effect playing
		/// </summary>
		public bool IsPlaying { get; private set; } = false;

		/// <summary>
		/// Is effect paused
		/// </summary>
		public bool IsPaused { get; private set; } = false;

		/// <summary>
		/// Is effect changed
		/// </summary>
		public bool IsChanged { get; set; } = true;

		/// <summary>
		/// current frame
		/// </summary>
		public int Current
		{
			get
			{
				return current;
			}
			set
			{
				Step(value);
			}
		}

		public Viewer(swig.Native native)
		{
			this.native = native;
		}

		public bool ResizeWindow(int width, int height)
		{
			// Hack for old GUI
			if (!isViewerShown) return true;

			return native.ResizeWindow(width, height);
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
			if (!isViewerShown) return false;
			return native.Zoom(zoom);
		}

		public bool SetRandomSeed(int seed)
		{
			return native.SetRandomSeed(seed);
		}

		public bool LoadEffectFunc()
		{
			StopViewer();

			RemoveEffect();

			Export();

			IsChanged = false;

			return true;
		}

		public bool Record(string path, int count, int offsetFrame, int freq, swig.TransparenceType transparenceType)
		{
			var dir = System.IO.Path.GetDirectoryName(path);
			var fileWExt = System.IO.Path.GetFileNameWithoutExtension(path);
			var ext = System.IO.Path.GetExtension(path);

			return native.Record(dir + "/" + fileWExt, ext, count, offsetFrame, freq, transparenceType);
		}

		public bool Record(string path, int count, int xCount, int offsetFrame, int freq, swig.TransparenceType transparenceType)
		{
			return native.Record(path, count, xCount, offsetFrame, freq, transparenceType);
		}

		public bool RecordAsGifAnimation(string path, int count, int offsetFrame, int freq, swig.TransparenceType transparenceType)
		{
			return native.RecordAsGifAnimation(path, count, offsetFrame, freq, transparenceType);
		}

		public bool RecordAsAVI(string path, int count, int offsetFrame, int freq, swig.TransparenceType transparenceType)
		{
			return native.RecordAsAVI(path, count, offsetFrame, freq, transparenceType);
		}


		public swig.ViewerParamater GetViewerParamater()
		{
			return native.GetViewerParamater();
		}

		public void SetViewerParamater(swig.ViewerParamater paramater)
		{
			native.SetViewerParamater(paramater);
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

		public void SetAllColor(byte r, byte g, byte b, byte a)
		{
			var behavior = native.GetEffectBehavior();
			behavior.AllColorR = r;
			behavior.AllColorG = g;
			behavior.AllColorB = b;
			behavior.AllColorA = a;
			native.SetViewerEffectBehavior(behavior);
		}

		public void SetEffectTimeSpan(int timeSpan)
		{
			var behavior = native.GetEffectBehavior();
			behavior.TimeSpan = timeSpan;
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

		public void SetDistortionType(int type)
		{
			var param = native.GetViewerParamater();

			param.Distortion = (swig.DistortionType)type;

			native.SetViewerParamater(param);
		}
		
		public void SetRenderMode(int renderMode)
		{
			var param = native.GetViewerParamater();

			param.RenderingMode = (swig.RenderMode)renderMode;

			native.SetViewerParamater(param);
		}

		public void SetIsRightHand(bool value)
		{
			native.SetIsRightHand(value);
		}

		public void SetMouseInverseFlag(bool rotX, bool rotY, bool slideX, bool slideY)
		{
			native.SetMouseInverseFlag(rotX, rotY, slideX, slideY);
		}

		public bool ShowViewer(IntPtr handle, int width, int height, swig.DeviceType deviceType)
		{
			if (isViewerShown) return false;

			if(native == null)
			{
				throw new Exception("native is null.");
			}

			if (native.CreateWindow_Effekseer(
				handle, width <= 0 ? 1 : width, 
				height <= 0 ? 1 : height,
				Core.Option.ColorSpace.Value == Data.OptionValues.ColorSpaceType.LinearSpace,
				deviceType))
			{
				isViewerShown = true;
				return true;
			}
			else
			{
				Core.OnOutputMessage("描画画面の生成に失敗しました。DirectXのバージョンの問題、メモリの不足等が考えられます。");
			}

			return false;
		}

		public void HideViewer()
		{
			if (!isViewerShown) return;
			isViewerShown = false;
			native.DestroyWindow();
		}

		public void UpdateViewer()
		{
			if (isViewerShown)
			{
				if (IsChanged && (IsPlaying || IsPaused))
				{
					Reload(false);
					IsChanged = false;
				}

				if (IsPlaying && !IsPaused)
				{
					StepViewer(true);
				}

				native.SetBackgroundColor(
				(byte)Core.Option.BackgroundColor.R,
				(byte)Core.Option.BackgroundColor.G,
				(byte)Core.Option.BackgroundColor.B);

				native.SetBackgroundImage(Core.Option.BackgroundImage.AbsolutePath);

				native.SetGridColor(
				(byte)Core.Option.GridColor.R,
				(byte)Core.Option.GridColor.G,
				(byte)Core.Option.GridColor.B,
				(byte)Core.Option.GridColor.A);

				native.SetGridLength(
					Core.Option.GridLength);

				native.SetIsGridShown(
					Core.Option.IsGridShown,
					Core.Option.IsXYGridShown,
					Core.Option.IsXZGridShown,
					Core.Option.IsYZGridShown);

				native.SetLightDirection(
					Core.Option.LightDirection.X,
					Core.Option.LightDirection.Y,
					Core.Option.LightDirection.Z);

				native.SetLightColor(
					(byte)Core.Option.LightColor.R,
					(byte)Core.Option.LightColor.G,
					(byte)Core.Option.LightColor.B,
					(byte)Core.Option.LightColor.A);

				native.SetLightAmbientColor(
					(byte)Core.Option.LightAmbientColor.R,
					(byte)Core.Option.LightAmbientColor.G,
					(byte)Core.Option.LightAmbientColor.B,
					(byte)Core.Option.LightAmbientColor.A);

				native.SetMouseInverseFlag(
					Core.Option.MouseRotInvX,
					Core.Option.MouseRotInvY,
					Core.Option.MouseSlideInvX,
					Core.Option.MouseSlideInvY);

				if (Core.Culling.Type.Value == Data.EffectCullingValues.ParamaterType.Sphere)
				{
					native.SetCullingParameter(Core.Culling.IsShown, Core.Culling.Sphere.Radius.Value, Core.Culling.Sphere.Location.X, Core.Culling.Sphere.Location.Y, Core.Culling.Sphere.Location.Z);
				}
				else if (Core.Culling.Type.Value == Data.EffectCullingValues.ParamaterType.None)
				{
					native.SetCullingParameter(false, 0.0f, 0.0f, 0.0f, 0.0f);
				}
			}
			else
			{
				System.Threading.Thread.Sleep(1);
			}
		}

		public void UpdateWindow()
		{
			if(isViewerShown)
			{
				native.UpdateWindow();
				native.RenderWindow();
			}
		}

		public void PresentWindow()
		{
			if (isViewerShown)
			{
				native.Present();
			}
		}

		public void PlayViewer()
		{
			if (isViewerShown)
			{
				PlayNew();
				IsPaused = false;
			}
		}

		public void StopViewer()
		{
			if (isViewerShown)
			{
				native.StopEffect();

				IsPlaying = false;
				IsPaused = false;
				current = Core.StartFrame;
			}
		}

		public void PauseAndResumeViewer()
		{
			if (isViewerShown)
			{
				IsPaused = !IsPaused;
			}
		}

		public void StepViewer(bool isLooping)
		{
			int next = Current + 1;

			if (isLooping)
			{
				if (next > Core.EndFrame) next = 0;
			}

			Step(next);
		}

		public void BackStepViewer()
		{
			Step(Current - 1);
		}

		unsafe void Export()
		{
			SetLotationVelocity(
				Core.EffectBehavior.LocationVelocity.X,
				Core.EffectBehavior.LocationVelocity.Y,
				Core.EffectBehavior.LocationVelocity.Z);

			SetRotationVelocity(
				Core.EffectBehavior.RotationVelocity.X / 180.0f * 3.141592f,
				Core.EffectBehavior.RotationVelocity.Y / 180.0f * 3.141592f,
				Core.EffectBehavior.RotationVelocity.Z / 180.0f * 3.141592f);

			SetScaleVelocity(
				Core.EffectBehavior.ScaleVelocity.X,
				Core.EffectBehavior.ScaleVelocity.Y,
				Core.EffectBehavior.ScaleVelocity.Z);

			if (Core.EffectBehavior.RemovedTime.Infinite.Value)
			{
				SetRemovedTime(int.MaxValue);
			}
			else
			{
				SetRemovedTime(Core.EffectBehavior.RemovedTime.Value);
			}

			SetLotation(
				Core.EffectBehavior.Location.X,
				Core.EffectBehavior.Location.Y,
				Core.EffectBehavior.Location.Z);

			SetRotation(
				Core.EffectBehavior.Rotation.X / 180.0f * 3.141592f,
				Core.EffectBehavior.Rotation.Y / 180.0f * 3.141592f,
				Core.EffectBehavior.Rotation.Z / 180.0f * 3.141592f);

			SetScale(
				Core.EffectBehavior.Scale.X,
				Core.EffectBehavior.Scale.Y,
				Core.EffectBehavior.Scale.Z);

			SetTargetLocation(
				Core.EffectBehavior.TargetLocation.X,
				Core.EffectBehavior.TargetLocation.Y,
				Core.EffectBehavior.TargetLocation.Z);

			SetEffectCount(
				Core.EffectBehavior.CountX,
				Core.EffectBehavior.CountY,
				Core.EffectBehavior.CountZ);

			SetAllColor(
				(byte)Core.EffectBehavior.ColorAll.R,
				(byte)Core.EffectBehavior.ColorAll.G,
				(byte)Core.EffectBehavior.ColorAll.B,
				(byte)Core.EffectBehavior.ColorAll.A);

			SetEffectTimeSpan(Core.EffectBehavior.TimeSpan);

			SetEffectDistance(Core.EffectBehavior.Distance);

			SetBackgroundColor(
				(byte)Core.Option.BackgroundColor.R,
				(byte)Core.Option.BackgroundColor.G,
				(byte)Core.Option.BackgroundColor.B);

			SetGridLength(
				Core.Option.GridLength);

			SetStep((int)Core.Option.FPS.Value);
			SetIsRightHand(Core.Option.Coordinate.Value == Data.OptionValues.CoordinateType.Right);

			SetDistortionType((int)Core.Option.DistortionType.Value);
			SetRenderMode((int)Core.Option.RenderingMode.Value);
			
			if (Core.Culling.Type.Value == Data.EffectCullingValues.ParamaterType.Sphere)
			{
				SetCullingParameter(Core.Culling.IsShown, Core.Culling.Sphere.Radius.Value, Core.Culling.Sphere.Location.X, Core.Culling.Sphere.Location.Y, Core.Culling.Sphere.Location.Z);
			}
			else if (Core.Culling.Type.Value == Data.EffectCullingValues.ParamaterType.None)
			{
				SetCullingParameter(false, 0.0f, 0.0f, 0.0f, 0.0f);
			}

			var binaryExporter = new Binary.Exporter();
			var data = binaryExporter.Export(Core.Option.Magnification);
			fixed (byte* p = &data[0])
			{
				LoadEffect(new IntPtr(p), data.Length, Core.FullPath);
			}
		}

		/// <summary>
		/// 新規再生
		/// </summary>
		unsafe void PlayNew()
		{
			native.StopEffect();

			if (IsChanged)
			{
				RemoveEffect();

				Export();

				IsChanged = false;
			}

			random_seed = rand.Next(0, 0xffff);
			SetRandomSeed(random_seed);
			PlayEffect();
			IsPlaying = true;

			if (Core.StartFrame > 0)
			{
				StepEffect(Core.StartFrame);
			}
			current = Core.StartFrame;
		}

		/// <summary>
		/// 状態はそのまま、エフェクトのデータだけ差し替え
		/// </summary>
		public unsafe void Reload(bool isResourceReloaded)
		{
			if(isResourceReloaded)
			{
				native.InvalidateTextureCache();
			}

			native.StopEffect();

			RemoveEffect();

			Export();

			SetRandomSeed(random_seed);
			PlayEffect();
			StepEffect(Current);
		}

		/// <summary>
		/// 指定フレームに移動
		/// </summary>
		/// <param name="new_frame"></param>
		unsafe void Step(int new_frame)
		{
			// 同一フレーム
			if (current == new_frame) return;

			if (new_frame < Core.StartFrame) new_frame = Core.StartFrame;
			if (new_frame > Core.EndFrame) new_frame = Core.EndFrame;

			if (isViewerShown)
			{
				if (!IsPlaying)
				{
					PlayNew();
					PauseAndResumeViewer();
				}

				if (IsPaused)
				{
					if (current == new_frame)
					{
					}
					else if (current > new_frame)
					{
						native.StopEffect();
						SetRandomSeed(random_seed);
						PlayEffect();
						StepEffect(new_frame);
					}
					else
					{
						StepEffect(new_frame - current);
					}
					current = new_frame;
				}
				else
				{
					if (current == new_frame)
					{
					}
					else if (current > new_frame)
					{
						native.StopEffect();

						PlayEffect();
						StepEffect(new_frame);
					}
					else
					{
						StepEffect(new_frame - current);
					}
					current = new_frame;
				}

			}
		}
	}
}
