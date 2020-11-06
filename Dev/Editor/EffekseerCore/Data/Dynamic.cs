using Effekseer.Data.Value;
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

	public class DynamicEquation : INamedObject
	{
		public Value.String Name { get; private set; }

		public Value.String Code { get; private set; }

		public DynamicEquation()
		{
			Name = new Value.String("");
			Code = new Value.String("");
			Code.IsMultiLine = true;
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

		public void Add(bool isUndoEnabled = true)
		{
			var old_value = values;
			var new_value = new List<DynamicInput>(values);
			new_value.Add(new DynamicInput());

			if (isUndoEnabled)
			{
				var cmd = new Command.DelegateCommand(
				() =>
				{
					values = new_value;
					if (OnChanged != null)
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
			else
			{
				values = new_value;
			}
		}

		public EditableValue[] GetValues()
		{
			List<EditableValue> ret = new List<EditableValue>();

			for (int i = 0; i < values.Count; i++)
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

	public class DynamicEquationCollection : ObjectCollection<DynamicEquation>
	{
		public DynamicEquationCollection()
		{
			ElementMax = 16;

			CreateValue += () =>
			{
				var value = new DynamicEquation();
				value.Name.SetValue("New Expression");
				value.Code.SetValue("@O.x = @In0\n@O.y = @In1");
				return value;
			};

			GetEditableValues += (v) =>
			{
				List<EditableValue> ret = new List<EditableValue>();

				EditableValue vn = new EditableValue();
				vn.Value = v.Name;
				vn.Title = Resources.GetString("DynamicName");
				vn.IsUndoEnabled = true;
				ret.Add(vn);

				EditableValue vx = new EditableValue();
				vx.Value = v.Code;
				vx.Title = Resources.GetString("DynamicEq");
				vx.IsUndoEnabled = true;
				ret.Add(vx);

				return ret.ToArray();
			};
		}
	}

	public class DynamicValues
	{
		public DynamicInputCollection Inputs { get; private set; }

		public DynamicEquationCollection Equations { get; private set; }

		public DynamicValues()
		{
			Inputs = new DynamicInputCollection();

			for (int i = 0; i < 4; i++)
			{
				Inputs.Add(false);
			}

			Equations = new DynamicEquationCollection();
		}
	}


}
