using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TestCSharp.Misc
{
	class TestUtils
	{
		public static void Assert(bool state, string message = "")
		{
			if (!state)
			{
				throw new Exception(message);
			}
		}

		public static void Equals(int data1, int data2, string message = "")
		{
			if (data1 != data2)
			{
				throw new Exception(message);
			}
		}

		public static void Equals(string data1, string data2, string message = "")
		{
			if (data1 != data2)
			{
				throw new Exception(message);
			}
		}

		public static void CompareBytes(byte[] data1, byte[] data2, string message = "")
		{
			if (data1.SequenceEqual(data2) == false)
			{
				throw new Exception(message);
			}
		}
	}
}
