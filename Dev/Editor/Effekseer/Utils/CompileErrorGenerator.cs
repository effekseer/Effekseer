using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.Utils
{
	class CompileErrorGenerator
	{
		public static string Generate(string code, Exception e)
		{
			List<int> separators = new List<int>();
			for(int i = 0; i < code.Length; i++)
			{
				if (code[i] == '\n')
				{
					separators.Add(i);
				}
			}

			int errorLine = 0;
			int errorPos = 0;

			if(e is InternalScript.CompileException)
			{
				var e_ = e as InternalScript.CompileException;
				GetErrorPosition(separators, e_.Line, out errorLine, out errorPos);
			}

			var errorLinePos = string.Format(Resources.GetString("CompilePosition"), errorLine, errorPos);

			if(e is InternalScript.InvalidTokenException)
			{
				var e_ = e as InternalScript.InvalidTokenException;

				return string.Format(Resources.GetString("CompileErrror_InvalidToken"), e_.Message) + errorLinePos;
			}
			else if (e is InternalScript.InvalidEOFException)
			{
				var e_ = e as InternalScript.InvalidEOFException;

				return string.Format(Resources.GetString("CompileErrror_InvalidEoF")) + errorLinePos;
			}
			else if (e is InternalScript.InvalidOperationException)
			{
				var e_ = e as InternalScript.InvalidOperationException;

				return string.Format(Resources.GetString("CompileError_InvalidOperation")) + errorLinePos;
			}
			else if (e is InternalScript.ArgSizeException)
			{
				var e_ = e as InternalScript.ArgSizeException;

				return string.Format(Resources.GetString("CompileError_ArgSize"), string.Join(",", e_.Expected), e_.Actual) + errorLinePos;
			}
			else if (e is InternalScript.InvalidSubstitution)
			{
				var e_ = e as InternalScript.InvalidSubstitution;

				return string.Format(Resources.GetString("CompileError_InvalidSubstitution")) + errorLinePos;
			}
			else if (e is InternalScript.UnknownFunctionException)
			{
				var e_ = e as InternalScript.UnknownFunctionException;

				return string.Format(Resources.GetString("CompileError_UnknownFunction"), e_.Message) + errorLinePos;
			}
			else if (e is InternalScript.UnknownValueException)
			{
				var e_ = e as InternalScript.UnknownValueException;

				return string.Format(Resources.GetString("CompileError_UnknownValue"), e_.Message) + errorLinePos;
			}
			else
			{
				return "Unknown error";
			}
		}

		static void GetErrorPosition(List<int> sep, int ind, out int line, out int pos)
		{
			line = 0;
			pos = 0;

			for(int i = 0; i < sep.Count; i++)
			{
				if(ind < sep[i])
				{
					line = i + 1;

					if (i == 0)
					{
						pos = ind + 1;
					}
					else
					{
						pos = ind - sep[i - 1];
					}
					return;
				}
			}

			if (sep.Count == 0)
			{
				line = 1;
				pos = ind + 1;
			}
			else
			{
				line = sep.Count + 1;
				pos = ind - sep[sep.Count - 1];
			}
		}
	}
}
