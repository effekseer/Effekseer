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
			Func<string, string> doubleBackslashToSlash = (string s) =>
			{
				string input = s;
				string pattern = @"\\";
				string replacement = @"/";
				string result = System.Text.RegularExpressions.Regex.Replace(input, pattern, replacement);
				return result;
			};

			var basePath_ecs = new Uri(basePath, UriKind.Relative);
			var path_ecs = new Uri(path, UriKind.Relative);
			var basePath_slash = doubleBackslashToSlash(basePath_ecs.ToString());
			var basePath_uri = new Uri(basePath_slash, UriKind.Absolute);
			var path_uri = new Uri(path_ecs.ToString(), UriKind.Relative);
			var targetPath = new Uri(basePath_uri, path_uri);
			var ret = targetPath.LocalPath.ToString();
			return ret;
		}
	}
}
