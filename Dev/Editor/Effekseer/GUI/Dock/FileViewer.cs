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

		string menuOpenFile;
		string menuShowInFileManager;

		public FileViewer()
		{
			Label = Resources.GetString("FileViewer") + "###FileVeiwer";
			Core.OnAfterLoad += OnAfterLoad;
			Core.OnAfterNew += OnAfterLoad;
			Core.OnAfterSave += OnAfterSave;

			UpdateFileListWithProjectPath(Core.FullPath);

			Icon = Images.GetIcon("PanelFileViewer");
			TabToolTip = Resources.GetString("FileViewer");

			menuOpenFile = Resources.GetString("FileViewer_OpenFile");

			if (swig.GUIManager.IsMacOSX())
			{
				menuShowInFileManager = Resources.GetString("FileViewer_ShowInFinder");
			}
			else
			{
				menuShowInFileManager = Resources.GetString("FileViewer_ShowInExplorer");
			}
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
				// Back directory (BS shortcut key)
				if (Manager.NativeManager.IsWindowFocused() &&
					Manager.NativeManager.IsKeyPressed(Manager.NativeManager.GetKeyIndex(swig.Key.Backspace)) &&
					!String.IsNullOrEmpty(currentPath))
				{
					UpdateFileListWithProjectPath(currentPath);
				}

				// Back directory
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

				{
					float iconSize = Manager.NativeManager.GetTextLineHeight();
					Manager.NativeManager.Image(image, iconSize, iconSize);
				}

				Manager.NativeManager.SameLine();

				string caption = Path.GetFileName(item.FilePath);
				if (Manager.NativeManager.Selectable(caption, i == selectedIndex, swig.SelectableFlags.AllowDoubleClick))
				{
					selectedIndex = i;

					if (Manager.NativeManager.IsMouseDoubleClicked(0) ||
						Manager.NativeManager.IsKeyDown(Manager.NativeManager.GetKeyIndex(swig.Key.Enter)))
					{
						OnFilePicked();
					}
				}

				// File Context Menu
				string menuId = "###FileViewerFilePopupMenu" + i;
				if (Manager.NativeManager.BeginPopupContextItem(menuId))
				{
					selectedIndex = i;
					if (Manager.NativeManager.MenuItem(menuOpenFile))
					{
						OnFilePicked();
					}
					Manager.NativeManager.Separator();
					
					if (Manager.NativeManager.MenuItem(menuShowInFileManager))
					{
						ShowInFileManager();
					}
					Manager.NativeManager.EndPopup();
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

		void OnAfterSave(object sender, EventArgs e)
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
				else if (System.IO.Path.GetExtension(filePath).ToLower() == ".efkefc")
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

		private void OnFilePicked()
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
			else if (Path.GetExtension(fileItem.FilePath) == ".efkefc")
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
				try
				{
					// open process
					System.Diagnostics.Process.Start(fileItem.FilePath);
				}
				catch (Exception e)
				{
				}
			}
		}

		private void ShowInFileManager()
		{
			if (items.Count == 0)
			{
				return;
			}

			if (selectedIndex == -1) return;

			var fileItem = items[selectedIndex];

			try
			{
				string dirPath = Directory.Exists(fileItem.FilePath) ? 
					fileItem.FilePath : Path.GetDirectoryName(fileItem.FilePath);

				if (swig.GUIManager.IsMacOSX())
				{
					System.Diagnostics.Process.Start("open", dirPath);
				}
				else
				{
					System.Diagnostics.Process.Start(dirPath);
				}
			}
			catch (Exception e)
			{
			}
		}
	}
}
