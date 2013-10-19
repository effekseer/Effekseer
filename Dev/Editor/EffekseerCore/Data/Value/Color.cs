using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data.Value
{
	public class Color
	{
		public Int R
		{
			get;
			private set;
		}

		public Int G
		{
			get;
			private set;
		}

		public Int B
		{
			get;
			private set;
		}

		public Int A
		{
			get;
			private set;
		}
		
		internal Color(
			byte r = 0,
			byte g = 0,
			byte b = 0,
			byte a = 0,
			byte r_max = byte.MaxValue,
			byte r_min = byte.MinValue,
			byte g_max = byte.MaxValue,
			byte g_min = byte.MinValue,
			byte b_max = byte.MaxValue,
			byte b_min = byte.MinValue,
			byte a_max = byte.MaxValue,
			byte a_min = byte.MinValue)
		{
			R = new Int(r, r_max, r_min);
			G = new Int(g, g_max, g_min);
			B = new Int(b, b_max, b_min);
			A = new Int(a, a_max, a_min);
		}

		public void SetValue(int r, int g, int b)
		{
			Command.CommandManager.StartCollection();
			R.SetValue(r);
			G.SetValue(g);
			B.SetValue(b);
			Command.CommandManager.EndCollection();
		}

		public static implicit operator byte[](Color value)
		{
			byte[] values = new byte[sizeof(byte) * 4] { (byte)value.R, (byte)value.G, (byte)value.B, (byte)value.A };
			return values;
		}
	}
}
