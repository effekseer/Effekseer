using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data.Value
{
	public class Vector2D : IResettableValue
	{
		public Float X
		{
			get;
			private set;
		}

		public Float Y
		{
			get;
			private set;
		}

		public bool IsValueChangedFromDefault
		{
			get { return X.IsValueChangedFromDefault || Y.IsValueChangedFromDefault; }
		}

		internal Vector2D(
			float x = 0,
			float y = 0,
			float x_max = float.MaxValue,
			float x_min = float.MinValue,
			float y_max = float.MaxValue,
			float y_min = float.MinValue,
			float x_step = 1.0f,
			float y_step = 1.0f)
		{
			X = new Float(x, x_max, x_min, x_step);
			Y = new Float(y, y_max, y_min, y_step);
		}

		public void ResetValue()
		{
			Command.CommandManager.StartCollection();
			X.ResetValue();
			Y.ResetValue();
			Command.CommandManager.EndCollection();
		}

		public void ChangeDefaultValue(float x, float y)
		{
			X.ChangeDefaultValue(x);
			Y.ChangeDefaultValue(y);
		}

		public static explicit operator byte[](Vector2D value)
		{
			byte[] values = new byte[sizeof(float) * 2];
			byte[] x = BitConverter.GetBytes(value.X.GetValue());
			byte[] y = BitConverter.GetBytes(value.Y.GetValue());
			x.CopyTo(values, 0);
			y.CopyTo(values, sizeof(float) * 1);
			return values;
		}

		public byte[] GetBytes(float mul = 1.0f)
		{
			byte[] values = new byte[sizeof(float) * 2];
			BitConverter.GetBytes(X * mul).CopyTo(values, sizeof(float) * 0);
			BitConverter.GetBytes(Y * mul).CopyTo(values, sizeof(float) * 1);
			return values;
		}
	}
}
