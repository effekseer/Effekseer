namespace Effekseer.GUI
{
	partial class DockViewerController
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
			this.btn_step = new System.Windows.Forms.Button();
			this.btn_pause = new System.Windows.Forms.Button();
			this.btn_stop = new System.Windows.Forms.Button();
			this.lb_current = new System.Windows.Forms.Label();
			this.lb_end = new System.Windows.Forms.Label();
			this.tb_end = new Effekseer.GUI.Component.DelegateIntTextBox();
			this.lb_current_value = new System.Windows.Forms.Label();
			this.lb_st = new System.Windows.Forms.Label();
			this.tb_start = new Effekseer.GUI.Component.DelegateIntTextBox();
			this.btn_play = new System.Windows.Forms.Button();
			this.trackBar = new System.Windows.Forms.TrackBar();
			this.cb_loop = new System.Windows.Forms.CheckBox();
			((System.ComponentModel.ISupportInitialize)(this.trackBar)).BeginInit();
			this.SuspendLayout();
			// 
			// btn_step
			// 
			this.btn_step.Location = new System.Drawing.Point(169, 46);
			this.btn_step.Name = "btn_step";
			this.btn_step.Size = new System.Drawing.Size(50, 23);
			this.btn_step.TabIndex = 4;
			this.btn_step.Text = "step";
			this.btn_step.UseVisualStyleBackColor = true;
			this.btn_step.Click += new System.EventHandler(this.btn_step_Click);
			// 
			// btn_pause
			// 
			this.btn_pause.Location = new System.Drawing.Point(113, 46);
			this.btn_pause.Name = "btn_pause";
			this.btn_pause.Size = new System.Drawing.Size(50, 23);
			this.btn_pause.TabIndex = 3;
			this.btn_pause.Text = "pause";
			this.btn_pause.UseVisualStyleBackColor = true;
			this.btn_pause.Click += new System.EventHandler(this.btn_pause_Click);
			// 
			// btn_stop
			// 
			this.btn_stop.Location = new System.Drawing.Point(57, 46);
			this.btn_stop.Name = "btn_stop";
			this.btn_stop.Size = new System.Drawing.Size(50, 23);
			this.btn_stop.TabIndex = 2;
			this.btn_stop.Text = "stop";
			this.btn_stop.UseVisualStyleBackColor = true;
			this.btn_stop.Click += new System.EventHandler(this.btn_stop_Click);
			// 
			// lb_current
			// 
			this.lb_current.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.lb_current.AutoSize = true;
			this.lb_current.Location = new System.Drawing.Point(220, 23);
			this.lb_current.Name = "lb_current";
			this.lb_current.Size = new System.Drawing.Size(29, 12);
			this.lb_current.TabIndex = 7;
			this.lb_current.Text = "現在";
			// 
			// lb_end
			// 
			this.lb_end.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.lb_end.AutoSize = true;
			this.lb_end.Location = new System.Drawing.Point(220, 41);
			this.lb_end.Name = "lb_end";
			this.lb_end.Size = new System.Drawing.Size(29, 12);
			this.lb_end.TabIndex = 6;
			this.lb_end.Text = "終了";
			// 
			// tb_end
			// 
			this.tb_end.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.tb_end.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(200)))), ((int)(((byte)(200)))));
			this.tb_end.Enabled = false;
			this.tb_end.Font = new System.Drawing.Font("MS UI Gothic", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.tb_end.IsEnable = null;
			this.tb_end.Location = new System.Drawing.Point(265, 38);
			this.tb_end.Name = "tb_end";
			this.tb_end.ReadMethod = null;
			this.tb_end.Size = new System.Drawing.Size(25, 19);
			this.tb_end.TabIndex = 6;
			this.tb_end.WriteMethod = null;
			// 
			// lb_current_value
			// 
			this.lb_current_value.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.lb_current_value.AutoSize = true;
			this.lb_current_value.Location = new System.Drawing.Point(267, 23);
			this.lb_current_value.Name = "lb_current_value";
			this.lb_current_value.Size = new System.Drawing.Size(23, 12);
			this.lb_current_value.TabIndex = 4;
			this.lb_current_value.Text = "123";
			// 
			// lb_st
			// 
			this.lb_st.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.lb_st.AutoSize = true;
			this.lb_st.Location = new System.Drawing.Point(220, 4);
			this.lb_st.Name = "lb_st";
			this.lb_st.Size = new System.Drawing.Size(29, 12);
			this.lb_st.TabIndex = 3;
			this.lb_st.Text = "開始";
			// 
			// tb_start
			// 
			this.tb_start.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.tb_start.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(200)))), ((int)(((byte)(200)))));
			this.tb_start.Enabled = false;
			this.tb_start.Font = new System.Drawing.Font("MS UI Gothic", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.tb_start.IsEnable = null;
			this.tb_start.Location = new System.Drawing.Point(265, 1);
			this.tb_start.Name = "tb_start";
			this.tb_start.ReadMethod = null;
			this.tb_start.Size = new System.Drawing.Size(25, 19);
			this.tb_start.TabIndex = 5;
			this.tb_start.WriteMethod = null;
			// 
			// btn_play
			// 
			this.btn_play.Location = new System.Drawing.Point(1, 46);
			this.btn_play.Name = "btn_play";
			this.btn_play.Size = new System.Drawing.Size(50, 23);
			this.btn_play.TabIndex = 1;
			this.btn_play.Text = "play";
			this.btn_play.UseVisualStyleBackColor = true;
			this.btn_play.Click += new System.EventHandler(this.btn_play_Click);
			// 
			// trackBar
			// 
			this.trackBar.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.trackBar.Location = new System.Drawing.Point(1, 1);
			this.trackBar.Margin = new System.Windows.Forms.Padding(0);
			this.trackBar.Name = "trackBar";
			this.trackBar.Size = new System.Drawing.Size(212, 45);
			this.trackBar.TabIndex = 0;
			this.trackBar.ValueChanged += new System.EventHandler(this.trackBar_ValueChanged);
			this.trackBar.CursorChanged += new System.EventHandler(this.trackBar_CursorChanged);
			// 
			// cb_loop
			// 
			this.cb_loop.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.cb_loop.AutoSize = true;
			this.cb_loop.Location = new System.Drawing.Point(237, 63);
			this.cb_loop.Name = "cb_loop";
			this.cb_loop.Size = new System.Drawing.Size(53, 16);
			this.cb_loop.TabIndex = 8;
			this.cb_loop.Text = "ループ";
			this.cb_loop.UseVisualStyleBackColor = true;
			this.cb_loop.CheckedChanged += new System.EventHandler(this.cb_loop_CheckedChanged);
			// 
			// DockViewerController
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(292, 92);
			this.Controls.Add(this.cb_loop);
			this.Controls.Add(this.btn_step);
			this.Controls.Add(this.btn_pause);
			this.Controls.Add(this.btn_stop);
			this.Controls.Add(this.lb_current);
			this.Controls.Add(this.lb_end);
			this.Controls.Add(this.tb_end);
			this.Controls.Add(this.lb_current_value);
			this.Controls.Add(this.lb_st);
			this.Controls.Add(this.tb_start);
			this.Controls.Add(this.btn_play);
			this.Controls.Add(this.trackBar);
			this.DockAreas = ((WeifenLuo.WinFormsUI.Docking.DockAreas)(((((WeifenLuo.WinFormsUI.Docking.DockAreas.Float | WeifenLuo.WinFormsUI.Docking.DockAreas.DockLeft) 
            | WeifenLuo.WinFormsUI.Docking.DockAreas.DockRight) 
            | WeifenLuo.WinFormsUI.Docking.DockAreas.DockTop) 
            | WeifenLuo.WinFormsUI.Docking.DockAreas.DockBottom)));
			this.Font = new System.Drawing.Font("MS UI Gothic", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.MinimumSize = new System.Drawing.Size(200, 100);
			this.Name = "DockViewerController";
			this.Text = "ビュワー操作";
			((System.ComponentModel.ISupportInitialize)(this.trackBar)).EndInit();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.TrackBar trackBar;
		private System.Windows.Forms.Button btn_play;
		private GUI.Component.DelegateIntTextBox tb_start;
		private System.Windows.Forms.Label lb_st;
		private System.Windows.Forms.Label lb_current_value;
		private GUI.Component.DelegateIntTextBox tb_end;
		private System.Windows.Forms.Label lb_end;
		private System.Windows.Forms.Label lb_current;
		private System.Windows.Forms.Button btn_stop;
		private System.Windows.Forms.Button btn_pause;
		private System.Windows.Forms.Button btn_step;
		private System.Windows.Forms.CheckBox cb_loop;
	}
}
