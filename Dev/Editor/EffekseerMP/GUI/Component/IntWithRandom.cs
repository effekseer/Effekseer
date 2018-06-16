using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Component
{
	class IntWithRandom : Control, IParameterControl
	{
		string id = "";
		string id_r1 = "";
		string id_r2 = "";
		string id_c = "";

		public string Label { get; set; } = string.Empty;

		public string Description { get; set; } = string.Empty;

		Data.Value.IntWithRandom binding = null;

		bool isActive = false;

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

		public IntWithRandom(string label = null)
		{
			if (label != null)
			{
				Label = label;
			}

			id = "###" + Manager.GetUniqueID().ToString();
			id_r1 = "###" + Manager.GetUniqueID().ToString();
			id_r2 = "###" + Manager.GetUniqueID().ToString();
			id_c = "###" + Manager.GetUniqueID().ToString();
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

			var txt_r1 = string.Empty;
			var txt_r2 = string.Empty;

			if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
			{
				txt_r1 = Resources.GetString("Mean");
				txt_r2 = Resources.GetString("Deviation");
			}
			else
			{
				txt_r1 = Resources.GetString("Max");
				txt_r2 = Resources.GetString("Min");
			}

			if (Manager.NativeManager.DragInt2EfkEx(id, internalValue, 1, int.MinValue, int.MaxValue, txt_r1 + ":" + "%.3f", txt_r2 + ":" + "%.3f"))
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

			var isActive_Current = Manager.NativeManager.IsItemActive();

			if (isActive && !isActive_Current)
			{
				FixValue();
			}

			isActive = isActive_Current;

			if (Manager.NativeManager.BeginPopupContextItem(id_c))
			{
				var txt_r_r1 = Resources.GetString("Gauss");
				var txt_r_r2 = Resources.GetString("Range");

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
			}
		}
	}
}
