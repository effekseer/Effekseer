using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.BindableComponent
{
	class Boolean : Control, IParameterControl
	{
		string id = "";
		string id_c = "";
		string id_reset = "";

		Data.Value.Boolean binding = null;

		bool isPopupShown = false;

		ValueChangingProperty valueChangingProp = new ValueChangingProperty();

		bool[] internalValue = new bool[] { false };

		public bool EnableUndo { get; set; } = true;

		public Data.Value.Boolean Binding
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

		public Boolean()
		{
			id = "###" + Manager.GetUniqueID().ToString();
			id_c = "###" + Manager.GetUniqueID().ToString();
			id_reset = "###" + Manager.GetUniqueID().ToString();
		}

		public void SetBinding(object o)
		{
			var o_ = o as Data.Value.Boolean;
			Binding = o_;
		}

		public void FixValue()
		{
		}

		public override void Update()
		{
			isPopupShown = false;

			if (binding != null)
			{
				internalValue[0] = binding.Value;
			}

			valueChangingProp.Enable(binding);

			if (Manager.NativeManager.Checkbox(id, internalValue))
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

			Popup();

			valueChangingProp.Disable();
		}

		void Popup()
		{
			if (isPopupShown) return;

			if (Manager.NativeManager.BeginPopupContextItem(id_c))
			{
				Functions.ShowReset(binding, id_reset);

				Manager.NativeManager.EndPopup();

				isPopupShown = true;
			}
		}
	}
}