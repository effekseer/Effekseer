using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace Effekseer.Process
{
	public class MaterialEditor
	{
		static System.Diagnostics.Process process = null;
		static swig.ProcessConnector processConnector = null;
		const string LaunchLogFileName = "EffekseerMaterialEditor.launch.log.txt";

		public static bool Run()
		{
			if (IsRunning)
				return true;

			try
			{
				var app = new ProcessStartInfo();

				string appDirectory = GUI.Manager.GetEntryDirectory();
				string executableName = "EffekseerMaterialEditor";
				if (Environment.OSVersion.Platform == PlatformID.Win32NT)
				{
					executableName += ".exe";
				}
				string fullPath = System.IO.Path.Combine(appDirectory, executableName);

				if (!System.IO.File.Exists(fullPath))
				{
					ShowLaunchError("Failed to start EffekseerMaterialEditor. The executable was not found.\n" + fullPath);
					return false;
				}

				app.FileName = fullPath;
				app.UseShellExecute = true;
				app.WorkingDirectory = appDirectory;
				app.Arguments = "ipc -l " + QuoteArgument(LanguageTable.Languages[LanguageTable.SelectedIndex]);

				process = System.Diagnostics.Process.Start(app);
				if (process == null)
				{
					ShowLaunchError("Failed to start EffekseerMaterialEditor. Process.Start returned null.\n" + fullPath);
					return false;
				}

				if (process.WaitForExit(500))
				{
					int exitCode = 0;
					try
					{
						exitCode = process.ExitCode;
					}
					catch
					{
					}

					ShowLaunchError("EffekseerMaterialEditor exited immediately. ExitCode: " + exitCode + "\n" + fullPath);
					process = null;
					return false;
				}

				return IsRunning;
			}
			catch (Exception e)
			{
				ShowLaunchError("Failed to start EffekseerMaterialEditor.\n" + e.ToString());
				process = null;
				return false;
			}
		}

		public static void Update()
		{
			if (process != null && process.HasExited)
			{
				LogLaunchMessage("EffekseerMaterialEditor exited. ExitCode: " + process.ExitCode);
				process = null;
			}

			if (processConnector != null)
			{
				processConnector.Update();
			}
		}

		public static void Init()
		{
			if (processConnector == null)
			{
				processConnector = new swig.ProcessConnector();
			}
		}


		public static void Terminate()
		{
			if (!IsRunning)
				return;
			processConnector.TerminateMaterialEditor();
		}

		public static bool IsRunning
		{
			get
			{
				if (processConnector == null) return false;
				if (process == null) return false;
				try
				{
					return !process.HasExited;
				}
				catch
				{
					return false;
				}
			}
		}

		public static void OpenOrCreateMaterial(string path)
		{
			if (!IsRunning)
			{
				ShowLaunchError("EffekseerMaterialEditor is not running. The material open command was not sent.\n" + path);
				return;
			}

			try
			{
				processConnector.OpenOrCreateMaterial(path);
			}
			catch (Exception e)
			{
				ShowLaunchError("Failed to send a material open command to EffekseerMaterialEditor.\n" + e.ToString());
				return;
			}

			if (Environment.OSVersion.Platform == PlatformID.Win32NT)
			{
				var handle = process.MainWindowHandle;
				if (handle != IntPtr.Zero)
				{
					if (IsIconic(handle))
						ShowWindow(handle, 9);
					else
						SetForegroundWindow(handle);
				}
			}
		}

		static string QuoteArgument(string value)
		{
			if (value == null)
			{
				return "\"\"";
			}

			return "\"" + value.Replace("\\", "\\\\").Replace("\"", "\\\"") + "\"";
		}

		static void ShowLaunchError(string message)
		{
			LogLaunchMessage(message);
			try
			{
				swig.GUIManager.show(message, "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);
			}
			catch
			{
			}
		}

		static void LogLaunchMessage(string message)
		{
			try
			{
				var line = DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss.fff") + " " + message;

				if (!string.IsNullOrEmpty(Utils.Logger.LogPath))
				{
					Utils.Logger.Write(line);
				}

				var appDirectory = GUI.Manager.GetEntryDirectory();
				var logPath = System.IO.Path.Combine(appDirectory, LaunchLogFileName);
				System.IO.File.AppendAllText(logPath, line + Environment.NewLine);
			}
			catch
			{
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
