using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Text.RegularExpressions;

namespace Effekseer.GUI.Component
{
	public partial class PathForModel : UserControl
	{
		public PathForModel()
		{
			InitializeComponent();

			EnableUndo = true;

			this.SuspendLayout();
			Anchor = AnchorStyles.Left | AnchorStyles.Right;
			this.ResumeLayout(false);

			Read();

			HandleDestroyed += new EventHandler(Path_HandleDestroyed);
			Core.OnAfterNew += Core_OnAfterNew;
			Core.OnAfterSave += Core_OnAfterSave;
			Core.OnAfterLoad += Core_OnAfterLoad;
			Core.OnReload += Core_OnReload;

			this.btn_load.Text = global::Effekseer.Properties.Resources.Load;
			this.btn_delete.Text = global::Effekseer.Properties.Resources.Delete;
			this.btn_reload.Text = global::Effekseer.Properties.Resources.ResetMaginification;
		}

		Data.Value.Path binding = null;

		public bool EnableUndo { get; set; }

		public Data.Value.Path Binding
		{
			get
			{
				return binding;
			}
			set
			{
				if (binding == value) return;

				if (binding != null)
				{
					
				}

				binding = value;

				if (binding != null)
				{
					
				}

				Read();
			}
		}

		public void SetBinding(object o)
		{
			var o_ = o as Data.Value.Path;
			Binding = o_;
		}

		void Read()
		{
			if (binding != null)
			{
				btn_load.Enabled = true;
				lbl_file.Text = binding.GetRelativePath();
			}
			else
			{
				btn_load.Enabled = false;
				lbl_file.Text = string.Empty;
			}
		}

		private void btn_load_Click(object sender, EventArgs e)
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

		void Core_OnAfterLoad(object sender, EventArgs e)
		{
			Read();
		}

		void Core_OnAfterSave(object sender, EventArgs e)
		{
			Read();
		}

		void Core_OnReload(object sender, EventArgs e)
		{
			Read();
		}

		void Core_OnAfterNew(object sender, EventArgs e)
		{
			Read();
		}

		void Path_HandleDestroyed(object sender, EventArgs e)
		{
			Binding = null;
			Core.OnAfterNew -= Core_OnAfterNew;
			Core.OnAfterSave -= Core_OnAfterSave;
			Core.OnAfterLoad -= Core_OnAfterLoad;
			Core.OnReload -= Core_OnReload;
		}
		
		private bool CheckExtension(string path)
		{
			Match match = Regex.Match(binding.Filter, "\\*(\\.[a-zA-Z0-9]*)");
			string extension = match.Value.Substring(1);
			return System.IO.Path.GetExtension(path) == extension;
		}

		private string GetDragFile(DragEventArgs e)
		{
			{	// FileViewerからのDrag
				var fileItem = e.Data.GetData(typeof(DockFileViewer.FileItem)) as DockFileViewer.FileItem;
				if (fileItem != null) {
					return fileItem.FilePath;
				}
			}

			{	// ExplorerからのDrag
				var dropFiles = (string[])e.Data.GetData(DataFormats.FileDrop);
				if (dropFiles != null && dropFiles.Length == 1) {
					return dropFiles[0];
				}
			}
			return null;
		}

		private void Path_DragEnter(object sender, DragEventArgs e)
		{
			string filePath = GetDragFile(e);
			
			if (filePath != null) {
				if (CheckExtension(filePath)) {
					e.Effect = DragDropEffects.Link;
				}
			}
		}

		private void Path_DragLeave(object sender, EventArgs e)
		{
		}

		private void Path_DragDrop(object sender, DragEventArgs e)
		{
			string filePath = GetDragFile(e);
			
			if (filePath != null) {
				if (CheckExtension(filePath)) {
					binding.SetAbsolutePath(filePath);
					Read();
				}
			}
		}

		private void btn_delete_Click(object sender, EventArgs e)
		{
			if (binding == null) return;
			binding.SetAbsolutePath(string.Empty);

			Read();
		}

		private void btn_reload_Click(object sender, EventArgs e)
		{
			if (binding == null) return;
			var filepath = binding.AbsolutePath;
			if (string.IsNullOrEmpty(filepath)) return;
			LoadFile(filepath, true);

			Read();
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
				if(!isReloading)
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

			Dialog.OpenModelDialog omd = new Dialog.OpenModelDialog(modelInfo.Scale);

			if (ext == "fbx" || ext == "mqo")
			{
				omd.ShowDialog();
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
					string converterPath = System.IO.Path.GetDirectoryName(appPath) + "/tools/fbxToEffekseerModelConverter";

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
					string converterPath = System.IO.Path.GetDirectoryName(appPath) + "/tools/mqoToEffekseerModelConverter";

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
