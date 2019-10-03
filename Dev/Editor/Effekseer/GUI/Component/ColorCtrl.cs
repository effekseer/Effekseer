using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Component
{
	class ColorCtrl : Control, IParameterControl
	{
		string id = "";

		public string Label { get; set; } = string.Empty;

		public string Description { get; set; } = string.Empty;

		Data.Value.Color binding = null;

		/// <summary>
		/// function. A border is shown when value is changed.
		/// </summary>
		ValueChangingProperty valueChangingProp = new ValueChangingProperty();

		float[] internalValue = new float[] { 1.0f, 1.0f, 1.0f, 1.0f };

		bool isActive = false;

		public bool EnableUndo { get; set; } = true;

		/// <summary>
		/// a parameter which is bound this instance
		/// </summary>
		public Data.Value.Color Binding
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
					internalValue[0] = binding.R / 255.0f;
					internalValue[1] = binding.G / 255.0f;
					internalValue[2] = binding.B / 255.0f;
					internalValue[3] = binding.A / 255.0f;
				}
			}
		}

		public ColorCtrl(string label = null)
		{
			if (label != null)
			{
				Label = label;
			}

			id = "###" + Manager.GetUniqueID().ToString();
		}

		public void SetBinding(object o)
		{
			var o_ = o as Data.Value.Color;
			Binding = o_;
		}

		public void FixValue()
		{
			if (binding == null) return;
			binding.SetValue(
				(int)(internalValue[0] * 255),
				(int)(internalValue[1] * 255),
				(int)(internalValue[2] * 255),
				(int)(internalValue[3] * 255),
				isActive);
		}

		public override void Update()
		{
			if (binding != null)
			{
				internalValue[0] = binding.R / 255.0f;
				internalValue[1] = binding.G / 255.0f;
				internalValue[2] = binding.B / 255.0f;
				internalValue[3] = binding.A / 255.0f;
			}

			valueChangingProp.Enable(binding);

			var colorSpace = binding.ColorSpace == Data.ColorSpace.RGBA ? swig.ColorEditFlags.RGB : swig.ColorEditFlags.HSV;

			if (Manager.NativeManager.ColorEdit4(id, internalValue, colorSpace))
			{
				if (EnableUndo)
				{
					FixValue();
				}
				else
				{
					binding.R.SetValueDirectly((int)(internalValue[0] * 255));
					binding.G.SetValueDirectly((int)(internalValue[1] * 255));
					binding.B.SetValueDirectly((int)(internalValue[2] * 255));
					binding.A.SetValueDirectly((int)(internalValue[3] * 255));
				}
			}

			valueChangingProp.Disable();

			var isActive_Current = Manager.NativeManager.IsItemActive();

			if (isActive && !isActive_Current)
			{
				FixValue();
			}

			isActive = isActive_Current;
		}
	}
}
