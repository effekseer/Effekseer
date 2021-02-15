using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace mqoToEffekseerModelConverter
{
	[System.Runtime.InteropServices.StructLayout(System.Runtime.InteropServices.LayoutKind.Sequential)]
	public struct Vector4D
	{
		public float X;
		public float Y;
		public float Z;
		public float W;

		public Vector4D(float x, float y, float z, float w)
		{
			X = x;
			Y = y;
			Z = z;
			W = w;
		}
	}
}
