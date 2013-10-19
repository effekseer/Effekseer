using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace Effekseer.Binary
{
	enum NodeType : int
	{ 
		Root = -1,
		None = 0,
		Sprite = 2,
		Ribbon = 3,
        Ring = 4,
		Model = 5,
		Track = 6,
	};

	[StructLayout(LayoutKind.Sequential)]
	struct Vector3D
	{
		float X;
		float Y;
		float Z;

		public Vector3D(float x, float y, float z)
		{
			X = x;
			Y = y;
			Z = z;
		}
	}
}
