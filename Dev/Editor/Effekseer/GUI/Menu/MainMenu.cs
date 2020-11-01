using System;
using System.Collections.Generic;
using System.Linq;

namespace Effekseer.GUI.Menu
{
    class MainMenu : IRemovableControl
    {
		private readonly RecentFilesMenuManager recentFilesMenuManager = new RecentFilesMenuManager();

        internal List<IControl> Controls = new List<IControl>();

		private string currentTitle = string.Empty;
		private bool isFirstUpdate = true;
		
		public bool ShouldBeRemoved { get; } = false;

		public MainMenu()
		{
			// assgin events
			Core.OnAfterNew += new EventHandler(Core_OnAfterNew);
			Core.OnAfterSave += new EventHandler(Core_OnAfterSave);
			Core.OnAfterLoad += new EventHandler(Core_OnAfterLoad);
			GUI.RecentFiles.OnChangeRecentFiles += new EventHandler(GUIManager_OnChangeRecentFiles);
		}

        public void Update()
        {
			if(isFirstUpdate)
			{
				ReloadMenu();
				ReloadTitle();
				isFirstUpdate = false;
			}

			if (Manager.IsWindowFrameless)
			{
				UpdateSystemBar();
			}
			else
			{
				Manager.NativeManager.BeginMainMenuBar();

				foreach (var ctrl in Controls)
				{
					ctrl.Update();
				}

				Manager.NativeManager.EndMainMenuBar();
			}

			ReloadTitle();
		}

		public void UpdateSystemBar()
		{
			Manager.NativeManager.PushStyleVar(swig.ImGuiStyleVarFlags.FramePadding, new swig.Vec2(0.0f, 8.0f * Manager.DpiScale));

			Manager.NativeManager.BeginMainMenuBar();

			var windowSize = Manager.NativeManager.GetWindowSize();

			float iconSize = 28.0f * Manager.DpiScale;
			Manager.NativeManager.SetCursorPosY((Manager.NativeManager.GetFrameHeight() - iconSize) / 2);
			Manager.NativeManager.Image(Images.GetIcon("AppIcon"), iconSize, iconSize);
			Manager.NativeManager.SetCursorPosY(0);

			foreach (var ctrl in Controls)
			{
				ctrl.Update();
			}

			{
				float pos = Manager.NativeManager.GetCursorPosX();
				float textWidth = Manager.NativeManager.CalcTextSize(currentTitle).X;
				float areaWidth = windowSize.X - pos - 56 * 3;
				if (textWidth < areaWidth)
				{
					Manager.NativeManager.SetCursorPosX(pos + (areaWidth - textWidth) / 2);
					Manager.NativeManager.Text(currentTitle);
				}
			}

			Manager.NativeManager.PushStyleColor(swig.ImGuiColFlags.Button, 0x00000000);
			Manager.NativeManager.PushStyleColor(swig.ImGuiColFlags.ButtonHovered, 0x20ffffff);

			float buttonX = 44 * Manager.DpiScale;
			float buttonY = 32 * Manager.DpiScale;

			Manager.NativeManager.SetCursorPosX(windowSize.X - buttonX * 3);
			if (Manager.NativeManager.ImageButtonOriginal(Images.GetIcon("ButtonMin"), buttonX, buttonY))
			{
				Manager.NativeManager.SetWindowMinimized(true);
			}

			bool maximized = Manager.NativeManager.IsWindowMaximized();
			Manager.NativeManager.SetCursorPosX(windowSize.X - buttonX * 2);
			if (Manager.NativeManager.ImageButtonOriginal(Images.GetIcon(maximized ? "ButtonMaxCancel" : "ButtonMax"), buttonX, buttonY))
			{
				Manager.NativeManager.SetWindowMaximized(!maximized);
			}

			Manager.NativeManager.SetCursorPosX(windowSize.X - buttonX * 1);
			if (Manager.NativeManager.ImageButtonOriginal(Images.GetIcon("ButtonClose"), buttonX, buttonY))
			{
				Manager.NativeManager.Close();
			}

			Manager.NativeManager.PopStyleColor(2);

			Manager.NativeManager.EndMainMenuBar();

			Manager.NativeManager.PopStyleVar(1);

		}

