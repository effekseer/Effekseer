using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Effekseer.Utl;

namespace Effekseer.GUI
{
	class GUIManagerCallback : swig.GUIManagerCallback
	{
		void HandleAction()
		{
		}


		public override void Resized(int x, int y)
		{			
			if(x > 0 && y > 0)
			{
				Manager.Native.ResizeWindow(x, y);

				Manager.WindowSize.X = x;
				Manager.WindowSize.Y = y;
			}

			Manager.resizedCount = 5;
			Manager.actualWidth = x;
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
				Commands.Open(this.GetPath());
			}
		}

		public override bool Closing()
		{
			if (Manager.IgnoreDisposingDialogBox) return true;
			if (!Core.IsChanged) return true;

			var dialog = new Dialog.SaveOnDisposing(
				() =>
			{
				Manager.IgnoreDisposingDialogBox = true;
				Manager.NativeManager.Close();
			});

			return false;
		}

		public override void Iconify(int f)
		{
			base.Iconify(f);
		}
	}

	public class Manager
	{
		public static swig.GUIManager NativeManager;
		public static swig.Native Native;

		static GUIManagerCallback guiManagerCallback;

		static int nextID = 10;

		public static Viewer Viewer;

		internal static Network Network;

		internal static swig.Vec2 WindowSize = new swig.Vec2(1280, 720);

		public static float TextOffsetY {get; private set;}

		static int resetCount = 0;
		internal static int resizedCount = 0;
		internal static int actualWidth = 1;

        /// <summary>
        /// if this flag is true, a dialog box on disposing is not shown
        /// </summary>
		internal static bool IgnoreDisposingDialogBox = false;

		/// <summary>
		/// 
		/// </summary>
		/// <remarks>
		/// This order is important for dock panel.
		/// </remarks>
		static Type[] dockTypes =
		{
			typeof(Dock.ViewerController),
			typeof(Dock.NodeTreeView),
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
			typeof(Dock.GlobalValues),
			typeof(Dock.BehaviorValues),
			typeof(Dock.Culling),
			typeof(Dock.Network),
			typeof(Dock.FileViewer),
		};

		static Dock.DockManager dockManager = null;

		static Dock.EffectViwer effectViewer = null;

		static Dock.DockPanel[] panels = new Dock.DockPanel[0];

		public static bool IsDockMode() { return dockManager != null; }

		internal static Utils.DelayedList<IRemovableControl> Controls = new Utils.DelayedList<IRemovableControl>();

		public static bool Initialize(int width, int height, swig.DeviceType deviceType)
		{
			var mgr = new swig.GUIManager();
			if (mgr.Initialize("Effekseer", 1280, 720, deviceType, false))
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
			if (!Viewer.ShowViewer(mgr.GetNativeHandle(), 1280, 720, deviceType))
			{
				mgr.Dispose();
				mgr = null;
				return false;
			}

            mgr.InitializeGUI(Native);
			
			NativeManager = mgr;

			Images.Load(GUI.Manager.Native);

			guiManagerCallback = new GUIManagerCallback();
			NativeManager.SetCallback(guiManagerCallback);

			panels = new Dock.DockPanel[dockTypes.Length];

			var appDirectory = Manager.GetEntryDirectory();

			// Load font
			NativeManager.AddFontFromFileTTF(System.IO.Path.Combine(appDirectory, "resources/GenShinGothic-Monospace-Normal.ttf"), 16);

			// Load window icon
			NativeManager.SetWindowIcon(System.IO.Path.Combine(appDirectory, "resources/icon.png"));

			// Load config
			RecentFiles.LoadRecentConfig();
			Shortcuts.LoadShortcuts();
			Commands.Register();

			// Add controls
			var mainMenu = new GUI.Menu.MainMenu();
			GUI.Manager.AddControl(mainMenu);

			dockManager = new GUI.Dock.DockManager();
			GUI.Manager.AddControl(dockManager);

			// Default 
			effectViewer = new Dock.EffectViwer();
			if(dockManager != null)
			{
				effectViewer.InitialDockSlot = swig.DockSlot.None;
				dockManager.Controls.Add(effectViewer);
			}
			else
			{
				AddControl(effectViewer);
			}

			if (LoadWindowConfig("config.Dock.xml"))
			{
				Manager.NativeManager.LoadDock("config.Dock.config");
			}
			else
			{
				ResetWindowActually();
			}

			TextOffsetY = (NativeManager.GetTextLineHeightWithSpacing() - NativeManager.GetTextLineHeight()) / 2;

			Network = new Network(Native);
			Network.Load();

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

			// Set imgui path
			var entryDirectory = GetEntryDirectory();
			swig.GUIManager.SetIniFilename(entryDirectory + "/imgui.ini");

			return true;
		}

		public static void Terminate()
		{
			var entryDirectory = GetEntryDirectory();
			System.IO.Directory.SetCurrentDirectory(entryDirectory);

			Manager.NativeManager.SaveDock("config.Dock.config");
			SaveWindowConfig("config.Dock.xml");

			foreach (var p in panels)
			{
				if(p != null)
				{
					p.DispatchDisposed();
				}
			}

			if (effectViewer != null)
			{
				effectViewer.DispatchDisposed();
			}

			Network.Save();
			Shortcuts.SeveShortcuts();
			RecentFiles.SaveRecentConfig();

			Viewer.HideViewer();

			NativeManager.SetCallback(null);
			NativeManager.Terminate();

			Images.Unload();
		}

		public static void AddControl(IRemovableControl control)
		{
			Controls.Add(control);
		}

		static swig.Vec2 mousePos_pre;

		static bool isFirstUpdate = true;

		public static void Update()
		{
			// Reset
			NativeManager.SetNextDockRate(0.5f);
			NativeManager.SetNextDock(swig.DockSlot.Tab);
			NativeManager.ResetNextParentDock();

			Shortcuts.Update();
			Network.Update();

			var handle = false;
			if(!handle)
			{
				var cursor = Manager.NativeManager.GetMouseCursor();
				if (cursor == swig.MouseCursor.None || cursor == swig.MouseCursor.Arrow)
				{
					Shortcuts.ProcessCmdKey(ref handle);
				}
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

			if (resetCount < 0)
			{
				resetCount++;
				if (resetCount == 0)
				{
					ResetWindowActually();
				}
			}

			if(resizedCount > 0)
			{
				resizedCount--;
			}

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

			NativeManager.RenderGUI(resizedCount == 0);

			Native.Present();
			NativeManager.Present();

			isFirstUpdate = false;

			// TODO more smart
			// When minimized, suppress CPU activity
			if (actualWidth == 0)
			{
				System.Threading.Thread.Sleep(16);
			}
		}

		public static void ResetWindow()
		{
			effectViewer?.Close();
			effectViewer = null;

			for (int i = 0; i < panels.Length; i++)
			{
				panels[i]?.Close();
			}
			resetCount = -5;

			NativeManager.ShutdownDock();
		}

		static void ResetWindowActually()
		{
			if(effectViewer == null)
			{
				effectViewer = new Dock.EffectViwer();
				if (dockManager != null)
				{
					effectViewer.InitialDockSlot = swig.DockSlot.None;
					dockManager.Controls.Add(effectViewer);
				}
			}
			
			SelectOrShowWindow(typeof(Dock.ViewerController), null, swig.DockSlot.Bottom, 0.15f, true);
			SelectOrShowWindow(typeof(Dock.NodeTreeView), null, swig.DockSlot.Right, 0.3f, true);
			var basicPanel = SelectOrShowWindow(typeof(Dock.CommonValues), null, swig.DockSlot.Top, 0.7f, false);
			SelectOrShowWindow(typeof(Dock.LocationValues), null, swig.DockSlot.Tab, 1.0f, false);
			SelectOrShowWindow(typeof(Dock.RotationValues), null, swig.DockSlot.Tab, 1.0f, false);
			SelectOrShowWindow(typeof(Dock.ScaleValues), null, swig.DockSlot.Tab, 1.0f, false);
			SelectOrShowWindow(typeof(Dock.RendererCommonValues), null, swig.DockSlot.Tab, 1.0f, false);
			SelectOrShowWindow(typeof(Dock.RendererValues), null, swig.DockSlot.Tab, 1.0f, false);

			basicPanel.InitialDockActive = 2;
		}

		internal static Dock.DockPanel GetWindow(Type t)
		{
			foreach(var panel in panels)
			{
				if (panel.GetType() == t) return panel;
			}

			return null;
		}

		internal static Dock.DockPanel SelectOrShowWindow(Type t, swig.Vec2 defaultSize = null, swig.DockSlot slot = swig.DockSlot.None, float dockRate = 0.5f, bool isResetParent = false)
		{
			for(int i = 0; i < dockTypes.Length; i++)
			{
				if (dockTypes[i] != t) continue;

				if(panels[i] != null)
				{
					return panels[i];
				}
				else
				{
					if(defaultSize == null)
					{
						defaultSize = new swig.Vec2();
					}

					panels[i] = (Dock.DockPanel)t.GetConstructor(Type.EmptyTypes).Invoke(null);
					panels[i].InitialDockSlot = slot;
					panels[i].InitialDockSize = defaultSize;
					panels[i].InitialDockReset = isResetParent;
					panels[i].InitialDockRate = dockRate;
					panels[i].IsInitialized = -1;

					if (dockManager != null)
					{
						dockManager.Controls.Add(panels[i]);
					}
					else
					{
						AddControl(panels[i]);
					}

					return panels[i];
				}
			}

			return null;
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

		public static bool LoadWindowConfig(string path)
		{
			if (!System.IO.File.Exists(path)) return false;

			var doc = new System.Xml.XmlDocument();

			doc.Load(path);

			if (doc.ChildNodes.Count != 2) return false;
			if (doc.ChildNodes[1].Name != "Root") return false;

			
			var windowWidth = doc["Root"]["WindowWidth"]?.GetTextAsInt() ?? 1280;
			var windowHeight = doc["Root"]["WindowHeight"]?.GetTextAsInt() ?? 720;

			Manager.NativeManager.SetSize(windowWidth, windowHeight);

			var docks = doc["Root"]["Docks"];

			if(docks != null)
			{
				for (int i = 0; i < docks.ChildNodes.Count; i++)
				{
					var panel = docks.ChildNodes[i];
					var type = dockTypes.FirstOrDefault(_ => _.ToString() == panel.Name);

					if(type != null)
					{
						SelectOrShowWindow(type);
					}
				}
			}

			return true;
		}

		public static void SaveWindowConfig(string path)
		{
			var size = Manager.WindowSize;

			System.Xml.XmlDocument doc = new System.Xml.XmlDocument();

			System.Xml.XmlElement project_root = doc.CreateElement("Root");
			project_root.AppendChild(doc.CreateTextElement("WindowWidth", size.X.ToString()));
			project_root.AppendChild(doc.CreateTextElement("WindowHeight", size.Y.ToString()));

			System.Xml.XmlElement docks = doc.CreateElement("Docks");

			foreach(var panel in panels)
			{
				if(panel != null)
				{
					docks.AppendChild(doc.CreateTextElement(panel.GetType().ToString(), "Open"));
				}
			}

			project_root.AppendChild(docks);

			doc.AppendChild(project_root);

			var dec = doc.CreateXmlDeclaration("1.0", "utf-8", null);
			doc.InsertBefore(dec, project_root);
			doc.Save(path);
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
			var dir = System.IO.Path.GetDirectoryName(path);

			// for mkbundle
			if (dir == string.Empty)
			{
				dir = System.IO.Path.GetDirectoryName(
				System.IO.Path.GetFullPath(
				Environment.GetCommandLineArgs()[0]));
			}

			return dir;
		}
	}
}
