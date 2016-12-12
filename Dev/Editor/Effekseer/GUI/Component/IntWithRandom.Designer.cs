namespace Effekseer.GUI.Component
{
	partial class IntWithRandom
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
			this.tb_v2 = new Effekseer.GUI.Component.DelegateIntTextBox();
			this.tb_v1 = new Effekseer.GUI.Component.DelegateIntTextBox();
			this.SuspendLayout();
			// 
			// drawnas_1
			// 
			this.drawnas_1.AutoSize = true;
			this.drawnas_1.Font = new System.Drawing.Font("MS UI Gothic", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
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
			this.drawnas_2.Font = new System.Drawing.Font("MS UI Gothic", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.drawnas_2.Location = new System.Drawing.Point(67, 3);
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
			this.lb_v1.Font = new System.Drawing.Font("MS UI Gothic", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.lb_v1.Location = new System.Drawing.Point(3, 21);
			this.lb_v1.Name = "lb_v1";
			this.lb_v1.Size = new System.Drawing.Size(15, 11);
			this.lb_v1.TabIndex = 4;
			this.lb_v1.Text = "aa";
			// 
			// lb_v2
			// 
			this.lb_v2.AutoSize = true;
			this.lb_v2.Font = new System.Drawing.Font("MS UI Gothic", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.lb_v2.Location = new System.Drawing.Point(65, 21);
			this.lb_v2.Name = "lb_v2";
			this.lb_v2.Size = new System.Drawing.Size(15, 11);
			this.lb_v2.TabIndex = 5;
			this.lb_v2.Text = "aa";
			// 
			// tb_v2
			// 
			this.tb_v2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(200)))), ((int)(((byte)(200)))));
			this.tb_v2.Enabled = false;
			this.tb_v2.IsEnable = null;
			this.tb_v2.Location = new System.Drawing.Point(67, 35);
			this.tb_v2.Name = "tb_v2";
			this.tb_v2.ReadMethod = null;
			this.tb_v2.Size = new System.Drawing.Size(50, 19);
			this.tb_v2.TabIndex = 3;
			this.tb_v2.WheelStep = 1;
			this.tb_v2.WriteMethod = null;
			// 
			// tb_v1
			// 
			this.tb_v1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(200)))), ((int)(((byte)(200)))));
			this.tb_v1.Enabled = false;
			this.tb_v1.IsEnable = null;
			this.tb_v1.Location = new System.Drawing.Point(3, 35);
			this.tb_v1.Name = "tb_v1";
			this.tb_v1.ReadMethod = null;
			this.tb_v1.Size = new System.Drawing.Size(50, 19);
			this.tb_v1.TabIndex = 2;
			this.tb_v1.WheelStep = 1;
			this.tb_v1.WriteMethod = null;
			// 
			// IntWithRandom
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.lb_v2);
			this.Controls.Add(this.lb_v1);
			this.Controls.Add(this.tb_v2);
			this.Controls.Add(this.tb_v1);
			this.Controls.Add(this.drawnas_2);
			this.Controls.Add(this.drawnas_1);
			this.Name = "IntWithRandom";
			this.Size = new System.Drawing.Size(135, 60);
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.RadioButton drawnas_1;
		private System.Windows.Forms.RadioButton drawnas_2;
		private DelegateIntTextBox tb_v1;
		private DelegateIntTextBox tb_v2;
		private System.Windows.Forms.Label lb_v1;
		private System.Windows.Forms.Label lb_v2;
	}
}
