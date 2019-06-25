using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Component
{
	class Vector4D : Control, IParameterControl
	{
		string id = "";
		string id_c = "";


		public string Label { get; set; } = string.Empty;

		public string Description { get; set; } = string.Empty;

		bool isActive = false;

		bool isPopupShown = false;

		Data.Value.Vector4D binding = null;

		ValueChangingProperty valueChangingProp = new ValueChangingProperty();

		float[] internalValue = new float[] { 0.0f, 0.0f, 0.0f, 0.0f };

		public bool EnableUndo { get; set; } = true;

		public Data.Value.Vector4D Binding
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

		public Vector4D(string label = null)
		{
			if (label != null)
			{
				Label = label;
			}

			var rand = new Random();
			id = "###" + Manager.GetUniqueID().ToString();
			id_c = "###" + Manager.GetUniqueID().ToString();
		}

		public void SetBinding(object o)
		{
			var o_ = o as Data.Value.Vector4D;
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
				binding.W.SetValue(internalValue[3], combined);
			}
			else
			{
				binding.X.SetValueDirectly(internalValue[0]);
				binding.Y.SetValueDirectly(internalValue[1]);
				binding.Z.SetValueDirectly(internalValue[2]);
				binding.W.SetValueDirectly(internalValue[3]);
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
				internalValue[3] = binding.W.Value;

				step = Binding.X.Step / 10.0f;
			}

			if(Manager.NativeManager.ColorEdit4(id, internalValue, swig.ColorEditFlags.HDR | swig.ColorEditFlags.Float))
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
				Manager.NativeManager.Text(Resources.GetString("DynamicEq"));
				Manager.NativeManager.SameLine();

				var nextParam = DynamicSelector.Select("", "", binding.DynamicEquation, false, false);

				if (binding.DynamicEquation != nextParam)
				{
					binding.SetDynamicEquation(nextParam);
				}

				Popup();
			}

			valueChangingProp.Disable();
		}

		void Popup()
		{
			if (isPopupShown) return;

			if (Manager.NativeManager.BeginPopupContextItem(id_c))
			{
				if (Manager.NativeManager.RadioButton(Resources.GetString("DynamicFixed") + id_c + "_1", !binding.IsDynamicEquationEnabled))
				{
					binding.IsDynamicEquationEnabled = false;
					binding.SetDynamicEquation(null);
				}

				Manager.NativeManager.SameLine();

				if (Manager.NativeManager.RadioButton(Resources.GetString("DynamicDynamic") + id_c + "_2", binding.IsDynamicEquationEnabled))
				{
					binding.IsDynamicEquationEnabled = true;
				}

				Manager.NativeManager.EndPopup();

				isPopupShown = true;
			}
		}
	}
}
