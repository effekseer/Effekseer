using System;
using System.Collections.Generic;
using Effekseer.swig;

namespace Effekseer.GUI.Menu
{
	internal sealed class MainMenu : IRemovableControl
	{
		private readonly RecentFilesMenuManager _recentFilesMenuManager = new RecentFilesMenuManager();
		private readonly CommandMenuProvider _commandMenuProvider;
		private readonly WindowTitleControl _windowTitleControl = new WindowTitleControl();
		private readonly WindowMenu _windowMenu = new WindowMenu();

		internal List<IControl> Controls = new List<IControl>();

		private bool _isFirstUpdate = true;
		
		public bool ShouldBeRemoved { get; } = false;

		public MainMenu()
		{
			// assgin events
			Core.OnAfterNew += (sender, e) => _windowTitleControl.Reload();
			Core.OnAfterSave += (sender, e) => _windowTitleControl.Reload();
			Core.OnAfterLoad += (sender, e) => _windowTitleControl.Reload();
			RecentFiles.OnChangeRecentFiles += (sender, e) => _recentFilesMenuManager.Reload();

			_commandMenuProvider = new CommandMenuProvider(_recentFilesMenuManager);
		}

		public void Update()
		{
			if(_isFirstUpdate)
			{
				ReloadMenu();
				_windowTitleControl.Reload();
				_isFirstUpdate = false;
			}

			if (GUIManager.IsWindowFrameless)
			{
				UpdateSystemBar();
			}
			else
			{
				GUIManager.NativeManager.BeginMainMenuBar();

				foreach (var ctrl in Controls)
				{
					ctrl.Update();
				}

				GUIManager.NativeManager.EndMainMenuBar();
			}

			_windowTitleControl.Reload();
		}

		public void UpdateSystemBar()
		{
			GUIManager.NativeManager.PushStyleVar(swig.ImGuiStyleVarFlags.FramePadding, new swig.Vec2(0.0f, 8.0f * GUIManager.DpiScale));
			GUIManager.NativeManager.BeginMainMenuBar();

			var windowSize = GUIManager.NativeManager.GetWindowSize();

			AddAppIcon();
			AddCommands();
			AddTitle(windowSize);
			AddWindowButtons(windowSize);

			GUIManager.NativeManager.EndMainMenuBar();
			GUIManager.NativeManager.PopStyleVar(1);
		}

		private void AddWindowButtons(Vec2 windowSize)
		{
			var size = GUIManager.NativeManager.GetWindowSize();
			float buttonY = size.Y - 1;
			float buttonX = buttonY * 44 / 32;

			void ShowButton(int offset, ImageResource icon, Action onClick)
			{
				GUIManager.NativeManager.SetCursorPosX(windowSize.X - buttonX * offset);
				if (GUIManager.NativeManager.ImageButtonOriginal(icon, buttonX, buttonY))
				{
					onClick();
				}
			}

			GUIManager.NativeManager.PushStyleColor(swig.ImGuiColFlags.Button, 0x00000000);
			GUIManager.NativeManager.PushStyleColor(swig.ImGuiColFlags.ButtonHovered, 0x20ffffff);

			ShowButton(3, Images.GetIcon("ButtonMin"), () => GUIManager.NativeManager.SetWindowMinimized(true));

			bool maximized = GUIManager.NativeManager.IsWindowMaximized();
			ShowButton(2, Images.GetIcon(maximized ? "ButtonMaxCancel" : "ButtonMax"),
				() => GUIManager.NativeManager.SetWindowMaximized(!maximized));

			ShowButton(1, Images.GetIcon("ButtonClose"), () => GUIManager.NativeManager.Close());

			GUIManager.NativeManager.PopStyleColor(2);
		}

		private void AddTitle(Vec2 windowSize)
		{
			var size = GUIManager.NativeManager.GetWindowSize();
			float buttonX = size.Y * 44 / 32;

			float pos = GUIManager.NativeManager.GetCursorPosX();
			float textWidth = GUIManager.NativeManager.CalcTextSize(_windowTitleControl.CurrentTitle).X;
			float areaWidth = windowSize.X - pos - buttonX * 3;
			if (textWidth < areaWidth)
			{
				GUIManager.NativeManager.SetCursorPosX(pos + (areaWidth - textWidth) / 2);
				GUIManager.NativeManager.Text(_windowTitleControl.CurrentTitle);
			}
		}

		private void AddCommands()
		{
			foreach (var ctrl in Controls)
			{
				ctrl.Update();
			}
		}

		private void AddAppIcon()
		{
			float iconSize = 28.0f * GUIManager.DpiScale;
			GUIManager.NativeManager.SetCursorPosY((GUIManager.NativeManager.GetFrameHeight() - iconSize) / 2);
			GUIManager.NativeManager.Image(Images.GetIcon("AppIcon"), iconSize, iconSize);
			GUIManager.NativeManager.SetCursorPosY(0);
		}

		private void ReloadMenu()
		{
			this.Controls.Add(_commandMenuProvider.SetupFilesMenu());
			this.Controls.Add(_commandMenuProvider.SetupEditMenu());
			this.Controls.Add(_commandMenuProvider.SetupViewMenu());
			this.Controls.Add(_windowMenu.SetupWindowMenu());
			this.Controls.Add(_commandMenuProvider.SetupHelpMenu());
		}
	}
}
