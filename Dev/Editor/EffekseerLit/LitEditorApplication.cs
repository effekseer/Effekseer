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
			var dockTypes = new Type[]
			{
				typeof(Dock.FileViewer),
			};

			Manager.dockTypes = dockTypes;
			if (!Manager.Initialize(960, 540, () => new GUI.Menu.MainMenu(), System.IO.Path.Combine(EntryDirectory, "resources/icon2.png")))
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

			var fileViewer = Manager.SelectOrShowWindow(typeof(GUI.Dock.FileViewer), null);

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
			Manager.NativeManager.DockSetWindow(dockLeftBottom, fileViewer.WindowID);

			Manager.NativeManager.EndDockLayout();
		}
	}
}
