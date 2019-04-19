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



		Component.Enum renderMode;

		public EffectViwer()
		{
			Label = Resources.GetString("Viewer") + "###Viewer";
			renderMode = new Component.Enum();
			renderMode.Initialize(typeof(Data.OptionValues.RenderMode));
			renderMode.SetBinding(Core.Option.RenderingMode);
		}

		protected override void UpdateInternal()
		{
			IsHovered = false;

			var windowSize = Manager.NativeManager.GetWindowSize();

			// Menu
			windowSize.X = System.Math.Max(1, windowSize.X - 20);
			windowSize.Y = System.Math.Max(1, windowSize.Y - 50);

			var p = Manager.Native.RenderView((int)windowSize.X, (int)windowSize.Y);
			Manager.NativeManager.Image(p, (int)windowSize.X, (int)windowSize.Y);

			IsHovered = Manager.NativeManager.IsWindowHovered();

			// Enum
			renderMode.Update();

			Manager.NativeManager.SameLine();

			// DrawCall
			Manager.NativeManager.Text("Draw : " + Manager.Native.GetAndResetDrawCall().ToString());

			Manager.NativeManager.SameLine();

			// DrawCall
			Manager.NativeManager.Text("Vertex : " + Manager.Native.GetAndResetVertexCount().ToString());

			Manager.NativeManager.SameLine();

			// DrawCall
			Manager.NativeManager.Text("Particle : " + Manager.Native.GetInstanceCount().ToString());

		}
	}
}
