using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Component
{
	class Vector3D : Control, IParameterControl
	{
		string id = "";
		string id_d = "";
		string id_c = "";
		string id_reset = "";

		bool isActive = false;

		bool isPopupShown = false;

		Data.Value.Vector3D binding = null;

		ValueChangingProperty valueChangingProp = new ValueChangingProperty();

		float[] internalValue = new float[] { 0.0f, 0.0f, 0.0f };

		public bool EnableUndo { get; set; } = true;

		public Data.Value.Vector3D Binding
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
					internalValue[0] = binding.X.Value;
					internalValue[1] = binding.Y.Value;
					internalValue[2] = binding.Z.Value;
				}
			}
		}

		public Vector3D()
		{
			var rand = new Random();
			id = "###" + Manager.GetUniqueID().ToString();
			id_d = "###" + Manager.GetUniqueID().ToString();
			id_c = "###" + Manager.GetUniqueID().ToString();
			id_reset = "###" + Manager.GetUniqueID().ToString();
		}

		public void SetBinding(object o)
		{
			var o_ = o as Data.Value.Vector3D;
			Binding = o_;
		}

		public void FixValue()
		{
			FixValueInternal(false);
		}

		void FixValueInternal(bool combined)
		{
			if (binding == null) return;

			if (EnableUndo)
			{
				binding.X.SetValue(internalValue[0], combined);
				binding.Y.SetValue(internalValue[1], combined);
				binding.Z.SetValue(internalValue[2], combined);
			}
			else
			{
				binding.X.SetValueDirectly(internalValue[0]);
				binding.Y.SetValueDirectly(internalValue[1]);
				binding.Z.SetValueDirectly(internalValue[2]);
			}
		}

		public override void OnDisposed()
		{
			FixValueInternal(false);
		}

		public override void Update()
		{
			isPopupShown = false;

			if (binding == null) return;

			valueChangingProp.Enable(binding);

			float step = 1.0f;

			if (binding != null)
			{
				internalValue[0] = binding.X.Value;
				internalValue[1] = binding.Y.Value;
				internalValue[2] = binding.Z.Value;

				step = Binding.X.Step / 10.0f;
			}

			if (Manager.NativeManager.DragFloat3EfkEx(id, internalValue, step,
				float.MinValue, float.MaxValue,
				float.MinValue, float.MaxValue,
				float.MinValue, float.MaxValue,
				"X:" + "%.3f", "Y:" + "%.3f", "Z:" + "%.3f"))
			{
				FixValueInternal(isActive);
			}

			var isActive_Current = Manager.NativeManager.IsItemActive();

			Popup();

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

			if (Manager.NativeManager.BeginPopupContextItem(id_c))
			{
				Functions.ShowReset(binding, id_reset);

				if (binding.CanSelectDynamicEquation)
				{
					DynamicSelector.Popup(id_c, binding.DynamicEquation, binding.IsDynamicEquationEnabled);
				}

				Manager.NativeManager.EndPopup();

				isPopupShown = true;
			}
		}
	}
}
