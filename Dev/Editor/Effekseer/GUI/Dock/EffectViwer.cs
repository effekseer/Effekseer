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

			Manager.NativeManager.Indent(4 * dpiScale);

			// Enum
			Manager.NativeManager.PushItemWidth(120 * dpiScale);
			renderMode.Update();
			Manager.NativeManager.PopItemWidth();

			Manager.NativeManager.SameLine();

			Manager.NativeManager.PushItemWidth(50 * dpiScale);
			viewMode.Update();
			Manager.NativeManager.PopItemWidth();

			Manager.NativeManager.SameLine(contentSize.X - 170 * dpiScale);

			Manager.NativeManager.PushItemWidth(50 * dpiScale);

			// DrawCall
			Manager.NativeManager.InputText("##DrawCalls", "D:" + Manager.Native.GetAndResetDrawCall().ToString(), swig.InputTextFlags.ReadOnly);
			if (Manager.NativeManager.IsItemHovered())
			{
				Manager.NativeManager.SetTooltip("Draw calls of current rendering.");
			}

			Manager.NativeManager.SameLine();

			// DrawCall
			Manager.NativeManager.InputText("##VertexCount", "V:" + Manager.Native.GetAndResetVertexCount().ToString(), swig.InputTextFlags.ReadOnly);
			if (Manager.NativeManager.IsItemHovered())
			{
				Manager.NativeManager.SetTooltip("Vertex count of current rendering.");
			}
   
			Manager.NativeManager.SameLine();

			// DrawCall
			Manager.NativeManager.InputText("##ParticleCount", "P:" + Manager.Native.GetInstanceCount().ToString());
			if (Manager.NativeManager.IsItemHovered())
			{
				Manager.NativeManager.SetTooltip("Particle count of current rendering.");
			}

			Manager.NativeManager.PopItemWidth();

			Manager.NativeManager.Spacing();
		}
	}
}
