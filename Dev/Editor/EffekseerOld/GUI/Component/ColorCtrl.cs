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
	public partial class ColorCtrl : UserControl
	{
		public ColorCtrl()
		{
			InitializeComponent();

			EnableUndo = true;
			
			this.SuspendLayout();
			Anchor = AnchorStyles.Left | AnchorStyles.Right;
			this.ResumeLayout(false);
			colorDialog.FullOpen = true;

			HandleDestroyed += new EventHandler(ColorCtrl_HandleDestroyed);
		}

		Data.Value.Color binding = null;

		public bool EnableUndo { get; set; }

		public Data.Value.Color Binding
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
					tb_r.Binding = null;
					tb_g.Binding = null;
					tb_b.Binding = null;
					tb_a.Binding = null;

					binding.R.OnChanged -= OnChanged;
					binding.G.OnChanged -= OnChanged;
					binding.B.OnChanged -= OnChanged;
					binding.A.OnChanged -= OnChanged;
				}

				binding = value;

				if (binding != null)
				{
					tb_r.Binding = binding.R;
					tb_g.Binding = binding.G;
					tb_b.Binding = binding.B;
					tb_a.Binding = binding.A;

					binding.R.OnChanged += OnChanged;
					binding.G.OnChanged += OnChanged;
					binding.B.OnChanged += OnChanged;
					binding.A.OnChanged += OnChanged;
				}

				Read();
			}
		}

		public void SetBinding(object o)
		{
			var o_ = o as Data.Value.Color;
			Binding = o_;
		}

		void Read()
		{
			if (binding != null)
			{
				btn_color.BackColor = Color.FromArgb(binding.R, binding.G, binding.B);
			}
			else
			{
				btn_color.BackColor = Color.FromArgb(255, 255, 255);
			}
		}

		void OnChanged(object sender, ChangedValueEventArgs e)
		{
			Read();
		}

		private void btn_color_Click(object sender, EventArgs e)
		{
			if (binding != null)
			{
				colorDialog.Color = Color.FromArgb(binding.R, binding.G, binding.B);
				colorDialog.ShowDialog();
				binding.SetValue(colorDialog.Color.R, colorDialog.Color.G, colorDialog.Color.B);
			}
			else
			{ 
			
			}
		}

		void ColorCtrl_HandleDestroyed(object sender, EventArgs e)
		{
			Binding = null;
		}
	}
}
