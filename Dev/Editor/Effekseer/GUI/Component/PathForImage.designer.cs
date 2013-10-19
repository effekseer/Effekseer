namespace Effekseer.GUI.Component
{
	partial class PathForImage
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
			this.btn_load = new System.Windows.Forms.Button();
			this.lbl_file = new System.Windows.Forms.Label();
			this.pic_preview = new System.Windows.Forms.PictureBox();
			this.lbl_info = new System.Windows.Forms.Label();
			this.btn_delete = new System.Windows.Forms.Button();
			((System.ComponentModel.ISupportInitialize)(this.pic_preview)).BeginInit();
			this.SuspendLayout();
			// 
			// btn_load
			// 
			this.btn_load.Font = new System.Drawing.Font("MS UI Gothic", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.btn_load.Location = new System.Drawing.Point(0, 0);
			this.btn_load.Name = "btn_load";
			this.btn_load.Size = new System.Drawing.Size(40, 20);
			this.btn_load.TabIndex = 0;
			this.btn_load.Text = "読込";
			this.btn_load.UseVisualStyleBackColor = true;
			this.btn_load.Click += new System.EventHandler(this.btn_load_Click);
			// 
			// lbl_file
			// 
			this.lbl_file.AutoSize = true;
			this.lbl_file.Font = new System.Drawing.Font("MS UI Gothic", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.lbl_file.Location = new System.Drawing.Point(42, 5);
			this.lbl_file.Name = "lbl_file";
			this.lbl_file.Size = new System.Drawing.Size(29, 11);
			this.lbl_file.TabIndex = 1;
			this.lbl_file.Text = "none";
			// 
			// pic_preview
			// 
			this.pic_preview.Location = new System.Drawing.Point(0, 40);
			this.pic_preview.Name = "pic_preview";
			this.pic_preview.Size = new System.Drawing.Size(128, 128);
			this.pic_preview.TabIndex = 2;
			this.pic_preview.TabStop = false;
			// 
			// lbl_info
			// 
			this.lbl_info.AutoSize = true;
			this.lbl_info.Font = new System.Drawing.Font("MS UI Gothic", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.lbl_info.Location = new System.Drawing.Point(42, 24);
			this.lbl_info.Name = "lbl_info";
			this.lbl_info.Size = new System.Drawing.Size(29, 11);
			this.lbl_info.TabIndex = 3;
			this.lbl_info.Text = "none";
			// 
			// btn_delete
			// 
			this.btn_delete.Font = new System.Drawing.Font("MS UI Gothic", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.btn_delete.Location = new System.Drawing.Point(0, 19);
			this.btn_delete.Name = "btn_delete";
			this.btn_delete.Size = new System.Drawing.Size(40, 20);
			this.btn_delete.TabIndex = 4;
			this.btn_delete.Text = "解除";
			this.btn_delete.UseVisualStyleBackColor = true;
			this.btn_delete.Click += new System.EventHandler(this.btn_delete_Click);
			// 
			// PathForImage
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.btn_delete);
			this.Controls.Add(this.lbl_info);
			this.Controls.Add(this.pic_preview);
			this.Controls.Add(this.lbl_file);
			this.Controls.Add(this.btn_load);
			this.Name = "PathForImage";
			this.Size = new System.Drawing.Size(148, 169);
			((System.ComponentModel.ISupportInitialize)(this.pic_preview)).EndInit();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Button btn_load;
		private System.Windows.Forms.Label lbl_file;
		private System.Windows.Forms.PictureBox pic_preview;
		private System.Windows.Forms.Label lbl_info;
		private System.Windows.Forms.Button btn_delete;
	}
}
