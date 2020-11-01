using System;
using System.Collections.Generic;
using Effekseer.InternalScript;
using InvalidOperationException = Effekseer.InternalScript.InvalidOperationException;

namespace Effekseer.Utils
{
	internal static class CompileErrorGenerator
	{
		public static string Generate(string code, Exception e)
		{
			List<int> separators = new List<int>();
			for (int i = 0; i < code.Length; i++)
			{
				if (code[i] == '\n')
				{
					separators.Add(i);
				}
			}

			int errorLine = 0;
			int errorPos = 0;

			if (e is CompileException compileException)
			{
				GetErrorPosition(separators, compileException.Line, out errorLine, out errorPos);
			}

			var errorLinePos = string.Format(Resources.GetString("CompilePosition"), errorLine, errorPos);


			switch (e)
			{
			case InvalidTokenException exception:
				return string.Format(Resources.GetString("CompileErrror_InvalidToken"), exception.Message) + errorLinePos;

			case InvalidEOFException _:
				return string.Format(Resources.GetString("CompileErrror_InvalidEoF")) + errorLinePos;

			case InvalidOperationException _:
				return string.Format(Resources.GetString("CompileError_InvalidOperation")) + errorLinePos;

			case ArgSizeException exception:
				return string.Format(Resources.GetString("CompileError_ArgSize"), string.Join(",", exception.Expected), exception.Actual) + errorLinePos;

			case InvalidSubstitution _:
				return string.Format(Resources.GetString("CompileError_InvalidSubstitution")) + errorLinePos;

			case UnknownFunctionException exception:
				return string.Format(Resources.GetString("CompileError_UnknownFunction"), exception.Message) + errorLinePos;

			case UnknownValueException exception:
				return string.Format(Resources.GetString("CompileError_UnknownValue"), exception.Message) + errorLinePos;

			default:
				return "Unknown error";
			}
		}

		private static void GetErrorPosition(List<int> sep, int ind, out int line, out int pos)
		{
			line = 0;
			pos = 0;

			for (int i = 0; i < sep.Count; i++)
			{
				if (ind < sep[i])
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
