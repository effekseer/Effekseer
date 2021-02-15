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
				GUIManager.Native.ResizeWindow(x, y);

				GUIManager.WindowSize.X = x;
				GUIManager.WindowSize.Y = y;
			}

			GUIManager.resizedCount = 5;
			GUIManager.actualWidth = x;
		}

		public override void Focused()
		{
			Core.Reload();
		}

		public override void Droped()
		{
			var path = GetPath();
			var handle = false;

			GUIManager.Controls.Lock();

			foreach (var c in GUIManager.Controls.Internal.OfType<Control>())
			{
				c.DispatchDropped(path, ref handle);
			}

			GUIManager.Controls.Unlock();

			if (!handle)
			{
				Commands.Open(this.GetPath());
			}
		}

		public override bool Closing()
		{
			if (GUIManager.IgnoreDisposingDialogBox) return true;
			if (!Core.IsChanged) return true;

			var dialog = new Dialog.SaveOnDisposing(
				() =>
				{
					GUIManager.IgnoreDisposingDialogBox = true;
					GUIManager.NativeManager.Close();
				});

			return false;
		}

		public override void Iconify(int f)
		{
			base.Iconify(f);
		}

		public override void DpiChanged(float f)
		{
			GUIManager.UpdateFont();
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

	public class GUIManager
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
		protected static Dock.EffectViwer effectViewer = null;



		public static ManagerIOCallback ioCallback;
		public static GUIManagerCallback guiManagerCallback;

		protected static int resetCount = 0;

		public static swig.Vec2 WindowSize = new swig.Vec2(1280, 720);
		public static int resizedCount = 0;
		public static int actualWidth = 1;

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

		protected static bool InitializeBase(int width, int height, swig.DeviceType deviceType, Func<IRemovableControl> createMainMenu)
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


			Images.Load(Native);


			guiManagerCallback = new GUIManagerCallback();
			NativeManager.SetCallback(guiManagerCallback);


			return true;
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
	}
}
