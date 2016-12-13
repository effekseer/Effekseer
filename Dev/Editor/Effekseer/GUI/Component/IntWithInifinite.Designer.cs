namespace Effekseer.GUI.Component
{
	partial class IntWithInifinite
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
			this.cb_infinite = new Effekseer.GUI.Component.Boolean();
			this.tb_int = new Effekseer.GUI.Component.Int();
			this.SuspendLayout();
			// 
			// cb_infinite
			// 
			this.cb_infinite.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)));
			this.cb_infinite.AutoSize = true;
			this.cb_infinite.Binding = null;
			this.cb_infinite.Enabled = false;
			this.cb_infinite.Location = new System.Drawing.Point(85, 5);
			this.cb_infinite.Name = "cb_infinite";
			this.cb_infinite.Size = new System.Drawing.Size(48, 16);
			this.cb_infinite.TabIndex = 1;
			this.cb_infinite.Text = Properties.Resources.Infinite;
			// 
			// tb_int
			// 
			this.tb_int.Anchor = System.Windows.Forms.AnchorStyles.Left;
			this.tb_int.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(200)))), ((int)(((byte)(200)))));
			this.tb_int.Binding = null;
			this.tb_int.Enabled = false;
			this.tb_int.IsEnable = null;
			this.tb_int.Location = new System.Drawing.Point(3, 3);
			this.tb_int.Name = "tb_int";
			this.tb_int.Size = new System.Drawing.Size(80, 19);
			this.tb_int.TabIndex = 0;
			// 
			// IntWithInifinite
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.cb_infinite);
			this.Controls.Add(this.tb_int);
			this.Name = "IntWithInifinite";
			this.Size = new System.Drawing.Size(140, 25);
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private Int tb_int;
		private Boolean cb_infinite;
	}
}
