using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace Effekseer.Process
{
	class MaterialEditor
	{
		static System.Diagnostics.Process process = null;

		public static bool Run()
		{
			if (IsRunning)
				return true;

			try
			{
				var app = new ProcessStartInfo();

				string appDirectory = GUI.Manager.GetEntryDirectory();
				string fullPath = System.IO.Path.Combine(appDirectory, "EffekseerMaterialEditor");

				app.FileName = fullPath;
				app.UseShellExecute = true;
				app.Arguments = "ipc";

				process = System.Diagnostics.Process.Start(app);

				return IsRunning;
			}
			catch
			{
				process = null;
				return false;
			}
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

			if (Environment.OSVersion.Platform == PlatformID.Win32NT)
			{
				if (IsIconic(process.MainWindowHandle))
					ShowWindow(process.MainWindowHandle, 9);
				else
					SetForegroundWindow(process.MainWindowHandle);
			}
		}

		[DllImport("user32.dll")]
		private static extern bool SetForegroundWindow(IntPtr hWnd);
		[DllImport("user32.dll")]
		[return: MarshalAs(UnmanagedType.Bool)]
		static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);
		[DllImport("user32.dll")]
		[return: MarshalAs(UnmanagedType.Bool)]
		static extern bool IsIconic(IntPtr hWnd);
	}
}
