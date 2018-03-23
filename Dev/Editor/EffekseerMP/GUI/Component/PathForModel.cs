using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Media;
using System.IO;
using System.Text.RegularExpressions;

namespace Effekseer.GUI.Component
{
	class PathForModel : Control, IParameterControl
	{
		string id = "";

		public string Label { get; set; } = string.Empty;

		public string Description { get; set; } = string.Empty;

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

		public PathForModel(string label = null)
		{
			if (label != null)
			{
				Label = label;
			}

			id = "###" + Manager.GetUniqueID().ToString();
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

			if (Manager.NativeManager.Button("読込"))
			{
				btn_load_Click();
			}

			isHovered = isHovered || Manager.NativeManager.IsItemHovered();

			Manager.NativeManager.SameLine();

			Manager.NativeManager.Text(filePath);

			isHovered = isHovered || Manager.NativeManager.IsItemHovered();

			if (Manager.NativeManager.Button("解除"))
			{
				btn_delete_Click();
			}

			isHovered = isHovered || Manager.NativeManager.IsItemHovered();

			if (Manager.NativeManager.Button("再読み込み"))
			{
				btn_reload_Click();
			}

			isHovered = isHovered || Manager.NativeManager.IsItemHovered();
		}

		private void btn_load_Click()
		{
			if (binding == null) return;

			OpenFileDialog ofd = new OpenFileDialog();

			ofd.InitialDirectory = System.IO.Directory.GetCurrentDirectory();
			ofd.Filter = binding.Filter;
			ofd.FilterIndex = 2;
			ofd.Multiselect = false;

			if (ofd.ShowDialog() == DialogResult.OK)
			{
				var filepath = ofd.FileName;
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
			Match match = Regex.Match(binding.Filter, "\\*(\\.[a-zA-Z0-9]*)");
			string extension = match.Value.Substring(1);
			return System.IO.Path.GetExtension(path) == extension;
		}

		void LoadFile(string filepath, bool isReloading)
		{

			// Convert file
			var filenameWE = System.IO.Path.GetDirectoryName(filepath) + "/" + System.IO.Path.GetFileNameWithoutExtension(filepath);
			var ext = System.IO.Path.GetExtension(filepath).ToLower().Replace(".", "");
			var newFilepath = filenameWE + ".efkmodel";

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
				if (!omd.OK) return;
			}

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
					string appPath = System.Reflection.Assembly.GetExecutingAssembly().Location;
					string converterPath = System.IO.Path.GetDirectoryName(appPath) + "/tools/fbxToEffekseerModelConverter.exe";

					System.Diagnostics.ProcessStartInfo info = new System.Diagnostics.ProcessStartInfo();
					info.FileName = converterPath;
					info.Arguments = "\"" + oldFilepath + "\" \"" + newFilepath + "\" -scale " + omd.Magnification.ToString();

					System.Diagnostics.Process p = System.Diagnostics.Process.Start(info);
					p.WaitForExit();
					p.Dispose();

					if (System.IO.File.Exists(newFilepath))
					{
						System.IO.File.SetLastWriteTime(newFilepath, System.IO.File.GetLastWriteTime(oldFilepath));
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
					string appPath = System.Reflection.Assembly.GetExecutingAssembly().Location;
					string converterPath = System.IO.Path.GetDirectoryName(appPath) + "/tools/mqoToEffekseerModelConverter.exe";

					System.Diagnostics.ProcessStartInfo info = new System.Diagnostics.ProcessStartInfo();
					info.FileName = converterPath;
					info.Arguments = "\"" + oldFilepath + "\" \"" + newFilepath + "\" -scale " + omd.Magnification.ToString();

					System.Diagnostics.Process p = System.Diagnostics.Process.Start(info);
					p.WaitForExit();
					p.Dispose();

					if (System.IO.File.Exists(newFilepath))
					{
						System.IO.File.SetLastWriteTime(newFilepath, System.IO.File.GetLastWriteTime(oldFilepath));
					}
				}
			}

			binding.SetAbsolutePath(filepath);

			System.IO.Directory.SetCurrentDirectory(System.IO.Path.GetDirectoryName(filepath));
		}

	}
}
