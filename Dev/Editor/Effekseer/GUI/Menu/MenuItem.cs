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
			GUIManager.NativeManager.Separator();
		}
	}

	class MenuItem : IControl
    {
        string id = "";

        public object Label { get; set; }

		public string Shortcut { get; set; } = null;

        public bool ShouldBeRemoved { get; private set; } = false;

        public Action Clicked;

		public string Icon = Icons.Empty;

        public MenuItem()
        {
            var rand = new Random();
			id = "###" + GUIManager.GetUniqueID().ToString();
		}

        public void Update()
        {
			if (GUIManager.NativeManager.MenuItem(Icon + " " +  Label + id, Shortcut, false, true))
            {
                if(Clicked != null)
                {
                    Clicked();
                }
            }
        }
    }
}
