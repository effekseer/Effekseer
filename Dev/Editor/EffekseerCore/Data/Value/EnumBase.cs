using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data.Value
{
	public abstract class EnumBase
	{
		public abstract int GetValueAsInt();
		public abstract void SetValue(int value);
		public event ChangedValueEventHandler OnChanged;

		protected void Change(object value, ChangedValueType type)
		{
			if (OnChanged != null)
			{
				OnChanged(this, new ChangedValueEventArgs(value, type));
			}
		}
	}
}
