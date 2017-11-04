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
            this.grp_option = new System.Windows.Forms.GroupBox();
            this.cb_tranceparence = new System.Windows.Forms.ComboBox();
            this.lbl_translucent = new System.Windows.Forms.Label();
            this.grp_type = new System.Windows.Forms.GroupBox();
            this.txt_number_v = new Effekseer.GUI.Component.DelegateIntTextBox();
            this.lbl_the_number_of_image_h = new System.Windows.Forms.Label();
            this.cb_type = new System.Windows.Forms.ComboBox();
            this.grp_frame = new System.Windows.Forms.GroupBox();
            this.txt_freq = new Effekseer.GUI.Component.DelegateIntTextBox();
            this.lbl_freq = new System.Windows.Forms.Label();
            this.txt_endingFrame = new Effekseer.GUI.Component.DelegateIntTextBox();
            this.lbl_ending = new System.Windows.Forms.Label();
            this.txt_startingFrame = new Effekseer.GUI.Component.DelegateIntTextBox();
            this.lbl_starting = new System.Windows.Forms.Label();
            this.grp_area = new System.Windows.Forms.GroupBox();
            this.cb_area = new System.Windows.Forms.CheckBox();
            this.lbl_showArea = new System.Windows.Forms.Label();
            this.txt_areaHeight = new Effekseer.GUI.Component.DelegateIntTextBox();
            this.lbl_h = new System.Windows.Forms.Label();
            this.lbl_w = new System.Windows.Forms.Label();
            this.txt_areaWidth = new Effekseer.GUI.Component.DelegateIntTextBox();
            this.btn_record = new System.Windows.Forms.Button();
            this.grp_option.SuspendLayout();
            this.grp_type.SuspendLayout();
            this.grp_frame.SuspendLayout();
            this.grp_area.SuspendLayout();
            this.SuspendLayout();
            // 
            // grp_option
            // 
            this.grp_option.Controls.Add(this.cb_tranceparence);
            this.grp_option.Controls.Add(this.lbl_translucent);
            this.grp_option.Location = new System.Drawing.Point(12, 258);
            this.grp_option.Name = "grp_option";
            this.grp_option.Size = new System.Drawing.Size(200, 48);
            this.grp_option.TabIndex = 51;
            this.grp_option.TabStop = false;
			this.grp_option.Text = Properties.Resources.Options;
            // 
            // cb_tranceparence
            // 
            this.cb_tranceparence.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cb_tranceparence.FormattingEnabled = true;
            this.cb_tranceparence.Items.AddRange(new object[] {
            global::Effekseer.Properties.Resources.None,
            global::Effekseer.Properties.Resources.UseOriginalImage,
            global::Effekseer.Properties.Resources.GenerateAlpha});
            this.cb_tranceparence.Location = new System.Drawing.Point(111, 18);
            this.cb_tranceparence.Name = "cb_tranceparence";
            this.cb_tranceparence.Size = new System.Drawing.Size(83, 20);
            this.cb_tranceparence.TabIndex = 61;
            // 
            // lbl_translucent
            // 
            this.lbl_translucent.AutoSize = true;
            this.lbl_translucent.Location = new System.Drawing.Point(6, 21);
            this.lbl_translucent.Margin = new System.Windows.Forms.Padding(3, 6, 3, 0);
            this.lbl_translucent.Name = "lbl_translucent";
            this.lbl_translucent.Size = new System.Drawing.Size(99, 12);
            this.lbl_translucent.TabIndex = 60;
			this.lbl_translucent.Text = Properties.Resources.MakeTransparent;
            // 
            // grp_type
            // 
            this.grp_type.Controls.Add(this.txt_number_v);
            this.grp_type.Controls.Add(this.lbl_the_number_of_image_h);
            this.grp_type.Controls.Add(this.cb_type);
            this.grp_type.Location = new System.Drawing.Point(12, 183);
            this.grp_type.Name = "grp_type";
            this.grp_type.Size = new System.Drawing.Size(200, 69);
            this.grp_type.TabIndex = 3;
            this.grp_type.TabStop = false;
			this.grp_type.Text = Properties.Resources.Format;
            // 
            // txt_number_v
            // 
            this.txt_number_v.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(170)))), ((int)(((byte)(170)))));
            this.txt_number_v.Enabled = false;
            this.txt_number_v.IsEnable = null;
            this.txt_number_v.Location = new System.Drawing.Point(122, 44);
            this.txt_number_v.Name = "txt_number_v";
            this.txt_number_v.ReadMethod = null;
            this.txt_number_v.Size = new System.Drawing.Size(40, 19);
            this.txt_number_v.TabIndex = 31;
            this.txt_number_v.WheelStep = 1;
            this.txt_number_v.WriteMethod = null;
            // 
            // lbl_the_number_of_image_h
            // 
            this.lbl_the_number_of_image_h.AutoSize = true;
            this.lbl_the_number_of_image_h.Location = new System.Drawing.Point(7, 47);
            this.lbl_the_number_of_image_h.Name = "lbl_the_number_of_image_h";
            this.lbl_the_number_of_image_h.Size = new System.Drawing.Size(65, 12);
            this.lbl_the_number_of_image_h.TabIndex = 31;
			this.lbl_the_number_of_image_h.Text = Properties.Resources.XCount;
            // 
            // cb_type
            // 
            this.cb_type.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cb_type.FormattingEnabled = true;
            this.cb_type.Items.AddRange(new object[] {
            global::Effekseer.Properties.Resources.ExportAsSingleImage,
            global::Effekseer.Properties.Resources.ExportAsImages,
            global::Effekseer.Properties.Resources.ExportAsGifAnimation,
            global::Effekseer.Properties.Resources.ExportAsAvi});
            this.cb_type.Location = new System.Drawing.Point(8, 18);
            this.cb_type.Name = "cb_type";
            this.cb_type.Size = new System.Drawing.Size(154, 20);
            this.cb_type.TabIndex = 30;
            this.cb_type.SelectedIndexChanged += new System.EventHandler(this.cb_type_SelectedIndexChanged);
            // 
            // grp_frame
            // 
            this.grp_frame.Controls.Add(this.txt_freq);
            this.grp_frame.Controls.Add(this.lbl_freq);
            this.grp_frame.Controls.Add(this.txt_endingFrame);
            this.grp_frame.Controls.Add(this.lbl_ending);
            this.grp_frame.Controls.Add(this.txt_startingFrame);
            this.grp_frame.Controls.Add(this.lbl_starting);
            this.grp_frame.Location = new System.Drawing.Point(12, 92);
            this.grp_frame.Name = "grp_frame";
            this.grp_frame.Size = new System.Drawing.Size(200, 85);
            this.grp_frame.TabIndex = 2;
            this.grp_frame.TabStop = false;
			this.grp_frame.Text = Properties.Resources.ExportedFrame;
            // 
            // txt_freq
            // 
            this.txt_freq.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(170)))), ((int)(((byte)(170)))));
            this.txt_freq.Enabled = false;
            this.txt_freq.IsEnable = null;
            this.txt_freq.Location = new System.Drawing.Point(122, 61);
            this.txt_freq.Name = "txt_freq";
            this.txt_freq.ReadMethod = null;
            this.txt_freq.Size = new System.Drawing.Size(40, 19);
            this.txt_freq.TabIndex = 22;
            this.txt_freq.WheelStep = 1;
            this.txt_freq.WriteMethod = null;
            // 
            // lbl_freq
            // 
            this.lbl_freq.AutoSize = true;
            this.lbl_freq.Location = new System.Drawing.Point(6, 64);
            this.lbl_freq.Name = "lbl_freq";
            this.lbl_freq.Size = new System.Drawing.Size(110, 12);
            this.lbl_freq.TabIndex = 29;
			this.lbl_freq.Text = Properties.Resources.Frequency_Frame;
            // 
            // txt_endingFrame
            // 
            this.txt_endingFrame.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(170)))), ((int)(((byte)(170)))));
            this.txt_endingFrame.Enabled = false;
            this.txt_endingFrame.IsEnable = null;
            this.txt_endingFrame.Location = new System.Drawing.Point(122, 36);
            this.txt_endingFrame.Name = "txt_endingFrame";
            this.txt_endingFrame.ReadMethod = null;
            this.txt_endingFrame.Size = new System.Drawing.Size(40, 19);
            this.txt_endingFrame.TabIndex = 21;
            this.txt_endingFrame.WheelStep = 1;
            this.txt_endingFrame.WriteMethod = null;
            // 
            // lbl_ending
            // 
            this.lbl_ending.AutoSize = true;
            this.lbl_ending.Location = new System.Drawing.Point(6, 39);
            this.lbl_ending.Name = "lbl_ending";
            this.lbl_ending.Size = new System.Drawing.Size(66, 12);
            this.lbl_ending.TabIndex = 27;
			this.lbl_ending.Text = Properties.Resources.EndFrame;
            // 
            // txt_startingFrame
            // 
            this.txt_startingFrame.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(170)))), ((int)(((byte)(170)))));
            this.txt_startingFrame.Enabled = false;
            this.txt_startingFrame.IsEnable = null;
            this.txt_startingFrame.Location = new System.Drawing.Point(122, 12);
            this.txt_startingFrame.Name = "txt_startingFrame";
            this.txt_startingFrame.ReadMethod = null;
            this.txt_startingFrame.Size = new System.Drawing.Size(40, 19);
            this.txt_startingFrame.TabIndex = 20;
            this.txt_startingFrame.WheelStep = 1;
            this.txt_startingFrame.WriteMethod = null;
            // 
            // lbl_starting
            // 
            this.lbl_starting.AutoSize = true;
            this.lbl_starting.Location = new System.Drawing.Point(6, 15);
            this.lbl_starting.Name = "lbl_starting";
            this.lbl_starting.Size = new System.Drawing.Size(66, 12);
            this.lbl_starting.TabIndex = 26;
			this.lbl_starting.Text = Properties.Resources.StartFrame;
            // 
            // grp_area
            // 
            this.grp_area.Controls.Add(this.cb_area);
            this.grp_area.Controls.Add(this.lbl_showArea);
            this.grp_area.Controls.Add(this.txt_areaHeight);
            this.grp_area.Controls.Add(this.lbl_h);
            this.grp_area.Controls.Add(this.lbl_w);
            this.grp_area.Controls.Add(this.txt_areaWidth);
            this.grp_area.Location = new System.Drawing.Point(12, 12);
            this.grp_area.Name = "grp_area";
            this.grp_area.Size = new System.Drawing.Size(200, 74);
            this.grp_area.TabIndex = 1;
            this.grp_area.TabStop = false;
			this.grp_area.Text = Properties.Resources.Resolution;
            // 
            // cb_area
            // 
            this.cb_area.AutoSize = true;
            this.cb_area.Location = new System.Drawing.Point(122, 50);
            this.cb_area.Name = "cb_area";
            this.cb_area.Size = new System.Drawing.Size(15, 14);
            this.cb_area.TabIndex = 12;
            this.cb_area.UseVisualStyleBackColor = true;
            this.cb_area.CheckedChanged += new System.EventHandler(this.cb_guide_CheckedChanged);
            // 
            // lbl_showArea
            // 
            this.lbl_showArea.AutoSize = true;
            this.lbl_showArea.Location = new System.Drawing.Point(6, 50);
            this.lbl_showArea.Name = "lbl_showArea";
            this.lbl_showArea.Size = new System.Drawing.Size(86, 12);
            this.lbl_showArea.TabIndex = 25;
			this.lbl_showArea.Text = Properties.Resources.ShowGuide;
            // 
            // txt_areaHeight
            // 
            this.txt_areaHeight.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(170)))), ((int)(((byte)(170)))));
            this.txt_areaHeight.Enabled = false;
            this.txt_areaHeight.IsEnable = null;
            this.txt_areaHeight.Location = new System.Drawing.Point(122, 19);
            this.txt_areaHeight.Name = "txt_areaHeight";
            this.txt_areaHeight.ReadMethod = null;
            this.txt_areaHeight.Size = new System.Drawing.Size(40, 19);
            this.txt_areaHeight.TabIndex = 11;
            this.txt_areaHeight.WheelStep = 1;
            this.txt_areaHeight.WriteMethod = null;
            // 
            // lbl_h
            // 
            this.lbl_h.AutoSize = true;
            this.lbl_h.Location = new System.Drawing.Point(87, 22);
            this.lbl_h.Name = "lbl_h";
            this.lbl_h.Size = new System.Drawing.Size(29, 12);
            this.lbl_h.TabIndex = 24;
			this.lbl_h.Text = Properties.Resources.Height;
            // 
            // lbl_w
            // 
            this.lbl_w.AutoSize = true;
            this.lbl_w.Location = new System.Drawing.Point(6, 22);
            this.lbl_w.Name = "lbl_w";
            this.lbl_w.Size = new System.Drawing.Size(29, 12);
            this.lbl_w.TabIndex = 23;
			this.lbl_w.Text = Properties.Resources.Width;
            // 
            // txt_areaWidth
            // 
            this.txt_areaWidth.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(170)))), ((int)(((byte)(170)))));
            this.txt_areaWidth.Enabled = false;
            this.txt_areaWidth.IsEnable = null;
            this.txt_areaWidth.Location = new System.Drawing.Point(41, 19);
            this.txt_areaWidth.Name = "txt_areaWidth";
            this.txt_areaWidth.ReadMethod = null;
            this.txt_areaWidth.Size = new System.Drawing.Size(40, 19);
            this.txt_areaWidth.TabIndex = 10;
            this.txt_areaWidth.WheelStep = 1;
            this.txt_areaWidth.WriteMethod = null;
            // 
            // btn_record
            // 
            this.btn_record.Location = new System.Drawing.Point(12, 312);
            this.btn_record.Name = "btn_record";
            this.btn_record.Size = new System.Drawing.Size(75, 23);
            this.btn_record.TabIndex = 50;
            this.btn_record.Text = global::Effekseer.Properties.Resources.Record;
            this.btn_record.UseVisualStyleBackColor = true;
            this.btn_record.Click += new System.EventHandler(this.btn_record_Click);
            // 
            // DockRecorder
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(647, 468);
            this.Controls.Add(this.grp_option);
            this.Controls.Add(this.grp_type);
            this.Controls.Add(this.grp_frame);
            this.Controls.Add(this.grp_area);
            this.Controls.Add(this.btn_record);
            this.Font = new System.Drawing.Font("MS UI Gothic", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
            this.Name = "DockRecorder";
            this.grp_option.ResumeLayout(false);
            this.grp_option.PerformLayout();
            this.grp_type.ResumeLayout(false);
            this.grp_type.PerformLayout();
            this.grp_frame.ResumeLayout(false);
            this.grp_frame.PerformLayout();
            this.grp_area.ResumeLayout(false);
            this.grp_area.PerformLayout();
            this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.Label lbl_translucent;
		private System.Windows.Forms.Button btn_record;
		private System.Windows.Forms.GroupBox grp_area;
		private System.Windows.Forms.CheckBox cb_area;
		private System.Windows.Forms.Label lbl_showArea;
		private Component.DelegateIntTextBox txt_areaHeight;
		private System.Windows.Forms.Label lbl_h;
		private System.Windows.Forms.Label lbl_w;
		private Component.DelegateIntTextBox txt_areaWidth;
		private System.Windows.Forms.GroupBox grp_frame;
		private Component.DelegateIntTextBox txt_freq;
		private System.Windows.Forms.Label lbl_freq;
		private Component.DelegateIntTextBox txt_endingFrame;
		private System.Windows.Forms.Label lbl_ending;
		private Component.DelegateIntTextBox txt_startingFrame;
		private System.Windows.Forms.Label lbl_starting;
		private System.Windows.Forms.GroupBox grp_type;
		private System.Windows.Forms.ComboBox cb_type;
		private Component.DelegateIntTextBox txt_number_v;
		private System.Windows.Forms.Label lbl_the_number_of_image_h;
		private System.Windows.Forms.GroupBox grp_option;
		private System.Windows.Forms.ComboBox cb_tranceparence;
	}
}
