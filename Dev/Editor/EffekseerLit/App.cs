using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Effekseer.GUI;

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
				typeof(Dock.FileViewer),
				typeof(Dock.PrefabListDock),
				typeof(Dock.TimelineDock),
				typeof(Dock.NodePropertiesDock),
				typeof(Dock.CellPropertiesDock),
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
			Manager.effectViewer = new Dock.EffectViwerDock(deviceType);
			Manager.dockManager.Controls.Add(Manager.effectViewer);
		}

		protected override void OnResetWindowActually()
		{
			if (Manager.effectViewer == null)
			{
				OnCreateEffectViwer();
			}

			var fileViewer = Manager.SelectOrShowWindow(typeof(Dock.FileViewer), null);
			var prefabListDock = Manager.SelectOrShowWindow(typeof(Dock.PrefabListDock), null);
			var timelineDock = Manager.SelectOrShowWindow(typeof(Dock.TimelineDock), null);
			var nodePropertiesDock = Manager.SelectOrShowWindow(typeof(Dock.NodePropertiesDock), null);
			var cellPropertiesDock = Manager.SelectOrShowWindow(typeof(Dock.CellPropertiesDock), null);


			uint windowId = Manager.NativeManager.BeginDockLayout();

			uint dockLeftID = 0, dockRightID = 0;
			uint dockLeftLeftID = 0, dockLeftRightID = 0;
			Manager.NativeManager.DockSplitNode(windowId, swig.DockSplitDir.Left, 0.35f, ref dockLeftID, ref dockRightID);
			Manager.NativeManager.DockSplitNode(dockLeftID, swig.DockSplitDir.Left, 0.5f, ref dockLeftLeftID, ref dockLeftRightID);

			uint dockRightTop = 0, dockRightBottom = 0;
			Manager.NativeManager.DockSplitNode(dockRightID, swig.DockSplitDir.Top, 0.75f, ref dockRightTop, ref dockRightBottom);

			uint dockRightTopLeft = 0, dockRightTopRight = 0;
			Manager.NativeManager.DockSplitNode(dockRightTop, swig.DockSplitDir.Left, 0.75f, ref dockRightTopLeft, ref dockRightTopRight);

			uint dockRightTopRightTop = 0, dockRightTopRightBottom = 0;
			Manager.NativeManager.DockSplitNode(dockRightTopRight, swig.DockSplitDir.Top, 0.5f, ref dockRightTopRightTop, ref dockRightTopRightBottom);

			//uint dockRightTop = 0, dockRightBottom = 0;
			//Manager.NativeManager.DockSplitNode(dockRightID, swig.DockSplitDir.Top, 0.6f, ref dockRightTop, ref dockRightBottom);

			//swig.DockNodeFlags flags = (swig.DockNodeFlags)0xFF;
			swig.DockNodeFlags flags = swig.DockNodeFlags.NoTabBar | swig.DockNodeFlags.HiddenTabBar | swig.DockNodeFlags.NoWindowMenuButton | swig.DockNodeFlags.NoCloseButton | swig.DockNodeFlags.NoDocking;
			Manager.NativeManager.DockSetNodeFlags(dockRightTopLeft, flags);
			Manager.NativeManager.DockSetNodeFlags(dockLeftLeftID, flags);
			Manager.NativeManager.DockSetNodeFlags(dockLeftRightID, flags);
			Manager.NativeManager.DockSetNodeFlags(dockRightBottom, flags);
			Manager.NativeManager.DockSetNodeFlags(dockRightTopRightTop, flags);
			Manager.NativeManager.DockSetNodeFlags(dockRightTopRightBottom, flags);


			Manager.NativeManager.DockSetWindow(dockRightTopLeft, Manager.effectViewer.WindowID);
			Manager.NativeManager.DockSetWindow(dockLeftLeftID, fileViewer.WindowID);
			Manager.NativeManager.DockSetWindow(dockLeftRightID, prefabListDock.WindowID);

			Manager.NativeManager.DockSetWindow(dockRightBottom, timelineDock.WindowID);
			Manager.NativeManager.DockSetWindow(dockRightTopRightTop, nodePropertiesDock.WindowID);
			Manager.NativeManager.DockSetWindow(dockRightTopRightBottom, cellPropertiesDock.WindowID);


			Manager.NativeManager.EndDockLayout();
		}
	}
}
