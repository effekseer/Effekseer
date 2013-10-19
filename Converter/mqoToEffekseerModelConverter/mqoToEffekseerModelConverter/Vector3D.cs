using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace mqoToEffekseerModelConverter
{
	[System.Runtime.InteropServices.StructLayout(System.Runtime.InteropServices.LayoutKind.Sequential)]
	public struct Vector3D
	{
		public float X;
		public float Y;
		public float Z;

		public Vector3D(float x, float y, float z)
		{
			X = x;
			Y = y;
			Z = z;
		}

		public static void Cross(out Vector3D o, ref Vector3D in1, ref Vector3D in2)
		{
			float x = in1.Y * in2.Z - in1.Z * in2.Y;
			float y = in1.Z * in2.X - in1.X * in2.Z;
			float z = in1.X * in2.Y - in1.Y * in2.X;
			o.X = x;
			o.Y = y;
			o.Z = z;
		}

		public static float Dot(ref Vector3D in1, ref Vector3D in2)
		{
			return in1.X * in2.X + in1.Y * in2.Y + in1.Z * in2.Z;
		}

		public float GetLength()
		{
			return (float)System.Math.Sqrt(X * X + Y * Y + Z * Z);
		}

		public static void Normal(out Vector3D o, ref Vector3D in1)
		{
			float length = in1.GetLength();
			o.X = in1.X / length;
			o.Y = in1.Y / length;
			o.Z = in1.Z / length;
		}

		public static Vector3D operator +(Vector3D value1, Vector3D value2)
		{
			return new Vector3D(value1.X + value2.X, value1.Y + value2.Y, value1.Z + value2.Z);
		}

		public static Vector3D operator -(Vector3D value1, Vector3D value2)
		{
			return new Vector3D(value1.X - value2.X, value1.Y - value2.Y, value1.Z - value2.Z);
		}

		public static Vector3D operator *(Vector3D value1, float value2)
		{
			return new Vector3D(value1.X * value2, value1.Y * value2, value1.Z * value2);
		}

		public static Vector3D operator /(Vector3D value1, float value2)
		{
			return new Vector3D(value1.X / value2, value1.Y / value2, value1.Z / value2);
		}
	}
}
