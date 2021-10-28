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
		bool addressActivated = false;

		List<FileItem> items = new List<FileItem>();
		int selectedIndex = -1;

		string menuOpenFile;
		string menuShowInFileManager;
		string menuImportFromPackage;
		string menuExportToPackage;

		FileSystemWatcher directoryWatcher = new FileSystemWatcher();
		bool shouldUpdateFileList = true;

		const string OptionMenuPopupId = "###FileViewerOptionMenu";
		const string ContextMenuPopupId = "###FileViewerContextMenu";

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
			var regionSize = Manager.NativeManager.GetContentRegionAvail();
			float dpiScale = Manager.DpiScale;
			var spacing = Manager.NativeManager.GetStyleVar2(swig.ImGuiStyleVarFlags.ItemSpacing);

			Manager.NativeManager.PushItemWidth(-1);

			// Address bar
			UpdateAddressBar(dpiScale, regionSize, spacing);

			Manager.NativeManager.Separator();

			// Display all files
			UpdateFileView(dpiScale, regionSize, spacing);

			Manager.NativeManager.PopItemWidth();

			UpdateKeyNavication();

			if (shouldUpdateFileList)
			{
				UpdateFileList();
				shouldUpdateFileList = false;
			}
		}

		void UpdateAddressBar(float dpiScale, swig.Vec2 regionSize, swig.Vec2 spacing)
		{
			float buttonWidth = Manager.NativeManager.GetTextLineHeight() + Manager.NativeManager.GetStyleVar2(swig.ImGuiStyleVarFlags.FramePadding).X * 2;

			// Back directory
			if (Manager.NativeManager.Button("↑", buttonWidth) &&
				!String.IsNullOrEmpty(CurrentPath))
			{
				UpdateFileListWithProjectPath(CurrentPath);
			}

			Manager.NativeManager.SameLine();

			// Display current directory
			if (addressActivated || addressEditing)
			{
				Manager.NativeManager.PushItemWidth(regionSize.X - buttonWidth * 2 - spacing.X * 2);

				if (addressActivated)
				{
					Manager.NativeManager.SetKeyboardFocusHere();
					addressEditing = true;
					addressActivated = false;
				}

				if (Manager.NativeManager.InputText("###AddressText", addressText))
				{
					addressText = Manager.NativeManager.GetInputTextResult();
					UpdateFileList(addressText);
				}
				if (Manager.NativeManager.IsItemDeactivated())
				{
					addressEditing = false;
					addressText = CurrentPath;
				}

				Manager.NativeManager.PopItemWidth();
			}
			else
			{
				string label = Path.GetFileName(CurrentPath) + "###AddressButton";
				if (Manager.NativeManager.Button(label, regionSize.X - buttonWidth * 2 - spacing.X * 2))
				{
					addressActivated = true;
				}
			}

			Manager.NativeManager.SameLine();

			if (Manager.NativeManager.Button(Icons.NavMenu, buttonWidth))
			{
				Manager.NativeManager.OpenPopup(OptionMenuPopupId);
			}

			UpdateOptionMenu();
		}

		void UpdateOptionMenu()
		{
			float fontSize = Manager.NativeManager.GetTextLineHeight();

			Manager.NativeManager.SetNextWindowSize(fontSize * 20, 0.0f, swig.Cond.Always);

			if (Manager.NativeManager.BeginPopup(OptionMenuPopupId))
			{
				var viewMode = Core.Option.FileViewerViewMode.GetValue();

				Manager.NativeManager.PushItemWidth(fontSize * 8);

				// View Mode setting
				string comboLabel = MultiLanguageTextProvider.GetText("Options_FileViewerViewMode_Name");
				string comboSelectedLabel = MultiLanguageTextProvider.GetText("FileViewer_ViewMode_" + Core.Option.FileViewerViewMode.GetValue().ToString() + "_Name");
				if (Manager.NativeManager.BeginCombo(comboLabel, comboSelectedLabel, swig.ComboFlags.None))
				{
					string iconViewLabel = MultiLanguageTextProvider.GetText("FileViewer_ViewMode_" + Data.OptionValues.FileViewMode.IconView.ToString() + "_Name");
					if (Manager.NativeManager.Selectable(iconViewLabel, viewMode == Data.OptionValues.FileViewMode.IconView))
					{
						Core.Option.FileViewerViewMode.SetValueDirectly(Data.OptionValues.FileViewMode.IconView);
					}
					string listViewLabel = MultiLanguageTextProvider.GetText("FileViewer_ViewMode_" + Data.OptionValues.FileViewMode.ListView.ToString() + "_Name");
					if (Manager.NativeManager.Selectable(listViewLabel, viewMode == Data.OptionValues.FileViewMode.ListView))
					{
						Core.Option.FileViewerViewMode.SetValueDirectly(Data.OptionValues.FileViewMode.ListView);
					}
					Manager.NativeManager.EndCombo();
				}

				// Icon size setting
				if (viewMode == Data.OptionValues.FileViewMode.IconView)
				{
					int[] val = new int[1] { Core.Option.FileViewerIconSize.GetValue() };
					if (Manager.NativeManager.DragInt(MultiLanguageTextProvider.GetText("Options_FileViewerIconSize_Name"), val, 1, 32, 512, "%d"))
					{
						Core.Option.FileViewerIconSize.SetValueDirectly(val[0]);
					}
				}

				Manager.NativeManager.PopItemWidth();

				Manager.NativeManager.EndPopup();
			}
		}

		void UpdateFileView(float dpiScale, swig.Vec2 regionSize, swig.Vec2 spacing)
		{
			var viewMode = Core.Option.FileViewerViewMode.GetValue();
			float iconSize = Core.Option.FileViewerIconSize.GetValue();

			int contentCountX = 1;

			if (viewMode == Data.OptionValues.FileViewMode.IconView)
			{
				contentCountX = (int)((regionSize.X + spacing.X * 2) / (iconSize + spacing.X));
				if (contentCountX <= 0) contentCountX = 1;
			}

			for (int i = 0; i < items.Count; i++)
			{
				var item = items[i];

				string icon = item.GetIcon(viewMode == Data.OptionValues.FileViewMode.ListView);

				if (i != 0 && i % contentCountX != 0)
				{
					Manager.NativeManager.SameLine();
				}

				item.DrawPosX = Manager.NativeManager.GetCursorPosX();
				item.DrawPosY = Manager.NativeManager.GetCursorPosY();

				string label = icon + Path.GetFileName(item.FilePath);
				bool selected = false;

				switch (viewMode)
				{
					case Data.OptionValues.FileViewMode.IconView:
						selected = Manager.NativeManager.SelectableContent("###FileContent_" + i, label,
							item.Selected, item.Image, iconSize, iconSize, swig.SelectableFlags.AllowDoubleClick);
						break;
					case Data.OptionValues.FileViewMode.ListView:
						selected = Manager.NativeManager.Selectable(label + "###FileContent_" + i, item.Selected, swig.SelectableFlags.AllowDoubleClick);
						break;
				}

				item.ItemID = Manager.NativeManager.GetItemID();

				if (selected)
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
					case FileType.Texture:
						DragAndDrops.UpdateFileSrc(item.FilePath, FileType.Texture);
						break;
					case FileType.Sound:
						DragAndDrops.UpdateFileSrc(item.FilePath, FileType.Sound);
						break;
					case FileType.Model:
						DragAndDrops.UpdateFileSrc(item.FilePath, FileType.Model);
						break;
					case FileType.Material:
						DragAndDrops.UpdateFileSrc(item.FilePath, FileType.Material);
						break;
					case FileType.Curve:
						DragAndDrops.UpdateFileSrc(item.FilePath, FileType.Curve);
						break;
					default:
						DragAndDrops.UpdateFileSrc(item.FilePath, FileType.Other);
						break;
				}

				if (viewMode == Data.OptionValues.FileViewMode.ListView)
				{
					if (item.Image != null)
					{
						Manager.NativeManager.SameLine();
						Manager.NativeManager.SetCursorPosX(item.DrawPosX);
						float listIconSize = Manager.NativeManager.GetTextLineHeight();
						Manager.NativeManager.Image(item.Image, listIconSize, listIconSize);
					}
				}
			}

			UpdateContextMenu();

			if (!Manager.NativeManager.IsPopupOpen(ContextMenuPopupId) &&
				!Manager.NativeManager.IsAnyItemActive() &&
				Manager.NativeManager.IsMouseClicked(0, false))
			{
				ResetSelected();
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

		void UpdateKeyNavication()
		{
			if (Manager.NativeManager.IsWindowFocused() && !Manager.NativeManager.IsAnyItemActive())
			{
				// Back directory (BS shortcut key)
				if (Manager.NativeManager.IsKeyPressed(Manager.NativeManager.GetKeyIndex(swig.Key.Backspace)) &&
					!string.IsNullOrEmpty(CurrentPath))
				{
					UpdateFileListWithProjectPath(CurrentPath);
				}

				// Find item by input key
				string inputChars = Manager.NativeManager.GetInputCharacters().ToLower();
				if (inputChars.Length > 0)
				{
					char navChar = inputChars[0];

					List<FileItem> hits = items.FindAll(item => Path.GetFileName(item.FilePath).ToLower().IndexOf(navChar) == 0);
					if (hits.Count > 0)
					{
						int hitIndex = items.IndexOf(hits[0]);
						int lastIndex = selectedIndex;

						ResetSelected();

						if (lastIndex >= 0 && hits.Contains(items[lastIndex]))
						{
							selectedIndex = lastIndex + 1;

							if (selectedIndex - hitIndex >= hits.Count)
							{
								selectedIndex = hitIndex;
							}
						}
						else
						{
							selectedIndex = hitIndex;
						}

						// Set focus
						FileItem item = items[selectedIndex];
						item.Selected = true;
						Manager.NativeManager.SetFocusID(item.ItemID);

						// Scroll to navigation item
						float relX = item.DrawPosX - Manager.NativeManager.GetScrollX();
						float relY = item.DrawPosY - Manager.NativeManager.GetScrollY();

						switch (Core.Option.FileViewerViewMode.GetValue())
						{
							case Data.OptionValues.FileViewMode.IconView:
								Manager.NativeManager.ScrollToBringRectIntoView(
									new swig.Vec2(relX, relY),
									new swig.Vec2(relX, relY + Core.Option.FileViewerIconSize.GetValue()));
								break;
							case Data.OptionValues.FileViewMode.ListView:
								Manager.NativeManager.ScrollToBringRectIntoView(
									new swig.Vec2(relX, relY),
									new swig.Vec2(relX, relY + Manager.NativeManager.GetTextLineHeightWithSpacing()));
								break;
						}
					}
				}
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

			public int ItemID { get; set; }

			public float DrawPosX { get; set; }

			public float DrawPosY { get; set; }

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
							Type = FileType.Effect;
							break;
						case ".png":
							Type = FileType.Texture;
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

			public string GetIcon(bool isListView)
			{
				switch (Type)
				{
					case FileType.Directory: return Icons.FileDirectory;
					case FileType.Effect: return Icons.FileEfkefc;
					case FileType.Texture: return isListView ? Icons.Empty : "";
					case FileType.Sound: return Icons.FileSound;
					case FileType.Model: return Icons.FileModel;
					case FileType.Material: return Icons.FileEfkmat;
					default: return Icons.FileOther;
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

			if (path.EndsWith(Path.DirectorySeparatorChar.ToString()))
			{
				path = path.Substring(0, path.Length - 1);
			}

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
			if (fileItem.Type == FileType.Effect)
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
