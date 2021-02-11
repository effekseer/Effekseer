using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Reflection;
using System.Runtime.ExceptionServices;
using EfkN = Effekseer.swig.EffekseerNative;

namespace Effekseer
{
	class Program
	{
		/// <summary>
		/// Starting directory
		/// </summary>
		public static string StartDirectory
		{
			get;
			private set;
		}

		public static string EntryDirectory
		{
			get;
			private set;
		}

		[STAThread]
		[HandleProcessCorruptedStateExceptions]
		static void Main(string[] args)
		{
			// HACK code for mac
			// To load libMonoPosix at first
			try
			{
				var dummy = Effekseer.Utils.Zlib.Decompress(new byte[] { 1, 2, 3, 4 });
			}
			catch(Exception e)
			{
				ExportError(e);
				return;
			}

			try
			{
				StartDirectory = System.IO.Directory.GetCurrentDirectory();
				EntryDirectory = GUI.Manager.GetEntryDirectory();
			}
			catch(Exception e)
			{
				ExportError(e);
				return;
			}

			bool gui = true;
			string input = string.Empty;
			string output = string.Empty;
			string format = "efk";
			string export = string.Empty;
			float magnification = 0.0f;
			bool materialCache = false;

			for (int i = 0; i < args.Length; i++)
			{
				if (args[i] == "-cui")
				{
					gui = false;
				}
				else if (args[i] == "-in")
				{
					i++;
					if (i < args.Length)
					{
						input = args[i];
					}
				}
				else if (args[i] == "-o")
				{
					i++;
					if (i < args.Length)
					{
						output = args[i];
					}
				}
				else if (args[i] == "-f")
				{
					i++;
					if (i < args.Length)
					{
						format = args[i];
					}
				}
				else if (args[i] == "-e")
				{
					i++;
					if (i < args.Length)
					{
						export = args[i];
					}
				}
				else if (args[i] == "-m")
				{
					i++;
					if (i < args.Length)
					{
						float.TryParse(args[i], out magnification);
					}
				}
				else if (args[i] == "--materialcache")
				{
					materialCache = true;
				}
				else
				{
					input = args[i];
				}
			}

			if (System.Diagnostics.Debugger.IsAttached)
			{
				Exec(gui, input, output, export, format, magnification, materialCache);
			}
			else
			{
				try
				{
					Exec(gui, input, output, export, format, magnification, materialCache);
				}
				catch (Exception e)
				{
					ExportError(e);
				}
			}
		}

