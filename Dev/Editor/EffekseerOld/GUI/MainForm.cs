using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using WeifenLuo.WinFormsUI.Docking;

namespace Effekseer.GUI
{
	partial class MainForm : Form
	{
		public MainForm()
		{
			InitializeComponent();

			// assgin events
			Core.OnAfterNew += new EventHandler(Core_OnAfterNew);
			Core.OnAfterSave += new EventHandler(Core_OnAfterSave);
			Core.OnAfterLoad += new EventHandler(Core_OnAfterLoad);
			RecentFiles.OnChangeRecentFiles += new EventHandler(GUIManager_OnChangeRecentFiles);

			recentFiles = new ToolStripMenuItem();
            recentFiles.Text = Properties.Resources.RecentFiles;
		}

		public DockPanel Panel
		{
			get
			{
				return dockPanel;
			}
		}

		/// <summary>
		/// recent files
		/// </summary>
		ToolStripMenuItem recentFiles = null;

		public void UpdateForm()
		{
			ReloadTitle();
		}

		void ReloadRecentFiles()
		{
			recentFiles.DropDownItems.Clear();

			var rf = RecentFiles.GetRecentFiles();

			foreach (var f in rf)
			{
				var item = new ToolStripMenuItem();
				var file = f;
				item.Text = file;
				item.Click += (object _sender, EventArgs _e) =>
				{
					Commands.Open(file);
				};
				recentFiles.DropDownItems.Add(item);
			}
		}

		void ReloadTitle()
		{
			var newTitle = "Effekseer Version " + Core.Version + " " + "[" + Core.FullPath + "] ";
			
			if (Core.IsChanged)
			{
				newTitle += Resources.GetString("UnsavedChanges");
			}

			if (Text != newTitle)
			{
				Text = newTitle;
			}
		}

