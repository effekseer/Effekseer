using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data.Value
{
	public class Enum<T> : EnumBase where T : struct, IComparable, IFormattable, IConvertible
	{
		T _value = default(T);

		public T Value
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

		internal T DefaultValue { get; private set; }

		internal Enum(T value = default(T))
		{
			_value = value;
			DefaultValue = value;
		}

		public T GetValue()
		{
			return _value;
		}

		public override int GetValueAsInt()
		{
			dynamic v = _value;
			return (int)v;
		}

		public override int GetDefaultValueAsInt()
		{
			dynamic v = DefaultValue;
			return (int)v;
		}

		public void SetValue(T value)
		{
			if (value.ToString() == _value.ToString()) return;

			var old_value = _value;
			var new_value = value;

			var cmd = new Command.DelegateCommand(
				() =>
				{
					_value = new_value;
					Change(new_value, ChangedValueType.Execute);
				},
				() =>
				{
					_value = old_value;
					Change(old_value, ChangedValueType.Unexecute);
				});

			Command.CommandManager.Execute(cmd);
		}

		public override void SetValue(int value)
		{
			dynamic v = value;
			SetValue((T)v);
		}

		public void SetValueDirectly(T value)
		{
			if (_value.ToString() == value.ToString()) return;

			_value = value;
			Change(_value, ChangedValueType.Execute);
		}

		public override void SetValueDirectly(int value)
		{
			dynamic v = value;
			SetValueDirectly((T)v);
		}

		public override Type GetEnumType()
		{
			return typeof(T);
		}

		public static implicit operator T(Enum<T> value)
		{
			return value._value;
		}

		public static implicit operator byte[](Enum<T> value)
		{
			var n = value.GetValueAsInt();
			return BitConverter.GetBytes(n);
		}
	}
}