		private void ReloadRecentFiles()
		{
			recentFilesMenuManager.Reload();
		}

		private void ReloadTitle()
		{
			string filePath = Core.Root.GetFullPath();
			string fileName = string.IsNullOrEmpty(filePath) ? "NewFile" : System.IO.Path.GetFileName(filePath);
			var newTitle = "Effekseer Version " + Core.Version + " " + "[" + fileName + "] ";

			if (Core.IsChanged)
			{
				newTitle += Resources.GetString("UnsavedChanges");
			}

			if (swig.Native.IsDebugMode())
			{
				newTitle += " - DebugMode";
			}

			if (currentTitle != newTitle)
			{
				currentTitle = newTitle;
				Manager.NativeManager.SetTitle(currentTitle);
			}
		}

		private void ReloadMenu()
		{
			this.Controls.Add(SetupFilesMenu(recentFilesMenuManager));
			this.Controls.Add(SetupEditMenu());
			this.Controls.Add(SetupViewMenu());
			this.Controls.Add(WindowMenu.SetupWindowMenu());
			this.Controls.Add(SetupHelpMenu());
		}

		private Menu SetupHelpMenu()
		{
			var menu = new Menu(new MultiLanguageString("Help"));

			menu.Controls.Add(CreateMenuItemFromCommands(Commands.ViewHelp));
			//menu.Controls.Add(create_menu_item_from_commands(Commands.OpenSample));

			menu.Controls.Add(new MenuSeparator());

			menu.Controls.Add(CreateMenuItemFromCommands(Commands.About));

			return menu;
		}

		private Menu SetupViewMenu()
		{
			var view = new MultiLanguageString("View");

			var menu = new Menu(view);

			menu.Controls.Add(CreateMenuItemFromCommands(Commands.Play));
			menu.Controls.Add(CreateMenuItemFromCommands(Commands.Stop));
			menu.Controls.Add(CreateMenuItemFromCommands(Commands.Step));
			menu.Controls.Add(CreateMenuItemFromCommands(Commands.BackStep));

			return menu;
		}

		private Menu SetupEditMenu()
		{
			var edit = new MultiLanguageString("Edit");

			var menu = new Menu(edit);

			menu.Controls.Add(CreateMenuItemFromCommands(Commands.AddNode));
			menu.Controls.Add(CreateMenuItemFromCommands(Commands.InsertNode));
			menu.Controls.Add(CreateMenuItemFromCommands(Commands.RemoveNode));

			menu.Controls.Add(new MenuSeparator());

			menu.Controls.Add(CreateMenuItemFromCommands(Commands.Copy));
			menu.Controls.Add(CreateMenuItemFromCommands(Commands.Paste));
			menu.Controls.Add(CreateMenuItemFromCommands(Commands.PasteInfo));

			menu.Controls.Add(new MenuSeparator());

			menu.Controls.Add(CreateMenuItemFromCommands(Commands.Undo));
			menu.Controls.Add(CreateMenuItemFromCommands(Commands.Redo));

			return menu;
		}

		private static Menu SetupFilesMenu(RecentFilesMenuManager recentFiles)
		{
			var file = new MultiLanguageString("Files");
			var input = new MultiLanguageString("Import");
			var output = new MultiLanguageString("Export");

			var menu = new Menu(file);
			menu.Controls.Add(CreateMenuItemFromCommands(Commands.New));
			menu.Controls.Add(CreateMenuItemFromCommands(Commands.Open));
			menu.Controls.Add(CreateMenuItemFromCommands(Commands.Overwrite));
			menu.Controls.Add(CreateMenuItemFromCommands(Commands.SaveAs));

			menu.Controls.Add(new MenuSeparator());

			menu.Controls.Add(SetupImportSubMenu(input));

			menu.Controls.Add(SetupExportSubMenu(output));

			menu.Controls.Add(new MenuSeparator());

			{
				recentFiles.Reload();
				menu.Controls.Add(recentFiles.Menu);
			}

			menu.Controls.Add(new MenuSeparator());

			menu.Controls.Add(CreateMenuItemFromCommands(Commands.Exit));

			return menu;
		}
		

