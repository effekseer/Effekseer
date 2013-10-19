using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Utl
{
	public static class MathExtensions
	{
		public static int Clipping(this int v, int max, int min)
		{
			if (v > max) return max;
			if (v < min) return min;
			return v;
		}

		public static float Clipping(this float v, float max, float min)
		{
			if (v > max) return max;
			if (v < min) return min;
			return v;
		}

		public static double Clipping(this double v, double max, double min)
		{
			if (v > max) return max;
			if (v < min) return min;
			return v;
		}
	}
}
