namespace Effekseer.GUI
{
	partial class DockNetwork
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
			this.btn_connect = new System.Windows.Forms.Button();
			this.btn_send = new System.Windows.Forms.Button();
			this.lbl_state = new System.Windows.Forms.Label();
			this.cb_sendOnSave = new System.Windows.Forms.CheckBox();
			this.cb_sendOnEdit = new System.Windows.Forms.CheckBox();
			this.cb_sendOnLoad = new System.Windows.Forms.CheckBox();
			this.cb_autoConnect = new System.Windows.Forms.CheckBox();
			this.lbl_port = new System.Windows.Forms.Label();
			this.txt_port = new Effekseer.GUI.Component.DelegateIntTextBox();
			this.lbl_target = new System.Windows.Forms.Label();
			this.txt_target = new Effekseer.GUI.Component.DelegateStringTextBox();
			this.SuspendLayout();
			// 
			// btn_connect
			// 
			this.btn_connect.Location = new System.Drawing.Point(12, 170);
			this.btn_connect.Name = "btn_connect";
			this.btn_connect.Size = new System.Drawing.Size(75, 23);
			this.btn_connect.TabIndex = 10;
			this.btn_connect.Text = Properties.Resources.Connect;
			this.btn_connect.UseVisualStyleBackColor = true;
			this.btn_connect.Click += new System.EventHandler(this.btn_connect_Click);
			// 
			// btn_send
			// 
			this.btn_send.Location = new System.Drawing.Point(12, 141);
			this.btn_send.Name = "btn_send";
			this.btn_send.Size = new System.Drawing.Size(75, 23);
			this.btn_send.TabIndex = 9;
			this.btn_send.Text = Properties.Resources.SendData;
			this.btn_send.UseVisualStyleBackColor = true;
			this.btn_send.Click += new System.EventHandler(this.btn_send_Click);
			// 
			// lbl_state
			// 
			this.lbl_state.AutoSize = true;
			this.lbl_state.Location = new System.Drawing.Point(13, 126);
			this.lbl_state.Name = "lbl_state";
			this.lbl_state.Size = new System.Drawing.Size(35, 12);
			this.lbl_state.TabIndex = 8;
            this.lbl_state.Text = Properties.Resources.NetworkState;
			// 
			// cb_sendOnSave
			// 
			this.cb_sendOnSave.AutoSize = true;
			this.cb_sendOnSave.Location = new System.Drawing.Point(14, 103);
			this.cb_sendOnSave.Name = "cb_sendOnSave";
			this.cb_sendOnSave.Size = new System.Drawing.Size(149, 16);
			this.cb_sendOnSave.TabIndex = 7;
			this.cb_sendOnSave.Text = Properties.Resources.TransmitDataOnSave;
			this.cb_sendOnSave.UseVisualStyleBackColor = true;
			// 
			// cb_sendOnEdit
			// 
			this.cb_sendOnEdit.AutoSize = true;
			this.cb_sendOnEdit.Location = new System.Drawing.Point(14, 81);
			this.cb_sendOnEdit.Name = "cb_sendOnEdit";
			this.cb_sendOnEdit.Size = new System.Drawing.Size(149, 16);
			this.cb_sendOnEdit.TabIndex = 6;
			this.cb_sendOnEdit.Text = Properties.Resources.TransmitDataOnEdit;
			this.cb_sendOnEdit.UseVisualStyleBackColor = true;
			// 
			// cb_sendOnLoad
			// 
			this.cb_sendOnLoad.AutoSize = true;
			this.cb_sendOnLoad.Location = new System.Drawing.Point(14, 59);
			this.cb_sendOnLoad.Name = "cb_sendOnLoad";
			this.cb_sendOnLoad.Size = new System.Drawing.Size(205, 16);
			this.cb_sendOnLoad.TabIndex = 5;
			this.cb_sendOnLoad.Text = Properties.Resources.TransmitDataOnLoad;
			this.cb_sendOnLoad.UseVisualStyleBackColor = true;
			// 
			// cb_autoConnect
			// 
			this.cb_autoConnect.AutoSize = true;
			this.cb_autoConnect.Location = new System.Drawing.Point(14, 37);
			this.cb_autoConnect.Name = "cb_autoConnect";
			this.cb_autoConnect.Size = new System.Drawing.Size(218, 16);
			this.cb_autoConnect.TabIndex = 4;
			this.cb_autoConnect.Text = Properties.Resources.AutoConnect;
			this.cb_autoConnect.UseVisualStyleBackColor = true;
			// 
			// lbl_port
			// 
			this.lbl_port.AutoSize = true;
			this.lbl_port.Location = new System.Drawing.Point(167, 15);
			this.lbl_port.Name = "lbl_port";
			this.lbl_port.Size = new System.Drawing.Size(33, 12);
			this.lbl_port.TabIndex = 3;
			this.lbl_port.Text = Properties.Resources.Port;
			// 
			// txt_port
			// 
			this.txt_port.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(170)))), ((int)(((byte)(170)))));
			this.txt_port.Enabled = false;
			this.txt_port.IsEnable = null;
			this.txt_port.Location = new System.Drawing.Point(206, 12);
			this.txt_port.Name = "txt_port";
			this.txt_port.ReadMethod = null;
			this.txt_port.Size = new System.Drawing.Size(40, 19);
			this.txt_port.TabIndex = 2;
			this.txt_port.WheelStep = 1;
			this.txt_port.WriteMethod = null;
			// 
			// lbl_target
			// 
			this.lbl_target.AutoSize = true;
			this.lbl_target.Location = new System.Drawing.Point(12, 15);
			this.lbl_target.Name = "lbl_target";
			this.lbl_target.Size = new System.Drawing.Size(41, 12);
			this.lbl_target.TabIndex = 1;
			this.lbl_target.Text = Properties.Resources.NetworkAddress;
			// 
			// txt_target
			// 
			this.txt_target.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(200)))), ((int)(((byte)(200)))));
			this.txt_target.Enabled = false;
			this.txt_target.IsEnable = null;
			this.txt_target.Location = new System.Drawing.Point(61, 12);
			this.txt_target.Name = "txt_target";
			this.txt_target.ReadMethod = null;
			this.txt_target.Size = new System.Drawing.Size(100, 19);
			this.txt_target.TabIndex = 0;
			this.txt_target.WheelStep = 1;
			this.txt_target.WriteMethod = null;
			// 
			// DockNetwork
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.AutoScroll = true;
			this.ClientSize = new System.Drawing.Size(284, 262);
			this.Controls.Add(this.btn_connect);
			this.Controls.Add(this.btn_send);
			this.Controls.Add(this.lbl_state);
			this.Controls.Add(this.cb_sendOnSave);
			this.Controls.Add(this.cb_sendOnEdit);
			this.Controls.Add(this.cb_sendOnLoad);
			this.Controls.Add(this.cb_autoConnect);
			this.Controls.Add(this.lbl_port);
			this.Controls.Add(this.txt_port);
			this.Controls.Add(this.lbl_target);
			this.Controls.Add(this.txt_target);
			this.DockAreas = ((WeifenLuo.WinFormsUI.Docking.DockAreas)(((((WeifenLuo.WinFormsUI.Docking.DockAreas.Float | WeifenLuo.WinFormsUI.Docking.DockAreas.DockLeft) 
            | WeifenLuo.WinFormsUI.Docking.DockAreas.DockRight) 
            | WeifenLuo.WinFormsUI.Docking.DockAreas.DockTop) 
            | WeifenLuo.WinFormsUI.Docking.DockAreas.DockBottom)));
			this.Font = new System.Drawing.Font("MS UI Gothic", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.Name = "DockNetwork";
			this.Text = Properties.Resources.Network;
			this.Load += new System.EventHandler(this.DockNetwork_Load);
			this.ResumeLayout(false);
			this.PerformLayout();
		}

		#endregion

		private Component.DelegateStringTextBox txt_target;
		private System.Windows.Forms.Label lbl_target;
		private Component.DelegateIntTextBox txt_port;
		private System.Windows.Forms.Label lbl_port;
		private System.Windows.Forms.CheckBox cb_autoConnect;
		private System.Windows.Forms.CheckBox cb_sendOnLoad;
		private System.Windows.Forms.CheckBox cb_sendOnEdit;
		private System.Windows.Forms.CheckBox cb_sendOnSave;
		private System.Windows.Forms.Label lbl_state;
		private System.Windows.Forms.Button btn_send;
		private System.Windows.Forms.Button btn_connect;
	}
}
