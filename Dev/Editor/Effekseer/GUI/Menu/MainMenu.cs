using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Menu
{
    class MainMenu : IRemovableControl
    {
        public bool ShouldBeRemoved { get; private set; } = false;

        internal List<IControl> Controls = new List<IControl>();

		string currentTitle = string.Empty;

		Menu recentFiles = null;

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

			float pos = Manager.NativeManager.GetCursorPosX();
			Manager.NativeManager.SetCursorPosX(pos + (windowSize.X - pos - 56 * 3 - 140) / 2);
			Manager.NativeManager.Text("Effekseer Version 1.51");

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

		void ReloadTitle()
		{
			var newTitle = "Effekseer Version " + Core.Version + " " + "[" + Core.Root.GetFullPath() + "] ";

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

		void ReloadMenu()
		{

			Func<Func<bool>, MenuItem> create_menu_item_from_commands = (a) =>
			{
				var item = new MenuItem();
				var attributes = a.Method.GetCustomAttributes(false);
				var uniquename = UniqueNameAttribute.GetUniqueName(attributes);
				item.Label = NameAttribute.GetName(attributes);
				item.Shortcut = Shortcuts.GetShortcutText(uniquename);
				item.Clicked += () =>
				{
					a();
				};

				return item;
			};

			{
				var file = new MultiLanguageString("Files");
				var input = new MultiLanguageString("Import");
				var output = new MultiLanguageString("Export");

				var menu = new Menu(file);
				menu.Controls.Add(create_menu_item_from_commands(Commands.New));
				menu.Controls.Add(create_menu_item_from_commands(Commands.Open));
				menu.Controls.Add(create_menu_item_from_commands(Commands.Overwrite));
				menu.Controls.Add(create_menu_item_from_commands(Commands.SaveAs));

				menu.Controls.Add(new MenuSeparator());

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
							else
							{
								return;
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

					menu.Controls.Add(import_menu);
				}

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
							else
							{
								return;
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

					menu.Controls.Add(export_menu);
				}

				menu.Controls.Add(new MenuSeparator());

				{
					ReloadRecentFiles();
					menu.Controls.Add(recentFiles);
				}

				menu.Controls.Add(new MenuSeparator());

				menu.Controls.Add(create_menu_item_from_commands(Commands.Exit));

				this.Controls.Add(menu);
			}

			{
				var edit = new MultiLanguageString("Edit");

				var menu = new Menu(edit);

				menu.Controls.Add(create_menu_item_from_commands(Commands.AddNode));
				menu.Controls.Add(create_menu_item_from_commands(Commands.InsertNode));
				menu.Controls.Add(create_menu_item_from_commands(Commands.RemoveNode));

				menu.Controls.Add(new MenuSeparator());

				menu.Controls.Add(create_menu_item_from_commands(Commands.Copy));
				menu.Controls.Add(create_menu_item_from_commands(Commands.Paste));
				menu.Controls.Add(create_menu_item_from_commands(Commands.PasteInfo));

				menu.Controls.Add(new MenuSeparator());

				menu.Controls.Add(create_menu_item_from_commands(Commands.Undo));
				menu.Controls.Add(create_menu_item_from_commands(Commands.Redo));

				this.Controls.Add(menu);
			}

			{
				var view = new MultiLanguageString("View");

				var menu = new Menu(view);

				menu.Controls.Add(create_menu_item_from_commands(Commands.Play));
				menu.Controls.Add(create_menu_item_from_commands(Commands.Stop));
				menu.Controls.Add(create_menu_item_from_commands(Commands.Step));
				menu.Controls.Add(create_menu_item_from_commands(Commands.BackStep));

				this.Controls.Add(menu);
			}

			{
				var menu = new Menu(new MultiLanguageString("Window"));

				{
					var item = new MenuItem();
					item.Label = new MultiLanguageString("ResetWindow");
					item.Clicked += () =>
					{
						Manager.ResetWindow();
					};
					menu.Controls.Add(item);
				}

				Action<MultiLanguageString, Type, string> setDockWindow = (s, t, icon) =>
				{
					var item = new MenuItem();
					item.Label = s;
					item.Clicked += () =>
					{
						Manager.SelectOrShowWindow(t, new swig.Vec2(300, 300), true);
					};
					item.Icon = icon;
					menu.Controls.Add(item);
				};
				
				setDockWindow(new MultiLanguageString("NodeTree"), typeof(Dock.NodeTreeView), Icons.PanelNodeTree);
				setDockWindow(new MultiLanguageString("BasicSettings"), typeof(Dock.CommonValues), Icons.PanelCommon);
				setDockWindow(new MultiLanguageString("Position"), typeof(Dock.LocationValues), Icons.PanelLocation);
				setDockWindow(new MultiLanguageString("AttractionForces"), typeof(Dock.LocationAbsValues), Icons.PanelForceField);
				setDockWindow(new MultiLanguageString("SpawningMethod"), typeof(Dock.GenerationLocationValues), Icons.PanelGeneration);
				setDockWindow(new MultiLanguageString("Rotation"), typeof(Dock.RotationValues), Icons.PanelRotation);
				setDockWindow(new MultiLanguageString("Scale"), typeof(Dock.ScaleValues), Icons.PanelScale);
				setDockWindow(new MultiLanguageString("Depth"), typeof(Dock.DepthValues), Icons.PanelDepth);
				setDockWindow(new MultiLanguageString("BasicRenderSettings"), typeof(Dock.RendererCommonValues), Icons.PanelRenderCommon);
				setDockWindow(new MultiLanguageString("RenderSettings"), typeof(Dock.RendererValues), Icons.PanelRender);
#if __EFFEKSEER_BUILD_VERSION16__
				setDockWindow(new MultiLanguageString("AdvancedRenderSettings"), typeof(Dock.AdvancedRenderCommonValues), Icons.PanelDynamicParams);
#endif
				setDockWindow(new MultiLanguageString("Sound"), typeof(Dock.SoundValues), Icons.PanelSound);
				setDockWindow(new MultiLanguageString("FCurves"), typeof(Dock.FCurves), Icons.PanelFCurve);
				setDockWindow(new MultiLanguageString("ViewerControls"), typeof(Dock.ViewerController), Icons.PanelViewerCtrl);
				setDockWindow(new MultiLanguageString("CameraSettings"), typeof(Dock.ViewPoint), Icons.PanelViewPoint);
				setDockWindow(new MultiLanguageString("Recorder"), typeof(Dock.Recorder), Icons.PanelRecorder);
				setDockWindow(new MultiLanguageString("Options"), typeof(Dock.Option), Icons.PanelOptions);
				setDockWindow(new MultiLanguageString("Environment_Name"), typeof(Dock.Environement), Icons.PanelEnvironment);
				setDockWindow(new MultiLanguageString("Global"), typeof(Dock.GlobalValues), Icons.PanelGlobal);
				setDockWindow(new MultiLanguageString("Behavior"), typeof(Dock.BehaviorValues), Icons.PanelBehavior);
				setDockWindow(new MultiLanguageString("Culling"), typeof(Dock.Culling), Icons.PanelCulling);
                setDockWindow(new MultiLanguageString("Network"), typeof(Dock.Network), Icons.PanelNetwork);
				setDockWindow(new MultiLanguageString("FileViewer"), typeof(Dock.FileViewer), Icons.PanelFileViewer);
				setDockWindow(new MultiLanguageString("DynamicParameter_Name"), typeof(Dock.Dynamic), Icons.PanelDynamicParams);

				this.Controls.Add(menu);
			}

			{
				var menu = new Menu(new MultiLanguageString("Help"));

				menu.Controls.Add(create_menu_item_from_commands(Commands.ViewHelp));
				//menu.Controls.Add(create_menu_item_from_commands(Commands.OpenSample));

				menu.Controls.Add(new MenuSeparator());

				menu.Controls.Add(create_menu_item_from_commands(Commands.About));

				this.Controls.Add(menu);
			}
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

		void Core_OnAfterNew(object sender, EventArgs e)
		{
			ReloadTitle();
		}

		void Core_OnAfterSave(object sender, EventArgs e)
		{
			ReloadTitle();
		}

		void Core_OnAfterLoad(object sender, EventArgs e)
		{
			ReloadTitle();
		}

		void GUIManager_OnChangeRecentFiles(object sender, EventArgs e)
		{
			ReloadRecentFiles();
		}
	}
}
