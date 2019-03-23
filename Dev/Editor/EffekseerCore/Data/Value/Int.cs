using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Effekseer.Utl;

namespace Effekseer.Data.Value
{
	public class Int
	{
		int _value = 0;
		int _max = int.MaxValue;
		int _min = int.MinValue;
		
		public int Min
		{
			get
			{
				return _min;
			}
		}

		public int Max
		{
			get
			{
				return _max;
			}
		}

		public int Value
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
		public int Step
		{
			get;
			set;
		}

		public event ChangedValueEventHandler OnChanged;

		public int DefaultValue { get; private set; }

		public bool IsValueChangedFromDefault
		{
			get { return Value != DefaultValue; }
		}

		internal Int(int value = 0, int max = int.MaxValue, int min = int.MinValue, int step = 1 )
		{
			_max = max;
			_min = min;
			_value = value.Clipping(_max, _min);
			Step = step;

			DefaultValue = value;
		}

		protected void CallChanged(object value, ChangedValueType type)
		{
			if (OnChanged != null)
			{
				OnChanged(this, new ChangedValueEventArgs(value, type));
			}
		}

		public int GetValue()
		{
			return _value;
		}

		public void SetValue(int value, bool isCombined = false)
		{
			value = value.Clipping(_max, _min);

			if (value == _value) return;

			int old_value = _value;
			int new_value = value;

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

		public void SetValueDirectly(int value)
		{
			var converted = value.Clipping(_max, _min);
			if (_value == converted) return;
			_value = converted;

			CallChanged(_value, ChangedValueType.Execute);
		}

		public static implicit operator int(Int value)
		{
			return value._value;
		}
	}
}
