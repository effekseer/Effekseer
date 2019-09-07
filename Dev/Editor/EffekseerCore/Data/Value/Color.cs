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

		private ColorSpace _colorSpace;
		public ColorSpace ColorSpace
		{
			get
			{
				return GetColorSpace();
			}
		}

		public bool IsValueChangedFromDefault
		{
			get { return 
					R.IsValueChangedFromDefault || 
					G.IsValueChangedFromDefault || 
					B.IsValueChangedFromDefault || 
					A.IsValueChangedFromDefault; }
		}

		internal ColorSpace DefaultColorSpace { get; private set; }

		public event ChangedValueEventHandler OnChangedColorSpace;

		public void SetColorSpace(ColorSpace colorSpace, bool isColorConverted, bool isCombined)
		{
			ColorSpace oldval = ColorSpace;
			ColorSpace newval = colorSpace;

			if (oldval == newval) return;

			int old_r = R.Value;
			int old_g = G.Value;
			int old_b = B.Value;
			int old_a = A.Value;

			Utl.RGBHSVColor color;

			color.RH = old_r;
			color.GS = old_g;
			color.BV = old_b;

			if (isColorConverted)
			{
				if (newval == ColorSpace.HSVA)
				{
					color = Utl.RGBHSVColor.RGBToHSV(color);
				}
				else
				{
					color = Utl.RGBHSVColor.HSVToRGB(color);
				}
			}

			var cmd = new Command.DelegateCommand(
				() =>
				{
					_colorSpace = newval;

					R.SetValueDirectly(color.RH);
					G.SetValueDirectly(color.GS);
					B.SetValueDirectly(color.BV);

					CallChangedColorSpace(false, ChangedValueType.Execute);
				},
				() =>
				{
					R.SetValueDirectly(old_r);
					G.SetValueDirectly(old_g);
					B.SetValueDirectly(old_b);

					_colorSpace = oldval;
					CallChangedColorSpace(false, ChangedValueType.Unexecute);
				},
				this,
				isCombined);

			Command.CommandManager.Execute(cmd);
		}
		public ColorSpace GetColorSpace()
		{
			return _colorSpace;
		}

		protected void CallChangedColorSpace(object value, ChangedValueType type)
		{
			if (OnChangedColorSpace != null)
			{
				OnChangedColorSpace(this, new ChangedValueEventArgs(value, type));
			}
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
			DefaultColorSpace = ColorSpace.RGBA;
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
			if(value.ColorSpace == ColorSpace.RGBA)
			{
				byte[] values = new byte[sizeof(byte) * 4] { (byte)value.R, (byte)value.G, (byte)value.B, (byte)value.A };
				return values;
			}
			else
			{
				Utl.RGBHSVColor color;

				color.RH = value.R.Value;
				color.GS = value.G.Value;
				color.BV = value.B.Value;
				color = Utl.RGBHSVColor.HSVToRGB(color);

				byte[] values = new byte[sizeof(byte) * 4] { (byte)value.R, (byte)value.G, (byte)value.B, (byte)value.A };
				return values;

			}
		}

		public void ChangeColorSpace(ColorSpace colorSpace)
		{
			Command.CommandManager.StartCollection();

			SetColorSpace(colorSpace, true, false);
			CallChangedColorSpace(true, ChangedValueType.Execute);

			Command.CommandManager.EndCollection();
		}

	}
}
