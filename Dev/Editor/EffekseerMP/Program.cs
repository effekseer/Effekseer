using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer
{
	class Program
	{
		static void Main(string[] args)
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

            var noteTreeView = new GUI.Dock.NodeTreeView();
            noteTreeView.Renew();

            GUI.Manager.Controls.Add(mainMenu);
            GUI.Manager.Controls.Add(noteTreeView);
            //var img = native.LoadImageResource("resources/Play.png");

			while (mgr.DoEvents())
			{
				native.UpdateWindow();

				mgr.ResetGUI();

				mgr.Begin("ViewerController", open);
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

                if(isButton)
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

				if(mgr.Button("Play"))
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

                GUI.Manager.Update();

                mgr.End();

				mgr.RenderGUI();

				native.Present();
				mgr.Present();
			}

			native.DestroyWindow();
			native.Dispose();
			native = null;

			mgr.Terminate();
			mgr.Dispose();
			mgr = null;

            Core.Dispose();
		}
	}
}
