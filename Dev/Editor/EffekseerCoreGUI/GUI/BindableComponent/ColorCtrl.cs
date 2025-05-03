using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.BindableComponent
{
	class ColorCtrl : Control, IParameterControl
	{
		string id = "";
		string id_c = "";
		string id_reset = "";

		Data.Value.Color binding = null;

		bool isPopupShown = false;

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

		public ColorCtrl()
		{
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
				(int)Math.Round(internalValue[0] * 255, MidpointRounding.AwayFromZero),
				(int)Math.Round(internalValue[1] * 255, MidpointRounding.AwayFromZero),
				(int)Math.Round(internalValue[2] * 255, MidpointRounding.AwayFromZero),
				(int)Math.Round(internalValue[3] * 255, MidpointRounding.AwayFromZero),
				isActive);
		}

		public unsafe override void Update()
		{
			isPopupShown = false;

			if (binding != null)
			{
				StoreIfDifferentColor(ref internalValue[0], binding.R);
				StoreIfDifferentColor(ref internalValue[1], binding.G);
				StoreIfDifferentColor(ref internalValue[2], binding.B);
				StoreIfDifferentColor(ref internalValue[3], binding.A);
			}

			valueChangingProp.Enable(binding);

			var colorSpace = (binding.ColorSpace == Data.ColorSpace.RGBA)
				? (swig.ColorEditFlags.DisplayRGB | swig.ColorEditFlags.InputRGB)
				: (swig.ColorEditFlags.DisplayHSV | swig.ColorEditFlags.InputHSV);

			if (Manager.NativeManager.ColorEdit4(id, internalValue, swig.ColorEditFlags.NoOptions | colorSpace))
			{
				if (EnableUndo)
				{
					FixValue();
				}
				else
				{
					binding.R.SetValueDirectly((int)Math.Round(internalValue[0] * 255, MidpointRounding.AwayFromZero));
					binding.G.SetValueDirectly((int)Math.Round(internalValue[1] * 255, MidpointRounding.AwayFromZero));
					binding.B.SetValueDirectly((int)Math.Round(internalValue[2] * 255, MidpointRounding.AwayFromZero));
					binding.A.SetValueDirectly((int)Math.Round(internalValue[3] * 255, MidpointRounding.AwayFromZero));
				}
			}

			valueChangingProp.Disable();

			Popup();

			var isActive_Current = Manager.NativeManager.IsItemActive();

			if (isActive && !isActive_Current)
			{
				FixValue();
			}

			isActive = isActive_Current;
		}

		void StoreIfDifferentColor(ref float dst, int src)
		{
			var v = Math.Round(dst * 255.0f, MidpointRounding.AwayFromZero);
			if (v == src)
			{
				return;
			}

			dst = src / 255.0f;
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