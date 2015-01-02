namespace Effekseer.GUI
{
	partial class DockCulling
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
			this.lp_culling = new Effekseer.GUI.Component.LayoutPanel();
			this.SuspendLayout();
			// 
			// lp_culling
			// 
			this.lp_culling.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.lp_culling.Location = new System.Drawing.Point(12, 12);
			this.lp_culling.Name = "lp_culling";
			this.lp_culling.Size = new System.Drawing.Size(260, 238);
			this.lp_culling.TabIndex = 2;
			// 
			// DockCulling
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(284, 262);
			this.Controls.Add(this.lp_culling);
			this.Font = new System.Drawing.Font("MS UI Gothic", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.Name = "DockCulling";
			this.Load += new System.EventHandler(this.DockCulling_Load);
			this.ResumeLayout(false);

		}

		#endregion

		private Component.LayoutPanel lp_culling;
	}
}
