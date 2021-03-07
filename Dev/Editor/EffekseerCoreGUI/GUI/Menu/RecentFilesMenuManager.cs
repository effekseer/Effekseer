namespace Effekseer.GUI.Menu
{
	public sealed class RecentFilesMenuManager
	{
		public Menu Menu { get; } = new Menu
		{
			Label = new MultiLanguageString("RecentFiles"),
			Icon = Icons.Empty
		};

		public void Reload()
		{
			Menu.Controls.Clear();

			foreach (var file in RecentFiles.GetRecentFiles())
			{
				var item = new MenuItem();
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
