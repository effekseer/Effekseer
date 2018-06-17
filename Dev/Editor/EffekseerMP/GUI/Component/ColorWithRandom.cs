using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Component
{
	class ColorWithRandom : Control, IParameterControl
	{
		string id1 = "";
		string id2 = "";
		string id_c = "";
		string id_r1 = "";
		string id_r2 = "";

		bool isPopupShown = false;

		public string Label { get; set; } = string.Empty;

		public string Description { get; set; } = string.Empty;

		Data.Value.ColorWithRandom binding = null;

		bool isActive = false;

		float[] internalValueMax = new float[] { 1.0f, 1.0f, 1.0f, 1.0f };
		float[] internalValueMin = new float[] { 1.0f, 1.0f, 1.0f, 1.0f };

		/// <summary>
		/// This parameter is unused.
		/// </summary>
		public bool EnableUndo { get; set; } = true;

		public Data.Value.ColorWithRandom Binding
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
					// Force to minmax
					binding.R.DrawnAs = Data.DrawnAs.MaxAndMin;
					binding.G.DrawnAs = Data.DrawnAs.MaxAndMin;
					binding.B.DrawnAs = Data.DrawnAs.MaxAndMin;
					binding.A.DrawnAs = Data.DrawnAs.MaxAndMin;

					internalValueMax[0] = binding.R.Max / 255.0f;
					internalValueMax[1] = binding.G.Max / 255.0f;
					internalValueMax[2] = binding.B.Max / 255.0f;
					internalValueMax[3] = binding.A.Max / 255.0f;

					internalValueMin[0] = binding.R.Min / 255.0f;
					internalValueMin[1] = binding.G.Min / 255.0f;
					internalValueMin[2] = binding.B.Min / 255.0f;
					internalValueMin[3] = binding.A.Min / 255.0f;
				}
			}
		}

		public ColorWithRandom(string label = null)
		{
			if (label != null)
			{
				Label = label;
			}

			id1 = "###" + Manager.GetUniqueID().ToString();
			id2 = "###" + Manager.GetUniqueID().ToString();
			id_c = "###" + Manager.GetUniqueID().ToString();
			id_r1 = "###" + Manager.GetUniqueID().ToString();
			id_r2 = "###" + Manager.GetUniqueID().ToString();
		}

		public void SetBinding(object o)
		{
			var o_ = o as Data.Value.ColorWithRandom;
			Binding = o_;
		}

		public void FixValue()
		{
			binding.SetMin(
				(int)(internalValueMin[0] * 255),
				(int)(internalValueMin[1] * 255),
				(int)(internalValueMin[2] * 255),
				(int)(internalValueMin[3] * 255),
				isActive);

			binding.SetMax(
			(int)(internalValueMax[0] * 255),
			(int)(internalValueMax[1] * 255),
			(int)(internalValueMax[2] * 255),
			(int)(internalValueMax[3] * 255),
			isActive);
		}

		public override void Update()
		{
			isPopupShown = false;

			if (binding != null)
			{
				internalValueMax[0] = binding.R.Max / 255.0f;
				internalValueMax[1] = binding.G.Max / 255.0f;
				internalValueMax[2] = binding.B.Max / 255.0f;
				internalValueMax[3] = binding.A.Max / 255.0f;

				internalValueMin[0] = binding.R.Min / 255.0f;
				internalValueMin[1] = binding.G.Min / 255.0f;
				internalValueMin[2] = binding.B.Min / 255.0f;
				internalValueMin[3] = binding.A.Min / 255.0f;
			}

			var colorSpace = binding.ColorSpace == Data.ColorSpace.RGBA ? swig.ColorEditFlags.RGB : swig.ColorEditFlags.HSV;

			Manager.NativeManager.PushItemWidth(150);

			if (Manager.NativeManager.ColorEdit4(id1, internalValueMin, swig.ColorEditFlags.NoOptions | colorSpace))
			{
				binding.SetMin(
					(int)(internalValueMin[0] * 255),
					(int)(internalValueMin[1] * 255),
					(int)(internalValueMin[2] * 255),
					(int)(internalValueMin[3] * 255),
					isActive);
			}


			var isActive_Current = Manager.NativeManager.IsItemActive();

			Popup();

			Manager.NativeManager.SameLine();
			Manager.NativeManager.Text(Resources.GetString("Min"));

			if (Manager.NativeManager.ColorEdit4(id2, internalValueMax, swig.ColorEditFlags.NoOptions | colorSpace))
			{
				binding.SetMax(
					(int)(internalValueMax[0] * 255),
					(int)(internalValueMax[1] * 255),
					(int)(internalValueMax[2] * 255),
					(int)(internalValueMax[3] * 255),
					isActive);
			}


			isActive_Current = isActive_Current || Manager.NativeManager.IsItemActive();

			if (isActive && !isActive_Current)
			{
				FixValue();
			}

			isActive = isActive_Current;

			Popup();

			Manager.NativeManager.SameLine();
			Manager.NativeManager.Text(Resources.GetString("Max"));

			Manager.NativeManager.PopItemWidth();
		}

		void Popup()
		{
			if (isPopupShown) return;

			if (Manager.NativeManager.BeginPopupContextItem(id_c))
			{
				var txt_r_r1 = "RGBA";
				var txt_r_r2 = "HSVA";

				if (Manager.NativeManager.RadioButton(txt_r_r1 + id_r1, binding.ColorSpace == Data.ColorSpace.RGBA))
				{
					binding.ColorSpace = Data.ColorSpace.RGBA;
				}

				Manager.NativeManager.SameLine();

				if (Manager.NativeManager.RadioButton(txt_r_r2 + id_r2, binding.ColorSpace == Data.ColorSpace.HSVA))
				{
					binding.ColorSpace = Data.ColorSpace.HSVA;
				}

				Manager.NativeManager.EndPopup();
				isPopupShown = true;
			}
		}
	}
}
