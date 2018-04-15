using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Effekseer.swig;

namespace Effekseer.GUI.Menu
{
	class MenuSeparator : IControl
	{
		public void Update()
		{
			Manager.NativeManager.Separator();
		}
	}

	class MenuItem : IControl
    {
        string id = "";

        public string Label { get; set; } = string.Empty;

		public string Shortcut { get; set; } = null;

        public bool ShouldBeRemoved { get; private set; } = false;

        public Action Clicked;

		public ImageResource Icon;

        public MenuItem()
        {
            var rand = new Random();
			id = "###" + Manager.GetUniqueID().ToString();
		}

        public void Update()
        {
			if (Manager.NativeManager.MenuItem(Label + id, Shortcut, false, true, Icon))
            {
                if(Clicked != null)
                {
                    Clicked();
                }
            }
        }
    }
}