		static void Exec(bool gui, string input, string output, string export, string format, float magnification, bool materialCache)
		{
			// Register UI
			GUI.Component.ParameterListComponentFactory.Register(typeof(Data.LanguageSelector), () => { return new GUI.Component.LanguageSelector(); });

			GUI.Component.ParameterListComponentFactory.Register(typeof(Data.ProcedualModelReference), () => {
				return new GUI.Component.ObjectReference<Data.ProcedualModelParameter>(Core.ProcedualModel.ProcedualModels);
			});

			// Debug
			bool isDebugMode = false;
#if DEBUG
			isDebugMode = true;
#endif
			if(System.IO.File.Exists(Path.Combine(EntryDirectory, "debug.txt")) || isDebugMode)
			{
				swig.Native.SetFileLogger(Path.Combine(GUI.Manager.GetEntryDirectory(),"Effekseer.log.txt"));
				Utils.Logger.LogPath = Path.Combine(GUI.Manager.GetEntryDirectory(), "Effekseer.Managed.log.txt");
			}

			LanguageTable.LoadTable(Path.Combine(EntryDirectory, "resources/languages/languages.txt"));

			var systemLanguage = EfkN.GetSystemLanguage();
			string language = null;

			if(systemLanguage != swig.SystemLanguage.Unknown)
			{
				if(systemLanguage == swig.SystemLanguage.Japanese)
				{
					language = "ja";
				}
				else if (systemLanguage == swig.SystemLanguage.English)
				{
					language = "en";
				}
			}
			else
			{
				language = "en";
			}

			Core.OnOutputMessage += new Action<string>(Core_OnOutputMessage);
			Core.Initialize(language);

			if (gui)
			{
				ChangeLanguage();
				LanguageTable.OnLanguageChanged += (o,e) => { ChangeLanguage(); };

				// Failed to compile script
				if (Core.ExportScripts.Count == 0)
				{
					Script.ExportScript efkpkgExporter = new Script.ExportScript(
						Script.ScriptPosition.External,
						Plugin.ExportEfkPkg.UniqueName,
						Plugin.ExportEfkPkg.Author,
						Plugin.ExportEfkPkg.Title,
						Plugin.ExportEfkPkg.Description,
						Plugin.ExportEfkPkg.Filter,
						Plugin.ExportEfkPkg.Call);
					Core.ExportScripts.Add(efkpkgExporter);

					Script.ExportScript defaultExporter = new Script.ExportScript(
						Script.ScriptPosition.External,
						Plugin.ExportDefault.UniqueName,
						Plugin.ExportDefault.Author,
						Plugin.ExportDefault.Title,
						Plugin.ExportDefault.Description,
						Plugin.ExportDefault.Filter,
						Plugin.ExportDefault.Call);
					Core.ExportScripts.Add(defaultExporter);

					Script.ImportScript efkpkgImporter = new Script.ImportScript(
						Script.ScriptPosition.External,
						Plugin.ImportEfkPkg.UniqueName,
						Plugin.ImportEfkPkg.Author,
						Plugin.ImportEfkPkg.Title,
						Plugin.ImportEfkPkg.Description,
						Plugin.ImportEfkPkg.Filter,
						Plugin.ImportEfkPkg.Call);
					Core.ImportScripts.Add(efkpkgImporter);
				}

				System.OperatingSystem os = System.Environment.OSVersion;
				swig.DeviceType deviceType = swig.DeviceType.DirectX11;

				if (!(os.Platform == PlatformID.Win32NT ||
				os.Platform == PlatformID.Win32S ||
				os.Platform == PlatformID.Win32Windows ||
				os.Platform == PlatformID.WinCE))
				{
					deviceType = swig.DeviceType.OpenGL;
				}

				if (!GUI.Manager.Initialize(960, 540, deviceType))
				{
					return;
				}
			}

			try
			{
				if (input != string.Empty)
				{
					Core.LoadFrom(input);
				}

				if (output != string.Empty)
				{
					Core.SaveTo(output);
				}

				if (export != string.Empty)
				{
					if (magnification == 0.0f)
					{
						magnification = Core.Option.Magnification;
					}

					if(format == "gltf")
					{
						var option = new Effekseer.Exporter.glTFExporterOption();
						option.Scale = magnification;
						var exporter = new Effekseer.Exporter.glTFExporter();
						exporter.Export(export, option);
					}
					else if (format == "glb")
					{
						var option = new Effekseer.Exporter.glTFExporterOption();
						option.Scale = magnification;
						option.Format = Exporter.glTFExporterFormat.glb;
						var exporter = new Effekseer.Exporter.glTFExporter();
						exporter.Export(export, option);
					}
					else
					{
						var binaryExporter = new Binary.Exporter();
						var binary = binaryExporter.Export(Core.Root, magnification);
						System.IO.File.WriteAllBytes(export, binary);
					}
				}
			}
			catch (Exception e)
			{
				System.Console.Error.WriteLine(e.Message);
			}

			try
			{
				if (materialCache)
				{
					IO.MaterialCacheGenerator.GenerateMaterialCaches();
				}
			}
			catch (Exception e)
			{
				System.Console.Error.WriteLine(e.Message);
			}

			if (gui)
			{
				while (GUI.Manager.NativeManager.DoEvents())
				{
					GUI.Manager.Update();
				}

				GUI.Manager.Terminate();
				Process.MaterialEditor.Terminate();
			}

			Core.Dispose();
		}

		private static void ChangeLanguage()
		{
			MultiLanguageTextProvider.RootDirectory = GUI.Manager.GetEntryDirectory() + "/";
			MultiLanguageTextProvider.Reset();
			MultiLanguageTextProvider.LoadCSV("Base.csv");
			MultiLanguageTextProvider.LoadCSV("Effekseer.csv");
			MultiLanguageTextProvider.LoadCSV("Effekseer_Options.csv");
			MultiLanguageTextProvider.LoadCSV("Effekseer_BasicSettings.csv");
			MultiLanguageTextProvider.LoadCSV("Effekseer_Position.csv");
			MultiLanguageTextProvider.LoadCSV("Effekseer_Rotation.csv");
			MultiLanguageTextProvider.LoadCSV("Effekseer_Scale.csv");
			MultiLanguageTextProvider.LoadCSV("Effekseer_BasicRenderSettings.csv");
			MultiLanguageTextProvider.LoadCSV("Effekseer_RenderSettings.csv");
			MultiLanguageTextProvider.LoadCSV("Effekseer_LocalForceField.csv");
			MultiLanguageTextProvider.LoadCSV("Effekseer_AdvancedRenderCommon.csv");
			MultiLanguageTextProvider.LoadCSV("Effekseer_Environment.csv");
			MultiLanguageTextProvider.LoadCSV("Effekseer_ProcedualModel.csv");

			GUI.Manager.UpdateFont();
		}

