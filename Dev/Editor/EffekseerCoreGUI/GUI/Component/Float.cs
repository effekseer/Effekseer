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
		string id_c = "";
		string id_d = "";
		string id_reset = "";

		Data.Value.Float binding = null;

		ValueChangingProperty valueChangingProp = new ValueChangingProperty();

		float[] internalValue = new float[] { 0.0f };

		bool isActive = false;

		bool isPopupShown = false;

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

		public Float()
		{
			id = "###" + GUIManager.GetUniqueID().ToString();
			id_c = "###" + GUIManager.GetUniqueID().ToString();
			id_d = "###" + GUIManager.GetUniqueID().ToString();
			id_reset = "###" + GUIManager.GetUniqueID().ToString();
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
			isPopupShown = false;

			if (binding == null) return;

			valueChangingProp.Enable(binding);

			internalValue[0] = binding.Value;

			if (GUIManager.NativeManager.DragFloat(id, internalValue, binding.Step / 10.0f, binding.RangeMin, binding.RangeMax))
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

			Popup();

			var isActive_Current = GUIManager.NativeManager.IsItemActive();

			if (isActive && !isActive_Current)
			{
				FixValue();
			}

			isActive = isActive_Current;

			if (binding.IsDynamicEquationEnabled)
			{
				DynamicSelector.SelectInComponent(id_d, binding.DynamicEquation);
				Popup();
			}

			valueChangingProp.Disable();
		}

		void Popup()
		{
			if (isPopupShown) return;

			if (GUIManager.NativeManager.BeginPopupContextItem(id_c))
			{
				Functions.ShowReset(binding, id_reset);

				if (binding.CanSelectDynamicEquation)
				{
					DynamicSelector.Popup(id_c, binding.DynamicEquation, binding.IsDynamicEquationEnabled);
				}

				GUIManager.NativeManager.EndPopup();

				isPopupShown = true;
			}
		}
	}
}
