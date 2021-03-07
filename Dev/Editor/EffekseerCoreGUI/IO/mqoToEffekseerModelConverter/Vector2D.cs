using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace mqoToEffekseerModelConverter
{
	[System.Runtime.InteropServices.StructLayout(System.Runtime.InteropServices.LayoutKind.Sequential)]
	public struct Vector2D
	{
		public float X;
		public float Y;

		public Vector2D(float x, float y)
		{
			X = x;
			Y = y;
		}

		public static bool operator ==(Vector2D value1, Vector2D value2)
		{
			return value1.X == value2.X && value1.Y == value2.Y;
		}

		public static bool operator !=(Vector2D value1, Vector2D value2)
		{
			return value1.X != value2.X && value1.Y != value2.Y;
		}
	}
}
