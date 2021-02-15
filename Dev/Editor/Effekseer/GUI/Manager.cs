using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Effekseer.swig;
using Effekseer.Utl;

namespace Effekseer.GUI
{

	public class Manager : GUIManager
	{




		public static bool Initialize(int width, int height, swig.DeviceType deviceType)
		{
			dockTypes = new Type[]
			{
				typeof(Dock.ViewerController),
				typeof(Dock.NodeTreeView),
				typeof(Dock.CommonValues),
				typeof(Dock.LocationValues),
				typeof(Dock.LocationAbsValues),
				typeof(Dock.GenerationLocationValues),
				typeof(Dock.RotationValues),
				typeof(Dock.ScaleValues),
				typeof(Dock.DepthValues),
				typeof(Dock.RendererCommonValues),
				typeof(Dock.RendererValues),
				typeof(Dock.SoundValues),
				typeof(Dock.FCurves),
				typeof(Dock.ViewPoint),
				typeof(Dock.Recorder),
				typeof(Dock.Option),
				typeof(Dock.Environement),
				typeof(Dock.GlobalValues),
				typeof(Dock.BehaviorValues),
				typeof(Dock.Culling),
				typeof(Dock.Network),
				typeof(Dock.FileViewer),
				typeof(Dock.Dynamic),
				typeof(Dock.ProcedualModel),
				typeof(Dock.AdvancedRenderCommonValues),
			};

			if (!InitializeBase(width, height, deviceType, () => new GUI.Menu.MainMenu())) return false;

			return true;
		}

		static swig.Vec2 mousePos_pre;

		static bool isFirstUpdate = true;

		protected static int LEFT_SHIFT = 340;
		protected static int RIGHT_SHIFT = 344;

		protected static int LEFT_CONTROL = 341;
		protected static int RIGHT_CONTROL = 345;

		protected static int LEFT_ALT = 342;
		protected static int RIGHT_ALT = 346;

		protected static int LEFT_SUPER = 343;
		protected static int RIGHT_SUPER = 347;

		struct ViewportControllerResult
		{
			public bool Rotate;
			public bool Slide;
			public bool Zoom;
		}

		static ViewportControllerResult ControllViewport()
		{
			bool isLeftPressed = NativeManager.GetMouseButton(0) > 0;
			bool isRightPressed = NativeManager.GetMouseButton(1) > 0;
			bool isWheelPressed = NativeManager.GetMouseButton(2) > 0;

			bool isShiftPressed = Manager.NativeManager.IsKeyDown(LEFT_SHIFT) || Manager.NativeManager.IsKeyDown(RIGHT_SHIFT);
			bool isCtrlPressed = Manager.NativeManager.IsKeyDown(LEFT_CONTROL) || Manager.NativeManager.IsKeyDown(RIGHT_CONTROL);
			bool isAltPressed = Manager.NativeManager.IsKeyDown(LEFT_ALT) || Manager.NativeManager.IsKeyDown(RIGHT_ALT);
			bool isSuperPressed = Manager.NativeManager.IsKeyDown(LEFT_SUPER) || Manager.NativeManager.IsKeyDown(RIGHT_SUPER);

			bool isSlidePressed = false;
			bool isZoomPressed = false;
			bool isRotatePressed = false;

			switch (Core.Option.MouseMappingType.Value)
			{
				case Data.MouseMappingType.Effekseer:
					isSlidePressed = isWheelPressed || (isRightPressed && isShiftPressed);
					isZoomPressed = isRightPressed && isCtrlPressed;
					isRotatePressed = isRightPressed;
					break;

				case Data.MouseMappingType.Blender:
					isSlidePressed = isWheelPressed && isShiftPressed;
					isZoomPressed = isWheelPressed && isCtrlPressed;
					isRotatePressed = isWheelPressed;
					break;

				case Data.MouseMappingType.Maya:
					isSlidePressed = isWheelPressed && isAltPressed;
					isZoomPressed = isRightPressed && isAltPressed;
					isRotatePressed = isLeftPressed && isAltPressed;
					break;

				case Data.MouseMappingType.Unity:
					isSlidePressed = isRightPressed;
					isZoomPressed = false;
					isRotatePressed = isWheelPressed;
					break;
			}

			ViewportControllerResult result;

			result.Rotate = isRotatePressed;
			result.Zoom = isZoomPressed;
			result.Slide = isSlidePressed;

			return result;
		}

