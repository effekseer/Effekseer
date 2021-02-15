using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	public class EffectViwer : DockPanel
	{
		public bool IsHovered = false;



		Component.Enum renderMode;
		Component.Enum viewMode;

		public EffectViwer()
		{
			Label = Resources.GetString("Viewer") + "###Viewer";
			renderMode = new Component.Enum();
			renderMode.Initialize(typeof(Data.OptionValues.RenderMode));
			renderMode.SetBinding(Core.Option.RenderingMode);
			renderMode.EnableUndo = false;
			viewMode = new Component.Enum();
			viewMode.Initialize(typeof(Data.OptionValues.ViewMode));
			viewMode.SetBinding(Core.Option.ViewerMode);
			viewMode.EnableUndo = false;

			NoPadding = true;
			NoScrollBar = true;
			NoCloseButton = true;
			AllowsShortTab = false;
		}

		protected override void UpdateInternal()
		{
			float textHeight = GUIManager.NativeManager.GetTextLineHeight();
			float frameHeight = GUIManager.NativeManager.GetFrameHeightWithSpacing();
			float dpiScale = GUIManager.DpiScale;
			float padding = 4.0f * dpiScale;

			IsHovered = false;

			var contentSize = GUIManager.NativeManager.GetContentRegionAvail();

			// Menu
			contentSize.X = System.Math.Max(1, contentSize.X);
			contentSize.Y = System.Math.Max(1, contentSize.Y - frameHeight - padding);

			var p = GUIManager.Native.RenderView((int)contentSize.X, (int)contentSize.Y);
			GUIManager.NativeManager.Image(p, (int)contentSize.X, (int)contentSize.Y);

			IsHovered = GUIManager.NativeManager.IsWindowHovered();

			GUIManager.NativeManager.Indent(padding);

			// Enum
			GUIManager.NativeManager.PushItemWidth(textHeight * 7.0f);
			renderMode.Update();
			GUIManager.NativeManager.PopItemWidth();

			GUIManager.NativeManager.SameLine();

			GUIManager.NativeManager.PushItemWidth(textHeight * 2.5f);
			viewMode.Update();
			GUIManager.NativeManager.PopItemWidth();

			string perfText = 
				"D:" + GUIManager.Native.GetAndResetDrawCall().ToString("D3") + "  " + 
				"V:" + GUIManager.Native.GetAndResetVertexCount().ToString("D5") + "  " +
				"P:" + GUIManager.Native.GetInstanceCount().ToString("D5") + " ";

			GUIManager.NativeManager.SameLine(contentSize.X - GUIManager.NativeManager.CalcTextSize(perfText).X);

			// Display performance information
			GUIManager.NativeManager.Text(perfText);
			if (GUIManager.NativeManager.IsItemHovered())
			{
				GUIManager.NativeManager.SetTooltip(
					"D: Draw calls of current rendering.\n" +
					"V: Vertex count of current rendering.\n" +
					"P: Particle count of current rendering.");
			}
		}
	}
}
