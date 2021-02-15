using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Effekseer.swig;
using Effekseer.Utl;

namespace Effekseer.GUI
{
	/// <summary>
	/// Native 側から通知される GUI イベントを受け取るためのクラス
	/// </summary>
	public class GUIManagerCallback : swig.GUIManagerCallback
	{
		void HandleAction()
		{
		}


		public override void Resized(int x, int y)
		{
			if (x > 0 && y > 0)
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

			if (!handle)
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

		public override void DpiChanged(float f)
		{
			Manager.UpdateFont();
		}

		public override bool ClickLink(string path)
		{
			try
			{
				System.Diagnostics.Process.Start(path);
			}
			catch
			{
				return false;
			}

			return true;
		}
	}

	public class Manager
	{
		public class ManagerIOCallback : swig.IOCallback
		{
			public override void OnFileChanged(StaticFileType fileType, string path)
			{
				var ext = System.IO.Path.GetExtension(path).ToLower();
				if (ext == ".efkmat")
				{
					Core.UpdateResourcePaths(path);
					Viewer.IsRequiredToReload = true;
				}
			}
		}

		struct ViewportControllerResult
		{
			public bool Rotate;
			public bool Slide;
			public bool Zoom;
		}

		private const int LEFT_SHIFT = 340;
		private const int RIGHT_SHIFT = 344;
		private const int LEFT_CONTROL = 341;
		private const int RIGHT_CONTROL = 345;
		private const int LEFT_ALT = 342;
		private const int RIGHT_ALT = 346;
		private const int LEFT_SUPER = 343;
		private const int RIGHT_SUPER = 347;



		public static swig.GUIManager NativeManager;
		public static swig.Native Native;
		public static swig.MainWindow MainWindow;
		public static swig.IO IO;
		public static Viewer Viewer;
		public static Network Network;


		internal static bool DoesChangeColorOnChangedValue = true;

		private static int nextID = 10;

		protected static bool isFontSizeDirtied = true;

		public static Utils.DelayedList<IRemovableControl> Controls = new Utils.DelayedList<IRemovableControl>();
		public static Dock.DockManager dockManager = null;
		public static Dock.DockPanel[] panels = new Dock.DockPanel[0];
		public static Dock.EffectViwer effectViewer = null;



		public static ManagerIOCallback ioCallback;
		public static GUIManagerCallback guiManagerCallback;

		protected static int resetCount = 0;

		public static swig.Vec2 WindowSize = new swig.Vec2(1280, 720);
		public static int resizedCount = 0;
		public static int actualWidth = 1;

		static swig.Vec2 mousePos_pre;
		static bool isFirstUpdate = true;

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
		public static Type[] dockTypes = null;
		
		public static bool IsWindowFrameless { get; private set; }
		public static bool IsDockMode() { return dockManager != null; }

		public static float TextOffsetY { get; set; }

		public static float DpiScale
		{
			get
			{
				return NativeManager.GetDpiScale();
			}
		}

		public static string WindowConfigFileFullPath { get { return System.IO.Path.Combine(Application.EntryDirectory, Application.Name + ".config.Dock.xml"); } }

		public static string ImGuiINIFileFullPath { get { return System.IO.Path.Combine(Application.EntryDirectory, Application.Name + ".imgui.ini"); } }

