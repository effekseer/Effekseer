using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Effekseer.GUI;
using Effekseer.GUI.Dock;
using Effekseer.swig;

namespace Effekseer
{
	using Dock = Effekseer.GUI.Dock;

	public class App : Application
	{
		swig.DeviceType deviceType;

		protected override void OnInitialize()
		{
			var dockTypes = new Type[]
			{
				typeof(Dock.ViewerController),
				typeof(Dock.NodeTreeView),
				typeof(Dock.CommonValues),
				typeof(Dock.LocationValues),
				typeof(Dock.LocationAbsValues),
				typeof(Dock.GenerationLocationValues),
				typeof(Dock.KillRules),
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
				typeof(Dock.LOD),
				typeof(Dock.Network),
				typeof(Dock.FileBrowser),
				typeof(Dock.Dynamic),
				typeof(Dock.ProceduralModel),
				typeof(Dock.AdvancedRenderCommonValues),

				typeof(GUI.Inspector.InspectorPanel),
				typeof(DockPanels.LocationValues),
				typeof(DockPanels.RotationValues),
				typeof(Dock.NodeTreeEditor),
				typeof(Dock.Profiler),
			};

			System.OperatingSystem os = System.Environment.OSVersion;
			deviceType = swig.DeviceType.DirectX11;

			if (!(os.Platform == PlatformID.Win32NT ||
			os.Platform == PlatformID.Win32S ||
			os.Platform == PlatformID.Win32Windows ||
			os.Platform == PlatformID.WinCE))
			{
				deviceType = swig.DeviceType.OpenGL;
			}

			if (!GUI.Manager.Initialize(960, 540, deviceType, dockTypes))
			{
				throw new InvalidOperationException("Initialization failed.");
			}
		}

		protected override void OnUpdate()
		{
		}

		protected override void OnTerminate()
		{
		}

		protected override void OnCreateMainMenu()
		{
			var mainMenu = new GUI.Menu.MainMenu();
			GUI.Manager.AddControl(mainMenu);
		}

		protected override void OnCreateEffectViwer()
		{
			Debug.Assert(Manager.effectViewer == null);
			Manager.effectViewer = new Dock.EffectViwerPaneBase(deviceType);
			Manager.dockManager.Controls.Add(Manager.effectViewer);
		}

		protected override void OnResetWindowActually()
		{
			if (Manager.effectViewer == null)
			{
				OnCreateEffectViwer();
			}

			var viewerController = Manager.SelectOrShowWindow(typeof(Dock.ViewerController), null);
			var nodeTreeView = Manager.SelectOrShowWindow(typeof(Dock.NodeTreeView), null);
			var commonValues = Manager.SelectOrShowWindow(typeof(Dock.CommonValues), null);
			var locationValues = Manager.SelectOrShowWindow(typeof(Dock.LocationValues), null);
			var rotationValues = Manager.SelectOrShowWindow(typeof(Dock.RotationValues), null);
			var scaleValues = Manager.SelectOrShowWindow(typeof(Dock.ScaleValues), null);
			var rendererCommonValues = Manager.SelectOrShowWindow(typeof(Dock.RendererCommonValues), null);
			var rendererValues = Manager.SelectOrShowWindow(typeof(Dock.RendererValues), null);

			uint windowId = Manager.NativeManager.BeginDockLayout();

			uint dockLeftID = 0, dockRightID = 0;
			Manager.NativeManager.DockSplitNode(windowId, DockSplitDir.Left, 0.65f, ref dockLeftID, ref dockRightID);

			uint dockLeftTop = 0, dockLeftBottom = 0;
			Manager.NativeManager.DockSplitNode(dockLeftID, DockSplitDir.Top, 0.85f, ref dockLeftTop, ref dockLeftBottom);

			uint dockRightTop = 0, dockRightBottom = 0;
			Manager.NativeManager.DockSplitNode(dockRightID, DockSplitDir.Top, 0.6f, ref dockRightTop, ref dockRightBottom);

			Manager.NativeManager.DockSetNodeFlags(dockLeftTop, DockNodeFlags.HiddenTabBar);
			Manager.NativeManager.DockSetNodeFlags(dockLeftBottom, DockNodeFlags.HiddenTabBar);

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