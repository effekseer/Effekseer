namespace Effekseer.GUI.Menu
{
	public sealed class WindowTitleControl
	{
		public string CurrentTitle { get; set; } = string.Empty;

		public void Reload()
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

			if (CurrentTitle != newTitle)
			{
				CurrentTitle = newTitle;
				Manager.NativeManager.SetTitle(CurrentTitle);
			}
		}
	}
}
