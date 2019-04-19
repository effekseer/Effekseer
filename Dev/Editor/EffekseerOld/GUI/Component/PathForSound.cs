using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Media;
using System.IO;
using System.Text.RegularExpressions;

namespace Effekseer.GUI.Component
{
	public partial class PathForSound : UserControl
	{
		public PathForSound()
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
				if (lbl_file.Text.Length > 0)
				{
					UpdateInfo();
					//this.Height = 60;
				}
				else
				{
					//this.Height = 20;
				}
			}
			else
			{
				btn_load.Enabled = false;
				lbl_file.Text = string.Empty;
			}
		}

		void UpdateInfo()
		{
			string path = binding.GetAbsolutePath();

			try
			{
				var fs = new FileStream(path, FileMode.Open, FileAccess.Read);
				var reader = new BinaryReader(fs);

				if (reader.ReadUInt32() != 0x46464952)	// "RIFF"
					throw new Exception();
				reader.BaseStream.Seek(4, SeekOrigin.Current);
				if (reader.ReadUInt32() != 0x45564157)	// "WAVE"
					throw new Exception();

				ushort channels = 1;
				uint sampleRate = 44100;
				uint bytePerSec = sampleRate * 2;
				while (true)
				{
					uint chunk = reader.ReadUInt32();
					uint chunkSize = reader.ReadUInt32();
					
					if (chunk == 0x20746D66)		// "fmt "
					{
						reader.BaseStream.Seek(2, SeekOrigin.Current);
						channels = reader.ReadUInt16();
						sampleRate = reader.ReadUInt32();
						bytePerSec = reader.ReadUInt32();
						reader.BaseStream.Seek(-12, SeekOrigin.Current);
					}
					else if (chunk == 0x61746164)	// "data"
					{
						ulong time = (ulong)chunkSize * 1000 / bytePerSec;
						lbl_info1.Text = System.String.Format("[Format] {0}ch {1}Hz", 
							channels, sampleRate);
						lbl_info2.Text = System.String.Format("[TotalTime] {0:00}:{1:00}.{2:00}",
							time / 1000 / 60, time / 1000 % 60, time % 1000 / 10);
						break;
					}
					reader.BaseStream.Seek(chunkSize, SeekOrigin.Current);
				}
				
				fs.Close();
				fs.Dispose();
			}
			catch (Exception e)
			{
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

		private void btn_delete_Click(object sender, EventArgs e)
		{
			if (player != null)
			{
				player.Stop();
				player.Dispose();
				player = null;
			}
			binding.SetAbsolutePath("");
			Read();
		}

		SoundPlayer player = null;

		private void btn_play_Click(object sender, EventArgs e)
		{
			if (player != null)
			{
				player.Stop();
				player.Dispose();
			}
			string path = binding.GetAbsolutePath();
			if (File.Exists(path)) {
				player = new SoundPlayer(path);
				player.Play();
			}
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

		void Core_OnReload(object sender, EventArgs e)
		{
			Read();
		}

		void Path_HandleDestroyed(object sender, EventArgs e)
		{
			Binding = null;
			Core.OnAfterNew -= Core_OnAfterNew;
			Core.OnAfterSave -= Core_OnAfterSave;
			Core.OnAfterLoad -= Core_OnAfterLoad;
			if (player != null)
			{
				player.Stop();
				player.Dispose();
				player = null;
			}
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

		private void PathForSound_DragEnter(object sender, DragEventArgs e)
		{
			string filePath = GetDragFile(e);
			
			if (filePath != null) {
				if (CheckExtension(filePath)) {
					e.Effect = DragDropEffects.Link;
				}
			}
		}

		private void PathForSound_DragLeave(object sender, EventArgs e)
		{
		}

		private void PathForSound_DragDrop(object sender, DragEventArgs e)
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
