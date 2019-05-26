using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data.Value
{
	public class Vector3D
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

        bool isDynamicParameterEnabled = false;
        public bool IsDynamicParameterEnabled
        {
            get
            {
                return isDynamicParameterEnabled;
            }
            set
            {
                var old_value = isDynamicParameterEnabled;
                var new_value = value;

                var cmd = new Command.DelegateCommand(
                    () =>
                    {
                        isDynamicParameterEnabled = new_value;

                        if (OnChanged != null)
                        {
                            OnChanged(this, new ChangedValueEventArgs(new_value, ChangedValueType.Execute));
                        }
                    },
                    () =>
                    {
                        isDynamicParameterEnabled = old_value;

                        if (OnChanged != null)
                        {
                            OnChanged(this, new ChangedValueEventArgs(old_value, ChangedValueType.Unexecute));
                        }
                    });

                Command.CommandManager.Execute(cmd);
            }
        }
		public DynamicVector DynamicParameter
		{
			get;
			private set;
		}

		public void SetDynamicParameter(DynamicVector param)
		{
			if (param == DynamicParameter) return;

			var old_value = DynamicParameter;
			var new_value = param;

			var cmd = new Command.DelegateCommand(
				() =>
				{
					DynamicParameter = new_value;

					if (OnChanged != null)
					{
						OnChanged(this, new ChangedValueEventArgs(new_value, ChangedValueType.Execute));
					}
				},
				() =>
				{
					DynamicParameter = old_value;

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
		}

		public static explicit operator byte[](Vector3D value)
		{
			byte[] values = new byte[sizeof(float) * 3];
			byte[] x = BitConverter.GetBytes(value.X.GetValue());
			byte[] y = BitConverter.GetBytes(value.Y.GetValue());
			byte[] z = BitConverter.GetBytes(value.Z.GetValue());
			x.CopyTo(values, 0);
			y.CopyTo(values, sizeof(float) * 1);
			z.CopyTo(values, sizeof(float) * 2);
			return values;
		}
	}
}
