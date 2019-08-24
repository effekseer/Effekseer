using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;

namespace Effekseer.Process
{
	class MaterialEditor
	{
		static System.Diagnostics.Process process = null;

		public static void Run()
		{
			if (IsRunning)
				return;

			var app = new ProcessStartInfo();

			string appDirectory = GUI.Manager.GetEntryDirectory();
			string fullPath = System.IO.Path.Combine(appDirectory, "EffekseerMaterialEditor");

			app.FileName = fullPath;
			app.UseShellExecute = true;

			process = System.Diagnostics.Process.Start(app);
		}

		public static void Terminate()
		{
			if (!IsRunning)
				return;
			GUI.Manager.Native.TerminateMaterialEditor();
		}

		public static bool IsRunning
		{
			get
			{
				if (process == null) return false;
				return !process.HasExited;
			}
		}

		public static void OpenOrCreateMaterial(string path)
		{
			GUI.Manager.Native.OpenOrCreateMaterial(path);
		}
    }
}
