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

	public class DynamicVector
	{
		public Value.String Name { get; private set; }

		public Value.String X { get; private set; }
		public Value.String Y { get; private set; }
		public Value.String Z { get; private set; }
		public Value.String W { get; private set; }

		DynamicVectorCollection parent = null;

		public DynamicVector(string name, DynamicVectorCollection parent)
		{
			Name = new Value.String(name);
			X = new Value.String();
			Y = new Value.String();
			Z = new Value.String();
			W = new Value.String();

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
				v.Title = (i + 1).ToString();
				v.IsUndoEnabled = false;
				ret.Add(v);
			}

			return ret.ToArray();
		}

		public event ChangedValueEventHandler OnChanged;
	}

	public class DynamicVectorCollection : IEditableValueCollection
	{
		List<DynamicVector> values = new List<DynamicVector>();

		public List<DynamicVector> Values
		{
			get
			{
				return values;
			}
		}

		DynamicVector selected = null;
		public DynamicVector Selected
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

			var old_value = values;
			var new_value = new List<DynamicVector>(values);
			new_value.Add(new DynamicVector("Param", this));


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

			return true;
		}

		public EditableValue[] GetValues()
		{
			List<EditableValue> ret = new List<EditableValue>();

			if (selected == null) return ret.ToArray();

			EditableValue vn = new EditableValue();
			vn.Value = selected.Name;
			vn.Title = "Name";
			vn.IsUndoEnabled = true;
			ret.Add(vn);

			EditableValue vx = new EditableValue();
			vx.Value = selected.X;
			vx.Title = "X";
			vx.IsUndoEnabled = true;
			ret.Add(vx);

			EditableValue vy = new EditableValue();
			vy.Value = selected.Y;
			vy.Title = "Y";
			vy.IsUndoEnabled = true;
			ret.Add(vy);

			EditableValue vz = new EditableValue();
			vz.Value = selected.Z;
			vz.Title = "Z";
			vz.IsUndoEnabled = true;
			ret.Add(vz);

			EditableValue vw = new EditableValue();
			vw.Value = selected.W;
			vw.Title = "W";
			vw.IsUndoEnabled = true;
			ret.Add(vw);

			return ret.ToArray();
		}

		public event ChangedValueEventHandler OnChanged;
	}

	public class DynamicValues
	{
		public DynamicInputCollection Inputs { get; private set; }

		public DynamicVectorCollection Vectors { get; private set; }

		public DynamicValues()
		{
			Inputs = new DynamicInputCollection();

			for(int i = 0; i < 4; i++)
			{
				Inputs.Add();
			}

			Vectors = new DynamicVectorCollection();
		}
	}
}
