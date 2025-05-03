using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data.Value
{
	public class Vector2DWithRandom : IResettableValue, IValueChangedFromDefault
	{
		public FloatWithRandom X
		{
			get;
			private set;
		}

		public FloatWithRandom Y
		{
			get;
			private set;
		}

		public DrawnAs DrawnAs
		{
			get;
			set;
		}

		internal DrawnAs DefaultDrawnAs { get; private set; }

		public bool IsValueChangedFromDefault
		{
			get { return X.IsValueChangedFromDefault || Y.IsValueChangedFromDefault || DrawnAs != DefaultDrawnAs; }
		}

		internal Vector2DWithRandom(
			float x = 0,
			float y = 0,
			float x_max = float.MaxValue,
			float x_min = float.MinValue,
			float y_max = float.MaxValue,
			float y_min = float.MinValue,
			DrawnAs drawnas = Data.DrawnAs.CenterAndAmplitude,
			float x_step = 1.0f,
			float y_step = 1.0f)
		{
			X = new FloatWithRandom(x, x_max, x_min, drawnas, x_step);
			Y = new FloatWithRandom(y, y_max, y_min, drawnas, y_step);
			DrawnAs = drawnas;
			DefaultDrawnAs = DrawnAs;
		}

		public void ResetValue()
		{
			Command.CommandManager.StartCollection();
			X.ResetValue();
			Y.ResetValue();
			DrawnAs = DefaultDrawnAs;
			Command.CommandManager.EndCollection();
		}

		public byte[] GetBytes(float mul = 1.0f)
		{
			byte[] values = new byte[sizeof(float) * 4];
			BitConverter.GetBytes(X.Max * mul).CopyTo(values, sizeof(float) * 0);
			BitConverter.GetBytes(Y.Max * mul).CopyTo(values, sizeof(float) * 1);
			BitConverter.GetBytes(X.Min * mul).CopyTo(values, sizeof(float) * 2);
			BitConverter.GetBytes(Y.Min * mul).CopyTo(values, sizeof(float) * 3);
			return values;
		}

		public static explicit operator byte[](Vector2DWithRandom value)
		{
			return value.GetBytes();
		}
	}
}