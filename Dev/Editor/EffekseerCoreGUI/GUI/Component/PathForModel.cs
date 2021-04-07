using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Text.RegularExpressions;

namespace Effekseer.GUI.Component
{
	class PathForModel : Control, IParameterControl
	{
		string id1 = "";
		string id2 = "";
		string id3 = "";

		Data.Value.PathForModel binding = null;

		string filePath = string.Empty;
		bool isHovered = false;

		public bool EnableUndo { get; set; } = true;

		public Data.Value.PathForModel Binding
		{
			get
			{
				return binding;
			}
			set
			{
				if (binding == value) return;

				binding = value;

				Read();
			}
		}

		public PathForModel()
		{
			id1 = "###" + Manager.GetUniqueID().ToString();
			id2 = "###" + Manager.GetUniqueID().ToString();
			id3 = "###" + Manager.GetUniqueID().ToString();
		}

		public void SetBinding(object o)
		{
			var o_ = o as Data.Value.PathForModel;
			Binding = o_;
		}

		public void FixValue()
		{
		}

		public override void OnDisposed()
		{
		}

		public override void OnDropped(string path, ref bool handle)
		{
			if (isHovered)
			{
				if (CheckExtension(path))
				{
					binding.SetAbsolutePath(path);
					Read();
				}

				handle = true;
			}
		}

		public override void Update()
		{
			isHovered = false;

			if (binding == null) return;

			string dd = null;

			float buttonSizeX = Manager.NativeManager.GetTextLineHeightWithSpacing() * 2;

			if (Manager.NativeManager.Button(Resources.GetString("Load") + id1, buttonSizeX))
			{
				btn_load_Click();
			}

			if (dd == null) dd = DragAndDrops.UpdateFileDst(DragAndDrops.FileType.Model);

			isHovered = isHovered || Manager.NativeManager.IsItemHovered();

			Manager.NativeManager.SameLine();

			Manager.NativeManager.Text(filePath);

			if (dd == null) dd = DragAndDrops.UpdateFileDst(DragAndDrops.FileType.Model);

			if (Manager.NativeManager.IsItemHovered())
			{
				Manager.NativeManager.SetTooltip(filePath);
			}

			isHovered = isHovered || Manager.NativeManager.IsItemHovered();
			
			if (filePath != string.Empty)
			{
				if (Manager.NativeManager.Button(Resources.GetString("Delete") + id2, buttonSizeX))
				{
					btn_delete_Click();
				}

				Manager.NativeManager.SameLine();

				isHovered = isHovered || Manager.NativeManager.IsItemHovered();

				if (Manager.NativeManager.Button(Resources.GetString("ResetMaginification") + id3, buttonSizeX * 2))
				{
					btn_reload_Click();
				}

				isHovered = isHovered || Manager.NativeManager.IsItemHovered();
			}

			if (dd != null)
			{
				Dropped(dd);
			}
		}

		public void Dropped(string path)
		{
			if (CheckExtension(path))
			{
				LoadFile(path, false);
				Read();
			}
		}

		private void btn_load_Click()
		{
			if (binding == null) return;

			var filter = Resources.GetString("ModelFilter");
			var result = swig.FileDialog.OpenDialog(filter, System.IO.Directory.GetCurrentDirectory());

			if (!string.IsNullOrEmpty(result))
			{
				var filepath = result;
				/*
				OpenFileDialog ofd = new OpenFileDialog();

				ofd.InitialDirectory = System.IO.Directory.GetCurrentDirectory();
				ofd.Filter = binding.Filter;
				ofd.FilterIndex = 2;
				ofd.Multiselect = false;

				if (ofd.ShowDialog() == DialogResult.OK)
				{
				var filepath = ofd.FileName;
					*/

				LoadFile(filepath, false);
			}
			else
			{
				return;
			}

			Read();
		}

		private void btn_delete_Click()
		{
			if (binding == null) return;
			binding.SetAbsolutePath(string.Empty);

			Read();
		}

		private void btn_reload_Click()
		{
			if (binding == null) return;
			var filepath = binding.AbsolutePath;
			if (string.IsNullOrEmpty(filepath)) return;
			LoadFile(filepath, true);

			Read();
		}

		void Read()
		{
			if (binding != null)
			{
				filePath = binding.GetRelativePath();
			}
			else
			{
				filePath = string.Empty;
			}
		}

		private bool CheckExtension(string path)
		{
			var filters = Resources.GetString("ModelFilter").Split(',');
			return filters.Any(_ => "." + _ == System.IO.Path.GetExtension(path).ToLower());
		}

		void LoadFile(string filepath, bool isReloading)
		{

			// Convert file
			var ext = System.IO.Path.GetExtension(filepath).ToLower().Replace(".", "");
			var newFilepath = System.IO.Path.ChangeExtension(filepath, ".efkmodel");

			Effekseer.Utl.ModelInformation modelInfo = new Utl.ModelInformation();
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

			if (ext == "fbx" || ext == "mqo")
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

				if (ext == "fbx")
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
						string converterPath = Manager.GetEntryDirectory() + "/tools/fbxToEffekseerModelConverter";

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

						info.Arguments = "\"" + oldFilepath + "\" \"" + newFilepath + "\" -scale " + omd.Magnification.ToString();

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
						info.RedirectStandardInput = false;
						info.CreateNoWindow = true;

						System.Diagnostics.Process p = System.Diagnostics.Process.Start(info);

						string outputs = p.StandardOutput.ReadToEnd();

						p.WaitForExit();
						p.Dispose();
						

						if (System.IO.File.Exists(newFilepath))
						{
							System.IO.File.SetLastWriteTime(newFilepath, System.IO.File.GetLastWriteTime(oldFilepath));
						}
						else
						{
							var msg =" Failed to load. \n" + outputs;

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

				if (ext == "mqo")
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
						string converterPath = Manager.GetEntryDirectory() + "/tools/mqoToEffekseerModelConverter";

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

						try
						{
							mqoToEffekseerModelConverter.Program.Call(new[] { oldFilepath, newFilepath, "-scale", omd.Magnification.ToString() });
						}
						catch
						{

						}

						if (System.IO.File.Exists(newFilepath))
						{
							System.IO.File.SetLastWriteTime(newFilepath, System.IO.File.GetLastWriteTime(oldFilepath));
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
