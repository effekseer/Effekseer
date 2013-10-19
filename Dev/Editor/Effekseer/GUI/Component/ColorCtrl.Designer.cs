namespace Effekseer.GUI.Component
{
	partial class ColorCtrl
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

		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.btn_color = new System.Windows.Forms.Button();
			this.lb_r = new System.Windows.Forms.Label();
			this.lb_g = new System.Windows.Forms.Label();
			this.label1 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.colorDialog = new System.Windows.Forms.ColorDialog();
			this.tb_a = new Effekseer.GUI.Component.Int();
			this.tb_b = new Effekseer.GUI.Component.Int();
			this.tb_g = new Effekseer.GUI.Component.Int();
			this.tb_r = new Effekseer.GUI.Component.Int();
			this.SuspendLayout();
			// 
			// btn_color
			// 
			this.btn_color.Location = new System.Drawing.Point(120, 12);
			this.btn_color.Name = "btn_color";
			this.btn_color.Size = new System.Drawing.Size(21, 20);
			this.btn_color.TabIndex = 0;
			this.btn_color.UseVisualStyleBackColor = true;
			this.btn_color.Click += new System.EventHandler(this.btn_color_Click);
			// 
			// lb_r
			// 
			this.lb_r.AutoSize = true;
			this.lb_r.Font = new System.Drawing.Font("MS UI Gothic", 8F);
			this.lb_r.Location = new System.Drawing.Point(10, 2);
			this.lb_r.Name = "lb_r";
			this.lb_r.Size = new System.Drawing.Size(12, 11);
			this.lb_r.TabIndex = 2;
			this.lb_r.Text = "R";
			// 
			// lb_g
			// 
			this.lb_g.AutoSize = true;
			this.lb_g.Font = new System.Drawing.Font("MS UI Gothic", 8F);
			this.lb_g.Location = new System.Drawing.Point(39, 2);
			this.lb_g.Name = "lb_g";
			this.lb_g.Size = new System.Drawing.Size(12, 11);
			this.lb_g.TabIndex = 6;
			this.lb_g.Text = "G";
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Font = new System.Drawing.Font("MS UI Gothic", 8F);
			this.label1.Location = new System.Drawing.Point(69, 2);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(12, 11);
			this.label1.TabIndex = 7;
			this.label1.Text = "B";
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Font = new System.Drawing.Font("MS UI Gothic", 8F);
			this.label2.Location = new System.Drawing.Point(98, 2);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(12, 11);
			this.label2.TabIndex = 8;
			this.label2.Text = "A";
			// 
			// tb_a
			// 
			this.tb_a.Anchor = System.Windows.Forms.AnchorStyles.Left;
			this.tb_a.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(170)))), ((int)(((byte)(170)))));
			this.tb_a.Binding = null;
			this.tb_a.Enabled = false;
			this.tb_a.IsEnable = null;
			this.tb_a.Location = new System.Drawing.Point(90, 13);
			this.tb_a.Margin = new System.Windows.Forms.Padding(0);
			this.tb_a.MaxLength = 8;
			this.tb_a.Name = "tb_a";
			this.tb_a.Size = new System.Drawing.Size(30, 19);
			this.tb_a.TabIndex = 4;
			this.tb_a.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			// 
			// tb_b
			// 
			this.tb_b.Anchor = System.Windows.Forms.AnchorStyles.Left;
			this.tb_b.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(170)))), ((int)(((byte)(170)))));
			this.tb_b.Binding = null;
			this.tb_b.Enabled = false;
			this.tb_b.IsEnable = null;
			this.tb_b.Location = new System.Drawing.Point(60, 13);
			this.tb_b.Margin = new System.Windows.Forms.Padding(0);
			this.tb_b.MaxLength = 8;
			this.tb_b.Name = "tb_b";
			this.tb_b.Size = new System.Drawing.Size(30, 19);
			this.tb_b.TabIndex = 3;
			this.tb_b.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			// 
			// tb_g
			// 
			this.tb_g.Anchor = System.Windows.Forms.AnchorStyles.Left;
			this.tb_g.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(170)))), ((int)(((byte)(170)))));
			this.tb_g.Binding = null;
			this.tb_g.Enabled = false;
			this.tb_g.IsEnable = null;
			this.tb_g.Location = new System.Drawing.Point(30, 13);
			this.tb_g.Margin = new System.Windows.Forms.Padding(0);
			this.tb_g.MaxLength = 8;
			this.tb_g.Name = "tb_g";
			this.tb_g.Size = new System.Drawing.Size(30, 19);
			this.tb_g.TabIndex = 2;
			this.tb_g.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			// 
			// tb_r
			// 
			this.tb_r.Anchor = System.Windows.Forms.AnchorStyles.Left;
			this.tb_r.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(170)))), ((int)(((byte)(170)))));
			this.tb_r.Binding = null;
			this.tb_r.Enabled = false;
			this.tb_r.IsEnable = null;
			this.tb_r.Location = new System.Drawing.Point(0, 13);
			this.tb_r.Margin = new System.Windows.Forms.Padding(0);
			this.tb_r.MaxLength = 8;
			this.tb_r.Name = "tb_r";
			this.tb_r.Size = new System.Drawing.Size(30, 19);
			this.tb_r.TabIndex = 1;
			this.tb_r.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			// 
			// ColorCtrl
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.label2);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.lb_g);
			this.Controls.Add(this.tb_a);
			this.Controls.Add(this.tb_b);
			this.Controls.Add(this.tb_g);
			this.Controls.Add(this.lb_r);
			this.Controls.Add(this.btn_color);
			this.Controls.Add(this.tb_r);
			this.Name = "ColorCtrl";
			this.Size = new System.Drawing.Size(142, 33);
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private Int tb_r;
		private System.Windows.Forms.Button btn_color;
		private System.Windows.Forms.Label lb_r;
		private Int tb_g;
		private Int tb_b;
		private Int tb_a;
		private System.Windows.Forms.Label lb_g;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.ColorDialog colorDialog;
	}
}
