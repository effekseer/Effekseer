using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Effekseer.GUI
{
	public partial class DockViewerController : DockContent
	{
		public DockViewerController()
		{
			InitializeComponent();

			Core.OnAfterChangeStartFrame += new EventHandler(Core_OnAfterChangeStartFrame);
			Core.OnAfterChangeEndFrame += new EventHandler(Core_OnAfterChangeEndFrame);
			Core.OnAfterChangeIsLoop += new EventHandler(Core_OnAfterChangeIsLoop);
			tb_start.ReadMethod = () =>
				{
					return Core.StartFrame;
				};
			tb_start.WriteMethod = (n,wheel) =>
				{
					Core.StartFrame = n;
				};

			tb_end.ReadMethod = () =>
			{
				return Core.EndFrame;
			};
			tb_end.WriteMethod = (n, wheel) =>
			{
				Core.EndFrame = n;
			};

			tb_start.Reload();
			tb_end.Reload();

			trackBar.Minimum = Core.StartFrame;
			trackBar.Maximum = Core.EndFrame;
			
			Icon = Icon.FromHandle(((Bitmap)Properties.Resources.IconViewer).GetHicon());
		}

		public void UpdateController()
		{
			lb_current_value.Text = GUIManager.DockViewer.Viewer.Current.ToString();

			if (GUIManager.DockViewer.Viewer.IsPaused)
			{
				//btn_pause.Text = "resume";
			}
			else
			{
				//btn_pause.Text = "pause";
            }

            trackBar.Value = GUIManager.DockViewer.Viewer.Current;

			if (GUIManager.DockViewer.Viewer.IsPlaying)
			{
				if(GUIManager.DockViewer.Viewer.IsPaused)
				{
					btn_play_pause.Image = global::Effekseer.Properties.Resources.Play;
				}
				else
				{
					btn_play_pause.Image = global::Effekseer.Properties.Resources.Pause;
				}
			}
			else
			{
				btn_play_pause.Image = global::Effekseer.Properties.Resources.Play;
			}
		}

		private void trackBar_CursorChanged(object sender, EventArgs e)
		{
			
		}

		private void btn_play_Click(object sender, EventArgs e)
		{
			if (GUIManager.DockViewer.Viewer.IsPlaying && !GUIManager.DockViewer.Viewer.IsPaused)
			{
				GUIManager.DockViewer.PauseAndResumeViewer();
			}
			else
			{
				if(GUIManager.DockViewer.Viewer.IsPaused)
				{
					GUIManager.DockViewer.PauseAndResumeViewer();
				}
				else
				{
					GUIManager.DockViewer.PlayViewer();
				}
			}
		}

		private void btn_stop_Click(object sender, EventArgs e)
		{
			Commands.Stop();
			btn_play_pause.Image = global::Effekseer.Properties.Resources.Play;
		}

		private void btn_step_Click(object sender, EventArgs e)
		{
			Commands.Step();
		}

		private void btn_backstep_Click(object sender, EventArgs e)
		{
			Commands.BackStep();
		}

		void Core_OnAfterChangeStartFrame(object sender, EventArgs e)
		{
			tb_start.Reload();
			if (GUIManager.DockViewer.Viewer.Current < Core.StartFrame)
			{
				GUIManager.DockViewer.Viewer.Current = Core.StartFrame;
			}
			trackBar.Minimum = Core.StartFrame;
		}

		void Core_OnAfterChangeEndFrame(object sender, EventArgs e)
		{
			tb_end.Reload();
			if (GUIManager.DockViewer.Viewer.Current > Core.EndFrame)
			{
				GUIManager.DockViewer.Viewer.Current = Core.EndFrame;
			}
			trackBar.Maximum = Core.EndFrame;
		}

		void Core_OnAfterChangeIsLoop(object sender, EventArgs e)
		{
			//cb_loop.Checked = Core.IsLoop;
		}

		private void trackBar_ValueChanged(object sender, EventArgs e)
		{
			GUIManager.DockViewer.Viewer.Current = trackBar.Value;
		}
	}
}
