namespace Effekseer.GUI.Component
{
	partial class FCurves
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
			this.splitContainer = new System.Windows.Forms.SplitContainer();
			this.lbl_sampling = new System.Windows.Forms.Label();
			this.lbl_offset_min = new System.Windows.Forms.Label();
			this.lbl_offset_max = new System.Windows.Forms.Label();
			this.lbl_offset = new System.Windows.Forms.Label();
			this.lbl_end = new System.Windows.Forms.Label();
			this.lbl_start = new System.Windows.Forms.Label();
			this.lbl_type = new System.Windows.Forms.Label();
			this.lbl_value = new System.Windows.Forms.Label();
			this.lbl_frame = new System.Windows.Forms.Label();
			this.lbl_right_y = new System.Windows.Forms.Label();
			this.lbl_right_x = new System.Windows.Forms.Label();
			this.lbl_right = new System.Windows.Forms.Label();
			this.lbl_left_y = new System.Windows.Forms.Label();
			this.lbl_left_x = new System.Windows.Forms.Label();
			this.lbl_left = new System.Windows.Forms.Label();
			this.cb_fcurveEdgeEnd = new Effekseer.GUI.Component.Enum();
			this.cb_fcurveEdgeStart = new Effekseer.GUI.Component.Enum();
			this.cb_fcurveInterpolation = new Effekseer.GUI.Component.Enum();
			this.txt_offset_min = new Effekseer.GUI.Component.Float();
			this.txt_offset_max = new Effekseer.GUI.Component.Float();
			this.txt_sampling = new Effekseer.GUI.Component.Int();
			this.txt_frame = new Effekseer.GUI.Component.DelegateIntTextBox();
			this.txt_value = new Effekseer.GUI.Component.DelegateFloatTextBox();
			this.txt_right_y = new Effekseer.GUI.Component.DelegateFloatTextBox();
			this.txt_right_x = new Effekseer.GUI.Component.DelegateFloatTextBox();
			this.txt_left_y = new Effekseer.GUI.Component.DelegateFloatTextBox();
			this.txt_left_x = new Effekseer.GUI.Component.DelegateFloatTextBox();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer)).BeginInit();
			this.splitContainer.SuspendLayout();
			this.SuspendLayout();
			// 
			// splitContainer
			// 
			this.splitContainer.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.splitContainer.Location = new System.Drawing.Point(0, 53);
			this.splitContainer.Name = "splitContainer";
			// 
			// splitContainer.Panel1
			// 
			this.splitContainer.Panel1.BackColor = System.Drawing.SystemColors.ControlDark;
			this.splitContainer.Panel1.Paint += new System.Windows.Forms.PaintEventHandler(this.splitContainer1_Panel1_Paint);
			this.splitContainer.Panel1.MouseDown += new System.Windows.Forms.MouseEventHandler(this.splitContainer_Panel1_MouseDown);
			// 
			// splitContainer.Panel2
			// 
			this.splitContainer.Panel2.AutoScroll = true;
			this.splitContainer.Panel2.BackColor = System.Drawing.SystemColors.Control;
			this.splitContainer.Panel2.Scroll += new System.Windows.Forms.ScrollEventHandler(this.splitContainer_Panel2_Scroll);
			this.splitContainer.Panel2.SizeChanged += new System.EventHandler(this.splitContainer_Panel2_SizeChanged);
			this.splitContainer.Size = new System.Drawing.Size(1062, 362);
			this.splitContainer.SplitterDistance = 242;
			this.splitContainer.TabIndex = 2;
			// 
			// lbl_sampling
			// 
			this.lbl_sampling.AutoSize = true;
			this.lbl_sampling.Location = new System.Drawing.Point(545, 7);
			this.lbl_sampling.Name = "lbl_sampling";
			this.lbl_sampling.Size = new System.Drawing.Size(58, 12);
			this.lbl_sampling.TabIndex = 26;
			this.lbl_sampling.Text = "サンプリング";
			// 
			// lbl_offset_min
			// 
			this.lbl_offset_min.AutoSize = true;
			this.lbl_offset_min.Location = new System.Drawing.Point(570, 31);
			this.lbl_offset_min.Name = "lbl_offset_min";
			this.lbl_offset_min.Size = new System.Drawing.Size(29, 12);
			this.lbl_offset_min.TabIndex = 20;
			this.lbl_offset_min.Text = "最小";
			// 
			// lbl_offset_max
			// 
			this.lbl_offset_max.AutoSize = true;
			this.lbl_offset_max.Location = new System.Drawing.Point(477, 31);
			this.lbl_offset_max.Name = "lbl_offset_max";
			this.lbl_offset_max.Size = new System.Drawing.Size(29, 12);
			this.lbl_offset_max.TabIndex = 19;
			this.lbl_offset_max.Text = "最大";
			// 
			// lbl_offset
			// 
			this.lbl_offset.AutoSize = true;
			this.lbl_offset.Location = new System.Drawing.Point(424, 31);
			this.lbl_offset.Name = "lbl_offset";
			this.lbl_offset.Size = new System.Drawing.Size(47, 12);
			this.lbl_offset.TabIndex = 18;
			this.lbl_offset.Text = "オフセット";
			// 
			// lbl_end
			// 
			this.lbl_end.AutoSize = true;
			this.lbl_end.Location = new System.Drawing.Point(327, 6);
			this.lbl_end.Name = "lbl_end";
			this.lbl_end.Size = new System.Drawing.Size(29, 12);
			this.lbl_end.TabIndex = 16;
			this.lbl_end.Text = "終了";
			// 
			// lbl_start
			// 
			this.lbl_start.AutoSize = true;
			this.lbl_start.Location = new System.Drawing.Point(221, 6);
			this.lbl_start.Name = "lbl_start";
			this.lbl_start.Size = new System.Drawing.Size(29, 12);
			this.lbl_start.TabIndex = 15;
			this.lbl_start.Text = "開始";
			// 
			// lbl_type
			// 
			this.lbl_type.AutoSize = true;
			this.lbl_type.Location = new System.Drawing.Point(434, 6);
			this.lbl_type.Name = "lbl_type";
			this.lbl_type.Size = new System.Drawing.Size(29, 12);
			this.lbl_type.TabIndex = 14;
			this.lbl_type.Text = "補完";
			// 
			// lbl_value
			// 
			this.lbl_value.AutoSize = true;
			this.lbl_value.Location = new System.Drawing.Point(127, 6);
			this.lbl_value.Name = "lbl_value";
			this.lbl_value.Size = new System.Drawing.Size(17, 12);
			this.lbl_value.TabIndex = 13;
			this.lbl_value.Text = "値";
			// 
			// lbl_frame
			// 
			this.lbl_frame.AutoSize = true;
			this.lbl_frame.Location = new System.Drawing.Point(7, 6);
			this.lbl_frame.Name = "lbl_frame";
			this.lbl_frame.Size = new System.Drawing.Size(42, 12);
			this.lbl_frame.TabIndex = 12;
			this.lbl_frame.Text = "フレーム";
			// 
			// lbl_right_y
			// 
			this.lbl_right_y.AutoSize = true;
			this.lbl_right_y.Location = new System.Drawing.Point(312, 31);
			this.lbl_right_y.Name = "lbl_right_y";
			this.lbl_right_y.Size = new System.Drawing.Size(11, 12);
			this.lbl_right_y.TabIndex = 11;
			this.lbl_right_y.Text = "y";
			// 
			// lbl_right_x
			// 
			this.lbl_right_x.AutoSize = true;
			this.lbl_right_x.Location = new System.Drawing.Point(239, 31);
			this.lbl_right_x.Name = "lbl_right_x";
			this.lbl_right_x.Size = new System.Drawing.Size(11, 12);
			this.lbl_right_x.TabIndex = 10;
			this.lbl_right_x.Text = "x";
			// 
			// lbl_right
			// 
			this.lbl_right.AutoSize = true;
			this.lbl_right.Location = new System.Drawing.Point(216, 31);
			this.lbl_right.Name = "lbl_right";
			this.lbl_right.Size = new System.Drawing.Size(17, 12);
			this.lbl_right.TabIndex = 9;
			this.lbl_right.Text = "右";
			// 
			// lbl_left_y
			// 
			this.lbl_left_y.AutoSize = true;
			this.lbl_left_y.Location = new System.Drawing.Point(113, 31);
			this.lbl_left_y.Name = "lbl_left_y";
			this.lbl_left_y.Size = new System.Drawing.Size(11, 12);
			this.lbl_left_y.TabIndex = 8;
			this.lbl_left_y.Text = "y";
			// 
			// lbl_left_x
			// 
			this.lbl_left_x.AutoSize = true;
			this.lbl_left_x.Location = new System.Drawing.Point(38, 31);
			this.lbl_left_x.Name = "lbl_left_x";
			this.lbl_left_x.Size = new System.Drawing.Size(11, 12);
			this.lbl_left_x.TabIndex = 7;
			this.lbl_left_x.Text = "x";
			// 
			// lbl_left
			// 
			this.lbl_left.AutoSize = true;
			this.lbl_left.Location = new System.Drawing.Point(15, 31);
			this.lbl_left.Name = "lbl_left";
			this.lbl_left.Size = new System.Drawing.Size(17, 12);
			this.lbl_left.TabIndex = 6;
			this.lbl_left.Text = "左";
			// 
			// cb_fcurveEdgeEnd
			// 
			this.cb_fcurveEdgeEnd.Binding = null;
			this.cb_fcurveEdgeEnd.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cb_fcurveEdgeEnd.Enabled = false;
			this.cb_fcurveEdgeEnd.EnableUndo = true;
			this.cb_fcurveEdgeEnd.FormattingEnabled = true;
			this.cb_fcurveEdgeEnd.Location = new System.Drawing.Point(362, 3);
			this.cb_fcurveEdgeEnd.Name = "cb_fcurveEdgeEnd";
			this.cb_fcurveEdgeEnd.Size = new System.Drawing.Size(50, 20);
			this.cb_fcurveEdgeEnd.TabIndex = 13;
			// 
			// cb_fcurveEdgeStart
			// 
			this.cb_fcurveEdgeStart.Binding = null;
			this.cb_fcurveEdgeStart.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cb_fcurveEdgeStart.Enabled = false;
			this.cb_fcurveEdgeStart.EnableUndo = true;
			this.cb_fcurveEdgeStart.FormattingEnabled = true;
			this.cb_fcurveEdgeStart.Location = new System.Drawing.Point(256, 3);
			this.cb_fcurveEdgeStart.Name = "cb_fcurveEdgeStart";
			this.cb_fcurveEdgeStart.Size = new System.Drawing.Size(50, 20);
			this.cb_fcurveEdgeStart.TabIndex = 12;
			// 
			// cb_fcurveInterpolation
			// 
			this.cb_fcurveInterpolation.Binding = null;
			this.cb_fcurveInterpolation.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cb_fcurveInterpolation.Enabled = false;
			this.cb_fcurveInterpolation.EnableUndo = true;
			this.cb_fcurveInterpolation.FormattingEnabled = true;
			this.cb_fcurveInterpolation.Location = new System.Drawing.Point(469, 3);
			this.cb_fcurveInterpolation.Name = "cb_fcurveInterpolation";
			this.cb_fcurveInterpolation.Size = new System.Drawing.Size(50, 20);
			this.cb_fcurveInterpolation.TabIndex = 14;
			// 
			// txt_offset_min
			// 
			this.txt_offset_min.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(170)))), ((int)(((byte)(200)))), ((int)(((byte)(170)))));
			this.txt_offset_min.Binding = null;
			this.txt_offset_min.Enabled = false;
			this.txt_offset_min.EnableUndo = true;
			this.txt_offset_min.IsEnable = null;
			this.txt_offset_min.Location = new System.Drawing.Point(605, 28);
			this.txt_offset_min.Name = "txt_offset_min";
			this.txt_offset_min.Size = new System.Drawing.Size(50, 19);
			this.txt_offset_min.TabIndex = 31;
			// 
			// txt_offset_max
			// 
			this.txt_offset_max.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(170)))), ((int)(((byte)(200)))), ((int)(((byte)(170)))));
			this.txt_offset_max.Binding = null;
			this.txt_offset_max.Enabled = false;
			this.txt_offset_max.EnableUndo = true;
			this.txt_offset_max.IsEnable = null;
			this.txt_offset_max.Location = new System.Drawing.Point(512, 28);
			this.txt_offset_max.Name = "txt_offset_max";
			this.txt_offset_max.Size = new System.Drawing.Size(50, 19);
			this.txt_offset_max.TabIndex = 30;
			// 
			// txt_sampling
			// 
			this.txt_sampling.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(170)))), ((int)(((byte)(170)))));
			this.txt_sampling.Binding = null;
			this.txt_sampling.Enabled = false;
			this.txt_sampling.EnableUndo = true;
			this.txt_sampling.IsEnable = null;
			this.txt_sampling.Location = new System.Drawing.Point(605, 4);
			this.txt_sampling.Name = "txt_sampling";
			this.txt_sampling.Size = new System.Drawing.Size(50, 19);
			this.txt_sampling.TabIndex = 15;
			// 
			// txt_frame
			// 
			this.txt_frame.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(170)))), ((int)(((byte)(170)))));
			this.txt_frame.Enabled = false;
			this.txt_frame.IsEnable = null;
			this.txt_frame.Location = new System.Drawing.Point(55, 3);
			this.txt_frame.Name = "txt_frame";
			this.txt_frame.ReadMethod = null;
			this.txt_frame.Size = new System.Drawing.Size(50, 19);
			this.txt_frame.TabIndex = 10;
			this.txt_frame.WheelStep = 1;
			this.txt_frame.WriteMethod = null;
			// 
			// txt_value
			// 
			this.txt_value.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(170)))), ((int)(((byte)(200)))), ((int)(((byte)(170)))));
			this.txt_value.Enabled = false;
			this.txt_value.IsEnable = null;
			this.txt_value.Location = new System.Drawing.Point(150, 3);
			this.txt_value.Name = "txt_value";
			this.txt_value.ReadMethod = null;
			this.txt_value.Size = new System.Drawing.Size(50, 19);
			this.txt_value.TabIndex = 11;
			this.txt_value.WheelStep = 1F;
			this.txt_value.WriteMethod = null;
			// 
			// txt_right_y
			// 
			this.txt_right_y.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(170)))), ((int)(((byte)(200)))), ((int)(((byte)(170)))));
			this.txt_right_y.Enabled = false;
			this.txt_right_y.IsEnable = null;
			this.txt_right_y.Location = new System.Drawing.Point(329, 28);
			this.txt_right_y.Name = "txt_right_y";
			this.txt_right_y.ReadMethod = null;
			this.txt_right_y.Size = new System.Drawing.Size(50, 19);
			this.txt_right_y.TabIndex = 24;
			this.txt_right_y.WheelStep = 1F;
			this.txt_right_y.WriteMethod = null;
			// 
			// txt_right_x
			// 
			this.txt_right_x.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(170)))), ((int)(((byte)(200)))), ((int)(((byte)(170)))));
			this.txt_right_x.Enabled = false;
			this.txt_right_x.IsEnable = null;
			this.txt_right_x.Location = new System.Drawing.Point(256, 28);
			this.txt_right_x.Name = "txt_right_x";
			this.txt_right_x.ReadMethod = null;
			this.txt_right_x.Size = new System.Drawing.Size(50, 19);
			this.txt_right_x.TabIndex = 22;
			this.txt_right_x.WheelStep = 1F;
			this.txt_right_x.WriteMethod = null;
			// 
			// txt_left_y
			// 
			this.txt_left_y.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(170)))), ((int)(((byte)(200)))), ((int)(((byte)(170)))));
			this.txt_left_y.Enabled = false;
			this.txt_left_y.IsEnable = null;
			this.txt_left_y.Location = new System.Drawing.Point(129, 28);
			this.txt_left_y.Name = "txt_left_y";
			this.txt_left_y.ReadMethod = null;
			this.txt_left_y.Size = new System.Drawing.Size(50, 19);
			this.txt_left_y.TabIndex = 21;
			this.txt_left_y.WheelStep = 1F;
			this.txt_left_y.WriteMethod = null;
			// 
			// txt_left_x
			// 
			this.txt_left_x.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(170)))), ((int)(((byte)(200)))), ((int)(((byte)(170)))));
			this.txt_left_x.Enabled = false;
			this.txt_left_x.IsEnable = null;
			this.txt_left_x.Location = new System.Drawing.Point(55, 28);
			this.txt_left_x.Name = "txt_left_x";
			this.txt_left_x.ReadMethod = null;
			this.txt_left_x.Size = new System.Drawing.Size(50, 19);
			this.txt_left_x.TabIndex = 20;
			this.txt_left_x.WheelStep = 1F;
			this.txt_left_x.WriteMethod = null;
			// 
			// FCurves
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.lbl_start);
			this.Controls.Add(this.cb_fcurveEdgeEnd);
			this.Controls.Add(this.lbl_end);
			this.Controls.Add(this.cb_fcurveEdgeStart);
			this.Controls.Add(this.cb_fcurveInterpolation);
			this.Controls.Add(this.txt_offset_min);
			this.Controls.Add(this.txt_offset_max);
			this.Controls.Add(this.lbl_sampling);
			this.Controls.Add(this.lbl_offset_min);
			this.Controls.Add(this.txt_sampling);
			this.Controls.Add(this.lbl_offset_max);
			this.Controls.Add(this.splitContainer);
			this.Controls.Add(this.lbl_offset);
			this.Controls.Add(this.txt_frame);
			this.Controls.Add(this.lbl_frame);
			this.Controls.Add(this.lbl_right_y);
			this.Controls.Add(this.txt_value);
			this.Controls.Add(this.lbl_right_x);
			this.Controls.Add(this.lbl_value);
			this.Controls.Add(this.lbl_right);
			this.Controls.Add(this.lbl_type);
			this.Controls.Add(this.txt_right_y);
			this.Controls.Add(this.txt_right_x);
			this.Controls.Add(this.lbl_left_y);
			this.Controls.Add(this.lbl_left_x);
			this.Controls.Add(this.lbl_left);
			this.Controls.Add(this.txt_left_y);
			this.Controls.Add(this.txt_left_x);
			this.Name = "FCurves";
			this.Size = new System.Drawing.Size(1062, 415);
			this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.FCurves_MouseDown);
			this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.FCurves_MouseMove);
			this.MouseUp += new System.Windows.Forms.MouseEventHandler(this.FCurves_MouseUp);
			((System.ComponentModel.ISupportInitialize)(this.splitContainer)).EndInit();
			this.splitContainer.ResumeLayout(false);
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.SplitContainer splitContainer;
		private System.Windows.Forms.Label lbl_right_y;
		private System.Windows.Forms.Label lbl_right_x;
		private System.Windows.Forms.Label lbl_right;
		private System.Windows.Forms.Label lbl_left_y;
		private System.Windows.Forms.Label lbl_left_x;
		private System.Windows.Forms.Label lbl_left;
		private DelegateFloatTextBox txt_value;
		private DelegateFloatTextBox txt_right_y;
		private DelegateFloatTextBox txt_right_x;
		private DelegateFloatTextBox txt_left_x;
		private DelegateFloatTextBox txt_left_y;
		private System.Windows.Forms.Label lbl_value;
		private System.Windows.Forms.Label lbl_frame;
		private System.Windows.Forms.Label lbl_end;
		private System.Windows.Forms.Label lbl_start;
		private System.Windows.Forms.Label lbl_type;
		private DelegateIntTextBox txt_frame;
		private System.Windows.Forms.Label lbl_offset_min;
		private System.Windows.Forms.Label lbl_offset_max;
		private System.Windows.Forms.Label lbl_offset;
		private Float txt_offset_min;
		private Float txt_offset_max;
		private System.Windows.Forms.Label lbl_sampling;
		private Int txt_sampling;
		private Enum cb_fcurveInterpolation;
		private Enum cb_fcurveEdgeStart;
		private Enum cb_fcurveEdgeEnd;
	}
}
