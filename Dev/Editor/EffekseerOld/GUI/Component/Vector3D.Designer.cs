namespace Effekseer.GUI.Component
{
	partial class Vector3D
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
			this.lb_x = new System.Windows.Forms.Label();
			this.lb_y = new System.Windows.Forms.Label();
			this.lb_z = new System.Windows.Forms.Label();
			this.tb_z = new Effekseer.GUI.Component.Float();
			this.tb_y = new Effekseer.GUI.Component.Float();
			this.tb_x = new Effekseer.GUI.Component.Float();
			this.SuspendLayout();
			// 
			// lb_x
			// 
			this.lb_x.AutoSize = true;
			this.lb_x.ForeColor = System.Drawing.Color.Maroon;
			this.lb_x.Location = new System.Drawing.Point(4, 4);
			this.lb_x.Name = "lb_x";
			this.lb_x.Size = new System.Drawing.Size(11, 12);
			this.lb_x.TabIndex = 1;
			this.lb_x.Text = "x";
			// 
			// lb_y
			// 
			this.lb_y.AutoSize = true;
			this.lb_y.ForeColor = System.Drawing.Color.Green;
			this.lb_y.Location = new System.Drawing.Point(4, 23);
			this.lb_y.Name = "lb_y";
			this.lb_y.Size = new System.Drawing.Size(11, 12);
			this.lb_y.TabIndex = 2;
			this.lb_y.Text = "y";
			// 
			// lb_z
			// 
			this.lb_z.AutoSize = true;
			this.lb_z.ForeColor = System.Drawing.Color.Navy;
			this.lb_z.Location = new System.Drawing.Point(5, 42);
			this.lb_z.Name = "lb_z";
			this.lb_z.Size = new System.Drawing.Size(10, 12);
			this.lb_z.TabIndex = 5;
			this.lb_z.Text = "z";
			// 
			// tb_z
			// 
			this.tb_z.Anchor = System.Windows.Forms.AnchorStyles.Left;
			this.tb_z.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(200)))), ((int)(((byte)(200)))));
			this.tb_z.Binding = null;
			this.tb_z.Enabled = false;
			this.tb_z.EnableUndo = true;
			this.tb_z.IsEnable = null;
			this.tb_z.Location = new System.Drawing.Point(20, 40);
			this.tb_z.Name = "tb_z";
			this.tb_z.Size = new System.Drawing.Size(80, 19);
			this.tb_z.TabIndex = 4;
			// 
			// tb_y
			// 
			this.tb_y.Anchor = System.Windows.Forms.AnchorStyles.Left;
			this.tb_y.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(200)))), ((int)(((byte)(200)))));
			this.tb_y.Binding = null;
			this.tb_y.Enabled = false;
			this.tb_y.EnableUndo = true;
			this.tb_y.IsEnable = null;
			this.tb_y.Location = new System.Drawing.Point(20, 21);
			this.tb_y.Name = "tb_y";
			this.tb_y.Size = new System.Drawing.Size(80, 19);
			this.tb_y.TabIndex = 3;
			// 
			// tb_x
			// 
			this.tb_x.Anchor = System.Windows.Forms.AnchorStyles.Left;
			this.tb_x.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(200)))), ((int)(((byte)(200)))));
			this.tb_x.Binding = null;
			this.tb_x.Enabled = false;
			this.tb_x.EnableUndo = true;
			this.tb_x.IsEnable = null;
			this.tb_x.Location = new System.Drawing.Point(20, 2);
			this.tb_x.Name = "tb_x";
			this.tb_x.Size = new System.Drawing.Size(80, 19);
			this.tb_x.TabIndex = 0;
			// 
			// Vector3D
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.lb_z);
			this.Controls.Add(this.tb_z);
			this.Controls.Add(this.tb_y);
			this.Controls.Add(this.lb_y);
			this.Controls.Add(this.lb_x);
			this.Controls.Add(this.tb_x);
			this.Name = "Vector3D";
			this.Size = new System.Drawing.Size(110, 58);
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private Float tb_x;
		private System.Windows.Forms.Label lb_x;
		private System.Windows.Forms.Label lb_y;
		private Float tb_y;
		private Float tb_z;
		private System.Windows.Forms.Label lb_z;
	}
}
