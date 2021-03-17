using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.Data.Value
{
	/// <summary>
	/// An object which has a name to show it in a list
	/// </summary>
	public interface INamedObject
	{
		String Name { get; }
	}

	class ObjectHolder<T> where T : class
	{
		public T Value { get; set; }

		public int Index { get; set; }
	}

	public class ObjectCollection<T> : IEditableValueCollection where T : class, new()
	{
		List<ObjectHolder<T>> values = new List<ObjectHolder<T>>();

		internal ObjectHolder<T> GetHolder(T value)
		{
			return values.FirstOrDefault(_ => _.Value == value);
		}

		public int GetIndex(T value)
		{
			if (value == null) return -1;

			return values.FindIndex(_ => _.Value == value);
		}

		public IReadOnlyList<T> Values
		{
			get
			{
				return values.Select(_ => _.Value).ToList();
			}
		}

		T selected = null;
		public T Selected
		{
			get
			{
				return selected;
			}
			set
			{
				selected = value;
				if (OnChanged != null)
				{
					OnChanged(this, null);
				}
			}
		}

		public bool Add(T value)
		{
			if (values.Count >= ElementMax) return false;

			var old_selected = selected;
			var old_values = values;
			var new_values = new List<ObjectHolder<T>>(values);

			var new_holder = new ObjectHolder<T>();
			new_holder.Value = value;
			new_values.Add(new_holder);

			var cmd = new Command.DelegateCommand(
				() =>
				{
					values = new_values;
					selected = new_values[new_values.Count - 1].Value;

					AssignIndexes(values);


					if (OnChanged != null)
					{
						OnChanged(this, null);
					}
				},
				() =>
				{
					values = old_values;
					selected = old_selected;
					new_holder.Index = -1;

					AssignIndexes(values);


					if (OnChanged != null)
					{
						OnChanged(this, null);
					}
				});

			Command.CommandManager.Execute(cmd);

			return true;
		}

		public bool New()
		{
			return Add(CreateValue());
		}

		public bool Delete(T o)
		{
			if (o == null)
				return false;

			var removing_holder = values.FirstOrDefault(_ => _.Value == o);
			var old_index = values.FindIndex(_ => _.Value == o);
			var old_values = values;
			var new_values = new List<ObjectHolder<T>>(values);
			new_values.Remove(removing_holder);

			var cmd = new Command.DelegateCommand(
				() =>
				{
					values = new_values;

					AssignIndexes(values);

					removing_holder.Index = -1;

					if (old_index < values.Count) selected = new_values[old_index].Value;
					else if (old_index > 0 && values.Count > 0) selected = new_values[old_index - 1].Value;
					else selected = null;

					if (OnChanged != null)
					{
						OnChanged(this, null);
					}
				},
				() =>
				{
					values = old_values;

					AssignIndexes(values);

					selected = o;
					if (OnChanged != null)
					{
						OnChanged(this, null);
					}
				});

			Command.CommandManager.Execute(cmd);

			return true;
		}

		public void Clear()
		{
			var old_values = values;
			var old_selected = selected;
			var new_values = new List<ObjectHolder<T>>();
			
			var cmd = new Command.DelegateCommand(
				() =>
				{
					values = new_values;

					AssignIndexes(values);
					selected = null;

					if (OnChanged != null)
					{
						OnChanged(this, null);
					}
				},
				() =>
				{
					values = old_values;

					AssignIndexes(values);

					selected = old_selected;
					if (OnChanged != null)
					{
						OnChanged(this, null);
					}
				});

			Command.CommandManager.Execute(cmd);
		}

		static void AssignIndexes(List<ObjectHolder<T>> values)
		{
			for (int i = 0; i < values.Count; i++)
			{
				values[i].Index = i;
			}
		}

		public EditableValue[] GetValues()
		{
			List<EditableValue> ret = new List<EditableValue>();

			if (selected == null) return ret.ToArray();

			return GetEditableValues(selected);
		}

		public event ChangedValueEventHandler OnChanged;

		public Func<T> CreateValue;

		public Func<T, EditableValue[]> GetEditableValues;

		public int ElementMax = int.MaxValue;
	}

	public class ObjectReference<T> where T : class
	{
		public T Value
		{
			get
			{
				return holderedValue?.Value;
			}
		}

		ObjectHolder<T> holderedValue;

		public void SetValue(T param)
		{
			if (param == Value) return;

			var holder = GetHolder(param);

			var old_value = holderedValue;
			var new_value = holder;

			var cmd = new Command.DelegateCommand(
				() =>
				{
					holderedValue = new_value;
				},
				() =>
				{
					holderedValue = old_value;
				});

			Command.CommandManager.Execute(cmd);
		}

		public void SetValueWithIndex(int index)
		{
			var v = GetValueWithIndex(index);
			if (v == null)
			{
				return;
			}
			SetValue(v);
		}

		public int Index
		{
			get
			{
				return holderedValue?.Index ?? -1;
			}
		}

		internal Func<T, ObjectHolder<T>> GetHolder;

		internal Func<int, T> GetValueWithIndex;
	}

}
