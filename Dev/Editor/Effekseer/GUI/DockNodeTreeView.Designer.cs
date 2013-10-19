namespace Effekseer.GUI
{
	partial class DockNodeTreeView
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
			this.components = new System.ComponentModel.Container();
			this.contextMenuStrip = new System.Windows.Forms.ContextMenuStrip(this.components);
			this.btn_down = new System.Windows.Forms.Button();
			this.btn_up = new System.Windows.Forms.Button();
			this.nodeTreeView = new Effekseer.GUI.Component.NodeTreeView();
			this.SuspendLayout();
			// 
			// contextMenuStrip
			// 
			this.contextMenuStrip.Name = "contextMenuStrip1";
			this.contextMenuStrip.Size = new System.Drawing.Size(61, 4);
			// 
			// btn_down
			// 
			this.btn_down.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.btn_down.Location = new System.Drawing.Point(12, 262);
			this.btn_down.Name = "btn_down";
			this.btn_down.Size = new System.Drawing.Size(268, 20);
			this.btn_down.TabIndex = 2;
			this.btn_down.Text = "↓";
			this.btn_down.UseVisualStyleBackColor = true;
			this.btn_down.Click += new System.EventHandler(this.btn_down_Click);
			// 
			// btn_up
			// 
			this.btn_up.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.btn_up.Location = new System.Drawing.Point(12, 237);
			this.btn_up.Name = "btn_up";
			this.btn_up.Size = new System.Drawing.Size(268, 20);
			this.btn_up.TabIndex = 1;
			this.btn_up.Text = "↑";
			this.btn_up.UseVisualStyleBackColor = true;
			this.btn_up.Click += new System.EventHandler(this.btn_up_Click);
			// 
			// nodeTreeView
			// 
			this.nodeTreeView.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.nodeTreeView.CheckBoxes = true;
			this.nodeTreeView.Location = new System.Drawing.Point(12, 12);
			this.nodeTreeView.Name = "nodeTreeView";
			this.nodeTreeView.Size = new System.Drawing.Size(268, 219);
			this.nodeTreeView.TabIndex = 0;
			// 
			// DockNodeTreeView
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(292, 282);
			this.ContextMenuStrip = this.contextMenuStrip;
			this.Controls.Add(this.nodeTreeView);
			this.Controls.Add(this.btn_down);
			this.Controls.Add(this.btn_up);
			this.DockAreas = ((WeifenLuo.WinFormsUI.Docking.DockAreas)(((((WeifenLuo.WinFormsUI.Docking.DockAreas.Float | WeifenLuo.WinFormsUI.Docking.DockAreas.DockLeft) 
            | WeifenLuo.WinFormsUI.Docking.DockAreas.DockRight) 
            | WeifenLuo.WinFormsUI.Docking.DockAreas.DockTop) 
            | WeifenLuo.WinFormsUI.Docking.DockAreas.DockBottom)));
			this.Font = new System.Drawing.Font("MS UI Gothic", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.Name = "DockNodeTreeView";
			this.Text = "ノードツリー";
			this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.DockNodeTreeView_FormClosed);
			this.Load += new System.EventHandler(this.DockNodeTreeView_Load);
			this.ResumeLayout(false);

		}

		#endregion

		private Component.NodeTreeView nodeTreeView;
		private System.Windows.Forms.ContextMenuStrip contextMenuStrip;
		private System.Windows.Forms.Button btn_up;
		private System.Windows.Forms.Button btn_down;
	}
}
