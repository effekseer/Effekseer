using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Menu
{
	class RecentFilesMenuManager
	{
		public Menu Menu { get; } = null;

		public RecentFilesMenuManager()
		{
			Menu = new Menu();
			Menu.Label = new MultiLanguageString("RecentFiles");
			Menu.Icon = Icons.Empty;
		}
		
		public void Reload()
		{
			Menu.Controls.Clear();

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
				Menu.Controls.Add(item);
			}
		}
	}
}
