using System;
using System.Collections.Generic;
using Effekseer.swig;

namespace Effekseer.GUI.Menu
{
    class MainMenu : IRemovableControl
    {
		private readonly RecentFilesMenuManager recentFilesMenuManager = new RecentFilesMenuManager();
		private readonly CommandMenuProvider commandMenuProvider;
		private readonly WindowTitleControl windowTitleControl = new WindowTitleControl();

        internal List<IControl> Controls = new List<IControl>();

		private bool isFirstUpdate = true;
		
		public bool ShouldBeRemoved { get; } = false;

		public MainMenu()
		{
			// assgin events
			Core.OnAfterNew += (sender, e) => windowTitleControl.Reload();
			Core.OnAfterSave += (sender, e) => windowTitleControl.Reload();
			Core.OnAfterLoad += (sender, e) => windowTitleControl.Reload();
			RecentFiles.OnChangeRecentFiles += (sender, e) => recentFilesMenuManager.Reload();

			commandMenuProvider = new CommandMenuProvider(recentFilesMenuManager);
		}

        public void Update()
        {
			if(isFirstUpdate)
			{
				ReloadMenu();
				windowTitleControl.Reload();
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

			windowTitleControl.Reload();
		}

		public void UpdateSystemBar()
		{
			Manager.NativeManager.PushStyleVar(swig.ImGuiStyleVarFlags.FramePadding, new swig.Vec2(0.0f, 8.0f * Manager.DpiScale));
			Manager.NativeManager.BeginMainMenuBar();

			var windowSize = Manager.NativeManager.GetWindowSize();

			AddAppIcon();
			AddCommands();
			AddTitle(windowSize);
			AddWindowButtons(windowSize);

			Manager.NativeManager.EndMainMenuBar();
			Manager.NativeManager.PopStyleVar(1);
		}

		private static void AddWindowButtons(Vec2 windowSize)
		{
			void ShowButton(int offset, ImageResource icon, Action onClick)
			{
				float buttonX = 44 * Manager.DpiScale;
				float buttonY = 32 * Manager.DpiScale;

				Manager.NativeManager.SetCursorPosX(windowSize.X - buttonX * offset);
				if (Manager.NativeManager.ImageButtonOriginal(icon, buttonX, buttonY))
				{
					onClick();
				}
			}

			Manager.NativeManager.PushStyleColor(swig.ImGuiColFlags.Button, 0x00000000);
			Manager.NativeManager.PushStyleColor(swig.ImGuiColFlags.ButtonHovered, 0x20ffffff);

			ShowButton(3, Images.GetIcon("ButtonMin"), () => Manager.NativeManager.SetWindowMinimized(true));

			bool maximized = Manager.NativeManager.IsWindowMaximized();
			ShowButton(2, Images.GetIcon(maximized ? "ButtonMaxCancel" : "ButtonMax"),
				() => Manager.NativeManager.SetWindowMaximized(!maximized));

			ShowButton(1, Images.GetIcon("ButtonClose"), () => Manager.NativeManager.Close());

			Manager.NativeManager.PopStyleColor(2);
		}

		private void AddTitle(Vec2 windowSize)
		{
			float pos = Manager.NativeManager.GetCursorPosX();
			float textWidth = Manager.NativeManager.CalcTextSize(windowTitleControl.CurrentTitle).X;
			float areaWidth = windowSize.X - pos - 56 * 3;
			if (textWidth < areaWidth)
			{
				Manager.NativeManager.SetCursorPosX(pos + (areaWidth - textWidth) / 2);
				Manager.NativeManager.Text(windowTitleControl.CurrentTitle);
			}
		}

		private void AddCommands()
		{
			foreach (var ctrl in Controls)
			{
				ctrl.Update();
			}
		}

		private static void AddAppIcon()
		{
			float iconSize = 28.0f * Manager.DpiScale;
			Manager.NativeManager.SetCursorPosY((Manager.NativeManager.GetFrameHeight() - iconSize) / 2);
			Manager.NativeManager.Image(Images.GetIcon("AppIcon"), iconSize, iconSize);
			Manager.NativeManager.SetCursorPosY(0);
		}

		private void ReloadMenu()
		{
			this.Controls.Add(commandMenuProvider.SetupFilesMenu());
			this.Controls.Add(commandMenuProvider.SetupEditMenu());
			this.Controls.Add(commandMenuProvider.SetupViewMenu());
			this.Controls.Add(WindowMenu.SetupWindowMenu());
			this.Controls.Add(commandMenuProvider.SetupHelpMenu());
		}
	}
}
