namespace Effekseer.GUI
{
	partial class DockNodeLocationAbsValues
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
			this.lp_TranslationAbs = new Effekseer.GUI.Component.LayoutPanel();
			this.SuspendLayout();
			// 
			// lp_TranslationAbs
			// 
			this.lp_TranslationAbs.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.lp_TranslationAbs.AutoScroll = true;
			this.lp_TranslationAbs.Location = new System.Drawing.Point(1, 1);
			this.lp_TranslationAbs.Name = "lp_TranslationAbs";
			this.lp_TranslationAbs.Size = new System.Drawing.Size(289, 269);
			this.lp_TranslationAbs.TabIndex = 0;
			// 
			// DockNodeLocationAbsValues
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.AutoScroll = true;
			this.ClientSize = new System.Drawing.Size(292, 273);
			this.Controls.Add(this.lp_TranslationAbs);
			this.DockAreas = ((WeifenLuo.WinFormsUI.Docking.DockAreas)(((((WeifenLuo.WinFormsUI.Docking.DockAreas.Float | WeifenLuo.WinFormsUI.Docking.DockAreas.DockLeft) 
            | WeifenLuo.WinFormsUI.Docking.DockAreas.DockRight) 
            | WeifenLuo.WinFormsUI.Docking.DockAreas.DockTop) 
            | WeifenLuo.WinFormsUI.Docking.DockAreas.DockBottom)));
			this.Font = new System.Drawing.Font("MS UI Gothic", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.Name = "DockNodeLocationAbsValues";
			this.Text = Properties.Resources.AttractionForces;
			this.Load += new System.EventHandler(this.DockNodeTranslationAbsValues_Load);
			this.ResumeLayout(false);

		}

		#endregion

		private Component.LayoutPanel lp_TranslationAbs;
	}
}
