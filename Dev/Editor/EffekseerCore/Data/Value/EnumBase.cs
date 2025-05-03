using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data.Value
{
	public abstract class EnumBase : IResettableValue
	{
		public abstract int GetValueAsInt();
		public abstract int GetDefaultValueAsInt();
		public abstract void SetValue(int value);
		public abstract void SetValueDirectly(int value);
		public event ChangedValueEventHandler OnChanged;

		public abstract Type GetEnumType();

		public abstract void ResetValue();

		protected void Change(object value, ChangedValueType type)
		{
			if (OnChanged != null)
			{
				OnChanged(this, new ChangedValueEventArgs(value, type));
			}
		}
	}
}