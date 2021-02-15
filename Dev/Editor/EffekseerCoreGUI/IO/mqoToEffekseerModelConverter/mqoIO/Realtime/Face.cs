using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace mqoToEffekseerModelConverter.mqoIO.Realtime
{
	public class Face
	{
		public int[] Indexes = new int[3];
		public Vector2D[] UV = new Vector2D[3];
		public Color[] Colors = new Color[3];
		public int Material;

		public Face()
		{
			for (int i = 0; i < 3; i++)
			{
				Colors[i] = new Color(1.0f, 1.0f, 1.0f, 1.0f);
			}
		}
	}
}
