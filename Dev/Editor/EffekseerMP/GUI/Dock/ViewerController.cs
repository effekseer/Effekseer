using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class ViewerController : DockPanel
	{
		public ViewerController()
		{

		}

		protected override void UpdateInternal()
		{
			int[] currentFrame = new int[] { Manager.Viewer.Current };
			int[] frameMin = new int[] { Core.StartFrame };
			int[] frameMax = new int[] { Core.EndFrame };

			if (Manager.NativeManager.SliderInt("###Timeline", currentFrame, Core.StartFrame, Core.EndFrame))
			{
				Manager.Viewer.Current = currentFrame[0];
			}

			Manager.NativeManager.Separator();

			Manager.NativeManager.PushItemWidth(200);

			if (Manager.NativeManager.DragIntRange2("###TimeRange", frameMin, frameMax, 1.0f, 0, 1200))
			{
				Core.StartFrame = frameMin[0];
				Core.EndFrame = frameMax[0];

				Manager.Viewer.Current = Math.Max(Manager.Viewer.Current, Core.StartFrame);
				Manager.Viewer.Current = Math.Min(Manager.Viewer.Current, Core.EndFrame);
			}

			Manager.NativeManager.PopItemWidth();

			Manager.NativeManager.SameLine();
			if(Manager.NativeManager.ImageButton(Images.BackStep, 48, 21))
			{
				Commands.BackStep();
			}

			Manager.NativeManager.SameLine();
			if(Manager.NativeManager.ImageButton(Images.Step, 48, 21))
			{
				Commands.Step();
			}

			Manager.NativeManager.SameLine();
			if(Manager.NativeManager.ImageButton(Images.Stop, 48, 21))
			{
				Commands.Stop();
			}

			Manager.NativeManager.SameLine();

			var icon = Images.Play;
			if (Manager.Viewer.IsPlaying && !Manager.Viewer.IsPaused)
			{
				icon = Images.Pause;
			}

			if (Manager.NativeManager.ImageButton(icon, 48, 21))
			{
				if (Manager.Viewer.IsPlaying && !Manager.Viewer.IsPaused)
				{
					Manager.Viewer.PauseAndResumeViewer();
				}
				else
				{
					if (Manager.Viewer.IsPaused)
					{
						Manager.Viewer.PauseAndResumeViewer();
					}
					else
					{
						Manager.Viewer.PlayViewer();
					}
				}
			}
		}
	}
}
