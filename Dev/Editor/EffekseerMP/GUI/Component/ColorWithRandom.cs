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

				if(binding != null)
				{
					binding.OnChangedColorSpace -= Binding_OnChangedColorSpace;
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
			
			Manager.NativeManager.PushItemWidth(Manager.NativeManager.GetColumnWidth() - 60);

			var ivmin = (float[])internalValueMin.Clone();
			if(binding.ColorSpace == Data.ColorSpace.HSVA)
			{
				convertHSV2RGB(ivmin);
			}

			if (Manager.NativeManager.ColorEdit4(id1, ivmin, swig.ColorEditFlags.NoOptions | colorSpace))
			{
				if (binding.ColorSpace == Data.ColorSpace.HSVA)
				{
					convertRGB2HSV(ivmin);
				}

				binding.SetMin(
					(int)(ivmin[0] * 255),
					(int)(ivmin[1] * 255),
					(int)(ivmin[2] * 255),
					(int)(ivmin[3] * 255),
					isActive);
			}


			var isActive_Current = Manager.NativeManager.IsItemActive();

			Popup();

			Manager.NativeManager.SameLine();
			Manager.NativeManager.Text(Resources.GetString("Min"));

			var ivmax = (float[])internalValueMax.Clone();
			if (binding.ColorSpace == Data.ColorSpace.HSVA)
			{
				convertHSV2RGB(ivmax);
			}

			if (Manager.NativeManager.ColorEdit4(id2, ivmax, swig.ColorEditFlags.NoOptions | colorSpace))
			{
				if (binding.ColorSpace == Data.ColorSpace.HSVA)
				{
					convertRGB2HSV(ivmax);
				}

				binding.SetMax(
					(int)(ivmax[0] * 255),
					(int)(ivmax[1] * 255),
					(int)(ivmax[2] * 255),
					(int)(ivmax[3] * 255),
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
					binding.ChangeColorSpace(Data.ColorSpace.RGBA, true);
				}

				Manager.NativeManager.SameLine();

				if (Manager.NativeManager.RadioButton(txt_r_r2 + id_r2, binding.ColorSpace == Data.ColorSpace.HSVA))
				{
					binding.ChangeColorSpace(Data.ColorSpace.HSVA, true);
				}

				Manager.NativeManager.EndPopup();
				isPopupShown = true;
			}

		}

		private void Binding_OnChangedColorSpace(object sender, ChangedValueEventArgs e)
		{
			bool changeColor = (bool)e.Value;

			if (changeColor)
			{
				if (binding.ColorSpace == Data.ColorSpace.RGBA)
				{
					convertHSV2RGB(internalValueMin);
					convertHSV2RGB(internalValueMax);
					FixValue();
				}
				else
				{
					convertRGB2HSV(internalValueMin);
					convertRGB2HSV(internalValueMax);
					FixValue();
				}
			}
		}

		void convertRGB2HSV(float[] values)
		{
			color c = new color();
			c.R = (int)(values[0] * 255);
			c.G = (int)(values[1] * 255);
			c.B = (int)(values[2] * 255);
			c = RGBToHSV(c);
			values[0] = c.R / 255.0f;
			values[1] = c.G / 255.0f;
			values[2] = c.B / 255.0f;
		}

		void convertHSV2RGB(float[] values)
		{
			color c = new color();
			c.R = (int)(values[0] * 255);
			c.G = (int)(values[1] * 255);
			c.B = (int)(values[2] * 255);
			c = HSVToRGB(c);
			values[0] = c.R / 255.0f;
			values[1] = c.G / 255.0f;
			values[2] = c.B / 255.0f;
		}

		struct color
		{
			public int R;
			public int G;
			public int B;
		}

		static color RGBToHSV(color rgb)
		{
			float max;
			float min;
			float R, G, B, H, S, V;

			R = (float)rgb.R / 255;
			G = (float)rgb.G / 255;
			B = (float)rgb.B / 255;

			if (R >= G && R >= B)
			{
				max = R;
				min = (G < B) ? G : B;
			}
			else if (G >= R && G >= B)
			{
				max = G;
				min = (R < B) ? R : B;
			}
			else
			{
				max = B;
				min = (R < G) ? R : G;
			}
			if (R == G && G == B)
			{
				H = 0.0f;
			}
			else if (max == R)
			{
				H = 60 * (G - B) / (max - min);
			}
			else if (max == G)
			{
				H = 60 * (B - R) / (max - min) + 120;
			}
			else
			{
				H = 60 * (R - G) / (max - min) + 240;
			}
			if (H < 0.0f)
			{
				H += 360.0f;
			}
			if (max == 0.0f)
			{
				S = 0.0f;
			}
			else
			{
				S = (max - min) / max;
			}
			V = max;

			color ret = new color();
			ret.R = (int)(H / 360 * 252);
			ret.G = (int)(S * 255);
			ret.B = (int)(V * 255);
			return ret;
		}

		static color HSVToRGB(color hsv)
		{
			int H = hsv.R, S = hsv.G, V = hsv.B;
			int i, R = 0, G = 0, B = 0, f, p, q, t;

			i = H / 42 % 6;
			f = H % 42 * 6;
			p = (V * (256 - S)) >> 8;
			q = (V * (256 - ((S * f) >> 8))) >> 8;
			t = (V * (256 - ((S * (252 - f)) >> 8))) >> 8;
			if (p < 0) p = 0;
			if (p > 255) p = 255;
			if (q < 0) q = 0;
			if (q > 255) q = 255;
			if (t < 0) t = 0;
			if (t > 255) t = 255;

			switch (i)
			{
				case 0: R = V; G = t; B = p; break;
				case 1: R = q; G = V; B = p; break;
				case 2: R = p; G = V; B = t; break;
				case 3: R = p; G = q; B = V; break;
				case 4: R = t; G = p; B = V; break;
				case 5: R = V; G = p; B = q; break;
			}

			color ret = new color();
			ret.R = R;
			ret.G = G;
			ret.B = B;
			return ret;
		}
	}
}
