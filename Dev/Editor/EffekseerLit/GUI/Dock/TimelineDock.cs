using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class TimelineDock : DockPanel
	{
		private int _scrollFirstFrame = 0;

		public TimelineDock()
		{
			Label = Icons.PanelOptions + Resources.GetString("Options") + "###TimelineDock";

			Core.OnAfterLoad += OnAfter;
			Core.OnAfterNew += OnAfter;

			TabToolTip = Resources.GetString("Options");

			NoPadding = true;
			NoScrollBar = false;
			NoCloseButton = true;
			AllowsShortTab = false;
		}

		public override void OnDisposed()
		{
			Core.OnAfterLoad -= OnAfter;
			Core.OnAfterNew -= OnAfter;
		}

		protected override void UpdateInternal()
		{
			// Playback control buttons
			{
				float buttonSizeY = Manager.NativeManager.GetFrameHeight();
				float buttonSizeX = buttonSizeY * 2.2f;


				if (Manager.NativeManager.ImageButton(Images.BackStep, buttonSizeX, buttonSizeY))
				{
					Commands.BackStep();
				}

				Manager.NativeManager.SameLine();
				if (Manager.NativeManager.ImageButton(Images.Step, buttonSizeX, buttonSizeY))
				{
					Commands.Step();
				}

				Manager.NativeManager.SameLine();
				if (Manager.NativeManager.ImageButton(Images.Stop, buttonSizeX, buttonSizeY))
				{
					if (Manager.Viewer.IsPlaying && !Manager.Viewer.IsPaused)
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
			}

			Manager.NativeManager.Separator();

			if (Manager.NativeManager.BeginNodeFrameTimeline())
			{
				// NOTE:
				// [生成開始時間]  がオフセット
				// [生存時間] が、1インスタンスの表示時間
				// [生成時間] が [生成数] に対応して作られる 1　インスタンスごとのオフセット
				// 
				// 生成数: MaxGeneration
				// 生成開始時間: GenerationTimeOffset
				// 生存時間: Life
				// 生成時間: GenerationTime

				int frameMin = Core.StartFrame;
				int frameMax = Core.EndFrame;


				var timelineNodes = Core.Root.Children;
				var currentSelectedIndex = timelineNodes.Internal.FindIndex(x => x == Core.SelectedNode);

				for (int i = 0; i < timelineNodes.Count; i++)
				{
					var node = timelineNodes[i];

					int frameStart = 0;
					int frameLast = 0;
					if (!Manager.Viewer.native.GetNodeLifeTimes(node.EditorNodeId, ref frameStart, ref frameLast))
					{
						// エディタ起動直後など、Native 側のインスタンスが作られていないことがある。
						// その場合は空のタイムラインにしておく。
					}

					Manager.NativeManager.TimelineNode(node.Name, (int)frameStart, (int)frameLast);
				}

				{
					int currentFrame = (int)Manager.Viewer.Current;
					int selectedIndex = currentSelectedIndex;
					Manager.NativeManager.EndNodeFrameTimeline(ref frameMin, ref frameMax, ref currentFrame, ref selectedIndex, ref _scrollFirstFrame);

					// Selection changed?
					if (selectedIndex != currentSelectedIndex)
					{
						Core.SelectedNode = timelineNodes[selectedIndex];
					}

					Core.StartFrame = frameMin;
					Core.EndFrame = frameMax;
					Manager.Viewer.Current = currentFrame;
					Manager.Viewer.Current = System.Math.Max(Manager.Viewer.Current, Core.StartFrame);
					Manager.Viewer.Current = System.Math.Min(Manager.Viewer.Current, Core.EndFrame);
				}
			}
		}

		void OnAfter(object sender, EventArgs e)
		{
			_scrollFirstFrame = 0;
		}
	}
}
