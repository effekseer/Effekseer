using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Effekseer.GUI.Dialog
{
	class OpenModelDialog : System.Windows.Forms.Form
	{
		public float Magnification = 1.0f;
		public bool OK = false;

		private System.ComponentModel.IContainer components = null;

		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		private void InitializeComponent()
		{
			this.lbl_mag = new System.Windows.Forms.Label();
			this.txt_magnification = new System.Windows.Forms.TextBox();
			this.btn_ok = new System.Windows.Forms.Button();
			this.btn_cancel = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// lbl_mag
			// 
			this.lbl_mag.AutoSize = true;
			this.lbl_mag.Location = new System.Drawing.Point(12, 9);
			this.lbl_mag.Name = "lbl_mag";
			this.lbl_mag.Size = new System.Drawing.Size(29, 12);
			this.lbl_mag.TabIndex = 0;

			if(Core.Language == Language.Japanese)
			{
				this.lbl_mag.Text = "拡大";
			}
			else
			{
				this.lbl_mag.Text = "Scale";
			}
			
			// 
			// txt_magnification
			// 
			this.txt_magnification.Location = new System.Drawing.Point(47, 6);
			this.txt_magnification.Name = "txt_magnification";
			this.txt_magnification.Size = new System.Drawing.Size(100, 19);
			this.txt_magnification.TabIndex = 1;
			this.txt_magnification.Text = Magnification.ToString();
			// 
			// btn_ok
			// 
			this.btn_ok.Location = new System.Drawing.Point(12, 29);
			this.btn_ok.Name = "btn_ok";
			this.btn_ok.Size = new System.Drawing.Size(70, 23);
			this.btn_ok.TabIndex = 2;

			if (Core.Language == Language.Japanese)
			{
				this.btn_ok.Text = "保存";
			}
			else
			{
				this.btn_ok.Text = "Save";
			}
			
			this.btn_ok.UseVisualStyleBackColor = true;
			this.btn_ok.Click += new System.EventHandler(this.btn_ok_Click);
			// 
			// btn_cancel
			// 
			this.btn_cancel.Location = new System.Drawing.Point(88, 29);
			this.btn_cancel.Name = "btn_cancel";
			this.btn_cancel.Size = new System.Drawing.Size(70, 23);
			this.btn_cancel.TabIndex = 3;

			if (Core.Language == Language.Japanese)
			{
				this.btn_cancel.Text = "キャンセル";
			}
			else
			{
				this.btn_cancel.Text = "Cancel";
			}
			
			this.btn_cancel.UseVisualStyleBackColor = true;
			this.btn_cancel.Click += new System.EventHandler(this.btn_cancel_Click);
			// 
			// magnification
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(184, 62);
			this.ControlBox = false;
			this.Controls.Add(this.btn_cancel);
			this.Controls.Add(this.btn_ok);
			this.Controls.Add(this.txt_magnification);
			this.Controls.Add(this.lbl_mag);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
			this.Name = "magnification";
			
			if (Core.Language == Language.Japanese)
			{
				this.Text = "拡大";
			}
			else
			{
				this.Text = "Scale";
			}

			this.ResumeLayout(false);
			this.PerformLayout();

		}

		private System.Windows.Forms.Label lbl_mag;
		private System.Windows.Forms.TextBox txt_magnification;
		private System.Windows.Forms.Button btn_ok;
		private System.Windows.Forms.Button btn_cancel;

		public OpenModelDialog(float scale)
		{
			this.Magnification = scale;

			InitializeComponent();
			this.Size = new System.Drawing.Size(220, 120);
		}

		private void btn_ok_Click(object sender, System.EventArgs e)
		{
			float mag = 1.0f;
			if (float.TryParse(txt_magnification.Text, out mag))
			{
				Magnification = mag;
				OK = true;
				Close();
			}
		}

		private void btn_cancel_Click(object sender, System.EventArgs e)
		{
			Close();
		}
	}
}