		private static MenuItem CreateMenuItemFromCommands(Func<bool> onClicked)
		{
			var item = new MenuItem();
			var attributes = onClicked.Method.GetCustomAttributes(false);
			var uniquename = UniqueNameAttribute.GetUniqueName(attributes);
			item.Label = NameAttribute.GetName(attributes);
			item.Shortcut = Shortcuts.GetShortcutText(uniquename);
			item.Clicked += () => { onClicked(); };

			return item;
		}

		private static Menu SetupExportSubMenu(MultiLanguageString output)
		{
			var export_menu = new Menu(output, Icons.Empty);

			for (int c = 0; c < Core.ExportScripts.Count; c++)
			{
				var item = new MenuItem();
				var script = Core.ExportScripts[c];
				item.Label = script.Title;
				item.Clicked += () =>
				{
					var filter = script.Filter.Split('.').Last();
					var result = swig.FileDialog.SaveDialog(filter, System.IO.Directory.GetCurrentDirectory());

					if (!string.IsNullOrEmpty(result))
					{
						if (System.IO.Path.GetExtension(result) != "." + filter)
						{
							result += "." + filter;
						}

						var filepath = result;
						script.Function(filepath);

						System.IO.Directory.SetCurrentDirectory(System.IO.Path.GetDirectoryName(filepath));
					}

					/*
							SaveFileDialog ofd = new SaveFileDialog();

							ofd.InitialDirectory = System.IO.Directory.GetCurrentDirectory();
							ofd.Filter = script.Filter;
							ofd.FilterIndex = 2;
							ofd.OverwritePrompt = true;

							if (ofd.ShowDialog() == DialogResult.OK)
							{
								var filepath = ofd.FileName;
								script.Function(filepath);

								System.IO.Directory.SetCurrentDirectory(System.IO.Path.GetDirectoryName(filepath));
							}
							else
							{
								return;
							}
							*/
				};
				export_menu.Controls.Add(item);
			}

			return export_menu;
		}

		private static Menu SetupImportSubMenu(MultiLanguageString input)
		{
			var import_menu = new Menu(input, Icons.Empty);

			for (int c = 0; c < Core.ImportScripts.Count; c++)
			{
				var item = new MenuItem();
				var script = Core.ImportScripts[c];
				item.Label = script.Title;
				item.Clicked += () =>
				{
					var filter = script.Filter.Split('.').Last();
					var result = swig.FileDialog.OpenDialog(filter, System.IO.Directory.GetCurrentDirectory());

					if (!string.IsNullOrEmpty(result))
					{
						var filepath = result;

						script.Function(filepath);
						System.IO.Directory.SetCurrentDirectory(System.IO.Path.GetDirectoryName(filepath));
					}

					/*
							OpenFileDialog ofd = new OpenFileDialog();

							ofd.InitialDirectory = System.IO.Directory.GetCurrentDirectory();
							ofd.Filter = script.Filter;
							ofd.FilterIndex = 2;
							ofd.Multiselect = false;

							if (ofd.ShowDialog() == DialogResult.OK)
							{
								var filepath = ofd.FileName;
								script.Function(filepath);

								System.IO.Directory.SetCurrentDirectory(System.IO.Path.GetDirectoryName(filepath));
							}
							else
							{
								return;
							}
							*/
				};
				import_menu.Controls.Add(item);
			}

			return import_menu;
		}

		private void MainForm_Load(object sender, EventArgs e)
		{
			ReloadTitle();
			ReloadMenu();
		}

		private void MainForm_Activated(object sender, EventArgs e)
		{
			if (Core.MainForm != null)
			{
				Core.Reload();
			}
		}

		private void Core_OnAfterNew(object sender, EventArgs e)
		{
			ReloadTitle();
		}

		private void Core_OnAfterSave(object sender, EventArgs e)
		{
			ReloadTitle();
		}

		private void Core_OnAfterLoad(object sender, EventArgs e)
		{
			ReloadTitle();
		}

		private void GUIManager_OnChangeRecentFiles(object sender, EventArgs e)
		{
			ReloadRecentFiles();
		}
	}
}
