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

            var messageBoxOpened = new[] { false };

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
				mgr.Button("Back");
				mgr.SameLine();
				mgr.Button("Next");
				mgr.SameLine();
				mgr.Button("Back");
				mgr.SameLine();

				if(mgr.Button("Play"))
                {
                    mgr.OpenPopup("Message##AA");   
                }

                if (mgr.BeginPopupModal("Message##AA", null, swig.WindowFlags.AlwaysAutoResize))
                {
                    mgr.Text("testaaaaaaa");
                    mgr.EndPopup();
                }

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
		}
	}
}
