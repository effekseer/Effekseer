using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Effekseer.swig;

namespace Effekseer.GUI.Menu
{
	public class MenuSeparator : IControl
	{
		public void Update()
		{
			Manager.NativeManager.Separator();
		}
	}

	public class MenuItem : IControl
    {
        string id = "";

        public object Label { get; set; }

		public string Shortcut { get; set; } = null;

        public bool ShouldBeRemoved { get; private set; } = false;

        public Action Clicked;

		public Func<string> GetLabel;

		public string Icon = Icons.Empty;

        public MenuItem()
        {
            var rand = new Random();
			id = "###" + Manager.GetUniqueID().ToString();
		}

        public void Update()
        {
			var label = Label;
			if (GetLabel != null)
			{
				label = GetLabel();
			}

			if (Manager.NativeManager.MenuItem(Icon + " " +  label + id, Shortcut, false, true))
            {
                if(Clicked != null)
                {
                    Clicked();
                }
            }
        }
    }
}
