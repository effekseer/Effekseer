using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.BindableComponent
{
	class Int : Control, IParameterControl
	{
		string id = "";

		Data.Value.Int binding = null;

		ValueChangingProperty valueChangingProp = new ValueChangingProperty();

		int[] internalValue = new int[] { 0 };

		bool isActive = false;

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

		public Int()
		{
			id = "###" + Manager.GetUniqueID().ToString();
		}

		public void SetBinding(object o)
		{
			var o_ = o as Data.Value.Int;
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
			if (binding != null)
			{
				internalValue[0] = binding.Value;
			}

			valueChangingProp.Enable(binding);

			if (Manager.NativeManager.DragInt(id, internalValue, binding.Step, binding.Min, binding.Max, "%d"))
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

			valueChangingProp.Disable();
		}
	}
}