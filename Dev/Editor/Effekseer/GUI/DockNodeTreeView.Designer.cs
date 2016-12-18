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
			this.imageList1 = new System.Windows.Forms.ImageList(this.components);
			this.SuspendLayout();
			// 
			// contextMenuStrip
			// 
			this.contextMenuStrip.ImageScalingSize = new System.Drawing.Size(20, 20);
			this.contextMenuStrip.Name = "contextMenuStrip1";
			this.contextMenuStrip.Size = new System.Drawing.Size(61, 4);
			// 
			// btn_down
			// 
			this.btn_down.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.btn_down.Location = new System.Drawing.Point(16, 328);
			this.btn_down.Margin = new System.Windows.Forms.Padding(4);
			this.btn_down.Name = "btn_down";
			this.btn_down.Size = new System.Drawing.Size(357, 25);
			this.btn_down.TabIndex = 2;
			this.btn_down.Text = "↓";
			this.btn_down.UseVisualStyleBackColor = true;
			this.btn_down.Click += new System.EventHandler(this.btn_down_Click);
			// 
			// btn_up
			// 
			this.btn_up.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.btn_up.Location = new System.Drawing.Point(16, 296);
			this.btn_up.Margin = new System.Windows.Forms.Padding(4);
			this.btn_up.Name = "btn_up";
			this.btn_up.Size = new System.Drawing.Size(357, 25);
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
			this.nodeTreeView.ImageIndex = 0;
			this.nodeTreeView.ImageList = this.imageList1;
			this.nodeTreeView.Location = new System.Drawing.Point(16, 15);
			this.nodeTreeView.Margin = new System.Windows.Forms.Padding(4);
			this.nodeTreeView.Name = "nodeTreeView";
			this.nodeTreeView.SelectedImageIndex = 0;
			this.nodeTreeView.Size = new System.Drawing.Size(356, 273);
			this.nodeTreeView.TabIndex = 0;
			// 
			// imageList1
			// 
			this.imageList1.ColorDepth = System.Windows.Forms.ColorDepth.Depth8Bit;
			this.imageList1.ImageSize = new System.Drawing.Size(18, 18);
			this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
			// 
			// DockNodeTreeView
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 15F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(389, 352);
			this.ContextMenuStrip = this.contextMenuStrip;
			this.Controls.Add(this.nodeTreeView);
			this.Controls.Add(this.btn_down);
			this.Controls.Add(this.btn_up);
			this.DockAreas = ((WeifenLuo.WinFormsUI.Docking.DockAreas)(((((WeifenLuo.WinFormsUI.Docking.DockAreas.Float | WeifenLuo.WinFormsUI.Docking.DockAreas.DockLeft) 
            | WeifenLuo.WinFormsUI.Docking.DockAreas.DockRight) 
            | WeifenLuo.WinFormsUI.Docking.DockAreas.DockTop) 
            | WeifenLuo.WinFormsUI.Docking.DockAreas.DockBottom)));
			this.Font = new System.Drawing.Font("MS UI Gothic", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.Margin = new System.Windows.Forms.Padding(4);
			this.Name = "DockNodeTreeView";
			this.Text = Properties.Resources.NodeTree;
			this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.DockNodeTreeView_FormClosed);
			this.Load += new System.EventHandler(this.DockNodeTreeView_Load);
			this.ResumeLayout(false);

		}

		#endregion

		private Component.NodeTreeView nodeTreeView;
		private System.Windows.Forms.ContextMenuStrip contextMenuStrip;
		private System.Windows.Forms.Button btn_up;
		private System.Windows.Forms.Button btn_down;
		private System.Windows.Forms.ImageList imageList1;
	}
}
