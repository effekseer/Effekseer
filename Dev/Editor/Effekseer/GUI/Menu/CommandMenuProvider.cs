using System;
using System.Linq;
using Effekseer.Script;

namespace Effekseer.GUI.Menu
{
	internal sealed class CommandMenuProvider
	{
		public static Menu SetupHelpMenu()
		{
			var menu = new Menu(new MultiLanguageString("Help"));

			menu.Controls.Add(CreateMenuItemFromCommands(Commands.ViewHelp));

			menu.Controls.Add(new MenuSeparator());

			menu.Controls.Add(CreateMenuItemFromCommands(Commands.About));

			return menu;
		}

		public static Menu SetupViewMenu()
		{
			var view = new MultiLanguageString("View");

			var menu = new Menu(view);

			menu.Controls.Add(CreateMenuItemFromCommands(Commands.Play));
			menu.Controls.Add(CreateMenuItemFromCommands(Commands.Stop));
			menu.Controls.Add(CreateMenuItemFromCommands(Commands.Step));
			menu.Controls.Add(CreateMenuItemFromCommands(Commands.BackStep));

			return menu;
		}

		public static Menu SetupEditMenu()
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

		public static Menu SetupFilesMenu(RecentFilesMenuManager recentFiles)
		{
			var menu = new Menu(new MultiLanguageString("Files"));
			
			recentFiles.Reload();
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

				recentFiles.Menu,

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
			item.Clicked += () => { onClicked(); };

			return item;
		}

		private static void AddRange(Menu menu, IControl[] controls)
		{
			foreach (IControl control in controls)
			{
				menu.Controls.Add(control);
			}
		}

		private static Menu SetupImportSubMenu(MultiLanguageString input)
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

		private static void Import(ImportScript script)
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
		

		private static Menu SetupExportSubMenu(MultiLanguageString output)
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

		private static void Export(ExportScript script)
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
