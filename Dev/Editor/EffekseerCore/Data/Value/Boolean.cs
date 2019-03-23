using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data.Value
{
	public class Boolean
	{
		bool _value = false;

		public bool Value
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

		public event ChangedValueEventHandler OnChanged;

		public bool DefaultValue { get; private set; }

		public bool IsValueChangedFromDefault
		{
			get { return Value != DefaultValue; }
		}

		internal Boolean(bool value = false)
		{
			_value = value;
			DefaultValue = _value;
		}

		public bool GetValue()
		{
			return _value;
		}

		public void SetValue(bool value)
		{
			if (value == _value) return;

			bool old_value = _value;
			bool new_value = value;

			var cmd = new Command.DelegateCommand(
				() =>
				{
					_value = new_value;

					if (OnChanged != null)
					{ 
						OnChanged(this, new ChangedValueEventArgs(new_value, ChangedValueType.Execute));
					}
				},
				() =>
				{
					_value = old_value;

					if (OnChanged != null)
					{
						OnChanged(this, new ChangedValueEventArgs(old_value, ChangedValueType.Unexecute));
					}
				});

			Command.CommandManager.Execute(cmd);
		}

		public void SetValueDirectly(bool value)
		{
			if (_value == value) return;

			_value = value;

			if (OnChanged != null)
			{
				OnChanged(this, new ChangedValueEventArgs(_value, ChangedValueType.Execute));
			}
		}

		public static implicit operator bool(Boolean value)
		{
			return value._value;
		}
	}
}
