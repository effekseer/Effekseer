using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
	public class DockManager : GroupControl, IRemovableControl, IDroppableControl
	{
		public string Label { get; set; } = string.Empty;

		string id = "";

		bool opened = true ;

		public DockManager()
		{
			id = "###" + GUIManager.GetUniqueID().ToString();
		}

		public override void Update()
		{
			if (opened)
			{
				//if (GUIManager.NativeManager.Begin(Label + id, ref opened))
				if(GUIManager.NativeManager.BeginFullscreen(Label + id))
				{
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
				}

				GUIManager.NativeManager.End();
			}
			else
			{
				ShouldBeRemoved = true;
			}
		}
	}
}
