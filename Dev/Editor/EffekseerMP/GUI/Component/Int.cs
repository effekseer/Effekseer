using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Component
{
	class Int : IParameterControl
	{
		string id = "";

		public string Label { get; set; } = string.Empty;

		Data.Value.Int binding = null;

		int[] internalValue = new int[] { 0 };

		public bool EnableUndo { get; set; } = true;

		public Data.Value.Int Binding
		{
			get
			{
				return binding;
			}
			set
			{
				if (binding == value) return;

				binding = value;

				if (binding != null)
				{
					internalValue[0] = binding.Value;
				}
			}
		}

		public Int(string label = null)
		{
			if (label != null)
			{
				Label = label;
			}

			id = "###" + Manager.GetUniqueID().ToString();
		}

		public void SetBinding(object o)
		{
			var o_ = o as Data.Value.Int;
			Binding = o_;
		}

		public void FixValue()
		{
		}

		public void OnDisposed()
		{
		}

		public void Update()
		{
			if (binding != null)
			{
				internalValue[0] = binding.Value;
			}

			if (Manager.NativeManager.DragInt(Label + id, internalValue))
			{
				if (EnableUndo)
				{
					binding.SetValue(internalValue[0]);
				}
				else
				{
					binding.SetValueDirectly(internalValue[0]);
				}
			}
		}
	}
}
