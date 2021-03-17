using System;
using System.Linq;
using Effekseer.Script;

namespace Effekseer.GUI.Menu
{
	public sealed class CommandMenuProvider
	{
		private readonly RecentFilesMenuManager _recentFiles;

		public CommandMenuProvider(RecentFilesMenuManager recentFiles)
		{
			_recentFiles = recentFiles;
		}

		public Menu SetupHelpMenu()
		{
			var menu = new Menu(new MultiLanguageString("Help"));

			AddRange(menu, new IControl[]
			{
				CreateMenuItemFromCommands(Commands.ViewHelp),

				new MenuSeparator(),

				CreateMenuItemFromCommands(Commands.About),
			});

			return menu;
		}

		public Menu SetupViewMenu()
		{
			var menu = new Menu(new MultiLanguageString("View"));

			AddRange(menu, new IControl[]
			{
				CreateMenuItemFromCommands(Commands.Play),
				CreateMenuItemFromCommands(Commands.Stop),
				CreateMenuItemFromCommands(Commands.Step),
				CreateMenuItemFromCommands(Commands.BackStep),
			});

			return menu;
		}

		public Menu SetupEditMenu()
		{
			var menu = new Menu(new MultiLanguageString("Edit"));

			AddRange(menu, new IControl[]
			{
				CreateMenuItemFromCommands(Commands.AddNode),
				CreateMenuItemFromCommands(Commands.InsertNode),
				CreateMenuItemFromCommands(Commands.RemoveNode),

				new MenuSeparator(),

				CreateMenuItemFromCommands(Commands.Copy),
				CreateMenuItemFromCommands(Commands.Paste),
				CreateMenuItemFromCommands(Commands.PasteInfo),

				new MenuSeparator(),

				CreateMenuItemFromCommands(Commands.Undo),
				CreateMenuItemFromCommands(Commands.Redo),
			});

			return menu;
		}

		public Menu SetupFilesMenu()
		{
			var menu = new Menu(new MultiLanguageString("Files"));

			_recentFiles.Reload();
			AddRange(menu, new IControl[]
			{
				CreateMenuItemFromCommands(Commands.New),
				CreateMenuItemFromCommands(Commands.Open),
				CreateMenuItemFromCommands(Commands.Overwrite),
				CreateMenuItemFromCommands(Commands.SaveAs),

				new MenuSeparator(),

				SetupImportSubMenu(new MultiLanguageString("Import")),
				SetupExportSubMenu(new MultiLanguageString("Export")),

				new MenuSeparator(),

				_recentFiles.Menu,

				new MenuSeparator(),

				CreateMenuItemFromCommands(Commands.Exit),
			});

			return menu;
		}

		private static MenuItem CreateMenuItemFromCommands(Func<bool> onClicked)
		{
			var item = new MenuItem();
			var attributes = onClicked.Method.GetCustomAttributes(false);
			var uniquename = UniqueNameAttribute.GetUniqueName(attributes);
			item.Label = NameAttribute.GetName(attributes);
			item.Shortcut = Shortcuts.GetShortcutText(uniquename);
			item.Clicked += () => onClicked();

			return item;
		}

		private void AddRange(Menu menu, IControl[] controls)
		{
			foreach (IControl control in controls)
			{
				menu.Controls.Add(control);
			}
		}

		private Menu SetupImportSubMenu(MultiLanguageString input)
		{
			var importMenu = new Menu(input, Icons.Empty);

			for (int c = 0; c < Core.ImportScripts.Count; c++)
			{
				var item = new MenuItem();
				var script = Core.ImportScripts[c];
				item.Label = script.Title;
				item.Clicked += () => Import(script);
				importMenu.Controls.Add(item);
			}

			return importMenu;
		}

		private void Import(ImportScript script)
		{
			var filter = script.Filter.Split('.').Last();
			var result = swig.FileDialog.OpenDialog(filter, System.IO.Directory.GetCurrentDirectory());

			if (string.IsNullOrEmpty(result))
			{
				return;
			}

			var filepath = result;

			script.Function(filepath);
			System.IO.Directory.SetCurrentDirectory(System.IO.Path.GetDirectoryName(filepath));
		}


		private Menu SetupExportSubMenu(MultiLanguageString output)
		{
			var exportMenu = new Menu(output, Icons.Empty);

			for (int c = 0; c < Core.ExportScripts.Count; c++)
			{
				var item = new MenuItem();
				var script = Core.ExportScripts[c];
				item.Label = script.Title;
				item.Clicked += () => Export(script);
				exportMenu.Controls.Add(item);
			}

			return exportMenu;
		}

		private void Export(ExportScript script)
		{
			var filter = script.Filter.Split('.').Last();
			var result = swig.FileDialog.SaveDialog(filter, System.IO.Directory.GetCurrentDirectory());

			if (string.IsNullOrEmpty(result))
			{
				return;
			}

			if (System.IO.Path.GetExtension(result) != "." + filter)
			{
				result += "." + filter;
			}

			var filepath = result;
			script.Function(filepath);

			System.IO.Directory.SetCurrentDirectory(System.IO.Path.GetDirectoryName(filepath));
		}
	}
}
