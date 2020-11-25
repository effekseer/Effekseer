using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace mqoToEffekseerModelConverter
{
	[System.Runtime.InteropServices.StructLayout(System.Runtime.InteropServices.LayoutKind.Sequential)]
	public unsafe struct Matrix44
	{
		public float this[int r, int c]
		{
			get
			{
				fixed (float* v = values)
				{
					var v_ = v + (c + r * 4);
					return *v_;
				}
			}
			set
			{
				fixed (float* v = values)
				{
					var v_ = v + (c + r * 4);
					*v_ = value;
				}
			}
		}

		public void Identfy()
		{
			this[0, 0] = 1.0f;
			this[0, 1] = 0.0f;
			this[0, 2] = 0.0f;
			this[0, 3] = 0.0f;

			this[1, 0] = 0.0f;
			this[1, 1] = 1.0f;
			this[1, 2] = 0.0f;
			this[1, 3] = 0.0f;

			this[2, 0] = 0.0f;
			this[2, 1] = 0.0f;
			this[2, 2] = 1.0f;
			this[2, 3] = 0.0f;

			this[3, 0] = 0.0f;
			this[3, 1] = 0.0f;
			this[3, 2] = 0.0f;
			this[3, 3] = 1.0f;
		}

		fixed float values[16];
	}
}