		void ReloadMenu()
		{
			
			Func<Func<bool>, ToolStripMenuItem> create_menu_item_from_commands = (a) =>
			{
				var item = new ToolStripMenuItem();
				var attributes = a.Method.GetCustomAttributes(false);
				var uniquename = UniqueNameAttribute.GetUniqueName(attributes);
				item.Text = NameAttribute.GetName(attributes);
				item.ShowShortcutKeys = true;
				item.ShortcutKeyDisplayString = Shortcuts.GetShortcutText(uniquename);
				item.Click += (object sender, EventArgs e) =>
				{
					a();
				};

				return item;
			};

			this.menuStrip.SuspendLayout();
			this.SuspendLayout();

			{
				string file = string.Empty;
				string input = string.Empty;
				string output = string.Empty;


				file = Properties.Resources.Files;
				input = Properties.Resources.Import;
				output = Properties.Resources.Export;

				var menu = new ToolStripMenuItem(file);
				menu.DropDownItems.Add(create_menu_item_from_commands(Commands.New));
				menu.DropDownItems.Add(create_menu_item_from_commands(Commands.Open));
				menu.DropDownItems.Add(create_menu_item_from_commands(Commands.Overwrite));
				menu.DropDownItems.Add(create_menu_item_from_commands(Commands.SaveAs));

				menu.DropDownItems.Add(new ToolStripSeparator());
				{
					var import_menu = new ToolStripMenuItem(input);

					for (int c = 0; c < Core.ImportScripts.Count; c++)
					{
						var item = new ToolStripMenuItem();
						var script = Core.ImportScripts[c];
						item.Text = script.Title;
						item.Click += (object _sender, EventArgs _e) =>
						{
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
						};
						import_menu.DropDownItems.Add(item);
					}

					menu.DropDownItems.Add(import_menu);
				}

				{
					var export_menu = new ToolStripMenuItem(output);

					for (int c = 0; c < Core.ExportScripts.Count; c++)
					{
						var item = new ToolStripMenuItem();
						var script = Core.ExportScripts[c];
						item.Text = script.Title;
						item.Click += (object _sender, EventArgs _e) =>
						{
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
						};
						export_menu.DropDownItems.Add(item);
					}

					menu.DropDownItems.Add(export_menu);
				}
				menu.DropDownItems.Add(new ToolStripSeparator());
				{
					ReloadRecentFiles();
					menu.DropDownItems.Add(recentFiles);
				}
				menu.DropDownItems.Add(new ToolStripSeparator());

				menu.DropDownItems.Add(create_menu_item_from_commands(Commands.Exit));

				menuStrip.Items.Add(menu);
			}

			{
				string edit = Properties.Resources.Edit;
				string view = Properties.Resources.View;

				var menu = new ToolStripMenuItem(edit);

				menu.DropDownItems.Add(create_menu_item_from_commands(Commands.AddNode));
				menu.DropDownItems.Add(create_menu_item_from_commands(Commands.InsertNode));
				menu.DropDownItems.Add(create_menu_item_from_commands(Commands.RemoveNode));

				menu.DropDownItems.Add(new ToolStripSeparator());

				menu.DropDownItems.Add(create_menu_item_from_commands(Commands.Copy));
				menu.DropDownItems.Add(create_menu_item_from_commands(Commands.Paste));
				menu.DropDownItems.Add(create_menu_item_from_commands(Commands.PasteInfo));

				menu.DropDownItems.Add(new ToolStripSeparator());

				menu.DropDownItems.Add(create_menu_item_from_commands(Commands.Undo));
				menu.DropDownItems.Add(create_menu_item_from_commands(Commands.Redo));
				
				menuStrip.Items.Add(menu);
			}

			{
                string edit = Properties.Resources.Edit;
                string view = Properties.Resources.View;

				var menu = new ToolStripMenuItem(view);

				menu.DropDownItems.Add(create_menu_item_from_commands(Commands.Play));
				menu.DropDownItems.Add(create_menu_item_from_commands(Commands.Stop));
				menu.DropDownItems.Add(create_menu_item_from_commands(Commands.Step));
				menu.DropDownItems.Add(create_menu_item_from_commands(Commands.BackStep));

				menuStrip.Items.Add(menu);
			}

			{
				var menu = new ToolStripMenuItem(Properties.Resources.Window);

				{
					var item = new ToolStripMenuItem();
					item.Text = Properties.Resources.ResetWindow;
					item.Click += (object sender, EventArgs e) =>
					{
						GUIManager.CloseDockWindow();
						GUIManager.AssignDockWindowIntoDefaultPosition();
					};
					menu.DropDownItems.Add(item);
				}

				Action<string, Type, Image> setDockWindow = (s, t, icon) =>
					{
						var item = new ToolStripMenuItem();
						item.Text = s;
						item.Click += (object sender, EventArgs e) =>
						{
							GUIManager.SelectOrShowWindow(t);
						};
						item.Image = icon;
						menu.DropDownItems.Add(item);
					};

				setDockWindow(Properties.Resources.NodeTree, typeof(DockNodeTreeView), Properties.Resources.IconNodeTree);
				setDockWindow(Properties.Resources.BasicSettings, typeof(DockNodeCommonValues), Properties.Resources.IconCommon);
				setDockWindow(Properties.Resources.Position, typeof(DockNodeLocationValues), Properties.Resources.IconLocation);
				setDockWindow(Properties.Resources.AttractionForces, typeof(DockNodeLocationAbsValues), Properties.Resources.IconLocationAbs);
				setDockWindow(Properties.Resources.SpawningMethod, typeof(DockNodeGenerationLocationValues), Properties.Resources.IconGenerationLocation);
				setDockWindow(Properties.Resources.Rotation, typeof(DockNodeRotationValues), Properties.Resources.IconRotation);
				setDockWindow(Properties.Resources.Scale, typeof(DockNodeScaleValues), Properties.Resources.IconScale);
				setDockWindow(Properties.Resources.Scale, typeof(DockNodeDepthValues), Properties.Resources.IconScale);
				setDockWindow(Properties.Resources.BasicRenderSettings, typeof(DockNodeRendererCommonValues), Properties.Resources.IconRendererCommon);
				setDockWindow(Properties.Resources.RenderSettings, typeof(DockNodeRendererValues), Properties.Resources.IconRenderer);
				setDockWindow(Properties.Resources.Sound, typeof(DockNodeSoundValues), Properties.Resources.IconSound);
				setDockWindow(Properties.Resources.FCurves, typeof(DockFCurves), Properties.Resources.IconFCurve);
				setDockWindow(Properties.Resources.ViewerControls, typeof(DockViewerController), Properties.Resources.IconViewer);
				setDockWindow(Properties.Resources.CameraSettings, typeof(DockViewPoint), Properties.Resources.IconViewPoint);
				setDockWindow(Properties.Resources.Recorder, typeof(DockRecorder), Properties.Resources.IconRecorder);
				setDockWindow(Properties.Resources.Options, typeof(DockOption), Properties.Resources.IconOption);
				setDockWindow(Properties.Resources.Options, typeof(DockGlobal), Properties.Resources.IconOption);
				setDockWindow(Properties.Resources.Behavior, typeof(DockEffectBehavior), Properties.Resources.IconBehavior);
				setDockWindow(Properties.Resources.Culling, typeof(DockCulling), Properties.Resources.IconCulling);
				setDockWindow(Properties.Resources.Network, typeof(DockNetwork), Properties.Resources.IconNetwork);
				setDockWindow(Properties.Resources.FileViewer, typeof(DockFileViewer), Properties.Resources.IconFileViewer);

				menuStrip.Items.Add(menu);
			}

			{
                var menu = new ToolStripMenuItem(Properties.Resources.Help);
				
				menu.DropDownItems.Add(create_menu_item_from_commands(Commands.ViewHelp));
				menu.DropDownItems.Add(create_menu_item_from_commands(Commands.OpenSample));
				
				menu.DropDownItems.Add(new ToolStripSeparator());

				menu.DropDownItems.Add(create_menu_item_from_commands(Commands.About));

				menuStrip.Items.Add(menu);
			}

			this.menuStrip.ResumeLayout(false);
			this.menuStrip.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();
		}

		protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
		{
			bool handle = false;
			Shortcuts.ProcessCmdKey(ref msg, keyData, ref handle);
			if (handle) return true;

			return base.ProcessCmdKey(ref msg, keyData);
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

		private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
		{
			if (Commands.SaveOnDisposing())
			{

			}
			else
			{
				e.Cancel = true;
				return;
			}

			GUIManager.SaveConfig();
			Shortcuts.SeveShortcuts();
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
			this.menuStrip.SuspendLayout();
			ReloadRecentFiles();
			this.menuStrip.ResumeLayout();
		}

		public Point BeforeResizeLocation
		{
			get;
			private set;
		}

		public int BeforeResizeWidth
		{
			get;
			private set;
		}

		public int BeforeResizeHeight
		{
			get;
			private set;
		}
 
		private void MainForm_Resize(object sender, EventArgs e)
		{
			// Save a size before maximization or miniimization
			if (this.WindowState != FormWindowState.Maximized && this.WindowState != FormWindowState.Minimized)
			{
				BeforeResizeWidth = this.Width;
				BeforeResizeHeight = this.Height;
			}
		}

		private void MainForm_Move(object sender, EventArgs e)
		{
			// Save a location before maximization or miniimization
			if (this.WindowState != FormWindowState.Maximized && this.WindowState != FormWindowState.Minimized)
				BeforeResizeLocation = this.Location;
		}
	}
}
