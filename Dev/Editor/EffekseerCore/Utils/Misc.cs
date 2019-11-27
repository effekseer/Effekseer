using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.Utils
{
	public class Misc
	{
		public static string GetRelativePath(string basePath, string path)
		{
			Func<string, string> escape = (string s) =>
			{
				return s.Replace("%", "%25");
			};

			Func<string, string> unescape = (string s) =>
			{
				return s.Replace("%25", "%");
			};

			Uri basepath = new Uri(escape(basePath));
			Uri targetPath = new Uri(escape(path));
			return unescape(Uri.UnescapeDataString(basepath.MakeRelativeUri(targetPath).ToString()));
		}
		public static string GetAbsolutePath(string basePath, string path)
		{
			Func<string, string> escape = (string s) =>
			{
				return s.Replace("%", "%25");
			};

			Func<string, string> unescape = (string s) =>
			{
				return s.Replace("%25", "%");
			};

			Uri escaped = new Uri(escape(basePath));
			Uri targetPath = new Uri(escaped, escape(path));
			return unescape(Uri.UnescapeDataString(targetPath.LocalPath));
		}
	}
}
