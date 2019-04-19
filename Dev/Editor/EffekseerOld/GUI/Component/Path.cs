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
	public partial class Path : UserControl
	{
		public Path()
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
				binding.SetAbsolutePath(filepath);

				System.IO.Directory.SetCurrentDirectory(System.IO.Path.GetDirectoryName(filepath));
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
	}
}
