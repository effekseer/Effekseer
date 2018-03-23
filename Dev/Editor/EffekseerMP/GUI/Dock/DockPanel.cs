using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
    class DockPanel : GroupControl, IRemovableControl, IDroppableControl
    {
		public string Label { get; set; } = string.Empty;

		string id = "";

		bool[] opened = new[] { true };

		public DockPanel()
		{
			id = "###" + Manager.GetUniqueID().ToString();
		}

		public override void Update()
		{
			if(opened[0])
			{
				if (Manager.NativeManager.Begin(Label + id, opened))
				{
					UpdateInternal();

					Controls.Lock();

					foreach (var c in Controls.Internal)
					{
						c.Update();
					}

					Controls.Unlock();
				}

				Manager.NativeManager.End();
			}
			else
			{
				ShouldBeRemoved = true;
			}
		}

        protected virtual void UpdateInternal()
        {
        }
    }
}
