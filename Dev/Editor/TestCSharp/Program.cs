using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Reflection;
using System.Runtime.ExceptionServices;

namespace TestCSharp
{
	class Program
	{
		[STAThread]
		[HandleProcessCorruptedStateExceptions]
		static void Main(string[] args)
		{
			ZLib.Test();
			MaterialInformation.Test();

			var script = new InternalScriptTest();
			script.Test();
		}
	}

	class InternalScriptTest
	{
		public void Test()
		{

			Effekseer.InternalScript.Compiler compiler = new Effekseer.InternalScript.Compiler();
			Validate(compiler.Compile("1 + 2 + 3"), null);
			Validate(compiler.Compile("1 * 2 + 3"), null);
			Validate(compiler.Compile("1 * (2 + 3)"), null);

			Validate(compiler.Compile("@O.x = 1 + 2 + 3"), null);
			Validate(compiler.Compile("@O.x = 1 + 2 + 3\n@O.y = 1 + 2 + 3"), null);

			Validate(compiler.Compile("@1"), null);
			Validate(compiler.Compile("sin(1.0)"), null);
			Validate(compiler.Compile("cos(2.0)"), null);

			Validate(compiler.Compile("rand()"), null);
			Validate(compiler.Compile("rand(2.0)"), null);
			Validate(compiler.Compile(".1"), null);

			Validate(compiler.Compile("randaaa()"), typeof(Effekseer.InternalScript.UnknownFunctionException));

		}

		public bool Validate(Effekseer.InternalScript.CompileResult result, Type type)
		{
			if (type == null)
			{
				if (result.Error != null) throw result.Error;
			}
			else
			{
				if (result.Error == null) throw new Exception();
				if (result.Error.GetType() != type) throw result.Error;
			}
			return true;
		}
	}
}
