using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;

namespace Effekseer.GUI.BindableComponent
{
	class PathForModel : PathBase
	{
		public PathForModel()
		{
			fileType = FileType.Model;
			filter = MultiLanguageTextProvider.GetText("ModelFilter");
		}

		protected override void UpdateSubParts(float width)
		{
			if (!string.IsNullOrEmpty(filePath))
			{
				var ext = System.IO.Path.GetExtension(filePath).ToLower();

				if (ext != ".efkmodel")
				{
					if (Manager.NativeManager.Button(MultiLanguageTextProvider.GetText("ResetMaginification")))
					{
						btn_reload_Click();
					}
				}
			}
		}

		private void btn_reload_Click()
		{
			if (binding == null) return;
			var filepath = binding.AbsolutePath;
			if (string.IsNullOrEmpty(filepath)) return;
			LoadFile(filepath, true);

			Read();
		}

		protected override void LoadFile(string filepath, bool isReloading)
		{
			// Convert file
			var ext = System.IO.Path.GetExtension(filepath).ToLower();
			var newFilepath = System.IO.Path.ChangeExtension(filepath, ".efkmodel");

			Effekseer.Utils.ModelInformation modelInfo = new Utils.ModelInformation();
			if (modelInfo.Load(newFilepath))
			{
				if (!isReloading)
				{
					binding.SetAbsolutePath(filepath);
					System.IO.Directory.SetCurrentDirectory(System.IO.Path.GetDirectoryName(filepath));
					return;
				}
			}
			else
			{
				// Remove invalid file
				if (System.IO.File.Exists(newFilepath))
				{
					try
					{
						System.IO.File.Delete(newFilepath);
					}
					catch
					{

					}
				}
			}

			Dialog.OpenModel omd = new Dialog.OpenModel(modelInfo.Scale);

			if (CheckExtension(ext) && ext != ".efkmodel")
			{
				omd.Show("");
			}
			else
			{
				binding.SetAbsolutePath(filepath);

				System.IO.Directory.SetCurrentDirectory(System.IO.Path.GetDirectoryName(filepath));

				Manager.Viewer.Reload(true);
			}

			omd.OnOK = () =>
			{

				if (CheckExtension(ext) && ext != ".efkmodel")
				{
					var oldFilepath = filepath;
					bool doGenerate = false;

					if (!System.IO.File.Exists(newFilepath) ||
						System.IO.File.GetLastWriteTime(oldFilepath) != System.IO.File.GetLastWriteTime(newFilepath) ||
						modelInfo.Scale != omd.Magnification)
					{
						doGenerate = true;
					}

					if (doGenerate)
					{
						string converterPath = Manager.GetEntryDirectory() + "/tools/EffekseerResourceConverter";

						// japanese file path is not supported.
						try
						{
							string tempFilePath = Path.GetTempPath() + System.IO.Path.GetFileName(filepath);
							System.IO.File.Copy(oldFilepath, tempFilePath);
							oldFilepath = tempFilePath;
						}
						catch
						{

						}


						System.Diagnostics.ProcessStartInfo info = new System.Diagnostics.ProcessStartInfo();

						var os = System.Environment.OSVersion;
						if (os.Platform == PlatformID.Win32NT ||
							os.Platform == PlatformID.Win32S ||
							os.Platform == PlatformID.Win32Windows ||
							os.Platform == PlatformID.WinCE)
						{
							converterPath += ".exe";
						}

						if (os.Platform == PlatformID.Unix && os.Platform != PlatformID.MacOSX)
						{
							string pathvar = System.Environment.GetEnvironmentVariable("LD_LIBRARY_PATH");
							info.EnvironmentVariables["LD_LIBRARY_PATH"] = pathvar + ";" + Manager.GetEntryDirectory() + "/tools/";
						}

						info.FileName = converterPath;

						info.Arguments = "\"" + oldFilepath + "\" -o \"" + newFilepath + "\" -s " + omd.Magnification.ToString();

						if (!System.IO.File.Exists(oldFilepath))
						{
							var msg = oldFilepath + " is not found.";

							swig.GUIManager.show(msg, "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);
							return;
						}

						if (!System.IO.File.Exists(converterPath))
						{
							var msg = converterPath + " is not found.";

							swig.GUIManager.show(msg, "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);
							return;
						}

						info.UseShellExecute = false;
						info.RedirectStandardOutput = true;
						info.RedirectStandardError = true;
						info.RedirectStandardInput = false;
						info.CreateNoWindow = true;

						System.Diagnostics.Process p = System.Diagnostics.Process.Start(info);

						string outputs = p.StandardOutput.ReadToEnd();
						string errors = p.StandardError.ReadToEnd();

						p.WaitForExit();
						p.Dispose();


						if (System.IO.File.Exists(newFilepath))
						{
							System.IO.File.SetLastWriteTime(newFilepath, System.IO.File.GetLastWriteTime(oldFilepath));
						}
						else
						{
							var msg = " Failed to load. \n" + outputs + errors;

							swig.GUIManager.show(msg, "Error", swig.DialogStyle.Error, swig.DialogButtons.OK);
						}

						try
						{
							string tempFilePath = Path.GetTempPath() + System.IO.Path.GetFileName(filepath);
							System.IO.File.Delete(tempFilePath);
						}
						catch
						{

						}
					}
				}

				binding.SetAbsolutePath(filepath);

				System.IO.Directory.SetCurrentDirectory(System.IO.Path.GetDirectoryName(filepath));

				Manager.Viewer.Reload(true);

				Read();
			};
		}

	}
}