namespace Effekseer.GUI
{
	partial class DockViewPoint
	{
		/// <summary> 
		/// 必要なデザイナー変数です。
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary> 
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		/// <param name="disposing">マネージ リソースが破棄される場合 true、破棄されない場合は false です。</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region コンポーネント デザイナーで生成されたコード

		/// <summary> 
		/// デザイナー サポートに必要なメソッドです。このメソッドの内容を 
		/// コード エディターで変更しないでください。
		/// </summary>
		private void InitializeComponent()
		{
			this.tableLayoutPanel = new System.Windows.Forms.TableLayoutPanel();
			this.txt_guideHeight = new Effekseer.GUI.Component.DelegateIntTextBox();
			this.lbl_guideHeight = new System.Windows.Forms.Label();
			this.lbl_guideWidth = new System.Windows.Forms.Label();
			this.lbl_guide = new System.Windows.Forms.Label();
			this.lbl_type = new System.Windows.Forms.Label();
			this.txt_mag = new Effekseer.GUI.Component.DelegateFloatTextBox();
			this.lbl_mag = new System.Windows.Forms.Label();
			this.txt_distance = new Effekseer.GUI.Component.DelegateFloatTextBox();
			this.lbl_distance = new System.Windows.Forms.Label();
			this.txt_angleY = new Effekseer.GUI.Component.DelegateFloatTextBox();
			this.txt_angleX = new Effekseer.GUI.Component.DelegateFloatTextBox();
			this.lbl_angleX = new System.Windows.Forms.Label();
			this.txt_focusZ = new Effekseer.GUI.Component.DelegateFloatTextBox();
			this.txt_focusX = new Effekseer.GUI.Component.DelegateFloatTextBox();
			this.txt_focusY = new Effekseer.GUI.Component.DelegateFloatTextBox();
			this.lbl_angleY = new System.Windows.Forms.Label();
			this.lbl_focus = new System.Windows.Forms.Label();
			this.cb_type = new System.Windows.Forms.ComboBox();
			this.cb_guide = new System.Windows.Forms.CheckBox();
			this.txt_guideWidth = new Effekseer.GUI.Component.DelegateIntTextBox();
			this.tableLayoutPanel.SuspendLayout();
			this.SuspendLayout();
			// 
			// tableLayoutPanel
			// 
			this.tableLayoutPanel.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.tableLayoutPanel.ColumnCount = 4;
			this.tableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
			this.tableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
			this.tableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
			this.tableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 25F));
			this.tableLayoutPanel.Controls.Add(this.txt_guideHeight, 3, 4);
			this.tableLayoutPanel.Controls.Add(this.lbl_guideHeight, 2, 4);
			this.tableLayoutPanel.Controls.Add(this.lbl_guideWidth, 0, 4);
			this.tableLayoutPanel.Controls.Add(this.lbl_guide, 2, 3);
			this.tableLayoutPanel.Controls.Add(this.lbl_type, 0, 3);
			this.tableLayoutPanel.Controls.Add(this.txt_mag, 3, 2);
			this.tableLayoutPanel.Controls.Add(this.lbl_mag, 2, 2);
			this.tableLayoutPanel.Controls.Add(this.txt_distance, 1, 2);
			this.tableLayoutPanel.Controls.Add(this.lbl_distance, 0, 2);
			this.tableLayoutPanel.Controls.Add(this.txt_angleY, 3, 1);
			this.tableLayoutPanel.Controls.Add(this.txt_angleX, 1, 1);
			this.tableLayoutPanel.Controls.Add(this.lbl_angleX, 0, 1);
			this.tableLayoutPanel.Controls.Add(this.txt_focusZ, 3, 0);
			this.tableLayoutPanel.Controls.Add(this.lbl_angleY, 2, 1);
			this.tableLayoutPanel.Controls.Add(this.lbl_focus, 0, 0);
			this.tableLayoutPanel.Controls.Add(this.cb_type, 1, 3);
			this.tableLayoutPanel.Controls.Add(this.cb_guide, 3, 3);
			this.tableLayoutPanel.Controls.Add(this.txt_guideWidth, 1, 4);
			this.tableLayoutPanel.Controls.Add(this.txt_focusY, 2, 0);
			this.tableLayoutPanel.Controls.Add(this.txt_focusX, 1, 0);
			this.tableLayoutPanel.Location = new System.Drawing.Point(2, 2);
			this.tableLayoutPanel.Name = "tableLayoutPanel";
			this.tableLayoutPanel.RowCount = 5;
			this.tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 20F));
			this.tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 20F));
			this.tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 20F));
			this.tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 20F));
			this.tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 20F));
			this.tableLayoutPanel.Size = new System.Drawing.Size(358, 254);
			this.tableLayoutPanel.TabIndex = 0;
			// 
			// txt_guideHeight
			// 
			this.txt_guideHeight.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(170)))), ((int)(((byte)(170)))));
			this.txt_guideHeight.Enabled = false;
			this.txt_guideHeight.IsEnable = null;
			this.txt_guideHeight.Location = new System.Drawing.Point(270, 203);
			this.txt_guideHeight.Name = "txt_guideHeight";
			this.txt_guideHeight.ReadMethod = null;
			this.txt_guideHeight.Size = new System.Drawing.Size(56, 19);
			this.txt_guideHeight.TabIndex = 19;
			this.txt_guideHeight.WriteMethod = null;
			// 
			// lbl_guideHeight
			// 
			this.lbl_guideHeight.AutoSize = true;
			this.lbl_guideHeight.Location = new System.Drawing.Point(181, 206);
			this.lbl_guideHeight.Margin = new System.Windows.Forms.Padding(3, 6, 3, 0);
			this.lbl_guideHeight.Name = "lbl_guideHeight";
			this.lbl_guideHeight.Size = new System.Drawing.Size(57, 12);
			this.lbl_guideHeight.TabIndex = 18;
			this.lbl_guideHeight.Text = "ガイド縦幅";
			// 
			// lbl_guideWidth
			// 
			this.lbl_guideWidth.AutoSize = true;
			this.lbl_guideWidth.Location = new System.Drawing.Point(3, 206);
			this.lbl_guideWidth.Margin = new System.Windows.Forms.Padding(3, 6, 3, 0);
			this.lbl_guideWidth.Name = "lbl_guideWidth";
			this.lbl_guideWidth.Size = new System.Drawing.Size(57, 12);
			this.lbl_guideWidth.TabIndex = 16;
			this.lbl_guideWidth.Text = "ガイド横幅";
			// 
			// lbl_guide
			// 
			this.lbl_guide.AutoSize = true;
			this.lbl_guide.Location = new System.Drawing.Point(181, 156);
			this.lbl_guide.Margin = new System.Windows.Forms.Padding(3, 6, 3, 0);
			this.lbl_guide.Name = "lbl_guide";
			this.lbl_guide.Size = new System.Drawing.Size(57, 12);
			this.lbl_guide.TabIndex = 14;
			this.lbl_guide.Text = "ガイド描画";
			// 
			// lbl_type
			// 
			this.lbl_type.AutoSize = true;
			this.lbl_type.Location = new System.Drawing.Point(3, 156);
			this.lbl_type.Margin = new System.Windows.Forms.Padding(3, 6, 3, 0);
			this.lbl_type.Name = "lbl_type";
			this.lbl_type.Size = new System.Drawing.Size(64, 12);
			this.lbl_type.TabIndex = 12;
			this.lbl_type.Text = "カメラの種類";
			// 
			// txt_mag
			// 
			this.txt_mag.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(170)))), ((int)(((byte)(200)))), ((int)(((byte)(170)))));
			this.txt_mag.Enabled = false;
			this.txt_mag.IsEnable = null;
			this.txt_mag.Location = new System.Drawing.Point(270, 103);
			this.txt_mag.Name = "txt_mag";
			this.txt_mag.ReadMethod = null;
			this.txt_mag.Size = new System.Drawing.Size(56, 19);
			this.txt_mag.TabIndex = 11;
			this.txt_mag.WriteMethod = null;
			// 
			// lbl_mag
			// 
			this.lbl_mag.AutoSize = true;
			this.lbl_mag.Location = new System.Drawing.Point(181, 106);
			this.lbl_mag.Margin = new System.Windows.Forms.Padding(3, 6, 3, 0);
			this.lbl_mag.Name = "lbl_mag";
			this.lbl_mag.Size = new System.Drawing.Size(65, 12);
			this.lbl_mag.TabIndex = 10;
			this.lbl_mag.Text = "画面拡大率";
			// 
			// txt_distance
			// 
			this.txt_distance.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(170)))), ((int)(((byte)(200)))), ((int)(((byte)(170)))));
			this.txt_distance.Enabled = false;
			this.txt_distance.IsEnable = null;
			this.txt_distance.Location = new System.Drawing.Point(92, 103);
			this.txt_distance.Name = "txt_distance";
			this.txt_distance.ReadMethod = null;
			this.txt_distance.Size = new System.Drawing.Size(56, 19);
			this.txt_distance.TabIndex = 9;
			this.txt_distance.WriteMethod = null;
			// 
			// lbl_distance
			// 
			this.lbl_distance.AutoSize = true;
			this.lbl_distance.Location = new System.Drawing.Point(3, 106);
			this.lbl_distance.Margin = new System.Windows.Forms.Padding(3, 6, 3, 0);
			this.lbl_distance.Name = "lbl_distance";
			this.lbl_distance.Size = new System.Drawing.Size(73, 12);
			this.lbl_distance.TabIndex = 8;
			this.lbl_distance.Text = "視点への距離";
			// 
			// txt_angleY
			// 
			this.txt_angleY.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(170)))), ((int)(((byte)(200)))), ((int)(((byte)(170)))));
			this.txt_angleY.Enabled = false;
			this.txt_angleY.IsEnable = null;
			this.txt_angleY.Location = new System.Drawing.Point(270, 53);
			this.txt_angleY.Name = "txt_angleY";
			this.txt_angleY.ReadMethod = null;
			this.txt_angleY.Size = new System.Drawing.Size(56, 19);
			this.txt_angleY.TabIndex = 7;
			this.txt_angleY.WriteMethod = null;
			// 
			// txt_angleX
			// 
			this.txt_angleX.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(170)))), ((int)(((byte)(200)))), ((int)(((byte)(170)))));
			this.txt_angleX.Enabled = false;
			this.txt_angleX.IsEnable = null;
			this.txt_angleX.Location = new System.Drawing.Point(92, 53);
			this.txt_angleX.Name = "txt_angleX";
			this.txt_angleX.ReadMethod = null;
			this.txt_angleX.Size = new System.Drawing.Size(56, 19);
			this.txt_angleX.TabIndex = 6;
			this.txt_angleX.WriteMethod = null;
			// 
			// lbl_angleX
			// 
			this.lbl_angleX.AutoSize = true;
			this.lbl_angleX.Location = new System.Drawing.Point(3, 56);
			this.lbl_angleX.Margin = new System.Windows.Forms.Padding(3, 6, 3, 0);
			this.lbl_angleX.Name = "lbl_angleX";
			this.lbl_angleX.Size = new System.Drawing.Size(48, 12);
			this.lbl_angleX.TabIndex = 4;
			this.lbl_angleX.Text = "X軸回転";
			// 
			// txt_focusZ
			// 
			this.txt_focusZ.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(170)))), ((int)(((byte)(200)))), ((int)(((byte)(170)))));
			this.txt_focusZ.Enabled = false;
			this.txt_focusZ.IsEnable = null;
			this.txt_focusZ.Location = new System.Drawing.Point(270, 3);
			this.txt_focusZ.Name = "txt_focusZ";
			this.txt_focusZ.ReadMethod = null;
			this.txt_focusZ.Size = new System.Drawing.Size(56, 19);
			this.txt_focusZ.TabIndex = 3;
			this.txt_focusZ.WriteMethod = null;
			// 
			// txt_focusX
			// 
			this.txt_focusX.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(170)))), ((int)(((byte)(200)))), ((int)(((byte)(170)))));
			this.txt_focusX.Enabled = false;
			this.txt_focusX.IsEnable = null;
			this.txt_focusX.Location = new System.Drawing.Point(92, 3);
			this.txt_focusX.Name = "txt_focusX";
			this.txt_focusX.ReadMethod = null;
			this.txt_focusX.Size = new System.Drawing.Size(56, 19);
			this.txt_focusX.TabIndex = 1;
			this.txt_focusX.WriteMethod = null;
			// 
			// txt_focusY
			// 
			this.txt_focusY.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(170)))), ((int)(((byte)(200)))), ((int)(((byte)(170)))));
			this.txt_focusY.Enabled = false;
			this.txt_focusY.IsEnable = null;
			this.txt_focusY.Location = new System.Drawing.Point(181, 3);
			this.txt_focusY.Name = "txt_focusY";
			this.txt_focusY.ReadMethod = null;
			this.txt_focusY.Size = new System.Drawing.Size(56, 19);
			this.txt_focusY.TabIndex = 2;
			this.txt_focusY.WriteMethod = null;
			// 
			// lbl_angleY
			// 
			this.lbl_angleY.AutoSize = true;
			this.lbl_angleY.Location = new System.Drawing.Point(181, 56);
			this.lbl_angleY.Margin = new System.Windows.Forms.Padding(3, 6, 3, 0);
			this.lbl_angleY.Name = "lbl_angleY";
			this.lbl_angleY.Size = new System.Drawing.Size(48, 12);
			this.lbl_angleY.TabIndex = 5;
			this.lbl_angleY.Text = "Y軸回転";
			// 
			// lbl_focus
			// 
			this.lbl_focus.AutoSize = true;
			this.lbl_focus.Location = new System.Drawing.Point(3, 6);
			this.lbl_focus.Margin = new System.Windows.Forms.Padding(3, 6, 3, 0);
			this.lbl_focus.Name = "lbl_focus";
			this.lbl_focus.Size = new System.Drawing.Size(29, 12);
			this.lbl_focus.TabIndex = 0;
			this.lbl_focus.Text = "焦点";
			// 
			// cb_type
			// 
			this.cb_type.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cb_type.FormattingEnabled = true;
			this.cb_type.Items.AddRange(new object[] {
            "Perspective",
            "Orthographic"});
			this.cb_type.Location = new System.Drawing.Point(92, 153);
			this.cb_type.Name = "cb_type";
			this.cb_type.Size = new System.Drawing.Size(56, 20);
			this.cb_type.TabIndex = 13;
			this.cb_type.SelectedIndexChanged += new System.EventHandler(this.cb_type_SelectedIndexChanged);
			// 
			// cb_guide
			// 
			this.cb_guide.AutoSize = true;
			this.cb_guide.Location = new System.Drawing.Point(270, 153);
			this.cb_guide.Name = "cb_guide";
			this.cb_guide.Size = new System.Drawing.Size(15, 14);
			this.cb_guide.TabIndex = 15;
			this.cb_guide.UseVisualStyleBackColor = true;
			this.cb_guide.CheckedChanged += new System.EventHandler(this.cb_guide_CheckedChanged);
			// 
			// txt_guideWidth
			// 
			this.txt_guideWidth.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(170)))), ((int)(((byte)(170)))));
			this.txt_guideWidth.Enabled = false;
			this.txt_guideWidth.IsEnable = null;
			this.txt_guideWidth.Location = new System.Drawing.Point(92, 203);
			this.txt_guideWidth.Name = "txt_guideWidth";
			this.txt_guideWidth.ReadMethod = null;
			this.txt_guideWidth.Size = new System.Drawing.Size(56, 19);
			this.txt_guideWidth.TabIndex = 17;
			this.txt_guideWidth.WriteMethod = null;
			// 
			// DockViewPoint
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(361, 257);
			this.Controls.Add(this.tableLayoutPanel);
			this.DockAreas = ((WeifenLuo.WinFormsUI.Docking.DockAreas)(((((WeifenLuo.WinFormsUI.Docking.DockAreas.Float | WeifenLuo.WinFormsUI.Docking.DockAreas.DockLeft) 
            | WeifenLuo.WinFormsUI.Docking.DockAreas.DockRight) 
            | WeifenLuo.WinFormsUI.Docking.DockAreas.DockTop) 
            | WeifenLuo.WinFormsUI.Docking.DockAreas.DockBottom)));
			this.Font = new System.Drawing.Font("MS UI Gothic", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.Name = "DockViewPoint";
			this.Text = "視点操作";
			this.tableLayoutPanel.ResumeLayout(false);
			this.tableLayoutPanel.PerformLayout();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.TableLayoutPanel tableLayoutPanel;
		private System.Windows.Forms.Label lbl_focus;
		private System.Windows.Forms.Label lbl_angleX;
		private Component.DelegateFloatTextBox txt_focusZ;
		private Component.DelegateFloatTextBox txt_focusX;
		private Component.DelegateFloatTextBox txt_focusY;
		private Component.DelegateFloatTextBox txt_angleY;
		private Component.DelegateFloatTextBox txt_angleX;
		private System.Windows.Forms.Label lbl_angleY;
		private System.Windows.Forms.Label lbl_distance;
		private Component.DelegateFloatTextBox txt_distance;
		private System.Windows.Forms.Label lbl_type;
		private Component.DelegateFloatTextBox txt_mag;
		private System.Windows.Forms.Label lbl_mag;
		private System.Windows.Forms.ComboBox cb_type;
		private System.Windows.Forms.Label lbl_guide;
		private Component.DelegateIntTextBox txt_guideHeight;
		private System.Windows.Forms.Label lbl_guideHeight;
		private System.Windows.Forms.Label lbl_guideWidth;
		private System.Windows.Forms.CheckBox cb_guide;
		private Component.DelegateIntTextBox txt_guideWidth;
	}
}
