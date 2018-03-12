using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Component
{
	class Float : IControl
	{
		string id = "";

		public string Label { get; set; } = string.Empty;

		Data.Value.Float binding = null;

		float[] internalValue = new float[] { 0.0f };

		public bool ShouldBeRemoved { get; private set; } = false;

		public bool EnableUndo { get; set; }

		public Data.Value.Float Binding
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

		public Float(string label = null)
		{
			if (label != null)
			{
				Label = label;
			}

			var rand = new Random();
			id = "###" + Manager.GetUniqueID().ToString();
		}

		public void SetBinding(object o)
		{
			var o_ = o as Data.Value.Float;
			Binding = o_;
		}

		public void Update()
		{
			if (binding != null)
			{
				internalValue[0] = binding.Value;
			}

			if (Manager.NativeManager.DragFloat(Label + id, internalValue))
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
