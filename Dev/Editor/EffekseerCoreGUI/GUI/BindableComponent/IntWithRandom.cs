using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.BindableComponent
{
	class IntWithRandom : Control, IParameterControl
	{
		string id = "";
		string id_r1 = "";
		string id_r2 = "";
		string id_c = "";
		string id_d1 = "";
		string id_d2 = "";
		string id_reset = "";

		Data.Value.IntWithRandom binding = null;

		ValueChangingProperty valueChangingProp = new ValueChangingProperty();

		bool isActive = false;

		bool isPopupShown = false;

		int[] internalValue = new int[] { 0, 0 };

		public bool EnableUndo { get; set; } = true;

		public Data.Value.IntWithRandom Binding
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
					internalValue[0] = binding.GetMin();
					internalValue[1] = binding.GetMax();
				}
			}
		}

		public IntWithRandom()
		{
			id = "###" + Manager.GetUniqueID().ToString();
			id_r1 = "###" + Manager.GetUniqueID().ToString();
			id_r2 = "###" + Manager.GetUniqueID().ToString();
			id_c = "###" + Manager.GetUniqueID().ToString();
			id_d1 = "###" + Manager.GetUniqueID().ToString();
			id_d2 = "###" + Manager.GetUniqueID().ToString();
			id_reset = "###" + Manager.GetUniqueID().ToString();
		}

		public void SetBinding(object o)
		{
			var o_ = o as Data.Value.IntWithRandom;
			Binding = o_;
		}

		public void FixValue()
		{
			FixValueInternal(isActive);
		}

		void FixValueInternal(bool combined)
		{
			if (binding == null) return;

			if (EnableUndo)
			{
				if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
				{
					binding.SetCenter(internalValue[0], combined);
					binding.SetAmplitude(internalValue[1], combined);
				}
				else
				{
					binding.SetMin(internalValue[0], combined);
					binding.SetMax(internalValue[1], combined);
				}
			}
			else
			{
				throw new Exception("Not Implemented.");
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

			if (binding != null)
			{
				if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
				{
					internalValue[0] = binding.GetCenter();
					internalValue[1] = binding.GetAmplitude();
				}
				else
				{
					internalValue[0] = binding.GetMin();
					internalValue[1] = binding.GetMax();
				}
			}

			valueChangingProp.Enable(binding);

			var txt_r1 = string.Empty;
			var txt_r2 = string.Empty;

			var range_1_min = int.MinValue;
			var range_1_max = int.MaxValue;
			var range_2_min = int.MinValue;
			var range_2_max = int.MaxValue;

			if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
			{
				txt_r1 = MultiLanguageTextProvider.GetText("Mean");
				txt_r2 = MultiLanguageTextProvider.GetText("Deviation");

				range_1_min = binding.ValueMin;
				range_1_max = binding.ValueMax;
			}
			else
			{
				txt_r1 = MultiLanguageTextProvider.GetText("Min");
				txt_r2 = MultiLanguageTextProvider.GetText("Max");

				range_1_min = binding.ValueMin;
				range_1_max = binding.ValueMax;
				range_2_min = binding.ValueMin;
				range_2_max = binding.ValueMax;
			}

			if (Manager.NativeManager.DragInt2EfkEx(id, internalValue, binding.Step,
				range_1_min, range_1_max,
				range_2_min, range_2_max,
				txt_r1 + ":" + "%d", txt_r2 + ":" + "%d"))
			{
				if (EnableUndo)
				{
					if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
					{
						binding.SetCenter(internalValue[0], isActive);
						binding.SetAmplitude(internalValue[1], isActive);
					}
					else
					{
						binding.SetMin(internalValue[0], isActive);
						binding.SetMax(internalValue[1], isActive);
					}
				}
				else
				{
					throw new Exception("Not Implemented.");
				}
			}

			Popup();


			if (binding.IsDynamicEquationEnabled)
			{
				DynamicSelector.SelectMaxInComponent(id_d1, binding.DynamicEquationMax);

				Popup();
			}

			if (binding.IsDynamicEquationEnabled)
			{
				DynamicSelector.SelectMinInComponent(id_d2, binding.DynamicEquationMin);

				Popup();
			}


			var isActive_Current = Manager.NativeManager.IsItemActive();

			if (isActive && !isActive_Current)
			{
				FixValue();
			}

			isActive = isActive_Current;

			Popup();

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
					DynamicSelector.Popup(id_c, binding.DynamicEquationMax, binding.DynamicEquationMin, binding.IsDynamicEquationEnabled);
				}

				if (binding.IsDynamicEquationEnabled)
				{
					// None
				}
				else
				{

					var txt_r_r1 = MultiLanguageTextProvider.GetText("Gauss");
					var txt_r_r2 = MultiLanguageTextProvider.GetText("Range");

					if (Manager.NativeManager.RadioButton(txt_r_r1 + id_r1, binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude))
					{
						binding.DrawnAs = Data.DrawnAs.CenterAndAmplitude;
					}

					Manager.NativeManager.SameLine();

					if (Manager.NativeManager.RadioButton(txt_r_r2 + id_r2, binding.DrawnAs == Data.DrawnAs.MaxAndMin))
					{
						binding.DrawnAs = Data.DrawnAs.MaxAndMin;
					}
				}

				Manager.NativeManager.EndPopup();

				isPopupShown = true;
			}
		}
	}
}