namespace Effekseer.GUI
{
	partial class DockRecorder
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
			this.lbl_isTranslucent = new System.Windows.Forms.Label();
			this.lbl_frameskip = new System.Windows.Forms.Label();
			this.txt_offset = new Effekseer.GUI.Component.DelegateIntTextBox();
			this.lbl_offset = new System.Windows.Forms.Label();
			this.lbl_yCount = new System.Windows.Forms.Label();
			this.lbl_xCount = new System.Windows.Forms.Label();
			this.txt_xCount = new Effekseer.GUI.Component.DelegateIntTextBox();
			this.txt_yCount = new Effekseer.GUI.Component.DelegateIntTextBox();
			this.txt_frameskip = new Effekseer.GUI.Component.DelegateIntTextBox();
			this.cb_isTranslucent = new System.Windows.Forms.CheckBox();
			this.btn_record = new System.Windows.Forms.Button();
			this.tableLayoutPanel.SuspendLayout();
			this.SuspendLayout();
			// 
			// tableLayoutPanel
			// 
			this.tableLayoutPanel.ColumnCount = 2;
			this.tableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
			this.tableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
			this.tableLayoutPanel.Controls.Add(this.lbl_isTranslucent, 0, 4);
			this.tableLayoutPanel.Controls.Add(this.lbl_frameskip, 0, 3);
			this.tableLayoutPanel.Controls.Add(this.txt_offset, 1, 2);
			this.tableLayoutPanel.Controls.Add(this.lbl_offset, 0, 2);
			this.tableLayoutPanel.Controls.Add(this.lbl_yCount, 0, 1);
			this.tableLayoutPanel.Controls.Add(this.lbl_xCount, 0, 0);
			this.tableLayoutPanel.Controls.Add(this.txt_xCount, 1, 0);
			this.tableLayoutPanel.Controls.Add(this.txt_yCount, 1, 1);
			this.tableLayoutPanel.Controls.Add(this.txt_frameskip, 1, 3);
			this.tableLayoutPanel.Controls.Add(this.cb_isTranslucent, 1, 4);
			this.tableLayoutPanel.Controls.Add(this.btn_record, 1, 5);
			this.tableLayoutPanel.Location = new System.Drawing.Point(2, 2);
			this.tableLayoutPanel.Name = "tableLayoutPanel";
			this.tableLayoutPanel.RowCount = 6;
			this.tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 16.66667F));
			this.tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 16.66667F));
			this.tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 16.66667F));
			this.tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 16.66667F));
			this.tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 16.66667F));
			this.tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 16.66667F));
			this.tableLayoutPanel.Size = new System.Drawing.Size(280, 258);
			this.tableLayoutPanel.TabIndex = 0;
			// 
			// lbl_isTranslucent
			// 
			this.lbl_isTranslucent.AutoSize = true;
			this.lbl_isTranslucent.Location = new System.Drawing.Point(3, 174);
			this.lbl_isTranslucent.Margin = new System.Windows.Forms.Padding(3, 6, 3, 0);
			this.lbl_isTranslucent.Name = "lbl_isTranslucent";
			this.lbl_isTranslucent.Size = new System.Drawing.Size(75, 12);
			this.lbl_isTranslucent.TabIndex = 9;
			this.lbl_isTranslucent.Text = "背景色が透明";
			// 
			// lbl_frameskip
			// 
			this.lbl_frameskip.AutoSize = true;
			this.lbl_frameskip.Location = new System.Drawing.Point(3, 132);
			this.lbl_frameskip.Margin = new System.Windows.Forms.Padding(3, 6, 3, 0);
			this.lbl_frameskip.Name = "lbl_frameskip";
			this.lbl_frameskip.Size = new System.Drawing.Size(77, 12);
			this.lbl_frameskip.TabIndex = 7;
			this.lbl_frameskip.Text = "フレームスキップ";
			// 
			// txt_offset
			// 
			this.txt_offset.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(170)))), ((int)(((byte)(170)))));
			this.txt_offset.Enabled = false;
			this.txt_offset.IsEnable = null;
			this.txt_offset.Location = new System.Drawing.Point(143, 87);
			this.txt_offset.Name = "txt_offset";
			this.txt_offset.ReadMethod = null;
			this.txt_offset.Size = new System.Drawing.Size(100, 19);
			this.txt_offset.TabIndex = 6;
			this.txt_offset.WheelStep = 1;
			this.txt_offset.WriteMethod = null;
			// 
			// lbl_offset
			// 
			this.lbl_offset.AutoSize = true;
			this.lbl_offset.Location = new System.Drawing.Point(3, 90);
			this.lbl_offset.Margin = new System.Windows.Forms.Padding(3, 6, 3, 0);
			this.lbl_offset.Name = "lbl_offset";
			this.lbl_offset.Size = new System.Drawing.Size(66, 12);
			this.lbl_offset.TabIndex = 5;
			this.lbl_offset.Text = "開始フレーム";
			// 
			// lbl_yCount
			// 
			this.lbl_yCount.AutoSize = true;
			this.lbl_yCount.Location = new System.Drawing.Point(3, 48);
			this.lbl_yCount.Margin = new System.Windows.Forms.Padding(3, 6, 3, 0);
			this.lbl_yCount.Name = "lbl_yCount";
			this.lbl_yCount.Size = new System.Drawing.Size(85, 12);
			this.lbl_yCount.TabIndex = 4;
			this.lbl_yCount.Text = "縦方向への枚数";
			// 
			// lbl_xCount
			// 
			this.lbl_xCount.AutoSize = true;
			this.lbl_xCount.Location = new System.Drawing.Point(3, 6);
			this.lbl_xCount.Margin = new System.Windows.Forms.Padding(3, 6, 3, 0);
			this.lbl_xCount.Name = "lbl_xCount";
			this.lbl_xCount.Size = new System.Drawing.Size(85, 12);
			this.lbl_xCount.TabIndex = 1;
			this.lbl_xCount.Text = "横方向への枚数";
			// 
			// txt_xCount
			// 
			this.txt_xCount.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(170)))), ((int)(((byte)(170)))));
			this.txt_xCount.Enabled = false;
			this.txt_xCount.IsEnable = null;
			this.txt_xCount.Location = new System.Drawing.Point(143, 3);
			this.txt_xCount.Name = "txt_xCount";
			this.txt_xCount.ReadMethod = null;
			this.txt_xCount.Size = new System.Drawing.Size(100, 19);
			this.txt_xCount.TabIndex = 2;
			this.txt_xCount.WheelStep = 1;
			this.txt_xCount.WriteMethod = null;
			// 
			// txt_yCount
			// 
			this.txt_yCount.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(170)))), ((int)(((byte)(170)))));
			this.txt_yCount.Enabled = false;
			this.txt_yCount.IsEnable = null;
			this.txt_yCount.Location = new System.Drawing.Point(143, 45);
			this.txt_yCount.Name = "txt_yCount";
			this.txt_yCount.ReadMethod = null;
			this.txt_yCount.Size = new System.Drawing.Size(100, 19);
			this.txt_yCount.TabIndex = 3;
			this.txt_yCount.WheelStep = 1;
			this.txt_yCount.WriteMethod = null;
			// 
			// txt_frameskip
			// 
			this.txt_frameskip.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(170)))), ((int)(((byte)(170)))));
			this.txt_frameskip.Enabled = false;
			this.txt_frameskip.IsEnable = null;
			this.txt_frameskip.Location = new System.Drawing.Point(143, 129);
			this.txt_frameskip.Name = "txt_frameskip";
			this.txt_frameskip.ReadMethod = null;
			this.txt_frameskip.Size = new System.Drawing.Size(100, 19);
			this.txt_frameskip.TabIndex = 8;
			this.txt_frameskip.WheelStep = 1;
			this.txt_frameskip.WriteMethod = null;
			// 
			// cb_isTranslucent
			// 
			this.cb_isTranslucent.AutoSize = true;
			this.cb_isTranslucent.Location = new System.Drawing.Point(143, 171);
			this.cb_isTranslucent.Name = "cb_isTranslucent";
			this.cb_isTranslucent.Size = new System.Drawing.Size(15, 14);
			this.cb_isTranslucent.TabIndex = 10;
			this.cb_isTranslucent.UseVisualStyleBackColor = true;
			// 
			// btn_record
			// 
			this.btn_record.Location = new System.Drawing.Point(143, 213);
			this.btn_record.Name = "btn_record";
			this.btn_record.Size = new System.Drawing.Size(75, 23);
			this.btn_record.TabIndex = 11;
			this.btn_record.Text = "録画";
			this.btn_record.UseVisualStyleBackColor = true;
			this.btn_record.Click += new System.EventHandler(this.btn_record_Click);
			// 
			// DockRecorder
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(284, 262);
			this.Controls.Add(this.tableLayoutPanel);
			this.Font = new System.Drawing.Font("MS UI Gothic", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.Name = "DockRecorder";
			this.tableLayoutPanel.ResumeLayout(false);
			this.tableLayoutPanel.PerformLayout();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.TableLayoutPanel tableLayoutPanel;
		private System.Windows.Forms.Label lbl_xCount;
		private System.Windows.Forms.Label lbl_yCount;
		private Component.DelegateIntTextBox txt_xCount;
		private Component.DelegateIntTextBox txt_yCount;
		private Component.DelegateIntTextBox txt_offset;
		private System.Windows.Forms.Label lbl_offset;
		private System.Windows.Forms.Label lbl_isTranslucent;
		private System.Windows.Forms.Label lbl_frameskip;
		private Component.DelegateIntTextBox txt_frameskip;
		private System.Windows.Forms.CheckBox cb_isTranslucent;
		private System.Windows.Forms.Button btn_record;
	}
}
