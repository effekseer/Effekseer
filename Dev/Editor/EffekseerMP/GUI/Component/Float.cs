using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Component
{
	class Float : Control, IParameterControl
	{
		string id = "";

		public string Label { get; set; } = string.Empty;

		public string Description { get; set; } = string.Empty;

		Data.Value.Float binding = null;

		float[] internalValue = new float[] { 0.0f };

		bool isActive = false;

		public bool EnableUndo { get; set; } = true;

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

			id = "###" + Manager.GetUniqueID().ToString();
		}

		public void SetBinding(object o)
		{
			var o_ = o as Data.Value.Float;
			Binding = o_;
		}

		public void FixValue()
		{
			if (binding == null) return;

			if (EnableUndo)
			{
				binding.SetValue(internalValue[0]);
			}
			else
			{
				binding.SetValueDirectly(internalValue[0]);
			}
		}

		public override void OnDisposed()
		{
			FixValue();
		}

		public override void Update()
		{
			if (binding == null) return;
			
			internalValue[0] = binding.Value;

			if (Manager.NativeManager.DragFloat(id, internalValue, binding.Step / 10.0f, binding.RangeMin, binding.RangeMax))
			{
				if (EnableUndo)
				{
					binding.SetValue(internalValue[0], isActive);
				}
				else
				{
					binding.SetValueDirectly(internalValue[0]);
				}
			}

			var isActive_Current = Manager.NativeManager.IsItemActive();

			if (isActive && !isActive_Current)
			{
				FixValue();
			}

			isActive = isActive_Current;
		}
	}
}
