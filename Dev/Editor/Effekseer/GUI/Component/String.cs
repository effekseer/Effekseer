using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Component
{
	class String : Control, IParameterControl
	{
		string id = "";

		public string Label { get; set; } = string.Empty;

		public string Description { get; set; } = string.Empty;

		Data.Value.String binding = null;

		ValueChangingProperty valueChangingProp = new ValueChangingProperty();

		string internalValue = string.Empty;

		bool isActive = false;

		public bool EnableUndo { get; set; } = true;

		public Data.Value.String Binding
		{
			get
			{
				return binding;
			}
			set
			{
				if (binding == value) return;

				if(binding != null)
				{
					FixValue();
				}

				binding = value;

				if (binding != null)
				{
					internalValue = binding.Value;
				}
			}
		}

		public String(string label = null)
		{
			if (label != null)
			{
				Label = label;
			}

			id = "###" + Manager.GetUniqueID().ToString();
		}

		public void SetBinding(object o)
		{
			var o_ = o as Data.Value.String;
			Binding = o_;
		}

		public void FixValue()
		{
			if (binding == null) return;
			if (binding.Value == internalValue) return;
			binding.SetValue(internalValue);
		}

		public override void OnDisposed()
		{
			FixValue();
			Binding = null;
		}

		public override void Update()
		{
			string initialValue = string.Empty;
			if (binding != null)
			{
				initialValue = binding.Value;
			}
			else
			{
				initialValue = internalValue;
			}

			valueChangingProp.Enable(binding);

			if(binding.IsMultiLine)
			{
				if (Manager.NativeManager.InputTextMultiline(id, initialValue))
				{
					var v = Manager.NativeManager.GetInputTextResult();

					if (EnableUndo)
					{
						internalValue = v;
					}
					else
					{
						throw new Exception();
					}
				}
			}
			else
			{
				if (Manager.NativeManager.InputText(id, initialValue))
				{
					var v = Manager.NativeManager.GetInputTextResult();

					if (EnableUndo)
					{
						internalValue = v;
					}
					else
					{
						throw new Exception();
					}
				}
			}


			var isActive_Current = Manager.NativeManager.IsItemActive();

			if(isActive && !isActive_Current)
			{
				FixValue();
			}

			isActive = isActive_Current;

			valueChangingProp.Disable();
		}
	}
}
