using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data.Value
{
	public class Vector3DWithRandom
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

		bool isDynamicEquationEnabled = false;
		public bool IsDynamicEquationEnabled
		{
			get
			{
				return isDynamicEquationEnabled;
			}
			set
			{
				var old_value = isDynamicEquationEnabled;
				var new_value = value;

				var cmd = new Command.DelegateCommand(
					() =>
					{
						isDynamicEquationEnabled = new_value;

						if (OnChanged != null)
						{
							OnChanged(this, new ChangedValueEventArgs(new_value, ChangedValueType.Execute));
						}
					},
					() =>
					{
						isDynamicEquationEnabled = old_value;

						if (OnChanged != null)
						{
							OnChanged(this, new ChangedValueEventArgs(old_value, ChangedValueType.Unexecute));
						}
					});

				Command.CommandManager.Execute(cmd);
			}
		}
		public DynamicEquation DynamicEquationMin
		{
			get;
			private set;
		}

		public void SetDynamicEquationMin(DynamicEquation param)
		{
			if (param == DynamicEquationMin) return;

			var old_value = DynamicEquationMin;
			var new_value = param;

			var cmd = new Command.DelegateCommand(
				() =>
				{
					DynamicEquationMin = new_value;

					if (OnChanged != null)
					{
						OnChanged(this, new ChangedValueEventArgs(new_value, ChangedValueType.Execute));
					}
				},
				() =>
				{
					DynamicEquationMin = old_value;

					if (OnChanged != null)
					{
						OnChanged(this, new ChangedValueEventArgs(old_value, ChangedValueType.Unexecute));
					}
				});

			Command.CommandManager.Execute(cmd);
		}

		public DynamicEquation DynamicEquationMax
		{
			get;
			private set;
		}

		public void SetDynamicEquationMax(DynamicEquation param)
		{
			if (param == DynamicEquationMax) return;

			var old_value = DynamicEquationMax;
			var new_value = param;

			var cmd = new Command.DelegateCommand(
				() =>
				{
					DynamicEquationMax = new_value;

					if (OnChanged != null)
					{
						OnChanged(this, new ChangedValueEventArgs(new_value, ChangedValueType.Execute));
					}
				},
				() =>
				{
					DynamicEquationMax = old_value;

					if (OnChanged != null)
					{
						OnChanged(this, new ChangedValueEventArgs(old_value, ChangedValueType.Unexecute));
					}
				});

			Command.CommandManager.Execute(cmd);
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
			DrawnAs = drawnas;
			DefaultDrawnAs = DrawnAs;
		}

		public byte[] GetBytes( float mul = 1.0f)
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
