using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Effekseer.IO;

namespace Effekseer.Plugin
{
	class ExportEfkPkg
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
				return Resources.GetString("ExportEfkPkgFormatDescription");
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
			var efkpkg = new EfkPkg();
			efkpkg.AddCurrentEffect();
			efkpkg.Export(path);
		}
	}
}
