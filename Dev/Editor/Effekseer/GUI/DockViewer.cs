using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Effekseer.GUI
{
	public partial class DockViewer : DockContent
	{
		public DockViewer()
		{
			InitializeComponent();

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

			Core.EffectBehavior.CountX.OnChanged += OnChanged;
			Core.EffectBehavior.CountY.OnChanged += OnChanged;
			Core.EffectBehavior.CountZ.OnChanged += OnChanged;

			Core.EffectBehavior.Distance.OnChanged += OnChanged;

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
			Core.OnReload +=  new EventHandler(Core_OnReload);
			HandleCreated += new EventHandler(DockViewer_HandleCreated);
			HandleDestroyed += new EventHandler(DockViewer_HandleDestroyed);
			MouseDown += new MouseEventHandler(DockViewer_MouseDown);
			MouseUp += new MouseEventHandler(DockViewer_MouseUp);
			MouseMove += new MouseEventHandler(DockViewer_MouseMove);
			MouseWheel += new MouseEventHandler(DockViewer_MouseWheel);
			SizeChanged += new EventHandler(DockViewer_SizeChanged);
			Load += new EventHandler(DockViewer_Load);
		}

		IViewer viewer = null;
		System.Drawing.Point mouse_left_location;
		System.Drawing.Point mouse_middle_location;
		System.Drawing.Point mouse_right_location;
		bool is_shown = false;
		int current = 0;
		int random_seed = 0;
		Random rand = new Random();

		/// <summary>
		/// ビュワーの取得を行う。
		/// </summary>
		public IViewer Viewer
		{
			get
			{
				return viewer;
			}
		}

		/// <summary>
		/// dynamic形式としてビュワーの取得を行う。
		/// </summary>
		public dynamic ViewerAsDynamic
		{
			get
			{
				return viewer;
			}
		}

		/// <summary>
		/// 現在再生中か?
		/// </summary>
		public bool IsPlaying
		{
			get;
			private set;
		}

		/// <summary>
		/// 現在一時停止中か?
		/// </summary>
		public bool IsPaused
		{
			get;
			private set;
		}

		/// <summary>
		/// パラメーターに変更点があるかどうか?
		/// </summary>
		public bool IsChanged
		{
			get;
			set;
		}

		/// <summary>
		/// 現在のフレーム
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

		public bool IsMouseDownLeft
		{
			get;
			private set;
		}

		public bool IsMouseDownMiddle
		{
			get;
			private set;
		}

		public bool IsMouseDownRight
		{
			get;
			private set;
		}

		public System.Drawing.Point MouseLeftLocation
		{
			get
			{
				return mouse_left_location;
			}
			private set
			{
				mouse_left_location = value;
			}
		}

		public System.Drawing.Point MouseMiddleLocation
		{
			get
			{
				return mouse_middle_location;
			}
			private set
			{
				mouse_middle_location = value;
			}
		}

		public System.Drawing.Point MouseRightLocation
		{
			get
			{
				return mouse_right_location;
			}
			private set
			{
				mouse_right_location = value;
			}
		}

		public void ShowViewer()
		{
			if (is_shown) return;

			if (viewer == null)
			{
				throw new Exception("Viewerが生成されていません。");
			}

			// WidthかHeightが0以下だとウィンドウの作成に失敗するので、その場合はとりあえずサイズを1にして回避
			if (viewer.CreateWindow(Handle, Width <= 0 ? 1 : Width, Height <= 0 ? 1 : Height))
			{
				is_shown = true;
			}
			else
			{
				MessageBox.Show("描画画面の生成に失敗しました。DirectXのバージョンの問題、メモリの不足等が考えられます。");
			}
		}

		public void HideViewer()
		{
			if (!is_shown) return;
			is_shown = false;

			viewer.DestroyWindow();
		}

		public void UpdateViewer()
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
                    StepViewer();
				}

				viewer.SetBackgroundColor(
				(byte)Core.Option.BackgroundColor.R,
				(byte)Core.Option.BackgroundColor.G,
				(byte)Core.Option.BackgroundColor.B);

				viewer.SetBackgroundImage(Core.Option.BackgroundImage.RelativePath);

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

		public void PlayViewer()
		{
			if (is_shown)
			{
				PlayNew();
                IsPaused = false;
			}
		}

		public void StopViewer()
		{
			if (is_shown)
			{
				viewer.StopEffect();
				
				IsPlaying = false;
				IsPaused = false;
				current = Core.StartFrame;
			}
		}

		public void PauseAndResumeViewer()
		{
			if (is_shown)
			{
                IsPaused = !IsPaused;
			}
		}

		public void StepViewer()
		{
            if (!IsPlaying)
            {
                return;
            }

            Step(Current + 1);

            if (Core.EndFrame < current)
            {
                if (Core.IsLoop)
                {
                    PlayNew();
                }
                else
                {
                    StopViewer();
                }
            }
		}

		unsafe void Export()
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

			viewer.SetEffectDistance(Core.EffectBehavior.Distance);

			viewer.SetBackgroundColor(
				(byte)Core.Option.BackgroundColor.R,
				(byte)Core.Option.BackgroundColor.G,
				(byte)Core.Option.BackgroundColor.B);

			viewer.SetGridLength(
				Core.Option.GridLength);

			viewer.SetStep((int)Core.Option.FPS.Value);
			viewer.SetIsRightHand(Core.Option.Coordinate.Value == Data.OptionValues.CoordinateType.Right);

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
		unsafe void PlayNew()
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
		unsafe void Reload()
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
		unsafe void Step(int new_frame )
		{
			// 同一フレーム
			if (current == new_frame) return;

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

		unsafe void DockViewer_Load(object sender, EventArgs e)
		{
			try
			{
				viewer = new Viewer();
				viewer.LoadEffectFunc += () =>
					{
						StopViewer();

						viewer.RemoveEffect();

						Export();

						IsChanged = false;

						return true;
					};

				IsPlaying = false;
				IsPaused = false;
				IsChanged = true;
				current = 0;

				Core.Viewer = viewer;
			}
			catch(Exception exception)
			{
				MessageBox.Show("描画画面の生成に失敗しました。DirectXのバージョンの問題、メモリの不足等が考えられます。");
				throw exception;
			}
		}

		void DockViewer_SizeChanged(object sender, EventArgs e)
		{
			if (is_shown && Width > 0 && Height > 0)
			{
				if (!viewer.ResizeWindow(Width, Height))
				{
					MessageBox.Show("画面サイズの変更に失敗しました。");
				}
			}
		}

		void DockViewer_HandleCreated(object sender, EventArgs e)
		{

		}

		void DockViewer_HandleDestroyed(object sender, EventArgs e)
		{
			HideViewer();
			Core.Viewer = null;
		}

		void DockViewer_MouseDown(object sender, MouseEventArgs e)
		{
			if (e.Button == System.Windows.Forms.MouseButtons.Left)
			{
				IsMouseDownLeft = true;
				mouse_left_location.X = e.X;
				mouse_left_location.Y = e.Y;
			}

			if (e.Button == System.Windows.Forms.MouseButtons.Middle)
			{
				IsMouseDownMiddle = true;
				mouse_middle_location.X = e.X;
				mouse_middle_location.Y = e.Y;
			}

			if (e.Button == System.Windows.Forms.MouseButtons.Right)
			{
				IsMouseDownRight = true;
				mouse_right_location.X = e.X;
				mouse_right_location.Y = e.Y;
			}
		}

		void DockViewer_MouseUp(object sender, MouseEventArgs e)
		{
			if (e.Button == System.Windows.Forms.MouseButtons.Left)
			{
				IsMouseDownLeft = false;
				mouse_left_location.X = e.X;
				mouse_left_location.Y = e.Y;
			}

			if (e.Button == System.Windows.Forms.MouseButtons.Middle)
			{
				IsMouseDownMiddle = false;
				mouse_middle_location.X = e.X;
				mouse_middle_location.Y = e.Y;
			}

			if (e.Button == System.Windows.Forms.MouseButtons.Right)
			{
				IsMouseDownRight = false;
				mouse_right_location.X = e.X;
				mouse_right_location.Y = e.Y;
			}
		}

		void DockViewer_MouseMove(object sender, MouseEventArgs e)
		{
			if (e.Button == System.Windows.Forms.MouseButtons.Left)
			{

				mouse_left_location.X = e.X;
				mouse_left_location.Y = e.Y;
			}

			if (e.Button == System.Windows.Forms.MouseButtons.Middle)
			{
				// 並行移動
				viewer.Slide(
					(float)(e.X - mouse_middle_location.X) / 30.0f,
					(float)(e.Y - mouse_middle_location.Y) / 30.0f);

				mouse_middle_location.X = e.X;
				mouse_middle_location.Y = e.Y;
			}

			if (e.Button == System.Windows.Forms.MouseButtons.Right)
			{
				// 回転移動
				viewer.Rotate(
					(float)(e.X - mouse_right_location.X),
					(float)(e.Y - mouse_right_location.Y));

				mouse_right_location.X = e.X;
				mouse_right_location.Y = e.Y;
			}
		}

		void DockViewer_MouseWheel(object sender, MouseEventArgs e)
		{
			if (e.X >= this.Width) return;
			if (e.X <= 0) return;

			if (e.Y >= this.Height) return;
			if (e.Y <= 0) return;

			if (!is_shown) return;
			viewer.Zoom((float)e.Delta / 120.0f);
		}

		void Core_OnAfterLoad(object sender, EventArgs e)
		{
			StopViewer();
			
			if (GUIManager.Network.SendOnLoad)
			{
				GUIManager.Network.Send();
			}
		}

		void Core_OnAfterNew(object sender, EventArgs e)
		{
			StopViewer();
		}
		
		void Core_OnReload(object sender, EventArgs e)
		{
			viewer.InvalidateTextureCache();
			Reload();
		}

		void OnChanged(object sender, EventArgs e)
		{
			IsChanged = true;

			if (GUIManager.Network.SendOnEdit)
			{
				GUIManager.Network.Send();
			}
		}

		private void DockViewer_DragEnter(object sender, DragEventArgs e)
		{
			if (e.Data.GetDataPresent(DataFormats.FileDrop))
			{
				e.Effect = DragDropEffects.Copy;
			}
			else
			{
				e.Effect = DragDropEffects.None;
			}
		}

		private void DockViewer_DragDrop(object sender, DragEventArgs e)
		{
			string[] fileNames = e.Data.GetData(DataFormats.FileDrop, false) as string[];
			if (fileNames.Length > 0)
			{
				Commands.Open(fileNames[0]);
			}
		}
	}
}
