namespace Effekseer.GUI
{
	partial class DockNodeDepthValues
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
			this.lp_Depth = new Effekseer.GUI.Component.LayoutPanel();
			this.SuspendLayout();
			// 
			// lp_Scale
			// 
			this.lp_Depth.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.lp_Depth.AutoScroll = true;
			this.lp_Depth.Location = new System.Drawing.Point(1, 1);
			this.lp_Depth.Name = "lp_Scale";
			this.lp_Depth.Size = new System.Drawing.Size(290, 270);
			this.lp_Depth.TabIndex = 0;
			// 
			// DockNodeScaleValues
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(292, 273);
			this.Controls.Add(this.lp_Depth);
			this.Font = new System.Drawing.Font("MS UI Gothic", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.Name = "DockNodeScaleValues";
			this.Text = Properties.Resources.Scale;
			this.Load += new System.EventHandler(this.DockNodeScalingValues_Load);
			this.ResumeLayout(false);

		}

		#endregion

		private Component.LayoutPanel lp_Depth;
	}
}
