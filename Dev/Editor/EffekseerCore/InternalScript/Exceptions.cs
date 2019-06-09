using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.InternalScript
{
	public class CompileException : System.Exception
	{
		public CompileException(string message, int line)
			: base(message)
		{
			Line = line;
		}
		public int Line
		{
			get;
			private set;
		}
	}

	public class InvalidTokenException : CompileException
	{
		public InvalidTokenException(string message, int line)
			: base(message, line)
		{
		}
	}
}
