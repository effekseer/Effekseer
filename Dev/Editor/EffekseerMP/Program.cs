using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

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
            Core.Initialize();

            var mgr = new swig.GUIManager();
			if(mgr.Initialize("Effekseer", 960, 540, false))
			{
			}
			else
			{
				mgr.Dispose();
				mgr = null;
				return;
			}

			var native = new swig.Native();
			if(!native.CreateWindow_Effekseer(mgr.GetNativeHandle(), 960, 540, false, true))
			{
				native.Dispose();
				native = null;

				mgr.Terminate();
				mgr.Dispose();
				mgr = null;
				return;
			}

			Resources.Load(native);

			var open = new[] { true };
			var currentTime = new[] { 0 };
			var currentMin = new[] { 0 };
			var currentMax = new[] { 100 };

            GUI.Manager.NativeManager = mgr;

            var mainMenu = new GUI.Menu.MainMenu();
            var menu = new GUI.Menu.Menu();
            menu.Label = "Nyan";
            mainMenu.Controls.Add(menu);

            bool isButton = true;

            GUI.Manager.Controls.Add(mainMenu);
            GUI.Manager.Controls.Add(new DebugMenu());
            //var img = native.LoadImageResource("resources/Play.png");

			while (mgr.DoEvents())
			{
				native.UpdateWindow();

				mgr.ResetGUI();

				if (mgr.Begin("ViewerController", open))
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

					mgr.End();
				}

				GUI.Manager.Update();

				mgr.RenderGUI();

				native.Present();
				mgr.Present();
			}

			Resources.Unload();

			native.DestroyWindow();
			native.Dispose();
			native = null;

			mgr.Terminate();
			mgr.Dispose();
			mgr = null;

            Core.Dispose();
		}
	}

	class DebugMenu : GUI.IRemovableControl
	{
		bool[] opened = new[] { true };

		public bool ShouldBeRemoved { get; private set; } = false;

		public void Update()
		{
			if(GUI.Manager.NativeManager.Begin("DebugMenu", opened))
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

				GUI.Manager.NativeManager.End();
			}
		}
	}

	class Resources
	{
		public static swig.ImageResource Icon;

		public static void Load(swig.Native native)
		{
			Icon = native.LoadImageResource("resources/icon.png");
		}

		public static void Unload()
		{
			Icon.Dispose();
		}
	}
}
