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
			int[] currentFrame = new int[] { GUIManager.Viewer.Current };
			int[] frameMin = new int[] { Core.StartFrame };
			int[] frameMax = new int[] { Core.EndFrame };

			GUIManager.NativeManager.PushItemWidth(-1);

			if (GUIManager.NativeManager.SliderInt("###Timeline", currentFrame, Core.StartFrame, Core.EndFrame))
			{
				GUIManager.Viewer.Current = currentFrame[0];
			}

			GUIManager.NativeManager.PopItemWidth();

			GUIManager.NativeManager.Separator();

			GUIManager.NativeManager.PushItemWidth(200);

			if (GUIManager.NativeManager.DragIntRange2("###TimeRange", frameMin, frameMax, 1.0f, 0, 1200))
			{
				Core.StartFrame = frameMin[0];
				Core.EndFrame = frameMax[0];

				GUIManager.Viewer.Current = System.Math.Max(GUIManager.Viewer.Current, Core.StartFrame);
				GUIManager.Viewer.Current = System.Math.Min(GUIManager.Viewer.Current, Core.EndFrame);
			}

			GUIManager.NativeManager.PopItemWidth();

			GUIManager.NativeManager.SameLine();
			
			float buttonSizeY = GUIManager.NativeManager.GetFrameHeight();
			float buttonSizeX = buttonSizeY * 2.2f;
			
			if(GUIManager.NativeManager.ImageButton(Images.BackStep, buttonSizeX, buttonSizeY))
			{
				Commands.BackStep();
			}

			GUIManager.NativeManager.SameLine();
			if(GUIManager.NativeManager.ImageButton(Images.Step, buttonSizeX, buttonSizeY))
			{
				Commands.Step();
			}

			GUIManager.NativeManager.SameLine();
			if(GUIManager.NativeManager.ImageButton(Images.Stop, buttonSizeX, buttonSizeY))
			{
				if(GUIManager.Viewer.IsPlaying && !GUIManager.Viewer.IsPaused)
				{
					Commands.Stop();
					Commands.Play();
				}
				else
				{
					Commands.Stop();
				}
			}

			GUIManager.NativeManager.SameLine();

			var icon = Images.Play;
			if (GUIManager.Viewer.IsPlaying && !GUIManager.Viewer.IsPaused)
			{
				icon = Images.Pause;
			}

			if (GUIManager.NativeManager.ImageButton(icon, buttonSizeX, buttonSizeY))
			{
				if (GUIManager.Viewer.IsPlaying && !GUIManager.Viewer.IsPaused)
				{
					GUIManager.Viewer.PauseAndResumeViewer();
				}
				else
				{
					if (GUIManager.Viewer.IsPaused)
					{
						GUIManager.Viewer.PauseAndResumeViewer();
					}
					else
					{
						GUIManager.Viewer.PlayViewer();
					}
				}
			}
		}
	}
}
