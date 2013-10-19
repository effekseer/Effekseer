using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Utl
{
	class MathUtl
	{
		/// <summary>
		/// Easingの三次方程式の項3つを求める。
		/// </summary>
		/// <param name="a1">始点の角度(度)</param>
		/// <param name="a2">終点の角度(度)</param>
		/// <returns></returns>
		public static float[] Easing(float a1, float a2)
		{ 
			float g1 = (float)Math.Tan( ((float)a1 + 45.0) / 180.0 * Math.PI );
			float g2 = (float)Math.Tan( ((float)a2 + 45.0) / 180.0 * Math.PI );

			float c = g1;
			float a = g2 - g1 - ( 1.0f - c ) * 2.0f;
			float b = (g2 - g1 - (a * 3.0f)) / 2.0f;

			return new float[3] { a, b, c };
		}
	}
}
