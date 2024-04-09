using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Drawing;

namespace Effekseer.GUI.Dock
{
	public class FileBrowser : DockPanel
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

		const string OptionMenuPopupId = "###FileBrowserOptionMenu";
		const string ContextMenuPopupId = "###FileBrowserContextMenu";

		public FileBrowser()
		{
			Label = Icons.PanelFileBrowser + MultiLanguageTextProvider.GetText("FileBrowser") + "###FileVeiwer";
			DocPage = "fileviewer.html";

			TabToolTip = MultiLanguageTextProvider.GetText("FileBrowser");

			menuOpenFile = MultiLanguageTextProvider.GetText("FileBrowser_OpenFile");

			if (swig.GUIManager.IsMacOSX())
			{
				menuShowInFileManager = MultiLanguageTextProvider.GetText("FileBrowser_ShowInFinder");
			}
			else
			{
				menuShowInFileManager = MultiLanguageTextProvider.GetText("FileBrowser_ShowInExplorer");
			}

			menuImportFromPackage = MultiLanguageTextProvider.GetText("FileBrowser_ImportFromPackage");
			menuExportToPackage = MultiLanguageTextProvider.GetText("FileBrowser_ExportToPackage");

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

		protected override void UpdateToolbar()
		{
			if (Manager.NativeManager.BeginMenuBar())
			{
				Manager.NativeManager.PushStyleColor(swig.ImGuiColFlags.Button, 0);
				//Manager.NativeManager.SetCursorPosY(Manager.NativeManager.GetCursorPosY() + 4);

				var padding = Manager.NativeManager.GetStyleVar2(swig.ImGuiStyleVarFlags.WindowPadding);
				var windowSize = Manager.NativeManager.GetWindowSize();
				float regionWidth = windowSize.X - padding.X * 2.0f;
				float buttonSize = 20 * Manager.DpiScale;
				float spacing = Manager.NativeManager.GetStyleVar2(swig.ImGuiStyleVarFlags.ItemSpacing).X;

				// Back directory
				if (Manager.NativeManager.IconButton(Icons.ArrowUp) &&
					!String.IsNullOrEmpty(CurrentPath))
				{
					UpdateFileListWithProjectPath(CurrentPath);
				}

				// Display current directory
				if (addressActivated || addressEditing)
				{
					Manager.NativeManager.PushItemWidth(regionWidth - buttonSize * 3 - spacing * 3);

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

					Manager.NativeManager.PushStyleColor(swig.ImGuiColFlags.Button, 0x20FFFFFF);

					if (Manager.NativeManager.Button(label, regionWidth - buttonSize * 3 - spacing * 3))
					{
						addressActivated = true;
					}

					Manager.NativeManager.PopStyleColor();
				}

				if (Manager.NativeManager.IconButton(Icons.NavMenu))
				{
					Manager.NativeManager.OpenPopup(OptionMenuPopupId);
				} 

				UpdateOptionMenu();

				if (Manager.NativeManager.IconButton(Icons.Help))
				{
					Commands.ShowURL(DocURL);
				}

				if (BindableComponent.Functions.CanShowTip())
				{
					Manager.NativeManager.SetTooltip(MultiLanguageTextProvider.GetText("Panel_Help_Desc"));
				}

				Manager.NativeManager.PopStyleColor();
				Manager.NativeManager.EndMenuBar();
			}
		}

		void UpdateOptionMenu()
		{
			float fontSize = Manager.NativeManager.GetTextLineHeight();

			Manager.NativeManager.SetNextWindowSize(fontSize * 20, 0.0f, swig.Cond.Always);

			if (Manager.NativeManager.BeginPopup(OptionMenuPopupId))
			{
				var viewMode = Core.Option.FileBrowserViewMode.GetValue();

				Manager.NativeManager.PushItemWidth(fontSize * 8);

				// View Mode setting
				string comboLabel = MultiLanguageTextProvider.GetText("Options_FileBrowserViewMode_Name");
				string comboSelectedLabel = MultiLanguageTextProvider.GetText("FileBrowser_ViewMode_" + Core.Option.FileBrowserViewMode.GetValue().ToString() + "_Name");
				if (Manager.NativeManager.BeginCombo(comboLabel, comboSelectedLabel, swig.ComboFlags.None))
				{
					string iconViewLabel = MultiLanguageTextProvider.GetText("FileBrowser_ViewMode_" + Data.OptionValues.FileViewMode.IconView.ToString() + "_Name");
					if (Manager.NativeManager.Selectable(iconViewLabel, viewMode == Data.OptionValues.FileViewMode.IconView))
					{
						Core.Option.FileBrowserViewMode.SetValueDirectly(Data.OptionValues.FileViewMode.IconView);
					}
					string listViewLabel = MultiLanguageTextProvider.GetText("FileBrowser_ViewMode_" + Data.OptionValues.FileViewMode.ListView.ToString() + "_Name");
					if (Manager.NativeManager.Selectable(listViewLabel, viewMode == Data.OptionValues.FileViewMode.ListView))
					{
						Core.Option.FileBrowserViewMode.SetValueDirectly(Data.OptionValues.FileViewMode.ListView);
					}
					Manager.NativeManager.EndCombo();
				}

				// Icon size setting
				if (viewMode == Data.OptionValues.FileViewMode.IconView)
				{
					int[] val = new int[1] { Core.Option.FileBrowserIconSize.GetValue() };
					if (Manager.NativeManager.DragInt(MultiLanguageTextProvider.GetText("Options_FileBrowserIconSize_Name"), val, 1, 32, 512, "%d"))
					{
						Core.Option.FileBrowserIconSize.SetValueDirectly(val[0]);
					}
				}

				Manager.NativeManager.PopItemWidth();

				Manager.NativeManager.EndPopup();
			}
		}

		void UpdateFileView(float dpiScale, swig.Vec2 regionSize, swig.Vec2 spacing)
		{
			var viewMode = Core.Option.FileBrowserViewMode.GetValue();
			float iconSize = Core.Option.FileBrowserIconSize.GetValue();

			int contentCountX = 1;

			if (viewMode == Data.OptionValues.FileViewMode.IconView)
			{
				contentCountX = (int)((regionSize.X + spacing.X * 2) / (iconSize + spacing.X));
				if (contentCountX <= 0) contentCountX = 1;
			}

			for (int i = 0; i < items.Count; i++)
			{
				var item = items[i];

				if (i != 0 && i % contentCountX != 0)
				{
					Manager.NativeManager.SameLine();
				}

				item.DrawPosX = Manager.NativeManager.GetCursorPosX();
				item.DrawPosY = Manager.NativeManager.GetCursorPosY();

				string idstr = "###FileContent_" + i;
				string label = Path.GetFileName(item.FilePath);
				bool selected = false;

				if (viewMode == Data.OptionValues.FileViewMode.IconView)
				{
					selected = Manager.NativeManager.SelectableContent(idstr, label,
						item.Selected, item.IconImage, iconSize, iconSize, swig.SelectableFlags.AllowDoubleClick);
				}
				else if (viewMode == Data.OptionValues.FileViewMode.ListView)
				{
					selected = Manager.NativeManager.Selectable(item.IconStr + label + idstr, 
						item.Selected, swig.SelectableFlags.AllowDoubleClick);
				}

				if (Manager.NativeManager.IsItemHovered(0.5f))
				{
					float tooltipBaseSize = dpiScale * 96.0f;
					Manager.NativeManager.SetNextWindowSize(tooltipBaseSize * 3.0f, 0.0f, swig.Cond.Always);
					Manager.NativeManager.BeginTooltip();

					Manager.NativeManager.Text("Name: " + item.FileName);

					if (item.Type != FileType.Other)
					{
						Manager.NativeManager.Text("Type: " + item.Type);
					}
					if (item.FileSize > 0)
					{
						Manager.NativeManager.Text("Size: " + item.FileSizeStr);
					}
					if (item.Type == FileType.Texture)
					{
						Manager.NativeManager.Text("Format: " + item.ImageSizeStr);
					}

					// Fullpath
					Manager.NativeManager.TextWrapped(item.FilePath);

					Manager.NativeManager.EndTooltip();
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
				if (item.Type != FileType.Directory)
				{
					DragAndDrops.UpdateFileSrc(item.FilePath, item.Type, () => {
						float imageSize = dpiScale * 96.0f;
						Manager.NativeManager.ImageData(item.IconImage, imageSize, imageSize);
						Manager.NativeManager.PushTextWrapPos(imageSize);
						Manager.NativeManager.Text(item.FileName);
						Manager.NativeManager.PopTextWrapPos();
					});
				}

				if (viewMode == Data.OptionValues.FileViewMode.ListView)
				{
					if (item.Type == FileType.Texture && item.IconImage != null)
					{
						Manager.NativeManager.SameLine();
						Manager.NativeManager.SetCursorPosX(item.DrawPosX);
						float listIconSize = Manager.NativeManager.GetTextLineHeight();
						Manager.NativeManager.ImageData(item.IconImage, listIconSize, listIconSize);
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

						switch (Core.Option.FileBrowserViewMode.GetValue())
						{
							case Data.OptionValues.FileViewMode.IconView:
								Manager.NativeManager.ScrollToRect(
									new swig.Vec2(relX, relY),
									new swig.Vec2(relX, relY + Core.Option.FileBrowserIconSize.GetValue()));
								break;
							case Data.OptionValues.FileViewMode.ListView:
								Manager.NativeManager.ScrollToRect(
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
			if (string.IsNullOrEmpty(CurrentPath))
			{
				UpdateFileListWithProjectPath(Core.Root.GetFullPath());
			}
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
			public string FileName { get; private set; }

			public string FilePath { get; private set; }

			public long FileSize { get; private set; }

			public FileType Type { get; private set; }

			public string IconStr { get; private set; }

			public swig.ReloadableImage IconImage { get; private set; }

			public bool Selected { get; set; }

			public int ItemID { get; set; }

			public float DrawPosX { get; set; }

			public float DrawPosY { get; set; }

			public FileItem(string fileName, string filePath, FileType type, long fileSize, 
				string iconStr, swig.ReloadableImage iconImage)
			{
				FileName = fileName;
				FilePath = filePath;
				Type = type;
				FileSize = fileSize;
				IconStr = iconStr;
				IconImage = iconImage;
			}

			public string FileSizeStr
			{
				get
				{
					if (FileSize >= 1024 * 1024)
					{
						return string.Format("{0:.##} MB", FileSize / 1024.0 / 1024.0);
					}
					else if (FileSize >= 1024)
					{
						return string.Format("{0:.##} KB", FileSize / 1024.0);
					}
					else
					{
						return string.Format("{0} B", FileSize);
					}
				}
			}

			public string ImageSizeStr
			{
				get
				{
					if (IconImage == null) return null;
					return string.Format("{0} x {1}", IconImage.GetWidth(), IconImage.GetHeight());
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

			// add directories
			foreach (string dirPath in Directory.EnumerateDirectories(path))
			{
				try
				{
					items.Add(CreateFileItem(dirPath, true));
				}
				catch (Exception e)
				{
					// Skip directory
				}
			}

			// add files
			foreach (string filePath in Directory.EnumerateFiles(path))
			{
				try
				{
					items.Add(CreateFileItem(filePath, false));
				}
				catch (Exception e)
				{
					// Skip file
				}
			}

			directoryWatcher.Path = path;
			directoryWatcher.EnableRaisingEvents = true;
		}

		private FileItem CreateFileItem(string filePath, bool isDirectory)
		{
			FileType type = FileType.Other;
			string iconStr = Icons.FileOther;
			swig.ReloadableImage iconImage = Images.Icons["FileOther128"];
			long fileSize = -1;

			if (isDirectory)
			{
				type = FileType.Directory;
				iconStr = Icons.FileDirectory;
				iconImage = Images.Icons["FileFolder128"];
			}
			else
			{
				var fileInfo = new FileInfo(filePath);
				fileSize = fileInfo.Length;

				switch (Path.GetExtension(filePath).ToLower())
				{
					case ".efkproj":
					case ".efkefc":
						type = FileType.Effect;
						iconStr = Icons.FileEfkefc;
						iconImage = Images.Icons["FileEfkefc128"];
						break;
					case ".png":
						type = FileType.Texture;
						iconStr = Icons.Empty; // Always show thumbnail
						iconImage = Images.Load(Manager.HardwareDevice.GraphicsDevice, filePath);
						break;
					case ".wav":
						type = FileType.Sound;
						iconStr = Icons.FileSound;
						iconImage = Images.Icons["FileSound128"];
						break;
					case ".efkmodel":
					case ".fbx":
					case ".mqo":
						type = FileType.Model;
						iconStr = Icons.FileModel;
						iconImage = Images.Icons["FileModel128"];
						break;
					case ".efkmat":
						type = FileType.Material;
						iconStr = Icons.FileEfkmat;
						iconImage = Images.Icons["FileEfkmat128"];
						break;
					case ".efkcurve":
						type = FileType.Curve;
						iconStr = Icons.PanelFCurve;
						iconImage = Images.Icons["FileCurve128"];
						break;
				}
			}

			return new FileItem(Path.GetFileName(filePath), filePath, type, fileSize, iconStr, iconImage);
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
					var info = new System.Diagnostics.ProcessStartInfo
					{
						FileName = fileItem.FilePath,
						UseShellExecute = true
					};
					System.Diagnostics.Process.Start(info);
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
					var info = new System.Diagnostics.ProcessStartInfo
					{
						FileName = "open",
						Arguments = dirPath,
						UseShellExecute = true
					};
					System.Diagnostics.Process.Start(info);
				}
				else
				{
					var info = new System.Diagnostics.ProcessStartInfo
					{
						FileName = dirPath,
						UseShellExecute = true
					};
					System.Diagnostics.Process.Start(info);
				}
			}
			catch (Exception)
			{
			}
		}
	}
}