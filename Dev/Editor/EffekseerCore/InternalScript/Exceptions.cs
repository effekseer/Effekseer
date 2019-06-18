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

	public class InvalidEOFException : CompileException
	{
		public InvalidEOFException(int line)
			: base("", line)
		{
		}
	}

	public class ArgSizeException : CompileException
	{
		public ArgSizeException(int actual, int expected, int line)
			: base("", line)
		{
			Actual = actual;
			Expected = expected;
		}

		public int Actual = 0;
		public int Expected = 0;
	}

	public class UnknownFunctionException : CompileException
	{
		public UnknownFunctionException(string message, int line)
			: base(message, line)
		{
		}
	}
}
