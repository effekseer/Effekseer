using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
    class DockPanel : IRemovableControl, IDroppableControl
    {
		public string Label { get; set; } = string.Empty;

		public bool ShouldBeRemoved { get; private set; } = false;

		string id = "";

		bool[] opened = new[] { true };

		internal Utils.DelayedList<IControl> Controls = new Utils.DelayedList<IControl>();

		public DockPanel()
		{
			id = "###" + Manager.GetUniqueID().ToString();
		}

		public void Update()
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

		public void OnDropped(string path, ref bool handle)
		{
			Controls.Lock();

			foreach (var c in Controls.Internal)
			{
				var dc = c as IDroppableControl;
				if (dc != null)
				{
					dc.OnDropped(path, ref handle);
					if (handle) break;
				}
			}

			Controls.Unlock();
		}

		public virtual void OnDisposed()
		{
		}

        protected virtual void UpdateInternal()
        {
        }
    }
}
