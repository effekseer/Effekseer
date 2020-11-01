using System;
using System.Collections.Generic;
using System.Linq;

namespace Effekseer.GUI.Menu
{
    class MainMenu : IRemovableControl
    {
        public bool ShouldBeRemoved { get; } = false;

        internal List<IControl> Controls = new List<IControl>();

		string currentTitle = string.Empty;

		readonly Menu recentFiles = null;

		bool isFirstUpdate = true;

		public MainMenu()
		{
			// assgin events
			Core.OnAfterNew += new EventHandler(Core_OnAfterNew);
			Core.OnAfterSave += new EventHandler(Core_OnAfterSave);
			Core.OnAfterLoad += new EventHandler(Core_OnAfterLoad);
			RecentFiles.OnChangeRecentFiles += new EventHandler(GUIManager_OnChangeRecentFiles);

			recentFiles = new Menu();
			recentFiles.Label = new MultiLanguageString("RecentFiles");
			recentFiles.Icon = Icons.Empty;
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

		void ReloadRecentFiles()
		{
			recentFiles.Controls.Clear();

			var rf = RecentFiles.GetRecentFiles();

			foreach (var f in rf)
			{
				var item = new MenuItem();
				var file = f;
				item.Label = file;
				item.Clicked += () =>
				{
					Commands.Open(file);
				};
				recentFiles.Controls.Add(item);
			}
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
			Section1();
			Section2();
			Section3();
			SetupWindowMenu();
			Section5();
		}

		private void Section5()
		{
			var menu = new Menu(new MultiLanguageString("Help"));

			menu.Controls.Add(CreateMenuItemFromCommands(Commands.ViewHelp));
			//menu.Controls.Add(create_menu_item_from_commands(Commands.OpenSample));

			menu.Controls.Add(new MenuSeparator());

			menu.Controls.Add(CreateMenuItemFromCommands(Commands.About));

			this.Controls.Add(menu);
		}

		private sealed class DockSettings
		{
			public string Title { get; }
			private readonly Type _type;
			private readonly string _iconName;

			public DockSettings(string title, Type type, string iconName)
			{
				Title = title;
				_type = type;
				_iconName = iconName;
			}

			public void AddTo(Menu menu)
			{
				var item = new MenuItem();
				item.Label = new MultiLanguageString(Title);
				item.Clicked += () => { Manager.SelectOrShowWindow(_type, new swig.Vec2(300, 300), true); };
				item.Icon = _iconName;
				menu.Controls.Add(item);
			}
		}

		private void SetupWindowMenu()
		{
			var menu = new Menu(new MultiLanguageString("Window"));

			{
				var item = new MenuItem();
				item.Label = new MultiLanguageString("ResetWindow");
				item.Clicked += () => { Manager.ResetWindow(); };
				menu.Controls.Add(item);
			}

			var settings = new[]
			{
				new DockSettings("NodeTree", typeof(Dock.NodeTreeView), Icons.PanelNodeTree),
				new DockSettings("BasicSettings", typeof(Dock.CommonValues), Icons.PanelCommon),
				new DockSettings("Position", typeof(Dock.LocationValues), Icons.PanelLocation),
				new DockSettings("AttractionForces", typeof(Dock.LocationAbsValues), Icons.PanelForceField),
				new DockSettings("SpawningMethod", typeof(Dock.GenerationLocationValues), Icons.PanelGeneration),
				new DockSettings("Rotation", typeof(Dock.RotationValues), Icons.PanelRotation),
				new DockSettings("Scale", typeof(Dock.ScaleValues), Icons.PanelScale),
				new DockSettings("Depth", typeof(Dock.DepthValues), Icons.PanelDepth),
				new DockSettings("RenderSettings", typeof(Dock.RendererValues), Icons.PanelRender),
				new DockSettings("BasicRenderSettings", typeof(Dock.RendererCommonValues), Icons.PanelRenderCommon),
				new DockSettings("AdvancedRenderSettings", typeof(Dock.AdvancedRenderCommonValues), Icons.PanelDynamicParams),
				new DockSettings("AdvancedRenderSettings2", typeof(Dock.AdvancedRenderCommonValues2), Icons.PanelDynamicParams),
				new DockSettings("Sound", typeof(Dock.SoundValues), Icons.PanelSound),
				new DockSettings("FCurves", typeof(Dock.FCurves), Icons.PanelFCurve),
				new DockSettings("Global", typeof(Dock.GlobalValues), Icons.PanelGlobal),
				new DockSettings("Culling", typeof(Dock.Culling), Icons.PanelCulling),
				new DockSettings("DynamicParameter_Name", typeof(Dock.Dynamic), Icons.PanelDynamicParams),
				new DockSettings("ViewerControls", typeof(Dock.ViewerController), Icons.PanelViewerCtrl),
				new DockSettings("CameraSettings", typeof(Dock.ViewPoint), Icons.PanelViewPoint),
				new DockSettings("Environment_Name", typeof(Dock.Environement), Icons.PanelEnvironment),
				new DockSettings("Behavior", typeof(Dock.BehaviorValues), Icons.PanelBehavior),
				new DockSettings("Network", typeof(Dock.Network), Icons.PanelNetwork),
				new DockSettings("Recorder", typeof(Dock.Recorder), Icons.PanelRecorder),
				new DockSettings("FileViewer", typeof(Dock.FileViewer), Icons.PanelFileViewer),
				new DockSettings("Options", typeof(Dock.Option), Icons.PanelOptions),
			};

			foreach (var setting in settings)
			{
				setting.AddTo(menu);
				if (setting.Title == "DynamicParameter_Name")
				{
					menu.Controls.Add(new MenuSeparator());
				}
			}

			this.Controls.Add(menu);
		}

		private void Section3()
		{
			var view = new MultiLanguageString("View");

			var menu = new Menu(view);

			menu.Controls.Add(CreateMenuItemFromCommands(Commands.Play));
			menu.Controls.Add(CreateMenuItemFromCommands(Commands.Stop));
			menu.Controls.Add(CreateMenuItemFromCommands(Commands.Step));
			menu.Controls.Add(CreateMenuItemFromCommands(Commands.BackStep));

			this.Controls.Add(menu);
		}

		private void Section2()
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

			this.Controls.Add(menu);
		}

		private void Section1()
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

			menu.Controls.Add(Section11(input));

			menu.Controls.Add(Section12(output));

			menu.Controls.Add(new MenuSeparator());

			{
				ReloadRecentFiles();
				menu.Controls.Add(recentFiles);
			}

			menu.Controls.Add(new MenuSeparator());

			menu.Controls.Add(CreateMenuItemFromCommands(Commands.Exit));

			this.Controls.Add(menu);
		}

		private static Menu Section12(MultiLanguageString output)
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

		private static Menu Section11(MultiLanguageString input)
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

		private MenuItem CreateMenuItemFromCommands(Func<bool> onClicked)
		{
			var item = new MenuItem();
			var attributes = onClicked.Method.GetCustomAttributes(false);
			var uniquename = UniqueNameAttribute.GetUniqueName(attributes);
			item.Label = NameAttribute.GetName(attributes);
			item.Shortcut = Shortcuts.GetShortcutText(uniquename);
			item.Clicked += () => { onClicked(); };

			return item;
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
