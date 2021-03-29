using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Reflection;
using System.Runtime.ExceptionServices;

namespace Effekseer
{
	class Program
	{
		[STAThread]
		[HandleProcessCorruptedStateExceptions]
		static void Main(string[] args)
		{
			var app = new App();
			app.Initialize(true);
			app.Run();
			Core.Dispose();
		}
	}
}
