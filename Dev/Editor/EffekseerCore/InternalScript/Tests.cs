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
			if (!Validate(compiler.Compile("1 + 2 + 3"))) throw new Exception();
			if(!Validate(compiler.Compile("1 * 2 + 3"))) throw new Exception();
			if (!Validate(compiler.Compile("1 * (2 + 3)"))) throw new Exception();
			if (!Validate(compiler.Compile("@1"))) throw new Exception();

			if (!Validate(compiler.Compile(".1"))) throw new Exception();
			if (Validate(compiler.Compile("..1"))) throw new Exception();
			if (Validate(compiler.Compile(".1.1"))) throw new Exception();

			if (compiler.Compile("@P.x + @P.y").RunningPhase != RunningPhaseType.Local) throw new Exception();

		}

		public bool Validate(CompileResult result)
		{
			return result.Error == null;
		}
	}
}
