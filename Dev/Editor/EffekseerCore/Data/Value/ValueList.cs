using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.Data.Value
{
	public class ValueList
	{
		public ICollection Values
		{
			get
			{
				return values;
			}
		}

		List<object> values = new List<object>();

		public void Add(object o)
		{
			var new_value = Clone(values);
			var old_value = Clone(values);
			new_value.Add(o);

			ExecCommand(new_value, old_value);
		}

		public void Remove(object o)
		{
			var new_value = Clone(values);
			var old_value = Clone(values);
			new_value.Remove(o);

			ExecCommand(new_value, old_value);
		}

		public void RemoveAt(int ind)
		{
			var new_value = Clone(values);
			var old_value = Clone(values);
			new_value.RemoveAt(ind);

			ExecCommand(new_value, old_value);
		}

		List<object> Clone(List<object> list)
		{
			var ret = new List<object>();
			ret.AddRange(list);
			return ret;
		}

		void ExecCommand(List<object> new_value, List<object> old_value)
		{
			var cmd = new Command.DelegateCommand(
	() =>
	{
		values = new_value;

		if (OnChanged != null)
		{
			OnChanged(this, new ChangedValueEventArgs(new_value, ChangedValueType.Execute));
		}
	},
	() =>
	{
		values = old_value;

		if (OnChanged != null)
		{
			OnChanged(this, new ChangedValueEventArgs(old_value, ChangedValueType.Unexecute));
		}
	});

			Command.CommandManager.Execute(cmd);
		}

		public event ChangedValueEventHandler OnChanged;
	}
}
