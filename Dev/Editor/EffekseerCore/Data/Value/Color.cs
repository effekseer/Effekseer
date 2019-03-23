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

		public bool IsValueChangedFromDefault
		{
			get { return 
					R.IsValueChangedFromDefault || 
					G.IsValueChangedFromDefault || 
					B.IsValueChangedFromDefault || 
					A.IsValueChangedFromDefault; }
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

		public void SetValue(int r, int g, int b, int a = -1, bool isCombined = false)
		{
			if (
				r == R.GetValue() &&
				g == G.GetValue() &&
				b == B.GetValue() &&
				a == A.GetValue()) return;

			int old_r = R.GetValue();
			int new_r = r;

			int old_g = G.GetValue();
			int new_g = g;

			int old_b = B.GetValue();
			int new_b = b;

			int old_a = A.GetValue();
			int new_a = a;

			var cmd = new Command.DelegateCommand(
				() =>
				{
					R.SetValueDirectly(new_r);
					G.SetValueDirectly(new_g);
					B.SetValueDirectly(new_b);
					A.SetValueDirectly(new_a);
				},
				() =>
				{
					R.SetValueDirectly(old_r);
					G.SetValueDirectly(old_g);
					B.SetValueDirectly(old_b);
					A.SetValueDirectly(old_a);
				},
				this,
				isCombined);

			Command.CommandManager.Execute(cmd);
		}

		public static implicit operator byte[](Color value)
		{
			byte[] values = new byte[sizeof(byte) * 4] { (byte)value.R, (byte)value.G, (byte)value.B, (byte)value.A };
			return values;
		}
	}
}
