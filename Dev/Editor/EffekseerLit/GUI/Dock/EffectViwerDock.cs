using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class EffectViwerDock : EffectViwerPaneBase
	{
		public EffectViwerDock()
		{
		}


		protected override void UpdateInternal()
		{
			float textHeight = Manager.NativeManager.GetTextLineHeight();
			float frameHeight = Manager.NativeManager.GetFrameHeightWithSpacing();
			float dpiScale = Manager.DpiScale;
			float padding = 4.0f * dpiScale;

			IsHovered = false;

			var contentSize = Manager.NativeManager.GetContentRegionAvail();

			// Menu
			contentSize.X = System.Math.Max(1, contentSize.X);
			contentSize.Y = System.Math.Max(1, contentSize.Y - frameHeight - padding);

			var p = Manager.Native.RenderView((int)contentSize.X, (int)contentSize.Y);
			Manager.NativeManager.Image(p, (int)contentSize.X, (int)contentSize.Y);

			if (Manager.NativeManager.BeginDragDropTarget())
			{
				int size = 0;
				byte[] data = new byte[5];
				if (Manager.NativeManager.AcceptDragDropPayload("PayloadName", data, data.Length, ref size))
				{
					// TODO: Test. D&D を受けたらからノードを追加してみる。ここを Prefab にしたいところ。
					Core.Root.AddChild();
				}

				Manager.NativeManager.EndDragDropTarget();
			}

			IsHovered = Manager.NativeManager.IsWindowHovered();

			Manager.NativeManager.Indent(padding);

			// Enum
			Manager.NativeManager.PushItemWidth(textHeight * 7.0f);
			renderMode.Update();
			Manager.NativeManager.PopItemWidth();

			Manager.NativeManager.SameLine();

			Manager.NativeManager.PushItemWidth(textHeight * 2.5f);
			viewMode.Update();
			Manager.NativeManager.PopItemWidth();

			// Display performance information
			if (Manager.NativeManager.IsItemHovered())
			{
				Manager.NativeManager.SetTooltip(
					"D: Draw calls of current rendering.\n" +
					"V: Vertex count of current rendering.\n" +
					"P: Particle count of current rendering.");
			}
		}
	}
}