		public static void Update()
		{
			if (isFontSizeDirtied)
			{
				NativeManager.InvalidateFont();
				var appDirectory = Application.EntryDirectory;
				var type = Core.Option.Font.Value;

				NativeManager.ClearAllFonts();

				if (type == Data.FontType.Normal)
				{
					NativeManager.AddFontFromFileTTF(System.IO.Path.Combine(appDirectory, MultiLanguageTextProvider.GetText("Font_Normal")), Core.Option.FontSize.Value);
				}
				else if (type == Data.FontType.Bold)
				{
					NativeManager.AddFontFromFileTTF(System.IO.Path.Combine(appDirectory, MultiLanguageTextProvider.GetText("Font_Bold")), Core.Option.FontSize.Value);
				}

				NativeManager.AddFontFromAtlasImage(System.IO.Path.Combine(appDirectory, "resources/icons/MenuIcons.png"), 0xec00, 24, 24, 16, 16);

				isFontSizeDirtied = false;
			}

			// Reset

			IO.Update();
			Shortcuts.Update();
			Network.Update();

			var handle = false;
			if(!handle)
			{
				if (!NativeManager.IsAnyItemActive())
				{
					Shortcuts.ProcessCmdKey(ref handle);
				}
			}

			var mousePos = NativeManager.GetMousePosition();

			if (isFirstUpdate)
			{
				mousePos_pre = mousePos;
			}

			if((effectViewer == null && !NativeManager.IsAnyWindowHovered()) || (effectViewer != null && effectViewer.IsHovered))
			{
				var result = ControllViewport();

				if (result.Slide)
				{
					var dx = mousePos.X - mousePos_pre.X;
					var dy = mousePos.Y - mousePos_pre.Y;

					if (Core.Option.ViewerMode.Value == Data.OptionValues.ViewMode._3D)
					{
						Viewer.Slide(dx / 30.0f, dy / 30.0f);
					}
					else if (Core.Option.ViewerMode.Value == Data.OptionValues.ViewMode._2D)
					{
						Viewer.Slide(dx / 16.0f, dy / 16.0f);
					}
				}
				else if (NativeManager.GetMouseWheel() != 0)
				{
					Viewer.Zoom(NativeManager.GetMouseWheel());
				}
				else if (result.Zoom)
				{
					var dx = mousePos.X - mousePos_pre.X;
					var dy = mousePos.Y - mousePos_pre.Y;
					Viewer.Zoom(-dy * 0.25f);
				}
				else if (result.Rotate)
				{
					var dx = mousePos.X - mousePos_pre.X;
					var dy = mousePos.Y - mousePos_pre.Y;

					if (Core.Option.ViewerMode.Value == Data.OptionValues.ViewMode._3D)
					{
						Viewer.Rotate(dx, dy);
					}
					else if (Core.Option.ViewerMode.Value == Data.OptionValues.ViewMode._2D)
					{
						Viewer.Slide(dx / 16.0f, dy / 16.0f);
					}
				}
			}

			mousePos_pre = mousePos;

			Viewer.UpdateViewer();

			Native.UpdateWindow();

			Native.ClearWindow(50, 50, 50, 0);

			//if(effectViewer == null)
			//{
			//	Native.RenderWindow();
			//}

			NativeManager.ResetGUI();

			if (resetCount < 0)
			{
				resetCount++;
				if (resetCount == 0)
				{
					ResetWindowActually();
				}
			}

			if(resizedCount > 0)
			{
				resizedCount--;
			}

			Controls.Lock();

			foreach (var c in Controls.Internal)
			{
				c.Update();
			}

			foreach(var _ in Controls.Internal)
			{
				if (!_.ShouldBeRemoved) continue;

				var dp = _ as Dock.DockPanel;
				if(dp != null)
				{
					dp.DispatchDisposed();
				}
			}

			foreach (var _ in Controls.Internal)
			{
				if (!_.ShouldBeRemoved) continue;
				Controls.Remove(_);
			}

			Controls.Unlock();

			for (int i = 0; i < dockTypes.Length; i++)
			{
				if (panels[i] != null && panels[i].ShouldBeRemoved)
				{
					panels[i] = null;
				}
			}

			NativeManager.RenderGUI(resizedCount == 0);

			Native.Present();
			NativeManager.Present();

			isFirstUpdate = false;

			// TODO more smart
			// When minimized, suppress CPU activity
			if (actualWidth == 0)
			{
				System.Threading.Thread.Sleep(16);
			}
		}

