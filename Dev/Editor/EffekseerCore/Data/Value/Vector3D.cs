using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using static Effekseer.InternalScript.SSAGenerator;

namespace Effekseer.Data.Value
{
	public class Vector3D : IResettableValue, IValueChangedFromDefault
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

		public Float Z
		{
			get;
			private set;
		}

		public bool CanSelectDynamicEquation = false;

		public Boolean IsDynamicEquationEnabled
		{
			get;
			private set;
		}

		public DynamicEquationReference DynamicEquation
		{
			get;
			private set;
		}

		public event ChangedValueEventHandler OnChanged;

		public bool IsValueChangedFromDefault
		{
			get { return X.IsValueChangedFromDefault || Y.IsValueChangedFromDefault || Z.IsValueChangedFromDefault; }
		}


		internal Vector3D(
			float x = 0,
			float y = 0,
			float z = 0,
			float x_max = float.MaxValue,
			float x_min = float.MinValue,
			float y_max = float.MaxValue,
			float y_min = float.MinValue,
			float z_max = float.MaxValue,
			float z_min = float.MinValue,
			float x_step = 1.0f,
			float y_step = 1.0f,
			float z_step = 1.0f)
		{
			X = new Float(x, x_max, x_min, x_step);
			Y = new Float(y, y_max, y_min, y_step);
			Z = new Float(z, z_max, z_min, z_step);

			IsDynamicEquationEnabled = new Boolean();
			DynamicEquation = new DynamicEquationReference();
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
				DynamicEquation.SetValue(null);
			}
			Command.CommandManager.EndCollection();
		}

		public void ChangeDefaultValue(float x, float y, float z)
		{
			X.ChangeDefaultValue(x);
			Y.ChangeDefaultValue(y);
			Z.ChangeDefaultValue(z);
		}

		public byte[] GetBytes(float mul = 1.0f)
		{
			byte[] values = new byte[sizeof(float) * 3];
			byte[] x = BitConverter.GetBytes(X.GetValue() * mul);
			byte[] y = BitConverter.GetBytes(Y.GetValue() * mul);
			byte[] z = BitConverter.GetBytes(Z.GetValue() * mul);
			x.CopyTo(values, 0);
			y.CopyTo(values, sizeof(float) * 1);
			z.CopyTo(values, sizeof(float) * 2);
			return values;
		}

		public static explicit operator byte[](Vector3D value)
		{
			return value.GetBytes();
		}

		public bool ValueEquals(object obj)
		{
			var o = obj as Vector3D;
			if (o == null)
				return false;

			if (X.Value != o.X.Value)
				return false;

			if (Y.Value != o.Y.Value)
				return false;

			if (Z.Value != o.Z.Value)
				return false;

			return true;
		}

		public int GetValueHashCode()
		{
			return Utils.Misc.CombineHashCodes(new[] { X.Value.GetHashCode(), Y.Value.GetHashCode(), Z.Value.GetHashCode() });
		}
	}
}