using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.Data
{
	public class DynamicInput
	{
		public Value.Float Input { get; private set; }

		public DynamicInput()
		{
			Input = new Value.Float();
		}
	}

	public class DynamicEquation
	{
		public Value.String Name { get; private set; }

		public Value.String Code { get; private set; }

		DynamicEquationCollection parent = null;

		public DynamicEquation(DynamicEquationCollection parent)
		{
			Name = new Value.String("");
			Code = new Value.String("");
			Code.IsMultiLine = true;
			this.parent = parent;
		}


		/// <summary>
		/// This instance is enabled if it exists in a list and there is not instances with same name
		/// </summary>
		public bool IsValid
		{
			get
			{
				if (!parent.Values.Any(_ => _ == this)) return false;
				return true;
			}
		}
	}

	public class DynamicInputCollection : IEditableValueCollection
	{
		List<DynamicInput> values = new List<DynamicInput>();

		public List<DynamicInput> Values
		{
			get
			{
				return values;
			}
		}

		public void Add()
		{
			var old_value = values;
			var new_value = new List<DynamicInput>(values);
			new_value.Add(new DynamicInput());


			var cmd = new Command.DelegateCommand(
				() =>
				{
					values = new_value;
					if(OnChanged != null)
					{
						OnChanged(this, null);
					}
				},
				() =>
				{
					values = old_value;
					if (OnChanged != null)
					{
						OnChanged(this, null);
					}
				});

			Command.CommandManager.Execute(cmd);
		}

		public EditableValue[] GetValues()
		{
			List<EditableValue> ret = new List<EditableValue>();

			for(int i = 0; i < values.Count; i++)
			{
				EditableValue v = new EditableValue();

				v.Value = values[i].Input;
				v.Title = (i).ToString();
				v.IsUndoEnabled = false;
				ret.Add(v);
			}

			return ret.ToArray();
		}

		public event ChangedValueEventHandler OnChanged;
	}

	public class DynamicEquationCollection : IEditableValueCollection
	{
		List<DynamicEquation> values = new List<DynamicEquation>();

		public int GetIndex(DynamicEquation vector)
		{
			if (vector == null) return -1;
			return Values.IndexOf(vector);
		}

		public List<DynamicEquation> Values
		{
			get
			{
				return values;
			}
		}

		DynamicEquation selected = null;
		public DynamicEquation Selected
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

		public bool Add()
		{
			if (values.Count >= 16) return false;

			var old_selected = selected;
			var old_value = values;
			var new_value = new List<DynamicEquation>(values);

			var value = new DynamicEquation(this);
			value.Name.SetValue("New Expression");
			value.Code.SetValue("@O.x = @In0\n@O.y = @In1");
			new_value.Add(value);

			var cmd = new Command.DelegateCommand(
				() =>
				{
					values = new_value;
					selected = new_value[new_value.Count - 1];
					if (OnChanged != null)
					{
						OnChanged(this, null);
					}
				},
				() =>
				{
					values = old_value;
					selected = old_selected;
					if (OnChanged != null)
					{
						OnChanged(this, null);
					}
				});

			Command.CommandManager.Execute(cmd);

			return true;
		}

		public bool Delete(DynamicEquation o)
		{
			if (o == null)
				return false;

			var old_index = values.IndexOf(o);
			var old_value = values;
			var new_value = new List<DynamicEquation>(values);
			new_value.Remove(o);

			var cmd = new Command.DelegateCommand(
				() =>
				{
					values = new_value;
					
					if (old_index < values.Count) selected = new_value[old_index];
					else if (old_index > 0 && values.Count > 0) selected = new_value[old_index - 1];
					else selected = null;

					if (OnChanged != null)
					{
						OnChanged(this, null);
					}
				},
				() =>
				{
					values = old_value;
					selected = o;
					if (OnChanged != null)
					{
						OnChanged(this, null);
					}
				});

			Command.CommandManager.Execute(cmd);

			return true;
		}

		public EditableValue[] GetValues()
		{
			List<EditableValue> ret = new List<EditableValue>();

			if (selected == null) return ret.ToArray();

			EditableValue vn = new EditableValue();
			vn.Value = selected.Name;
			vn.Title = Resources.GetString("DynamicName");
			vn.IsUndoEnabled = true;
			ret.Add(vn);

			EditableValue vx = new EditableValue();
			vx.Value = selected.Code;
			vx.Title = Resources.GetString("DynamicEq");
			vx.IsUndoEnabled = true;
			ret.Add(vx);

			return ret.ToArray();
		}

		public event ChangedValueEventHandler OnChanged;
	}

	public class DynamicValues
	{
		public DynamicInputCollection Inputs { get; private set; }

		public DynamicEquationCollection Equations { get; private set; }

		public DynamicValues()
		{
			Inputs = new DynamicInputCollection();

			for(int i = 0; i < 4; i++)
			{
				Inputs.Add();
			}

			Equations = new DynamicEquationCollection();
		}
	}
}