		static void Core_OnOutputMessage(string obj)
		{
			swig.GUIManager.show(obj, "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);
		}

		static void ExportError(Exception e)
		{
			string messageBase = "Error has been caused.";

			if (e is UnauthorizedAccessException)
			{
				if (Core.Language == Language.Japanese)
				{
					messageBase = "アクセスが拒否されエラーが発生しました。\n他のディレクトリにインストールしてください。\nもしくはアクセスできないファイルを選択しています。\n";
				}
				else
				{
					messageBase = "Access is denied and an error occurred. \nPlease install it in another directory. \nOr you have selected a file that you cannot access.\n";
				}
			}
			else
			{
				if (Core.Language == Language.Japanese)
				{
					messageBase = "エラーが発生しました。";
				}
			}

			DateTime dt = DateTime.Now;
			var filename = string.Format("error_{0:D4}_{1:D2}_{2:D2}_{3:D2}_{4:D2}_{5:D2}.txt", dt.Year, dt.Month, dt.Day, dt.Hour, dt.Minute, dt.Second);
			var filepath = Path.Combine(EntryDirectory, filename);

			try
			{
				System.IO.File.WriteAllText(filepath, e.ToString());

				string message = messageBase + "Error log is written in " + filepath + "\nWe are glad if you send this error to Effekseer with a mail or twitter.\n";

				if (Core.Language == Language.Japanese)
				{
					message = messageBase + "エラーログが" + filepath + "に出力されました。\nもしエラーをメールやTwitterでEffekseerに送っていただけると助かります。\n";
				}
				swig.GUIManager.show(message, "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);
			}
			catch (Exception e2)
			{
				string message = messageBase;

				message += e.ToString();
				message += "\n";
				message += e2.ToString();
				swig.GUIManager.show(message, "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);
			}
		}
	}

	class Images
	{
		public static swig.ImageResource Play;
		public static swig.ImageResource Stop;
		public static swig.ImageResource Pause;
		public static swig.ImageResource Step;
		public static swig.ImageResource BackStep;
		
		public static Dictionary<string, swig.ImageResource> Icons = new Dictionary<string, swig.ImageResource>();

		public static Dictionary<string, swig.ImageResource> tempImages = new Dictionary<string, swig.ImageResource>();

		public static swig.ImageResource Load(swig.Native native, string path, bool isRequiredToReload = false)
		{
			if(tempImages.ContainsKey(path) && !isRequiredToReload)
			{
				return tempImages[path];
			}
			else
			{
				if (tempImages.ContainsKey(path))
				{
					tempImages[path].Invalidate();
					tempImages[path].Validate();
					return tempImages[path];
				}

				var img = native.LoadImageResource(path);
				if(img != null)
				{
					tempImages.Add(path, img);
				}
				return img;
			}
		}

		public static swig.ImageResource LoadAppResource(swig.Native native, string path)
		{
			string appDirectory = GUI.Manager.GetEntryDirectory();
			string fullPath = Path.Combine(appDirectory, path);
			return native.LoadImageResource(fullPath);
		}

		public static void Load(swig.Native native)
		{
			Play = LoadAppResource(native, "resources/Play.png");
			Stop = LoadAppResource(native, "resources/Stop.png");
			Pause = LoadAppResource(native, "resources/Pause.png");
			Step = LoadAppResource(native, "resources/Step.png");
			BackStep = LoadAppResource(native, "resources/BackStep.png");

			Icons["Copy"] = LoadAppResource(native, "resources/icons/Copy.png");
			Icons["Paste"] = LoadAppResource(native, "resources/icons/Paste.png");

			Icons["AppIcon"] = LoadAppResource(native, "resources/icon.png");

			Icons["VisibleShow"] = LoadAppResource(native, "resources/icons/Visible_Show.png");
			Icons["VisibleHide"] = LoadAppResource(native, "resources/icons/Visible_Hide.png");

			Icons["EnlargeAnchor"] = LoadAppResource(native, "resources/icons/EnlargeAnchor.png");
			Icons["ShrinkAnchor"] = LoadAppResource(native, "resources/icons/ShrinkAnchor.png");

			Icons["EnlargeAnchor"] = LoadAppResource(native, "resources/icons/EnlargeAnchor.png");
			Icons["ShrinkAnchor"] = LoadAppResource(native, "resources/icons/ShrinkAnchor.png");

			Icons["AutoZoom_On"] = LoadAppResource(native, "resources/icons/AutoZoom_On.png");
			Icons["AutoZoom_Off"] = LoadAppResource(native, "resources/icons/AutoZoom_Off.png");

			Icons["FileViewer_Directory"] = LoadAppResource(native, "resources/icons/FileViewer_Directory.png");
			Icons["FileViewer_EffekseerProj"] = LoadAppResource(native, "resources/icons/FileViewer_EffekseerProj.png");

			Icons["ButtonMin"] = LoadAppResource(native, "resources/icons/Button_Min.png");
			Icons["ButtonMax"] = LoadAppResource(native, "resources/icons/Button_Max.png");
			Icons["ButtonMaxCancel"] = LoadAppResource(native, "resources/icons/Button_MaxCancel.png");
			Icons["ButtonClose"] = LoadAppResource(native, "resources/icons/Button_Close.png");
		}

		public static void Unload()
		{
			Play.Dispose();
			Stop.Dispose();
			Pause.Dispose();
			Step.Dispose();
			BackStep.Dispose();
			foreach (var keyValue in Icons)
			{
				if (keyValue.Value != null)
				{
					keyValue.Value.Dispose();
				}
			}

			foreach (var keyValue in tempImages)
			{
				if (keyValue.Value != null)
				{
					keyValue.Value.Dispose();
				}
			}
		}

		public static swig.ImageResource GetIcon(string resourceName) {
			return Icons.ContainsKey(resourceName) ? Icons[resourceName] : null;
		}
	}
}
