using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.BindableComponent
{
	class Vector2DWithRandom : Control, IParameterControl
	{
		string id1 = "";
		string id2 = "";
		string id_r1 = "";
		string id_r2 = "";
		string id_c = "";
		string id_reset = "";

		bool isPopupShown = false;

		bool isActive = false;

		Data.Value.Vector2DWithRandom binding = null;

		ValueChangingProperty valueChangingProp = new ValueChangingProperty();

		float[] internalValue1 = new float[] { 0.0f, 0.0f };
		float[] internalValue2 = new float[] { 0.0f, 0.0f };

		public bool EnableUndo { get; set; } = true;

		public Data.Value.Vector2DWithRandom Binding
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
					if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
					{
						internalValue1[0] = binding.X.Center;
						internalValue1[1] = binding.Y.Center;
						internalValue2[0] = binding.X.Amplitude;
						internalValue2[1] = binding.Y.Amplitude;
					}
					else
					{
						internalValue1[0] = binding.X.Min;
						internalValue1[1] = binding.Y.Min;
						internalValue2[0] = binding.X.Max;
						internalValue2[1] = binding.Y.Max;
					}
				}
			}
		}

		public Vector2DWithRandom()
		{
			id1 = "###" + Manager.GetUniqueID().ToString();
			id2 = "###" + Manager.GetUniqueID().ToString();
			id_r1 = "###" + Manager.GetUniqueID().ToString();
			id_r2 = "###" + Manager.GetUniqueID().ToString();
			id_c = "###" + Manager.GetUniqueID().ToString();
			id_reset = "###" + Manager.GetUniqueID().ToString();
		}

		public void SetBinding(object o)
		{
			var o_ = o as Data.Value.Vector2DWithRandom;
			Binding = o_;
		}

		public void FixValue()
		{
		}

		void FixValueInternal(bool combined)
		{
			if (EnableUndo)
			{
				if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
				{
					binding.X.SetCenter(internalValue1[0], combined);
					binding.Y.SetCenter(internalValue1[1], combined);
				}
				else
				{
					binding.X.SetMin(internalValue1[0], combined);
					binding.Y.SetMin(internalValue1[1], combined);
				}
			}
			else
			{
				throw new Exception("Not Implemented.");
			}

			if (EnableUndo)
			{
				if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
				{
					binding.X.SetAmplitude(internalValue2[0], combined);
					binding.Y.SetAmplitude(internalValue2[1], combined);
				}
				else
				{
					binding.X.SetMax(internalValue2[0], combined);
					binding.Y.SetMax(internalValue2[1], combined);
				}
			}
			else
			{
				throw new Exception("Not Implemented.");
			}
		}

		public override void OnDisposed()
		{
		}

		public override void Update()
		{
			if (binding == null) return;

			valueChangingProp.Enable(binding);

			isPopupShown = false;

			float step = 1.0f;

			if (binding != null)
			{
				if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
				{
					internalValue1[0] = binding.X.Center;
					internalValue1[1] = binding.Y.Center;
					internalValue2[0] = binding.X.Amplitude;
					internalValue2[1] = binding.Y.Amplitude;
				}
				else
				{
					internalValue1[0] = binding.X.Min;
					internalValue1[1] = binding.Y.Min;
					internalValue2[0] = binding.X.Max;
					internalValue2[1] = binding.Y.Max;
				}

				step = Binding.X.Step / 10.0f;
			}

			var txt_r1 = string.Empty;
			var txt_r2 = string.Empty;

			if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
			{
				txt_r1 = MultiLanguageTextProvider.GetText("Mean");
				txt_r2 = MultiLanguageTextProvider.GetText("Deviation");
			}
			else
			{
				txt_r1 = MultiLanguageTextProvider.GetText("Min");
				txt_r2 = MultiLanguageTextProvider.GetText("Max");
			}

			Manager.NativeManager.PushItemWidth(Manager.NativeManager.GetColumnWidth() - 48 * Manager.DpiScale);
			if (Manager.NativeManager.DragFloat2EfkEx(id1, internalValue1, step,
				float.MinValue, float.MaxValue,
				float.MinValue, float.MaxValue,
				"X:" + Core.Option.GetFloatFormat(), "Y:" + Core.Option.GetFloatFormat()))
			{
				if (EnableUndo)
				{
					if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
					{
						binding.X.SetCenter(internalValue1[0], isActive);
						binding.Y.SetCenter(internalValue1[1], isActive);
					}
					else
					{
						binding.X.SetMin(internalValue1[0], isActive);
						binding.Y.SetMin(internalValue1[1], isActive);
					}
				}
				else
				{
					throw new Exception("Not Implemented.");
				}
			}

			var isActive_Current = Manager.NativeManager.IsItemActive();

			Popup();

			Manager.NativeManager.SameLine();
			Manager.NativeManager.Text(txt_r1);

			if (Manager.NativeManager.DragFloat2EfkEx(id2, internalValue2, step,
				float.MinValue, float.MaxValue,
				float.MinValue, float.MaxValue,
				"X:" + Core.Option.GetFloatFormat(), "Y:" + Core.Option.GetFloatFormat()))
			{
				if (EnableUndo)
				{
					if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
					{
						binding.X.SetAmplitude(internalValue2[0], isActive);
						binding.Y.SetAmplitude(internalValue2[1], isActive);
					}
					else
					{
						binding.X.SetMax(internalValue2[0], isActive);
						binding.Y.SetMax(internalValue2[1], isActive);
					}
				}
				else
				{
					throw new Exception("Not Implemented.");
				}
			}

			isActive_Current |= Manager.NativeManager.IsItemActive();

			if (isActive && !isActive_Current)
			{
				FixValue();
			}

			isActive = isActive_Current;

			Popup();

			Manager.NativeManager.SameLine();
			Manager.NativeManager.Text(txt_r2);

			Manager.NativeManager.PopItemWidth();

			valueChangingProp.Disable();
		}

		void Popup()
		{
			if (isPopupShown) return;

			if (Manager.NativeManager.BeginPopupContextItem(id_c))
			{
				Functions.ShowReset(binding, id_reset);

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

				Manager.NativeManager.EndPopup();
				isPopupShown = true;
			}
		}

	}
}