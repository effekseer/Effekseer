using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Effekseer.swig;

namespace Effekseer.GUI.Dock
{
	public class EffectViwerPaneBase : DockPanel
	{
		public bool IsHovered = false;

		protected BindableComponent.Enum renderMode;
		protected BindableComponent.Enum viewMode;

		swig.DeviceType deviceType;

		public EffectViwerPaneBase(swig.DeviceType deviceType)
		{
			Label = MultiLanguageTextProvider.GetText("Viewer") + "###Viewer";
			renderMode = new BindableComponent.Enum();
			renderMode.Initialize(typeof(Data.OptionValues.RenderMode));
			renderMode.SetBinding(Core.Option.RenderingMode);
			renderMode.EnableUndo = false;
			viewMode = new BindableComponent.Enum();
			viewMode.Initialize(typeof(Data.OptionValues.ViewMode));
			viewMode.SetBinding(Core.Option.ViewerMode);
			viewMode.EnableUndo = false;

			NoPadding = true;
			NoScrollBar = true;
			NoCloseButton = true;
			AllowsShortTab = false;

			this.deviceType = deviceType;
		}

		protected void DrawMainImage(swig.Vec2 contentSize, float frameHeight, float padding)
		{
			// Menu
			contentSize.X = System.Math.Max(1, contentSize.X);
			contentSize.Y = System.Math.Max(1, contentSize.Y - frameHeight - padding);

			Manager.Viewer.ViewPointController.SetScreenSize((int)contentSize.X, (int)contentSize.Y);
			Manager.Viewer.ViewPointController.Update();

			var ray = Manager.Viewer.ViewPointController.GetCameraRay();

			var renderParam = Manager.Viewer.EffectRenderer.GetParameter();
			renderParam.CameraMatrix = Manager.Viewer.ViewPointController.GetCameraMatrix();
			renderParam.ProjectionMatrix = Manager.Viewer.ViewPointController.GetProjectionMatrix();
			renderParam.CameraPosition = ray.Origin;
			renderParam.CameraFrontDirection = ray.Direction;
			Manager.Viewer.EffectRenderer.SetParameter(renderParam);

			Manager.Viewer.EffectRenderer.ResizeScreen(Manager.Viewer.ViewPointController.GetScreenSize());
			Manager.MainViewImage.Resize((int)contentSize.X, (int)contentSize.Y);

			Manager.Viewer.EffectRenderer.Render(Manager.MainViewImage);

			if (deviceType == swig.DeviceType.OpenGL)
			{
				Manager.NativeManager.ImageData(Manager.MainViewImage, (int)contentSize.X, (int)contentSize.Y, 0, 1, 1, 0);
			}
			else
			{
				Manager.NativeManager.ImageData(Manager.MainViewImage, (int)contentSize.X, (int)contentSize.Y);
			}
		}

		private static int getLodIndexFromLodBit(int lodBits)
		{
			if (lodBits == 0)
			{
				return 0;
			}
			return (int)((Math.Log10(lodBits & -lodBits)) / Math.Log10(2));
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
			DrawMainImage(contentSize, frameHeight, padding);

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

			
			string perfText =
				"Current LOD: " + getLodIndexFromLodBit(Manager.Viewer.EffectRenderer.GetCurrentLOD()) + "  " + 
				"D:" + Manager.Viewer.EffectRenderer.GetAndResetDrawCall().ToString("D3") + "  " +
				"V:" + Manager.Viewer.EffectRenderer.GetAndResetVertexCount().ToString("D5") + "  " +
				"P:" + Manager.Viewer.EffectRenderer.GetInstanceCount().ToString("D5") + " ";

			Manager.NativeManager.SameLine(contentSize.X - Manager.NativeManager.CalcTextSize(perfText).X);

			// Display performance information
			Manager.NativeManager.Text(perfText);
			if (Manager.NativeManager.IsItemHovered())
			{
				Manager.NativeManager.SetTooltip(
					"Current LOD: Level of Detail is currently utilized\n" +
					"D: Draw calls of current rendering.\n" +
					"V: Vertex count of current rendering.\n" +
					"P: Particle count of current rendering.");
			}
		}
	}
}