		static void ResetWindowActually()
		{
			if (effectViewer == null)
			{
				effectViewer = new Dock.EffectViwer();
				if (dockManager != null)
				{
					dockManager.Controls.Add(effectViewer);
				}
			}
			
			var viewerController = SelectOrShowWindow(typeof(Dock.ViewerController), null);
			var nodeTreeView = SelectOrShowWindow(typeof(Dock.NodeTreeView), null);
			var commonValues = SelectOrShowWindow(typeof(Dock.CommonValues), null);
			var locationValues = SelectOrShowWindow(typeof(Dock.LocationValues), null);
			var rotationValues = SelectOrShowWindow(typeof(Dock.RotationValues), null);
			var scaleValues = SelectOrShowWindow(typeof(Dock.ScaleValues), null);
			var rendererCommonValues = SelectOrShowWindow(typeof(Dock.RendererCommonValues), null);
			var rendererValues = SelectOrShowWindow(typeof(Dock.RendererValues), null);

			uint windowId = NativeManager.BeginDockLayout();

			uint dockLeftID = 0, dockRightID = 0;
			NativeManager.DockSplitNode(windowId, DockSplitDir.Left, 0.65f, ref dockLeftID, ref dockRightID);

			uint dockLeftTop = 0, dockLeftBottom = 0;
			NativeManager.DockSplitNode(dockLeftID, DockSplitDir.Top, 0.85f, ref dockLeftTop, ref dockLeftBottom);

			uint dockRightTop = 0, dockRightBottom = 0;
			NativeManager.DockSplitNode(dockRightID, DockSplitDir.Top, 0.6f, ref dockRightTop, ref dockRightBottom);

			NativeManager.DockSetNodeFlags(dockLeftTop, DockNodeFlags.HiddenTabBar);
			NativeManager.DockSetNodeFlags(dockLeftBottom, DockNodeFlags.HiddenTabBar);

			NativeManager.DockSetWindow(dockLeftTop, effectViewer.WindowID);
			NativeManager.DockSetWindow(dockLeftBottom, viewerController.WindowID);
			NativeManager.DockSetWindow(dockRightTop, commonValues.WindowID);
			NativeManager.DockSetWindow(dockRightTop, locationValues.WindowID);
			NativeManager.DockSetWindow(dockRightTop, rotationValues.WindowID);
			NativeManager.DockSetWindow(dockRightTop, scaleValues.WindowID);
			NativeManager.DockSetWindow(dockRightTop, rendererValues.WindowID);
			NativeManager.DockSetWindow(dockRightTop, rendererCommonValues.WindowID);
			NativeManager.DockSetWindow(dockRightBottom, nodeTreeView.WindowID);

			NativeManager.EndDockLayout();
		}
	}
}
