using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Component
{
	class Vector3DWithRandom : Control, IParameterControl
	{
		string id1 = "";
		string id2 = "";
		string id_r1 = "";
		string id_r2 = "";
		string id_c = "";

		bool isPopupShown = false;

		public string Label { get; set; } = string.Empty;

		public string Description { get; set; } = string.Empty;

		Data.Value.Vector3DWithRandom binding = null;

		float[] internalValue1 = new float[] { 0.0f, 0.0f, 0.0f };
		float[] internalValue2 = new float[] { 0.0f, 0.0f, 0.0f };

		public bool ShouldBeRemoved { get; private set; } = false;

		public bool EnableUndo { get; set; } = true;

		public Data.Value.Vector3DWithRandom Binding
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
						internalValue1[2] = binding.Z.Center;
						internalValue2[0] = binding.X.Amplitude;
						internalValue2[1] = binding.Y.Amplitude;
						internalValue2[2] = binding.Z.Amplitude;
					}
					else
					{
						internalValue1[0] = binding.X.Min;
						internalValue1[1] = binding.Y.Min;
						internalValue1[2] = binding.Z.Min;
						internalValue2[0] = binding.X.Max;
						internalValue2[1] = binding.Y.Max;
						internalValue2[2] = binding.Z.Max;
					}
				}
			}
		}

		public Vector3DWithRandom(string label = null)
		{
			if (label != null)
			{
				Label = label;
			}

			id1 = "###" + Manager.GetUniqueID().ToString();
			id2 = "###" + Manager.GetUniqueID().ToString();
			id_r1 = "###" + Manager.GetUniqueID().ToString();
			id_r2 = "###" + Manager.GetUniqueID().ToString();
			id_c = "###" + Manager.GetUniqueID().ToString();
		}

		public void SetBinding(object o)
		{
			var o_ = o as Data.Value.Vector3DWithRandom;
			Binding = o_;
		}

		public void FixValue()
		{
		}

		public override void OnDisposed()
		{
		}

		public override void Update()
		{
			if (binding == null) return;
			isPopupShown = false;

			if (binding != null)
			{
				if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
				{
					internalValue1[0] = binding.X.Center;
					internalValue1[1] = binding.Y.Center;
					internalValue1[2] = binding.Z.Center;
					internalValue2[0] = binding.X.Amplitude;
					internalValue2[1] = binding.Y.Amplitude;
					internalValue2[2] = binding.Z.Amplitude;
				}
				else
				{
					internalValue1[0] = binding.X.Min;
					internalValue1[1] = binding.Y.Min;
					internalValue1[2] = binding.Z.Min;
					internalValue2[0] = binding.X.Max;
					internalValue2[1] = binding.Y.Max;
					internalValue2[2] = binding.Z.Max;
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

			if (Manager.NativeManager.DragFloat3EfkEx(id1, internalValue1, 1, float.MinValue, float.MaxValue, txt_r1 + ":" + "%.3f", txt_r1 + ":" + "%.3f", txt_r1 + ":" + "%.3f"))
			{
				if (EnableUndo)
				{
					if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
					{
						binding.X.SetCenter(internalValue1[0]);
						binding.Y.SetCenter(internalValue1[1]);
						binding.Z.SetCenter(internalValue1[2]);
					}
					else
					{
						binding.X.SetMin(internalValue1[0]);
						binding.Y.SetMin(internalValue1[1]);
						binding.Z.SetMin(internalValue1[2]);
					}
				}
				else
				{
					Console.WriteLine("Not implemented.");
				}
			}

			Popup();

			if (Manager.NativeManager.DragFloat3EfkEx(id2, internalValue2, 1, float.MinValue, float.MaxValue, txt_r2 + ":" + "%.3f", txt_r2 + ":" + "%.3f", txt_r2 + ":" + "%.3f"))
			{
				if (EnableUndo)
				{
					if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
					{
						binding.X.SetAmplitude(internalValue2[0]);
						binding.Y.SetAmplitude(internalValue2[1]);
						binding.Z.SetAmplitude(internalValue2[2]);
					}
					else
					{
						binding.X.SetMax(internalValue2[0]);
						binding.Y.SetMax(internalValue2[1]);
						binding.Z.SetMax(internalValue2[2]);
					}
				}
				else
				{
					throw new Exception("Not Implemented.");
				}
			}

			Popup();
		}

		void Popup()
		{
			if (isPopupShown) return;

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
				isPopupShown = true;
			}
		}

	}
}
