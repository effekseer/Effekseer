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
			recentFiles.Label = Resources.GetString("RecentFiles");
		}

        public void Update()
        {
			if(isFirstUpdate)
			{
				ReloadMenu();
				ReloadTitle();
				isFirstUpdate = false;
			}

            Manager.NativeManager.BeginMainMenuBar();

            foreach (var ctrl in Controls)
            {
                ctrl.Update();
            }

            Manager.NativeManager.EndMainMenuBar();

			ReloadTitle();
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
			var newTitle = "Effekseer Version " + Core.Version + " " + "[" + Core.FullPath + "] ";

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
				string file = string.Empty;
				string input = string.Empty;
				string output = string.Empty;


				file = Resources.GetString("Files");
				input = Resources.GetString("Import");
				output = Resources.GetString("Export");

				var menu = new Menu(file);
				menu.Controls.Add(create_menu_item_from_commands(Commands.New));
				menu.Controls.Add(create_menu_item_from_commands(Commands.Open));
				menu.Controls.Add(create_menu_item_from_commands(Commands.Overwrite));
				menu.Controls.Add(create_menu_item_from_commands(Commands.SaveAs));

				menu.Controls.Add(new MenuSeparator());

				{
					var import_menu = new Menu(input);

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
					var export_menu = new Menu(output);

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
				string edit = Resources.GetString("Edit");

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
				string view = Resources.GetString("View");

				var menu = new Menu(view);

				menu.Controls.Add(create_menu_item_from_commands(Commands.Play));
				menu.Controls.Add(create_menu_item_from_commands(Commands.Stop));
				menu.Controls.Add(create_menu_item_from_commands(Commands.Step));
				menu.Controls.Add(create_menu_item_from_commands(Commands.BackStep));

				this.Controls.Add(menu);
			}

			{
				var menu = new Menu(Resources.GetString("Window"));

				{
					var item = new MenuItem();
					item.Label = Resources.GetString("ResetWindow");
					item.Clicked += () =>
					{
						Manager.ResetWindow();
					};
					menu.Controls.Add(item);
				}

				Action<string, Type, swig.ImageResource> setDockWindow = (s, t, icon) =>
				{
					var item = new MenuItem();
					item.Label = s;
					item.Clicked += () =>
					{
						var panel = Manager.SelectOrShowWindow(t, new swig.Vec2(300, 300));
						if(panel != null)
						{
							panel.InitialDockActive = 1;
						}
					};
					item.Icon = icon;
					menu.Controls.Add(item);
				};
				
				setDockWindow(Resources.GetString("NodeTree"), typeof(Dock.NodeTreeView), Images.GetIcon("PanelNodeTree"));
				setDockWindow(Resources.GetString("BasicSettings"), typeof(Dock.CommonValues), Images.GetIcon("PanelCommon"));
				setDockWindow(Resources.GetString("Position"), typeof(Dock.LocationValues), Images.GetIcon("PanelLocation"));
				setDockWindow(Resources.GetString("AttractionForces"), typeof(Dock.LocationAbsValues), Images.GetIcon("PanelLocationAbs"));
				setDockWindow(Resources.GetString("SpawningMethod"), typeof(Dock.GenerationLocationValues), Images.GetIcon("PanelGenerationLocation"));
				setDockWindow(Resources.GetString("Rotation"), typeof(Dock.RotationValues), Images.GetIcon("PanelRotation"));
				setDockWindow(Resources.GetString("Scale"), typeof(Dock.ScaleValues), Images.GetIcon("PanelScale"));
				setDockWindow(Resources.GetString("Depth"), typeof(Dock.DepthValues), Images.GetIcon("PanelDepth"));
				setDockWindow(Resources.GetString("BasicRenderSettings"), typeof(Dock.RendererCommonValues), Images.GetIcon("PanelRendererCommon"));
				setDockWindow(Resources.GetString("RenderSettings"), typeof(Dock.RendererValues), Images.GetIcon("PanelRenderer"));
				setDockWindow(Resources.GetString("Sound"), typeof(Dock.SoundValues), Images.GetIcon("PanelSound"));
				setDockWindow(Resources.GetString("FCurves"), typeof(Dock.FCurves), Images.GetIcon("PanelFCurve"));
				setDockWindow(Resources.GetString("ViewerControls"), typeof(Dock.ViewerController), Images.GetIcon("PanelViewer"));
				setDockWindow(Resources.GetString("CameraSettings"), typeof(Dock.ViewPoint), Images.GetIcon("PanelViewPoint"));
				setDockWindow(Resources.GetString("Recorder"), typeof(Dock.Recorder), Images.GetIcon("PanelRecorder"));
				setDockWindow(Resources.GetString("Options"), typeof(Dock.Option), Images.GetIcon("PanelOption"));
				setDockWindow(Resources.GetString("Global"), typeof(Dock.GlobalValues), Images.GetIcon("PanelGlobal"));
				setDockWindow(Resources.GetString("Behavior"), typeof(Dock.BehaviorValues), Images.GetIcon("PanelBehavior"));
				setDockWindow(Resources.GetString("Culling"), typeof(Dock.Culling), Images.GetIcon("PanelCulling"));
                setDockWindow(Resources.GetString("Network"), typeof(Dock.Network), Images.GetIcon("PanelNetwork"));
				setDockWindow(Resources.GetString("FileViewer"), typeof(Dock.FileViewer), Images.GetIcon("PanelFileViewer"));

				this.Controls.Add(menu);
			}

			{
				var menu = new Menu(Resources.GetString("Help"));

				menu.Controls.Add(create_menu_item_from_commands(Commands.ViewHelp));
				menu.Controls.Add(create_menu_item_from_commands(Commands.OpenSample));

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
