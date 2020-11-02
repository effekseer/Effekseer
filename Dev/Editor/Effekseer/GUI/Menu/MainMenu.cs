using System;
using System.Collections.Generic;
using System.Linq;

namespace Effekseer.GUI.Menu
{
    class MainMenu : IRemovableControl
    {
		private readonly RecentFilesMenuManager recentFilesMenuManager = new RecentFilesMenuManager();
		private readonly CommandMenuProvider commandMenuProvider;

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

			commandMenuProvider = new CommandMenuProvider(recentFilesMenuManager);
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
			this.Controls.Add(commandMenuProvider.SetupFilesMenu());
			this.Controls.Add(commandMenuProvider.SetupEditMenu());
			this.Controls.Add(commandMenuProvider.SetupViewMenu());
			this.Controls.Add(WindowMenu.SetupWindowMenu());
			this.Controls.Add(commandMenuProvider.SetupHelpMenu());
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
