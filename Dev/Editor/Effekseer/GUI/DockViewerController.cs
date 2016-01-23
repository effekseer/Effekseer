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

			if (Core.Language == Language.English)
			{
				Text = "Viewer Control";
				lb_st.Text = "Start";
				lb_end.Text = "End";
			}

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
		}

		public void UpdateController()
		{
			lb_current_value.Text = GUIManager.DockViewer.Current.ToString();

			if (GUIManager.DockViewer.IsPaused)
			{
				//btn_pause.Text = "resume";
			}
			else
			{
				//btn_pause.Text = "pause";
            }

            trackBar.Value = GUIManager.DockViewer.Current;

			if (GUIManager.DockViewer.IsPlaying)
			{
				if(GUIManager.DockViewer.IsPaused)
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
			if (GUIManager.DockViewer.IsPlaying && !GUIManager.DockViewer.IsPaused)
			{
				GUIManager.DockViewer.PauseAndResumeViewer();
			}
			else
			{
				if(GUIManager.DockViewer.IsPaused)
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
			GUIManager.DockViewer.StopViewer();
			btn_play_pause.Image = global::Effekseer.Properties.Resources.Play;
		}

		private void btn_pause_Click(object sender, EventArgs e)
		{
			//GUIManager.DockViewer.PauseAndResumeViewer();
		}

		private void btn_step_Click(object sender, EventArgs e)
		{
			GUIManager.DockViewer.StepViewer(false);
		}

		private void btn_backstep_Click(object sender, EventArgs e)
		{
			GUIManager.DockViewer.BackStepViewer();
		}

		private void cb_loop_CheckedChanged(object sender, EventArgs e)
		{
			//Core.IsLoop = cb_loop.Checked;
		}

		void Core_OnAfterChangeStartFrame(object sender, EventArgs e)
		{
			tb_start.Reload();
			if (GUIManager.DockViewer.Current < Core.StartFrame)
			{
				GUIManager.DockViewer.Current = Core.StartFrame;
			}
			trackBar.Minimum = Core.StartFrame;
		}

		void Core_OnAfterChangeEndFrame(object sender, EventArgs e)
		{
			tb_end.Reload();
			if (GUIManager.DockViewer.Current > Core.EndFrame)
			{
				GUIManager.DockViewer.Current = Core.EndFrame;
			}
			trackBar.Maximum = Core.EndFrame;
		}

		void Core_OnAfterChangeIsLoop(object sender, EventArgs e)
		{
			//cb_loop.Checked = Core.IsLoop;
		}

		private void trackBar_ValueChanged(object sender, EventArgs e)
		{
			GUIManager.DockViewer.Current = trackBar.Value;
		}
	}
}
