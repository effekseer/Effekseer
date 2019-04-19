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
			Core.Option.RenderingMode.OnChanged += OnChanged;

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

		Viewer viewer = null;
		System.Drawing.Point mouse_left_location;
		System.Drawing.Point mouse_middle_location;
		System.Drawing.Point mouse_right_location;

		/// <summary>
		/// ビュワーの取得を行う。
		/// </summary>
		public Viewer Viewer
		{
			get
			{
				return viewer;
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
			if (viewer == null)
			{
				throw new Exception("Viewerが生成されていません。");
			}

			viewer.ShowViewer(Handle, Width, Height, swig.DeviceType.DirectX9);
		}

		public void UpdateViewer()
		{
			viewer.UpdateViewer();
			viewer.UpdateWindow();
			viewer.PresentWindow();
		}

		public void PlayViewer()
		{
			viewer.PlayViewer();
		}

		public void StopViewer()
		{
			viewer.StopViewer();
		}

		public void PauseAndResumeViewer()
		{
			viewer.PauseAndResumeViewer();
		}

		public void StepViewer(bool isLooping)
		{
			viewer.StepViewer(isLooping);
		}

		unsafe void DockViewer_Load(object sender, EventArgs e)
		{
			try
			{
				viewer = new Viewer(new swig.Native());
			}
			catch(Exception exception)
			{
				MessageBox.Show("描画画面の生成に失敗しました。DirectXのバージョンの問題、メモリの不足等が考えられます。");
				throw exception;
			}
		}

		void DockViewer_SizeChanged(object sender, EventArgs e)
		{
			if (Width > 0 && Height > 0 && viewer != null)
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
			viewer.HideViewer();
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

			viewer.Zoom((float)e.Delta / 120.0f);
		}

		void Core_OnAfterLoad(object sender, EventArgs e)
		{
			viewer.StopViewer();

			if (GUIManager.Network.SendOnLoad)
			{
				GUIManager.Network.Send();
			}
		}

		void Core_OnAfterNew(object sender, EventArgs e)
		{
			viewer.StopViewer();
		}
		
		void Core_OnReload(object sender, EventArgs e)
		{
			viewer.Reload(true);
		}

		void OnChanged(object sender, EventArgs e)
		{
			viewer.IsChanged = true;
			
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
