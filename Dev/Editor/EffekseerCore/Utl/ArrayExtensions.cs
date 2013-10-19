using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Utl
{
	public static class ArrayExtensions
	{
		public static T[] ToArray<T>(this T[][] arrayArray)
		{
			int count = 0;
			T[] ret = null;

			foreach (var ar in arrayArray)
			{
				count += ar.Length;
			}

			ret = new T[count];

			int offset = 0;
			foreach (var ar in arrayArray)
			{
				foreach (var a in ar)
				{
					ret[offset] = a;
					offset++;
				}
			}

			return ret;
		}

		public static T[] ToArray<T>(this List<T[]> arrayList)
		{
			return arrayList.ToArray().ToArray();
		}
	}
}
