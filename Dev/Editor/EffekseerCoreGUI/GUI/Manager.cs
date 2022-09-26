using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Effekseer.swig;
using Effekseer.Utils;

namespace Effekseer.GUI
{
	class GUIManagerCallback : swig.GUIManagerCallback
	{
		void HandleAction()
		{
		}

		public override void Resized(int x, int y)
		{
			if (x > 0 && y > 0)
			{
				Manager.HardwareDevice.GraphicsDevice.Resize(x, y);

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

	public class HardwareDevice : IDisposable
	{
		public swig.GraphicsDevice GraphicsDevice { get; private set; }

		public swig.SoundDevice SoundDevice { get; private set; }

		public bool Initialize(IntPtr handle, int width, int height, swig.DeviceType deviceType)
		{
			GraphicsDevice = swig.GraphicsDevice.Create(handle, width, height, Core.Option.ColorSpace.Value == Data.OptionValues.ColorSpaceType.LinearSpace, deviceType);
			SoundDevice = swig.SoundDevice.Create();
			return GraphicsDevice != null;
		}

		public void Dispose()
		{
			if (GraphicsDevice != null)
			{
				GraphicsDevice.Dispose();
			}

			if (SoundDevice != null)
			{
				SoundDevice.Dispose();
			}
		}
	}

	public class Manager
	{
		class ManagerIOCallback : swig.IOCallback
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

		public static swig.GUIManager NativeManager;

		public static HardwareDevice HardwareDevice { get; private set; }

		public static swig.RenderImage MainViewImage;
		public static swig.MainWindow MainWindow;
		public static swig.IO IO;
		static ManagerIOCallback ioCallback;

		static GUIManagerCallback guiManagerCallback;

		static int nextID = 10;

		static bool isFontSizeDirtied = true;

		public static Viewer Viewer;

		public static Network Network;

		internal static swig.Vec2 WindowSize = new swig.Vec2(1280, 720);

		internal static bool DoesChangeColorOnChangedValue = true;

		public static float TextOffsetY { get; private set; }

		public static float DpiScale
		{
			get
			{
				return NativeManager.GetDpiScale();
			}
		}

		public static bool IsWindowFrameless { get; private set; }

		static int resetCount = 0;
		internal static int resizedCount = 0;
		internal static int actualWidth = 1;

		private static DateTime lastAutoSaveTime;

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
		static Type[] dockTypes;

		public static Dock.DockManager dockManager = null;

		public static Dock.EffectViwerPaneBase effectViewer = null;

		static Dock.DockPanel[] panels = new Dock.DockPanel[0];

		public static bool IsDockMode() { return dockManager != null; }

		internal static Utils.DelayedList<IRemovableControl> Controls = new Utils.DelayedList<IRemovableControl>();

		public static string ConfigFilePath { get { return System.IO.Path.Combine(GetEntryDirectory(), Application.Name + ".config.Dock.xml"); } }

		public static string ImGuiIniFilePath { get { return System.IO.Path.Combine(GetEntryDirectory(), Application.Name + ".imgui.ini"); } }

		public static bool Initialize(int width, int height, swig.DeviceType deviceType, Type[] dockTypes_)
		{
			dockTypes = dockTypes_;
			var appDirectory = Manager.GetEntryDirectory();

			swig.MainWindowState state = new swig.MainWindowState();

			if (System.Environment.OSVersion.Platform == PlatformID.Win32NT)
			{
				IsWindowFrameless = true;
			}

			// TODO : refactor
			var windowConfig = new Configs.WindowConfig();
			if (windowConfig.Load(ConfigFilePath))
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

			if (!swig.MainWindow.Initialize("Effekseer", state, false, deviceType == swig.DeviceType.OpenGL))
			{
				return false;
			}
			MainWindow = swig.MainWindow.GetInstance();

			swig.IO.Initialize(1000);
			IO = swig.IO.GetInstance();
			ioCallback = new ManagerIOCallback();
			IO.AddCallback(ioCallback);

			Core.OnFileLoaded += (string path) =>
			{

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

			HardwareDevice = new HardwareDevice();
			if (!HardwareDevice.Initialize(mgr.GetNativeHandle(), state.Width, state.Height, deviceType))
			{
				return false;
			}

			Viewer = new Viewer(HardwareDevice);
			if (!Viewer.Initialize(deviceType))
			{
				mgr.Dispose();
				mgr = null;
				return false;
			}

			mgr.InitializeGUI(HardwareDevice.GraphicsDevice);

			NativeManager = mgr;

			MainViewImage = RenderImage.Create(HardwareDevice.GraphicsDevice);

			Images.Load(HardwareDevice.GraphicsDevice);

			guiManagerCallback = new GUIManagerCallback();
			NativeManager.SetCallback(guiManagerCallback);

			panels = new Dock.DockPanel[dockTypes.Length];

			// Load font
			UpdateFont();

			// Load window icon
			NativeManager.SetWindowIcon(System.IO.Path.Combine(appDirectory, "resources/icon.png"));

			// Load config
			RecentFiles.LoadRecentConfig();
			Shortcuts.LoadShortcuts();
			Commands.Register();

			// Add controls
			Application.Current.OnCreateMainMenu();

			dockManager = new GUI.Dock.DockManager();
			GUI.Manager.AddControl(dockManager);

			// EffectViwer 
			Application.Current.OnCreateEffectViwer();

			if (LoadWindowConfig(ConfigFilePath))
			{
			}
			else
			{
				ResetWindow();
			}

			TextOffsetY = (NativeManager.GetTextLineHeightWithSpacing() - NativeManager.GetTextLineHeight()) / 2;

			lastAutoSaveTime = DateTime.UtcNow;
			
			Network = new Network();
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

			Core.LodValues.Lod0Enabled.OnChanged += OnChanged;
			Core.LodValues.Distance0.OnChanged += OnChanged;
			Core.LodValues.Lod1Enabled.OnChanged += OnChanged;
			Core.LodValues.Distance1.OnChanged += OnChanged;
			Core.LodValues.Lod2Enabled.OnChanged += OnChanged;
			Core.LodValues.Distance2.OnChanged += OnChanged;
			Core.LodValues.Lod3Enabled.OnChanged += OnChanged;
			Core.LodValues.Distance3.OnChanged += OnChanged;

			Core.OnAfterLoad += new EventHandler(Core_OnAfterLoad);
			Core.OnAfterNew += new EventHandler(Core_OnAfterNew);
			Core.OnReload += new EventHandler(Core_OnReload);

			// Set imgui path
			var entryDirectory = GetEntryDirectory();
			swig.GUIManager.SetIniFilename(ImGuiIniFilePath);

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
			Core.SaveBackup(System.IO.Path.GetTempPath() + "/efk_quit.efkbac");
			
			var entryDirectory = GetEntryDirectory();
			System.IO.Directory.SetCurrentDirectory(entryDirectory);

			SaveWindowConfig(ConfigFilePath);

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

			Network.Dispose();
			Network = null;

			Viewer.HideViewer();
			Viewer.Dispose();
			Viewer = null;

			if (MainViewImage != null)
			{
				MainViewImage.Dispose();
				MainViewImage = null;
			}

			NativeManager.SetCallback(null);
			NativeManager.Terminate();

			Images.Unload();

			if (HardwareDevice != null)
			{
				HardwareDevice.Dispose();
				HardwareDevice = null;
			}

			swig.MainWindow.Terminate();
			MainWindow.Dispose();
			MainWindow = null;

			ThumbnailManager.Terminate();
			swig.IO.Terminate();
			IO.Dispose();
			IO = null;
		}

		public static void UpdateFont()
		{
			isFontSizeDirtied = true;
		}
		public static void AddControl(IRemovableControl control)
		{
			Controls.Add(control);
		}

		static swig.Vec2 mousePos_pre;

		static bool isFirstUpdate = true;

		protected static int LEFT_SHIFT = 340;
		protected static int RIGHT_SHIFT = 344;

		protected static int LEFT_CONTROL = 341;
		protected static int RIGHT_CONTROL = 345;

		protected static int LEFT_ALT = 342;
		protected static int RIGHT_ALT = 346;

		protected static int LEFT_SUPER = 343;
		protected static int RIGHT_SUPER = 347;

		struct ViewportControllerResult
		{
			public bool Rotate;
			public bool Slide;
			public bool Zoom;
		}

		static ViewportControllerResult ControllViewport()
		{
			bool isLeftPressed = NativeManager.GetMouseButton(0) > 0;
			bool isRightPressed = NativeManager.GetMouseButton(1) > 0;
			bool isWheelPressed = NativeManager.GetMouseButton(2) > 0;

			bool isShiftPressed = Manager.NativeManager.IsKeyDown(LEFT_SHIFT) || Manager.NativeManager.IsKeyDown(RIGHT_SHIFT);
			bool isCtrlPressed = Manager.NativeManager.IsKeyDown(LEFT_CONTROL) || Manager.NativeManager.IsKeyDown(RIGHT_CONTROL);
			bool isAltPressed = Manager.NativeManager.IsKeyDown(LEFT_ALT) || Manager.NativeManager.IsKeyDown(RIGHT_ALT);
			bool isSuperPressed = Manager.NativeManager.IsKeyDown(LEFT_SUPER) || Manager.NativeManager.IsKeyDown(RIGHT_SUPER);

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

		public static void Update()
		{
			if (isFontSizeDirtied)
			{
				NativeManager.InvalidateFont();
				var appDirectory = Manager.GetEntryDirectory();
				var type = Core.Option.Font.Value;

				NativeManager.ClearAllFonts();

				var characterTable = System.IO.Path.Combine(appDirectory, "resources/languages/characterTable.txt");

				if (type == Data.FontType.Normal)
				{
					NativeManager.AddFontFromFileTTF(System.IO.Path.Combine(appDirectory, MultiLanguageTextProvider.GetText("Font_Normal")), characterTable, MultiLanguageTextProvider.GetText("CharacterTable"), Core.Option.FontSize.Value);
				}
				else if (type == Data.FontType.Bold)
				{
					NativeManager.AddFontFromFileTTF(System.IO.Path.Combine(appDirectory, MultiLanguageTextProvider.GetText("Font_Bold")), characterTable, MultiLanguageTextProvider.GetText("CharacterTable"), Core.Option.FontSize.Value);
				}

				NativeManager.AddFontFromAtlasImage(System.IO.Path.Combine(appDirectory, "resources/icons/MenuIcons.png"), 0xec00, 24, 24, 16, 16);

				isFontSizeDirtied = false;
			}

			// Reset

			NativeManager.ResetGUI();

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

			if (HardwareDevice.SoundDevice != null)
			{
				var ray = Viewer.ViewPointController.GetCameraRay();
				var rayOrigin = ray.Origin;
				var rayDirection = ray.Direction;
				var rayPos = new swig.Vector3F(rayOrigin.X + rayDirection.X, rayOrigin.Y + rayDirection.Y, rayOrigin.Z + rayDirection.Z);
				HardwareDevice.SoundDevice.SetListener(ray.Origin, rayPos, new swig.Vector3F(0, 1, 0));
			}

			HardwareDevice.GraphicsDevice.ResetRenderTargets();
			HardwareDevice.GraphicsDevice.ClearColor(50, 50, 50, 0);

			//if(effectViewer == null)
			//{
			//	Native.RenderWindow();
			//}

			UpdateAutoSave();

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

			HardwareDevice.GraphicsDevice.Present();

			NativeManager.Present();

			isFirstUpdate = false;

			// TODO more smart
			// When minimized, suppress CPU activity
			if (actualWidth == 0)
			{
				System.Threading.Thread.Sleep(16);
			}
		}

		private static void UpdateAutoSave()
		{
			int autoSaveIntervalMin = Core.Option.AutoSaveIntervalMin.GetValue();
			if (autoSaveIntervalMin <= 0) return;
			
			
			DateTime now = DateTime.UtcNow;
			if (lastAutoSaveTime.AddMinutes(autoSaveIntervalMin) <= now)
			{
				Commands.SaveBackup();
				lastAutoSaveTime = now;
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


		public static Dock.DockPanel GetWindow(Type t)
		{
			foreach (var panel in panels)
			{
				if (panel != null && panel.GetType() == t) return panel;
			}

			return null;
		}

		public static Dock.DockPanel SelectOrShowWindow(Type t, swig.Vec2 defaultSize = null, bool resetSize = false, bool requireClose = true)
		{
			for (int i = 0; i < dockTypes.Length; i++)
			{
				if (dockTypes[i] != t) continue;

				if (panels[i] != null)
				{
					if (panels[i].Visibled && requireClose)
					{
						panels[i].Close();
					}
					else
					{
						panels[i].SetFocus();
					}

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
					panels[i].ResetSize = resetSize;

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

		/// <summary>
		/// get a scale based on font size for margin, etc.
		/// </summary>
		/// <returns></returns>
		public static float GetUIScaleBasedOnFontSize()
		{
			return Core.Option.FontSize.Value / 16.0f * DpiScale;
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
		/// TODO refactor
		/// </summary>
		/// <param name="path"></param>
		/// <returns></returns>
		public static bool LoadWindowConfig(string path)
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
			var size = Manager.NativeManager.GetSize();

			const int minimumSize = 200;

			var state = MainWindow.GetState();
			if (state.Width <= minimumSize || state.Height <= minimumSize) return;

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