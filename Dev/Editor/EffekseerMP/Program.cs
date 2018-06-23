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

            Core.Initialize(language);

			if(Core.Language == Language.Japanese)
			{
				Resources.LoadLanguageFile("resources/lang_ja.txt");
			}
			if (Core.Language == Language.English)
			{
				Resources.LoadLanguageFile("resources/lang_en.txt");
			}

			if (!GUI.Manager.Initialize(960,540))
			{
				return;
			}

			//Images.Load(GUI.Manager.Native);

            //GUI.Manager.AddControl(new DebugMenu());

			//GUI.Manager.AddControl(new TestWindow());

			while (GUI.Manager.NativeManager.DoEvents())
			{
				GUI.Manager.Update();
			}

			//Images.Unload();

			GUI.Manager.Terminate();

			Core.Dispose();
		}
	}

	class TestWindow : GUI.IRemovableControl
	{
		bool opened = true;

		public bool ShouldBeRemoved { get; private set; } = false;

		bool[] open = new[] { true };
		int[] currentTime = new[] { 0 };
		int[] currentMin = new[] { 0 };
		int[] currentMax = new[] { 100 };

		bool isButton = true;

		public void Update()
		{
			var mgr = GUI.Manager.NativeManager;

			if (mgr.Begin("ViewerController", ref opened))
			{

				mgr.SliderInt("Timeline", currentTime, currentMin[0], currentMax[0]);
				mgr.Separator();
				mgr.PushItemWidth(200);
				mgr.DragIntRange2("Range", currentMin, currentMax, 1.0f, 0, 1200);
				mgr.PopItemWidth();
				mgr.SameLine();
				mgr.Button("Back_");
				mgr.SameLine();
				mgr.Button("Next");
				mgr.SameLine();

				//mgr.Image(img, 48, 21);

				if (isButton)
				{
					if (mgr.Button("Back"))
					{
						var messageBox = new GUI.Dialog.About();
						messageBox.Show();
					}
				}
				else
				{
					mgr.Text("Back");
				}

				if (mgr.BeginPopupContextItem("aaaaaaa"))
				{
					if (mgr.Button("Change"))
					{
						isButton = !isButton;
					}
					mgr.EndPopup();
				}

				mgr.SameLine();

				if (mgr.Button("Play"))
				{
					var messageBox = new GUI.Dialog.MessageBox();
					messageBox.Show("TestTitle", "TestMessage");
					//mgr.OpenPopup("###AA");   
				}

				if (mgr.TreeNode("AAA"))
				{
					mgr.Text("aa");

					if (mgr.TreeNode("BBB"))
					{
						mgr.Text("bb");
						mgr.Button("cc");
						mgr.TreePop();
					}

					mgr.TreePop();
				}


				//if (mgr.BeginPopupModal("Message###AA", null, swig.WindowFlags.AlwaysAutoResize))
				//{
				//    mgr.Text("testaaaaaaa");
				//    mgr.EndPopup();
				//}
			}
			mgr.End();
		}
	}

	class DebugMenu : GUI.IRemovableControl
	{
		bool opened = true;

		public bool ShouldBeRemoved { get; private set; } = false;

		public void Update()
		{
			if(GUI.Manager.NativeManager.Begin("DebugMenu", ref opened))
			{
				if(GUI.Manager.NativeManager.Button("About"))
				{
					var messageBox = new GUI.Dialog.About();
					messageBox.Show();
				}

				if (GUI.Manager.NativeManager.Button("MessageBox"))
				{
					var messageBox = new GUI.Dialog.MessageBox();
					messageBox.Show("Title", "Message");
				}

				if(GUI.Manager.NativeManager.Button("NodeTreeView"))
				{
					var nodeTreeView = new GUI.Dock.NodeTreeView();
					nodeTreeView.Renew();
					GUI.Manager.AddControl(nodeTreeView);
				}

				if (GUI.Manager.NativeManager.Button("Common"))
				{
					var window = new GUI.Dock.CommonValues();
					GUI.Manager.AddControl(window);
				}

				if (GUI.Manager.NativeManager.Button("Location"))
				{
					var window = new GUI.Dock.LocationValues();
					GUI.Manager.AddControl(window);
				}
			}

			GUI.Manager.NativeManager.End();
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
			string appDirectory = Path.GetDirectoryName(Assembly.GetEntryAssembly().Location);
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
