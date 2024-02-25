using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.BindableComponent
{
	class IntWithInifinite : Control, IParameterControl
	{
		string id1 = "";
		string id2 = "";
		string id_c = "";
		string id_d = "";

		Data.Value.IntWithInifinite binding = null;

		ValueChangingProperty valueChangingProp = new ValueChangingProperty();

		int[] internalValue = new int[] { 0 };
		bool[] isInfinite = new bool[] { false };

		bool isActive = false;

		bool isPopupShown = false;

		public bool EnableUndo { get; set; } = true;

		public Data.Value.IntWithInifinite Binding
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
					isInfinite[0] = binding.Infinite.Value;
				}
			}
		}

		public IntWithInifinite()
		{
			id1 = "###" + Manager.GetUniqueID().ToString();
			id2 = "###" + Manager.GetUniqueID().ToString();
			id_c = "###" + Manager.GetUniqueID().ToString();
			id_d = "###" + Manager.GetUniqueID().ToString();
		}

		public void SetBinding(object o)
		{
			var o_ = o as Data.Value.IntWithInifinite;
			Binding = o_;
		}

		public void FixValue()
		{
			if (binding == null) return;

			if (EnableUndo)
			{
				binding.Value.SetValue(internalValue[0]);
				binding.Infinite.SetValue(isInfinite[0]);
			}
			else
			{
				binding.Value.SetValueDirectly(internalValue[0]);
				binding.Infinite.SetValueDirectly(isInfinite[0]);
			}
		}

		public override void OnDisposed()
		{
			FixValue();
		}

		public override void Update()
		{
			isPopupShown = false;

			if (binding != null)
			{
				internalValue[0] = binding.Value;
				isInfinite[0] = binding.Infinite.Value;
			}

			valueChangingProp.Enable(binding);

			Manager.NativeManager.PushItemWidth(60);

			if (Manager.NativeManager.DragInt(id1, internalValue, binding.Value.Step, binding.Value.Min, binding.Value.Max, "%d"))
			{
				if (EnableUndo)
				{
					binding.Value.SetValue(internalValue[0], isActive);
				}
				else
				{
					binding.Value.SetValueDirectly(internalValue[0]);
				}
			}

			Popup();

			Manager.NativeManager.PopItemWidth();

			var isActive_Current = Manager.NativeManager.IsItemActive();

			if (isActive && !isActive_Current)
			{
				FixValue();
			}

			isActive = isActive_Current;

			Manager.NativeManager.SameLine();

			var inf = MultiLanguageTextProvider.GetText("Infinite");
			if (Manager.NativeManager.Checkbox(inf + id2, isInfinite))
			{
				if (EnableUndo)
				{
					binding.Infinite.SetValue(isInfinite[0]);
				}
				else
				{
					binding.Infinite.SetValueDirectly(isInfinite[0]);
				}
			}

			Popup();

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

			if (!binding.CanSelectDynamicEquation) return;

			if (Manager.NativeManager.BeginPopupContextItem(id_c))
			{
				DynamicSelector.Popup(id_c, binding.DynamicEquation, binding.IsDynamicEquationEnabled);

				Manager.NativeManager.EndPopup();

				isPopupShown = true;
			}
		}
	}
}