		public static bool Initialize(int width, int height, Func<IRemovableControl> createMainMenu, string appIconPath)
		{
			var appDirectory = Application.EntryDirectory;

			swig.MainWindowState state = new swig.MainWindowState();

			if (System.Environment.OSVersion.Platform == PlatformID.Win32NT)
			{
				IsWindowFrameless = true;
			}

			// TODO : refactor
			var windowConfig = new Configs.WindowConfig();
			if (windowConfig.Load(WindowConfigFileFullPath))
			{
				state.PosX = windowConfig.WindowPosX;
				state.PosY = windowConfig.WindowPosY;
				state.Width = windowConfig.WindowWidth;
				state.Height = windowConfig.WindowHeight;
				state.IsMaximumMode = windowConfig.WindowIsMaximumMode;
			}
			else
			{
				state.PosX = -10000; // nodata
				state.Width = 1280;
				state.Height = 720;
				windowConfig = null;
			}

			state.IsFrameless = IsWindowFrameless;

			System.OperatingSystem os = System.Environment.OSVersion;
			swig.DeviceType deviceType = swig.DeviceType.DirectX11;
			if (!(os.Platform == PlatformID.Win32NT ||
				os.Platform == PlatformID.Win32S ||
				os.Platform == PlatformID.Win32Windows ||
				os.Platform == PlatformID.WinCE))
			{
				deviceType = swig.DeviceType.OpenGL;
			}

			if (!swig.MainWindow.Initialize("Effekseer", state, false, deviceType == swig.DeviceType.OpenGL))
			{
				return false;
			}
			MainWindow = swig.MainWindow.GetInstance();

			swig.IO.Initialize(1000);
			IO = swig.IO.GetInstance();
			ioCallback = new ManagerIOCallback();
			IO.AddCallback(ioCallback);

			Core.OnFileLoaded += (string path) => {

#if DEBUG
				Console.WriteLine("OnFileLoaded : " + path);
#endif

				var f = IO.LoadIPCFile(path);
				if (f == null)
				{
					f = IO.LoadFile(path);
				}

				// TODO : refactor it
				// Permission error
				if (f != null && f.GetSize() == 0)
				{
					var message = MultiLanguageTextProvider.GetText("PermissionError_File");

					if (swig.GUIManager.IsMacOSX())
					{
						message += "\n";
						message += MultiLanguageTextProvider.GetText("PermissionError_File_Mac");
					}

					message = string.Format(message, System.IO.Path.GetFileName(path));

					throw new FileLoadPermissionException(message);
				}

				if (f == null) return null;

				byte[] ret = new byte[f.GetSize()];
				System.Runtime.InteropServices.Marshal.Copy(f.GetData(), ret, 0, ret.Length);
				f.Dispose();
				return ret;
			};

			ThumbnailManager.Initialize();

			var mgr = new swig.GUIManager();
			if (mgr.Initialize(MainWindow, deviceType))
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
			if (!Viewer.ShowViewer(mgr.GetNativeHandle(), state.Width, state.Height, deviceType))
			{
				mgr.Dispose();
				mgr = null;
				return false;
			}

			mgr.InitializeGUI(Native);

			NativeManager = mgr;


			Images.LoadCommonIcons(Native, appIconPath);


			guiManagerCallback = new GUIManagerCallback();
			NativeManager.SetCallback(guiManagerCallback);



			panels = new Dock.DockPanel[dockTypes.Length];

			// Load font
			UpdateFont();

			// Load window icon
			NativeManager.SetWindowIcon(appIconPath);

			// Load config
			RecentFiles.LoadRecentConfig();
			Shortcuts.LoadShortcuts();
			Commands.Register();

			// Add controls
			var mainMenu = createMainMenu();
			AddControl(mainMenu);

			dockManager = new GUI.Dock.DockManager();
			AddControl(dockManager);

			// Default 
			effectViewer = new Dock.EffectViwer();
			if (dockManager != null)
			{
				dockManager.Controls.Add(effectViewer);
			}
			else
			{
				AddControl(effectViewer);
			}

			if (LoadWindowConfig(WindowConfigFileFullPath))
			{
			}
			else
			{
				ResetWindow();
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

			Core.EffectBehavior.PlaybackSpeed.OnChanged += OnChanged;

			Core.Option.Magnification.OnChanged += OnChanged;
			Core.Option.IsGridShown.OnChanged += OnChanged;
			Core.Option.GridLength.OnChanged += OnChanged;
			Core.Option.GridColor.R.OnChanged += OnChanged;
			Core.Option.GridColor.G.OnChanged += OnChanged;
			Core.Option.GridColor.B.OnChanged += OnChanged;
			Core.Option.GridColor.A.OnChanged += OnChanged;
			Core.Option.FPS.OnChanged += OnChanged;

			Core.Option.DistortionType.OnChanged += OnChanged;
			Core.Option.Coordinate.OnChanged += OnChanged;

			Core.Environment.Background.BackgroundColor.R.OnChanged += OnChanged;
			Core.Environment.Background.BackgroundColor.G.OnChanged += OnChanged;
			Core.Environment.Background.BackgroundColor.B.OnChanged += OnChanged;
			Core.Environment.Background.BackgroundColor.A.OnChanged += OnChanged;
			Core.Environment.Background.BackgroundImage.OnChanged += OnChanged;

			Core.Environment.Ground.IsShown.OnChanged += OnChanged;
			Core.Environment.Ground.Height.OnChanged += OnChanged;
			Core.Environment.Ground.Extent.OnChanged += OnChanged;

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
			swig.GUIManager.SetIniFilename(ImGuiINIFileFullPath);

			// check files
			if (!System.IO.File.Exists(System.IO.Path.Combine(appDirectory, "resources/fonts/GenShinGothic-Monospace-Bold.ttf")))
			{
				ErrorUtils.ThrowFileNotfound();
			}

			if (!System.IO.File.Exists(System.IO.Path.Combine(appDirectory, "resources/icons/MenuIcons.png")))
			{
				ErrorUtils.ThrowFileNotfound();
			}

			return true;
		}

		public static void Terminate()
		{
			var entryDirectory = Application.EntryDirectory;
			System.IO.Directory.SetCurrentDirectory(entryDirectory);

			SaveWindowConfig(WindowConfigFileFullPath);

			foreach (var p in panels)
			{
				if (p != null)
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

			swig.MainWindow.Terminate();
			MainWindow.Dispose();
			MainWindow = null;

			ThumbnailManager.Terminate();
			swig.IO.Terminate();
			IO.Dispose();
			IO = null;
		}

		public static void Update()
		{
			if (isFontSizeDirtied)
			{
				NativeManager.InvalidateFont();
				var appDirectory = Application.EntryDirectory;
				var type = Core.Option.Font.Value;

				NativeManager.ClearAllFonts();

				if (type == Data.FontType.Normal)
				{
					NativeManager.AddFontFromFileTTF(System.IO.Path.Combine(appDirectory, MultiLanguageTextProvider.GetText("Font_Normal")), Core.Option.FontSize.Value);
				}
				else if (type == Data.FontType.Bold)
				{
					NativeManager.AddFontFromFileTTF(System.IO.Path.Combine(appDirectory, MultiLanguageTextProvider.GetText("Font_Bold")), Core.Option.FontSize.Value);
				}

				NativeManager.AddFontFromAtlasImage(System.IO.Path.Combine(appDirectory, "resources/icons/MenuIcons.png"), 0xec00, 24, 24, 16, 16);

				isFontSizeDirtied = false;
			}

			// Reset

			IO.Update();
			Shortcuts.Update();
			Network.Update();

			var handle = false;
			if (!handle)
			{
				if (!NativeManager.IsAnyItemActive())
				{
					Shortcuts.ProcessCmdKey(ref handle);
				}
			}

			var mousePos = NativeManager.GetMousePosition();

			if (isFirstUpdate)
			{
				mousePos_pre = mousePos;
			}

			if ((effectViewer == null && !NativeManager.IsAnyWindowHovered()) || (effectViewer != null && effectViewer.IsHovered))
			{
				var result = ControllViewport();

				if (result.Slide)
				{
					var dx = mousePos.X - mousePos_pre.X;
					var dy = mousePos.Y - mousePos_pre.Y;

					if (Core.Option.ViewerMode.Value == Data.OptionValues.ViewMode._3D)
					{
						Viewer.Slide(dx / 30.0f, dy / 30.0f);
					}
					else if (Core.Option.ViewerMode.Value == Data.OptionValues.ViewMode._2D)
					{
						Viewer.Slide(dx / 16.0f, dy / 16.0f);
					}
				}
				else if (NativeManager.GetMouseWheel() != 0)
				{
					Viewer.Zoom(NativeManager.GetMouseWheel());
				}
				else if (result.Zoom)
				{
					var dx = mousePos.X - mousePos_pre.X;
					var dy = mousePos.Y - mousePos_pre.Y;
					Viewer.Zoom(-dy * 0.25f);
				}
				else if (result.Rotate)
				{
					var dx = mousePos.X - mousePos_pre.X;
					var dy = mousePos.Y - mousePos_pre.Y;

					if (Core.Option.ViewerMode.Value == Data.OptionValues.ViewMode._3D)
					{
						Viewer.Rotate(dx, dy);
					}
					else if (Core.Option.ViewerMode.Value == Data.OptionValues.ViewMode._2D)
					{
						Viewer.Slide(dx / 16.0f, dy / 16.0f);
					}
				}
			}

			mousePos_pre = mousePos;

			Viewer.UpdateViewer();

			Native.UpdateWindow();

			Native.ClearWindow(50, 50, 50, 0);

			//if(effectViewer == null)
			//{
			//	Native.RenderWindow();
			//}

			NativeManager.ResetGUI();

			if (resetCount < 0)
			{
				resetCount++;
				if (resetCount == 0)
				{
					Application.Current.OnResetWindowActually();
				}
			}

			if (resizedCount > 0)
			{
				resizedCount--;
			}

			Controls.Lock();

			foreach (var c in Controls.Internal)
			{
				c.Update();
			}

			foreach (var _ in Controls.Internal)
			{
				if (!_.ShouldBeRemoved) continue;

				var dp = _ as Dock.DockPanel;
				if (dp != null)
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

		static ViewportControllerResult ControllViewport()
		{
			bool isLeftPressed = NativeManager.GetMouseButton(0) > 0;
			bool isRightPressed = NativeManager.GetMouseButton(1) > 0;
			bool isWheelPressed = NativeManager.GetMouseButton(2) > 0;

			bool isShiftPressed = NativeManager.IsKeyDown(LEFT_SHIFT) || NativeManager.IsKeyDown(RIGHT_SHIFT);
			bool isCtrlPressed = NativeManager.IsKeyDown(LEFT_CONTROL) || NativeManager.IsKeyDown(RIGHT_CONTROL);
			bool isAltPressed = NativeManager.IsKeyDown(LEFT_ALT) || NativeManager.IsKeyDown(RIGHT_ALT);
			bool isSuperPressed = NativeManager.IsKeyDown(LEFT_SUPER) || NativeManager.IsKeyDown(RIGHT_SUPER);

			bool isSlidePressed = false;
			bool isZoomPressed = false;
			bool isRotatePressed = false;

			switch (Core.Option.MouseMappingType.Value)
			{
				case Data.MouseMappingType.Effekseer:
					isSlidePressed = isWheelPressed || (isRightPressed && isShiftPressed);
					isZoomPressed = isRightPressed && isCtrlPressed;
					isRotatePressed = isRightPressed;
					break;

				case Data.MouseMappingType.Blender:
					isSlidePressed = isWheelPressed && isShiftPressed;
					isZoomPressed = isWheelPressed && isCtrlPressed;
					isRotatePressed = isWheelPressed;
					break;

				case Data.MouseMappingType.Maya:
					isSlidePressed = isWheelPressed && isAltPressed;
					isZoomPressed = isRightPressed && isAltPressed;
					isRotatePressed = isLeftPressed && isAltPressed;
					break;

				case Data.MouseMappingType.Unity:
					isSlidePressed = isRightPressed;
					isZoomPressed = false;
					isRotatePressed = isWheelPressed;
					break;
			}

			ViewportControllerResult result;

			result.Rotate = isRotatePressed;
			result.Zoom = isZoomPressed;
			result.Slide = isSlidePressed;

			return result;
		}


		/// <summary>
		/// get a scale based on font size for margin, etc.
		/// </summary>
		/// <returns></returns>
		public static float GetUIScaleBasedOnFontSize()
		{
			return Core.Option.FontSize.Value / 16.0f * DpiScale;
		}

		public static void UpdateFont()
		{
			isFontSizeDirtied = true;
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

		public static void AddControl(IRemovableControl control)
		{
			Controls.Add(control);
		}

		internal static Dock.DockPanel GetWindow(Type t)
		{
			foreach (var panel in panels)
			{
				if (panel != null && panel.GetType() == t) return panel;
			}

			return null;
		}

		public static Dock.DockPanel SelectOrShowWindow(Type t, swig.Vec2 defaultSize = null, bool resetRect = false)
		{
			for (int i = 0; i < dockTypes.Length; i++)
			{
				if (dockTypes[i] != t) continue;

				if (panels[i] != null)
				{
					panels[i].SetFocus();
					return panels[i];
				}
				else
				{
					if (defaultSize == null)
					{
						defaultSize = new swig.Vec2();
					}

					panels[i] = (Dock.DockPanel)t.GetConstructor(Type.EmptyTypes).Invoke(null);
					panels[i].InitialDockSize = defaultSize;
					panels[i].IsInitialized = -1;
					panels[i].ResetSize = resetRect;

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

		protected static void Core_OnAfterLoad(object sender, EventArgs e)
		{
			Viewer.StopViewer();

			if (Network.SendOnLoad)
			{
				Network.Send();
			}
		}

		protected static void Core_OnAfterNew(object sender, EventArgs e)
		{
			Viewer.StopViewer();
		}

		protected static void Core_OnReload(object sender, EventArgs e)
		{
			Viewer.Reload(true);
		}

		protected static void OnChanged(object sender, EventArgs e)
		{
			Viewer.IsChanged = true;

			if (Network.SendOnEdit)
			{
				Network.Send();
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
		}

		/// <summary>
		/// TODO refactor
		/// </summary>
		/// <param name="path"></param>
		/// <returns></returns>
		protected static bool LoadWindowConfig(string path)
		{
			if (!System.IO.File.Exists(path)) return false;

			try
			{
				var doc = new System.Xml.XmlDocument();

				doc.Load(path);

				if (doc.ChildNodes.Count != 2) return false;
				if (doc.ChildNodes[1].Name != "Root") return false;


				var windowWidth = doc["Root"]["WindowWidth"]?.GetTextAsInt() ?? 1280;
				var windowHeight = doc["Root"]["WindowHeight"]?.GetTextAsInt() ?? 720;

				var docks = doc["Root"]["Docks"];

				if (docks != null)
				{
					for (int i = 0; i < docks.ChildNodes.Count; i++)
					{
						var panel = docks.ChildNodes[i];
						var type = dockTypes.FirstOrDefault(_ => _.ToString() == panel.Name);

						if (type != null)
						{
							SelectOrShowWindow(type);
						}
					}
				}
			}
			catch
			{
				return false;
			}

			return true;
		}

		/// <summary>
		/// TODO refactor
		/// </summary>
		/// <param name="path"></param>
		public static void SaveWindowConfig(string path)
		{
			var size = NativeManager.GetSize();

			var state = MainWindow.GetState();
			if (state.Width <= 0 || state.Height <= 0) return;

			System.Xml.XmlDocument doc = new System.Xml.XmlDocument();

			System.Xml.XmlElement project_root = doc.CreateElement("Root");
			project_root.AppendChild(doc.CreateTextElement("WindowWidth", state.Width.ToString()));
			project_root.AppendChild(doc.CreateTextElement("WindowHeight", state.Height.ToString()));
			project_root.AppendChild(doc.CreateTextElement("WindowPosX", state.PosX.ToString()));
			project_root.AppendChild(doc.CreateTextElement("WindowPosY", state.PosY.ToString()));
			project_root.AppendChild(doc.CreateTextElement("WindowIsMaximumMode", state.IsMaximumMode ? "1" : "0"));

			System.Xml.XmlElement docks = doc.CreateElement("Docks");

			foreach (var panel in panels)
			{
				if (panel != null)
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
	}
}
