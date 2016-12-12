namespace Effekseer.GUI
{
	partial class DockNodeRendererCommonValues
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
			this.lp_RendererCommon = new Effekseer.GUI.Component.LayoutPanel();
			this.SuspendLayout();
			// 
			// lp_RendererCommon
			// 
			this.lp_RendererCommon.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.lp_RendererCommon.AutoScroll = true;
			this.lp_RendererCommon.Location = new System.Drawing.Point(-2, -3);
			this.lp_RendererCommon.Name = "lp_RendererCommon";
			this.lp_RendererCommon.Size = new System.Drawing.Size(305, 281);
			this.lp_RendererCommon.TabIndex = 1;
			// 
			// DockNodeRendererCommonValues
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(300, 274);
			this.Controls.Add(this.lp_RendererCommon);
			this.Font = new System.Drawing.Font("MS UI Gothic", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.Name = "DockNodeRendererCommonValues";
			this.Text = Properties.Resources.BasicRenderSettings;
			this.ResumeLayout(false);

		}

		#endregion

		private Component.LayoutPanel lp_RendererCommon;
	}
}
