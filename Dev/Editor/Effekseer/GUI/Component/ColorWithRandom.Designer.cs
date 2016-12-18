namespace Effekseer.GUI.Component
{
	partial class ColorWithRandom
	{
		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary> 
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Component Designer generated code

		private void InitializeComponent()
		{
			this.btn_color_min = new System.Windows.Forms.Button();
			this.lb_r = new System.Windows.Forms.Label();
			this.lb_g = new System.Windows.Forms.Label();
			this.btn_color_max = new System.Windows.Forms.Button();
			this.lb_v1 = new System.Windows.Forms.Label();
			this.lb_v2 = new System.Windows.Forms.Label();
			this.lb_b = new System.Windows.Forms.Label();
			this.lb_a = new System.Windows.Forms.Label();
			this.colorDialog = new System.Windows.Forms.ColorDialog();
			this.panel1 = new System.Windows.Forms.Panel();
			this.drawnas_2 = new System.Windows.Forms.RadioButton();
			this.drawnas_1 = new System.Windows.Forms.RadioButton();
			this.panel2 = new System.Windows.Forms.Panel();
			this.colorSpace_2 = new System.Windows.Forms.RadioButton();
			this.colorSpace_1 = new System.Windows.Forms.RadioButton();
			this.tb_a_v2 = new Effekseer.GUI.Component.DelegateIntTextBox();
			this.tb_b_v2 = new Effekseer.GUI.Component.DelegateIntTextBox();
			this.tb_g_v2 = new Effekseer.GUI.Component.DelegateIntTextBox();
			this.tb_r_v2 = new Effekseer.GUI.Component.DelegateIntTextBox();
			this.tb_a_v1 = new Effekseer.GUI.Component.DelegateIntTextBox();
			this.tb_b_v1 = new Effekseer.GUI.Component.DelegateIntTextBox();
			this.tb_g_v1 = new Effekseer.GUI.Component.DelegateIntTextBox();
			this.tb_r_v1 = new Effekseer.GUI.Component.DelegateIntTextBox();
			this.panel1.SuspendLayout();
			this.panel2.SuspendLayout();
			this.SuspendLayout();
			// 
			// btn_color_min
			// 
			this.btn_color_min.Location = new System.Drawing.Point(144, 68);
			this.btn_color_min.Name = "btn_color_min";
			this.btn_color_min.Size = new System.Drawing.Size(21, 20);
			this.btn_color_min.TabIndex = 1;
			this.btn_color_min.UseVisualStyleBackColor = true;
			this.btn_color_min.Click += new System.EventHandler(this.btn_color_min_Click);
			// 
			// lb_r
			// 
			this.lb_r.AutoSize = true;
			this.lb_r.Font = new System.Drawing.Font("MS UI Gothic", 8F);
			this.lb_r.Location = new System.Drawing.Point(36, 38);
			this.lb_r.Name = "lb_r";
			this.lb_r.Size = new System.Drawing.Size(12, 11);
			this.lb_r.TabIndex = 2;
			this.lb_r.Text = "R";
			// 
			// lb_g
			// 
			this.lb_g.AutoSize = true;
			this.lb_g.Font = new System.Drawing.Font("MS UI Gothic", 8F);
			this.lb_g.Location = new System.Drawing.Point(65, 38);
			this.lb_g.Name = "lb_g";
			this.lb_g.Size = new System.Drawing.Size(12, 11);
			this.lb_g.TabIndex = 6;
			this.lb_g.Text = "G";
			// 
			// btn_color_max
			// 
			this.btn_color_max.Location = new System.Drawing.Point(144, 47);
			this.btn_color_max.Name = "btn_color_max";
			this.btn_color_max.Size = new System.Drawing.Size(21, 20);
			this.btn_color_max.TabIndex = 0;
			this.btn_color_max.UseVisualStyleBackColor = true;
			this.btn_color_max.Click += new System.EventHandler(this.btn_color_max_Click);
			// 
			// lb_v1
			// 
			this.lb_v1.AutoSize = true;
			this.lb_v1.Font = new System.Drawing.Font("MS UI Gothic", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.lb_v1.Location = new System.Drawing.Point(1, 52);
			this.lb_v1.Name = "lb_v1";
			this.lb_v1.Size = new System.Drawing.Size(10, 11);
			this.lb_v1.TabIndex = 12;
			this.lb_v1.Text = "a";
			// 
			// lb_v2
			// 
			this.lb_v2.AutoSize = true;
			this.lb_v2.Font = new System.Drawing.Font("MS UI Gothic", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.lb_v2.Location = new System.Drawing.Point(1, 73);
			this.lb_v2.Name = "lb_v2";
			this.lb_v2.Size = new System.Drawing.Size(10, 11);
			this.lb_v2.TabIndex = 13;
			this.lb_v2.Text = "a";
			// 
			// lb_b
			// 
			this.lb_b.AutoSize = true;
			this.lb_b.Font = new System.Drawing.Font("MS UI Gothic", 8F);
			this.lb_b.Location = new System.Drawing.Point(94, 38);
			this.lb_b.Name = "lb_b";
			this.lb_b.Size = new System.Drawing.Size(12, 11);
			this.lb_b.TabIndex = 14;
			this.lb_b.Text = "B";
			// 
			// lb_a
			// 
			this.lb_a.AutoSize = true;
			this.lb_a.Font = new System.Drawing.Font("MS UI Gothic", 8F);
			this.lb_a.Location = new System.Drawing.Point(123, 38);
			this.lb_a.Name = "lb_a";
			this.lb_a.Size = new System.Drawing.Size(12, 11);
			this.lb_a.TabIndex = 15;
			this.lb_a.Text = "A";
			// 
			// panel1
			// 
			this.panel1.Controls.Add(this.drawnas_2);
			this.panel1.Controls.Add(this.drawnas_1);
			this.panel1.Location = new System.Drawing.Point(4, 18);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(161, 18);
			this.panel1.TabIndex = 18;
			// 
			// drawnas_2
			// 
			this.drawnas_2.AutoSize = true;
			this.drawnas_2.Font = new System.Drawing.Font("MS UI Gothic", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.drawnas_2.Location = new System.Drawing.Point(87, 1);
			this.drawnas_2.Name = "drawnas_2";
			this.drawnas_2.Size = new System.Drawing.Size(74, 15);
			this.drawnas_2.TabIndex = 12;
			this.drawnas_2.TabStop = true;
			this.drawnas_2.Text = Properties.Resources.Gauss;
			this.drawnas_2.UseVisualStyleBackColor = true;
			this.drawnas_2.CheckedChanged += new System.EventHandler(this.drawnas_2_CheckedChanged);
			// 
			// drawnas_1
			// 
			this.drawnas_1.AutoSize = true;
			this.drawnas_1.Font = new System.Drawing.Font("MS UI Gothic", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.drawnas_1.Location = new System.Drawing.Point(1, 1);
			this.drawnas_1.Name = "drawnas_1";
			this.drawnas_1.Size = new System.Drawing.Size(74, 15);
			this.drawnas_1.TabIndex = 11;
			this.drawnas_1.TabStop = true;
            this.drawnas_1.Text = Properties.Resources.Range;
			this.drawnas_1.UseVisualStyleBackColor = true;
			this.drawnas_1.CheckedChanged += new System.EventHandler(this.drawnas_1_CheckedChanged);
			// 
			// panel2
			// 
			this.panel2.Controls.Add(this.colorSpace_2);
			this.panel2.Controls.Add(this.colorSpace_1);
			this.panel2.Location = new System.Drawing.Point(4, -1);
			this.panel2.Name = "panel2";
			this.panel2.Size = new System.Drawing.Size(161, 18);
			this.panel2.TabIndex = 19;
			// 
			// colorSpace_2
			// 
			this.colorSpace_2.AutoSize = true;
			this.colorSpace_2.Font = new System.Drawing.Font("MS UI Gothic", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.colorSpace_2.Location = new System.Drawing.Point(87, 1);
			this.colorSpace_2.Name = "colorSpace_2";
			this.colorSpace_2.Size = new System.Drawing.Size(51, 15);
			this.colorSpace_2.TabIndex = 19;
			this.colorSpace_2.TabStop = true;
			this.colorSpace_2.Text = "HSVA";
			this.colorSpace_2.UseVisualStyleBackColor = true;
			this.colorSpace_2.CheckedChanged += new System.EventHandler(this.colorSpace_2_CheckedChanged);
			// 
			// colorSpace_1
			// 
			this.colorSpace_1.AutoSize = true;
			this.colorSpace_1.Font = new System.Drawing.Font("MS UI Gothic", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.colorSpace_1.Location = new System.Drawing.Point(1, 1);
			this.colorSpace_1.Name = "colorSpace_1";
			this.colorSpace_1.Size = new System.Drawing.Size(51, 15);
			this.colorSpace_1.TabIndex = 18;
			this.colorSpace_1.TabStop = true;
			this.colorSpace_1.Text = "RGBA";
			this.colorSpace_1.UseVisualStyleBackColor = true;
			this.colorSpace_1.CheckedChanged += new System.EventHandler(this.colorSpace_1_CheckedChanged);
			// 
			// tb_a_v2
			// 
			this.tb_a_v2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(200)))), ((int)(((byte)(200)))));
			this.tb_a_v2.Enabled = false;
			this.tb_a_v2.IsEnable = null;
			this.tb_a_v2.Location = new System.Drawing.Point(113, 69);
			this.tb_a_v2.Margin = new System.Windows.Forms.Padding(0);
			this.tb_a_v2.MaxLength = 8;
			this.tb_a_v2.Name = "tb_a_v2";
			this.tb_a_v2.ReadMethod = null;
			this.tb_a_v2.Size = new System.Drawing.Size(30, 19);
			this.tb_a_v2.TabIndex = 9;
			this.tb_a_v2.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			this.tb_a_v2.WriteMethod = null;
			// 
			// tb_b_v2
			// 
			this.tb_b_v2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(200)))), ((int)(((byte)(200)))));
			this.tb_b_v2.Enabled = false;
			this.tb_b_v2.IsEnable = null;
			this.tb_b_v2.Location = new System.Drawing.Point(83, 69);
			this.tb_b_v2.Margin = new System.Windows.Forms.Padding(0);
			this.tb_b_v2.MaxLength = 8;
			this.tb_b_v2.Name = "tb_b_v2";
			this.tb_b_v2.ReadMethod = null;
			this.tb_b_v2.Size = new System.Drawing.Size(30, 19);
			this.tb_b_v2.TabIndex = 8;
			this.tb_b_v2.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			this.tb_b_v2.WriteMethod = null;
			// 
			// tb_g_v2
			// 
			this.tb_g_v2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(200)))), ((int)(((byte)(200)))));
			this.tb_g_v2.Enabled = false;
			this.tb_g_v2.IsEnable = null;
			this.tb_g_v2.Location = new System.Drawing.Point(54, 69);
			this.tb_g_v2.Margin = new System.Windows.Forms.Padding(0);
			this.tb_g_v2.MaxLength = 8;
			this.tb_g_v2.Name = "tb_g_v2";
			this.tb_g_v2.ReadMethod = null;
			this.tb_g_v2.Size = new System.Drawing.Size(30, 19);
			this.tb_g_v2.TabIndex = 7;
			this.tb_g_v2.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			this.tb_g_v2.WriteMethod = null;
			// 
			// tb_r_v2
			// 
			this.tb_r_v2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(200)))), ((int)(((byte)(200)))));
			this.tb_r_v2.Enabled = false;
			this.tb_r_v2.IsEnable = null;
			this.tb_r_v2.Location = new System.Drawing.Point(25, 69);
			this.tb_r_v2.Margin = new System.Windows.Forms.Padding(0);
			this.tb_r_v2.MaxLength = 8;
			this.tb_r_v2.Name = "tb_r_v2";
			this.tb_r_v2.ReadMethod = null;
			this.tb_r_v2.Size = new System.Drawing.Size(30, 19);
			this.tb_r_v2.TabIndex = 6;
			this.tb_r_v2.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			this.tb_r_v2.WriteMethod = null;
			// 
			// tb_a_v1
			// 
			this.tb_a_v1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(200)))), ((int)(((byte)(200)))));
			this.tb_a_v1.Enabled = false;
			this.tb_a_v1.IsEnable = null;
			this.tb_a_v1.Location = new System.Drawing.Point(113, 48);
			this.tb_a_v1.Margin = new System.Windows.Forms.Padding(0);
			this.tb_a_v1.MaxLength = 8;
			this.tb_a_v1.Name = "tb_a_v1";
			this.tb_a_v1.ReadMethod = null;
			this.tb_a_v1.Size = new System.Drawing.Size(30, 19);
			this.tb_a_v1.TabIndex = 5;
			this.tb_a_v1.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			this.tb_a_v1.WriteMethod = null;
			// 
			// tb_b_v1
			// 
			this.tb_b_v1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(200)))), ((int)(((byte)(200)))));
			this.tb_b_v1.Enabled = false;
			this.tb_b_v1.IsEnable = null;
			this.tb_b_v1.Location = new System.Drawing.Point(83, 48);
			this.tb_b_v1.Margin = new System.Windows.Forms.Padding(0);
			this.tb_b_v1.MaxLength = 8;
			this.tb_b_v1.Name = "tb_b_v1";
			this.tb_b_v1.ReadMethod = null;
			this.tb_b_v1.Size = new System.Drawing.Size(30, 19);
			this.tb_b_v1.TabIndex = 4;
			this.tb_b_v1.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			this.tb_b_v1.WriteMethod = null;
			// 
			// tb_g_v1
			// 
			this.tb_g_v1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(200)))), ((int)(((byte)(200)))));
			this.tb_g_v1.Enabled = false;
			this.tb_g_v1.IsEnable = null;
			this.tb_g_v1.Location = new System.Drawing.Point(54, 48);
			this.tb_g_v1.Margin = new System.Windows.Forms.Padding(0);
			this.tb_g_v1.MaxLength = 8;
			this.tb_g_v1.Name = "tb_g_v1";
			this.tb_g_v1.ReadMethod = null;
			this.tb_g_v1.Size = new System.Drawing.Size(30, 19);
			this.tb_g_v1.TabIndex = 3;
			this.tb_g_v1.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			this.tb_g_v1.WriteMethod = null;
			// 
			// tb_r_v1
			// 
			this.tb_r_v1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(200)))), ((int)(((byte)(200)))));
			this.tb_r_v1.Enabled = false;
			this.tb_r_v1.IsEnable = null;
			this.tb_r_v1.Location = new System.Drawing.Point(25, 48);
			this.tb_r_v1.Margin = new System.Windows.Forms.Padding(0);
			this.tb_r_v1.MaxLength = 8;
			this.tb_r_v1.Name = "tb_r_v1";
			this.tb_r_v1.ReadMethod = null;
			this.tb_r_v1.Size = new System.Drawing.Size(30, 19);
			this.tb_r_v1.TabIndex = 2;
			this.tb_r_v1.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			this.tb_r_v1.WriteMethod = null;
			// 
			// ColorWithRandom
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.panel2);
			this.Controls.Add(this.panel1);
			this.Controls.Add(this.tb_a_v2);
			this.Controls.Add(this.tb_b_v2);
			this.Controls.Add(this.tb_g_v2);
			this.Controls.Add(this.tb_r_v2);
			this.Controls.Add(this.btn_color_max);
			this.Controls.Add(this.tb_a_v1);
			this.Controls.Add(this.tb_b_v1);
			this.Controls.Add(this.tb_g_v1);
			this.Controls.Add(this.btn_color_min);
			this.Controls.Add(this.tb_r_v1);
			this.Controls.Add(this.lb_v2);
			this.Controls.Add(this.lb_v1);
			this.Controls.Add(this.lb_a);
			this.Controls.Add(this.lb_b);
			this.Controls.Add(this.lb_g);
			this.Controls.Add(this.lb_r);
			this.Name = "ColorWithRandom";
			this.Size = new System.Drawing.Size(164, 88);
			this.panel1.ResumeLayout(false);
			this.panel1.PerformLayout();
			this.panel2.ResumeLayout(false);
			this.panel2.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private DelegateIntTextBox tb_r_v1;
		private System.Windows.Forms.Button btn_color_min;
		private System.Windows.Forms.Label lb_r;
		private DelegateIntTextBox tb_g_v1;
		private DelegateIntTextBox tb_b_v1;
		private DelegateIntTextBox tb_a_v1;
		private System.Windows.Forms.Label lb_g;
		private System.Windows.Forms.Button btn_color_max;
		private System.Windows.Forms.Label lb_v1;
		private System.Windows.Forms.Label lb_v2;
		private System.Windows.Forms.Label lb_b;
		private System.Windows.Forms.Label lb_a;
		private DelegateIntTextBox tb_r_v2;
		private DelegateIntTextBox tb_g_v2;
		private DelegateIntTextBox tb_b_v2;
		private DelegateIntTextBox tb_a_v2;
		private System.Windows.Forms.ColorDialog colorDialog;
		private System.Windows.Forms.Panel panel1;
		private System.Windows.Forms.RadioButton drawnas_2;
		private System.Windows.Forms.RadioButton drawnas_1;
		private System.Windows.Forms.Panel panel2;
		private System.Windows.Forms.RadioButton colorSpace_2;
		private System.Windows.Forms.RadioButton colorSpace_1;
	}
}
