using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Effekseer.GUI;

namespace Effekseer
{
	using Dock = Effekseer.GUI.Dock;

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
				throw new InvalidOperationException("Initialization failed.");
			}
		}

		protected override void OnUpdate()
		{
			Manager.Update();
		}

		protected override void OnTerminate()
		{
			Manager.Terminate();
			Process.MaterialEditor.Terminate();
		}

		protected override void OnResetWindowActually()
		{
			if (Manager.effectViewer == null)
			{
				Manager.effectViewer = new GUI.Dock.EffectViwer();
				if (Manager.dockManager != null)
				{
					Manager.dockManager.Controls.Add(Manager.effectViewer);
				}
			}

			var viewerController = Manager.SelectOrShowWindow(typeof(GUI.Dock.ViewerController), null);
			var nodeTreeView = Manager.SelectOrShowWindow(typeof(GUI.Dock.NodeTreeView), null);
			var commonValues = Manager.SelectOrShowWindow(typeof(GUI.Dock.CommonValues), null);
			var locationValues = Manager.SelectOrShowWindow(typeof(GUI.Dock.LocationValues), null);
			var rotationValues = Manager.SelectOrShowWindow(typeof(GUI.Dock.RotationValues), null);
			var scaleValues = Manager.SelectOrShowWindow(typeof(GUI.Dock.ScaleValues), null);
			var rendererCommonValues = Manager.SelectOrShowWindow(typeof(GUI.Dock.RendererCommonValues), null);
			var rendererValues = Manager.SelectOrShowWindow(typeof(GUI.Dock.RendererValues), null);

			uint windowId = Manager.NativeManager.BeginDockLayout();

			uint dockLeftID = 0, dockRightID = 0;
			Manager.NativeManager.DockSplitNode(windowId, swig.DockSplitDir.Left, 0.65f, ref dockLeftID, ref dockRightID);

			uint dockLeftTop = 0, dockLeftBottom = 0;
			Manager.NativeManager.DockSplitNode(dockLeftID, swig.DockSplitDir.Top, 0.85f, ref dockLeftTop, ref dockLeftBottom);

			uint dockRightTop = 0, dockRightBottom = 0;
			Manager.NativeManager.DockSplitNode(dockRightID, swig.DockSplitDir.Top, 0.6f, ref dockRightTop, ref dockRightBottom);

			Manager.NativeManager.DockSetNodeFlags(dockLeftTop, swig.DockNodeFlags.HiddenTabBar);
			Manager.NativeManager.DockSetNodeFlags(dockLeftBottom, swig.DockNodeFlags.HiddenTabBar);

			Manager.NativeManager.DockSetWindow(dockLeftTop, Manager.effectViewer.WindowID);
			Manager.NativeManager.DockSetWindow(dockLeftBottom, viewerController.WindowID);
			Manager.NativeManager.DockSetWindow(dockRightTop, commonValues.WindowID);
			Manager.NativeManager.DockSetWindow(dockRightTop, locationValues.WindowID);
			Manager.NativeManager.DockSetWindow(dockRightTop, rotationValues.WindowID);
			Manager.NativeManager.DockSetWindow(dockRightTop, scaleValues.WindowID);
			Manager.NativeManager.DockSetWindow(dockRightTop, rendererValues.WindowID);
			Manager.NativeManager.DockSetWindow(dockRightTop, rendererCommonValues.WindowID);
			Manager.NativeManager.DockSetWindow(dockRightBottom, nodeTreeView.WindowID);

			Manager.NativeManager.EndDockLayout();
		}
	}
}
