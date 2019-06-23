using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.InternalScript
{
	/// <summary>
	/// It should be moved
	/// </summary>
	public class Tests
	{
		public Tests()
		{
			Compiler compiler = new Compiler();
			Validate(compiler.Compile("1 + 2 + 3"), null);
			Validate(compiler.Compile("1 * 2 + 3"), null);
			Validate(compiler.Compile("1 * (2 + 3)"), null);

			Validate(compiler.Compile("@O.x = 1 + 2 + 3"), null);
			Validate(compiler.Compile("@O.x = 1 + 2 + 3\n@O.y = 1 + 2 + 3"), null);

			Validate(compiler.Compile("@1"), null);
			Validate(compiler.Compile("sin(1.0)"), null);
			Validate(compiler.Compile("cos(2.0)"), null);
			Validate(compiler.Compile(".1"), null);
			//Validate(compiler.Compile("..1"), typeof(InvalidTokenException));
			//Validate(compiler.Compile(".1.1"), typeof(InvalidTokenException));

			/*

			if (!Validate(compiler.Compile(".1"))) throw new Exception();
			if (Validate(compiler.Compile("..1"))) throw new Exception();
			if (Validate(compiler.Compile(".1.1"))) throw new Exception();

			if (compiler.Compile("@P.x + @P.y").RunningPhase != RunningPhaseType.Local) throw new Exception();

			*/

		}

		public bool Validate(CompileResult result, Type type)
		{
			if(type == null)
			{
				if (result.Error != null) throw result.Error;
			}
			else
			{
				if (result.Error == null) throw new Exception();
				if(result.Error.GetType() != type) throw result.Error;
			}
			return true;
		}
	}
}
