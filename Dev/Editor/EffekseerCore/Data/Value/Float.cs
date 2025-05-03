using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Effekseer.Utils;

namespace Effekseer.Data.Value
{
	public class Float : IResettableValue, IValueChangedFromDefault
	{
		float? _value = null;
		float _max = float.MaxValue;
		float _min = float.MinValue;

		public float Value
		{
			get
			{
				return GetValue();
			}

			set
			{
				SetValue(value);
			}
		}

		/// <summary>
		/// 変更単位量
		/// </summary>
		public float Step
		{
			get;
			set;
		}

		public float RangeMin
		{
			get { return _min; }
		}

		public float RangeMax
		{
			get { return _max; }
		}

		public float DefaultValue { get; private set; }

		public bool IsValueChangedFromDefault
		{
			get { return Value != DefaultValue; }
		}

		public event ChangedValueEventHandler OnChanged;

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

		internal Float(float value = 0, float max = float.MaxValue, float min = float.MinValue, float step = 1.0f)
		{
			value = value.Clipping(_max, _min);

			_max = max;
			_min = min;
			DefaultValue = value;
			Step = step;

			IsDynamicEquationEnabled = new Boolean();
			DynamicEquation = new DynamicEquationReference();
		}

		protected void CallChanged(object value, ChangedValueType type)
		{
			if (OnChanged != null)
			{
				OnChanged(this, new ChangedValueEventArgs(value, type));
			}
		}

		public void ChangeDefaultValue(float value)
		{
			DefaultValue = value;
		}

		public float GetValue()
		{
			return _value.HasValue ? _value.Value : DefaultValue;
		}

		public void SetValue(float value, bool isCombined = false)
		{
			value = value.Clipping(_max, _min);

			if (value == Value) return;

			float? old_value = _value;
			float? new_value = value;

			var cmd = new Command.DelegateCommand(
				() =>
				{
					_value = new_value;

					CallChanged(new_value, ChangedValueType.Execute);
				},
				() =>
				{
					_value = old_value;

					CallChanged(old_value, ChangedValueType.Unexecute);
				},
				this,
				isCombined);

			Command.CommandManager.Execute(cmd);
		}

		public void SetValueDirectly(float value)
		{
			var converted = value.Clipping(_max, _min);
			if (_value == converted) return;
			_value = converted;

			CallChanged(_value, ChangedValueType.Execute);
		}

		public void ResetValue()
		{
			Command.CommandManager.StartCollection();
			SetValue(DefaultValue);
			if (CanSelectDynamicEquation)
			{
				IsDynamicEquationEnabled.ResetValue();
				DynamicEquation.SetValue(null);
			}
			Command.CommandManager.EndCollection();
		}

		public static implicit operator float(Float value)
		{
			return value.GetValue();
		}

		public static explicit operator byte[](Float value)
		{
			return value.GetBytes();
		}

		public byte[] GetBytes(float mul = 1.0f)
		{
			byte[] values = new byte[sizeof(float) * 1];
			BitConverter.GetBytes(Value * mul).CopyTo(values, sizeof(float) * 0);
			return values;
		}
	}
}