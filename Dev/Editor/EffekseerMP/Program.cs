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

			while(mgr.DoEvents())
			{
				native.UpdateWindow();

				mgr.ResetGUI();

				mgr.Begin("Test", open);
				mgr.Text("Effekseer");
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
