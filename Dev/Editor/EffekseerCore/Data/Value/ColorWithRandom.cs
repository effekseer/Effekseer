using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data.Value
{
	public class ColorWithRandom : IResettableValue, IValueChangedFromDefault
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

		public bool IsValueChangedFromDefault
		{
			get
			{
				return
				  R.IsValueChangedFromDefault ||
				  G.IsValueChangedFromDefault ||
				  B.IsValueChangedFromDefault ||
				  A.IsValueChangedFromDefault ||
				  DrawnAs != DefaultDrawnAs ||
				  ColorSpace != DefaultColorSpace;
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

			Utils.RGBHSVColor color_min;
			Utils.RGBHSVColor color_max;

			color_max.RH = old_max_r;
			color_max.GS = old_max_g;
			color_max.BV = old_max_b;
			color_min.RH = old_min_r;
			color_min.GS = old_min_g;
			color_min.BV = old_min_b;

			if (isColorConverted)
			{
				if (newval == ColorSpace.HSVA)
				{
					color_min = Utils.RGBHSVColor.RGBToHSV(color_min);
					color_max = Utils.RGBHSVColor.RGBToHSV(color_max);
				}
				else
				{
					color_min = Utils.RGBHSVColor.HSVToRGB(color_min);
					color_max = Utils.RGBHSVColor.HSVToRGB(color_max);
				}
			}

			var cmd = new Command.DelegateCommand(
				() =>
				{
					_colorSpace = newval;

					R.SetMaxDirectly(color_max.RH);
					G.SetMaxDirectly(color_max.GS);
					B.SetMaxDirectly(color_max.BV);

					R.SetMinDirectly(color_min.RH);
					G.SetMinDirectly(color_min.GS);
					B.SetMinDirectly(color_min.BV);

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
			R = new IntWithRandom(r, r_max, r_min, DrawnAs.MaxAndMin);
			G = new IntWithRandom(g, g_max, g_min, DrawnAs.MaxAndMin);
			B = new IntWithRandom(b, b_max, b_min, DrawnAs.MaxAndMin);
			A = new IntWithRandom(a, a_max, a_min, DrawnAs.MaxAndMin);
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

		public void ResetValue()
		{
			Command.CommandManager.StartCollection();
			SetColorSpace(DefaultColorSpace, false, false);
			R.ResetValue();
			G.ResetValue();
			B.ResetValue();
			A.ResetValue();
			DrawnAs = DefaultDrawnAs;
			Command.CommandManager.EndCollection();
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

		public byte[] GetBytes()
		{
			byte[] values = new byte[sizeof(byte) * 10]
			{
				(byte)ColorSpace,
				(byte)0,	// reserved
				(byte)R.Max,
				(byte)G.Max,
				(byte)B.Max,
				(byte)A.Max,
				(byte)R.Min,
				(byte)G.Min,
				(byte)B.Min,
				(byte)A.Min
			};

			return values;
		}

		public static implicit operator byte[](ColorWithRandom value)
		{
			return value.GetBytes();
		}
	}
}