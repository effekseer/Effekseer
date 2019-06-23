using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data.Value
{
	public class String
	{
		string _value = string.Empty;

		public string Value
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

		public string DefaultValue { get; private set; }

		public bool IsValueChangedFromDefault
		{
			get { return Value != DefaultValue; }
		}

		/// <summary>
		/// Is multiline on GUI
		/// </summary>
		public bool IsMultiLine = false;

		internal String(string value = "")
		{
			_value = value;
			DefaultValue = _value;
		}

		public string GetValue()
		{
			return _value;
		}

		public void SetValue(string value)
		{
			if (value == _value) return;

			string old_value = _value;
			string new_value = value;

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

		internal void SetValueDirectly(string value)
		{
			if (_value == value) return;

			_value = value;
			if (OnChanged != null)
			{
				OnChanged(this, new ChangedValueEventArgs(_value, ChangedValueType.Execute));
			}
		}

		public static implicit operator string(String value)
		{
			return value._value;
		}
	}
}
