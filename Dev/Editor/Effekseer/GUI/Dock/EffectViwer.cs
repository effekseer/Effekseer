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

			NoPadding = true;
			NoScrollBar = true;
		}

		protected override void UpdateInternal()
		{
			float dpiScale = Manager.DpiScale;

			IsHovered = false;

			var contentSize = Manager.NativeManager.GetContentRegionAvail();

			// Menu
			contentSize.X = System.Math.Max(1, contentSize.X);
			contentSize.Y = System.Math.Max(1, contentSize.Y - 30 * dpiScale);

			var p = Manager.Native.RenderView((int)contentSize.X, (int)contentSize.Y);
			Manager.NativeManager.Image(p, (int)contentSize.X, (int)contentSize.Y);

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

			Manager.NativeManager.Spacing();
		}
	}
}
