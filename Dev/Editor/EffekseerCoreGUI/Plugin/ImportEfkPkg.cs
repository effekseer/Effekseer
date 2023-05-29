
namespace Effekseer.Plugin
{

	public class ImportEfkPkg
	{
		public static string UniqueName
		{
			get
			{
				return "Efkpkg";
			}
		}

		public static string Author
		{
			get
			{
				return "Efkpkg";
			}
		}

		public static string Title
		{
			get
			{
				return MultiLanguageTextProvider.GetText("EfkPkgFormat");
			}
		}

		public static string Description
		{
			get
			{
				return MultiLanguageTextProvider.GetText("ImportEfkPkgFormatDescription");
			}
		}

		public static string Filter
		{
			get
			{
				return MultiLanguageTextProvider.GetText("EfkPkgFormatFilter");
			}
		}

		public static void Call(string path)
		{
			var efkpkg = new Effekseer.IO.EfkPkg();
			efkpkg.Import(path);

			var window = new Effekseer.GUI.Dialog.ImportEfkPkg();
			window.Show(path, efkpkg);
		}
	}

}