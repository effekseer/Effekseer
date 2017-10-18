
public class Script
{
	public static string UniqueName
	{
		get
		{
			return "Default";
		}
	}	
	
	public static string Author
	{
		get
		{
			return "Default";
		}
	}

	public static string Title
	{
		get
		{
			if(Effekseer.Core.Language == Effekseer.Language.Japanese) return "標準形式";
			return "Default format";
		}
	}

	public static string Description
	{
		get
		{
			if(Effekseer.Core.Language == Effekseer.Language.Japanese) return "標準形式で出力する。";
			return "Export as default format";
		}
	}

	public static string Filter
	{
		get
		{
			if(Effekseer.Core.Language == Effekseer.Language.Japanese) return "標準形式 (*.efk)|*.efk";
			return "Default format (*.efk)|*.efk";
		}
	}

	public static void Call(string path)
	{
		var window = new magnification(path);
		window.ShowDialog();
	}

	partial class magnification : System.Windows.Forms.Form
	{
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
			this.lbl_mag.Text = "Magnification";
			if(Effekseer.Core.Language == Effekseer.Language.Japanese) this.lbl_mag.Text = "倍率";
			// 
			// txt_magnification
			// 
			this.txt_magnification.Location = new System.Drawing.Point(90, 6);
			this.txt_magnification.Name = "txt_magnification";
			this.txt_magnification.Size = new System.Drawing.Size(100, 19);
			this.txt_magnification.TabIndex = 1;
			this.txt_magnification.Text = Effekseer.Core.Option.ExternalMagnification.Value.ToString();
			// 
			// btn_ok
			// 
			this.btn_ok.Location = new System.Drawing.Point(12, 29);
			this.btn_ok.Name = "btn_ok";
			this.btn_ok.Size = new System.Drawing.Size(70, 23);
			this.btn_ok.TabIndex = 2;
			this.btn_ok.Text = "Save";
			if(Effekseer.Core.Language == Effekseer.Language.Japanese) this.btn_ok.Text = "保存";
			this.btn_ok.UseVisualStyleBackColor = true;
			this.btn_ok.Click += new System.EventHandler(this.btn_ok_Click);
			// 
			// btn_cancel
			// 
			this.btn_cancel.Location = new System.Drawing.Point(88, 29);
			this.btn_cancel.Name = "btn_cancel";
			this.btn_cancel.Size = new System.Drawing.Size(70, 23);
			this.btn_cancel.TabIndex = 3;
			this.btn_cancel.Text = "Cancel";
			if(Effekseer.Core.Language == Effekseer.Language.Japanese) this.btn_cancel.Text = "キャンセル";
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
			this.Text = "Magnification";
			if(Effekseer.Core.Language == Effekseer.Language.Japanese) this.Text = "倍率";
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		private System.Windows.Forms.Label lbl_mag;
		private System.Windows.Forms.TextBox txt_magnification;
		private System.Windows.Forms.Button btn_ok;
		private System.Windows.Forms.Button btn_cancel;

		public magnification(string path)
		{
			InitializeComponent();
			this.path = path;
			this.Size = new System.Drawing.Size(270, 120);
		}

		string path;

		private void btn_ok_Click(object sender, System.EventArgs e)
		{
			float mag = 1.0f;
			if (float.TryParse(txt_magnification.Text, out mag))
			{
				var binary = Effekseer.Binary.Exporter.Export(mag);
				System.IO.File.WriteAllBytes(path, binary);
				Effekseer.Core.Option.ExternalMagnification.SetValue(mag);
				Close();
			}
		}

		private void btn_cancel_Click(object sender, System.EventArgs e)
		{
			Close();
		}
	}
}
