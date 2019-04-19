namespace Effekseer.GUI.Component
{
	partial class Vector2DWithRandom
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
			this.drawnas_1 = new System.Windows.Forms.RadioButton();
			this.drawnas_2 = new System.Windows.Forms.RadioButton();
			this.lb_v1 = new System.Windows.Forms.Label();
			this.lb_v2 = new System.Windows.Forms.Label();
			this.lb_x = new System.Windows.Forms.Label();
			this.lb_y = new System.Windows.Forms.Label();
			this.tb_y_v2 = new Effekseer.GUI.Component.DelegateFloatTextBox();
			this.tb_y_v1 = new Effekseer.GUI.Component.DelegateFloatTextBox();
			this.tb_x_v2 = new Effekseer.GUI.Component.DelegateFloatTextBox();
			this.tb_x_v1 = new Effekseer.GUI.Component.DelegateFloatTextBox();
			this.SuspendLayout();
			// 
			// drawnas_1
			// 
			this.drawnas_1.AutoSize = true;
			this.drawnas_1.Font = new System.Drawing.Font("MS UI Gothic", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.drawnas_1.Location = new System.Drawing.Point(3, 3);
			this.drawnas_1.Name = "drawnas_1";
			this.drawnas_1.Size = new System.Drawing.Size(69, 15);
			this.drawnas_1.TabIndex = 0;
			this.drawnas_1.TabStop = true;
			this.drawnas_1.Text = Properties.Resources.Range;
			this.drawnas_1.UseVisualStyleBackColor = true;
			this.drawnas_1.CheckedChanged += new System.EventHandler(this.drawnas_1_CheckedChanged);
			// 
			// drawnas_2
			// 
			this.drawnas_2.AutoSize = true;
			this.drawnas_2.Font = new System.Drawing.Font("MS UI Gothic", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.drawnas_2.Location = new System.Drawing.Point(70, 3);
			this.drawnas_2.Name = "drawnas_2";
			this.drawnas_2.Size = new System.Drawing.Size(69, 15);
			this.drawnas_2.TabIndex = 1;
			this.drawnas_2.TabStop = true;
			this.drawnas_2.Text = Properties.Resources.Gauss;
			this.drawnas_2.UseVisualStyleBackColor = true;
			this.drawnas_2.CheckedChanged += new System.EventHandler(this.drawnas_2_CheckedChanged);
			// 
			// lb_v1
			// 
			this.lb_v1.AutoSize = true;
			this.lb_v1.Font = new System.Drawing.Font("MS UI Gothic", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.lb_v1.Location = new System.Drawing.Point(4, 21);
			this.lb_v1.Name = "lb_v1";
			this.lb_v1.Size = new System.Drawing.Size(10, 11);
			this.lb_v1.TabIndex = 4;
			this.lb_v1.Text = "?";
			// 
			// lb_v2
			// 
			this.lb_v2.AutoSize = true;
			this.lb_v2.Font = new System.Drawing.Font("MS UI Gothic", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.lb_v2.Location = new System.Drawing.Point(68, 21);
			this.lb_v2.Name = "lb_v2";
			this.lb_v2.Size = new System.Drawing.Size(10, 11);
			this.lb_v2.TabIndex = 5;
			this.lb_v2.Text = "?";
			// 
			// lb_x
			// 
			this.lb_x.AutoSize = true;
			this.lb_x.ForeColor = System.Drawing.Color.Maroon;
			this.lb_x.Location = new System.Drawing.Point(3, 36);
			this.lb_x.Name = "lb_x";
			this.lb_x.Size = new System.Drawing.Size(11, 12);
			this.lb_x.TabIndex = 6;
			this.lb_x.Text = "x";
			// 
			// lb_y
			// 
			this.lb_y.AutoSize = true;
			this.lb_y.ForeColor = System.Drawing.Color.Green;
			this.lb_y.Location = new System.Drawing.Point(3, 55);
			this.lb_y.Name = "lb_y";
			this.lb_y.Size = new System.Drawing.Size(11, 12);
			this.lb_y.TabIndex = 9;
			this.lb_y.Text = "y";
			// 
			// tb_y_v2
			// 
			this.tb_y_v2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(200)))), ((int)(((byte)(200)))));
			this.tb_y_v2.Enabled = false;
			this.tb_y_v2.IsEnable = null;
			this.tb_y_v2.Location = new System.Drawing.Point(70, 52);
			this.tb_y_v2.Margin = new System.Windows.Forms.Padding(0);
			this.tb_y_v2.Name = "tb_y_v2";
			this.tb_y_v2.ReadMethod = null;
			this.tb_y_v2.Size = new System.Drawing.Size(50, 19);
			this.tb_y_v2.TabIndex = 8;
			this.tb_y_v2.WheelStep = 1F;
			this.tb_y_v2.WriteMethod = null;
			// 
			// tb_y_v1
			// 
			this.tb_y_v1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(200)))), ((int)(((byte)(200)))));
			this.tb_y_v1.Enabled = false;
			this.tb_y_v1.IsEnable = null;
			this.tb_y_v1.Location = new System.Drawing.Point(12, 52);
			this.tb_y_v1.Margin = new System.Windows.Forms.Padding(0);
			this.tb_y_v1.Name = "tb_y_v1";
			this.tb_y_v1.ReadMethod = null;
			this.tb_y_v1.Size = new System.Drawing.Size(50, 19);
			this.tb_y_v1.TabIndex = 7;
			this.tb_y_v1.WheelStep = 1F;
			this.tb_y_v1.WriteMethod = null;
			// 
			// tb_x_v2
			// 
			this.tb_x_v2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(200)))), ((int)(((byte)(200)))));
			this.tb_x_v2.Enabled = false;
			this.tb_x_v2.IsEnable = null;
			this.tb_x_v2.Location = new System.Drawing.Point(70, 32);
			this.tb_x_v2.Margin = new System.Windows.Forms.Padding(0);
			this.tb_x_v2.Name = "tb_x_v2";
			this.tb_x_v2.ReadMethod = null;
			this.tb_x_v2.Size = new System.Drawing.Size(50, 19);
			this.tb_x_v2.TabIndex = 3;
			this.tb_x_v2.WheelStep = 1F;
			this.tb_x_v2.WriteMethod = null;
			// 
			// tb_x_v1
			// 
			this.tb_x_v1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(200)))), ((int)(((byte)(200)))));
			this.tb_x_v1.Enabled = false;
			this.tb_x_v1.IsEnable = null;
			this.tb_x_v1.Location = new System.Drawing.Point(12, 33);
			this.tb_x_v1.Margin = new System.Windows.Forms.Padding(0);
			this.tb_x_v1.Name = "tb_x_v1";
			this.tb_x_v1.ReadMethod = null;
			this.tb_x_v1.Size = new System.Drawing.Size(50, 19);
			this.tb_x_v1.TabIndex = 2;
			this.tb_x_v1.WheelStep = 1F;
			this.tb_x_v1.WriteMethod = null;
			// 
			// Vector2DWithRandom
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.lb_y);
			this.Controls.Add(this.tb_y_v2);
			this.Controls.Add(this.tb_y_v1);
			this.Controls.Add(this.lb_x);
			this.Controls.Add(this.lb_v2);
			this.Controls.Add(this.lb_v1);
			this.Controls.Add(this.tb_x_v2);
			this.Controls.Add(this.tb_x_v1);
			this.Controls.Add(this.drawnas_2);
			this.Controls.Add(this.drawnas_1);
			this.Name = "Vector2DWithRandom";
			this.Size = new System.Drawing.Size(135, 75);
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.RadioButton drawnas_1;
		private System.Windows.Forms.RadioButton drawnas_2;
		private DelegateFloatTextBox tb_x_v1;
		private DelegateFloatTextBox tb_x_v2;
		private System.Windows.Forms.Label lb_v1;
		private System.Windows.Forms.Label lb_v2;
		private System.Windows.Forms.Label lb_x;
		private DelegateFloatTextBox tb_y_v1;
		private DelegateFloatTextBox tb_y_v2;
		private System.Windows.Forms.Label lb_y;
	}
}
