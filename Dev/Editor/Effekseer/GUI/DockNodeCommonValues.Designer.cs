namespace Effekseer.GUI
{
	partial class DockNodeCommonValues
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

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.lp_Node = new Effekseer.GUI.Component.LayoutPanel();
			this.lp_Common = new Effekseer.GUI.Component.LayoutPanel();
			this.SuspendLayout();
			// 
			// lp_Node
			// 
			this.lp_Node.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.lp_Node.AutoScroll = true;
			this.lp_Node.Location = new System.Drawing.Point(1, 1);
			this.lp_Node.Name = "lp_Node";
			this.lp_Node.Size = new System.Drawing.Size(290, 60);
			this.lp_Node.TabIndex = 1;
			// 
			// lp_Common
			// 
			this.lp_Common.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.lp_Common.AutoScroll = true;
			this.lp_Common.Location = new System.Drawing.Point(1, 67);
			this.lp_Common.Name = "lp_Common";
			this.lp_Common.Size = new System.Drawing.Size(290, 312);
			this.lp_Common.TabIndex = 2;
			// 
			// DockNodeCommonValues
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.AutoScroll = true;
			this.ClientSize = new System.Drawing.Size(292, 381);
			this.Controls.Add(this.lp_Common);
			this.Controls.Add(this.lp_Node);
			this.DockAreas = ((WeifenLuo.WinFormsUI.Docking.DockAreas)(((((WeifenLuo.WinFormsUI.Docking.DockAreas.Float | WeifenLuo.WinFormsUI.Docking.DockAreas.DockLeft) 
            | WeifenLuo.WinFormsUI.Docking.DockAreas.DockRight) 
            | WeifenLuo.WinFormsUI.Docking.DockAreas.DockTop) 
            | WeifenLuo.WinFormsUI.Docking.DockAreas.DockBottom)));
			this.Font = new System.Drawing.Font("MS UI Gothic", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.Name = "DockNodeCommonValues";
			this.Text = Properties.Resources.BasicSettings;
			this.Load += new System.EventHandler(this.DockNodeCommonValues_Load);
			this.ResumeLayout(false);

		}

		#endregion

		private Component.LayoutPanel lp_Node;
		private Component.LayoutPanel lp_Common;

	}
}