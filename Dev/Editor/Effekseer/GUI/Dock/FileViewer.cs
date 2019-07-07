using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace Effekseer.GUI.Dock
{
	class FileViewer : DockPanel
	{
		string currentPath = null;
		List<FileItem> items = new List<FileItem>();
		int selectedIndex = -1;

		public FileViewer()
		{
			Label = Resources.GetString("FileViewer") + "###FileVeiwer";
			Core.OnAfterLoad += OnAfterLoad;
			Core.OnAfterNew += OnAfterLoad;

			UpdateFileListWithProjectPath(Core.FullPath);

			Icon = Images.GetIcon("PanelFileViewer");
			IconSize = new swig.Vec2(24, 24);
			TabToolTip = Resources.GetString("FileViewer");
		}

		public override void OnDisposed()
		{
			Core.OnAfterLoad -= OnAfterLoad;
			Core.OnAfterNew -= OnAfterLoad;
		}

		protected override void UpdateInternal()
		{
			Manager.NativeManager.PushItemWidth(-1);

			// Address bar
			{
				if (Manager.NativeManager.Button("↑") &&
					!String.IsNullOrEmpty(currentPath))
				{
					UpdateFileListWithProjectPath(currentPath);
				}
			
				Manager.NativeManager.SameLine();

				// Display current directory
				if (Manager.NativeManager.InputText("", (currentPath != null) ? currentPath : ""))
				{
					string path = Manager.NativeManager.GetInputTextResult();
					UpdateFileList(path);
				}
			}

			Manager.NativeManager.Separator();

			// Display all files
			for(int i = 0; i < items.Count; i++)
			{
				var item = items[i];

				swig.ImageResource image = null;

				if(item.Type == FileType.Directory)
				{
					image = Images.GetIcon("FileViewer_Directory");
				}
				if (item.Type == FileType.EffekseerProject)
				{
					image = Images.GetIcon("FileViewer_EffekseerProj");
				}
				if (item.Type == FileType.Image)
				{
					image = item.Image;
				}
				if (item.Type == FileType.Other)
				{
				}

				Manager.NativeManager.Image(image, 32, 32);

				Manager.NativeManager.SameLine();

				string caption = Path.GetFileName(item.FilePath);
				if (Manager.NativeManager.Selectable(caption, i == selectedIndex, swig.SelectableFlags.AllowDoubleClick))
				{
					selectedIndex = i;

					if(Manager.NativeManager.IsMouseDoubleClicked(0))
					{
						DoubleClick();
					}
				}
				
				// D&D
				DragAndDrops.UpdateImageSrc(item.FilePath);
			}
			
			Manager.NativeManager.PopItemWidth();
		}

		void OnAfterLoad(object sender, EventArgs e)
		{
			UpdateFileListWithProjectPath(Core.FullPath);
		}

		void UpdateFileListWithProjectPath(string path)
		{
			if (string.IsNullOrEmpty(path))
				return;

			string dirPath = string.Empty;

			try
			{
				dirPath = Path.GetDirectoryName(path);
			}
			catch (Exception e)
			{
				return;
			}

			UpdateFileList(dirPath);
		}

		public enum FileType
		{
			Directory,
			EffekseerProject,
			Image,
			Other,
		}

		/// <summary>
		/// A file in the list
		/// </summary>
		public class FileItem
		{
			/// <summary>
			/// File path
			/// </summary>
			public string FilePath { get; set; }

			public FileType Type { get; private set; }

			public swig.ImageResource Image { get; private set; }

			public FileItem(string name, string filePath)
			{
				FilePath = filePath;

				if (System.IO.Directory.Exists(filePath))
				{
					Type = FileType.Directory;
				}
				else if (System.IO.Path.GetExtension(filePath).ToLower() == ".efkproj")
				{
					Type = FileType.EffekseerProject;
				}
				else if (System.IO.Path.GetExtension(filePath).ToLower() == ".png")
				{
					Type = FileType.Image;
					Image = Images.Load(Manager.Native, filePath);
				}
				else
				{
					Type = FileType.Other;
				}
			}
		}

		/// <summary>
		/// Update list
		/// </summary>
		private void UpdateFileList(string path)
		{
			if (!Directory.Exists(path))
			{
				return;
			}

			selectedIndex = -1;
			currentPath = path;
			items.Clear();

			//string parentDirectory = System.IO.Path.GetDirectoryName(path);
			//if (parentDirectory != null)
			//{
			//	items.Add(new FileItem("Parent directory", Path.GetDirectoryName(path)));
			//}

			// add directories
			foreach (string dirPath in Directory.EnumerateDirectories(path))
			{
				//int imageIndex = GetImageIndexFileIcon(dirPath);
				var dirNode = new FileItem(Path.GetFileName(dirPath), dirPath);
				items.Add(dirNode);
			}

			// add files
			foreach (string filePath in Directory.EnumerateFiles(path))
			{
				//int imageIndex = GetImageIndexFileIcon(filePath);
				var fileNode = new FileItem(Path.GetFileName(filePath), filePath);
				items.Add(fileNode);
			}
		}

		void DoubleClick()
		{
			if (items.Count == 0)
			{
				return;
			}

			if (selectedIndex == -1) return;

			var fileItem = items[selectedIndex];
			if (Path.GetExtension(fileItem.FilePath) == ".efkproj")
			{
				// efkproj is opened internal function
				Commands.Open(fileItem.FilePath);
			}
			else if (Directory.Exists(fileItem.FilePath))
			{
				// move directory
				UpdateFileList(fileItem.FilePath);
			}
			else
			{
				// open process
				System.Diagnostics.Process.Start(fileItem.FilePath);
			}
		}
	}
}
