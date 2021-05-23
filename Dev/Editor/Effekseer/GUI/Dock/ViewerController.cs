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
			Label = Icons.PanelViewerCtrl + Resources.GetString("ViewerControls") + "###ViewerControls";
		}

		protected override void UpdateInternal()
		{
			int[] currentFrame = new int[] { (int)Manager.Viewer.Current };
			int[] frameMin = new int[] { Core.StartFrame };
			int[] frameMax = new int[] { Core.EndFrame };

			Manager.NativeManager.PushItemWidth(-1);

			if (Manager.NativeManager.SliderInt("###Timeline", currentFrame, Core.StartFrame, Core.EndFrame))
			{
				Manager.Viewer.Current = currentFrame[0];
			}

			Manager.NativeManager.PopItemWidth();

			Manager.NativeManager.Separator();

			Manager.NativeManager.PushItemWidth(200);

			if (Manager.NativeManager.DragIntRange2("###TimeRange", frameMin, frameMax, 1.0f, 0, 1200))
			{
				Core.StartFrame = frameMin[0];
				Core.EndFrame = frameMax[0];

				Manager.Viewer.Current = System.Math.Max(Manager.Viewer.Current, Core.StartFrame);
				Manager.Viewer.Current = System.Math.Min(Manager.Viewer.Current, Core.EndFrame);
			}

			Manager.NativeManager.PopItemWidth();

			Manager.NativeManager.SameLine();
			
			float buttonSizeY = Manager.NativeManager.GetFrameHeight();
			float buttonSizeX = buttonSizeY * 2.2f;
			
			if(Manager.NativeManager.ImageButton(Images.BackStep, buttonSizeX, buttonSizeY))
			{
				Commands.BackStep();
			}

			Manager.NativeManager.SameLine();
			if(Manager.NativeManager.ImageButton(Images.Step, buttonSizeX, buttonSizeY))
			{
				Commands.Step();
			}

			Manager.NativeManager.SameLine();
			if(Manager.NativeManager.ImageButton(Images.Stop, buttonSizeX, buttonSizeY))
			{
				if(Manager.Viewer.IsPlaying && !Manager.Viewer.IsPaused)
				{
					Commands.Stop();
					Commands.Play();
				}
				else
				{
					Commands.Stop();
				}
			}

			Manager.NativeManager.SameLine();

			var icon = Images.Play;
			if (Manager.Viewer.IsPlaying && !Manager.Viewer.IsPaused)
			{
				icon = Images.Pause;
			}

			if (Manager.NativeManager.ImageButton(icon, buttonSizeX, buttonSizeY))
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

			{
				Manager.NativeManager.Text("Triggers:");
				float buttonWidth = Manager.NativeManager.GetTextLineHeightWithSpacing();

				for (int i = 0; i < 4; i++)
				{
					Manager.NativeManager.SameLine();

					if (Manager.NativeManager.Button(i.ToString(), buttonWidth))
					{
						Manager.Viewer.SendTrigger(i);
					}
				}
			}
		}
	}
}
