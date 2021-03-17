using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace Effekseer.GUI.Dock
{
	public class FileViewer : DockPanel
	{
		public string CurrentPath { get; private set; }
		string addressText = string.Empty;
		bool addressEditing = false;
		List<FileItem> items = new List<FileItem>();
		int selectedIndex = -1;

		string menuOpenFile;
		string menuShowInFileManager;
		string menuImportFromPackage;
		string menuExportToPackage;
		
		FileSystemWatcher directoryWatcher = new FileSystemWatcher();
		bool shouldUpdateFileList = true;

		const string ContextMenuPopupId = "###FileViewerPopupMenu";

		public FileViewer()
		{
			Label = Icons.PanelFileViewer + Resources.GetString("FileViewer") + "###FileVeiwer";

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

			directoryWatcher.Changed += (o, e) => { shouldUpdateFileList = true; };
			directoryWatcher.Renamed += (o, e) => { shouldUpdateFileList = true; };
			directoryWatcher.Deleted += (o, e) => { shouldUpdateFileList = true; };
			directoryWatcher.Created += (o, e) => { shouldUpdateFileList = true; };

			UpdateFileListWithProjectPath(Core.Root.GetFullPath());

			Core.OnAfterLoad += OnAfterLoad;
			Core.OnAfterNew += OnAfterLoad;
			Core.OnAfterSave += OnAfterSave;
		}

		public override void OnDisposed()
		{
			Core.OnAfterLoad -= OnAfterLoad;
			Core.OnAfterNew -= OnAfterLoad;
			Core.OnAfterSave -= OnAfterSave;
		}

		protected override void UpdateInternal()
		{
			Manager.NativeManager.PushItemWidth(-1);

			// Address bar
			{
				// Back directory (BS shortcut key)
				if (Manager.NativeManager.IsWindowFocused() &&
					Manager.NativeManager.IsKeyPressed(Manager.NativeManager.GetKeyIndex(swig.Key.Backspace)) &&
					!Manager.NativeManager.IsAnyItemActive() &&
					!string.IsNullOrEmpty(CurrentPath))
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
				if (Manager.NativeManager.InputText("###AddressBar", addressText))
				{
					addressText = Manager.NativeManager.GetInputTextResult();
					UpdateFileList(addressText);
				}
				if (Manager.NativeManager.IsItemActivated())
				{
					addressEditing = true;
				}
				if (Manager.NativeManager.IsItemDeactivated())
				{
					addressEditing = false;
					addressText = CurrentPath;
				}
			}

			Manager.NativeManager.Separator();

			// Display all files
			for (int i = 0; i < items.Count; i++)
			{
				var item = items[i];

				string icon = Icons.Empty;
				swig.ImageResource image = null;

				switch (item.Type)
				{
					case FileType.Directory:
						icon = Icons.FileDirectory;
						break;
					case FileType.EffekseerProject:
						icon = Icons.FileEfkefc;
						break;
					case FileType.Image:
						image = item.Image;
						break;
					case FileType.Sound:
						icon = Icons.FileSound;
						break;
					case FileType.Model:
						icon = Icons.FileModel;
						break;
					case FileType.Material:
						icon = Icons.FileEfkmat;
						break;
					default:
						icon = Icons.FileOther;
						break;
				}

				float iconPosX = Manager.NativeManager.GetCursorPosX();

				string caption = icon + " " + Path.GetFileName(item.FilePath);
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
				switch (item.Type)
				{
					case FileType.Image:
						DragAndDrops.UpdateFileSrc(item.FilePath, DragAndDrops.FileType.Image);
						break;
					case FileType.Sound:
						DragAndDrops.UpdateFileSrc(item.FilePath, DragAndDrops.FileType.Sound);
						break;
					case FileType.Model:
						DragAndDrops.UpdateFileSrc(item.FilePath, DragAndDrops.FileType.Model);
						break;
					case FileType.Material:
						DragAndDrops.UpdateFileSrc(item.FilePath, DragAndDrops.FileType.Material);
						break;
					case FileType.Curve:
						DragAndDrops.UpdateFileSrc(item.FilePath, DragAndDrops.FileType.Curve);
						break;
					default:
						DragAndDrops.UpdateFileSrc(item.FilePath, DragAndDrops.FileType.Other);
						break;
				}

				if (image != null)
				{
					Manager.NativeManager.SameLine();
					Manager.NativeManager.SetCursorPosX(iconPosX);
					float iconSize = Manager.NativeManager.GetTextLineHeight();
					Manager.NativeManager.Image(image, iconSize, iconSize);
				}
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
			catch (Exception)
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
			Sound,
			Model,
			Material,
			Curve,
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
				else
				{
					switch (System.IO.Path.GetExtension(filePath).ToLower())
					{
						case ".efkproj":
						case ".efkefc":
							Type = FileType.EffekseerProject;
							break;
						case ".png":
							Type = FileType.Image;
							Image = Images.Load(Manager.Native, filePath);
							break;
						case ".wav":
							Type = FileType.Sound;
							break;
						case ".efkmodel":
						case ".fbx":
						case ".mqo":
							Type = FileType.Model;
							break;
						case ".efkmat":
							Type = FileType.Material;
							break;
						case ".efkcurve":
							Type = FileType.Curve;
							break;
						default:
							Type = FileType.Other;
							break;
					}
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

			path = Path.GetFullPath(path);

			ResetSelected();
			CurrentPath = path;
			if (!addressEditing) { addressText = path; }
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
			if (fileItem.Type == FileType.EffekseerProject)
			{
				// efkproj is opened internal function
				Commands.Open(fileItem.FilePath);
			}
			else if (fileItem.Type == FileType.Material)
			{
				// execute external material editor
				if (Process.MaterialEditor.Run())
				{
					Process.MaterialEditor.OpenOrCreateMaterial(fileItem.FilePath);
				}
			}
			else if (fileItem.Type == FileType.Directory)
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
				catch (Exception)
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
			catch (Exception)
			{
			}
		}
	}
}
