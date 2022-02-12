using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.GUI
{
	public class Viewer : IDisposable
	{
		HardwareDevice hardwareDevice;
		swig.EffectSetting effectSetting;
		public swig.MainScreenEffectRenderer EffectRenderer { get; private set; }

		public swig.Effect CurrentEffect { get; private set; }
		swig.EffectFactory effectFactory;

		public swig.ViewPointController ViewPointController = null;
		string backgroundImagePath = string.Empty;
		bool isViewerShown = false;

		int random_seed = 0;
		Random rand = new Random();

		float current = 0;

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
		/// Is required to reload in the next frame
		/// </summary>
		public bool IsRequiredToReload { get; set; } = false;

		/// <summary>
		/// current frame
		/// </summary>
		public float Current
		{
			get
			{
				return current;
			}
			set
			{
				MoveFrame(value);
			}
		}

		public Viewer(HardwareDevice hardwareDevice)
		{
			this.hardwareDevice = hardwareDevice;
		}

		public void Dispose()
		{
			if (CurrentEffect != null)
			{
				CurrentEffect.Dispose();
				CurrentEffect = null;
			}

			if (effectFactory != null)
			{
				effectFactory.Dispose();
				effectFactory = null;
			}

			if (effectSetting != null)
			{
				effectSetting.Dispose();
				effectSetting = null;
			}

			if (EffectRenderer != null)
			{
				EffectRenderer.Dispose();
				EffectRenderer = null;
			}
		}

		public void StepEffectFrame(int frame)
		{
			EffectRenderer.Update(frame);
		}

		public bool Rotate(float x, float y)
		{
			return ViewPointController.Rotate(x, y);
		}

		public bool Slide(float x, float y)
		{
			return ViewPointController.Slide(x, y);
		}

		public bool Zoom(float zoom)
		{
			if (!isViewerShown) return false;
			return ViewPointController.Zoom(zoom);
		}

		public bool LoadEffectFunc()
		{
			StopViewer();

			EffectRenderer.SetEffect(null);
			EffectRenderer.ResetEffect();

			Export();

			IsChanged = false;

			return true;
		}

		swig.EffectRecorder CreateRecorder(swig.RecordingParameter recordingParameter)
		{
			var screenSize = EffectRenderer.GetScreenSize();

			var recorder = new swig.EffectRecorder();
			if (recorder.Begin(
					EffectRenderer.GetSquareMaxCount(),
					EffectRenderer.GetParameter(),
					screenSize,
					hardwareDevice.GraphicsDevice,
					effectSetting,
					recordingParameter,
					new swig.Vector2I(EffectRenderer.GuideWidth, EffectRenderer.GuideHeight),
					EffectRenderer.GetIsSRGBMode(),
					EffectRenderer.GetBehavior(),
					EffectRenderer.GetPostEffectParameter(),
					EffectRenderer.GetEffect()))
			{
				return recorder;
			}

			recorder.Dispose();
			return null;
		}

		public swig.EffectRecorder RecordSprite(string path, swig.RecordingParameter recordingParameter)
		{
			var dir = System.IO.Path.GetDirectoryName(path);
			var fileWExt = System.IO.Path.GetFileNameWithoutExtension(path);
			var ext = System.IO.Path.GetExtension(path);

			recordingParameter.SetPath(dir + "/" + fileWExt);
			recordingParameter.SetExt(ext);
			recordingParameter.RecordingMode = swig.RecordingModeType.Sprite;
			return CreateRecorder(recordingParameter);
		}

		public swig.EffectRecorder RecordSpriteSheet(string path, swig.RecordingParameter recordingParameter)
		{
			var dir = System.IO.Path.GetDirectoryName(path);
			var fileWExt = System.IO.Path.GetFileNameWithoutExtension(path);
			var ext = System.IO.Path.GetExtension(path);

			recordingParameter.SetPath(dir + "/" + fileWExt);
			recordingParameter.SetExt(ext);
			recordingParameter.RecordingMode = swig.RecordingModeType.SpriteSheet;
			return CreateRecorder(recordingParameter);
		}

		public swig.EffectRecorder RecordAsGifAnimation(string path, swig.RecordingParameter recordingParameter)
		{
			var dir = System.IO.Path.GetDirectoryName(path);
			var fileWExt = System.IO.Path.GetFileNameWithoutExtension(path);
			var ext = System.IO.Path.GetExtension(path);

			recordingParameter.SetPath(dir + "/" + fileWExt);
			recordingParameter.SetExt(ext);
			recordingParameter.RecordingMode = swig.RecordingModeType.Gif;
			return CreateRecorder(recordingParameter);
		}

		public swig.EffectRecorder RecordAsAVI(string path, swig.RecordingParameter recordingParameter)
		{
			var dir = System.IO.Path.GetDirectoryName(path);
			var fileWExt = System.IO.Path.GetFileNameWithoutExtension(path);
			var ext = System.IO.Path.GetExtension(path);

			recordingParameter.SetPath(dir + "/" + fileWExt);
			recordingParameter.SetExt(ext);
			recordingParameter.RecordingMode = swig.RecordingModeType.Avi;
			return CreateRecorder(recordingParameter);
		}

		public swig.EffectRecorder RecordAsH264(string path, swig.RecordingParameter recordingParameter)
		{
			var dir = System.IO.Path.GetDirectoryName(path);
			var fileWExt = System.IO.Path.GetFileNameWithoutExtension(path);
			var ext = System.IO.Path.GetExtension(path);

			recordingParameter.SetPath(dir + "/" + fileWExt);
			recordingParameter.SetExt(ext);
			recordingParameter.RecordingMode = swig.RecordingModeType.H264;
			return CreateRecorder(recordingParameter);
		}

		public void SendTrigger(int index)
		{
			var behavior = EffectRenderer.GetBehavior();
			switch (index)
			{
				case 0: behavior.TriggerInput0 = true; break;
				case 1: behavior.TriggerInput1 = true; break;
				case 2: behavior.TriggerInput2 = true; break;
				case 3: behavior.TriggerInput3 = true; break;
			}
			EffectRenderer.SetBehavior(behavior);
		}

		public void SetDynamicInput(float v1, float v2, float v3, float v4)
		{
			var behavior = EffectRenderer.GetBehavior();
			behavior.DynamicInput1 = v1;
			behavior.DynamicInput2 = v2;
			behavior.DynamicInput3 = v3;
			behavior.DynamicInput4 = v4;
			EffectRenderer.SetBehavior(behavior);
		}

		public bool Initialize(swig.DeviceType deviceType)
		{
			if (isViewerShown) return false;

			// because internal buffer is 16bit
			var spriteCount = 65000 / 4;

			effectSetting = swig.EffectSetting.Create(hardwareDevice.GraphicsDevice, hardwareDevice.SoundDevice);

			EffectRenderer = new swig.MainScreenEffectRenderer();
			EffectRenderer.Initialize(hardwareDevice.GraphicsDevice, hardwareDevice.SoundDevice, effectSetting, spriteCount, hardwareDevice.GraphicsDevice.GetIsSRGBMode());

			ViewPointController = new swig.ViewPointController();

			ViewPointController.ProjectionStyle = deviceType == swig.DeviceType.OpenGL ? swig.ProjectionMatrixStyle.OpenGLStyle : swig.ProjectionMatrixStyle.DirectXStyle;

			ViewMode_OnChanged(null, null);
			Core.Option.ViewerMode.OnChanged += ViewMode_OnChanged;

			Core.Option.RenderingMode.OnChanged += RenderingMode_OnChanged;
			RenderingMode_OnChanged(null, null);

			Bloom_OnChanged(null, null);
			Core.Environment.PostEffect.BloomSwitch.OnChanged += Bloom_OnChanged;
			Core.Environment.PostEffect.Bloom.Intensity.OnChanged += Bloom_OnChanged;
			Core.Environment.PostEffect.Bloom.Threshold.OnChanged += Bloom_OnChanged;
			Core.Environment.PostEffect.Bloom.SoftKnee.OnChanged += Bloom_OnChanged;

			Tonemap_OnChanged(null, null);
			Core.Environment.PostEffect.TonemapSelector.OnChanged += Tonemap_OnChanged;
			Core.Environment.PostEffect.TonemapReinhard.Exposure.OnChanged += Tonemap_OnChanged;

			effectFactory = new swig.EffectFactory(effectSetting);

			isViewerShown = true;

			return true;
		}

		private void RenderingMode_OnChanged(object sender, ChangedValueEventArgs e)
		{
			var renderParam = EffectRenderer.GetParameter();
			renderParam.RenderingMethod = (swig.RenderingMethodType)(int)Core.Option.RenderingMode.Value;
			EffectRenderer.SetParameter(renderParam);
		}

		public void HideViewer()
		{
			if (!isViewerShown) return;
			isViewerShown = false;

			Core.Environment.PostEffect.BloomSwitch.OnChanged -= Bloom_OnChanged;
			Core.Environment.PostEffect.Bloom.Intensity.OnChanged -= Bloom_OnChanged;
			Core.Environment.PostEffect.Bloom.Threshold.OnChanged -= Bloom_OnChanged;
			Core.Environment.PostEffect.Bloom.SoftKnee.OnChanged -= Bloom_OnChanged;

			Core.Environment.PostEffect.TonemapSelector.OnChanged -= Tonemap_OnChanged;
			Core.Environment.PostEffect.TonemapReinhard.Exposure.OnChanged -= Tonemap_OnChanged;

			Core.Option.ViewerMode.OnChanged -= ViewMode_OnChanged;
		}

		public void UpdateViewer()
		{
			if (isViewerShown)
			{
				if ((IsChanged && (IsPlaying || IsPaused)) || IsRequiredToReload)
				{
					if (IsRequiredToReload)
					{
						Reload(true);
					}
					else
					{
						Reload(false);
					}

					IsChanged = false;
					IsRequiredToReload = false;
				}

				if (IsPlaying && !IsPaused)
				{
					var stepFrame = Manager.NativeManager.GetDeltaSecond() * 60.0f;

					// regard as 1 frame because of accuracy problem
					if (Math.Abs(1.0f - stepFrame) < 0.05f)
					{
						stepFrame = 1.0f;
					}

					// large step is not better than slow
					stepFrame = Math.Min(stepFrame, 4);

					StepViewer(stepFrame, true);
				}

				// update environment
				var renderParam = EffectRenderer.GetParameter();
				renderParam.IsGroundShown = Core.Environment.Ground.IsShown.Value;
				renderParam.GroundHeight = Core.Environment.Ground.Height.Value;
				renderParam.GroundExtent = Core.Environment.Ground.Extent.Value;

				renderParam.BackgroundColor = new swig.Color
				{
					R = (byte)Core.Environment.Background.BackgroundColor.R,
					G = (byte)Core.Environment.Background.BackgroundColor.G,
					B = (byte)Core.Environment.Background.BackgroundColor.B,
					A = 255,
				};

				renderParam.LightDirection = new swig.Vector3F
				{
					X = Core.Environment.Lighting.LightDirection.X,
					Y = Core.Environment.Lighting.LightDirection.Y,
					Z = Core.Environment.Lighting.LightDirection.Z,
				};

				renderParam.LightColor = new swig.Color
				{
					R = (byte)Core.Environment.Lighting.LightColor.R,
					G = (byte)Core.Environment.Lighting.LightColor.G,
					B = (byte)Core.Environment.Lighting.LightColor.B,
					A = (byte)Core.Environment.Lighting.LightColor.A
				};

				renderParam.LightAmbientColor = new swig.Color
				{
					R = (byte)Core.Environment.Lighting.LightAmbientColor.R,
					G = (byte)Core.Environment.Lighting.LightAmbientColor.G,
					B = (byte)Core.Environment.Lighting.LightAmbientColor.B,
					A = (byte)Core.Environment.Lighting.LightAmbientColor.A
				};

				renderParam.Distortion = (swig.DistortionType)(int)Core.Option.DistortionType.Value;

				renderParam.RenderingMethod = (swig.RenderingMethodType)(int)Core.Option.RenderingMode.Value;

				EffectRenderer.SetParameter(renderParam);

				EffectRenderer.LoadBackgroundImage(Core.Environment.Background.BackgroundImage.AbsolutePath);

				EffectRenderer.GridColor = new swig.Color
				{
					R = (byte)Core.Option.GridColor.R,
					G = (byte)Core.Option.GridColor.G,
					B = (byte)Core.Option.GridColor.B,
					A = (byte)Core.Option.GridColor.A
				};

				EffectRenderer.GridLength = Core.Option.GridLength;
				EffectRenderer.IsGridShown = Core.Option.IsGridShown;
				EffectRenderer.IsGridXYShown = Core.Option.IsXYGridShown;
				EffectRenderer.IsGridXZShown = Core.Option.IsXZGridShown;
				EffectRenderer.IsGridYZShown = Core.Option.IsYZGridShown;

				if (Core.Culling.Type.Value == Data.EffectCullingValues.ParamaterType.Sphere)
				{
					EffectRenderer.IsCullingShown = true;
					EffectRenderer.CullingRadius = Core.Culling.Sphere.Radius.Value;
					EffectRenderer.CullingPosition.X = Core.Culling.Sphere.Location.X;
					EffectRenderer.CullingPosition.Y = Core.Culling.Sphere.Location.Y;
					EffectRenderer.CullingPosition.Z = Core.Culling.Sphere.Location.Z;
				}
				else if (Core.Culling.Type.Value == Data.EffectCullingValues.ParamaterType.None)
				{
					EffectRenderer.IsCullingShown = false;
					EffectRenderer.CullingRadius = 0.0f;
					EffectRenderer.CullingPosition.X = 0.0f;
					EffectRenderer.CullingPosition.Y = 0.0f;
					EffectRenderer.CullingPosition.Z = 0.0f;
				}

				ViewPointController.SetMouseInverseFlag(
					Core.Option.MouseRotInvX,
					Core.Option.MouseRotInvY,
					Core.Option.MouseSlideInvX,
					Core.Option.MouseSlideInvY);
			}
			else
			{
				System.Threading.Thread.Sleep(1);
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
				EffectRenderer.ResetEffect();

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

		public void StepViewer(float frame, bool isLooping)
		{
			var next = Current + frame;

			if (isLooping)
			{
				if (next > Core.EndFrame) next = 0;
			}

			MoveFrame(next);
		}

		public void BackStepViewer()
		{
			MoveFrame(Current - 1);
		}

		unsafe void Export()
		{
			var behavior = EffectRenderer.GetBehavior();
			behavior.DynamicInput1 = Core.Dynamic.Inputs.Values[0].Input.Value;
			behavior.DynamicInput2 = Core.Dynamic.Inputs.Values[1].Input.Value;
			behavior.DynamicInput3 = Core.Dynamic.Inputs.Values[2].Input.Value;
			behavior.DynamicInput4 = Core.Dynamic.Inputs.Values[3].Input.Value;

			behavior.PositionVelocityX = Core.EffectBehavior.LocationVelocity.X;
			behavior.PositionVelocityY = Core.EffectBehavior.LocationVelocity.Y;
			behavior.PositionVelocityZ = Core.EffectBehavior.LocationVelocity.Z;

			behavior.RotationVelocityX = Core.EffectBehavior.RotationVelocity.X / 180.0f * 3.141592f;
			behavior.RotationVelocityY = Core.EffectBehavior.RotationVelocity.Y / 180.0f * 3.141592f;
			behavior.RotationVelocityZ = Core.EffectBehavior.RotationVelocity.Z / 180.0f * 3.141592f;

			behavior.ScaleVelocityX = Core.EffectBehavior.ScaleVelocity.X;
			behavior.ScaleVelocityY = Core.EffectBehavior.ScaleVelocity.Y;
			behavior.ScaleVelocityZ = Core.EffectBehavior.ScaleVelocity.Z;

			if (Core.EffectBehavior.RemovedTime.Infinite.Value)
			{
				behavior.RemovedTime = int.MaxValue;
			}
			else
			{
				behavior.RemovedTime = Core.EffectBehavior.RemovedTime.Value;
			}

			behavior.PositionX = Core.EffectBehavior.Location.X;
			behavior.PositionY = Core.EffectBehavior.Location.Y;
			behavior.PositionZ = Core.EffectBehavior.Location.Z;

			behavior.RotationX = Core.EffectBehavior.Rotation.X / 180.0f * 3.141592f;
			behavior.RotationY = Core.EffectBehavior.Rotation.Y / 180.0f * 3.141592f;
			behavior.RotationZ = Core.EffectBehavior.Rotation.Z / 180.0f * 3.141592f;

			behavior.ScaleX = Core.EffectBehavior.Scale.X;
			behavior.ScaleY = Core.EffectBehavior.Scale.Y;
			behavior.ScaleZ = Core.EffectBehavior.Scale.Z;

			behavior.TargetPositionX = Core.EffectBehavior.TargetLocation.X;
			behavior.TargetPositionY = Core.EffectBehavior.TargetLocation.Y;
			behavior.TargetPositionZ = Core.EffectBehavior.TargetLocation.Z;

			behavior.CountX = Core.EffectBehavior.CountX;
			behavior.CountY = Core.EffectBehavior.CountY;
			behavior.CountZ = Core.EffectBehavior.CountZ;

			behavior.AllColorR = (byte)Core.EffectBehavior.ColorAll.R;
			behavior.AllColorG = (byte)Core.EffectBehavior.ColorAll.G;
			behavior.AllColorB = (byte)Core.EffectBehavior.ColorAll.B;
			behavior.AllColorA = (byte)Core.EffectBehavior.ColorAll.A;

			behavior.PlaybackSpeed = Core.EffectBehavior.PlaybackSpeed.Value;

			behavior.TimeSpan = Core.EffectBehavior.TimeSpan;
			behavior.Distance = Core.EffectBehavior.Distance;

			EffectRenderer.SetBehavior(behavior);

			EffectRenderer.SetStep((int)Core.Option.FPS.Value);

			effectSetting.SetCoordinateSyatem(
				Core.Option.Coordinate.Value == Data.OptionValues.CoordinateType.Right ? swig.CoordinateSystemType.RH : swig.CoordinateSystemType.LH);

			var binaryExporter = new Binary.Exporter();
			var data = binaryExporter.Export(Core.Root, Core.Option.Magnification);
			var path = Utils.Misc.BackSlashToSlash(Core.Root.GetFullPath());
			fixed (byte* p = &data[0])
			{
				var newEffect = effectFactory.LoadEffect(new IntPtr(p), data.Length, path);

				EffectRenderer.SetEffect(newEffect);

				if (CurrentEffect != null)
				{
					CurrentEffect.Dispose();
				}

				CurrentEffect = newEffect;
			}
		}

		unsafe void PlayNew()
		{
			EffectRenderer.ResetEffect();

			if (IsChanged)
			{
				EffectRenderer.SetEffect(null);
				EffectRenderer.ResetEffect();

				Export();

				IsChanged = false;
			}

			random_seed = rand.Next(0, 0xffff);
			EffectRenderer.RandomSeed = random_seed;
			EffectRenderer.PlayEffect();
			IsPlaying = true;

			if (Core.StartFrame > 0)
			{
				StepEffectFrame(Core.StartFrame);
			}
			current = Core.StartFrame;
		}

		/// <summary>
		/// reload an effect including resources
		/// </summary>
		public unsafe void Reload(bool isResourceReloaded)
		{
			if (isResourceReloaded)
			{
				effectFactory.ReloadAllResources();
			}

			EffectRenderer.SetEffect(null);
			EffectRenderer.ResetEffect();

			Export();

			EffectRenderer.RandomSeed = random_seed;
			EffectRenderer.PlayEffect();
			StepEffectFrame((int)Current);
		}

		unsafe void MoveFrame(float new_frame)
		{
			// Same frame
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
					if ((int)current == (int)new_frame)
					{
					}
					else if ((int)current > (int)new_frame)
					{
						EffectRenderer.ResetEffect();
						EffectRenderer.RandomSeed = random_seed;
						EffectRenderer.PlayEffect();
						StepEffectFrame((int)new_frame);
					}
					else
					{
						StepEffectFrame((int)new_frame - (int)current);
					}
				}
				else
				{
					if ((int)current == (int)new_frame)
					{
					}
					else if ((int)current > (int)new_frame)
					{
						EffectRenderer.ResetEffect();
						EffectRenderer.PlayEffect();
						StepEffectFrame((int)new_frame);
					}
					else
					{
						StepEffectFrame((int)new_frame - (int)current);
					}
				}
				current = new_frame;
			}
		}


		private void ViewMode_OnChanged(object sender, ChangedValueEventArgs e)
		{
			var viewerMode = Core.Option.ViewerMode.Value;

			if (viewerMode == Data.OptionValues.ViewMode._3D)
			{
				ViewPointController.SetFocusPosition(new swig.Vector3F(0, 0, 0));
				ViewPointController.SetProjectionType(swig.ProjectionType.Perspective);
				ViewPointController.SetAngleX(30.0f);
				ViewPointController.SetAngleY(-30.0f);
				Core.Option.IsXYGridShown.SetValueDirectly(false);
				Core.Option.IsXZGridShown.SetValueDirectly(true);
				Core.Option.IsYZGridShown.SetValueDirectly(false);
			}
			else if (viewerMode == Data.OptionValues.ViewMode._2D)
			{
				ViewPointController.SetFocusPosition(new swig.Vector3F(0, 0, 0));
				ViewPointController.SetProjectionType(swig.ProjectionType.Orthographic);
				ViewPointController.SetAngleX(0.0f);
				ViewPointController.SetAngleY(0.0f);
				Core.Option.IsXYGridShown.SetValueDirectly(true);
				Core.Option.IsXZGridShown.SetValueDirectly(false);
				Core.Option.IsYZGridShown.SetValueDirectly(false);
			}
		}

		void PostEffectChanged()
		{
			var postEffectParam = new swig.PostEffectParameter();
			postEffectParam.BloomEnabled = Core.Environment.PostEffect.BloomSwitch.Value == Data.EnvironmentPostEffectValues.EffectSwitch.On;
			postEffectParam.BoomIntensity = Core.Environment.PostEffect.Bloom.Intensity.Value;
			postEffectParam.BloomThreshold = Core.Environment.PostEffect.Bloom.Threshold.Value;
			postEffectParam.BloomSoftKnee = Core.Environment.PostEffect.Bloom.SoftKnee.Value;

			postEffectParam.ToneMapAlgorithm = (int)Core.Environment.PostEffect.TonemapSelector.Value;
			postEffectParam.ToneMapExposure = Core.Environment.PostEffect.TonemapReinhard.Exposure.Value;

			EffectRenderer.SetPostEffectParameter(postEffectParam);
		}

		private void Bloom_OnChanged(object sender, ChangedValueEventArgs e)
		{
			PostEffectChanged();
		}

		private void Tonemap_OnChanged(object sender, ChangedValueEventArgs e)
		{
			PostEffectChanged();
		}
	}
}
