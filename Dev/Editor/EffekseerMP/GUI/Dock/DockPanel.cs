using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Effekseer.GUI.Dock
{
    class DockPanel : IRemovableControl
    {
		public string Label { get; set; } = string.Empty;

		public bool ShouldBeRemoved { get; private set; } = false;

		string id = "";

		bool[] opened = new[] { true };

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
				}

				Manager.NativeManager.End();
			}
			else
			{
				ShouldBeRemoved = true;
			}
		}

		public virtual void OnDisposed()
		{
		}

        protected virtual void UpdateInternal()
        {
        }
    }
}
