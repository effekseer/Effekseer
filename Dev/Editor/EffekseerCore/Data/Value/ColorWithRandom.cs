using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data.Value
{
	public class ColorWithRandom
	{
		public IntWithRandom R
		{
			get;
			private set;
		}

		public IntWithRandom G
		{
			get;
			private set;
		}

		public IntWithRandom B
		{
			get;
			private set;
		}

		public IntWithRandom A
		{
			get;
			private set;
		}

		public DrawnAs DrawnAs
		{
			get;
			set;
		}
		
		private ColorSpace _colorSpace;
		public ColorSpace ColorSpace
		{
			get
			{
				return GetColorSpace();
			}
		}

		public ColorWithRandom Link
		{
			get;
			set;
		}

		internal DrawnAs DefaultDrawnAs { get; private set; }

		internal ColorSpace DefaultColorSpace { get; private set; }

		public event ChangedValueEventHandler OnChangedColorSpace;

		public void SetColorSpace(ColorSpace colorSpace, bool isColorConverted, bool isCombined)
		{
			ColorSpace oldval = ColorSpace;
			ColorSpace newval = colorSpace;
			
			if (oldval == newval) return;

			int old_min_r = R.GetMin();
			int old_min_g = G.GetMin();
			int old_min_b = B.GetMin();
			int old_min_a = A.GetMin();
			int old_max_r = R.GetMax();
			int old_max_g = G.GetMax();
			int old_max_b = B.GetMax();
			int old_max_a = A.GetMax();

			color color_min;
			color color_max;

			color_max.R = old_max_r;
			color_max.G = old_max_g;
			color_max.B = old_max_b;
			color_min.R = old_min_r;
			color_min.G = old_min_g;
			color_min.B = old_min_b;

			if (isColorConverted)
			{
				if (newval == ColorSpace.HSVA)
				{
					color_min = RGBToHSV(color_min);
					color_max = RGBToHSV(color_max);
				}
				else
				{
					color_min = HSVToRGB(color_min);
					color_max = HSVToRGB(color_max);
				}
			}

			var cmd = new Command.DelegateCommand(
				() =>
				{
					_colorSpace = newval;

					R.SetMaxDirectly(color_max.R);
					G.SetMaxDirectly(color_max.G);
					B.SetMaxDirectly(color_max.B);

					R.SetMinDirectly(color_min.R);
					G.SetMinDirectly(color_min.G);
					B.SetMinDirectly(color_min.B);

					CallChangedColorSpace(false, ChangedValueType.Execute);
				},
				() =>
				{
					R.SetMaxDirectly(old_max_r);
					G.SetMaxDirectly(old_max_g);
					B.SetMaxDirectly(old_max_b);

					R.SetMinDirectly(old_min_r);
					G.SetMinDirectly(old_min_g);
					B.SetMinDirectly(old_min_b);

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

		internal ColorWithRandom(
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
			byte a_min = byte.MinValue,
			DrawnAs drawnas = Data.DrawnAs.CenterAndAmplitude,
			ColorSpace colorSpace = Data.ColorSpace.RGBA)
		{
			R = new IntWithRandom(r, r_max, r_min);
			G = new IntWithRandom(g, g_max, g_min);
			B = new IntWithRandom(b, b_max, b_min);
			A = new IntWithRandom(a, a_max, a_min);
			DrawnAs = drawnas;
			SetColorSpace(colorSpace, false, false);

			DefaultDrawnAs = DrawnAs;
			DefaultColorSpace = ColorSpace;
		}

		public void SetMin(int r, int g, int b, int a = -1, bool isCombined = false)
		{
			if (
				r == R.GetMin() &&
				g == G.GetMin() &&
				b == B.GetMin() &&
				a == A.GetMin()) return;

			int old_r = R.GetMin();
			int new_r = r;

			int old_g = G.GetMin();
			int new_g = g;

			int old_b = B.GetMin();
			int new_b = b;

			int old_a = A.GetMin();
			int new_a = a;

			var cmd = new Command.DelegateCommand(
				() =>
				{
					R.SetMinDirectly(new_r);
					G.SetMinDirectly(new_g);
					B.SetMinDirectly(new_b);
					A.SetMinDirectly(new_a);

					R.CallChanged(new_r, ChangedValueType.Execute);
					G.CallChanged(new_g, ChangedValueType.Execute);
					B.CallChanged(new_b, ChangedValueType.Execute);
					A.CallChanged(new_a, ChangedValueType.Execute);
				},
				() =>
				{
					R.SetMinDirectly(old_r);
					G.SetMinDirectly(old_g);
					B.SetMinDirectly(old_b);
					A.SetMinDirectly(old_a);

					R.CallChanged(old_r, ChangedValueType.Unexecute);
					G.CallChanged(old_g, ChangedValueType.Unexecute);
					B.CallChanged(old_b, ChangedValueType.Unexecute);
					A.CallChanged(old_a, ChangedValueType.Unexecute);
				},
				this,
				isCombined);

			Command.CommandManager.Execute(cmd);
		}

		public void SetMax(int r, int g, int b, int a = -1, bool isCombined = false)
		{
			if (
				r == R.GetMax() &&
				g == G.GetMax() &&
				b == B.GetMax() &&
				a == A.GetMax()) return;

			int old_r = R.GetMax();
			int new_r = r;

			int old_g = G.GetMax();
			int new_g = g;

			int old_b = B.GetMax();
			int new_b = b;

			int old_a = A.GetMax();
			int new_a = a;

			var cmd = new Command.DelegateCommand(
				() =>
				{
					R.SetMaxDirectly(new_r);
					G.SetMaxDirectly(new_g);
					B.SetMaxDirectly(new_b);
					A.SetMaxDirectly(new_a);

					R.CallChanged(new_r, ChangedValueType.Execute);
					G.CallChanged(new_g, ChangedValueType.Execute);
					B.CallChanged(new_b, ChangedValueType.Execute);
					A.CallChanged(new_a, ChangedValueType.Execute);
				},
				() =>
				{
					R.SetMaxDirectly(old_r);
					G.SetMaxDirectly(old_g);
					B.SetMaxDirectly(old_b);
					A.SetMaxDirectly(old_a);

					R.CallChanged(old_r, ChangedValueType.Unexecute);
					G.CallChanged(old_g, ChangedValueType.Unexecute);
					B.CallChanged(old_b, ChangedValueType.Unexecute);
					A.CallChanged(old_a, ChangedValueType.Unexecute);
				},
				this,
				isCombined);

			Command.CommandManager.Execute(cmd);
		}

		public void ChangeColorSpace(ColorSpace colorSpace, bool link = false)
		{
			Command.CommandManager.StartCollection();

			SetColorSpace(colorSpace, true, false);
			CallChangedColorSpace(true, ChangedValueType.Execute);
			
			if (link && Link != null)
			{
				Link.ChangeColorSpace(colorSpace, false);
			}

			Command.CommandManager.EndCollection();
		}

		public static implicit operator byte[](ColorWithRandom value)
		{
			byte[] values = new byte[sizeof(byte) * 10]
			{
				(byte)value.ColorSpace,
				(byte)0,	// reserved
				(byte)value.R.Max,
				(byte)value.G.Max,
				(byte)value.B.Max,
				(byte)value.A.Max,
				(byte)value.R.Min,
				(byte)value.G.Min,
				(byte)value.B.Min,
				(byte)value.A.Min
			};

			return values;
		}

		struct color
		{
			public int R;
			public int G;
			public int B;
		}

		static color RGBToHSV(color rgb)
		{
			double max;
			double min;
			double R, G, B, H, S, V;

			R = (double)rgb.R / 255.0;
			G = (double)rgb.G / 255.0;
			B = (double)rgb.B / 255.0;

			if (R >= G && R >= B)
			{
				max = R;
				min = (G < B) ? G : B;
			}
			else if (G >= R && G >= B)
			{
				max = G;
				min = (R < B) ? R : B;
			}
			else
			{
				max = B;
				min = (R < G) ? R : G;
			}
			if (R == G && G == B)
			{
				H = 0.0f;
			}
			else if (max == R)
			{
				H = 60 * (G - B) / (max - min);
			}
			else if (max == G)
			{
				H = 60 * (B - R) / (max - min) + 120;
			}
			else
			{
				H = 60 * (R - G) / (max - min) + 240;
			}
			if (H < 0.0f)
			{
				H += 360.0f;
			}
			if (max == 0.0f)
			{
				S = 0.0f;
			}
			else
			{
				S = (max - min) / max;
			}
			V = max;

			color ret = new color();
			ret.R = (int)Math.Round(H / 360 * 252, MidpointRounding.AwayFromZero);
			ret.G = (int)Math.Round(S * 255, MidpointRounding.AwayFromZero);
			ret.B = (int)Math.Round(V * 255, MidpointRounding.AwayFromZero);
			return ret;
		}

		static color HSVToRGB(color hsv)
		{
			int H = hsv.R, S = hsv.G, V = hsv.B;
			int i, R = 0, G = 0, B = 0, f, p, q, t;

			i = H / 42 % 6;
			f = H % 42 * 6;
			p = (V * (256 - S)) >> 8;
			q = (V * (256 - ((S * f) >> 8))) >> 8;
			t = (V * (256 - ((S * (252 - f)) >> 8))) >> 8;
			if (p < 0) p = 0;
			if (p > 255) p = 255;
			if (q < 0) q = 0;
			if (q > 255) q = 255;
			if (t < 0) t = 0;
			if (t > 255) t = 255;

			switch (i)
			{
				case 0: R = V; G = t; B = p; break;
				case 1: R = q; G = V; B = p; break;
				case 2: R = p; G = V; B = t; break;
				case 3: R = p; G = q; B = V; break;
				case 4: R = t; G = p; B = V; break;
				case 5: R = V; G = p; B = q; break;
			}

			color ret = new color();
			ret.R = R;
			ret.G = G;
			ret.B = B;
			return ret;
		}
	}
}
