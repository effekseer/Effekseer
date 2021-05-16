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

				for (int i = 0; i < Core.Root.Children.Count; i++)
				{
					var node = Core.Root.Children[i];

					var common = node.CommonValues;

					var frameStart = common.GenerationTimeOffset.Min;


					var nodeLife = common.Life.Max;
					var instanceTime = common.GenerationTime.Max;

					var instanceCount = common.MaxGeneration.Value.Value;


					var instanceOneOffset = instanceTime;//common.GenerationTimeOffset.Max - common.GenerationTimeOffset.Min;

					var instanceMaxTime = (instanceOneOffset * (instanceCount - 1)) + common.Life.Max;//common.GenerationTime.Max;


					var instanceOffsetLast = frameStart + instanceMaxTime;


					// Nodeの開始オフセットの終端 + 
					var frameLast = instanceOffsetLast;//common.GenerationTimeOffset.Max + common.Life.Max;

					if (node.CommonValues.MaxGeneration.Infinite)
					{
						frameLast = frameMax;
					}
					else
					{
						var coreLife = node.CommonValues.MaxGeneration.Value;

						frameLast = Math.Max(frameStart, frameLast);

						Manager.NativeManager.TimelineNode(node.Name, (int)frameStart, (int)frameLast);
					}

				}



				{

					int currentFrame = (int)Manager.Viewer.Current;
					int selectedEntry = 0;
					Manager.NativeManager.EndNodeFrameTimeline(ref frameMin, ref frameMax, ref currentFrame, ref selectedEntry, ref _scrollFirstFrame);

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
