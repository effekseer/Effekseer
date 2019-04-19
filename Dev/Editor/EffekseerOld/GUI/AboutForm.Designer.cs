namespace Effekseer.GUI
{
	partial class AboutForm
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

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(AboutForm));
			this.ok = new System.Windows.Forms.Button();
			this.icon = new System.Windows.Forms.PictureBox();
			this.subject = new System.Windows.Forms.Label();
			this.licence = new System.Windows.Forms.TextBox();
			this.link1 = new System.Windows.Forms.Label();
			this.link2 = new System.Windows.Forms.Label();
			((System.ComponentModel.ISupportInitialize)(this.icon)).BeginInit();
			this.SuspendLayout();
			// 
			// ok
			// 
			this.ok.Location = new System.Drawing.Point(354, 263);
			this.ok.Name = "ok";
			this.ok.Size = new System.Drawing.Size(94, 39);
			this.ok.TabIndex = 0;
			this.ok.Text = "OK";
			this.ok.UseVisualStyleBackColor = true;
			this.ok.Click += new System.EventHandler(this.ok_Click);
			// 
			// icon
			// 
			this.icon.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("icon.BackgroundImage")));
			this.icon.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
			this.icon.Location = new System.Drawing.Point(12, 12);
			this.icon.Name = "icon";
			this.icon.Size = new System.Drawing.Size(48, 48);
			this.icon.TabIndex = 1;
			this.icon.TabStop = false;
			// 
			// subject
			// 
			this.subject.AutoSize = true;
			this.subject.Font = new System.Drawing.Font("MS UI Gothic", 18F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.subject.Location = new System.Drawing.Point(77, 24);
			this.subject.Name = "subject";
			this.subject.Size = new System.Drawing.Size(232, 24);
			this.subject.TabIndex = 2;
			this.subject.Text = "Effekseer Version 0.xx";
			// 
			// licence
			// 
			this.licence.BackColor = System.Drawing.SystemColors.ButtonHighlight;
			this.licence.Location = new System.Drawing.Point(12, 77);
			this.licence.Multiline = true;
			this.licence.Name = "licence";
			this.licence.ReadOnly = true;
			this.licence.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
			this.licence.Size = new System.Drawing.Size(436, 173);
			this.licence.TabIndex = 4;
			this.licence.Text = resources.GetString("licence.Text");
			// 
			// link1
			// 
			this.link1.AutoSize = true;
			this.link1.Cursor = System.Windows.Forms.Cursors.Hand;
			this.link1.Font = new System.Drawing.Font("MS UI Gothic", 10F, System.Drawing.FontStyle.Underline);
			this.link1.ForeColor = System.Drawing.Color.Blue;
			this.link1.Location = new System.Drawing.Point(12, 263);
			this.link1.Name = "link1";
			this.link1.Size = new System.Drawing.Size(176, 14);
			this.link1.TabIndex = 5;
			this.link1.Text = "http://effekseer.github.io/jp/";
			this.link1.Click += new System.EventHandler(this.link1_Click);
			// 
			// link2
			// 
			this.link2.AutoSize = true;
			this.link2.Cursor = System.Windows.Forms.Cursors.Hand;
			this.link2.Font = new System.Drawing.Font("MS UI Gothic", 10F, System.Drawing.FontStyle.Underline);
			this.link2.ForeColor = System.Drawing.Color.Blue;
			this.link2.Location = new System.Drawing.Point(12, 288);
			this.link2.Name = "link2";
			this.link2.Size = new System.Drawing.Size(237, 14);
			this.link2.TabIndex = 6;
			this.link2.Text = "https://github.com/effekseer/Effekseer";
			this.link2.Click += new System.EventHandler(this.link2_Click);
			// 
			// AboutForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(458, 315);
			this.Controls.Add(this.link2);
			this.Controls.Add(this.link1);
			this.Controls.Add(this.licence);
			this.Controls.Add(this.subject);
			this.Controls.Add(this.icon);
			this.Controls.Add(this.ok);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "AboutForm";
			this.ShowInTaskbar = false;
			this.Text = Properties.Resources.AboutEffekseer;
			((System.ComponentModel.ISupportInitialize)(this.icon)).EndInit();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Button ok;
		private System.Windows.Forms.PictureBox icon;
		private System.Windows.Forms.Label subject;
		private System.Windows.Forms.TextBox licence;
		private System.Windows.Forms.Label link1;
		private System.Windows.Forms.Label link2;
	}
}