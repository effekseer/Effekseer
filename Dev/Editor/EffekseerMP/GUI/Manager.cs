using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI
{
    class Manager
    {
        internal static swig.GUIManager NativeManager;

		internal static Viewer Viewer { get { return viewer; } }

		static Viewer viewer;

		static bool is_shown = false;
		static int current = 0;
		static int random_seed = 0;
		static Random rand = new Random();

		internal static List<IRemovableControl> Controls = new List<IRemovableControl>();

		static List<IRemovableControl> addingControls = new List<IRemovableControl>();


		/// <summary>
		/// 現在再生中か?
		/// </summary>
		public static bool IsPlaying
		{
			get;
			private set;
		}

		/// <summary>
		/// 現在一時停止中か?
		/// </summary>
		public static bool IsPaused
		{
			get;
			private set;
		}

		/// <summary>
		/// パラメーターに変更点があるかどうか?
		/// </summary>
		public static bool IsChanged
		{
			get;
			set;
		}

		/// <summary>
		/// 現在のフレーム
		/// </summary>
		public static int Current
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

		public static bool Initialize(int width, int height)
		{
			var mgr = new swig.GUIManager();
			if (mgr.Initialize("Effekseer", 960, 540, false))
			{
			}
			else
			{
				mgr.Dispose();
				mgr = null;
				return false;
			}

			viewer = new Viewer();

			if (viewer.CreateWindow(mgr.GetNativeHandle(), width <= 0 ? 1 : width, height <= 0 ? 1 : height, Core.Option.ColorSpace.Value == Data.OptionValues.ColorSpaceType.LinearSpace))
			{
				is_shown = true;
			}
			else
			{
				mgr.Terminate();
				mgr.Dispose();
				mgr = null;
				return false;
			}

			IsPlaying = false;
			IsPaused = false;
			IsChanged = true;
			current = 0;

			Command.CommandManager.Changed += OnChanged;

			Core.EffectBehavior.Location.X.OnChanged += OnChanged;
			Core.EffectBehavior.Location.Y.OnChanged += OnChanged;
			Core.EffectBehavior.Location.Z.OnChanged += OnChanged;
			Core.EffectBehavior.Rotation.X.OnChanged += OnChanged;
			Core.EffectBehavior.Rotation.Y.OnChanged += OnChanged;
			Core.EffectBehavior.Rotation.Z.OnChanged += OnChanged;
			Core.EffectBehavior.Scale.X.OnChanged += OnChanged;
			Core.EffectBehavior.Scale.Y.OnChanged += OnChanged;
			Core.EffectBehavior.Scale.Z.OnChanged += OnChanged;

			Core.EffectBehavior.LocationVelocity.X.OnChanged += OnChanged;
			Core.EffectBehavior.LocationVelocity.Y.OnChanged += OnChanged;
			Core.EffectBehavior.LocationVelocity.Z.OnChanged += OnChanged;
			Core.EffectBehavior.RotationVelocity.X.OnChanged += OnChanged;
			Core.EffectBehavior.RotationVelocity.Y.OnChanged += OnChanged;
			Core.EffectBehavior.RotationVelocity.Z.OnChanged += OnChanged;
			Core.EffectBehavior.ScaleVelocity.X.OnChanged += OnChanged;
			Core.EffectBehavior.ScaleVelocity.Y.OnChanged += OnChanged;
			Core.EffectBehavior.ScaleVelocity.Z.OnChanged += OnChanged;
			Core.EffectBehavior.RemovedTime.Infinite.OnChanged += OnChanged;
			Core.EffectBehavior.RemovedTime.Value.OnChanged += OnChanged;

			Core.EffectBehavior.TargetLocation.X.OnChanged += OnChanged;
			Core.EffectBehavior.TargetLocation.Y.OnChanged += OnChanged;
			Core.EffectBehavior.TargetLocation.Z.OnChanged += OnChanged;

			Core.EffectBehavior.CountX.OnChanged += OnChanged;
			Core.EffectBehavior.CountY.OnChanged += OnChanged;
			Core.EffectBehavior.CountZ.OnChanged += OnChanged;

			Core.EffectBehavior.Distance.OnChanged += OnChanged;

			Core.EffectBehavior.TimeSpan.OnChanged += OnChanged;
			Core.EffectBehavior.ColorAll.R.OnChanged += OnChanged;
			Core.EffectBehavior.ColorAll.G.OnChanged += OnChanged;
			Core.EffectBehavior.ColorAll.B.OnChanged += OnChanged;
			Core.EffectBehavior.ColorAll.A.OnChanged += OnChanged;

			Core.Option.Magnification.OnChanged += OnChanged;
			Core.Option.IsGridShown.OnChanged += OnChanged;
			Core.Option.GridLength.OnChanged += OnChanged;
			Core.Option.BackgroundColor.R.OnChanged += OnChanged;
			Core.Option.BackgroundColor.G.OnChanged += OnChanged;
			Core.Option.BackgroundColor.B.OnChanged += OnChanged;
			Core.Option.BackgroundColor.A.OnChanged += OnChanged;
			Core.Option.GridColor.R.OnChanged += OnChanged;
			Core.Option.GridColor.G.OnChanged += OnChanged;
			Core.Option.GridColor.B.OnChanged += OnChanged;
			Core.Option.GridColor.A.OnChanged += OnChanged;
			Core.Option.FPS.OnChanged += OnChanged;

			Core.Option.DistortionType.OnChanged += OnChanged;
			Core.Option.Coordinate.OnChanged += OnChanged;

			Core.Option.BackgroundImage.OnChanged += OnChanged;

			Core.Culling.IsShown.OnChanged += OnChanged;
			Core.Culling.Type.OnChanged += OnChanged;
			Core.Culling.Sphere.Location.X.OnChanged += OnChanged;
			Core.Culling.Sphere.Location.Y.OnChanged += OnChanged;
			Core.Culling.Sphere.Location.Z.OnChanged += OnChanged;
			Core.Culling.Sphere.Radius.OnChanged += OnChanged;

			Core.OnAfterLoad += new EventHandler(Core_OnAfterLoad);
			Core.OnAfterNew += new EventHandler(Core_OnAfterNew);
			Core.OnReload += new EventHandler(Core_OnReload);

			return true;
		}

		public static void Terminate()
		{
			if (!is_shown) return;
			is_shown = false;

			viewer.DestroyWindow();
		}

		public static void AddControl(IRemovableControl control)
		{
			addingControls.Add(control);
		}

        public static void Update()
        {
			foreach (var c in Controls)
			{
				c.Update();
			}

			foreach (var c in addingControls)
			{
				Controls.Add(c);
			}

			addingControls.Clear();

			Controls.RemoveAll(_ => _.ShouldBeRemoved);
		}

		public static void UpdateViewer()
		{
			if (is_shown)
			{
				if (IsChanged && (IsPlaying || IsPaused))
				{
					Reload();
					IsChanged = false;
				}

				if (IsPlaying && !IsPaused)
				{
					StepViewer(true);
				}

				viewer.SetBackgroundColor(
				(byte)Core.Option.BackgroundColor.R,
				(byte)Core.Option.BackgroundColor.G,
				(byte)Core.Option.BackgroundColor.B);

				viewer.SetBackgroundImage(Core.Option.BackgroundImage.AbsolutePath);

				viewer.SetGridColor(
				(byte)Core.Option.GridColor.R,
				(byte)Core.Option.GridColor.G,
				(byte)Core.Option.GridColor.B,
				(byte)Core.Option.GridColor.A);

				viewer.SetGridLength(
					Core.Option.GridLength);

				viewer.SetIsGridShown(
					Core.Option.IsGridShown,
					Core.Option.IsXYGridShown,
					Core.Option.IsXZGridShown,
					Core.Option.IsYZGridShown);

				viewer.SetLightDirection(
					Core.Option.LightDirection.X,
					Core.Option.LightDirection.Y,
					Core.Option.LightDirection.Z);

				viewer.SetLightColor(
					(byte)Core.Option.LightColor.R,
					(byte)Core.Option.LightColor.G,
					(byte)Core.Option.LightColor.B,
					(byte)Core.Option.LightColor.A);

				viewer.SetLightAmbientColor(
					(byte)Core.Option.LightAmbientColor.R,
					(byte)Core.Option.LightAmbientColor.G,
					(byte)Core.Option.LightAmbientColor.B,
					(byte)Core.Option.LightAmbientColor.A);

				viewer.SetMouseInverseFlag(
					Core.Option.MouseRotInvX,
					Core.Option.MouseRotInvY,
					Core.Option.MouseSlideInvX,
					Core.Option.MouseSlideInvY);

				if (Core.Culling.Type.Value == Data.EffectCullingValues.ParamaterType.Sphere)
				{
					viewer.SetCullingParameter(Core.Culling.IsShown, Core.Culling.Sphere.Radius.Value, Core.Culling.Sphere.Location.X, Core.Culling.Sphere.Location.Y, Core.Culling.Sphere.Location.Z);
				}
				else if (Core.Culling.Type.Value == Data.EffectCullingValues.ParamaterType.None)
				{
					viewer.SetCullingParameter(false, 0.0f, 0.0f, 0.0f, 0.0f);
				}

				viewer.UpdateWindow();
			}
			else
			{
				System.Threading.Thread.Sleep(1);
			}
		}

		public static void PlayViewer()
		{
			if (is_shown)
			{
				PlayNew();
				IsPaused = false;
			}
		}

		public static void StopViewer()
		{
			if (is_shown)
			{
				viewer.StopEffect();

				IsPlaying = false;
				IsPaused = false;
				current = Core.StartFrame;
			}
		}

		public static void PauseAndResumeViewer()
		{
			if (is_shown)
			{
				IsPaused = !IsPaused;
			}
		}

		public static void StepViewer(bool isLooping)
		{
			int next = Current + 1;

			if (isLooping)
			{
				if (next > Core.EndFrame) next = 0;
			}

			Step(next);
		}

		public static void BackStepViewer()
		{
			Step(Current - 1);
		}

		unsafe static void Export()
		{
			viewer.SetLotationVelocity(
				Core.EffectBehavior.LocationVelocity.X,
				Core.EffectBehavior.LocationVelocity.Y,
				Core.EffectBehavior.LocationVelocity.Z);

			viewer.SetRotationVelocity(
				Core.EffectBehavior.RotationVelocity.X / 180.0f * 3.141592f,
				Core.EffectBehavior.RotationVelocity.Y / 180.0f * 3.141592f,
				Core.EffectBehavior.RotationVelocity.Z / 180.0f * 3.141592f);

			viewer.SetScaleVelocity(
				Core.EffectBehavior.ScaleVelocity.X,
				Core.EffectBehavior.ScaleVelocity.Y,
				Core.EffectBehavior.ScaleVelocity.Z);

			if (Core.EffectBehavior.RemovedTime.Infinite.Value)
			{
				viewer.SetRemovedTime(int.MaxValue);
			}
			else
			{
				viewer.SetRemovedTime(Core.EffectBehavior.RemovedTime.Value);
			}

			viewer.SetLotation(
				Core.EffectBehavior.Location.X,
				Core.EffectBehavior.Location.Y,
				Core.EffectBehavior.Location.Z);

			viewer.SetRotation(
				Core.EffectBehavior.Rotation.X / 180.0f * 3.141592f,
				Core.EffectBehavior.Rotation.Y / 180.0f * 3.141592f,
				Core.EffectBehavior.Rotation.Z / 180.0f * 3.141592f);

			viewer.SetScale(
				Core.EffectBehavior.Scale.X,
				Core.EffectBehavior.Scale.Y,
				Core.EffectBehavior.Scale.Z);

			viewer.SetTargetLocation(
				Core.EffectBehavior.TargetLocation.X,
				Core.EffectBehavior.TargetLocation.Y,
				Core.EffectBehavior.TargetLocation.Z);

			viewer.SetEffectCount(
				Core.EffectBehavior.CountX,
				Core.EffectBehavior.CountY,
				Core.EffectBehavior.CountZ);

			viewer.SetAllColor(
				(byte)Core.EffectBehavior.ColorAll.R,
				(byte)Core.EffectBehavior.ColorAll.G,
				(byte)Core.EffectBehavior.ColorAll.B,
				(byte)Core.EffectBehavior.ColorAll.A);

			viewer.SetEffectTimeSpan(Core.EffectBehavior.TimeSpan);

			viewer.SetEffectDistance(Core.EffectBehavior.Distance);

			viewer.SetBackgroundColor(
				(byte)Core.Option.BackgroundColor.R,
				(byte)Core.Option.BackgroundColor.G,
				(byte)Core.Option.BackgroundColor.B);

			viewer.SetGridLength(
				Core.Option.GridLength);

			viewer.SetStep((int)Core.Option.FPS.Value);
			viewer.SetIsRightHand(Core.Option.Coordinate.Value == Data.OptionValues.CoordinateType.Right);

			viewer.SetDistortionType((int)Core.Option.DistortionType.Value);

			if (Core.Culling.Type.Value == Data.EffectCullingValues.ParamaterType.Sphere)
			{
				viewer.SetCullingParameter(Core.Culling.IsShown, Core.Culling.Sphere.Radius.Value, Core.Culling.Sphere.Location.X, Core.Culling.Sphere.Location.Y, Core.Culling.Sphere.Location.Z);
			}
			else if (Core.Culling.Type.Value == Data.EffectCullingValues.ParamaterType.None)
			{
				viewer.SetCullingParameter(false, 0.0f, 0.0f, 0.0f, 0.0f);
			}

			var data = Binary.Exporter.Export(Core.Option.Magnification);
			fixed (byte* p = &data[0])
			{
				viewer.LoadEffect(new IntPtr(p), data.Length, Core.FullPath);
			}
		}

		/// <summary>
		/// 新規再生
		/// </summary>
		unsafe static void PlayNew()
		{
			viewer.StopEffect();

			if (IsChanged)
			{
				viewer.RemoveEffect();

				Export();

				IsChanged = false;
			}

			random_seed = rand.Next(0, 0xffff);
			viewer.SetRandomSeed(random_seed);
			viewer.PlayEffect();
			IsPlaying = true;

			if (Core.StartFrame > 0)
			{
				viewer.StepEffect(Core.StartFrame);
			}
			current = Core.StartFrame;
		}

		/// <summary>
		/// 状態はそのまま、エフェクトのデータだけ差し替え
		/// </summary>
		unsafe static void Reload()
		{
			viewer.StopEffect();

			viewer.RemoveEffect();

			Export();

			viewer.SetRandomSeed(random_seed);
			viewer.PlayEffect();
			viewer.StepEffect(Current);
		}

		/// <summary>
		/// 指定フレームに移動
		/// </summary>
		/// <param name="new_frame"></param>
		unsafe static void Step(int new_frame)
		{
			// 同一フレーム
			if (current == new_frame) return;

			if (new_frame < Core.StartFrame) new_frame = Core.StartFrame;
			if (new_frame > Core.EndFrame) new_frame = Core.EndFrame;

			if (is_shown)
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
						viewer.StopEffect();
						viewer.SetRandomSeed(random_seed);
						viewer.PlayEffect();
						viewer.StepEffect(new_frame);
					}
					else
					{
						viewer.StepEffect(new_frame - current);
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
						viewer.StopEffect();
						viewer.PlayEffect();
						viewer.StepEffect(new_frame);
					}
					else
					{
						viewer.StepEffect(new_frame - current);
					}
					current = new_frame;
				}

			}
		}

		static void Core_OnAfterLoad(object sender, EventArgs e)
		{
			StopViewer();

			Console.WriteLine("Not implemented");
			//if (GUIManager.Network.SendOnLoad)
			//{
			//	GUIManager.Network.Send();
			//}
		}

		static void Core_OnAfterNew(object sender, EventArgs e)
		{
			StopViewer();
		}

		static void Core_OnReload(object sender, EventArgs e)
		{
			viewer.InvalidateTextureCache();
			Reload();
		}

		static void OnChanged(object sender, EventArgs e)
		{
			IsChanged = true;

			Console.WriteLine("Not implemented");

			//if (GUIManager.Network.SendOnEdit)
			//{
			//	GUIManager.Network.Send();
			//}
		}
	}
}
