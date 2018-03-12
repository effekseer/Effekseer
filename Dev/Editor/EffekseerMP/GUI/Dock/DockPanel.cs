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
			if (Manager.NativeManager.Begin(Label + id, opened))
			{
				UpdateInternal();
			}

			Manager.NativeManager.End();
		}

        protected virtual void UpdateInternal()
        {

        }
    }
}
