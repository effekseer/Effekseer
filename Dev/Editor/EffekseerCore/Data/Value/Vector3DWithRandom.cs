using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data.Value
{
	public class Vector3DWithRandom : IResettableValue, IValueChangedFromDefault
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

		public FloatWithRandom Z
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

		public bool CanSelectDynamicEquation = false;

		public Boolean IsDynamicEquationEnabled
		{
			get;
			private set;
		}

		public DynamicEquationReference DynamicEquationMin
		{
			get;
			private set;
		}

		public DynamicEquationReference DynamicEquationMax
		{
			get;
			private set;
		}

		public event ChangedValueEventHandler OnChanged;

		public bool IsValueChangedFromDefault
		{
			get { return X.IsValueChangedFromDefault || Y.IsValueChangedFromDefault || Z.IsValueChangedFromDefault || DrawnAs != DefaultDrawnAs; }
		}

		internal Vector3DWithRandom(
			float x = 0,
			float y = 0,
			float z = 0,
			float x_max = float.MaxValue,
			float x_min = float.MinValue,
			float y_max = float.MaxValue,
			float y_min = float.MinValue,
			float z_max = float.MaxValue,
			float z_min = float.MinValue,
			DrawnAs drawnas = Data.DrawnAs.CenterAndAmplitude,
			float x_step = 1.0f,
			float y_step = 1.0f,
			float z_step = 1.0f)
		{
			X = new FloatWithRandom(x, x_max, x_min, drawnas, x_step);
			Y = new FloatWithRandom(y, y_max, y_min, drawnas, y_step);
			Z = new FloatWithRandom(z, z_max, z_min, drawnas, z_step);
			IsDynamicEquationEnabled = new Boolean();
			DynamicEquationMin = new DynamicEquationReference();
			DynamicEquationMax = new DynamicEquationReference();
			DrawnAs = drawnas;
			DefaultDrawnAs = DrawnAs;
		}

		public void ResetValue()
		{
			Command.CommandManager.StartCollection();
			X.ResetValue();
			Y.ResetValue();
			Z.ResetValue();
			if (CanSelectDynamicEquation)
			{
				IsDynamicEquationEnabled.ResetValue();
				DynamicEquationMin.SetValue(null);
				DynamicEquationMax.SetValue(null);
			}
			DrawnAs = DefaultDrawnAs;
			Command.CommandManager.EndCollection();
		}

		public byte[] GetBytes(float mul = 1.0f)
		{
			byte[] values = new byte[sizeof(float) * 6];
			BitConverter.GetBytes(X.Max * mul).CopyTo(values, sizeof(float) * 0);
			BitConverter.GetBytes(Y.Max * mul).CopyTo(values, sizeof(float) * 1);
			BitConverter.GetBytes(Z.Max * mul).CopyTo(values, sizeof(float) * 2);
			BitConverter.GetBytes(X.Min * mul).CopyTo(values, sizeof(float) * 3);
			BitConverter.GetBytes(Y.Min * mul).CopyTo(values, sizeof(float) * 4);
			BitConverter.GetBytes(Z.Min * mul).CopyTo(values, sizeof(float) * 5);
			return values;
		}

		public static explicit operator byte[](Vector3DWithRandom value)
		{
			return value.GetBytes();
		}
	}
}