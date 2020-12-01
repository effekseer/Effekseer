
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
				if (Effekseer.Core.Language == Effekseer.Language.Japanese) return "Effekseerパッケージ";
				return "Effekseer package";
			}
		}

		public static string Description
		{
			get
			{
				if (Effekseer.Core.Language == Effekseer.Language.Japanese) return "Effekseerパッケージで出力する。";
				return "Export as Effekseer package";
			}
		}

		public static string Filter
		{
			get
			{
				if (Effekseer.Core.Language == Effekseer.Language.Japanese) return "Effekseerパッケージ (*.efkpkg)|*.efkpkg";
				return "Effekseer package (*.efkpkg)|*.efkpkg";
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