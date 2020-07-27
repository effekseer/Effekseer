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
		public string CurrentPath { get; private set; }
		List<FileItem> items = new List<FileItem>();
		int selectedIndex = -1;

		string menuOpenFile;
		string menuShowInFileManager;
		string menuImportFromPackage;
		string menuExportToPackage;
		
		System.IO.FileSystemWatcher directoryWatcher;
		bool shouldUpdateFileList = true;

		const string ContextMenuPopupId = "###FileViewerPopupMenu";

		public FileViewer()
		{
			Label = Icons.PanelFileViewer + Resources.GetString("FileViewer") + "###FileVeiwer";
			Core.OnAfterLoad += OnAfterLoad;
			Core.OnAfterNew += OnAfterLoad;
			Core.OnAfterSave += OnAfterSave;

			UpdateFileListWithProjectPath(Core.Root.GetFullPath());

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
			menuImportFromPackage = Resources.GetString("FileViewer_ImportFromPackage");
			menuExportToPackage = Resources.GetString("FileViewer_ExportToPackage");

			directoryWatcher = new FileSystemWatcher();
			directoryWatcher.Changed += (o, e) => { shouldUpdateFileList = true; };
			directoryWatcher.Renamed += (o, e) => { shouldUpdateFileList = true; };
			directoryWatcher.Deleted += (o, e) => { shouldUpdateFileList = true; };
			directoryWatcher.Created += (o, e) => { shouldUpdateFileList = true; };
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
					!String.IsNullOrEmpty(CurrentPath))
				{
					UpdateFileListWithProjectPath(CurrentPath);
				}

				// Back directory
				if (Manager.NativeManager.Button("↑") &&
					!String.IsNullOrEmpty(CurrentPath))
				{
					UpdateFileListWithProjectPath(CurrentPath);
				}
			
				Manager.NativeManager.SameLine();

				// Display current directory
				if (Manager.NativeManager.InputText("###AddressBar", (CurrentPath != null) ? CurrentPath : ""))
				{
					string path = Manager.NativeManager.GetInputTextResult();
					UpdateFileList(path);
				}
			}

			Manager.NativeManager.Separator();

			// Display all files
			for (int i = 0; i < items.Count; i++)
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
				if (Manager.NativeManager.Selectable(caption, item.Selected, swig.SelectableFlags.AllowDoubleClick))
				{
					if (Manager.NativeManager.IsCtrlKeyDown())
					{
						item.Selected = !item.Selected;
					}
					else if (Manager.NativeManager.IsShiftKeyDown() && selectedIndex >= 0)
					{
						int min = Math.Min(selectedIndex, i);
						int max = Math.Max(selectedIndex, i);
						for (int j = min; j <= max; j++)
						{
							items[j].Selected = true;
						}
					}
					else
					{
						ResetSelected();
						item.Selected = true;
					}
					
					selectedIndex = i;

					if (Manager.NativeManager.IsMouseDoubleClicked(0) ||
						Manager.NativeManager.IsKeyDown(Manager.NativeManager.GetKeyIndex(swig.Key.Enter)))
					{
						OnFilePicked();
					}
				}

				if (Manager.NativeManager.IsItemFocused())
				{
					ResetSelected();
				}

				if (Manager.NativeManager.IsItemClicked(1))
				{
					if (!item.Selected)
					{
						ResetSelected();
						item.Selected = true;
					}
					selectedIndex = i;

					Manager.NativeManager.OpenPopup(ContextMenuPopupId);
				}

				// D&D
				DragAndDrops.UpdateImageSrc(item.FilePath);
			}

			UpdateContextMenu();

			Manager.NativeManager.PopItemWidth();

			if (shouldUpdateFileList)
			{
				UpdateFileList();
				shouldUpdateFileList = false;
			}
		}

		void UpdateContextMenu()
		{
			// File Context Menu
			if (Manager.NativeManager.BeginPopup(ContextMenuPopupId))
			{
				if (Manager.NativeManager.MenuItem(menuOpenFile))
				{
					OnFilePicked();
				}

				if (Manager.NativeManager.MenuItem(menuShowInFileManager))
				{
					ShowInFileManager();
				}

				Manager.NativeManager.Separator();

				if (Manager.NativeManager.MenuItem(menuImportFromPackage))
				{
					if (!string.IsNullOrEmpty(CurrentPath))
					{
						Commands.ImportPackage(null, CurrentPath);
					}
				}

				if (Manager.NativeManager.MenuItem(menuExportToPackage))
				{
					var files = items
						.Where(it => it.Selected && Path.GetExtension(it.FilePath) == ".efkefc")
						.Select(it => it.FilePath)
						.ToArray();
					if (files.Length > 0)
					{
						Commands.ExportPackage(null, files);
					}
				}

				Manager.NativeManager.EndPopup();
			}
		}

		void OnAfterLoad(object sender, EventArgs e)
		{
			UpdateFileListWithProjectPath(Core.Root.GetFullPath());
		}

		void OnAfterSave(object sender, EventArgs e)
		{
			UpdateFileListWithProjectPath(Core.Root.GetFullPath());
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

			public bool Selected { get; set; }

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
		private void UpdateFileList(string path = null)
		{
			if (path == null)
			{
				path = CurrentPath;
			}

			if (!Directory.Exists(path))
			{
				return;
			}

			ResetSelected();
			CurrentPath = path;
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

			directoryWatcher.Path = path;
			directoryWatcher.EnableRaisingEvents = true;
		}

		private void ResetSelected()
		{
			selectedIndex = -1;
			for (int i = 0; i < items.Count; i++)
			{
				items[i].Selected = false;
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
