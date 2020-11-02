using System;
using System.Linq;
using Effekseer.Utils;

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

		private static void AddRange(Menu menu, IControl[] controls)
		{
			foreach (IControl control in controls)
			{
				menu.Controls.Add(control);
			}
		}

		public static Menu SetupFilesMenu(RecentFilesMenuManager recentFiles)
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
				};
				import_menu.Controls.Add(item);
			}

			return import_menu;
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
	}
}
