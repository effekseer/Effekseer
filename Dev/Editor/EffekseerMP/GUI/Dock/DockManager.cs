using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	class DockManager : GroupControl, IRemovableControl, IDroppableControl
	{
		public string Label { get; set; } = string.Empty;

		string id = "";

		bool opened = true ;

		public DockManager()
		{
			id = "###" + Manager.GetUniqueID().ToString();
		}

		public override void Update()
		{
			if (opened)
			{
				//if (Manager.NativeManager.Begin(Label + id, ref opened))
				if(Manager.NativeManager.BeginFullscreen(Label + id))
				{
					Manager.NativeManager.BeginDockspace();

					Controls.Lock();

					foreach (var c in Controls.Internal.OfType<DockPanel>())
					{
						c.Update();
					}

					foreach(var c in Controls.Internal.OfType<DockPanel>())
					{
						if(c.ShouldBeRemoved)
						{
							Controls.Remove(c);
						}
					}

					Controls.Unlock();

					

					Manager.NativeManager.EndDockspace();
				}

				Manager.NativeManager.End();
			}
			else
			{
				ShouldBeRemoved = true;
			}
		}
	}
}
