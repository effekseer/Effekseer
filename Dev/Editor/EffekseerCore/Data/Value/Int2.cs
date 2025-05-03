using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data.Value
{
	public class Int2 : IResettableValue, IValueChangedFromDefault
	{
		public Int X
		{
			get;
			private set;
		}

		public Int Y
		{
			get;
			private set;
		}

		public bool IsValueChangedFromDefault
		{
			get { return X.IsValueChangedFromDefault || Y.IsValueChangedFromDefault; }
		}

		internal Int2(
			int x = 0,
			int y = 0,
			int x_max = int.MaxValue,
			int x_min = int.MinValue,
			int y_max = int.MaxValue,
			int y_min = int.MinValue,
			int x_step = 1,
			int y_step = 1)
		{
			X = new Int(x, x_max, x_min, x_step);
			Y = new Int(y, y_max, y_min, y_step);
		}

		public void ResetValue()
		{
			Command.CommandManager.StartCollection();
			X.ResetValue();
			Y.ResetValue();
			Command.CommandManager.EndCollection();
		}

		public static explicit operator byte[](Int2 value)
		{
			byte[] values = new byte[sizeof(int) * 2];
			byte[] x = BitConverter.GetBytes(value.X.GetValue());
			byte[] y = BitConverter.GetBytes(value.Y.GetValue());
			x.CopyTo(values, 0);
			y.CopyTo(values, sizeof(float) * 1);
			return values;
		}

		public byte[] GetBytes()
		{
			byte[] values = new byte[sizeof(float) * 2];
			BitConverter.GetBytes(X).CopyTo(values, sizeof(int) * 0);
			BitConverter.GetBytes(Y).CopyTo(values, sizeof(int) * 1);
			return values;
		}


		public bool ValueEquals(object obj)
		{
			var o = obj as Int2;
			if (o == null)
				return false;

			if (X.Value != o.X.Value)
				return false;

			if (Y.Value != o.Y.Value)
				return false;

			return true;
		}

		public int GetValueHashCode()
		{
			return Utils.Misc.CombineHashCodes(new[] { X.Value.GetHashCode(), Y.Value.GetHashCode() });
		}
	}
}