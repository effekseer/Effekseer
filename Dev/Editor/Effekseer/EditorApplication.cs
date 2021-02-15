using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Effekseer.GUI;

namespace Effekseer
{
	public class App : Application
	{
		protected override void OnInitialize()
		{
			System.OperatingSystem os = System.Environment.OSVersion;
			swig.DeviceType deviceType = swig.DeviceType.DirectX11;

			if (!(os.Platform == PlatformID.Win32NT ||
			os.Platform == PlatformID.Win32S ||
			os.Platform == PlatformID.Win32Windows ||
			os.Platform == PlatformID.WinCE))
			{
				deviceType = swig.DeviceType.OpenGL;
			}

			if (!GUI.Manager.Initialize(960, 540, deviceType))
			{
				return;
			}
		}

		protected override void OnUpdate()
		{
			GUI.Manager.Update();
		}

		protected override void OnTerminate()
		{
			GUI.Manager.Terminate();
			Process.MaterialEditor.Terminate();
		}

		protected override void OnResetWindowActually()
		{
			if (GUIManager.effectViewer == null)
			{
				GUIManager.effectViewer = new GUI.Dock.EffectViwer();
				if (GUIManager.dockManager != null)
				{
					GUIManager.dockManager.Controls.Add(GUIManager.effectViewer);
				}
			}

			var viewerController = GUIManager.SelectOrShowWindow(typeof(GUI.Dock.ViewerController), null);
			var nodeTreeView = GUIManager.SelectOrShowWindow(typeof(GUI.Dock.NodeTreeView), null);
			var commonValues = GUIManager.SelectOrShowWindow(typeof(GUI.Dock.CommonValues), null);
			var locationValues = GUIManager.SelectOrShowWindow(typeof(GUI.Dock.LocationValues), null);
			var rotationValues = GUIManager.SelectOrShowWindow(typeof(GUI.Dock.RotationValues), null);
			var scaleValues = GUIManager.SelectOrShowWindow(typeof(GUI.Dock.ScaleValues), null);
			var rendererCommonValues = GUIManager.SelectOrShowWindow(typeof(GUI.Dock.RendererCommonValues), null);
			var rendererValues = GUIManager.SelectOrShowWindow(typeof(GUI.Dock.RendererValues), null);

			uint windowId = GUIManager.NativeManager.BeginDockLayout();

			uint dockLeftID = 0, dockRightID = 0;
			GUIManager.NativeManager.DockSplitNode(windowId, swig.DockSplitDir.Left, 0.65f, ref dockLeftID, ref dockRightID);

			uint dockLeftTop = 0, dockLeftBottom = 0;
			GUIManager.NativeManager.DockSplitNode(dockLeftID, swig.DockSplitDir.Top, 0.85f, ref dockLeftTop, ref dockLeftBottom);

			uint dockRightTop = 0, dockRightBottom = 0;
			GUIManager.NativeManager.DockSplitNode(dockRightID, swig.DockSplitDir.Top, 0.6f, ref dockRightTop, ref dockRightBottom);

			GUIManager.NativeManager.DockSetNodeFlags(dockLeftTop, swig.DockNodeFlags.HiddenTabBar);
			GUIManager.NativeManager.DockSetNodeFlags(dockLeftBottom, swig.DockNodeFlags.HiddenTabBar);

			GUIManager.NativeManager.DockSetWindow(dockLeftTop, GUIManager.effectViewer.WindowID);
			GUIManager.NativeManager.DockSetWindow(dockLeftBottom, viewerController.WindowID);
			GUIManager.NativeManager.DockSetWindow(dockRightTop, commonValues.WindowID);
			GUIManager.NativeManager.DockSetWindow(dockRightTop, locationValues.WindowID);
			GUIManager.NativeManager.DockSetWindow(dockRightTop, rotationValues.WindowID);
			GUIManager.NativeManager.DockSetWindow(dockRightTop, scaleValues.WindowID);
			GUIManager.NativeManager.DockSetWindow(dockRightTop, rendererValues.WindowID);
			GUIManager.NativeManager.DockSetWindow(dockRightTop, rendererCommonValues.WindowID);
			GUIManager.NativeManager.DockSetWindow(dockRightBottom, nodeTreeView.WindowID);

			GUIManager.NativeManager.EndDockLayout();
		}
	}
}
