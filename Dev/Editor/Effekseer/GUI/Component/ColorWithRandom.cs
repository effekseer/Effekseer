using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Effekseer.GUI.Component
{
	public partial class ColorWithRandom : UserControl
	{
		public ColorWithRandom()
		{
			InitializeComponent();
			
			EnableUndo = true;

			this.SuspendLayout();
			Anchor = AnchorStyles.Left | AnchorStyles.Right;
			this.ResumeLayout(false);
			colorDialog.FullOpen = true;

			Reading = false;
			Writing = false;

			Reading = true;
			Read();
			Reading = false;

			HandleDestroyed += new EventHandler(_HandleDestroyed);
		}

		Data.Value.ColorWithRandom binding = null;

		public bool EnableUndo { get; set; }

		public Data.Value.ColorWithRandom Binding
		{
			get
			{
				return binding;
			}
			set
			{
				if (binding == value) return;

				if (binding != null)
				{
					binding.R.OnChanged -= OnChanged;
					binding.G.OnChanged -= OnChanged;
					binding.B.OnChanged -= OnChanged;
					binding.A.OnChanged -= OnChanged;
					binding.OnChangedColorSpace -= OnChangedColorSpace;
					tb_r_v1.ReadMethod = null;
					tb_r_v1.WriteMethod = null;
					tb_r_v2.ReadMethod = null;
					tb_r_v2.WriteMethod = null;
					tb_g_v1.ReadMethod = null;
					tb_g_v1.WriteMethod = null;
					tb_g_v2.ReadMethod = null;
					tb_g_v2.WriteMethod = null;
					tb_b_v1.ReadMethod = null;
					tb_b_v1.WriteMethod = null;
					tb_b_v2.ReadMethod = null;
					tb_b_v2.WriteMethod = null;
					tb_a_v1.ReadMethod = null;
					tb_a_v1.WriteMethod = null;
					tb_a_v2.ReadMethod = null;
					tb_a_v2.WriteMethod = null;
				}

				binding = value;

				if (binding != null)
				{
					binding.R.OnChanged += OnChanged;
					binding.G.OnChanged += OnChanged;
					binding.B.OnChanged += OnChanged;
					binding.A.OnChanged += OnChanged;
					binding.OnChangedColorSpace += OnChangedColorSpace;

					tb_r_v1.ReadMethod += () =>
					{
						if (binding.DrawnAs == Data.DrawnAs.MaxAndMin)
						{
							return binding.R.Max;
						}
						else if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
						{
							return binding.R.Center;
						}
						else
						{
							throw new Exception();
						}
					};

					tb_r_v1.WriteMethod += (f, wheel) =>
					{
						if (binding.DrawnAs == Data.DrawnAs.MaxAndMin)
						{
							binding.R.SetMax(f, wheel);
						}
						else if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
						{
							binding.R.SetCenter(f, wheel);
						}
						else
						{
							throw new Exception();
						}
					};

					tb_r_v2.ReadMethod += () =>
					{
						if (binding.DrawnAs == Data.DrawnAs.MaxAndMin)
						{
							return binding.R.Min;
						}
						else if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
						{
							return binding.R.Amplitude;
						}
						else
						{
							throw new Exception();
						}
					};

					tb_r_v2.WriteMethod += (f, wheel) =>
					{
						if (binding.DrawnAs == Data.DrawnAs.MaxAndMin)
						{
							binding.R.SetMin(f,wheel);
						}
						else if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
						{
							binding.R.SetAmplitude(f, wheel);
						}
						else
						{
							throw new Exception();
						}
					};

					tb_g_v1.ReadMethod += () =>
					{
						if (binding.DrawnAs == Data.DrawnAs.MaxAndMin)
						{
							return binding.G.Max;
						}
						else if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
						{
							return binding.G.Center;
						}
						else
						{
							throw new Exception();
						}
					};

					tb_g_v1.WriteMethod += (f, wheel) =>
					{
						if (binding.DrawnAs == Data.DrawnAs.MaxAndMin)
						{
							binding.G.SetMax(f, wheel);
						}
						else if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
						{
							binding.G.SetCenter(f, wheel);
						}
						else
						{
							throw new Exception();
						}
					};

					tb_g_v2.ReadMethod += () =>
					{
						if (binding.DrawnAs == Data.DrawnAs.MaxAndMin)
						{
							return binding.G.Min;
						}
						else if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
						{
							return binding.G.Amplitude;
						}
						else
						{
							throw new Exception();
						}
					};

					tb_g_v2.WriteMethod += (f, wheel) =>
					{
						if (binding.DrawnAs == Data.DrawnAs.MaxAndMin)
						{
							binding.G.SetMin(f, wheel);
						}
						else if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
						{
							binding.G.SetAmplitude(f, wheel);
						}
						else
						{
							throw new Exception();
						}
					};

					tb_b_v1.ReadMethod += () =>
					{
						if (binding.DrawnAs == Data.DrawnAs.MaxAndMin)
						{
							return binding.B.Max;
						}
						else if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
						{
							return binding.B.Center;
						}
						else
						{
							throw new Exception();
						}
					};

					tb_b_v1.WriteMethod += (f, wheel) =>
					{
						if (binding.DrawnAs == Data.DrawnAs.MaxAndMin)
						{
							binding.B.SetMax(f, wheel);
						}
						else if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
						{
							binding.B.SetCenter(f, wheel);
						}
						else
						{
							throw new Exception();
						}
					};

					tb_b_v2.ReadMethod += () =>
					{
						if (binding.DrawnAs == Data.DrawnAs.MaxAndMin)
						{
							return binding.B.Min;
						}
						else if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
						{
							return binding.B.Amplitude;
						}
						else
						{
							throw new Exception();
						}
					};

					tb_b_v2.WriteMethod += (f, wheel) =>
					{
						if (binding.DrawnAs == Data.DrawnAs.MaxAndMin)
						{
							binding.B.SetMin(f, wheel);
						}
						else if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
						{
							binding.B.SetAmplitude(f, wheel);
						}
						else
						{
							throw new Exception();
						}
					};

					tb_a_v1.ReadMethod += () =>
					{
						if (binding.DrawnAs == Data.DrawnAs.MaxAndMin)
						{
							return binding.A.Max;
						}
						else if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
						{
							return binding.A.Center;
						}
						else
						{
							throw new Exception();
						}
					};

					tb_a_v1.WriteMethod += (f, wheel) =>
					{
						if (binding.DrawnAs == Data.DrawnAs.MaxAndMin)
						{
							binding.A.SetMax(f, wheel);
						}
						else if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
						{
							binding.A.SetCenter(f, wheel);
						}
						else
						{
							throw new Exception();
						}
					};

					tb_a_v2.ReadMethod += () =>
					{
						if (binding.DrawnAs == Data.DrawnAs.MaxAndMin)
						{
							return binding.A.Min;
						}
						else if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
						{
							return binding.A.Amplitude;
						}
						else
						{
							throw new Exception();
						}
					};

					tb_a_v2.WriteMethod += (f, wheel) =>
					{
						if (binding.DrawnAs == Data.DrawnAs.MaxAndMin)
						{
							binding.A.SetMin(f, wheel);
						}
						else if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
						{
							binding.A.SetAmplitude(f, wheel);
						}
						else
						{
							throw new Exception();
						}
					};
				}

				Reading = true;
				Read();
				Reading = false;
			}
		}

		public void SetBinding(object o)
		{
			var o_ = o as Data.Value.ColorWithRandom;
			Binding = o_;
		}

		/// <summary>
		/// 他のクラスからデータ読み込み中
		/// </summary>
		public bool Reading
		{
			get;
			private set;
		}

		/// <summary>
		/// 他のクラスにデータ書き込み中
		/// </summary>
		public bool Writing
		{
			get;
			private set;
		}

		void Read()
		{
			if (!Reading) throw new Exception();

			if (binding != null)
			{
				drawnas_1.Enabled = true;
				drawnas_2.Enabled = true;
				drawnas_1.Checked = binding.DrawnAs == Data.DrawnAs.MaxAndMin;
				drawnas_2.Checked = binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude;
				colorSpace_1.Enabled = true;
				colorSpace_2.Enabled = true;
				colorSpace_1.Checked = binding.ColorSpace == Data.ColorSpace.RGBA;
				colorSpace_2.Checked = binding.ColorSpace == Data.ColorSpace.HSVA;

				if (drawnas_1.Checked)
				{
                    lb_v1.Text = Properties.Resources.Max;
                    lb_v2.Text = Properties.Resources.Min;
				}

				if (drawnas_2.Checked)
				{
                    lb_v1.Text = Properties.Resources.Mean;
                    lb_v2.Text = Properties.Resources.Deviation;
				}
				
				if (colorSpace_1.Checked)
				{
					lb_r.Text = "R";
					lb_g.Text = "G";
					lb_b.Text = "B";
					lb_a.Text = "A";
				}
				
				if (colorSpace_2.Checked)
				{
					lb_r.Text = "H";
					lb_g.Text = "S";
					lb_b.Text = "V";
					lb_a.Text = "A";
				}

				btn_color_min.Enabled = true;
				btn_color_max.Enabled = true;
				if (binding.ColorSpace == Data.ColorSpace.RGBA)
				{
					btn_color_min.BackColor = Color.FromArgb(binding.R.Min, binding.G.Min, binding.B.Min);
					btn_color_max.BackColor = Color.FromArgb(binding.R.Max, binding.G.Max, binding.B.Max);
				}
				else
				{
					btn_color_min.BackColor = HSVToRGB(Color.FromArgb(binding.R.Min, binding.G.Min, binding.B.Min));
					btn_color_max.BackColor = HSVToRGB(Color.FromArgb(binding.R.Max, binding.G.Max, binding.B.Max));
				}
			}
			else
			{
				drawnas_1.Enabled = false;
				drawnas_2.Enabled = false;
				drawnas_1.Checked = false;
				drawnas_2.Checked = false;
				colorSpace_1.Enabled = false;
				colorSpace_2.Enabled = false;
				colorSpace_1.Checked = false;
				colorSpace_2.Checked = false;

				lb_v1.Text = string.Empty;
				lb_v2.Text = string.Empty;

				btn_color_min.Enabled = false;
				btn_color_max.Enabled = false;
				btn_color_min.BackColor = Color.FromArgb(255, 255, 255);
				btn_color_max.BackColor = Color.FromArgb(255, 255, 255);
			}

			tb_r_v1.Reload();
			tb_r_v2.Reload();
			tb_g_v1.Reload();
			tb_g_v2.Reload();
			tb_b_v1.Reload();
			tb_b_v2.Reload();
			tb_a_v1.Reload();
			tb_a_v2.Reload();
		}

		void Write()
		{
			if (!Writing) throw new Exception();

			if (drawnas_1.Checked)
			{
				binding.DrawnAs = Data.DrawnAs.MaxAndMin;
			}
			if (drawnas_2.Checked)
			{
				binding.DrawnAs = Data.DrawnAs.CenterAndAmplitude;
			}
			if (colorSpace_1.Checked)
			{
				if (binding.ColorSpace != Data.ColorSpace.RGBA)
				{
					binding.ChangeColorSpace(Data.ColorSpace.RGBA, true);
				}
			}
			if (colorSpace_2.Checked)
			{
				if (binding.ColorSpace != Data.ColorSpace.HSVA)
				{
					binding.ChangeColorSpace(Data.ColorSpace.HSVA, true);
				}
			}
		}

		void OnChanged(object sender, ChangedValueEventArgs e)
		{
			if (Writing) return;

			Reading = true;
			Read();
			Reading = false;
		}
		
		void OnChangedColorSpace(object sender, ChangedValueEventArgs e)
		{
			bool changeColor = (bool)e.Value;

			if (changeColor)
			{
				if (binding.ColorSpace == Data.ColorSpace.RGBA)
				{
					Color max = HSVToRGB(Color.FromArgb(Binding.R.Max, Binding.G.Max, Binding.B.Max));
					Color min = HSVToRGB(Color.FromArgb(Binding.R.Min, Binding.G.Min, Binding.B.Min));
					Binding.SetMax(max.R, max.G, max.B);
					Binding.SetMin(min.R, min.G, min.B);
				}
				else
				{
					Color max = RGBToHSV(Color.FromArgb(Binding.R.Max, Binding.G.Max, Binding.B.Max));
					Color min = RGBToHSV(Color.FromArgb(Binding.R.Min, Binding.G.Min, Binding.B.Min));
					Binding.SetMax(max.R, max.G, max.B);
					Binding.SetMin(min.R, min.G, min.B);
				}
			}

			if (Writing) return;

			Reading = true;
			Read();
			Reading = false;
		}

		private void btn_color_min_Click(object sender, EventArgs e)
		{
			if (binding != null)
			{
				colorDialog.Color = Color.FromArgb(binding.R.Min, binding.G.Min, binding.B.Min);
				if (Binding.ColorSpace == Data.ColorSpace.HSVA)
				{
					colorDialog.Color = HSVToRGB(colorDialog.Color);
					colorDialog.ShowDialog();
					colorDialog.Color = RGBToHSV(colorDialog.Color);
				}
				else
				{
					colorDialog.ShowDialog();
				}
				binding.SetMin(colorDialog.Color.R, colorDialog.Color.G, colorDialog.Color.B);
			}
			else
			{

			}
		}

		private void btn_color_max_Click(object sender, EventArgs e)
		{
			if (binding != null)
			{
				colorDialog.Color = Color.FromArgb(binding.R.Max, binding.G.Max, binding.B.Max);
				if (Binding.ColorSpace == Data.ColorSpace.HSVA)
				{
					colorDialog.Color = HSVToRGB(colorDialog.Color);
					colorDialog.ShowDialog();
					colorDialog.Color = RGBToHSV(colorDialog.Color);
				}
				else
				{
					colorDialog.ShowDialog();
				}
				binding.SetMax(colorDialog.Color.R, colorDialog.Color.G, colorDialog.Color.B);
			}
			else
			{

			}
		}

		private void Update()
		{
			if (Reading) return;
			if (Writing) return;

			Writing = true;
			Write();
			Writing = false;

			Reading = true;
			Read();
			Reading = false;
		}

		private void drawnas_1_CheckedChanged(object sender, EventArgs e)
		{
			Update();
		}

		private void drawnas_2_CheckedChanged(object sender, EventArgs e)
		{
			Update();
		}

		private void colorSpace_1_CheckedChanged(object sender, EventArgs e)
		{
			Update();
		}

		private void colorSpace_2_CheckedChanged(object sender, EventArgs e)
		{
			Update();
		}

		void _HandleDestroyed(object sender, EventArgs e)
		{
			tb_r_v1.ReadMethod = null;
			tb_r_v1.WriteMethod = null;
			tb_r_v2.ReadMethod = null;
			tb_r_v2.WriteMethod = null;

			tb_g_v1.ReadMethod = null;
			tb_g_v1.WriteMethod = null;
			tb_g_v2.ReadMethod = null;
			tb_g_v2.WriteMethod = null;

			tb_b_v1.ReadMethod = null;
			tb_b_v1.WriteMethod = null;
			tb_b_v2.ReadMethod = null;
			tb_b_v2.WriteMethod = null;

			tb_a_v1.ReadMethod = null;
			tb_a_v1.WriteMethod = null;
			tb_a_v2.ReadMethod = null;
			tb_a_v2.WriteMethod = null;

			Binding = null;
		}

		static Color RGBToHSV(Color rgb)
		{
			float max;
			float min;
			float R, G, B, H, S, V;

			R = (float)rgb.R / 255;
			G = (float)rgb.G / 255;
			B = (float)rgb.B / 255;

			if (R >= G && R >= B) {
				max = R;
				min = (G < B) ? G : B;
			} else if (G >= R && G >= B) {
				max = G;
				min = (R < B) ? R : B;
			} else {
				max = B;
				min = (R < G) ? R : G;
			}
			if (R == G && G == B) {
				H = 0.0f;
			} else if (max == R) {
				H = 60 * (G - B) / (max - min);
			} else if (max == G){
				H = 60 * (B - R) / (max - min) + 120;
			} else {
				H = 60 * (R - G) / (max - min) + 240;
			}
			if (H < 0.0f) {
				H += 360.0f;
			}
			if (max == 0.0f) {
				S = 0.0f;
			} else {
				S = (max - min) / max;
			}
			V = max;
			return Color.FromArgb((int)(H / 360 * 252), (int)(S * 255), (int)(V * 255));
		}

		static Color HSVToRGB(Color hsv)
		{
			int H = hsv.R, S = hsv.G, V = hsv.B;
			int i, R = 0, G = 0, B = 0, f, p, q, t;
	
			i = H / 42 % 6;
			f = H % 42 * 6;
			p = (V * (256 -   S                   )) >> 8;
			q = (V * (256 - ((S * f        ) >> 8))) >> 8;
			t = (V * (256 - ((S * (252 - f)) >> 8))) >> 8;
			if (p < 0)   p = 0;
			if (p > 255) p = 255;
			if (q < 0)   q = 0;
			if (q > 255) q = 255;
			if (t < 0)   t = 0;
			if (t > 255) t = 255;

			switch (i) {
			case 0: R = V; G = t; B = p; break;
			case 1: R = q; G = V; B = p; break;
			case 2: R = p; G = V; B = t; break;
			case 3: R = p; G = q; B = V; break;
			case 4: R = t; G = p; B = V; break;
			case 5: R = V; G = p; B = q; break;
			}
			return Color.FromArgb(hsv.A, R, G, B);
		}
	}
}
