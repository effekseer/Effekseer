using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

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

        public bool ShouldBeRemoved { get; private set; } = false;

        public Action Clicked;

        public MenuItem()
        {
            var rand = new Random();
			id = "###" + Manager.GetUniqueID().ToString();
		}

        public void Update()
        {
            if (Manager.NativeManager.MenuItem(Label + id))
            {
                if(Clicked != null)
                {
                    Clicked();
                }
            }
        }
    }
}
