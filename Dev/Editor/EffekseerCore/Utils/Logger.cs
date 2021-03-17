using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.Utils
{
	public static class Logger
	{
		public static string LogPath;

		static object lockObject = new object();

		public static void Write(string log)
		{
			try
			{
				if (!string.IsNullOrEmpty(log))
				{
					lock (lockObject)
					{
						System.IO.File.AppendAllText(LogPath, log + "\n");
					}
				}
			}
			catch(Exception e)
			{
				Console.WriteLine(e.ToString());
			}
		}
	}
}
