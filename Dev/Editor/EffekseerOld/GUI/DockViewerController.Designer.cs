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
			this.btn_stop = new System.Windows.Forms.Button();
			this.lb_end = new System.Windows.Forms.Label();
			this.tb_end = new Effekseer.GUI.Component.DelegateIntTextBox();
			this.lb_current_value = new System.Windows.Forms.Label();
			this.lb_st = new System.Windows.Forms.Label();
			this.tb_start = new Effekseer.GUI.Component.DelegateIntTextBox();
			this.btn_play_pause = new System.Windows.Forms.Button();
			this.trackBar = new System.Windows.Forms.TrackBar();
			this.btn_backstep = new System.Windows.Forms.Button();
			((System.ComponentModel.ISupportInitialize)(this.trackBar)).BeginInit();
			this.SuspendLayout();
			// 
			// btn_step
			// 
			this.btn_step.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.btn_step.Image = global::Effekseer.Properties.Resources.Step;
			this.btn_step.Location = new System.Drawing.Point(238, 36);
			this.btn_step.Name = "btn_step";
			this.btn_step.Size = new System.Drawing.Size(30, 23);
			this.btn_step.TabIndex = 21;
			this.btn_step.UseVisualStyleBackColor = true;
			this.btn_step.Click += new System.EventHandler(this.btn_step_Click);
			// 
			// btn_stop
			// 
			this.btn_stop.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.btn_stop.Image = global::Effekseer.Properties.Resources.Stop;
			this.btn_stop.Location = new System.Drawing.Point(300, 36);
			this.btn_stop.Name = "btn_stop";
			this.btn_stop.Size = new System.Drawing.Size(50, 23);
			this.btn_stop.TabIndex = 30;
			this.btn_stop.UseVisualStyleBackColor = true;
			this.btn_stop.Click += new System.EventHandler(this.btn_stop_Click);
			// 
			// lb_end
			// 
			this.lb_end.AutoSize = true;
			this.lb_end.Location = new System.Drawing.Point(75, 43);
			this.lb_end.Name = "lb_end";
			this.lb_end.Size = new System.Drawing.Size(29, 12);
			this.lb_end.TabIndex = 6;
			this.lb_end.Text = Properties.Resources.End;
			// 
			// tb_end
			// 
			this.tb_end.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(200)))), ((int)(((byte)(200)))));
			this.tb_end.Enabled = false;
			this.tb_end.Font = new System.Drawing.Font("MS UI Gothic", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.tb_end.IsEnable = null;
			this.tb_end.Location = new System.Drawing.Point(110, 40);
			this.tb_end.Name = "tb_end";
			this.tb_end.ReadMethod = null;
			this.tb_end.Size = new System.Drawing.Size(25, 19);
			this.tb_end.TabIndex = 11;
			this.tb_end.WheelStep = 1;
			this.tb_end.WriteMethod = null;
			// 
			// lb_current_value
			// 
			this.lb_current_value.AutoSize = true;
			this.lb_current_value.Location = new System.Drawing.Point(209, 41);
			this.lb_current_value.Name = "lb_current_value";
			this.lb_current_value.Size = new System.Drawing.Size(23, 12);
			this.lb_current_value.TabIndex = 4;
			this.lb_current_value.Text = "123";
			this.lb_current_value.TextAlign = System.Drawing.ContentAlignment.TopCenter;
			// 
			// lb_st
			// 
			this.lb_st.AutoSize = true;
			this.lb_st.Location = new System.Drawing.Point(9, 43);
			this.lb_st.Name = "lb_st";
			this.lb_st.Size = new System.Drawing.Size(29, 12);
			this.lb_st.TabIndex = 3;
			this.lb_st.Text = Properties.Resources.Start;
			// 
			// tb_start
			// 
			this.tb_start.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(200)))), ((int)(((byte)(200)))));
			this.tb_start.Enabled = false;
			this.tb_start.Font = new System.Drawing.Font("MS UI Gothic", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.tb_start.IsEnable = null;
			this.tb_start.Location = new System.Drawing.Point(44, 40);
			this.tb_start.Name = "tb_start";
			this.tb_start.ReadMethod = null;
			this.tb_start.Size = new System.Drawing.Size(25, 19);
			this.tb_start.TabIndex = 10;
			this.tb_start.WheelStep = 1;
			this.tb_start.WriteMethod = null;
			// 
			// btn_play_pause
			// 
			this.btn_play_pause.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.btn_play_pause.Image = global::Effekseer.Properties.Resources.Play;
			this.btn_play_pause.Location = new System.Drawing.Point(356, 36);
			this.btn_play_pause.Name = "btn_play_pause";
			this.btn_play_pause.Size = new System.Drawing.Size(50, 23);
			this.btn_play_pause.TabIndex = 31;
			this.btn_play_pause.UseVisualStyleBackColor = true;
			this.btn_play_pause.Click += new System.EventHandler(this.btn_play_Click);
			// 
			// trackBar
			// 
			this.trackBar.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.trackBar.Location = new System.Drawing.Point(1, 1);
			this.trackBar.Margin = new System.Windows.Forms.Padding(0);
			this.trackBar.Name = "trackBar";
			this.trackBar.Size = new System.Drawing.Size(416, 45);
			this.trackBar.TabIndex = 0;
			this.trackBar.ValueChanged += new System.EventHandler(this.trackBar_ValueChanged);
			this.trackBar.CursorChanged += new System.EventHandler(this.trackBar_CursorChanged);
			// 
			// btn_backstep
			// 
			this.btn_backstep.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.btn_backstep.Image = global::Effekseer.Properties.Resources.BackStep;
			this.btn_backstep.Location = new System.Drawing.Point(173, 36);
			this.btn_backstep.Name = "btn_backstep";
			this.btn_backstep.Size = new System.Drawing.Size(30, 23);
			this.btn_backstep.TabIndex = 20;
			this.btn_backstep.UseVisualStyleBackColor = true;
			this.btn_backstep.Click += new System.EventHandler(this.btn_backstep_Click);
			// 
			// DockViewerController
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(496, 92);
			this.Controls.Add(this.btn_backstep);
			this.Controls.Add(this.btn_step);
			this.Controls.Add(this.btn_stop);
			this.Controls.Add(this.lb_end);
			this.Controls.Add(this.tb_end);
			this.Controls.Add(this.lb_current_value);
			this.Controls.Add(this.lb_st);
			this.Controls.Add(this.tb_start);
			this.Controls.Add(this.btn_play_pause);
			this.Controls.Add(this.trackBar);
			this.DockAreas = ((WeifenLuo.WinFormsUI.Docking.DockAreas)(((((WeifenLuo.WinFormsUI.Docking.DockAreas.Float | WeifenLuo.WinFormsUI.Docking.DockAreas.DockLeft) 
            | WeifenLuo.WinFormsUI.Docking.DockAreas.DockRight) 
            | WeifenLuo.WinFormsUI.Docking.DockAreas.DockTop) 
            | WeifenLuo.WinFormsUI.Docking.DockAreas.DockBottom)));
			this.Font = new System.Drawing.Font("MS UI Gothic", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.MinimumSize = new System.Drawing.Size(200, 100);
			this.Name = "DockViewerController";
			this.Text = Properties.Resources.ViewerControls;
			((System.ComponentModel.ISupportInitialize)(this.trackBar)).EndInit();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.TrackBar trackBar;
		private System.Windows.Forms.Button btn_play_pause;
		private GUI.Component.DelegateIntTextBox tb_start;
		private System.Windows.Forms.Label lb_st;
		private System.Windows.Forms.Label lb_current_value;
		private GUI.Component.DelegateIntTextBox tb_end;
		private System.Windows.Forms.Label lb_end;
		private System.Windows.Forms.Button btn_stop;
		private System.Windows.Forms.Button btn_step;
		private System.Windows.Forms.Button btn_backstep;
	}
}
