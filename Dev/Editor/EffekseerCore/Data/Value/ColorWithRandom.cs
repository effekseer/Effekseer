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
			set
			{
				SetColorSpace(value);
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

		public void SetColorSpace(ColorSpace colorSpace, bool isCombined = false)
		{
			ColorSpace oldval = ColorSpace;
			ColorSpace newval = colorSpace;
			
			if (oldval == newval) return;
			
			var cmd = new Command.DelegateCommand(
				() =>
				{
					_colorSpace = newval;
					CallChangedColorSpace(false, ChangedValueType.Execute);
				},
				() =>
				{
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
			ColorSpace = colorSpace;

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
				},
				() =>
				{
					R.SetMinDirectly(old_r);
					G.SetMinDirectly(old_g);
					B.SetMinDirectly(old_b);
					A.SetMinDirectly(old_a);
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
				},
				() =>
				{
					R.SetMaxDirectly(old_r);
					G.SetMaxDirectly(old_g);
					B.SetMaxDirectly(old_b);
					A.SetMaxDirectly(old_a);
				},
				this,
				isCombined);

			Command.CommandManager.Execute(cmd);
		}

		public void ChangeColorSpace(ColorSpace colorSpace, bool link = false)
		{
			Command.CommandManager.StartCollection();
			
			ColorSpace = colorSpace;
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
	}
}
