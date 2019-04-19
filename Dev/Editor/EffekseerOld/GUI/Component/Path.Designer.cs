namespace Effekseer.GUI.Component
{
	partial class Path
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
			this.SuspendLayout();
			// 
			// btn_load
			// 
			this.btn_load.Font = new System.Drawing.Font("MS UI Gothic", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.btn_load.Location = new System.Drawing.Point(0, 0);
			this.btn_load.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
			this.btn_load.Name = "btn_load";
			this.btn_load.Size = new System.Drawing.Size(53, 25);
			this.btn_load.TabIndex = 0;
			this.btn_load.Text = global::Effekseer.Properties.Resources.Load;
			this.btn_load.UseVisualStyleBackColor = true;
			this.btn_load.Click += new System.EventHandler(this.btn_load_Click);
			// 
			// lbl_file
			// 
			this.lbl_file.AutoSize = true;
			this.lbl_file.Font = new System.Drawing.Font("MS UI Gothic", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.lbl_file.Location = new System.Drawing.Point(61, 6);
			this.lbl_file.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
			this.lbl_file.Name = "lbl_file";
			this.lbl_file.Size = new System.Drawing.Size(28, 14);
			this.lbl_file.TabIndex = 1;
			this.lbl_file.Text = "aaa";
			// 
			// Path
			// 
			this.AllowDrop = true;
			this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 15F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.lbl_file);
			this.Controls.Add(this.btn_load);
			this.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
			this.Name = "Path";
			this.Size = new System.Drawing.Size(200, 28);
			this.DragDrop += new System.Windows.Forms.DragEventHandler(this.Path_DragDrop);
			this.DragEnter += new System.Windows.Forms.DragEventHandler(this.Path_DragEnter);
			this.DragLeave += new System.EventHandler(this.Path_DragLeave);
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Button btn_load;
		private System.Windows.Forms.Label lbl_file;
	}
}
