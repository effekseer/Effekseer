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

		/// <summary>
		/// a popup is shown in this frame.
		/// it prevent from showing a popup twice
		/// this flag is reset at the first of update
		/// </summary>
		bool isPopupShown = false;

		Data.Value.ColorWithRandom binding = null;

		/// <summary>
		/// function. A border is shown when value is changed.
		/// </summary>
		ValueChangingProperty valueChangingProp = new ValueChangingProperty();

		bool isActive = false;

		/// <summary>
		/// on writing.
		/// It prevents to read values while writing
		/// </summary>
		bool isWriting = false;

		float[] internalValueMax = new float[] { 1.0f, 1.0f, 1.0f, 1.0f };
		float[] internalValueMin = new float[] { 1.0f, 1.0f, 1.0f, 1.0f };

		public bool EnableUndo { get; set; } = true;

		/// <summary>
		/// a parameter which is bound this instance
		/// </summary>
		public Data.Value.ColorWithRandom Binding
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
					binding.OnChangedColorSpace -= Binding_OnChangedColorSpace;
					binding.R.OnChanged -= Binding_OnChanged;
					binding.G.OnChanged -= Binding_OnChanged;
					binding.B.OnChanged -= Binding_OnChanged;
					binding.A.OnChanged -= Binding_OnChanged;
				}

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

					binding.OnChangedColorSpace += Binding_OnChangedColorSpace;
					binding.R.OnChanged += Binding_OnChanged;
					binding.G.OnChanged += Binding_OnChanged;
					binding.B.OnChanged += Binding_OnChanged;
					binding.A.OnChanged += Binding_OnChanged;
				}
			}
		}


		public ColorWithRandom()
		{
			id1 = "###" + GUIManager.GetUniqueID().ToString();
			id2 = "###" + GUIManager.GetUniqueID().ToString();
			id_c = "###" + GUIManager.GetUniqueID().ToString();
			id_r1 = "###" + GUIManager.GetUniqueID().ToString();
			id_r2 = "###" + GUIManager.GetUniqueID().ToString();
		}

		public void SetBinding(object o)
		{
			var o_ = o as Data.Value.ColorWithRandom;
			Binding = o_;
		}

		public void FixValue()
		{
			binding.SetMin(
				(int)Math.Round(internalValueMin[0] * 255, MidpointRounding.AwayFromZero),
				(int)Math.Round(internalValueMin[1] * 255, MidpointRounding.AwayFromZero),
				(int)Math.Round(internalValueMin[2] * 255, MidpointRounding.AwayFromZero),
				(int)Math.Round(internalValueMin[3] * 255, MidpointRounding.AwayFromZero),
				isActive);

			binding.SetMax(
				(int)Math.Round(internalValueMax[0] * 255, MidpointRounding.AwayFromZero),
				(int)Math.Round(internalValueMax[1] * 255, MidpointRounding.AwayFromZero),
				(int)Math.Round(internalValueMax[2] * 255, MidpointRounding.AwayFromZero),
				(int)Math.Round(internalValueMax[3] * 255, MidpointRounding.AwayFromZero),
				isActive);
		}

		public override void Update()
		{
			if (binding == null) return;

			valueChangingProp.Enable(binding);

			isPopupShown = false;

			var colorSpace = binding.ColorSpace == Data.ColorSpace.RGBA ? swig.ColorEditFlags.RGB : swig.ColorEditFlags.HSV;

			GUIManager.NativeManager.PushItemWidth(GUIManager.NativeManager.GetColumnWidth() - 48 * GUIManager.DpiScale);

			if (GUIManager.NativeManager.ColorEdit4(id1, internalValueMin, swig.ColorEditFlags.NoOptions | colorSpace))
			{
				isWriting = true;

				FixValue();

				isWriting = false;
			}


			var isActive_Current = GUIManager.NativeManager.IsItemActive();

			Popup();

			GUIManager.NativeManager.SameLine();
			GUIManager.NativeManager.Text(Resources.GetString("Min"));

			if (GUIManager.NativeManager.ColorEdit4(id2, internalValueMax, swig.ColorEditFlags.NoOptions | colorSpace))
			{
				isWriting = true;

				FixValue();

				isWriting = false;
			}


			isActive_Current = isActive_Current || GUIManager.NativeManager.IsItemActive();

			if (isActive && !isActive_Current)
			{
				FixValue();
			}

			isActive = isActive_Current;

			Popup();

			GUIManager.NativeManager.SameLine();
			GUIManager.NativeManager.Text(Resources.GetString("Max"));

			GUIManager.NativeManager.PopItemWidth();

			valueChangingProp.Disable();
		}

		/// <summary>
		/// Show popup
		/// </summary>
		void Popup()
		{
			if (isPopupShown) return;

			if (GUIManager.NativeManager.BeginPopupContextItem(id_c))
			{
				var txt_r_r1 = "RGBA";
				var txt_r_r2 = "HSVA";

				if (GUIManager.NativeManager.RadioButton(txt_r_r1 + id_r1, binding.ColorSpace == Data.ColorSpace.RGBA))
				{
					binding.ChangeColorSpace(Data.ColorSpace.RGBA, true);
				}

				GUIManager.NativeManager.SameLine();

				if (GUIManager.NativeManager.RadioButton(txt_r_r2 + id_r2, binding.ColorSpace == Data.ColorSpace.HSVA))
				{
					binding.ChangeColorSpace(Data.ColorSpace.HSVA, true);
				}

				GUIManager.NativeManager.EndPopup();
				isPopupShown = true;
			}

		}

		/// <summary>
		/// Which is called when a color space is changed
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void Binding_OnChangedColorSpace(object sender, ChangedValueEventArgs e)
		{
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
		}

		/// <summary>
		/// Which is called when a color space is changed
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void Binding_OnChanged(object sender, ChangedValueEventArgs e)
		{
			if (isWriting) return;
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
		}
	}
}
