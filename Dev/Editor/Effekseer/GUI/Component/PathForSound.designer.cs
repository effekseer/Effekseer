namespace Effekseer.GUI.Component
{
	partial class PathForSound
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
			this.lbl_info1 = new System.Windows.Forms.Label();
			this.btn_delete = new System.Windows.Forms.Button();
			this.btn_play = new System.Windows.Forms.Button();
			this.lbl_info2 = new System.Windows.Forms.Label();
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
			this.lbl_file.Location = new System.Drawing.Point(56, 6);
			this.lbl_file.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
			this.lbl_file.Name = "lbl_file";
			this.lbl_file.Size = new System.Drawing.Size(28, 14);
			this.lbl_file.TabIndex = 1;
			this.lbl_file.Text = "aaa";
			// 
			// lbl_info1
			// 
			this.lbl_info1.AutoSize = true;
			this.lbl_info1.Font = new System.Drawing.Font("MS UI Gothic", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.lbl_info1.Location = new System.Drawing.Point(56, 30);
			this.lbl_info1.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
			this.lbl_info1.Name = "lbl_info1";
			this.lbl_info1.Size = new System.Drawing.Size(45, 14);
			this.lbl_info1.TabIndex = 3;
			this.lbl_info1.Text = "format";
			// 
			// btn_delete
			// 
			this.btn_delete.Font = new System.Drawing.Font("MS UI Gothic", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.btn_delete.Location = new System.Drawing.Point(0, 24);
			this.btn_delete.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
			this.btn_delete.Name = "btn_delete";
			this.btn_delete.Size = new System.Drawing.Size(53, 25);
			this.btn_delete.TabIndex = 4;
			this.btn_delete.Text = global::Effekseer.Properties.Resources.Delete;
			this.btn_delete.UseVisualStyleBackColor = true;
			this.btn_delete.Click += new System.EventHandler(this.btn_delete_Click);
			// 
			// btn_play
			// 
			this.btn_play.Font = new System.Drawing.Font("MS UI Gothic", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.btn_play.Location = new System.Drawing.Point(0, 48);
			this.btn_play.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
			this.btn_play.Name = "btn_play";
			this.btn_play.Size = new System.Drawing.Size(53, 25);
			this.btn_play.TabIndex = 5;
			this.btn_play.Text = global::Effekseer.Properties.Resources.PlayString;
			this.btn_play.UseVisualStyleBackColor = true;
			this.btn_play.Click += new System.EventHandler(this.btn_play_Click);
			// 
			// lbl_info2
			// 
			this.lbl_info2.AutoSize = true;
			this.lbl_info2.Font = new System.Drawing.Font("MS UI Gothic", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.lbl_info2.Location = new System.Drawing.Point(56, 54);
			this.lbl_info2.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
			this.lbl_info2.Name = "lbl_info2";
			this.lbl_info2.Size = new System.Drawing.Size(45, 14);
			this.lbl_info2.TabIndex = 6;
			this.lbl_info2.Text = "format";
			// 
			// PathForSound
			// 
			this.AllowDrop = true;
			this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 15F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.Controls.Add(this.lbl_info2);
			this.Controls.Add(this.btn_play);
			this.Controls.Add(this.btn_delete);
			this.Controls.Add(this.lbl_info1);
			this.Controls.Add(this.lbl_file);
			this.Controls.Add(this.btn_load);
			this.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
			this.Name = "PathForSound";
			this.Size = new System.Drawing.Size(209, 74);
			this.DragDrop += new System.Windows.Forms.DragEventHandler(this.PathForSound_DragDrop);
			this.DragEnter += new System.Windows.Forms.DragEventHandler(this.PathForSound_DragEnter);
			this.DragLeave += new System.EventHandler(this.PathForSound_DragLeave);
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Button btn_load;
		private System.Windows.Forms.Label lbl_file;
		private System.Windows.Forms.Label lbl_info1;
		private System.Windows.Forms.Button btn_delete;
		private System.Windows.Forms.Button btn_play;
		private System.Windows.Forms.Label lbl_info2;
	}
}
