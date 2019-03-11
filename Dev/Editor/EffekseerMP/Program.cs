using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Reflection;

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

		[STAThread]
		static void Main(string[] args)
		{
			StartDirectory = System.IO.Directory.GetCurrentDirectory();

			bool gui = true;
			string input = string.Empty;
			string output = string.Empty;
			string export = string.Empty;
			float magnification = 0.0f;

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
				else
				{
					input = args[i];
				}
			}

			if (System.Diagnostics.Debugger.IsAttached)
			{
				Exec(gui, input, output, export, magnification);
			}
			else
			{
				try
				{
					Exec(gui, input, output, export, magnification);
				}
				catch (Exception e)
				{
					System.IO.File.WriteAllText("error.txt", e.ToString());
				}
			}
		}

		static void Exec(bool gui, string input, string output, string export, float magnification)
		{
			var languageIndex = swig.GUIManager.GetLanguage();
			Language? language = null;

			if(languageIndex >= 0)
			{
				language = (Language)languageIndex;
			}

			Core.OnOutputMessage += new Action<string>(Core_OnOutputMessage);
			Core.Initialize(language);

			if (gui)
			{
				// Failed to compile script
				if (Core.ExportScripts.Count == 0)
				{
					Script.ExportScript script = new Script.ExportScript(
						Script.ScriptPosition.External,
						Plugin.ExportDefault.UniqueName,
						Plugin.ExportDefault.Author,
						Plugin.ExportDefault.Title,
						Plugin.ExportDefault.Description,
						Plugin.ExportDefault.Filter,
						Plugin.ExportDefault.Call);
					Core.ExportScripts.Add(script);
				}

				{
					var appDirectory = GUI.Manager.GetEntryDirectory();
					if (Core.Language == Language.Japanese)
					{
						var fullPath = Path.Combine(appDirectory, "resources/lang_ja.txt");
						Resources.LoadLanguageFile(fullPath);
					}
					if (Core.Language == Language.English)
					{
						var fullPath = Path.Combine(appDirectory, "resources/lang_en.txt");
						Resources.LoadLanguageFile(fullPath);
					}
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

					var binary = Binary.Exporter.Export(magnification);
					System.IO.File.WriteAllBytes(export, binary);
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
			}

			Core.Dispose();
		}

		static void Core_OnOutputMessage(string obj)
		{
			swig.GUIManager.show(obj, "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);
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

		public static swig.ImageResource Load(swig.Native native, string path)
		{
			if(tempImages.ContainsKey(path))
			{
				return tempImages[path];
			}
			else
			{
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

			Icons["AppIcon"] = LoadAppResource(native, "resources/icon.png");
			Icons["NodeEmpty"] = LoadAppResource(native, "resources/icons/NodeType_Empty.png");
			Icons["NodeModel"] = LoadAppResource(native, "resources/icons/NodeType_Model.png");
			Icons["NodeRibbon"] = LoadAppResource(native, "resources/icons/NodeType_Ribbon.png");
			Icons["NodeRing"] = LoadAppResource(native, "resources/icons/NodeType_Ring.png");
			Icons["NodeSprite"] = LoadAppResource(native, "resources/icons/NodeType_Sprite.png");
			Icons["NodeTrack"] = LoadAppResource(native, "resources/icons/NodeType_Track.png");

			Icons["VisibleShow"] = LoadAppResource(native, "resources/icons/Visible_Show.png");
			Icons["VisibleHide"] = LoadAppResource(native, "resources/icons/Visible_Hide.png");

			Icons["PanelBehavior"] = LoadAppResource(native, "resources/icons/Panel_Behavior.png");
			Icons["PanelCommon"] = LoadAppResource(native, "resources/icons/Panel_Common.png");
			Icons["PanelCulling"] = LoadAppResource(native, "resources/icons/Panel_Culling.png");
			Icons["PanelFCurve"] = LoadAppResource(native, "resources/icons/Panel_FCurve.png");
			Icons["PanelFileViewer"] = LoadAppResource(native, "resources/icons/Panel_FileViewer.png");
			Icons["PanelGenerationLocation"] = LoadAppResource(native, "resources/icons/Panel_GenerationLocation.png");
			Icons["PanelLocation"] = LoadAppResource(native, "resources/icons/Panel_Location.png");
			Icons["PanelLocationAbs"] = LoadAppResource(native, "resources/icons/Panel_LocationAbs.png");
			Icons["PanelNetwork"] = LoadAppResource(native, "resources/icons/Panel_Network.png");
			Icons["PanelNodeTree"] = LoadAppResource(native, "resources/icons/Panel_NodeTree.png");
			Icons["PanelOption"] = LoadAppResource(native, "resources/icons/Panel_Option.png");
			Icons["PanelRecorder"] = LoadAppResource(native, "resources/icons/Panel_Recorder.png");
			Icons["PanelRenderer"] = LoadAppResource(native, "resources/icons/Panel_Renderer.png");
			Icons["PanelRendererCommon"] = LoadAppResource(native, "resources/icons/Panel_RendererCommon.png");
			Icons["PanelRotation"] = LoadAppResource(native, "resources/icons/Panel_Rotation.png");
			Icons["PanelScale"] = LoadAppResource(native, "resources/icons/Panel_Scale.png");
			Icons["PanelSound"] = LoadAppResource(native, "resources/icons/Panel_Sound.png");
			Icons["PanelViewer"] = LoadAppResource(native, "resources/icons/Panel_Viewer.png");
			Icons["PanelViewPoint"] = LoadAppResource(native, "resources/icons/Panel_ViewPoint.png");
			Icons["PanelDepth"] = LoadAppResource(native, "resources/icons/Panel_Depth.png");
			Icons["PanelGlobal"] = LoadAppResource(native, "resources/icons/Panel_Global.png");

			Icons["EnlargeAnchor"] = LoadAppResource(native, "resources/icons/EnlargeAnchor.png");
			Icons["ShrinkAnchor"] = LoadAppResource(native, "resources/icons/ShrinkAnchor.png");

			Icons["EnlargeAnchor"] = LoadAppResource(native, "resources/icons/EnlargeAnchor.png");
			Icons["ShrinkAnchor"] = LoadAppResource(native, "resources/icons/ShrinkAnchor.png");

			Icons["AutoZoom_On"] = LoadAppResource(native, "resources/icons/AutoZoom_On.png");
			Icons["AutoZoom_Off"] = LoadAppResource(native, "resources/icons/AutoZoom_Off.png");

			Icons["FileViewer_Directory"] = LoadAppResource(native, "resources/icons/FileViewer_Directory.png");
			Icons["FileViewer_EffekseerProj"] = LoadAppResource(native, "resources/icons/FileViewer_EffekseerProj.png");
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
