using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace mqoToEffekseerModelConverter
{
	[System.Runtime.InteropServices.StructLayout(System.Runtime.InteropServices.LayoutKind.Sequential)]
	public struct Color
	{
		public float R;
		public float G;
		public float B;
		public float A;

		public Color(float r, float g, float b, float a = 1.0f)
		{
			R = r;
			G = g;
			B = b;
			A = a;
		}

		public static bool operator ==(Color value1, Color value2)
		{
			return value1.R == value2.R && value1.G == value2.G && value1.B == value2.B && value1.A == value2.A;
		}

		public static bool operator !=(Color value1, Color value2)
		{
			return value1.R == value2.R && value1.G == value2.G && value1.B == value2.B && value1.A == value2.A;
		}
	}


}
