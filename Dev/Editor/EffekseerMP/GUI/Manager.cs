using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI
{
	class GUIManagerCallback : swig.GUIManagerCallback
	{
		public override void Resized(int x, int y)
		{
			Manager.Native.ResizeWindow(x, y);
		}

		public override void Focused()
		{
			Core.Reload();
		}

		public override void Droped()
		{
			var path = GetPath();
			var handle = false;

			Manager.Controls.Lock();

			foreach (var c in Manager.Controls.Internal.OfType<Control>())
			{
				c.DispatchDropped(path, ref handle);
			}

			Manager.Controls.Unlock();
		
			if(!handle)
			{
				Commands.Open();
			}
		}
	}

	class Manager
	{


		internal static swig.GUIManager NativeManager;
		internal static swig.Native Native;

		static GUIManagerCallback guiManagerCallback;

		static int nextID = 10;

		public static Viewer Viewer;

		public static Network Network;

		static Type[] dockTypes =
		{
			typeof(Dock.NodeTreeView),
			typeof(Dock.ViewerController),
			typeof(Dock.CommonValues),
			typeof(Dock.LocationValues),
			typeof(Dock.LocationAbsValues),
			typeof(Dock.GenerationLocationValues),
			typeof(Dock.RotationValues),
			typeof(Dock.ScaleValues),
			typeof(Dock.DepthValues),
			typeof(Dock.RendererCommonValues),
			typeof(Dock.RendererValues),
			typeof(Dock.SoundValues),
			typeof(Dock.FCurves),
			typeof(Dock.ViewPoint),
			typeof(Dock.Recorder),

			typeof(Dock.Option),

			typeof(Dock.Network),
			typeof(Dock.FileViewer),
		};

		static Dock.DockManager dockManager = null;

		static Dock.EffectViwer effectViewer = null;

		static Dock.DockPanel[] panels = new Dock.DockPanel[0];

		public static bool IsDockMode() { return dockManager != null; }

		internal static Utils.DelayedList<IRemovableControl> Controls = new Utils.DelayedList<IRemovableControl>();

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

			Native = new swig.Native();

			Viewer = new Viewer(Native);
			if (!Viewer.ShowViewer(mgr.GetNativeHandle(), 960, 540, true))
			{
				mgr.Dispose();
				mgr = null;
				return false;
			}

			NativeManager = mgr;

			guiManagerCallback = new GUIManagerCallback();
			NativeManager.SetCallback(guiManagerCallback);

			panels = new Dock.DockPanel[dockTypes.Length];

			// Load font
			NativeManager.AddFontFromFileTTF("resources/GenShinGothic-Monospace-Normal.ttf", 16);

			// Load config
			RecentFiles.LoadRecentConfig();
			Shortcuts.LoadShortcuts();
			Commands.Register();

			// Add controls
			var mainMenu = new GUI.Menu.MainMenu();
			GUI.Manager.AddControl(mainMenu);

			/*
			dockManager = new GUI.Dock.DockManager();
			GUI.Manager.AddControl(dockManager);

			effectViewer = new Dock.EffectViwer();
			if(dockManager != null)
			{
				dockManager.Controls.Add(effectViewer);
			}
			else
			{
				AddControl(effectViewer);
			}
			*/

			Network = new Network(Native);

			SelectOrShowWindow(typeof(Dock.NodeTreeView));
			SelectOrShowWindow(typeof(Dock.ViewerController));
			
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
			foreach(var p in panels)
			{
				if(p != null)
				{
					p.DispatchDisposed();
				}
			}

			if(effectViewer != null)
			{
				effectViewer.DispatchDisposed();
			}

			Shortcuts.SeveShortcuts();
			RecentFiles.SaveRecentConfig();

			Viewer.HideViewer();

			NativeManager.SetCallback(null);
			NativeManager.Terminate();
		}

		public static void AddControl(IRemovableControl control)
		{
			Controls.Add(control);
		}

		static swig.Vec2 mousePos_pre;

		static bool isFirstUpdate = true;

		public static void Update()
		{
			Shortcuts.Update();

			var handle = false;
			if(!handle)
			{
				Shortcuts.ProcessCmdKey(ref handle);
			}

			var mousePos = NativeManager.GetMousePosition();

			if (isFirstUpdate)
			{
				mousePos_pre = mousePos;
			}

			if((effectViewer == null && !NativeManager.IsAnyWindowHovered()) || (effectViewer != null && effectViewer.IsHovered))
			{
				if (NativeManager.GetMouseButton(2) > 0)
				{
					var dx = mousePos.X - mousePos_pre.X;
					var dy = mousePos.Y - mousePos_pre.Y;

					Viewer.Slide(dx / 30.0f, dy / 30.0f);
				}

				if (NativeManager.GetMouseButton(1) > 0)
				{
					var dx = mousePos.X - mousePos_pre.X;
					var dy = mousePos.Y - mousePos_pre.Y;

					Viewer.Rotate(dx, dy);
				}

				if (NativeManager.GetMouseWheel() != 0)
				{
					Viewer.Zoom(NativeManager.GetMouseWheel());
				}
			}

			mousePos_pre = mousePos;

			Viewer.UpdateViewer();

			Native.UpdateWindow();

			Native.ClearWindow(50, 50, 50, 0);

			if(effectViewer == null)
			{
				Native.RenderWindow();
			}

			NativeManager.ResetGUI();

			Controls.Lock();

			foreach (var c in Controls.Internal)
			{
				c.Update();
			}

			foreach(var _ in Controls.Internal)
			{
				if (!_.ShouldBeRemoved) continue;

				var dp = _ as Dock.DockPanel;
				if(dp != null)
				{
					dp.DispatchDisposed();
				}
			}

			foreach (var _ in Controls.Internal)
			{
				if (!_.ShouldBeRemoved) continue;
				Controls.Remove(_);
			}

			Controls.Unlock();

			for (int i = 0; i < dockTypes.Length; i++)
			{
				if (panels[i] != null && panels[i].ShouldBeRemoved)
				{
					panels[i] = null;
				}
			}

			NativeManager.RenderGUI();

			Native.Present();
			NativeManager.Present();

			isFirstUpdate = false;
		}

		public static void SelectOrShowWindow(Type t)
		{
			for(int i = 0; i < dockTypes.Length; i++)
			{
				if (dockTypes[i] != t) continue;

				if(panels[i] != null)
				{

				}
				else
				{
					panels[i] = (Dock.DockPanel)t.GetConstructor(Type.EmptyTypes).Invoke(null);

					if(dockManager != null)
					{
						dockManager.Controls.Add(panels[i]);
					}
					else
					{
						AddControl(panels[i]);
					}
				}
			}
		}

		static void Core_OnAfterLoad(object sender, EventArgs e)
		{
			Viewer.StopViewer();

			if (Network.SendOnLoad)
			{
				Network.Send();
			}
		}

		static void Core_OnAfterNew(object sender, EventArgs e)
		{
			Viewer.StopViewer();
		}

		static void Core_OnReload(object sender, EventArgs e)
		{
			Viewer.Reload(true);
		}

		static void OnChanged(object sender, EventArgs e)
		{
			Viewer.IsChanged = true;
			
			if (Network.SendOnEdit)
			{
				Network.Send();
			}
		}

		/// <summary>
		/// Get unique id in this aplication.
		/// </summary>
		/// <returns></returns>
		public static int GetUniqueID()
		{
			nextID++;
			return nextID;
		}

		/// <summary>
		/// Get a directory where this application is located.
		/// </summary>
		/// <returns></returns>
		public static string GetEntryDirectory()
		{
			var myAssembly = System.Reflection.Assembly.GetEntryAssembly();
			string path = myAssembly.Location;

			return System.IO.Path.GetDirectoryName(path);
		}
	}
}
