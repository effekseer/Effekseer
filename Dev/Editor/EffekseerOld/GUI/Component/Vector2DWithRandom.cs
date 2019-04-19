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
	public partial class Vector2DWithRandom : UserControl
	{
		public Vector2DWithRandom()
		{
			InitializeComponent();

			EnableUndo = true;

			this.SuspendLayout();
			Anchor = AnchorStyles.Left | AnchorStyles.Right;
			this.ResumeLayout(false);

			Reading = false;
			Writing = false;

			Reading = true;
			Read();
			Reading = false;

			HandleDestroyed += new EventHandler(_HandleDestroyed);
		}

		Data.Value.Vector2DWithRandom binding = null;

		public bool EnableUndo { get; set; }

		public Data.Value.Vector2DWithRandom Binding
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
					binding.X.OnChanged -= OnChanged;
					binding.Y.OnChanged -= OnChanged;
					tb_x_v1.ReadMethod = null;
					tb_x_v1.WriteMethod = null;
					tb_x_v2.ReadMethod = null;
					tb_x_v2.WriteMethod = null;
					tb_y_v1.ReadMethod = null;
					tb_y_v1.WriteMethod = null;
					tb_y_v2.ReadMethod = null;
					tb_y_v2.WriteMethod = null;
				}

				binding = value;

				if (binding != null)
				{
					binding.X.OnChanged += OnChanged;
					binding.Y.OnChanged += OnChanged;
					tb_x_v1.WheelStep = binding.X.Step;
					tb_x_v2.WheelStep = binding.X.Step;
					tb_y_v1.WheelStep = binding.Y.Step;
					tb_y_v2.WheelStep = binding.Y.Step;

					tb_x_v1.ReadMethod += () =>
					{
						if (binding.DrawnAs == Data.DrawnAs.MaxAndMin)
						{
							return binding.X.Max;
						}
						else if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
						{
							return binding.X.Center;
						}
						else
						{
							throw new Exception();
						}
					};

					tb_x_v1.WriteMethod += (f, wheel) =>
					{
						if (binding.DrawnAs == Data.DrawnAs.MaxAndMin)
						{
							binding.X.SetMax(f, wheel);
						}
						else if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
						{
							binding.X.SetCenter(f, wheel);
						}
						else
						{
							throw new Exception();
						}
					};

					tb_x_v2.ReadMethod += () =>
					{
						if (binding.DrawnAs == Data.DrawnAs.MaxAndMin)
						{
							return binding.X.Min;
						}
						else if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
						{
							return binding.X.Amplitude;
						}
						else
						{
							throw new Exception();
						}
					};

					tb_x_v2.WriteMethod += (f, wheel) =>
					{
						if (binding.DrawnAs == Data.DrawnAs.MaxAndMin)
						{
							binding.X.SetMin(f, wheel);
						}
						else if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
						{
							binding.X.SetAmplitude(f, wheel);
						}
						else
						{
							throw new Exception();
						}
					};

					tb_y_v1.ReadMethod += () =>
					{
						if (binding.DrawnAs == Data.DrawnAs.MaxAndMin)
						{
							return binding.Y.Max;
						}
						else if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
						{
							return binding.Y.Center;
						}
						else
						{
							throw new Exception();
						}
					};

					tb_y_v1.WriteMethod += (f, wheel) =>
					{
						if (binding.DrawnAs == Data.DrawnAs.MaxAndMin)
						{
							binding.Y.SetMax(f, wheel);
						}
						else if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
						{
							binding.Y.SetCenter(f, wheel);
						}
						else
						{
							throw new Exception();
						}
					};

					tb_y_v2.ReadMethod += () =>
					{
						if (binding.DrawnAs == Data.DrawnAs.MaxAndMin)
						{
							return binding.Y.Min;
						}
						else if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
						{
							return binding.Y.Amplitude;
						}
						else
						{
							throw new Exception();
						}
					};

					tb_y_v2.WriteMethod += (f, wheel) =>
					{
						if (binding.DrawnAs == Data.DrawnAs.MaxAndMin)
						{
							binding.Y.SetMin(f, wheel);
						}
						else if (binding.DrawnAs == Data.DrawnAs.CenterAndAmplitude)
						{
							binding.Y.SetAmplitude(f, wheel);
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
			var o_ = o as Data.Value.Vector2DWithRandom;
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
			}
			else
			{
				drawnas_1.Enabled = false;
				drawnas_2.Enabled = false;
				drawnas_1.Checked = false;
				drawnas_2.Checked = false;

				lb_v1.Text = string.Empty;
				lb_v2.Text = string.Empty;
			}

			tb_x_v1.Reload();
			tb_x_v2.Reload();
			tb_y_v1.Reload();
			tb_y_v2.Reload();
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
		}

		void OnChanged(object sender, ChangedValueEventArgs e)
		{
			if (Writing) return;

			Reading = true;
			Read();
			Reading = false;
		}

		private void drawnas_1_CheckedChanged(object sender, EventArgs e)
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

		private void drawnas_2_CheckedChanged(object sender, EventArgs e)
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

		void _HandleDestroyed(object sender, EventArgs e)
		{
			tb_x_v1.ReadMethod = null;
			tb_x_v1.WriteMethod = null;
			tb_x_v2.ReadMethod = null;
			tb_x_v2.WriteMethod = null;

			tb_y_v1.ReadMethod = null;
			tb_y_v1.WriteMethod = null;
			tb_y_v2.ReadMethod = null;
			tb_y_v2.WriteMethod = null;

			Binding = null;
		}
	}
}
