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
		public DockRecorder()
		{
			InitializeComponent();

			cb_type.SelectedIndex = 0;
			cb_tranceparence.SelectedIndex = 0;

			txt_startingFrame.ReadMethod = () =>
				{
					return startingFrame;
				};

			txt_startingFrame.WriteMethod = (value, wheel) =>
				{
					startingFrame = Math.Max(value, 1);
				};

			txt_endingFrame.ReadMethod = () =>
			{
				return endingFrame;
			};

			txt_endingFrame.WriteMethod = (value, wheel) =>
			{
				endingFrame = Math.Max(value, 1);
			};

			txt_freq.ReadMethod = () =>
			{
				return freq;
			};

			txt_freq.WriteMethod = (value, wheel) =>
			{
				freq = Math.Max(value, 1);
			};

			txt_number_v.ReadMethod = () =>
				{
					return theNumberOfImageV;
				};

			txt_number_v.WriteMethod = (value, wheel) =>
			{
				theNumberOfImageV = Math.Max(value, 1);
			};

			txt_areaWidth.ReadMethod = () =>
			{
				if (GUIManager.DockViewer.Viewer != null) return GUIManager.DockViewer.Viewer.GetViewerParamater().GuideWidth;
				return 0;
			};
			txt_areaWidth.WriteMethod = (value, wheel) =>
			{
				if (GUIManager.DockViewer.Viewer != null)
				{
					var param = GUIManager.DockViewer.Viewer.GetViewerParamater();
					param.GuideWidth = Math.Max(0, value);
					GUIManager.DockViewer.Viewer.SetViewerParamater(param);
				}
			};

			txt_areaHeight.ReadMethod = () =>
			{
				if (GUIManager.DockViewer.Viewer != null) return GUIManager.DockViewer.Viewer.GetViewerParamater().GuideHeight;
				return 0;
			};
			txt_areaHeight.WriteMethod = (value, wheel) =>
			{
				if (GUIManager.DockViewer.Viewer != null)
				{
					var param = GUIManager.DockViewer.Viewer.GetViewerParamater();
					param.GuideHeight = Math.Max(0, value);
					GUIManager.DockViewer.Viewer.SetViewerParamater(param);
				}
			};

			cb_type.SelectedIndex = 0;
			txt_areaWidth.Reload();
			txt_areaHeight.Reload();
			txt_startingFrame.Reload();
			txt_endingFrame.Reload();
			txt_number_v.Reload();
			txt_freq.Reload();
			
			Icon = Icon.FromHandle(((Bitmap)Properties.Resources.IconRecorder).GetHicon());

			if(Core.Language == Language.Japanese)
			{
				this.Text = "録画";
			}
			else
			{
				this.Text = "Recorder";
			}
		}

		int startingFrame = 1;
		int endingFrame = 16;
		int freq = 1;
		int theNumberOfImageV = 4;

		bool nowReloading = false;

		public void UpdateRecorder()
		{
			nowReloading = true;

			if (!txt_areaWidth.Changed) txt_areaWidth.Reload();
			if (!txt_areaHeight.Changed) txt_areaHeight.Reload();
			
			if (!txt_startingFrame.Changed) txt_startingFrame.Reload();
			if (!txt_endingFrame.Changed) txt_endingFrame.Reload();
			if (!txt_freq.Changed) txt_freq.Reload();

			if (GUIManager.DockViewer.Viewer != null)
			{
				var param = GUIManager.DockViewer.Viewer.GetViewerParamater();
				cb_area.Checked = param.RendersGuide;
			}

			nowReloading = false;
		}

		private void btn_record_Click(object sender, EventArgs e)
		{
			var during = endingFrame - startingFrame;
			if (during < 0)
			{
				MessageBox.Show("出力フレームが存在しません。");
			}

			var count = during / freq + 1;
			var width = theNumberOfImageV;
			var height = count / width;
			if (height * width != count) height++;

			if (GUIManager.DockViewer.Viewer != null)
			{
				var dialog = new SaveFileDialog();

				if(cb_type.SelectedIndex == 0)
				{
					dialog.Filter = "png(*.png)|*.png";
				}
				else if (cb_type.SelectedIndex == 1)
				{
					dialog.Filter = "png(*.png)|*.png";
				}
				else if (cb_type.SelectedIndex == 2)
				{
					dialog.Filter = "gif(*.gif)|*.gif";
				}
				else if (cb_type.SelectedIndex == 3)
				{
					dialog.Filter = "AVI(*.avi)|*.avi";
				}

				if (dialog.ShowDialog() != System.Windows.Forms.DialogResult.OK)
				{
					return;
				}

				var filename = dialog.FileName;

				var viewer = GUIManager.DockViewer.Viewer;
				var param = GUIManager.DockViewer.Viewer.GetViewerParamater();

				//if (viewer.LoadEffectFunc != null)
				{
					viewer.LoadEffectFunc();
				}

				
				var tempDirectory = System.IO.Directory.GetCurrentDirectory();
				System.IO.Directory.SetCurrentDirectory(Program.StartDirectory);

				if (cb_type.SelectedIndex == 0)
				{
					if (!viewer.Record(filename, count, width, startingFrame, freq, (swig.TransparenceType)cb_tranceparence.SelectedIndex))
					{
						MessageBox.Show("保存に失敗しました。コンピューターのスペックが低い、もしくは設定に問題があります。");
					}
				}
				else if (cb_type.SelectedIndex == 1)
				{
					if (!viewer.Record(filename, count, startingFrame, freq, (swig.TransparenceType)cb_tranceparence.SelectedIndex))
					{
						MessageBox.Show("保存に失敗しました。コンピューターのスペックが低い、もしくは設定に問題があります。");
					}
				}
				else if (cb_type.SelectedIndex == 2)
				{
					if (!viewer.RecordAsGifAnimation(filename, count, startingFrame, freq, (swig.TransparenceType)cb_tranceparence.SelectedIndex))
					{
						MessageBox.Show("保存に失敗しました。コンピューターのスペックが低い、もしくは設定に問題があります。");
					}
				}
				else if (cb_type.SelectedIndex == 3)
				{
					if (!viewer.RecordAsAVI(filename, count, startingFrame, freq, (swig.TransparenceType)cb_tranceparence.SelectedIndex))
					{
						MessageBox.Show("保存に失敗しました。コンピューターのスペックが低い、もしくは設定に問題があります。");
					}
				}

				System.IO.Directory.SetCurrentDirectory(tempDirectory);

			}
		}

		private void cb_guide_CheckedChanged(object sender, EventArgs e)
		{
			if (nowReloading) return;
			var param = GUIManager.DockViewer.Viewer.GetViewerParamater();
			param.RendersGuide = cb_area.Checked;
			GUIManager.DockViewer.Viewer.SetViewerParamater(param);
		}

		private void cb_type_SelectedIndexChanged(object sender, EventArgs e)
		{
			if(cb_type.SelectedIndex == 0)
			{
				txt_number_v.Enabled = true;
			}
			else if (cb_type.SelectedIndex == 1)
			{
				txt_number_v.Enabled = false;
			}
			else if (cb_type.SelectedIndex == 2)
			{
				txt_number_v.Enabled = false;
			}
			else if (cb_type.SelectedIndex == 3)
			{
				txt_number_v.Enabled = false;
			}
		}
	}
}