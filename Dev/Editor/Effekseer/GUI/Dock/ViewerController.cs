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
			Label = Icons.PanelViewerCtrl + MultiLanguageTextProvider.GetText("ViewerControls") + "###ViewerControls";
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

			{
				float spacing = Manager.NativeManager.GetStyleVar2(swig.ImGuiStyleVarFlags.ItemSpacing).X;
				float buttonWidth = Manager.NativeManager.GetTextLineHeightWithSpacing();

				for (int i = 0; i < 4; i++)
				{
					Manager.NativeManager.SameLine(0.0f, i == 0 ? spacing * 1.5f : spacing * 0.5f);

					if (Manager.NativeManager.Button(i.ToString(), buttonWidth))
					{
						Manager.Viewer.SendTrigger(i);
					}

					if (BindableComponent.Functions.CanShowTip())
					{
						Manager.NativeManager.SetTooltip(MultiLanguageTextProvider.GetText("SendTrigger_Desc"));
					}
				}
			}

			Manager.NativeManager.SameLine();
			{
				
				float[] bias = { Manager.Viewer.LODDistanceBias };
				Manager.NativeManager.SetNextItemWidth(120F);
				if (Manager.NativeManager.DragFloat(MultiLanguageTextProvider.GetText("LOD_Bias_Name"), bias))
				{
					Manager.Viewer.LODDistanceBias = bias[0];
				}

				if (Manager.NativeManager.IsItemHovered())
				{
					Manager.NativeManager.SetTooltip(MultiLanguageTextProvider.GetText("LOD_Bias_Description"));
				}
			}

			UpdateTransformButtons();
		}

		/// <summary>
		/// エフェクト全体を反転するボタン。
		/// 各ノードを手作業で編集せずに済むよう、プレビューの隣に置く。
		/// </summary>
		void UpdateTransformButtons()
		{
			Manager.NativeManager.Separator();

			float spacing = Manager.NativeManager.GetStyleVar2(swig.ImGuiStyleVarFlags.ItemSpacing).X;
			float buttonWidth = Manager.NativeManager.GetTextLineHeightWithSpacing() * 1.4f;

			Manager.NativeManager.Text(MultiLanguageTextProvider.GetText("Transform_Flip_Label"));

			FlipButton("X###FlipX", Commands.FlipX, spacing);
			FlipButton("Y###FlipY", Commands.FlipY, spacing);
			FlipButton("Z###FlipZ", Commands.FlipZ, spacing);

			Manager.NativeManager.SameLine(0.0f, spacing * 2.0f);

			if (Manager.NativeManager.Button(MultiLanguageTextProvider.GetText("InternalReverseTimeline")))
			{
				Commands.ReverseTimeline();
			}

			if (Manager.NativeManager.IsItemHovered())
			{
				Manager.NativeManager.SetTooltip(MultiLanguageTextProvider.GetText("Transform_ReverseTimeline_Desc"));
			}

			void FlipButton(string label, Func<bool> command, float gap)
			{
				Manager.NativeManager.SameLine(0.0f, gap);

				if (Manager.NativeManager.Button(label, buttonWidth))
				{
					command();
				}

				if (Manager.NativeManager.IsItemHovered())
				{
					Manager.NativeManager.SetTooltip(MultiLanguageTextProvider.GetText("Transform_Flip_Desc"));
				}
			}
		}
	}
}