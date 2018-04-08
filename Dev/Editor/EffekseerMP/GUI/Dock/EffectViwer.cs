using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class EffectViwer : DockPanel
	{
		public bool IsHovered = false;

		public EffectViwer()
		{
			Label = Resources.GetString("Viewer");
		}

		protected override void UpdateInternal()
		{
			IsHovered = false;

			var windowSize = Manager.NativeManager.GetWindowSize();

			// Menu
			windowSize.Y = System.Math.Max(1, windowSize.Y - 40);

			var p = Manager.Native.RenderView((int)windowSize.X, (int)windowSize.Y);
			Manager.NativeManager.Image(p, (int)windowSize.X, (int)windowSize.Y);

			IsHovered = Manager.NativeManager.IsWindowHovered();
		}
	}
}
