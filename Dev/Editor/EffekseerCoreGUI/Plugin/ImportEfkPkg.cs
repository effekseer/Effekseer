
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
				return Resources.GetString("EfkPkgFormat");
			}
		}

		public static string Description
		{
			get
			{
				return Resources.GetString("ImportEfkPkgFormatDescription");
			}
		}

		public static string Filter
		{
			get
			{
				return Resources.GetString("EfkPkgFormatFilter");
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