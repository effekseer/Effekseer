using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Xml;

namespace Effekseer.GUI
{
	public partial class DockRecorder : DockContent
	{
		int xCount = 1;
		int yCount = 1;
		int offset = 1;
		int frameskip = 0;

		public DockRecorder()
		{
			InitializeComponent();

			txt_xCount.ReadMethod = () =>
			{
				return xCount;
			};
			txt_xCount.WriteMethod = (value, wheel) =>
			{
				xCount = Math.Max(value, 1);
			};

			txt_yCount.ReadMethod = () =>
			{
				return yCount;
			};
			txt_yCount.WriteMethod = (value, wheel) =>
			{
				yCount = Math.Max(value, 1);
			};

			txt_offset.ReadMethod = () =>
			{
				return offset;
			};
			txt_offset.WriteMethod = (value, wheel) =>
			{
				offset = Math.Max(value, 1);
			};

			txt_frameskip.ReadMethod = () =>
			{
				return frameskip;
			};
			txt_frameskip.WriteMethod = (value, wheel) =>
			{
				frameskip = Math.Max(value, 0);
			};
		}

		public void UpdateRecorder()
		{
			if (!txt_xCount.Changed) txt_xCount.Reload();
			if (!txt_yCount.Changed) txt_yCount.Reload();
			if (!txt_offset.Changed) txt_offset.Reload();
			if (!txt_frameskip.Changed) txt_frameskip.Reload();
		}

		private void btn_record_Click(object sender, EventArgs e)
		{
			if (GUIManager.DockViewer.ViewerAsDynamic != null)
			{
				var viewer = GUIManager.DockViewer.ViewerAsDynamic;
				var param = GUIManager.DockViewer.ViewerAsDynamic.GetViewerParamater();

				DateTime time = DateTime.Now;
				string path = "";
				path += time.Year.ToString();
				path += time.Month.ToString("D2");
				path += time.Day.ToString("D2");
				path += time.Hour.ToString("D2");
				path += time.Minute.ToString("D2");
				path += time.Second.ToString("D2") + "_";
				path += param.GuideWidth.ToString() + "_";
				path += param.GuideHeight.ToString() + "_";
				path += xCount.ToString() + "_";
				path += yCount.ToString() + "_";
				path += offset.ToString() + "_";
				path += frameskip.ToString() + ".png";

				if (viewer.LoadEffectFunc != null)
				{
					viewer.LoadEffectFunc();
				}

				var tempDirectory = System.IO.Directory.GetCurrentDirectory();
				System.IO.Directory.SetCurrentDirectory(Program.StartDirectory);

				if (!viewer.Record(path, xCount, yCount, offset, frameskip, cb_isTranslucent.Checked))
				{
					MessageBox.Show("録画に失敗しました。コンピューターのスペックが低い、もしくは設定に問題があります。");
				}

				System.IO.Directory.SetCurrentDirectory(tempDirectory);

			}
		}
	}
}